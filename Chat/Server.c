//
//  Server.c
//  Chat
//
//  Created by William Edward Gillespie on 3/26/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */

#include "DieWithError.c"
#include "Server.h"
#include "ClientToServerMessage.h"
#include "ServerToClientMessage.h"
#include "ClientInfo.h"
#include "Constants.h"

int sock = 0;

int numberOfLoggedInUsers = 0;
Client users[MAX_USERS];

struct sockaddr_in server_address;
struct sockaddr_in client_address;
int server_port;

void DieWithError(char *errorMessage);  /* External error handling function */
/*
 main method of the server program
 @param: argv[1]: server's port number
 */
int main(int argc, char *argv[])
{
    //check for correct number of parameters (only 2; one from user)
    if (argc != 2) {
        char errorMessage[100];
        sprintf(errorMessage, "Usage: %s <Udp Server Port Number>", argv[0]);
    }
    
    //use parameter from user
    server_port = atoi(argv[1]);
    
    //print message to show server is working
    printf("Chat server starting up...\n");
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct local address structure */
    memset(&server_address, 0, sizeof(server_address));   /* Zero out structure */
    server_address.sin_family = AF_INET;                /* Internet address family */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    server_address.sin_port = htons(server_port);      /* Local port */
    //set up the socket
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        DieWithError("bind() failed");
    
    while(1){//loop forever and handle clients requests
        
        ClientToServerMessage *clientMessage = (ClientToServerMessage *)malloc(sizeof(ClientToServerMessage));
        
        /* Set the size of the in-out parameter */
        unsigned int clientAddressLength = sizeof(client_address);
        /* Block until receive message from a client */
        if (recvfrom(sock, &clientMessage, sizeof(clientMessage), 0,
                                    (struct sockaddr *) &client_address, &clientAddressLength) < 0)
            DieWithError("recvfrom() failed");
        
        //handle different types of requests
        if (clientMessage->requestType == Login) {
            
            ;
        } else if (clientMessage->requestType == Logout){
            ;
        } else if (clientMessage->requestType == Who){
            ;
        } else if (clientMessage->requestType == RequestChat){
            ;
        } else {
            ;
        }
    }
}

/*
 Sends a ServerToClientMessage to the client's address.  This is a method that is rather
    generic in that it sends any message to a client regardless of the information in it.
    Other methods that deal with login, logout, who, and requesChat use this method to
    sent the specific information that they want to send to the client.
 @param: sockaddr_in clientAddress - the address of the client to whom the message is sent
 */
void send_response(struct sockaddr_in clientAddress, ServerToClientMessage messageToClient){
    if (sendto(sock, &messageToClient, sizeof(messageToClient), 0,
               (struct sockaddr *) &clientAddress, sizeof(clientAddress)) < 0)
        DieWithError("sendto() sent a different number of bytes than expected");
}

void login_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage){
    //check if there are too many users to allow a new one to log in
    
    /*iterate through the users and check if the username is a duplicate
     * or if the tcp client address and port number are already in the list of users
     */
    
    //trace that a user has been logged in
    
    //initialize Client data struct; place user in array of users
    
    //send success response to client with list of users
    
}


























