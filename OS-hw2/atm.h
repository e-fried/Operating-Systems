//
//  atm.h
//  hw2
//
//  Created by Elisha Fried on 18/12/2018.
//  Copyright © 2018 Elisha Fried. All rights reserved.
//

#ifndef atm_h
#define atm_h

#include <stdio.h>
#include <string>
#include <pthread.h>

using namespace std;

//**************************************************************************************
// ATM class
// This class is an object holding parameters for atmRun
// job has 2 data fields-
// ATMInout- name of the input file for ATM
// ID- number of ATM thread created
//**************************************************************************************
class ATM {
public:
    ATM(int ID, string ATMInput): ID_(ID), ATMInput_(ATMInput){}
    
    int ID_;
    string ATMInput_;
};

void* atmRun(void *arg);

#endif /* atm_h */
