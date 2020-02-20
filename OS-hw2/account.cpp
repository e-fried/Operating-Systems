/*
 * account.cpp
 */

#include "account.h"
#include <iostream>
using namespace std;

//********************************************
// Constructor Function
//
// Parameters: accountum, password, balance
//**************************************************************************************
Account::Account(int accountNum, int password, int balance):
	  accountNum_(accountNum), password_(password), balance_(balance), is_VIP_(false) {}

//********************************************
// function name: withdrawMoney
// Description: dedudcts an amount of money from the Account
// Parameters: money
// Returns: void
//**************************************************************************************
void Account::withdrawMoney(int money){
    accountLock.enterWriter();
      balance_=balance_-money;
    sleep(1);
    accountLock.leaveWriter();

}
//********************************************
// function name: depositMoney
// Description: adds a sum of money to the Account
// Parameters: money
// Returns: void
//**************************************************************************************
void Account::depositMoney(int money){
    accountLock.enterWriter();
    balance_=balance_+money;
    sleep(1);
    accountLock.leaveWriter();
    
}
//********************************************
// function name: upgrade
// Description: turns the account to a VIP account which doesn't pay fees
// Parameters: none
// Returns: void
//**************************************************************************************
void Account::upgrade(){
    accountLock.enterWriter();
    is_VIP_=true;// do I return failure if he is already VIP?
    sleep(1);
    accountLock.leaveWriter();

}
//********************************************
// function name: innerWithdrawMoney
// Description: dedudcts an amount of money from the Account, function without locks used for money
// tranfer between accounts
// Parameters: money
// Returns: void
//**************************************************************************************
void Account::innerWithdrawMoney(int money){
    balance_=balance_-money;
}
//********************************************
// function name: innerDepositMoney
// Description: adds an amount of money to the Account, function without locks used for money
// tranfer between accounts
// Parameters: money
// Returns: void
//**************************************************************************************
void  Account::innerDepositMoney(int money){
    balance_=balance_+money;
}
//********************************************
// function name: innerGetBalance
// Description: returns the current balance in the account function without locks used for money
// tranfer between accounts
// Parameters: none
// Returns: money balance
//**************************************************************************************
int  Account::innerGetBalance(){
    return balance_;
}

//********************************************
// function name: getAccountNum
// Description: returns the accountNu
// Parameters: m
// Returns: accountNum
//**************************************************************************************

int Account::getAccountNum(){
    int x= accountNum_;
    return x;
}

//********************************************
// function name: getPassword
// Description: returns the password of the account
// Parameters: none
// Returns: password
//**************************************************************************************

int Account::getPassword(){
    accountLock.enterReader();
        int x= password_;
    accountLock.leaveReader();
    return x;
}

//********************************************
// function name: getBalance
// Description: returns the current balance in the account
// Parameters: none
// Returns: balance
//**************************************************************************************

int Account::getBalance(){
    accountLock.enterReader();
    int x= balance_;
    sleep(1);
    accountLock.leaveReader();
    return x;
    
}
//********************************************
// function name: getVIP
// Description: returns the VIP status of the account
// Parameters: none
// Returns: is VIP or not
//**************************************************************************************

bool Account::getVIP(){
    bool x= is_VIP_;
    return x;
}

//********************************************
// function name: print
// Description: prints the account status
// Parameters: none
// Returns: void
//**************************************************************************************


void Account::print(){
    cout<<"Account " << accountNum_ << ": Balance - " << balance_<< " $ , Account Password - "<< password_<< endl;    
}




