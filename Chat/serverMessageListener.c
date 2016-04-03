//
//  serverMessageListener.c
//  Chat
//
//  Created by William Edward Gillespie on 4/2/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "serverMessageListener.h"
#include "ServerToClientMessage.h"

void DieWithError(char *errorMessage);  /* External error handling function */

void initializeServerMessageListener(int portNumber, int *listenerSocket){
    struct sockaddr_in listenerAddress;
    
    //initialize address
    memset(&listenerAddress, 0, sizeof(listenerAddress));
    listenerAddress.sin_family = AF_INET;
    listenerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    listenerAddress.sin_port = htons(portNumber);
    
    //setup socket
    *listenerSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*listenerSocket < 0) {
        DieWithError("server message listener socket creation failed.\n");
    }
    
    if (bind(*listenerSocket, (struct sockaddr *) &listenerAddress, sizeof(listenerAddress))) {
        DieWithError("bind() failed");
    }
}

void enterListenForServerMessagesParallelUniverse(int *listenerSocket){
    struct sockaddr_in serverAddress;
    unsigned int serverAddressLength = sizeof(serverAddress);
    
    while (1) {
        ServerToClientMessage serv2cMess;
        
        if (recvfrom(*listenerSocket, &serv2cMess, sizeof(serv2cMess), 0, (struct sockaddr *)&serverAddress, &serverAddressLength) < 0) {
            DieWithError("recvfrom() failed when recieving a message from the server.\n");
        }
        if (serv2cMess.responseType == Success) {
            printf("updated userlist: \n%s", serv2cMess.content);
        }else{
            printf("received failure message from server.\n");
        }
        
    }
}



















