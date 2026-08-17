#include "../headers/signal.h"

void	signal_handler(void) {
	return ;
}

static sighandler_t signal_table[MAX_SIGNALS];

#define QUEUE_SIZE 64

typedef struct {
    int signals[QUEUE_SIZE];
    volatile u32 head;
    volatile u32 tail;
} signal_queue_t;

static signal_queue_t queue;

void signal_init(void) {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        signal_table[i] = 0;
    }
    queue.head = 0;
    queue.tail = 0;
}

int signal_register(int signum, sighandler_t handler) {
    if (signum <= 0 || signum >= MAX_SIGNALS) {
        return -1;
    }
    signal_table[signum] = handler;
    return 0;
}

void signal_emit(int signum) {
    if (signum > 0 && signum < MAX_SIGNALS) {
        sighandler_t handler = signal_table[signum];
        if (handler) {
            handler(signum);
        }
    }
}

// ---


bool schedule_signal(int signum) {
    if (signum <= 0 || signum >= MAX_SIGNALS) {
        return false;
    }

    u32 next_head = (queue.head + 1) % QUEUE_SIZE;

    if (next_head == queue.tail) {
        return false;
    }

    queue.signals[queue.head] = signum;
    queue.head = next_head;
    return true;
}

void dispatch_scheduled_signals(void) {
    while (queue.tail != queue.head) {
        int signum = queue.signals[queue.tail];
        queue.tail = (queue.tail + 1) % QUEUE_SIZE;

        signal_emit(signum);
    }
}
