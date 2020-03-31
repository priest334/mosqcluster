#include "mosq_client.h"

#include <vector>
#ifdef _WIN32
#include <Windows.h>
#define sleep(n) Sleep((n)*1000)
#define atomic_add(s,n) InterlockedExchangeAdd(&(s), (n));
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define atomic_add(s,n) __sync_add_and_fetch(&(s), (n));
#endif
#include <mosquitto.h>
#include "helper/lock.h"
#include "helper/lock_impl_posix.h"
#include "helper/xstring.h"

using std::vector;
typedef hlp::xstring mstr;



namespace {
	class mosqdelay {
	public:
		mosqdelay(int delay_base, int delay_max, bool exponential_backoff)
			: count_(0),
			delay_base_(delay_base), 
			delay_max_(delay_max), 
			exponential_backoff_(exponential_backoff),
			delay_(delay_base) {
		}

		int get() {
			if (delay_ < delay_max_) {
				if (exponential_backoff_) {
					delay_ = delay_ * (count_ + 1) * (count_ + 1);
				} else {
					delay_ = delay_ * (count_ + 1);
				}
				if (delay_ > delay_max_)
					delay_ = delay_max_;
				count_++;
			} else {
				delay_ = delay_max_;
			}
			return delay_;
		}

	private:
		int count_;
		int delay_base_;
		int delay_max_;
		bool exponential_backoff_;
		int delay_;
	};

	void* SubscribeThreadProc(void* argp) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->SubscribeLoop();
		return NULL;
	}

	void* PublishThreadProc(void* argp) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->PublishLoop();
		return NULL;
	}

	void mosq_on_connect(struct mosquitto* mosq, void* argp, int rc) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_connect(rc);
	}

	void mosq_on_disconnect(struct mosquitto* mosq, void* argp, int rc) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_disconnect(rc);
	}

	void mosq_on_publish(struct mosquitto* mosq, void* argp, int mid) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_publish(mid);
	}

	void mosq_on_message(struct mosquitto* mosq, void* argp, const struct mosquitto_message* message) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_message(message->mid, message->topic, message->payload, message->payloadlen, message->qos, message->retain);
	}

	void mosq_on_subscribe(struct mosquitto* mosq, void* argp, int mid, int qos_count, const int* granted_qos) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_subscribe(mid, qos_count, granted_qos);
	}

	void mosq_on_unsubscribe(struct mosquitto* mosq, void* argp, int mid) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_unsubscribe(mid);
	}

	void mosq_on_log(struct mosquitto* mosq, void* argp, int level, const char* message) {
		MosqClient* p = reinterpret_cast<MosqClient*>(argp);
		p->on_log(level, message);
	}
}

class MosqCommand {
public:
	MosqCommand(struct mosquitto* mosq, int command_id = 0) : mosq_(mosq), command_id_(command_id) {
	}
	virtual ~MosqCommand() {
	}
	virtual int SetCommandId(int command_id) {
		int old_command_id = command_id_;
		command_id_ = command_id;
		return old_command_id;
	}
	virtual int Run() = 0;
protected:
	struct mosquitto* mosq_;
	int command_id_;
};

class MosqCommandPublish : public MosqCommand {
public:
	MosqCommandPublish(int command_id, struct mosquitto* mosq, const char* topic, const void* payload, int payloadlen, int qos, bool retain) 
		: MosqCommand(mosq, command_id), topic_(topic), payload_((const char*)payload, payloadlen), qos_(qos), retain_(retain) {
	}
	int Run() {
		return mosquitto_publish(mosq_, &command_id_, topic_.c_str(), payload_.length(), payload_.c_str(), qos_, retain_);
	}
private:
	mstr topic_;
	mstr payload_;
	int qos_;
	bool retain_;
};

class MosqCommandSubscribe : public MosqCommand {
public:
	MosqCommandSubscribe(int command_id, struct mosquitto* mosq, const char* topic, int qos)
		: MosqCommand(mosq, command_id), topic_(topic), qos_(qos) {
	}
	int Run() {
		return mosquitto_subscribe(mosq_, &command_id_, topic_.c_str(), qos_);
	}
private:
	mstr topic_;
	int qos_;
};

class MosqCommandUnsubscribe : public MosqCommand {
public:
	MosqCommandUnsubscribe(int command_id, struct mosquitto* mosq, const char* topic) 
		: MosqCommand(mosq, command_id), topic_(topic){
	}
	int Run() {
		return mosquitto_unsubscribe(mosq_, &command_id_, topic_.c_str());
	}
private:
	mstr topic_;
};

void MosqClient::InitMosqLibrary() {
	mosquitto_lib_init();
}

void CleanMosqLibrary() {
	mosquitto_lib_cleanup();
}

