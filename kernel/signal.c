#include <signal.h>

static sighandler_t	signal_table[MAX_SIG];

_bool	sig = false;

signal_queue_t	queue;

void	signal_init(void) {
	for (int i = 0; i < MAX_SIG; i++) {
		signal_table[i] = 0;
	}
	queue.head = 0;
	queue.tail = 0;
}

int	signal_register(int signum, sighandler_t handler) {
	if (signum <= 0 || signum >= MAX_SIG) {
		return -1;
	}
	signal_table[signum] = handler;
	return 0;
}

void	signal_emit(int signum) {
	if (signum > 0 && signum < MAX_SIG) {
		sighandler_t handler = signal_table[signum];
	if (handler) {
		handler(signum);
	}
    }
}


_bool	schedule_signal(int signum) {
	if (signum <= 0 || signum >= MAX_SIG) {
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

void	dispatch_scheduled_signals(void) {
	while (queue.tail != queue.head) {
		int signum = queue.signals[queue.tail];
		queue.tail = (queue.tail + 1) % QUEUE_SIZE;

		signal_emit(signum);
	}
	sig = false;
}
