#ifndef __HANDLER_DISPATCH_H__
#define __HANDLER_DISPATCH_H__

#define DECLARE_HANDLER_CLUSTER(suffix, name_type, handler_type) \
protected: \
	struct _handler_##suffix {name_type _name; handler_type _handler;}; \
	static _handler_##suffix _cluster_of_##suffix[]; \
	handler_type get_##suffix##_handler(name_type _name) const \
	{ \
		handler_type entry = NULL; \
		for( int i = 0 ; ; i++ ) \
		{ \
			if( _cluster_of_##suffix[i]._handler == NULL) break; \
			if( _cluster_of_##suffix[i]._name == _name) \
			{ \
				entry = _cluster_of_##suffix[i]._handler; \
				break; \
			} \
		} \
		return entry; \
	} \
	handler_type get_##suffix##_handler_with_default(name_type _name, handler_type default_handler) const \
	{ \
		handler_type entry = get_##suffix##_handler(_name); \
		if (entry == NULL) \
			entry = default_handler; \
		return entry; \
	}

#define BEGIN_HANDLER_CLUSTER(suffix, class_name) \
	class_name::_handler_##suffix class_name::_cluster_of_##suffix[] = {

#define ADD_HANDLER_TO_CLUSTER(name, entry) \
	{name, entry},

#define END_HANDLER_CLUSTER(end_name) \
	{end_name, NULL} }; 

#define GET_HANDLER(suffix,handler_name) get_##suffix##_handler(handler_name)
#define GET_HANDLER_WITH_DEFAULT(suffix,handler_name,default_handler) get_##suffix##_handler_with_default(handler_name,default_handler)

#endif

