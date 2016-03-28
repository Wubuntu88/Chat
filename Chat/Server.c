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
/* function used to log the user in.  If successful, new Client is added to users array*/
void login_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage);
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
    printf("before bind; after socket\n");
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        DieWithError("bind() failed");
    printf("after bind.\n");
    while(1){//loop forever and handle clients requests
        
        //ClientToServerMessage *clientMessage = (ClientToServerMessage *)malloc(sizeof(ClientToServerMessage));
        //clientMessage->content = (char*)malloc(CLIENT_COMMAND_SIZE);
        ClientToServerMessage clientMessage;
        //clientMessage.content = (char*)malloc(CLIENT_COMMAND_SIZE);
        
        memset(&clientMessage, 0, sizeof(clientMessage));
        printf("content:%s\n", clientMessage.content);
        /* Set the size of the in-out parameter */
        unsigned int clientAddressLength = sizeof(client_address);
        /* Block until receive message from a client */
        printf("before recv");
        fflush(stdout);
        if (recvfrom(sock, &clientMessage, sizeof(clientMessage), 0,
                                    (struct sockaddr *) &client_address, &clientAddressLength) < 0)
            DieWithError("recvfrom() failed");
        //fprintf("size of struct: %d", sizeof(clientMessage));
        fflush(stdout);
        printf("content:%s\n", clientMessage.content);
        fflush(stdout);
        //handle different types of requests
        if (clientMessage.requestType == Login) {
            printf("was login\n");
            fflush(stdout);
            login_user(client_address, clientMessage);
        } else if (clientMessage.requestType == Logout){
            ;
        } else if (clientMessage.requestType == Who){
            ;
        } else if (clientMessage.requestType == RequestChat){
            ;
        } else {
            fprintf(stderr, "undefined\n");
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
/*
 * Logs in the user by inserting the user in the array of logged in users.
 *  Sends a confirmation to the user if the login was successful or a failure if not.
 * @param clientAddress: the client address to whom the confirmation will be sent.
 * @param clientMessage: contains the username used to log in the client and upd/tcp port number
 */
void login_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage){
    ServerToClientMessage serverToClientMessage;
    
    //check if there are too many users to allow a new one to log in
    if (numberOfLoggedInUsers == MAX_USERS) {
        char message[] = "The server cannot handle more users; try again later.";
        strcpy(serverToClientMessage.content, message);
        serverToClientMessage.responseType = Failure;
        send_response(clientAddress, serverToClientMessage);
        return;
    }
    
    /*
     * Iterate through the users and check if the username is a duplicate
     * or if the tcp client address and port number are already in the list of users
     */
    //the pointer to the client being examined in the array of users
    Client *currentClient;
    //the exclusive end address of the "last" client in the array of users
    Client *exclusiveEnd = users + numberOfLoggedInUsers;
    for (currentClient = users; currentClient < exclusiveEnd; currentClient++) {
        //check of identical username
        if (strncmp(clientMessage.content, currentClient->username, sizeof(currentClient->username)) == 0) {//if that username is taken, send a rejection.
            char message[] = "That username is already taken; choose a different one.";
            strcpy(serverToClientMessage.content, message);
            serverToClientMessage.responseType = Failure;
            send_response(clientAddress, serverToClientMessage);
            return;
        }
    }
    
    //trace that a user has been logged in
    printf("%s has logged in\n", clientMessage.content);
    
    
    //initialize Client data struct; place user in array of users
    strcpy(currentClient->username, clientMessage.content);
    currentClient->address = clientAddress;
    currentClient->udpPort = clientMessage.udpPort;
    currentClient->tcpPort = clientMessage.tcpPort;
    
    //send success response to client with list of users
    sprintf(serverToClientMessage.content, "You successfully logged in as <%s>", currentClient->username);
    serverToClientMessage.responseType = Success;
    printf("after");
    send_response(clientAddress, serverToClientMessage);
    return;
}


























