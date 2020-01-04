
#include <string>
#include <sstream>

#include "helper/command_line.h"
#include "helper/lock.h"
#include "helper/lock_impl_posix.h"
#include "helper/logger.h"
#include "server/http_server.h"
#include "server/linux_dummy.h"
#include "xauth.h"

#ifdef _WIN32
#include <Windows.h>
#define CurrentProcessId GetCurrentProcessId
#else
#include <unistd.h>
#define CurrentProcessId getpid
#endif


using std::string;
using std::ostringstream;

int main(int argc, char* argv[]) {
	hlp::CommandLine cmdline(argc, argv);

	if (cmdline.HasSwitch("daemon")) {
		pid_t pid = fork();
		if (pid != 0) {
			return 0;
		}
	}
	
	ostringstream oss;
	oss << cmdline.GetSwitchValueWithDefault("logdir", "/var/log/xauth") << "/" << CurrentProcessId();
	
	string filepath = oss.str();
	int port = cmdline.GetSwitchIntValue("port", 8881);
	string ip = cmdline.GetSwitchValueWithDefault("listen", "0.0.0.0");

	LogFile* logfile = new LogFile(filepath, new LockImplPosix());
	Logger::Initialize();
	Logger::Get()->SetLevel(logger::Debug);
	Logger::Get()->SetStorage(logfile);
	HttpServer server(new XAuthRoutes());
	server.Start(port, ip.c_str());
	return 0;
}



