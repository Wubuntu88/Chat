//
//  Client.c
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

#include "Client.h"
#include "ClientToServerMessage.h"
#include "ServerToClientMessage.h"
#include "Constants.h"

//#include "DieWithError.c"
void DieWithError(char *errorMessage);  /* External error handling function */

int udpPort;
int tcpPort;
char *serverIPAddress;
unsigned int serverPort;

//socket used for sending messages to the server
int udpSocket = 0;

//keeps track of whether the user is chatting or not
//0 means the user is not chatting; 1 means they are chatting
short isChatting = 0;

//keeps track of wheter the user is logged in or not
//0 means the user is not logged in; 1 means they are logged in
short isLoggedIn = 0;

/* Server address struct information */
struct sockaddr_in serverAddress;
/* Address of the server that sent a client this message.
 useful if an unknown source sent the client a message*/
struct sockaddr_in fromAddr;

/*
 Start of client program
 @param argv[1] local port for udp
 -used by the upd listener that accepts messages from the server.
 The server will send the client a message containing the user ids
 of other clients that have logged in to the chat application.
 -used to create the socket.  A forked child process will use that
 that socket to accept messages from the server
 @param argv[2] local port for tcp
 -used by the tcp listener that accepts chat messages from other clients.
 -used to create a socket.  The socket will be used by a child process to
 accept messages from other clients
 @param argv[3] server address: the IP address of the server
 @param argv[4] server port: the port of the server
 */
int main(int argc, const char * argv[]) {
    /* check number of parameters (should be 5) */
    if (argc != 5) {
        char errorMessage[100];
        sprintf(errorMessage, "Usage: %s <1:Local UDP Port> <2:Local TCP Port> <3:Server IP Address> <4:Server Port>", argv[0]);
        DieWithError(errorMessage);
    }
    
    /* set variables that come in through parameters */
    udpPort = atoi(argv[0]);
    tcpPort = atoi(argv[1]);
    serverIPAddress = (char*)argv[2];
    serverPort = atoi(argv[3]);
    
    /* Create a datagram/UDP socket */
    if ((udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    /**
     * set up variables for memory sharing amoung processes
     * with inspiration from Dr. Poh's mmap.c program
     */
    //TODO
    
    //initialize sockets for udp listener and tcp listener
    //TODO
    
    //set up address for the server
    /* Construct the server address structure */
    memset(&serverAddress, 0, sizeof(serverAddress));    /* Zero out structure */
    serverAddress.sin_family = AF_INET;                 /* Internet addr family */
    serverAddress.sin_addr.s_addr = inet_addr(serverIPAddress);  /* Server IP address */
    serverAddress.sin_port   = htons(serverPort);     /* Server port */
    
    
    
    //welcome the user
    printf("Welcome to the Chat Application \\^v^/\n");
    print_menu();
    
    
    
    //create udp socket to send messages to the server (for login, logout, etc)
    
    
    /*
     * Fork processes for the child that accepts messages from the server
     * and for the child that accepts messages from the other chat client.
     * The parent process will send messages to the server (login, logout, etc)
     */
    
    
    while (1) {//loop forever
        //accept input from user
        printf(">>");
        
        int messageSize = isChatting ? CLIENT_MESSAGE_SIZE : CLIENT_COMMAND_SIZE;
        
        char userInput[messageSize];
        memset(userInput, 0, sizeof(userInput));
        fgets(userInput, (int)sizeof(userInput), stdin);//get input from user
        
        //eliminates newline character when user gives input
        int len = (int)strnlen(userInput, sizeof(userInput));
        if(userInput[len - 1] == '\n'){
            userInput[len - 1] = '\0';
        }
        
        if (isChatting) {
            //chat
        } else {//client is giving a command to the server
            if(strncmp(MENU, userInput, sizeof(MENU)) == 0){
                print_menu();
            }else if (strncmp(LOGIN, userInput, sizeof(LOGIN)) == 0) {
                log_in();
            }else if (strncmp(LOGOUT, userInput, sizeof(LOGOUT)) == 0){
                
            }else if (strncmp(WHO, userInput, sizeof(WHO)) == 0){
                
            }else if (strncmp(STATUS, userInput, sizeof(STATUS)) == 0){
                
            }else if (strncmp(INVITE, userInput, sizeof(INVITE)) == 0){
                
            }else if (strncmp(QUIT, userInput, sizeof(QUIT)) == 0){
                
            }
        }
        
        //REMEMBER TO DELETE THE BREAK!!!!
        break;
        
        
        //if statement for if the user needs to communicate with the server
        
        //else statement for if the user is chatting with another user
        
    }
    return 0;
}

void print_menu(){
    printf("Options:\n");
    printf("\t%s: shows the menu\n", MENU);
    printf("\t%s: login to the server\n", LOGIN);
    printf("\t%s: logout from server\n", LOGOUT);
    printf("\t%s: retrieve list of users online\n", WHO);
    printf("\t%s: get status of whether you are logged in or not\n", STATUS);
    printf("\t%s: invite user to chat with you\n", INVITE);
    printf("\t%s: quit the application\n", QUIT);
    printf("\tctrl-d: terminates chat session with other user\n");
}

void log_in(){
    
    if (isLoggedIn) {//check if user already logged in
        printf("You are already logged in; to login as another user, logout first, then login again.");
    } else {//if not logged in, send message to server, get response
        //get username from user
        char username[CLIENT_USERNAME_MAX_SIZE];
        int len = (int)strnlen(username, sizeof(username));
        if(username[len - 1] == '\n'){//get rid of newline
            username[len - 1] = '\0';
        }
        //send request
        ClientToServerMessage message;
        message.udpPort = udpPort;
        message.tcpPort = tcpPort;
        message.requestType = Login;
        message.content = username;
        
        //send the message
        if (sendto(udpSocket, &message, sizeof(message), 0, (struct sockaddr *)
                   &serverAddress, sizeof(serverAddress)) != sizeof(message)){
            DieWithError("sendto() failed; unable to send login message");
        }
        
        ServerToClientMessage *servResponse = send_request(&message);
        //check if it was a success or failure
        if (servResponse->responseType == Success) {
            isLoggedIn = 1;
            printf("Login was successful.\nUsers Online:\n%s", servResponse->content);
        } else if(servResponse->responseType == Failure){
            printf("Login unsuccessful: %s", servResponse->content);
        }else{
            printf("unknown server response type.");
        }
        
    }
}

ServerToClientMessage *send_request(ClientToServerMessage *client_to_server_message){
    
    ServerToClientMessage *serverResponse = (ServerToClientMessage *)malloc(sizeof(ServerToClientMessage));
    
    //send request to server
    if (sendto(udpSocket, &client_to_server_message, sizeof(client_to_server_message), 0, (struct sockaddr *)
               &serverAddress, sizeof(serverAddress)) != sizeof(client_to_server_message))
        DieWithError("sendto() sent a different number of bytes than expected");
    
    //get response from server.  Response stored in the servResponse struct
    unsigned int fromAddrSize = sizeof(fromAddr);
    if (recvfrom(udpSocket, &serverResponse, sizeof(serverResponse), 0,
                                  (struct sockaddr *) &fromAddr, &fromAddrSize) != sizeof(ServerToClientMessage))
        DieWithError("recvfrom() failed");
    
    return serverResponse;
}




















