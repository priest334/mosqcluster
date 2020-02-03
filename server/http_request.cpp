#include "http_request.h"

#include <time.h>
#include "helper/url_encode.h"


/* Status Codes */
#define HTTP_RESP_STATUS_TABLE(XX)                                          \
  XX(200, OK,                              OK)                              \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_REQUEST,                     Bad Request)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required)

typedef struct {
	int code;
	const char* desc;
}HttpRespStatus;

HttpRespStatus http_resp_status_table[] = {
#define XX(num, name, str) {num, #str},
	HTTP_RESP_STATUS_TABLE(XX)
#undef XX
	{-1, "Not Used"}
};

const char* http_status_desc(int code) {
	int count = sizeof(http_resp_status_table) / sizeof(http_resp_status_table[0]) - 1;
	for (int i = 0; i < count; i++) {
		if (code == http_resp_status_table[i].code) {
			return http_resp_status_table[i].desc;
		}
	}
	return http_resp_status_table[count - 1].desc;
}


static int OnUrl(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnUrl(at, length);
}
static int OnHeadersComplete(http_parser* parser) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnHeadersComplete();
}
static int OnBody(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnBody(at, length);
}
static int OnHeaderField(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnHeaderField(at, length);
}
static int OnHeaderValue(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnHeaderValue(at, length);
}
static int OnMessageBegin(http_parser* parser) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnMessageBegin();
}
static int OnMessageComplete(http_parser* parser) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnMessageComplete();
}
static int OnQueryField(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnQueryField(at, length);
}
static int OnQueryValue(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnQueryValue(at, length);
}
static int OnFragment(http_parser* parser, const char* at, size_t length) {
	HttpRequestBase* hp = (HttpRequestBase*)parser->data;
	return hp->OnFragment(at, length);
}

http_parser_settings* HttpRequestBase::settings_ = NULL;

void HttpRequestBase::Init() {
	settings_ = new http_parser_settings;
	http_parser_settings& settings = *settings_;
	http_parser_settings_init(settings_);
	settings.on_url = ::OnUrl;
	settings.on_headers_complete = ::OnHeadersComplete;
	settings.on_body = ::OnBody;
	settings.on_header_field = ::OnHeaderField;
	settings.on_header_value = ::OnHeaderValue;
	settings.on_message_begin = ::OnMessageBegin;
	settings.on_message_complete = ::OnMessageComplete;
	settings.on_query_field = ::OnQueryField;
	settings.on_query_value = ::OnQueryValue;
	settings.on_fragment = ::OnFragment;
}
void HttpRequestBase::Cleanup() {
	delete settings_;
}


HttpRequestBase::HttpRequestBase() : finished_(false){
	http_parser_init(&parser_, HTTP_REQUEST);
	parser_.data = this;
}

HttpRequestBase::~HttpRequestBase() {
}

int HttpRequestBase::OnUrl(const char* at, size_t length) {
	url_.assign(at, length);
	return 0;
}

int HttpRequestBase::OnHeadersComplete() {
	method_ = parser_.method;//http_method_str((http_method)parser_.method);
	return 0;
}

int HttpRequestBase::OnBody(const char* at, size_t length) {
	body_.assign(at, length);
	return 0;
}

int HttpRequestBase::OnHeaderField(const char* at, size_t length) {
	tempkey_.assign(at, length);
	return 0;
}

int HttpRequestBase::OnHeaderValue(const char* at, size_t length) {
	tempval_.assign(at, length);
	headers_[tempkey_] = tempval_;
	return 0;
}

int HttpRequestBase::OnMessageBegin() {
	return 0;
}

int HttpRequestBase::OnMessageComplete() {
	finished_ = true;
	return 0;
}

int HttpRequestBase::OnQueryField(const char* at, size_t length) {
	tempkey_.assign(at, length);
	return 0;
}

int HttpRequestBase::OnQueryValue(const char* at, size_t length) {
	tempval_.assign(at, length);
	queries_[tempkey_] = tempval_;
	return 0;
}

int HttpRequestBase::OnFragment(const char* at, size_t length) {
	fragment_.assign(at, length);
	return 0;
}


HttpRequest::HttpRequest(int socket) : socket_(socket) {
	timestamp_ = time(NULL);
}

HttpRequest::~HttpRequest() {
}

bool HttpRequest::Finished() const {
	return finished_;
}

bool HttpRequest::IsTimeOut(time_t now, time_t timeout) const {
	return (now - timestamp_ ) > timeout;
}

size_t HttpRequest::Append(const char* buffer, size_t size) {
	return http_parser_execute(&parser_, settings_, buffer, size);
}

int HttpRequest::socket() const {
	return socket_;
}

mstr HttpRequest::url() const {
	return url_;
}

int HttpRequest::method() const {
	return method_;
}

mstr HttpRequest::fragment() const {
	return fragment_;
}

mstr HttpRequest::body() const {
	return body_;
}

mstr HttpRequest::header(const mstr& name) const{
	map<mstr, mstr>::const_iterator iter = headers_.find(name);
	if (iter != headers_.end()) {
		return iter->second;
	}
	return mstr("");
}

mstr HttpRequest::query(const mstr& key) const {
	map<mstr, mstr>::const_iterator iter = queries_.find(key);
	if (iter != headers_.end()) {
		return UrlDecode(iter->second.c_str());
	}
	return mstr("");
}

bool HttpRequest::HasHeader(const mstr& name) const {
	map<mstr, mstr>::const_iterator iter = headers_.find(name);
	return (iter != headers_.end());
}

bool HttpRequest::HasQuery(const mstr& key) const {
	map<mstr, mstr>::const_iterator iter = queries_.find(key);
	return (iter != headers_.end());
}

void HttpRequest::data(const mstr& str) {
	data_ = str;
}
mstr const& HttpRequest::data() const {
	return data_;
}


ostringstream& operator<<(ostringstream& oss, const HttpResponse& src) {
	oss << "HTTP/1.1 " << src.status_code_ << " " << http_status_desc(src.status_code_) << "\r\n";
	map<mstr, mstr>::const_iterator iter = src.headers_.begin();
	for (; iter != src.headers_.end(); ++iter) {
		oss << iter->first.c_str() << ":" << iter->second.c_str() << "\r\n";
	}
	oss << "Content-Length" << ":" << src.content_.length() << "\r\n";
	oss << "\r\n";
	oss << src.content_.c_str();
	return oss;
}

HttpResponse::HttpResponse() : status_code_(HTTP_STATUS_NOT_FOUND) {
	SetHeader("Connection", "close");
	SetHeader("Content-Type", "text/plain");
	SetHeader("Server", "xauth/1.0");
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::SetStatus(int code) {
	status_code_ = code;
}

void HttpResponse::SetHeader(const mstr& name, const mstr& value) {
	headers_[name] = value;
}

void HttpResponse::SetContent(const mstr& content) {
	content_ = content;
}

mstr HttpResponse::Message() const {
	ostringstream oss;
	oss << *this;
	return oss.str();
}


