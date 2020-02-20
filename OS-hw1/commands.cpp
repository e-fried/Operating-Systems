//		commands.c
//********************************************
#include "commands.h"
using namespace std;
string fgprogname;
extern pid_t runningPid;
extern vector <Job> jobs;
extern vector <Job> waiting;
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(vector<Job> &jobs, char* lineSize, char* cmdString,char* previousCd, vector<string> &history,char* L_Fg_Cmd)
{
	char* cmd;
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = (char *)" \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;
   	args[0] = cmd;
   	fgprogname=args[0];
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;

	}
	/*************************************************/
	if (!strcmp(cmd, "cd") )
	{
		if(num_arg==1){
			char tempCd[PATH_MAX];
			getcwd(tempCd, PATH_MAX);
				if (*args[1]=='-'){
					if(chdir(previousCd)==-1){
						return 0;
					}

					char myCwd[PATH_MAX];
				 	if(getcwd(myCwd, PATH_MAX)==NULL) {
							perror("Error: pwd ");
				     	return(1);
					}
				 	printf("%s\n", myCwd);
					strcpy(previousCd, tempCd);
				}
				else {
						if(chdir(args[1])==-1){
								printf("smash error: > “%s” - path not found\n", args[1]);
				  			return 1;
						}
						strcpy(previousCd,tempCd);
						}
		}
		else{
			illegal_cmd=true;
		}
	}

	/*************************************************/
	else if (!strcmp(cmd, "pwd"))
	{
		if(num_arg==0){
			char myCwd[PATH_MAX];
	    if(getcwd(myCwd, PATH_MAX)==NULL) {
				perror("Error: pwd ");
	      return(1);
			}
	    printf("%s\n", myCwd);
	    return 0;
		}
		else{
			illegal_cmd=true;
		}
	}
	/*************************************************/
		else if (!strcmp(cmd, "history"))
		{
			if(num_arg==0){
				if(history.size()>0){
					for(unsigned int i=0;i<history.size();i++){
						cout << history[i] << endl;
					}
				}
			}
			else{
				illegal_cmd=true;
			}
		}


	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if(num_arg==2){
		int value;
		value= rename(args[1], args[2]);

		if(!value)
		    {
		        cout<< args[1]<< " has been renamed to " << args[2] << endl;
		    }
		    else
		    {
		        perror("Error in mv");
		    }
		    return 0;
			}
			else{
				illegal_cmd==true;
			}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		int sigNumber=-1;
		if(num_arg==2 && args[1][0]=='-'){
			sigNumber=-1*atoi(args[1]);
			if (jobs.size() < (atoi(args[2])) || (atoi(args[2])<=0) || sigNumber<=0 ){
				cout << "smash error:> kill job-job does not exist"<< endl;
				return 1;
			}
			pid_t pidToSignal=(jobs[atoi(args[2])-1]).getPID();
			cout<<"Signal"<< args[1]<<" was sent to "<<pidToSignal<< endl;
			if (kill(pidToSignal, sigNumber)==-1){
				cout << "smash error:> kill job - cannot send signal"<< endl;
				return 1;
			}
			return 0;
		}
		else{
			illegal_cmd=true;
		}

	}
	/*************************************************/

	else if (!strcmp(cmd, "jobs"))
	{
		if(num_arg==0){
			int flag=1;
			while(flag)
			{
				int size=jobs.size();
				for(int i=0; i<size;i++)
						{
							int status;
							if((waitpid(jobs[i].getPID(),&status,WNOHANG)>0)&&(flag!=2))
							{
								flag=2;
								jobs.erase(jobs.begin()+i);
							}
						}
				if(flag==2)
						{flag=1;}
				else if(flag==1)
						{flag=0;}
		}
			for(unsigned int i=0;i< jobs.size(); i++){

					cout<< "[" << i+1 <<"] ";
					(jobs[i]).print();
					for(int j=0; j<waiting.size();j++)
					{
						if(waiting[j].getPID()==jobs[i].getPID())
						{
							cout<<" (Stopped)";
						}
					}
					cout << endl;
				}
			}
			else{
				illegal_cmd=true;
			}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid"))
	{
		if(num_arg==0){
			cout  << "smash pid is " << getpid() << endl;
		}
		else{
			illegal_cmd=true;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		int index=0;
		if(num_arg==0 || num_arg==1){
			if(args[1]==NULL){
				index= jobs.size()-1;
			}
			else{
				index=atoi(args[1])-1;
			}
			if( index >= jobs.size() || index <0 ){
				cout << "smash error: > no background processes with this index"<< endl;
				return 0;
			}
			pid_t pidToFg=jobs[index].getPID();
			cout << jobs[index].getName() << endl;
			fgprogname=jobs[index].getName();
			int statusFg;
			runningPid=pidToFg;
			cout<<"signal SIGCONT was sent to "<<pidToFg<< endl;
			kill(pidToFg, SIGCONT);


			waitpid(pidToFg,&statusFg,WUNTRACED);
				if(!WIFSTOPPED(statusFg)){
					if(WIFEXITED(statusFg)){
							if (waitpid(pidToFg,&statusFg,WNOHANG)<0){

								jobs.erase(jobs.begin()+index);
								for(int i=0; i<waiting.size();i++)
									{
											if(waiting[i].getPID()==pidToFg)
												waiting.erase(waiting.begin()+i);
									}
							}
						}
				}
		}
		else{
				illegal_cmd=true;
		}

	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		deleteFinishedJobs();
		if(num_arg==0 || num_arg ==1 ){
			int index=0;
			pid_t pidToBg;
				if(args[1]==NULL){
					if(waiting.size()==0){
						cout<< "there are no stopped processes"<< endl;
						return 0;
					}

					pidToBg=waiting.back().getPID();
					waiting.pop_back();
					for(int i=0; i<jobs.size();i++)
						{
							if(jobs[i].getPID()==pidToBg)
								{index=i;}
						}
					cout << jobs[index].getName() << endl;
					cout<<"signal SIGCONT was sent to "<<pidToBg<< endl;
					kill(pidToBg, SIGCONT);
				}
				else{
					index=atoi(args[1])-1;
					if(index <0 || index>jobs.size() || index> waiting.size() ){
						cout<< "no job with this index"<< endl;
						return 0;
					}

					pidToBg=jobs[index].getPID();
					cout << jobs[index].getName() << endl;
					cout<<"signal SIGCONT was sent to "<<pidToBg<< endl;
					kill(pidToBg, SIGCONT);
					for(int i=0; i<waiting.size();i++)
					{
						if(waiting[i].getPID()==pidToBg)
							{waiting.erase(waiting.begin()+i);}
					}
				}
		}
		else{
			illegal_cmd=true;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		if(num_arg==1 && !strcmp(args[1], "kill"))
   		{
   			deleteFinishedJobs();
   		   	for(unsigned int i=0; i< jobs.size();i++)
   		   	{
   		   		pid_t currentPid= jobs[i].getPID();

   				kill(currentPid, SIGTERM );
   				cout<< "["<<i+1<<"]"<< jobs[i].getName()<<" -Sending SIGTERM...";
   				for (int j=0; j<5; j++){
   					if(waitpid(currentPid,NULL,WNOHANG)>0){
   						sleep(1);
   					}
   					else{
   						j=5;
   						cout<< " Done."<< endl;
   					}
   				}
   				if(waitpid(currentPid,NULL,WNOHANG)>0){
   					kill(currentPid, SIGKILL );
   					cout << " (5 sec passed) Sending SIGKILL... Done." << endl;
   				}
   		   	}
   		   	delete[] (L_Fg_Cmd);
   		   	exit(0);

   		 }

   		else if(num_arg==0)
   		{
   			delete[] (L_Fg_Cmd);
   			exit(0);

   		}
			else{
				illegal_cmd=true;
			}
	}
	/*************************************************/
	else // external command
	{

 		ExeExternal(args, cmdString);

	 	return 0;
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork())
	{
    		case -1:
					perror("error in execution of external command");
					exit(1);
        	case 0:
                	// Child Process
        			setpgrp();

               		if(execvp(args[0], args) <0){
               			perror("error in child execution of external command");
     					exit(1);
               		}

               		break;

			default:


					int status;
					pid_t newPID= (pid_t)pID;
					runningPid=newPID;
					waitpid(newPID, &status, WUNTRACED);

	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command- not supported in this code
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		perror("complicated command- we don't support this issue");
		exit(1);
	}
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, jobs vector
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, vector <Job> &jobs)
{

	char* Command;
	char* delimiters = (char *)" \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
				int i = 0, num_arg = 0;
						Command = strtok(lineSize, delimiters);
				if (Command == NULL)
					return 0;
					args[0] = Command;
				int temp=0;
				for (i=1; i<MAX_ARG; i++)
				{
					args[i] = strtok(NULL, delimiters);
					if (args[i] != NULL){
						temp=i;
						num_arg++;
					}

				}

				args[temp]=NULL;
				time_t time0;
				time(&time0);

				int pID;
						switch(pID = fork())
				{
							case -1:

								perror("failed to run bg command");
								exit(1);

							case 0 :

												// Child Process
								setpgrp();
								if(execvp(args[0], args) <0){
									perror("error in child execution of bg command");
									exit(1);
								}

								return 0;
								break;

							default:
								Job x = Job(args[0],pID, time0);
								jobs.push_back(x);
								return 0;

				}

	}
	return -1;
}

//**************************************************************************************
// function name: DeleteFinishedJobs
// Description: if jobs are not running anymore, erase them from jobs list
// Parameters: none, accesses global variables
// Returns: void
//**************************************************************************************
void deleteFinishedJobs(){
	int flag=1;
			while(flag)
			{
				int size=jobs.size();
				for(int i=0; i<size;i++)
							{
								int status;
								if((waitpid(jobs[i].getPID(),&status,WNOHANG)>0)&&(flag!=2))
								{
									flag=2;
									jobs.erase(jobs.begin()+i);
								}
							}
				if(flag==2)
					{flag=1;}
				else if(flag==1)
					{flag=0;}
			}
}


//**************************************************************************************
// Job class functions
// These function are used to access inner fields in job objects
// getName- get name of command
// getPID= get PID of desired jobs
// getTimer- get the time the job entered the job list
// print- print function of job
//**************************************************************************************
Job::Job( string name, pid_t PID, time_t timer) :

		name_(name), PID_(PID),  timer_(timer) {}

string Job::getName() {
	return name_;
}
pid_t Job::getPID() {
	return PID_;
}
time_t Job::getTimer() {
	return timer_;
}
void Job::print() {
	time_t current_t;
	time(&current_t);
	cout << name_ << " : "<< PID_ << " " <<(current_t- timer_) << " secs" ;
}
