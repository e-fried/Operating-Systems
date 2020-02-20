// signals.c
// contains signal handler functions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include "commands.h"
using namespace std;

extern pid_t mainPid;
extern vector <Job> jobs;
extern vector <Job> waiting;
extern string fgprogname;
extern pid_t runningPid;


//**************************************************************************************
// function name: signalFunc
// Description: function to initialize a sigaction struct
// Parameters: signal number, function pointer
// Returns: void
//**************************************************************************************
void signalFunc(int signal, sig_handler mySigHandler) {

	struct sigaction mySig;
	sigset_t mask;
	sigfillset(&mask);
	mySig.sa_flags = 0;
	mySig.sa_mask = mask;
	mySig.sa_handler = mySigHandler;
	if(sigaction(signal,&mySig, NULL) == -1)
		perror("problem creating new signal\n");
}

//**************************************************************************************
// function name: terminateSignal
// Description: handler function for SIGINIT (ctrl C ), sends signal to terminate the proccess running
// Parameters: signal number
// Returns: void
//**************************************************************************************
void terminateSignal(int signal){
				if (runningPid!=mainPid)
				{
					if(kill(runningPid,SIGINT)<0)
						{
							perror("failed to send signal");
						}
					cout<< "smash > signal SIGINT was sent to pid"<< runningPid<< endl;
				}
}

//**************************************************************************************
// function name: StopSignal
// Description: handler function for SIGTSTP (ctrl z ), sends signal to stop the proccess running
// and add it to jobs list
// Parameters: signal number
// Returns: void
//**************************************************************************************
void stopSignal(int signal){
if (runningPid!=mainPid)
{
	if(kill(runningPid,SIGSTOP)<0)
	{
		perror("failed to send signal");
	}
	time_t time0;
	time(&time0);
	int flag=-1;
	for(int i=0; i<jobs.size();i++)
	{
		if(jobs[i].getPID()==runningPid)
		{
			flag=i;
		}
	}
	if(flag==-1)
	{
		Job x= Job(fgprogname,runningPid,time0);
		jobs.push_back(x);
		waiting.push_back(x);
	}
	else
	{
		int flag2=-1;
		for (int i=0; i< waiting.size();i++){
			if(waiting[i].getPID()==runningPid){
				flag2=1;
			}
		}
		if(flag2==-1){
			waiting.push_back(jobs[flag]);
		}

	}
	cout<< "smash > signal SIGSTOP was sent to pid "<< runningPid<< endl;

}
else
	return;

}
