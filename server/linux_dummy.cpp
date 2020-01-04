#include "linux_dummy.h"


#ifdef WIN32

int close(int fd) {
	return 0;
}

int usleep(int usec) {
	return 0;
}

int fcntl(int fd, int cmd, ... /* arg */) {
	return 0;
}

int epoll_create(int size) {
	return 0;
}

int epoll_create1(int flags) {
	return 0;
}

int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout) {
	return 0;
}

int epoll_pwait(int epfd, struct epoll_event* events, int maxevents, int timeout, const sigset_t* sigmask) {
	return 0;
}

int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event) {
	return 0;
}

int gettid() {
	return 0;
}

int getpid() {
	return 0;
}

int fork() {
	return 0;
}

int setsid() {
	return 0;
}

int open(const char* pathname, int flags, mode_t mode) {
	return 0;
}

int dup2(int oldfd, int newfd) {
	return 0;
}

int sleep(unsigned int seconds) {
	Sleep(1000);
}

#endif // WIN32




