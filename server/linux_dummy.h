#ifndef __LINUX_DUMMY_H__
#define __LINUX_DUMMY_H__

#ifdef _WIN32

#include <stdint.h>
#include <winsock.h>

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

typedef int socklen_t;
typedef int pid_t;
// <bits/sigset.h>
/* A `sigset_t' has a bit for each signal.  */

# define _SIGSET_NWORDS	(1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
	unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

typedef __sigset_t sigset_t;

// <unistd.h>
int close(int fd);
int usleep(int usec);
// <fcntl.h>
/* Values for the second argument to `fcntl'.  */
#define F_DUPFD         0       /* Duplicate file descriptor.  */
#define F_GETFD         1       /* Get file descriptor flags.  */
#define F_SETFD         2       /* Set file descriptor flags.  */
#define F_GETFL         3       /* Get file status flags.  */
#define F_SETFL         4       /* Set file status flags.  */
#define O_NONBLOCK      04000
int fcntl(int fd, int cmd, ... /* arg */);

#ifndef __EPOLL_PACKED
# define __EPOLL_PACKED
#endif

// <bits/epoll.h>
enum
{
	EPOLL_CLOEXEC = 02000000,
#define EPOLL_CLOEXEC EPOLL_CLOEXEC
	EPOLL_NONBLOCK = 00004000
#define EPOLL_NONBLOCK EPOLL_NONBLOCK
};

// <sys/epoll.h>
enum EPOLL_EVENTS
{
	EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
	EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
	EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
	EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
	EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
	EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
	EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
	EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
	EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
	EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
	EPOLLRDHUP = 0x2000,
#define EPOLLRDHUP EPOLLRDHUP
	EPOLLWAKEUP = 1u << 29,
#define EPOLLWAKEUP EPOLLWAKEUP
	EPOLLONESHOT = 1u << 30,
#define EPOLLONESHOT EPOLLONESHOT
	EPOLLET = 1u << 31
#define EPOLLET EPOLLET
};


/* Valid opcodes ( "op" parameter ) to issue to epoll_ctl().  */
#define EPOLL_CTL_ADD 1	/* Add a file descriptor to the interface.  */
#define EPOLL_CTL_DEL 2	/* Remove a file descriptor from the interface.  */
#define EPOLL_CTL_MOD 3	/* Change file descriptor epoll_event structure.  */


typedef union epoll_data
{
	void *ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event
{
	uint32_t events;	/* Epoll events */
	epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;

int epoll_create(int size);
int epoll_create1(int flags);
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
int epoll_pwait(int epfd, struct epoll_event* events, int maxevents, int timeout, const sigset_t* sigmask);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);

int gettid();
int getpid();
int fork();
int setsid();
typedef int mode_t;
int open(const char* pathname, int flags, mode_t mode);
int dup2(int oldfd, int newfd);
int sleep(unsigned int seconds);

#else // !os_win
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SOCKET_ERROR ~0
#endif // WIN32

#ifndef SOMAXCONN
#define SOMAXCONN 128
#endif

#endif // __LINUX_DUMMY_H__


