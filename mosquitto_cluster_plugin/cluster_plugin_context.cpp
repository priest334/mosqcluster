#include "cluster_plugin_context.h"
#include "helper/http_client.h"
#include "helper/http_wrapper.h"
#include "helper/config.h"
#include "helper/json_wrapper.h"
#include "helper/string_helper.h"
#include "helper/crypto/hash.h"
#include "mosq_client.h"
#include "helper/lock_impl_posix.h"


#define MOSQ_BASIC_INFO_OFFSET (sizeof(int)*2)
#define MosqClientAddress(o) (char*)(&(((char*)o)+MOSQ_BASIC_INFO_OFFSET))
#define MosqClientId(o) (char*)(&(((char*)o)+MOSQ_BASIC_INFO_OFFSET+sizeof(char*)))
#define MosqClientUsername(o) (char*)(&(((char*)o)+MOSQ_BASIC_INFO_OFFSET+2*sizeof(char*)))
#define MosqClientPassword(o) (char*)(&(((char*)o)+MOSQ_BASIC_INFO_OFFSET+3*sizeof(char*)))

#define MOSQUITTO_PLUGIN_VERSION "mosqcluster/1.6.9.1"


namespace {
	const char* config_files[] = {
		"/etc/mosquitto_cluster_plugin.conf",
		"/usr/etc/mosquitto_cluster_plugin.conf",
		"/usr/local/etc/mosquitto_cluster_plugin.conf",
		"mosquitto_cluster_plugin.conf"
	};

	class MosqClusterMonitor : public MosqClient {
	public:
		MosqClusterMonitor(ClusterPluginContext* plugin_context) 
			: plugin_context_(plugin_context), monitor_topic_("/mosq/cluster/join"){
		}

		void on_connect(int rc) {
			if (0 == rc) {
				Subscribe(monitor_topic_.c_str(), 1);
				string node = plugin_context_->NodeName();
				Publish(monitor_topic_.c_str(), node.c_str(), node.length(), 1, 0);
			}
		}

		void on_message(int mid, const char* topic, const void* payload, int payloadlen, int qos, bool retain) {
			if (monitor_topic_ == topic) {
				string name((const char*)payload, payloadlen);
				if (name != plugin_context_->NodeName()) {
					plugin_context_->MaintainProxyInstances(name);
				}
			}
		}

	private:
		ClusterPluginContext* plugin_context_;
		string monitor_topic_;
	};
}

ClusterPluginContext::ClusterPluginContext() : lock_(new LockImplPosix()), cluster_nodes_(NULL), nodes_last_update_(){
}

ClusterPluginContext::~ClusterPluginContext() {
}

string const& ClusterPluginContext::NodeName() const {
	return node_name_;
}

void ClusterPluginContext::LoadConfig(const string& file) {
	hlp::Config conf;
	for (size_t i = 0; i < (sizeof(config_files) / sizeof(config_files[0])); i++) {
		conf.LoadFile(config_files[i]);
	}
	conf.LoadFile(file);
	node_name_ = conf.Get("nodename");
	api_join_ = conf.Get("api-join");
}

void ClusterPluginContext::SetOpt(const string& key, const string& value) {
	if (key == "cluster_config_file") {
		config_file_ = value;
	}
}

void ClusterPluginContext::Initialize(bool reload/* = false*/) {
	if (!reload) {
		MosqClient::InitMosqLibrary();
		LoadConfig(config_file_);

		hlp::JsonDocument doc;
		doc.Set("action", "join");
		doc.Set("node", node_name_);

		HttpWrapper http;
		http.SetHeader("Connection", "Close");
		http.SetHeader("Content-Type", "application/json");
		http.SetHeader("User-Agent", MOSQUITTO_PLUGIN_VERSION);
		if (http.Post(api_join_, doc.Write(false))) {
			string text = http.Text();
			hlp::JsonDocument resp;
			resp.Parse(text);
			int code = -1;
			if (resp.Get("code", code) && code == 0) {
				vector<string> nodes;
				resp.Get("nodes", nodes);
				MaintainProxyInstances(nodes);
			}
		}
	}
}

void ClusterPluginContext::Cleanup(bool reload/* = false*/) {
	if (!reload) {
		MosqClient::CleanMosqLibrary();
	}
}

void ClusterPluginContext::MosqProxyPass(struct mosquitto* client, int access, const string& topic, const void* payload, long payloadlen, int qos, bool retain) {
	if (api_join_.empty())
		return;
	string clientid = MosqClientId(client);
	string address = MosqClientAddress(client);
	string username = MosqClientUsername(client);
	string password = MosqClientPassword(client);
	if (0 == clientid.find("mosqcluster"))
		return;
	AutoLock lock(lock_);
	MosqClusterNode* nodeiter = cluster_nodes_;
	while (nodeiter) {
		nodeiter->client_->Publish(topic.c_str(), payload, payloadlen, qos, retain);
		nodeiter = nodeiter->next_;
	}
}

void ClusterPluginContext::MaintainProxyInstances(const vector<string>& nodes) {
	vector<string>::const_iterator iter = nodes.begin();
	for (; iter != nodes.end(); ++iter) {
		MaintainProxyInstances(*iter);
	}
}

void ClusterPluginContext::MaintainProxyInstances(const string& name) {
	AutoLock lock(lock_);
	bool exists = false;
	MosqClusterNode* nodeiter = cluster_nodes_;
	while (nodeiter) {
		if (nodeiter->name_ == name) {
			exists = true;
			break;
		}
		nodeiter = nodeiter->next_;
	}
	if (!exists) {
		MosqClusterNode* node = new MosqClusterNode();
		node->name_ = name;
		node->client_ = new MosqClusterMonitor(this);
		node->next_ = NULL;
		hlp::StringVector v;
		v.Split(node->name_.c_str(), ":");
		if (v.Size() == 2) {
			node->client_->set_host(v[0]);
			node->client_->set_port(atoi(v[1].c_str()));
		} else {
			node->client_->set_host(node->name_.c_str());
		}
		hlp::String str;
		str.Format("mosqcluster/%s", hlp::crypt::md5(node->name_.c_str()).c_str());
		node->client_->set_client_id(str.str());
		node->client_->set_username("mosqcluster");
		node->client_->set_password("mosqcluster");
		node->client_->set_clean_session(false);
		node->client_->set_keep_alive(10);
		node->client_->Start();

		if (cluster_nodes_ == NULL) {
			cluster_nodes_ = node;
		} else {
			nodeiter = cluster_nodes_;
			while (nodeiter->next_) {
				nodeiter = nodeiter->next_;
			}
			nodeiter->next_ = node;
		}
	}
}


