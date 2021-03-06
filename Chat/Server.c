//
//  Server.c
//  Chat
//
//  This server accepts logins and requests from clients.
//  It can log users in and out, it can respond to who requests,
//  and it can send many users a list of all users when a a new user logs in.
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
#include "SockAddrHelper.h"

int sock = 0;

int numberOfLoggedInUsers = 0;
Client users[MAX_USERS];

struct sockaddr_in server_address;
struct sockaddr_in client_address;
int server_port;

//reused for every message sent to a client.  Data changed every time.
ServerToClientMessage serverToClientMessage;

void DieWithError(char *errorMessage);  /* External error handling function */
/* function used to log the user in.  If successful, new Client is added to users array*/
int login_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage);
int logout_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage);
void sendUserListToUser(struct sockaddr_in clientAddress);
char* userListString();
void copyClient(Client *destination, Client *source);
void handleUserInfoRequest(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage);
void copy_sockaddr_in(struct sockaddr_in *destination, struct sockaddr_in *source);
void broadcastUserListToAllExcept(char* username);
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
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        DieWithError("bind() failed");
    
    printf("Setup complete; ready to go!\n");
    
    while(1){//loop forever and handle clients requests
        ClientToServerMessage clientMessage;
        
        memset(&clientMessage, 0, sizeof(clientMessage));
        /* Set the size of the in-out parameter */
        unsigned int clientAddressLength = sizeof(client_address);
        /* Block until receive message from a client */
        if (recvfrom(sock, &clientMessage, sizeof(clientMessage), 0,
                                    (struct sockaddr *) &client_address, &clientAddressLength) < 0)
            DieWithError("recvfrom() failed");
        //handle different types of requests
        if (clientMessage.requestType == Login) {
            int success = login_user(client_address, clientMessage);
            if (success) {
                broadcastUserListToAllExcept(clientMessage.content);
            }
        } else if (clientMessage.requestType == Logout){
            logout_user(client_address, clientMessage);
        } else if (clientMessage.requestType == Who){
            printf("-%s requested a list of users logged in.\n", clientMessage.content);
            sendUserListToUser(client_address);
        } else if (clientMessage.requestType == UserInfo){
            printf("handling getInfo\n");
            fflush(stdout);
            handleUserInfoRequest(client_address, clientMessage);
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
 * @param 1 if login was successful; 0 if login was unsuccessful.
 */
int login_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage){
    //check if there are too many users to allow a new one to log in
    if (numberOfLoggedInUsers == MAX_USERS) {
        char message[] = "The server cannot handle more users; try again later.";
        strcpy(serverToClientMessage.content, message);
        serverToClientMessage.responseType = Failure;
        send_response(clientAddress, serverToClientMessage);
        return 0;
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
            return 0;
        }
    }
    
    //trace that a user has been logged in
    printf("-%s has logged in\n", clientMessage.content);
    
    //initialize Client data struct; place user in array of users
    strcpy(currentClient->username, clientMessage.content);
    
    copy_sockaddr_in(&currentClient->address, &clientAddress);
    //currentClient->address = clientAddress;
    currentClient->udpPort = clientMessage.udpPort;
    currentClient->tcpPort = clientMessage.tcpPort;
    //increment the number of logged in users
    numberOfLoggedInUsers++;
    
    //send success response to client with list of users
    char* userList = userListString();
    char list[500];
    strcpy(list, userList);
    list[strlen(list) - 1] = 0;
    
    sprintf(serverToClientMessage.content, "You successfully logged in as <%s>\nUsers Online:\n%s", currentClient->username, list);
    serverToClientMessage.responseType = Success;
    send_response(clientAddress, serverToClientMessage);
    return 1;
}

/*
 * Logs out a user.  The user is removed from an array of users.
 * When the user is removed, a confirmation is sent back to the client.
 * If the user could not be logged out because they were not found, a message to that
 * effect is sent back to the user with a "Failure" responseType.  However, this should
 * never happen because on the client side, the user cannot send a logout request when
 * that name is not already stored on the server; this check is done just in case.
 * @param: struct sockaddr_in clientAddress: the address to send the confirmation to.
 * @param: ClientToServerMessage: used to logout the user.  Note: the valuable username
 * is stored in the clientMessage.content variable.
 */
