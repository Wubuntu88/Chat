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
#include "Constants.h"

#include "DieWithError.c"

int udpPort;
int tcpPort;
char *serverIPAddress;
unsigned int serverPort;

/*Server address struct information*/
struct sockaddr_in serverAddress;
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
        char errorMes[80];
        sprintf(errorMes, "Usage: %s <1:Local UDP Port> <2:Local TCP Port> <3:Server IP Address> <4:Server Port>", argv[0]);
        DieWithError(errorMes);
    }
    
    /* set variables that come in through parameters */
    udpPort = atoi(argv[0]);
    tcpPort = atoi(argv[1]);
    serverIPAddress = (char*)argv[2];
    serverPort = atoi(argv[3]);
    
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
    printf("Welcome to the Chat Application \\^v^/");
    print_menu();
    
    //create udp socket to send messages to the server (for login, logout, etc)
    
    
    /*
     * Fork processes for the child that accepts messages from the server
     * and for the child that accepts messages from the other chat client.
     * The parent process will send messages to the server (login, logout, etc)
     */
    
    
    while (1) {//loop forever
        break;
        //accept input from user
        
        //if statement for if the user needs to communicate with the server
        
        //else statement for if the user is chatting with another user
        
    }
    return 0;
}

void print_menu(){
    printf("Options:\n");
    printf("\t%s: shows the menu\n", HELP);
    printf("\t%s: login to the server\n", LOGIN);
    printf("\t%s: logout from server\n", LOGOUT);
    printf("\t%s: retrieve list of users online\n", WHO);
    printf("\t%s: get status of whether you are logged in or not\n", STATUS);
    printf("\t%s: invite user to chat with you\n", INVITE);
    printf("\t%s: quit the application\n", QUIT);
    printf("\tctrl-d: terminates chat session with other user\n");
}

void log_in(char *userName){
    
}














