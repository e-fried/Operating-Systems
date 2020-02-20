//
//  atm.cpp
//  hw2
//
//  Created by Elisha Fried on 18/12/2018.
//  Copyright © 2018 Elisha Fried. All rights reserved.
//

#include "atm.h"
#include "bank.h"
#define MAX_LINE_SIZE 200
#define MAX_ARG 50
#define MAX_FILE_NAME 200
using namespace std;

//globals
extern vector <Account> accountList;
extern int bankBalance;
extern int doneFlag;
extern ofstream log_file;
extern Readers_Writers vectorLock;
extern Readers_Writers logLock;


//**************************************************************************************
// function name: atmRun
// Description: function to parse input for ATM and perform  the commands accordingly
// Parameters: arg struct consisting of file name and ATM number
// Returns: void
//**************************************************************************************
void* atmRun(void *arg){
    try {
    ATM *currentAtm= (ATM*)arg;
    char filename[MAX_FILE_NAME];
    strcpy(filename, currentAtm->ATMInput_.c_str());
    int atmNumber= currentAtm->ID_;
    char cmdString[MAX_LINE_SIZE];
    ifstream file2read (filename);
    string line;
    
        
        while(getline(file2read,line)){
            usleep(100000);
            cmdString[0]='\0';
            strcpy(cmdString, line.c_str());
            cmdString[strlen(line.c_str())]='\0';// removed -1
            char* cmd;
            char* args[MAX_ARG];
            char* delimiters = (char *)" \t\n";
            int i = 0, num_arg = 0;
            bool illegal_cmd = false; // illegal command
            cmd = strtok(cmdString, delimiters);
            if (cmd == NULL)
                return 0;
            args[0] = cmd;
            for (i=1; i<MAX_ARG; i++)
            {
                args[i] = strtok(NULL, delimiters);
                if (args[i] != NULL)
                    num_arg++;
                
            }

            if (!strcmp(cmd, "O") && num_arg==3 ){
                vectorLock.enterWriter();
                addAccount(atoi(args[1]), atoi(args[2]), atoi(args[3]), atmNumber);
                vectorLock.leaveWriter();
                continue;
                
            }
            
            if (!strcmp(cmd, "L") && num_arg==2 ){
                vectorLock.enterReader();
                upgradeAccount(atoi(args[1]), atoi(args[2]), atmNumber);
                vectorLock.leaveReader();
                continue;
            }
            if (!strcmp(cmd, "D") && num_arg==3 ){
                vectorLock.enterReader();
                depositToAccount(atoi(args[1]), atoi(args[2]), atoi(args[3]), atmNumber);
                vectorLock.leaveReader();
                continue;
                
            }
            if (!strcmp(cmd, "W") && num_arg==3 ){
                vectorLock.enterReader();
                withdrawFromAccount(atoi(args[1]), atoi(args[2]), atoi(args[3]), atmNumber);
                vectorLock.leaveReader();
                continue;
                
            }
            if (!strcmp(cmd, "B") && num_arg==2 ){
                vectorLock.enterReader();
                getBalance(atoi(args[1]), atoi(args[2]), atmNumber);
                vectorLock.leaveReader();
                continue;
            }
            if (!strcmp(cmd, "T") &&num_arg==4){
                 vectorLock.enterReader();
                transferBetweenAccounts(atoi(args[1]), atoi(args[2]), atoi(args[3]), atoi(args[4]), atmNumber );
                sleep(1);
                 vectorLock.leaveReader();
                continue;
                
            }
            else{//illegal CMD
                illegal_cmd = true;
                cout<< "Error: Illegal Arguments"<< endl;// add print
            }
            
            
            
        }
    
    file2read.close();
    
    
    
    
    
    pthread_exit(NULL);
    }
    catch(std::exception& e){
        cerr << "ATM function went wrong" << endl;
        exit(1);
    }
}
