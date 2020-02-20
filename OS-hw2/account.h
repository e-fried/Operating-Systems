
#ifndef _ACCOUNTS_H
#define _ACCOUNTS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

typedef enum Result_ { FAILURE, SUCCESS } Result;

using std::string;


//**************************************************************************************
// Readers_Writers class
// This class is a Readers writers class to schedule the access to shared memory
// the class uses 3 mutexes and a counter in order to assure that many reades can acess the memory simultaneously
// but when a writer enters no one else acceses
//**************************************************************************************
class Readers_Writers {
public:
    pthread_mutex_t queue, critical, writers;
    int reader;
    Readers_Writers(){
        reader=0;
        pthread_mutex_init(&queue, NULL);
        pthread_mutex_init(&critical, NULL);
        pthread_mutex_init(&writers, NULL);
    }
    void enterReader(){
        pthread_mutex_lock(&queue);
        pthread_mutex_lock(&critical);
        reader++;
        if(reader==1){
            pthread_mutex_lock(&writers);
        }
        pthread_mutex_unlock(&critical);
        pthread_mutex_unlock(&queue);
        
    }
    void enterWriter(){
        pthread_mutex_lock(&queue);
        pthread_mutex_lock(&writers);
        pthread_mutex_unlock(&queue);

    }
    void leaveReader(){
        pthread_mutex_lock(&critical);
        reader--;
        if(reader==0){
            pthread_mutex_unlock(&writers);
        }
        pthread_mutex_unlock(&critical);
    }
    void leaveWriter(){
        pthread_mutex_unlock(&writers);
    }
};


//**************************************************************************************
// Account class
// This class is a object containing the different fields that create an account
// there are 4 private fields
// accountNum- the number of the account, the identifier of the accunt
// password- password to access the account
// balance- current balance in given account
// isVIP- is this account a VIP account thus the bank does not take fees from it
//**************************************************************************************


class Account {
public:
  Account(int accountNum, int password, int balance);
  void withdrawMoney(int money);
  void  depositMoney(int money);
  void upgrade();
  void innerWithdrawMoney(int money);
  void  innerDepositMoney(int money);
  int  innerGetBalance();
  
   
  int getAccountNum();
  int getPassword();
  int getBalance();
  bool getVIP();
  void print();
  Readers_Writers accountLock;

  

    

private:
  int accountNum_;
  int password_;
  int balance_;
  bool is_VIP_;
};



#endif

