#include "mosquitto_context_manager.h"
#include "global.h"
#include "helper/lock_impl_posix.h"



MosquittoContext::TopicContext::TopicContext() {
}

MosquittoContext::ClientContext::ClientContext() {
}

MosquittoContext::MosquittoContext(const string& host) : host_(host) {
}

bool MosquittoContext::StatUsers(const string& clientid, const string& username, const string& password) {
	return true;
}

bool MosquittoContext::StatTopics(const string& clientid, int access, const string& topic) {
	return true;
}


MosquittoContextManager::MosquittoContextManager(const string& trust_username, const string& trust_password, const string& trust_prefix)
	: trust_username_(trust_username), trust_password_(trust_password), trust_prefix_(trust_prefix) {
}

MosquittoContext* MosquittoContextManager::Get(const string& name) const {
	AutoLock lock(lock_);
	map<mstr, MosquittoContext*>::const_iterator iter = mosquitto_instances_.find(name);
	if (iter != mosquitto_instances_.end()) {
		return iter->second;
	}
	return NULL;
}

int MosquittoContextManager::NodeNames(vector<string>& nodes) {
	AutoLock lock(lock_);
	map<mstr, MosquittoContext*>::const_iterator iter = mosquitto_instances_.begin();
	for (; iter != mosquitto_instances_.end(); ++iter) {
		nodes.push_back(iter->first.c_str());
	}
	return nodes.size();
}

bool MosquittoContextManager::Initialize(const string& name) {	
	AutoLock lock(lock_);
	map<mstr, MosquittoContext*>::const_iterator iter = mosquitto_instances_.find(name);
	if (iter == mosquitto_instances_.end()) {
		MosquittoContext* context = new MosquittoContext(name);
		mosquitto_instances_[name] = context;
	}
	return true;
}

bool MosquittoContextManager::CheckUser(const string& name, const string& clientid, const string& username, const string& password) {
	MosquittoContext* mosq = Get(name);
	if (mosq) {
		mosq->StatUsers(clientid, username, password);
	}
	if (trust_username_ == username && trust_password_ == password) {
		return true;
	} else if (0 == username.find(trust_prefix_)) {
		return true;
	}
	return false;
}

bool MosquittoContextManager::CheckAcl(const string& name, const string& clientid, int access, const string& topic) {
	MosquittoContext* mosq = Get(name);
	if (mosq) {
		mosq->StatTopics(clientid, access, topic);
	}
	return true;
}





