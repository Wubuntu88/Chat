//
//  ChatListener.c
//  Chat
//
//  This class is meant for the child process created by the Client.c program.
//  The child process will be initialized with the initializeChatListener() method.
//  The child process will be stuck in the enter_listening_parallel_universe() method
//  after the child process is forked.
//
//  Created by  on 3/29/16.
//  Copyright © 2016 EMU. All rights reserved.
//
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>

#include "ChatListener.h"
#include "ClientInfo.h"
#include "ClientToClientMessage.c"
#include "TCPHelper.h"
#include "SockAddrHelper.h"

#define MAXIMUM_PENDING_REQUESTS 5

void DieWithError(char *errorMessage);  /* External error handling function */
ClientToClientMessage receiveTCPMessage(int socket);
void awaitResponse(int *friendSocket, Client *chattingBuddy, int *isChatting, int *outstandingInvite, int *hasResponded);
void receiveInvitation(int *friendSocket, struct sockaddr_in *friendAddress, Client *chattingBuddy, int *isChatting, int *outstandingInvite, int *hasResponded);

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

/*
 * This method is for a child process of the Client.c file.  When a child process is forked, 
 * the child will enter this method (a.k.a. "parallel universe").  It will stay here and listen
 * for incomming tcp connections.  The parent process of another client will initiate those messages.
 * All of the input parameters are in shared memory with the Client.c parent process.
 * @param: int *chatListenerSocket
 * @param: Client *chattingBuddy
 * @param: int *isChatting
 * @param: int *outstandingInvite
 * @param: int *hasResponded
 * @param: int *friendSuddentlyTerminatedChat
 */
void enter_listening_parallel_universe(int *chatListenerSocket, Client *chattingBuddy, int *isChatting, int *outstandingInvite, int *hasResponded, int *friendSuddentlyTerminatedChat){
    //create local variables for friend socket, friend address, and len(friend address)
    int friendSocket;
    struct sockaddr_in friendAddress;
    unsigned int friendAddrLength = sizeof(friendAddress);
    
    while (1) {
        /*
         * This loop accepts connections from other clients that initiate a chat.
         * The child process' acceptance will correspond to when the other clients parent process
         * attempts a connection to this child process
         */
        friendSocket = accept(*chatListenerSocket, (struct sockaddr *) &friendAddress, &friendAddrLength);
        if (friendSocket < 0) {
            DieWithError("Client's child process unable to accept tcp connection.\n");
        }

        /*
         * If this client invited the other client; we wait for a response
         */
        if (*outstandingInvite) {
            awaitResponse(&friendSocket, chattingBuddy, isChatting, outstandingInvite, hasResponded);
        }
        /*
         * If this client was invited to chat; we respond to the invitation.
         * Note that the response to the invitation will be in the parent process,
         * so the process that executes this code will have to block until the variable
         * in shared memory (has responded) is updated by the parent process.  Only then
         * will we know if the user of this client responded with acceptance or rejection.
         */
        else { // this means we were invited; must wait for parent process to accept.
            //printf("responding to invitation.\n");
            receiveInvitation(&friendSocket, &friendAddress, chattingBuddy, isChatting, outstandingInvite, hasResponded);
            /* now I must block until the parent process accepts.  When the parent process switches
             * the *hasResponed shared variable to 1, then that means it has responded to the invitation.
             */
            while (*hasResponded == 0) {
                usleep(1000);//sleep for a millisecond
            }
            *hasResponded = 0;
        }
        
        /*
         * If we have gotten to this point, that means that out request has been rejected,
         * and we will do a continue and wait for another connection,
         * or we have accepted a request or the other person has accepted our request.
         */
        if (*isChatting == 0) {//if we are not chatting
            continue;
        }else{//this means that we are chatting
            //do a while loop to accept chat messages from the other user.
            while (1) {
                ClientToClientMessage c2cMess = receiveTCPMessage(friendSocket);
                if (c2cMess.messageType == EndOfTransmission) {
                    printf("Your friend has ended communication.\n");
                    fflush(stdout);
                    *isChatting = 0;
                    *outstandingInvite = 0;
                    *hasResponded = 0;
                    *friendSuddentlyTerminatedChat = 1;
                    break;
                }else{
                    fprintf(stdout, "%s: %s\n", c2cMess.usernameOfSender, c2cMess.content);
                }
            }
        }
    }
}

/*
 * This method is called to wait for a response to a chat request.  It blocks until it receives a response.
 * If the response is yes, it sets *isChatting to 1.
 * If the response is no, it sets *isChatting, *hasInvited, and *hasResponded all to 0;
 * @param int *friendSocket: this is the tcp socket of the friend child process that is accepting tcp messages
 * @param Client *chattingBuddy.  The client with whom the chat session is being initiated.
 * @param: int *isChatting
 * @param: int *outstandingInvite
 * @param: int *hasResponded
 */
void awaitResponse(int *friendSocket, Client *chattingBuddy, int *isChatting, int *outstandingInvite, int *hasResponded){
    //get the response by calling receiveTCPMessage(int socket)
    ClientToClientMessage c2cMess = receiveTCPMessage(*friendSocket);
    
    //check whether the other client accepted or declined the request
    if (c2cMess.messageType == Accept) {
        printf("%s accepted your chat request.\n", c2cMess.usernameOfSender);
        *isChatting = 1;
    } else if (c2cMess.messageType == Reject){
        printf("%s declined your chat request.\n", c2cMess.usernameOfSender);
        *isChatting = 0;
        *outstandingInvite = 0;
        *hasResponded = 0;
        //close(*friendSocket);
    }else {
        printf("Unexpected response from server; canceled chat request.\n");
        *isChatting = 0;
        *outstandingInvite = 0;
        *hasResponded = 0;
        //close(*friendSocket);
    }
}

/*
 * Sets the chattingBuddy's address and prompts the user if they want to accept or decline the invite.
 * note that the accepting is done in the parent process.  The child process gets the request and must
 * print out the prompt, but the parent process allows the user to respond.  The parent process will
 * be waiting for the user to give input.
 * @param: int *chatListenerSocket
 * @param: Client *chattingBuddy
 * @param: int *isChatting
 * @param: int *outstandingInvite
 * @param: int *hasResponded
 * @param: int *friendSuddentlyTerminatedChat
 */
void receiveInvitation(int *friendSocket, struct sockaddr_in *friendAddress, Client *chattingBuddy, int *isChatting, int *outstandingInvite, int *hasResponded){
    ClientToClientMessage c2cMess = receiveTCPMessage(*friendSocket);
    *outstandingInvite = 1;
    printf("%s requested to chat with you.\n", chattingBuddy->username);
    printf("Do you accept or decline? (yes/no): ");
    fflush(stdout);
    
    copy_sockaddr_in(&chattingBuddy->address, friendAddress);
    chattingBuddy->address.sin_family = AF_INET;
    chattingBuddy->address.sin_port = htons(c2cMess.tcpPort);
    strcpy(chattingBuddy->username, c2cMess.usernameOfSender);
}