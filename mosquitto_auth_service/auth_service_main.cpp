
#include <string>
#include <sstream>

#include "global.h"
#include "helper/command_line.h"
#include "server/http_server.h"
#include "server/linux_dummy.h"
#include "auth_service_routes.h"

#ifdef _WIN32
#include <Windows.h>
#define CurrentProcessId GetCurrentProcessId
#else
#include <unistd.h>
#define CurrentProcessId getpid
#endif


using std::string;
using std::ostringstream;

class GlobalInitializer {
	GlobalInitializer(const GlobalInitializer&);
	GlobalInitializer& operator=(const GlobalInitializer&);
public:
	GlobalInitializer(hlp::CommandLine* cmdline) {
		config::Initialize(cmdline);
		logger::Initialize();
		async::Initialize();
		service::Initialize();
	}

	~GlobalInitializer() {
		async::Cleanup();
		logger::Cleanup();
		config::Cleanup();
	}
};

int main(int argc, char* argv[]) {
	hlp::CommandLine cmdline(argc, argv);

	if (cmdline.HasSwitch("daemon")) {
		pid_t pid = fork();
		if (pid != 0) {
			return 0;
		}
	}

	GlobalInitializer initializer(&cmdline);
	int port = config::Get()->GetInt("port", 2020);
	string ip = config::Get()->Get("listen", "127.0.0.1");
	HttpServer server(new AuthServiceRoutes());
	server.Start(port, ip.c_str());
	return 0;
}



