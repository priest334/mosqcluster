#pragma once
#ifndef __MOSQ_CLIENT_H__
#define __MOSQ_CLIENT_H__

#include <string>
#include <list>
#include <pthread.h>
#include "helper/task_queue_posix.h"
#include "helper/lock.h"

using std::string;
using std::list;

struct mosquitto;
class MosqCommand;


class MosqClient {
	typedef enum _RunningState {
		Stopped,
		Running,
		Stopping,
		Reconnecting
	} RunningState;

public:
	MosqClient();

	void set_clean_session(bool cleansession);
	void set_keep_alive(int keepalive);
	void set_client_id(const string& clientid);
	void set_username(const string& username);
	void set_password(const string& password);
	void set_host(const string& host);
	void set_port(int port);
	bool clean_session() const;
	int keep_alive() const;
	string client_id() const;
	string username() const;
	string password() const;
	string host() const;
	int port() const;
	long next_command_id();

	virtual void on_connect(int rc);
	virtual void on_disconnect(int rc);
	virtual void on_publish(int mid);
	virtual void on_message(int mid, const char* topic, const void* payload, int payloadlen, int qos, bool retain);
	virtual void on_subscribe(int mid, int qos_count, const int* granted_qos);
	virtual void on_unsubscribe(int mid);
	virtual void on_log(int level, const char* message);

	void SubscribeLoop();
	void PublishLoop();

	void Start();
	void Stop();
	int Publish(const char* topic, const void* payload, int payloadlen, int qos, bool retain);
	int Subscribe(const char* topic, int qos);
	int Unsubscribe(const char* topic);
	
private:
	volatile RunningState state_;
	bool clean_session_;
	int keep_alive_;
	string client_id_;
	string username_;
	string password_;
	string host_;
	int port_;
	struct mosquitto* mosq_;
	pthread_t subscribeloop_thread_;
	pthread_t publishloop_thread_;
	TaskQueue<MosqCommand*> commands_;
	long command_id_;
};



#endif // __MOSQ_CLIENT_H__