MosqClient::MosqClient() 
	: state_(Stopped), 
	clean_session_(true),
	keep_alive_(10),
	client_id_(),
	host_("127.0.0.1"),
	port_(1883),
	command_id_(0){
}

void MosqClient::set_clean_session(bool cleansessinon) {
	clean_session_ = cleansessinon;
}

void MosqClient::set_keep_alive(int keepalive) {
	keep_alive_ = keepalive;
}

void MosqClient::set_client_id(const string& clientid) {
	client_id_ = clientid;
}

void MosqClient::set_username(const string& username) {
	username_ = username;
}

void MosqClient::set_password(const string& password) {
	password_ = password;
}

void MosqClient::set_host(const string& host) {
	host_ = host;
}

void MosqClient::set_port(int port) {
	port_ = port;
}


bool MosqClient::clean_session() const {
	return clean_session_;
}

int MosqClient::keep_alive() const {
	return keep_alive_;
}

string MosqClient::client_id() const {
	return client_id_;
}

string MosqClient::username() const {
	return username_;
}

string MosqClient::password() const {
	return password_;
}

string MosqClient::host() const {
	return host_;
}

int MosqClient::port() const {
	return port_;
}

long MosqClient::next_command_id() {
	return atomic_add(command_id_, 1);
}

void MosqClient::on_connect(int rc) {
}

void MosqClient::on_disconnect(int rc) {
}

void MosqClient::on_publish(int mid) {
}

void MosqClient::on_message(int mid, const char* topic, const void* payload, int payloadlen, int qos, bool retain) {
}

void MosqClient::on_subscribe(int mid, int qos_count, const int* granted_qos) {
}

void MosqClient::on_unsubscribe(int mid) {
}

void MosqClient::on_log(int level, const char* message) {
}

void MosqClient::SubscribeLoop() {
	int rc = -1;
	bool reconnect = false;
	mosqdelay delay(2, 35, true);
	while (state_ != Stopping) {
		if (!reconnect) {
			rc = mosquitto_connect(mosq_, host().c_str(), port(), keep_alive());
		}
		else {
			rc = mosquitto_reconnect(mosq_);
		}
		if (MOSQ_ERR_SUCCESS == rc) {
			reconnect = true;
			state_ = Running;
			do {
				rc = mosquitto_loop(mosq_, 1, 1);
			} while (state_ != Stopping && rc == MOSQ_ERR_SUCCESS);
			state_ = Reconnecting;
		}
		sleep(delay.get());
	}
}


void MosqClient::PublishLoop() {
	while (state_ != Stopping) {
		if (state_ != Running) {
			sleep(1);
			continue;
		}
		MosqCommand* command = NULL;
		if (commands_.Get(command)) {
			int rc = command->Run();
			if (MOSQ_ERR_SUCCESS != rc) {
				//
			}
		}
	}
}


void MosqClient::Start() {
	mosq_ = mosquitto_new(client_id().c_str(), clean_session(), this);
	mosquitto_username_pw_set(mosq_, username().c_str(), password().c_str());
	mosquitto_connect_callback_set(mosq_, mosq_on_connect);
	mosquitto_disconnect_callback_set(mosq_, mosq_on_disconnect);
	mosquitto_publish_callback_set(mosq_, mosq_on_publish);
	mosquitto_message_callback_set(mosq_, mosq_on_message);
	mosquitto_subscribe_callback_set(mosq_, mosq_on_subscribe);
	mosquitto_unsubscribe_callback_set(mosq_, mosq_on_unsubscribe);
	mosquitto_log_callback_set(mosq_, mosq_on_log);
	pthread_create(&subscribeloop_thread_, NULL, SubscribeThreadProc, this);
	pthread_create(&publishloop_thread_, NULL, PublishThreadProc, this);
}

void MosqClient::Stop() {
	state_ = Stopping;
	pthread_join(publishloop_thread_, NULL);
	pthread_join(subscribeloop_thread_, NULL);
	state_ = Stopped;
	mosquitto_destroy(mosq_);
}

int MosqClient::Publish(const char* topic, const void* payload, int payloadlen, int qos, bool retain) {
	int command_id = (int)next_command_id();
	commands_.Push(new MosqCommandPublish(command_id, mosq_, topic, payload, payloadlen, qos, retain));
	return command_id;
}

int MosqClient::Subscribe(const char* topic, int qos) {
	int command_id = (int)next_command_id();
	commands_.Push(new MosqCommandSubscribe(command_id, mosq_, topic, qos));
	return command_id;
}

int MosqClient::Unsubscribe(const char* topic) {
	int command_id = (int)next_command_id();
	commands_.Push(new MosqCommandUnsubscribe(command_id, mosq_, topic));
	return command_id;
}



