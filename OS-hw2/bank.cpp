/*
 * bank.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: compm
 */
#include "bank.h"
#define HALF_SECOND 500000// millisecond
#define THREE_SECONDS 3
#define EMPTY_ARG 0

using namespace std;

//Global Varibles
extern vector <Account> accountList;
extern int bankBalance;
extern int doneFlag;
extern ofstream log_file;
extern Readers_Writers vectorLock;
extern Readers_Writers logLock;




/*double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}
 */


//********************************************
// function name: addAccount
// Description: creates a new account in the Bank with the given parameters
// Parameters:
//      account- account Number/identifier
//      password
//      atmNumber- number of the ATM(thread) which created the account
// Returns: true or false if the function succeded
//**************************************************************************************


bool addAccount(int account, int password, int balance, int atmNumber )
{
    int size= (int)accountList.size();
    int flag=0;
    int count=0;
   
    for(int i=0; i<size; i++){
        if(accountList[i].getAccountNum()<account)
        {
            count+=1;
        }
        if(accountList[i].getAccountNum()==account)
            flag =1;
    }
    if (flag==0){
        Account x =Account(account,password,balance);
        accountList.insert(accountList.begin()+count,x);
        sleep(1);
        write2log(2, atmNumber, account, balance, 0, password,0, 0 );
        return true;
    }
    else
    {
        write2log(1, atmNumber, account, balance, 0, password, 0, 0);
        return false;
    }
              
}

//********************************************
// function name: depositToAccount
// Description: deposits an amount of money to the account with the password given if password is correct
// Parameters:
//      account- account Number/identifier,
//      password
//      atmNumber- number of the ATM(thread) which gave the command
//      amount- amount of money to deposit
// Returns: true or false if the function succeded
//**************************************************************************************
bool depositToAccount(int account, int password, int amount, int atmNumber)
{
    int size= (int)accountList.size();
    int flag=0;
    int tmp;
    
        for(int i=0; i<size; i++){
            if(accountList[i].getAccountNum()==account)
            {
                if(accountList[i].getPassword()==password)
                {
                    accountList[i].depositMoney(amount);
                    flag=2;
                    tmp=i;
                }
                else
                    flag=1;
            }
        }
        if (flag==1){
            write2log(0, atmNumber, account, 0, amount, password, 0, 0 );//error to log wrong password
            return false;
        }
        else if (flag==2){
            write2log(3, atmNumber, account, accountList[tmp].getBalance(), amount, password, 0, 0);
            return true;
        }
                  
        else
        {
             write2log(8, atmNumber, account, 0, 0, password, 0, 0);
             // account num dont exist, need to check waht to return
            return false;
        }
    
}
//********************************************
// function name: WithdrawFromAccount
// Description: Withdraws an amount of money from the account if password is correct
// Parameters:
//      account- account Number/identifier
//      password
//      atmNumber- number of the ATM(thread) which gave the command
//      amount- amount of money to withdraw
// Returns: true or false if the function succeded
//**************************************************************************************

