/**
 * Functions to handle signals
 *
 */

#ifndef SIGNAL_HEADER
#define SIGNAL_HEADER

// wait for all dead child processes
void sigchld_handler(int sig);

#endif