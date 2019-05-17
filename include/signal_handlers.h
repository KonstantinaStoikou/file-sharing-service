#ifndef SIGNAL_ACTIONS_HEADER
#define SIGNAL_ACTIONS_HEADER

#include <signal.h>

extern volatile sig_atomic_t exit_var;
extern volatile sig_atomic_t failure_var;

// Action that sets 1 to exit_var on signal
void exit_action(int sig);

#endif