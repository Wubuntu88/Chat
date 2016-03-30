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
void DieWithError(char *errorMessage);  /* External error handling function */

/*
 * Contents of message may vary depending on whether inviting, rejecting, or chatting
 * If inviting: "invite" ownUserName ownTcpPort
 * If reject: "reject" ...?
 * If chat: 
 */
void sendTCPMessage(int socket, char *message){
    //long messageLength = sizeof(clientToClientMessage);
    //long bytesSent = send(socket, &message, sizeof(clientToClientMessage), 0);
    //if (bytesSent != messageLength) {
    //    DieWithError("message sent unexpected number of bytes; quit the program.\n");
    //}
}
/*
char* receiveTCPMessage(int socket){
    
    long messageLength = sizeof(c2cMess);
    long bytesSent = recv(socket, &c2cMess, sizeof(c2cMess), 0);
    if (bytesSent != messageLength) {
        DieWithError("message received unexpected number of bytes; quit the program.\n");
    }
    return c2cMess;
}
*/