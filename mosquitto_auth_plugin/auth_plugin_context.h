#pragma once

#include <string>
using std::string;

struct mosquitto;


class AuthPluginContext {
public:
	typedef enum _AuthPluginError {
		Failed = -1,
		NoError = 0,
		Defer = 1
	} AuthPluginError;

	AuthPluginContext();
	~AuthPluginContext();

	void LoadConfig(const string& file);
	void SetOpt(const string& key, const string& value);

	void Initialize(bool reload = false);
	void Cleanup(bool reload = false);

	AuthPluginError CheckUser(struct mosquitto* client, const string& username, const string& password);
	AuthPluginError CheckAcl(struct mosquitto* client, int access, const string& topic, const void* payload, long payloadlen, int qos, bool retain);

private:
	string config_file_;
	string node_name_;
	string auth_host_;
	string checkuser_url_;
	string checkacl_url_;
};

