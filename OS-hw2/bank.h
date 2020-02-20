#ifndef _BANK_H
#define _BANK_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include "account.h"
#include <vector>
#include <iostream>
#include <fstream>



using namespace std;

extern ofstream log_file;

void* takeFee(void*);
bool addAccount(int accountNum, int password, int balance, int atmNumber);
bool depositToAccount(int account, int password, int amount, int atmNumber);
bool withdrawFromAccount(int account, int password, int amount, int atmNumber);
bool transferBetweenAccounts(int account1, int password1, int account2, int balance, int atmNumber);
bool getBalance(int account, int pasword, int atmNumber);
bool upgradeAccount(int account, int password, int atmNumber);
void write2log(int casenum, int atmid, int accountid, int bal, int amount,int password,int target_account,int target_bal );
void* outputPrint(void*);

   

//double fRand(double fMin, double fMax);

#endif
