//
//  TCPHelper.c
//  Chat
//
//  Created by  on 3/29/16.
//  Copyright © 2016 EMU. All rights reserved.
//
// This file has methods for sending and receiving tcp messages

#include <arpa/inet.h>

#include "TCPHelper.h"
#include "ClientToClientMessage.h"
void DieWithError(char *errorMessage);  /* External error handling function */


void sendTCPMessage(int socket, ClientToClientMessage clientToClientMessage){
    long messageLength = sizeof(clientToClientMessage);
    long bytesSent = send(socket, &clientToClientMessage, sizeof(clientToClientMessage), 0);
    if (bytesSent != messageLength) {
        DieWithError("message sent unexpected number of bytes; quit the program.\n");
    }
}

ClientToClientMessage receiveTCPMessage(int socket){
    ClientToClientMessage c2cMess;
    return c2cMess;
}