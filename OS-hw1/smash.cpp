//******************************************************************
// program name: smash
// Description: This program is a shell for linux (similar to bash) this program can run programs,
// execute a variety of commands, such as cd, pwd, fg, bg, chdir, mv, quit, kill, showpid, history,
// and jobs.
// the program will keep a log of the processes running in the program simultaneously
//**************************************************************************************

/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include <vector>
#include <iostream>
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_QUEUE 50
using std::vector;
using std :: cout;

// declaration of global variables
pid_t mainPid;
pid_t runningPid;
char* L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE];
vector <Job> jobs;
vector <Job> waiting;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];


	/************************************/
	// Init globals
  mainPid=getpid();
  char previousCd [PATH_MAX];
  vector <string> history;

  //signal declarations
  	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c

  signalFunc(SIGTSTP,&stopSignal);
  signalFunc(SIGINT,&terminateSignal);


	L_Fg_Cmd =new char(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL)
			exit (-1);
	L_Fg_Cmd[0] = '\0';

    	while (1)
    	{
    	runningPid=mainPid;
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize)-1]='\0';

		// fill history vector holding list of commands
		if(history.size() >= MAX_QUEUE){
		      history.erase(history.begin());
		    }
		    history.push_back(cmdString);

			// perform a complicated Command
		if(!ExeComp(lineSize)) continue;
					// background command
	 	if(!BgCmd(lineSize, jobs)) continue;
					// built in commands
		ExeCmd(jobs, lineSize, cmdString, previousCd, history,L_Fg_Cmd);




		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}
