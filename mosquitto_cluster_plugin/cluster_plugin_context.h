#pragma once

#include <time.h>
#include <string>
#include <vector>
#include "helper/xstring.h"
#include "helper/lock.h"

using std::string;
using std::vector;

struct mosquitto;
class MosqClient;

typedef hlp::xstring mstr;


typedef struct _MosqClusterNode {
	mstr name_;
	MosqClient* client_;
	MosqClusterNode* next_;
} MosqClusterNode;

class ClusterPluginContext {
public:
	typedef enum _PluginError {
		Failed = -1,
		NoError = 0,
		Defer = 1
	} PluginError;

	ClusterPluginContext();
	~ClusterPluginContext();
	string const& NodeName() const;

	void LoadConfig(const string& file);
	void SetOpt(const string& key, const string& value);

	void Initialize(bool reload = false);
	void Cleanup(bool reload = false);

	void MosqProxyPass(struct mosquitto* client, int access, const string& topic, const void* payload, long payloadlen, int qos, bool retain);
	void MaintainProxyInstances(const vector<string>& nodes);
	void MaintainProxyInstances(const string& name);

private:
	string config_file_;
	string node_name_;
	string api_join_;
	ILock* lock_;
	MosqClusterNode* cluster_nodes_;
	time_t nodes_last_update_;
};

