#pragma once

#include <string>
#include <map>
#include <vector>
#include "helper/xstring.h"
#include "helper/lock.h"

using std::string;
using std::map;
using std::vector;
typedef hlp::xstring mstr;


class MosquittoContext {
	class TopicContext {
	public:
		TopicContext();
	private:
		mstr topic_;
	};
	class ClientContext {
	public:
		ClientContext();
	private:
		vector<TopicContext*> topics_;
	};
public:
	MosquittoContext(const string& host);
	bool StatUsers(const string& clientid, const string& username, const string& password);
	bool StatTopics(const string& clientid, int access, const string& topic);
private:
	mstr host_;
	ILock* lock_;
	map<mstr, ClientContext*> clients_;
};

class MosquittoContextManager {
public:
	MosquittoContextManager(const string& trust_username, const string& trust_password, const string& trust_prefix);

	MosquittoContext* Get(const string& name) const;
	int NodeNames(vector<string>& nodes);

	bool Initialize(const string& name);
	bool CheckUser(const string& name, const string& clientid, const string& username, const string& password);
	bool CheckAcl(const string& name, const string& clientid, int access, const string& topic);

private:
	string trust_username_;
	string trust_password_;
	string trust_prefix_;
	ILock* lock_;
	map<mstr, MosquittoContext*> mosquitto_instances_;
};