bool withdrawFromAccount(int account, int password, int amount, int atmNumber)
{
    int size= (int)accountList.size();
    int flag=0;
    int tmp;
    for(int i=0; i<size; i++){
        if(accountList[i].getAccountNum()==account)
        {
            if(accountList[i].getPassword()==password)
            {
                if(accountList[i].getBalance()>=amount)
                {
                    accountList[i].withdrawMoney(amount);
                    flag=3;
                    tmp=i;
                    
                }
                else
                    flag=2;
            }
            else
                flag=1;
        }
    }
    if (flag==1){
        write2log(0, atmNumber, account, 0, amount, password, 0, 0 );
        return false;
        //error to log wrong password
    }
    else if (flag==2){
         write2log(4, atmNumber, account, 0, amount, password, 0, 0 );
        return false;
        // no money erorr
        }
    else if (flag==3){
        write2log(5, atmNumber, account, accountList[tmp].getBalance(), amount, password, 0, 0);
        return true;
    }
    else{
         write2log(8, atmNumber, account, 0, 0, password, 0, 0);
        return false;
        // account num dont exist- what to return , illegal command?
    }
    
}
//********************************************
// function name: transferBetweenAccounts
// Description: transfers an amount of money from account1 to account2 if password of account1 is correct
// Parameters:
//      account1- account Number/identifier of account to take from
//      password- password of account giving
//      account2- account recieving
//      amount- amount of money to transfer
//      atmNumber- number of the ATM(thread) which the command came from
// Returns: true or false if the function succeded
//**************************************************************************************
bool transferBetweenAccounts(int account1, int password1, int account2, int amount, int atmNumber)
{
    int size= (int)accountList.size();
    int flag=0;
    int flag2=0;
    int tmp1;
    int tmp2;
    
    for(int i=0; i<size; i++){
        if(accountList[i].getAccountNum()==account1)
        {
            if(accountList[i].getPassword()==password1)
            {
                if(accountList[i].getBalance()>=amount)
                {
                    for(int j=0; j<size; j++){
                        if(accountList[j].getAccountNum()==account2)
                        {
                            flag2=1;
                            tmp1=i;
                            tmp2=j;
                        }
                
                    }
                    flag=3;
                }
                else
                    flag=2;
            }
            else
                flag=1;
        }
    }
   


    if ((flag2==0)&&(flag==3)){
        write2log(8, atmNumber, account2, 0, 0, password1, 0, 0);
    }
    else if (flag==1){
        write2log(0, atmNumber, account1, 0, amount, password1, account2, 0);

        return false;
                      //error to log wrong password
    }
    else if (flag==2){
        write2log(4, atmNumber, account1, 0, amount, password1, account2, 0);
        return false;
                          // no money erorr
    }
    else if ((flag==3)&&(flag2==1)){
        int a= max(tmp2, tmp1);
        int b= min(tmp2, tmp1);
        accountList[a].accountLock.enterWriter();
        accountList[b].accountLock.enterWriter();
        accountList[tmp2].innerDepositMoney(amount);
        accountList[tmp1].innerWithdrawMoney(amount);
        accountList[a].accountLock.leaveWriter();
        accountList[b].accountLock.leaveWriter();
        write2log(7, atmNumber, account1, accountList[tmp1].getBalance(), amount, password1, account2, accountList[tmp2].getBalance());
        return true;
                              // massege succsess to log
    }
    else{
        write2log(8, atmNumber, account1, 0, 0, password1, 0, 0);
        return false;
                                  // account num dont exist
    }
    return false;
}


//********************************************
// function name: getBalance
// Description: prints to log file the current balance in the account given
// Parameters:
//      account- account Number/identifier
//      password
//      atmNumber- number of the ATM(thread) which gave the command
//
// Returns: true or false if the function succeded
//**************************************************************************************

bool getBalance(int account, int password, int atmNumber)
{
    int size= (int)accountList.size();
    int flag=0;
    int bal=0;
    int tmp;
    for(int i=0; i<size; i++){
        if(accountList[i].getAccountNum()==account)
        {
            if(accountList[i].getPassword()==password)
            {
                bal=accountList[i].getBalance();
                flag=2;
                tmp=i;
            }
            else{
                flag=1;
            }
        }
    }
    if (flag==1){
          write2log(0, atmNumber, account, 0, 0, password, 0, 0);
        return false;
    }
    else if (flag==2){
        write2log(6, atmNumber, account, accountList[tmp].getBalance(), 0, password, 0, 0);
        // success massege
        return true;
    }
    else{
        write2log(8, atmNumber, account, 0, 0, 0, 0, 0);
        // account num dont exist
        return false;
    }
                              
}

//********************************************
// function name: upgradeAccount
// Description: upgrades given account to VIP account thus, no fee is taken from the account by Bank
// Parameters:
//      account- account Number/identifier
//      password
//      atmNumber- number of the ATM(thread) which gave the command
// Returns: true or false if the function succeded
//**************************************************************************************

