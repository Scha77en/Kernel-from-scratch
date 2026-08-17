#ifndef SIGNAL_H
#define SIGNAL_H

#include "types.h"

#define MAX_SIG 32

#define SIGHUP   1
#define SIGINT   2   // CTRL + C
#define SIGQUIT  3
#define SIGILL   4
#define SIGTRAP  5
#define SIGABRT  6
#define SIGKILL  9
#define SIGSEGV  11
#define SIGTERM  15  // CTRL + D / Soft Termination


typedef void	(*sighandler_t)(int signum);

void	signal_handler(void);
void	signal(int sig, void (*handler)(void));

// Public Kernel API
void	signal_init(void);
int	signal_register(int signum, sighandler_t handler);
void	signal_emit(int signum);


// Scheduling API
bool	scheduling_signal(int signum);
void	dispatch_scheduled_signals(void);

typedef void	(*sighandler_t)(void);
sighandler_t	signal_handlers[32] = {0};

#endif
