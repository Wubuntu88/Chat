//
//  ChatListener.c
//  Chat
//
//  Created by  on 3/29/16.
//  Copyright © 2016 EMU. All rights reserved.
//
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */

#include "ChatListener.h"

#define MAXIMUM_PENDING_REQUESTS 5

void DieWithError(char *errorMessage);  /* External error handling function */

/*
 * Initializes the chat listener by creating its socket, binding the socket, and 
 * marking it for listening.
 * Reference: TCPEchoServer.c from the TCP Sockets in C book.
 * @param: tcpPort - the tcp port that the chat listener will bind to and listen on
 * @param: chatListenerSocket - the socket that the chat listener creates and will
 * use to receive communications
 */
void initializeChatListener(int tcpPort, int *chatListenerSocket){
    struct sockaddr_in chatListenerAddress;
    
    /* Create socket for incoming connections */
    if ((*chatListenerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct address structure */
    memset(&chatListenerAddress, 0, sizeof(chatListenerAddress));   /* Zero out structure */
    chatListenerAddress.sin_family = AF_INET;                /* Internet address family */
    chatListenerAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    chatListenerAddress.sin_port = htons(tcpPort);      /* Local port */
    
    /* Bind to the local address */
    if (bind(*chatListenerSocket, (struct sockaddr *) &chatListenerAddress, sizeof(chatListenerAddress)) < 0)
        DieWithError("bind() failed");
    
    /* Mark the socket so it will listen for incoming connections */
    if (listen(*chatListenerSocket, MAXIMUM_PENDING_REQUESTS) < 0)
        DieWithError("listen() failed");
}

