bool upgradeAccount(int account, int password, int atmNumber)
{
    int size= (int)accountList.size();
    int flag=0;
    for(int i=0; i<size; i++){
        if(accountList[i].getAccountNum()==account)
        {
            if(accountList[i].getPassword()==password)
            {
                accountList[i].upgrade();
                flag=2;
            }
            else
                flag=1;
        }
    }
    if (flag==1){
        write2log(0, atmNumber, account, 0, 0, password, 0, 0);
        return false;
        //error to log wrong password
    }
    else if (flag==2){
        return true;
        // success massege
    }
    else{
        write2log(8, atmNumber, account, 0, 0, 0, 0, 0);
        return false;
    // account num dont exist
    }
                              
                              
}

//********************************************
// function name: Write2log
// Description: writes to log file description of command executed and its results
// Parameters:
//      casenum- used to difreentiate between the different result of command, thus printing different ouput to log
//      password
//      atmNumber- number of the ATM(thread) which gave the command
//      amount- amount of money to withdraw
//      bal- current balance in given account
//      accountID- account Number of account command was executed on
//      target_account- used for transfer command, account Id of target account for transfer
//      target_bal-used for transfer command, account balance of target account for transfer

// Returns: void
//**************************************************************************************

void write2log(int casenum, int atmid, int accountid, int bal, int amount,int password,int target_account,int target_bal )
{
    logLock.enterWriter();
    switch (casenum){
            
        case 0: log_file << "Error "<<atmid<<": Your transaction failed – password for account id "<<accountid;
            log_file << " is incorrect"<< endl;
            break;
        case 1: log_file <<"Error "<<atmid<<": Your transaction failed – account with the same id exists"<< endl;
            break;
        case 2: log_file <<atmid<<": New account id is "<<accountid<<" with password ";
            log_file <<password<<" and initial balance "<<bal<< endl;
            break;
        case 3: log_file <<atmid<<": Account "<<accountid<<" new balance is ";
            log_file <<bal<<" after "<<amount<<" $ was deposited"<< endl;
            break;
        case 4: log_file <<"Error "<<atmid<<": Your transaction failed – account id ";
            log_file <<accountid<<" balance is lower than "<<amount<< endl;
            break;
        case 5: log_file <<atmid<<": Account "<<accountid<<" new balance is "<<bal;
            log_file<<" after "<<amount<<" $ was withdrew"<< endl;
            break;
        case 6: log_file <<atmid<<": Account "<<accountid<<" balance is "<<bal<< endl;
            break;
        case 7:  log_file <<atmid<<": Transfer "<<amount<<" from account "<<accountid;
            log_file <<" to account "<<target_account<<" new account balance is ";
            log_file <<bal<<" new target account balance is "<<target_bal<< endl;
            break;
        case 8: log_file <<"Error "<<atmid<<": Your transaction failed – account id "<<accountid;
            log_file <<" does not exist"<< endl;
            break;
        case 9: log_file<< "Bank: commissions of "<< atmid<< " % were charged, the bank gained " ;                        log_file<<amount<< " $ from account " <<  accountid<< endl;
            // atmId in this case represents the fee percentage
            break;
    }
   logLock.leaveWriter();
    
    
}


//********************************************
// function name: outputPrint
// Description: prnts to screen the current state of all acounts, this function runs on a seperate thread
// and prints the current bank status every half second
// Parameters: none
// Returns: void
//**************************************************************************************
              
void* outputPrint(void*)
{
  try{
    while(!doneFlag){
        vectorLock.enterReader();
        usleep(HALF_SECOND);
        
        printf("\033[2J");
        printf("\033[1;1H");
        for(int i=0;i<accountList.size(); i++){
            accountList[i].accountLock.enterReader();
        }
        cout<< "Current Bank Status" << endl;
       
        for (int i=0;i<accountList.size();i++){
            accountList[i].print();
        }
        cout<< "The bank has " << bankBalance << " $"<< endl;
        
        for(int i=0;i<accountList.size(); i++){
            accountList[i].accountLock.leaveReader();
        }
        vectorLock.leaveReader();
        
    }
    pthread_exit(NULL);
    }
  catch(std::exception& e){
    cerr << "Output function went wrong" << endl;
    exit(1);
  }
}



