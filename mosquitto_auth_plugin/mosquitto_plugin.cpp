#include "mosquitto_plugin.h"
#include "auth_plugin_context.h"


#ifdef __cplusplus
extern "C" {
#endif

	
	/* Error values */
	enum mosq_err_t {
		MOSQ_ERR_AUTH_CONTINUE = -4,
		MOSQ_ERR_NO_SUBSCRIBERS = -3,
		MOSQ_ERR_SUB_EXISTS = -2,
		MOSQ_ERR_CONN_PENDING = -1,
		MOSQ_ERR_SUCCESS = 0,
		MOSQ_ERR_NOMEM = 1,
		MOSQ_ERR_PROTOCOL = 2,
		MOSQ_ERR_INVAL = 3,
		MOSQ_ERR_NO_CONN = 4,
		MOSQ_ERR_CONN_REFUSED = 5,
		MOSQ_ERR_NOT_FOUND = 6,
		MOSQ_ERR_CONN_LOST = 7,
		MOSQ_ERR_TLS = 8,
		MOSQ_ERR_PAYLOAD_SIZE = 9,
		MOSQ_ERR_NOT_SUPPORTED = 10,
		MOSQ_ERR_AUTH = 11,
		MOSQ_ERR_ACL_DENIED = 12,
		MOSQ_ERR_UNKNOWN = 13,
		MOSQ_ERR_ERRNO = 14,
		MOSQ_ERR_EAI = 15,
		MOSQ_ERR_PROXY = 16,
		MOSQ_ERR_PLUGIN_DEFER = 17,
		MOSQ_ERR_MALFORMED_UTF8 = 18,
		MOSQ_ERR_KEEPALIVE = 19,
		MOSQ_ERR_LOOKUP = 20,
		MOSQ_ERR_MALFORMED_PACKET = 21,
		MOSQ_ERR_DUPLICATE_PROPERTY = 22,
		MOSQ_ERR_TLS_HANDSHAKE = 23,
		MOSQ_ERR_QOS_NOT_SUPPORTED = 24,
		MOSQ_ERR_OVERSIZE_PACKET = 25,
		MOSQ_ERR_OCSP = 26,
	};

	int mosquitto_auth_plugin_version(void) {
		return MOSQ_AUTH_PLUGIN_VERSION;
	}


	int mosquitto_auth_plugin_init(void **user_data, struct mosquitto_opt *opts, int opt_count) {
		AuthPluginContext* context = new AuthPluginContext();
		if (context) {
			struct mosquitto_opt* iter;
			for (int i = 0; i < opt_count; i++) {
				iter = opts + i;
				context->SetOpt(iter->key, iter->value);
			}
			context->Initialize();
			*user_data = context;
		}
		return MOSQ_ERR_SUCCESS;
	}

	int mosquitto_auth_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count) {
		AuthPluginContext* context = reinterpret_cast<AuthPluginContext*>(user_data);
		if (context) {
			context->Cleanup();
			delete context;
		}
		return MOSQ_ERR_SUCCESS;
	}

	int mosquitto_auth_security_init(void *user_data, struct mosquitto_opt *opts, int opt_count, bool reload) {
		AuthPluginContext* context = reinterpret_cast<AuthPluginContext*>(user_data);
		if (context && reload) {
			struct mosquitto_opt* iter;
			for (int i = 0; i < opt_count; i++) {
				iter = opts + i;
				context->SetOpt(iter->key, iter->value);
			}
			context->Initialize(reload);
		}
		return MOSQ_ERR_SUCCESS;
	}

	int mosquitto_auth_security_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count, bool reload) {
		AuthPluginContext* context = reinterpret_cast<AuthPluginContext*>(user_data);
		if (context && reload) {
			context->Cleanup(reload);
		}
		return MOSQ_ERR_SUCCESS;
	}

	int mosquitto_auth_acl_check(void *user_data, int access, struct mosquitto *client, const struct mosquitto_acl_msg *msg) {
		AuthPluginContext* context = reinterpret_cast<AuthPluginContext*>(user_data);
		AuthPluginContext::AuthPluginError code = context->CheckAcl(client, access, msg->topic, msg->payload, msg->payloadlen, msg->qos, msg->retain);
		return (code >= 0) ? MOSQ_ERR_SUCCESS : MOSQ_ERR_ACL_DENIED;
	}


	int mosquitto_auth_unpwd_check(void *user_data, struct mosquitto *client, const char *username, const char *password) {
		AuthPluginContext* context = reinterpret_cast<AuthPluginContext*>(user_data);
		AuthPluginContext::AuthPluginError code = context->CheckUser(client, username, password);
		return (code >= 0) ? MOSQ_ERR_SUCCESS : MOSQ_ERR_ACL_DENIED;
	}

	int mosquitto_auth_psk_key_get(void *user_data, struct mosquitto *client, const char *hint, const char *identity, char *key, int max_key_len) {
		return MOSQ_ERR_SUCCESS;
	}

	int mosquitto_auth_start(void *user_data, struct mosquitto *client, const char *method, bool reauth, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len) {
		return MOSQ_ERR_SUCCESS;
	}

	int mosquitto_auth_continue(void *user_data, struct mosquitto *client, const char *method, const void *data_in, uint16_t data_in_len, void **data_out, uint16_t *data_out_len) {
		return MOSQ_ERR_SUCCESS;
	}


#ifdef __cplusplus
}
#endif