int logout_user(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage){
    //must find the user in the list of users by iterating through list of users
    Client *currentClient;
    Client *exclusiveEnd = users + numberOfLoggedInUsers;
    for (currentClient = users; currentClient < exclusiveEnd; currentClient++) {
        if (strncmp(currentClient->username, clientMessage.content, sizeof(currentClient->username)) == 0) {
            break;//we found the current client
        }
    }
    
    if (currentClient == exclusiveEnd) {//the username was not found
        char message[] = "That username was not found in list of users.";
        strcpy(serverToClientMessage.content, message);
        serverToClientMessage.responseType = Failure;
        send_response(clientAddress, serverToClientMessage);
        return 0;
    }else{//the username was found
        //now I must delete that user if they were found.
        for (; currentClient < exclusiveEnd - 1; currentClient++) {
            copyClient(currentClient, currentClient + 1);
        }
        numberOfLoggedInUsers--;//decrement number of logged in users
        
        printf("%s logged out.\n", clientMessage.content);
        char message[] = "Logout successful";
        strcpy(clientMessage.content, message);
        serverToClientMessage.responseType = Success;
        send_response(clientAddress, serverToClientMessage);
        return 1;
    }
}

/*
 * This method sends a string containg the list of logged in usernames to the client at 
 * the clientAddress.
 * @param: struct sockaddr_in clientAddress: the address to send the userlist to.
 */
void sendUserListToUser(struct sockaddr_in clientAddress){
    char* userList = userListString();
    strcpy(serverToClientMessage.content, userList);
    serverToClientMessage.content[SERVER_MESSAGE_SIZE - 1] = 0;
    serverToClientMessage.responseType = Success;
    send_response(clientAddress, serverToClientMessage);
}

/*
 * Returns a list of usernames of the users that are currently logged in.
 * Each name is seperated by a newline character.
 * @return char*: The list of usernames of users that are logged in.
 */
char* userListString(){
    char message[SERVER_MESSAGE_SIZE];
    memset(message, 0, sizeof(message));
    char *ptr = message;
    
    Client *current;
    Client *exclusiveEnd = users + numberOfLoggedInUsers;
    
    for (current = users; current < exclusiveEnd; current++) {
        char username[CLIENT_USERNAME_MAX_SIZE + 1];
        sprintf(username, "%s\n", current->username);
        int len = (int)strlen(username);
        strcat(ptr, username);
        ptr += len;
    }
    ptr = 0;//null terminate the message
    char *retVal = message;
    return retVal;
}

/*
 * Sends a message to the requesting user containing the tcp port number and address
 * of the desired user.  If the user was not found, then a failure is sent back to the requesting user.
 * @param: struct sockaddr_in clientAddress: the address to send the confirmation to.
 * @param: ClientToServerMessage: used to get the information on who to retrieve.
 *          The desired username is located in the clientMessage.content variable.
 */
void handleUserInfoRequest(struct sockaddr_in clientAddress, ClientToServerMessage clientMessage){
    Client *currentClient;
    Client *exclusiveEnd = users + numberOfLoggedInUsers;
    int userWasFound = 0;
    for (currentClient = users; currentClient < exclusiveEnd; currentClient++) {
        if (strncmp(currentClient->username, clientMessage.content, sizeof(currentClient->username)) == 0) {
            userWasFound = 1;
            break;//we found the current client
        }
    }
    ServerToClientMessage serv2cMess;
    if (userWasFound) {
        serv2cMess.responseType = Success;
        serv2cMess.tcpPort = currentClient->tcpPort;
        strcpy(serv2cMess.content, currentClient->username);
        copy_sockaddr_in(&serv2cMess.address, &currentClient->address);
        send_response(clientAddress, serv2cMess);
    } else {
        serv2cMess.responseType = Failure;
        char message[100];
        sprintf(message, "%s was not found in the list of users.", clientMessage.content);
        strcpy(serv2cMess.content, message);
        send_response(clientAddress, serv2cMess);
    }
    
}

/*
 * This method broadcasts the userlist to all logged in users, except the one in the input
 * This is because the user that logged in should be exempt from this message because they
 * receive a list of users with the acknowledgement of being logged in.
 * @param: char* username - the name of the user who will not be sent the list of users.
 */
void broadcastUserListToAllExcept(char* username){
    char* userList = userListString();
    
    Client *current;
    Client *exclusiveEnd = users + numberOfLoggedInUsers;
    
    for (current = users; current < exclusiveEnd; current++) {
        if (strcmp(current->username, username) != 0) {//if it is not the exempt user...
            struct sockaddr_in addressToSendTo;
            ServerToClientMessage serv2cMess;
            strcpy(serv2cMess.content, userList);
            serv2cMess.responseType = Success;
            copy_sockaddr_in(&addressToSendTo, &current->address);
            addressToSendTo.sin_port = htons(current->udpPort);
            send_response(addressToSendTo, serv2cMess);
        }
    }
}