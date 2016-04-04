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

/*
 * Sends any message using tcp with the socket and c2cMessage
 * @param: int socket
 * @param: ClientToClientMessage
 */
void sendTCPMessage(int socket, ClientToClientMessage clientToClientMessage){
    long messageLength = sizeof(clientToClientMessage);
    long bytesSent = send(socket, &clientToClientMessage, sizeof(clientToClientMessage), 0);
    if (bytesSent != messageLength) {
        DieWithError("message sent unexpected number of bytes; quit the program.\n");
    }
}

/*
 * Receives a tcp message using the specified socket.
 * @param: int socket
 * @return: ClientToClientMessage
 */
ClientToClientMessage receiveTCPMessage(int socket){
    ClientToClientMessage c2cMess;
    long messageLength = sizeof(c2cMess);
    long totalBytesReceived = 0;
    while (totalBytesReceived < messageLength) {
        long bytesReceived = recv(socket, &c2cMess + totalBytesReceived, sizeof(c2cMess), 0);
        totalBytesReceived += bytesReceived;
    }
    return c2cMess;
}