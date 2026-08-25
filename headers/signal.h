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

#define QUEUE_SIZE 64

typedef void	(*sighandler_t)(int signum);

typedef struct {
    int signals[QUEUE_SIZE];
    volatile u32 head;
    volatile u32 tail;
} signal_queue_t;

extern signal_queue_t	queue;

extern _bool	sig;

//void	signal_handler(void);
void	signal(int sig, void (*handler)(void));

// Public Kernel API
void	signal_init(void);
int	signal_register(int signum, sighandler_t handler);
void	signal_emit(int signum);


// Scheduling API
bool	schedule_signal(int signum);
void	dispatch_scheduled_signals(void);

//sighandler_t	signal_handlers[32] = {0};

#endif
