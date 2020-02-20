#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <iostream>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define PATH_MAX 1024

using namespace std;

//**************************************************************************************
// Job class
// This class is an object holding a process in background and its data
// job has 3 data fields-
// name- name of the command entered
// pid- pid number of the process
// timer- system time of the time the process was created
//**************************************************************************************
class Job {
public:
	Job(string name, pid_t PID, time_t timer);
	pid_t getPID();
	string getName();
    time_t getTimer();
	void print();

private:
    string name_;
    pid_t PID_;
    time_t timer_;
};

int ExeComp(char* lineSize);
int BgCmd(char* lineSize, vector<Job> &jobs);
int ExeCmd(vector<Job> &jobs, char* lineSize, char* cmdString,char* previousCd,vector<string> &history,char* L_Fg_Cmd);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
void deleteFinishedJobs();


#endif

