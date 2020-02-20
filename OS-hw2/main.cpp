//******************************************************************
// program name: Bank
// Description: This program is a system to manage acess of different ATM's to the main branch Bank in order to
// execute  a number of commands, creating an account, Transferring between accounts, Deposit or Withdrawl of money
//
// the program will keep a log of the different commands results
//**************************************************************************************

/*    main.c
 main file. This file contains the main function of Bank
 *******************************************************************/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "account.h"
#include "bank.h"
#include "atm.h"
#include <vector>
#include <iostream>
#include <string>
#include <pthread.h>
#define MAX_FEE_PERCENT 4
#define MIN_FEE_PERCENT 2


using namespace std;

// declare and initalize Globals
vector <Account> accountList;
int bankBalance;
int doneFlag;
Readers_Writers vectorLock;
Readers_Writers logLock;
ofstream log_file;


//**************************************************************************************
// function name: takeFee
// Description: function to manage the fees the acccounts transfer to the Bank
// Parameters: Bank balance
// Returns: void
//**************************************************************************************


void *takeFee(void*){
  try{
    while(!doneFlag){
        sleep(3);
        vectorLock.enterWriter();
        int size= (int)accountList.size();
        int bal=0;
        int random = rand() % (int)(MAX_FEE_PERCENT - MIN_FEE_PERCENT + 1);
        int random_perc = MIN_FEE_PERCENT + random;
        for(int i=0;i<size; i++){
            accountList[i].accountLock.enterWriter();
        }
        if(!doneFlag)
        {
      
            for(int i=0; i<size; i++){
                if(accountList[i].getVIP()==false)
                {
                    bal=accountList[i].innerGetBalance();
                
                    int amount=(bal*random_perc)/100;
                    accountList[i].innerWithdrawMoney((amount));
                    bankBalance+=(amount);
                    write2log(9, random_perc,accountList[i].getAccountNum(), 0, amount, 0, 0, 0);
                }
            }
        }
        for(int i=0;i<size; i++){
            accountList[i].accountLock.leaveWriter();
        }
        vectorLock.leaveWriter();
    }
    
    pthread_exit(NULL);
  }
  catch(std::exception& e){
    cerr << "take fee function went wrong" << endl;
    exit(1);
  }
}

//**************************************************************************************
// function name: main
// Description: main function of Bank recieves input files from number of ATM's and creates a thread for each ATM
// so they can run Simulatneously
// another thread is created for printing the current bank Status, and one more to manage the fees the
// accounts pay to the Bank
//**************************************************************************************
 
int main(int argc, const char * argv[]) {
    try{
    if(argc<=1 || atoi(argv[1])!= argc-2){
        cout<<"Illegal arguments" <<endl;
    }
    
    
    log_file.open("log.txt", std::ofstream::out);
    
    int atmNum= argc-2;
    //initalize globals
    bankBalance=0;
    doneFlag=0;
    ATM* ATM_array[argc -2];// added
    
    // creating the different threads for ATM's
    pthread_t atmThreads[atmNum];
    for (int i=0;i<atmNum ;i++){
        ATM_array[i]= new ATM(i+1,argv[i+2]);
        pthread_create(&atmThreads[i], NULL, atmRun, (void *)ATM_array[i]);
    }
    
    // creating threads for Bank and output
    pthread_t bank;
    pthread_t output;
    
    pthread_create(&bank, NULL,&takeFee , (void*)&bankBalance);
    pthread_create(&output, NULL, outputPrint, (NULL));
    
    
    for (int i=0; i<atmNum; i++){
        pthread_join(atmThreads[i],NULL);
    }
    doneFlag=1;
    pthread_join(bank, NULL);
    
    pthread_join(output, NULL);

    for (int i=0 ; i<argc-2 ; i++){
        delete ATM_array[i];
    }
        
       
    
    log_file.close();
  
    return 0;
  }
  catch(std::exception& e){
    cerr << "main function went wrong" << endl;
    exit(1);
  }
}


