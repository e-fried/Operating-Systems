//
//  server.h
//  hw3
//
//  Created by Elisha Fried on 08/01/2019.
//  Copyright © 2019 Elisha Fried. All rights reserved.
//

#ifndef server_h
#define server_h
#define MAX_SIZE_DATA 512
#define ACK_PACK_SIZE 4
#define BYTE_SIZE 1

#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

typedef enum {FALSE,TRUE} bool;
//**********************************************************************************************************************************************
// this is a struct to send an acknowledgement to the client, the struct has the following fields
// op- this represents the appropriate opcode for an ack
// blocknum- this represents the previous block of data that has been sent
//**********************************************************************************************************************************************

typedef struct ack_BUFFER_{
    unsigned short op ;
    unsigned short blockNum ;
} __attribute__((packed)) ack_B;

//**********************************************************************************************************************************************
// this is a struct to recieve a write request from the client, the struct has the following fields
// op- this represents the appropriate opcode for the write request
// filename- this is the name of the filename to write the data to
//**********************************************************************************************************************************************

typedef struct writeRQ_BUFFER_{
    unsigned short op ;
    char filename_[MAX_SIZE_DATA];
} __attribute__((packed)) writeRQ_B;

//**********************************************************************************************************************************************
// this is a struct to recieve data from the client
// op- this represents the appropriate opcode for data pack transmission
// blocknum- this represents the current block of data that has been recieved
//**********************************************************************************************************************************************

typedef struct data_BUFFER_{
    unsigned short op ;
    unsigned short blockNum ;
    char data[MAX_SIZE_DATA];
} __attribute__((packed)) data_B;

void ACK_Print(unsigned short blocknum);







#endif /* server_h */
