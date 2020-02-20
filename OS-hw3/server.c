//
//  server.c
//  hw3
//
//  Created by Elisha Fried on 08/01/2019.
//  Copyright © 2019 Elisha Fried. All rights reserved.
//

//**********************************************************************************************************************************************
// ttftps
// this program is a trivial trivial FTP server, this is a file fransfer protocilbased on UDP
// this program supports only wrtite requests from the client
// the program recieves a write request from the client, if it is succesful returns an acknowledgemnt.
// then the client can sent data at packets of 512 bytes, after every data pack is recieved our server will send a acknoeldgemnt
// of the number of data pack that was recieved
//**********************************************************************************************************************************************


#include "server.h"


const int WAIT_FOR_PACKET_TIMEOUT = 3;
const int NUMBER_OF_FAILURES = 7;

int  main (int argc, char **argv){

    // initalize variables
    int sockfd_listen;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    data_B dataBuffer;
    
    // create socket to listen on
    if((sockfd_listen=socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("TFTP_ERROR:");
    }
    
     memset(&(server_addr), 0, sizeof(server_addr)) ;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   
   
    
    int sock_bind=bind(sockfd_listen, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if(sock_bind==-1){
         perror("TFTP_ERROR:");
        exit(1);
    }
    
    // loop to iterate in order to accept a new write request, every time one has finished
    do{
       
        //declaring  inner variables
        writeRQ_B recieveReq;
        int numbytes;
        unsigned int addr_len;
        char file_name[MAX_SIZE_DATA];
        char* reqMode;
        FILE* fp;
        int filelen = 0;
        int ackCount=0;
        int fatalError=0;
        int lastWriteSize=0;
        int timeoutExpiredCount=0;
        
    
        
        //call method recvfrom, which expects a WRQ packet
        addr_len = sizeof(struct sockaddr);
        numbytes = recvfrom(sockfd_listen, &recieveReq, 516 ,0, (struct sockaddr *)&client_addr, &addr_len);
        //check for recvfrom-related errors
        if (numbytes == -1) {
            perror("TFTP_ERROR:");
            continue;
        }
        else if(numbytes==0){  //The return value will be 0 when the peer has performed an orderly shutdown.
            printf("client has performed a shutdown");
            continue;
        }
        recieveReq.op=ntohs(recieveReq.op);
        strcpy(file_name,recieveReq.filename_);
        reqMode = recieveReq.filename_ + strlen(recieveReq.filename_) + 1;
        
        if(strcmp(reqMode, "octet")){
            printf("support octet mode only\n");
            continue;
        }
        if(recieveReq.op !=2){
            printf("Wrong opcode\n");
            continue;
        }
          printf("IN:WRQ,%s,%s\n",file_name, reqMode );
        
        
        
        
        //open file and get length
        fp = fopen(file_name, "w+");
        if (fp==NULL){   
            perror("TFTP_ERROR:");
            continue;
        }
        
        // first ack sent after write request succeded
        ack_B acknowledge;
        acknowledge.op=htons(4);
        acknowledge.blockNum=htons(ackCount);
        if(sendto(sockfd_listen, &acknowledge, ACK_PACK_SIZE, 0,(struct sockaddr *)&client_addr , addr_len)<0){
             perror("TFTP_ERROR:");
            continue;
        }
        ACK_Print(ackCount);
        
        
        
       
    
    
        do
        {
            do
            {
                 int dataFlag=0;
        
                do
                {
                    struct timeval my_timeout;
                    my_timeout.tv_sec=WAIT_FOR_PACKET_TIMEOUT;
                    my_timeout.tv_usec=0;
                    fd_set read_fd;
                    FD_ZERO(&read_fd);
                    FD_CLR(sockfd_listen, &read_fd);
                    FD_SET(sockfd_listen, &read_fd);
                   
                   

                    int result=select( sockfd_listen+1, &read_fd, NULL, NULL,  &my_timeout);// fix parameters
                        // TODO: Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
                        // for us at the socket (we are waiting for DATA)
            
                    if (result)
                    {
                         filelen = recvfrom(sockfd_listen, &dataBuffer, 516 ,0, (struct sockaddr *)&client_addr, &addr_len);
                        if (filelen == -1) {
                            perror("TFTP_ERROR:");
                            continue;
                        }
                        dataBuffer.blockNum=ntohs(dataBuffer.blockNum);
                        dataBuffer.op=ntohs(dataBuffer.op);
                        dataFlag=1;

                
        
                    }
                    if (result==0)// if there was no data recieved for 3 seconds, send another ack
                        
                    {
                        acknowledge.op=htons(4);
                        acknowledge.blockNum=htons(ackCount);
                        if(sendto(sockfd_listen, &acknowledge, ACK_PACK_SIZE, 0,(struct sockaddr *)&client_addr , addr_len)<0){
                             perror("TFTP_ERROR:");
                            continue;
                        }
                        ACK_Print(ackCount);
                        printf("FLOWERROR: waited 3 seconds with no result\n");
                        timeoutExpiredCount++;
                    }
            
                    if (timeoutExpiredCount>= NUMBER_OF_FAILURES)// if we have waited for 7 cycles, wait no more
                    {
                        fatalError=1;
                        printf("FLOWERROR: waited 7 times with no result\n");
                        break;
                // FATAL ERROR BAIL OUT
                    }
            
                }while (!dataFlag);
            
            if(fatalError){
                break;
            }
            if (dataBuffer.op!=3) //We got something else but DATA
            {
                fatalError=1;
                printf("FLOWERROR: didn't receive data\n");
                break;
                // FATAL ERROR BAIL OUT
            }
            if (dataBuffer.blockNum != ackCount+1) // The incoming block number is not what we have
            // expected, i.e. this is a DATA pkt but the block number
            // in DATA was wrong (not last ACK’s block number + 1)
            {
                fatalError=1;
                printf("FLOWERROR: recieved wrong block num\n");
                break;
            // FATAL ERROR BAIL OUT
            }
                //data recieved
            printf("IN:DATA, %d,%d\n",dataBuffer.blockNum, filelen);
            ackCount++;

        }while (FALSE);
        
        if(fatalError){
            break;
        }
            
        timeoutExpiredCount = 0;
        size_t tempSize=(unsigned int) (filelen-4);
        size_t byte= BYTE_SIZE;
        lastWriteSize = fwrite((void*)dataBuffer.data,byte,tempSize, fp ); // write next bulk of data
            if(ferror(fp)!=0){
                perror("TFTP_ERROR:");
                printf("RECVFAIL\n");
                exit(1);
            }
            printf("WRITING: %d\n", lastWriteSize);
            
            acknowledge.op=htons(4);
            acknowledge.blockNum=htons(ackCount);
            if(sendto(sockfd_listen, &acknowledge, ACK_PACK_SIZE, 0,(struct sockaddr *)&client_addr , addr_len)<0){
                perror("TFTP_ERROR:");
                continue;
            }
             ACK_Print(ackCount);
           
            
        
    // TODO: send ACK packet to the client
    }while (lastWriteSize==MAX_SIZE_DATA); // Have blocks left to be read from client (not end of transmission)
        if(fclose(fp)==EOF){
           perror("TFTP_ERROR:");
            printf("RECVFAIL\n");
            exit(1);
        }
        if(fatalError==1){
            printf("RECVFAIL\n");
        }
        else printf("RECVOK\n");
}while(TRUE);
   
    return 0;
}



//**********************************************************************************************************************************************
// function to print the acknowledgement to screen
//**********************************************************************************************************************************************

void ACK_Print(unsigned short blocknum){
    printf("OUT:ACK,%d\n", blocknum );
}
