#ifndef _SIGS_H
#define _SIGS_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <vector>

typedef void (*sig_handler)(int);
void signalFunc(int signal, sig_handler mySigHandler);
void stopSignal(int);
void terminateSignal(int);


#endif
