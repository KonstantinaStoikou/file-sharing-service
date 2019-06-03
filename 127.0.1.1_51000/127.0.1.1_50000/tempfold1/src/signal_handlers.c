#include "../include/signal_handlers.h"
#include <signal.h>
#include "../include/defines.h"

volatile sig_atomic_t exit_var = 0;

void exit_action(int sig) {
    exit_var = 1;
    return;
}
