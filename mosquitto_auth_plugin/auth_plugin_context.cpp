#include "auth_plugin_context.h"
#include "helper/local_stream.h"
#include "helper/http_client.h"
#include "helper/http_wrapper.h"
#include "helper/config.h"
#include "helper/json_wrapper.h"


#define MOSQ_BASIC_INFO_OFFSET (sizeof(int)*2)
#define MosqClientAddress(o) (*((char**)(((char*)o)+MOSQ_BASIC_INFO_OFFSET)))
#define MosqClientId(o) (*((char**)(((char*)o)+MOSQ_BASIC_INFO_OFFSET+sizeof(char**))))
#define MosqClientUsername(o) (*((char**)(((char*)o)+MOSQ_BASIC_INFO_OFFSET+2*sizeof(char**))))
#define MosqClientPassword(o) (*((char**)(((char*)o)+MOSQ_BASIC_INFO_OFFSET+3*sizeof(char**))))

#define MOSQUITTO_PLUGIN_VERSION "mosqauth/1.6.9.1"

namespace {
	const char* config_files[] = {
		"/etc/mosquitto_auth_plugin.conf",
		"/usr/etc/mosquitto_auth_plugin.conf",
		"/usr/local/etc/mosquitto_auth_plugin.conf",
		"mosquitto_auth_plugin.conf"
	};
}

AuthPluginContext::AuthPluginContext() {
}

AuthPluginContext::~AuthPluginContext() {
}

void AuthPluginContext::LoadConfig(const string& file) {
	hlp::Config conf;
	for (size_t i = 0; i < (sizeof(config_files) / sizeof(config_files[0])); i++) {
		conf.LoadFile(config_files[i]);
	}
	conf.LoadFile(file);
	node_name_ = conf.Get("nodename");
	checkuser_url_ = conf.Get("checkuser");
	checkacl_url_ = conf.Get("checkacl");
}

void AuthPluginContext::SetOpt(const string& key, const string& value) {
	if (key == "auth_config_file") {
		config_file_ = value;
	}
}

void AuthPluginContext::Initialize(bool reload/* = false*/) {
	if (!reload) {
		LoadConfig(config_file_);
	}
}

void AuthPluginContext::Cleanup(bool reload/* = false*/) {
}

AuthPluginContext::AuthPluginError AuthPluginContext::CheckUser(struct mosquitto* client, const string& username, const string& password) {
	if (checkuser_url_.empty())
		return Defer;
	string clientid = MosqClientId(client);
	if (0 == clientid.find("mosqcluster"))
		return NoError;
	hlp::JsonDocument data;
	data.Set("name", node_name_);
	data.Set("clientid", clientid);
	data.Set("username", username);
	data.Set("password", password);

	HttpWrapper http;
	http.SetHeader("Connection", "Close");
	http.SetHeader("Content-Type", "application/json");
	http.SetHeader("User-Agent", MOSQUITTO_PLUGIN_VERSION);
	if (http.Post(checkuser_url_, data.Write(false))) {
		int code = -1;
		string text = http.Text();
		hlp::JsonDocument resp;
		resp.Parse(text);
		if (resp.Get("code", code) && code == 0) {
			return NoError;
		}
	}
	return Failed;
}

AuthPluginContext::AuthPluginError AuthPluginContext::CheckAcl(struct mosquitto* client, int access, const string& topic, const void* payload, long payloadlen, int qos, bool retain) {
	if (checkacl_url_.empty())
		return Defer;
	string clientid = MosqClientId(client);
	if (0 == clientid.find("mosqcluster"))
		return NoError;
	hlp::JsonDocument data;
	data.Set("name", node_name_);
	data.Set("clientid", clientid);
	data.Set("access", access);
	data.Set("topic", topic);

	HttpWrapper http;
	http.SetHeader("Connection", "Close");
	http.SetHeader("Content-Type", "application/json");
	http.SetHeader("User-Agent", MOSQUITTO_PLUGIN_VERSION);
	if (http.Post(checkacl_url_, data.Write(false))) {
		int code = -1;
		string text = http.Text();
		hlp::JsonDocument resp;
		resp.Parse(text);
		if (resp.Get("code", code) && code == 0) {
			return NoError;
		}
	}
	return Failed;
}


