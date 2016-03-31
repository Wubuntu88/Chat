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
#include <sys/mman.h>   /* for shared memory between processes */
#include <signal.h>
#include <unistd.h>

#include "Client.h"
#include "ClientToServerMessage.h"
#include "ServerToClientMessage.h"
#include "ClientToClientMessage.h"
#include "ClientInfo.h"
#include "Constants.h"
#include "SockAddrHelper.h"
#include "TCPHelper.h"
#include "ChatListener.h"

void DieWithError(char *errorMessage);  /* External error handling function */
void send_who_request();
void send_chat_request();

void quit();
void initializeChatListener(int tcpPort, int *chatListenerSocket);
/* method for sending tcp messages to the other clients child process
 * chat messages, chat requests, and responses to invitations are sent.
 */
void sendTCPMessage(int socket, ClientToClientMessage clientToClientMessage);
void enter_listening_parallel_universe(int *chatListenerSocket, Client *chattingBuddy, int *isChatting, int *hasInvited, int *hasResponded);
void send_tcp_message_to_client(char *message, char *potentialNullPointer);

void copy_sockaddr_in(struct sockaddr_in *destination, struct sockaddr_in *source);

//what is sent to the server for login, logout, who requests,etc
int udpPort;//used by child process
int tcpPort;//used by child process
char *serverIPAddress;
unsigned int serverPort;

//keeps track of username for client so that we can send it to the server on logout
char username[CLIENT_USERNAME_MAX_SIZE];
//keeps track of wheter the user is logged in or not
//0 means the user is not logged in; 1 means they are logged in
short isLoggedIn = 0;

//socket used for sending messages to the server
int udpSocket = 0;//also for receiving acknowladgements from the server.

//child process ids
int udp_process_id;//for getting messages from the server to the child client proccess
int tcp_process_id;//for getting messages from another child process to this clients child process

//sockets that the child processes will use to accept messages from other clients and server
int chatListenerSocket;//used to accepts chat messages from other clients (TCP)
int spontaneousServerMessageListenerSocket;//used to get spontaneous (UDP)

int *outgoingTCPSocket;

//keeps information about the person the user is currently chatting with.
Client *chattingBuddy; //<---SHARED WITH CHILD PROCESS

//keeps track of whether the user is chatting or not
//0 means the user is not chatting; 1 means they are chatting
/* ALL ARE SHARED WITH CHILD PROCESS*/
int *isChatting = 0;
int *hasBeenInvited = 0;
int *hasResponded = 0;

/* Server address struct information */
struct sockaddr_in serverAddress;
/* Address of the server that sent a client this message.
 useful if an unknown source sent the client a message*/
struct sockaddr_in fromAddr;

ClientToServerMessage clientToServerMessage;

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
        sprintf(errorMessage, "Usage: %s <1:Local UDP Port> <2:Local TCP Port> <3:Server IP Address> <4:Server Port>\n", argv[0]);
        DieWithError(errorMessage);
    }
    
    /* set variables that come in through parameters */
    udpPort = atoi(argv[1]);
    tcpPort = atoi(argv[2]);
    serverIPAddress = (char*)argv[3];
    serverPort = atoi(argv[4]);
    
    /*setup the clientToServerMessage with reused variables*/
    memset(&clientToServerMessage, 0, sizeof(clientToServerMessage));
    clientToServerMessage.udpPort = udpPort;
    clientToServerMessage.tcpPort = tcpPort;

    /* Create a datagram/UDP socket */
    if ((udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    /**
     * set up variables for memory sharing amoung processes
     * with inspiration from Dr. Poh's mmap.c program
     */
    outgoingTCPSocket = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    chattingBuddy = (Client *) mmap(NULL, sizeof(Client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    isChatting = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    hasBeenInvited = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    hasResponded = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    
    printf("before init\n");
    //initialize sockets for udp listener and tcp listener
    initializeChatListener(tcpPort, &chatListenerSocket);
    printf("after init\n");
    fflush(stdout);
    //set up address for the server
    /* Construct the server address structure */
    memset(&serverAddress, 0, sizeof(serverAddress));    /* Zero out structure */
    serverAddress.sin_family = AF_INET;                 /* Internet addr family */
    serverAddress.sin_addr.s_addr = inet_addr(serverIPAddress);  /* Server IP address */
    serverAddress.sin_port   = htons(serverPort);     /* Server port */
    
    //welcome the user
    printf("Welcome to the Chat Application \\^v^/\n");
    print_menu();
    
    /*
     * Fork processes for the child that accepts messages from the server
     * and for the child that accepts messages from the other chat client.
     * The parent process will send messages to the server (login, logout, etc)
     */
    
    tcp_process_id = fork();
    if (tcp_process_id == 0) {
        enter_listening_parallel_universe(&chatListenerSocket, chattingBuddy, isChatting, hasBeenInvited, hasResponded);
    }
    
    printf("child process id: %d\n", tcp_process_id);
    
    while (1) {//loop forever
        //accept input from user
        printf("$");
        
        int messageSize = isChatting ? CLIENT_MESSAGE_SIZE : CLIENT_COMMAND_SIZE;
        
        char userInput[messageSize];
        memset(userInput, 0, sizeof(userInput));
        
        //if the user types ctrl-d, fgets will return a null pointer (i.e. 0)
        char* potentialNullPointer = fgets(userInput, (int)sizeof(userInput), stdin);
        
        //eliminates newline character when user gives input
        int len = (int)strnlen(userInput, sizeof(userInput));
        if(userInput[len - 1] == '\n'){
            userInput[len - 1] = '\0';
        }
        
        printf("parent process isChatting: %d", *isChatting);
        
        if (*isChatting || *hasBeenInvited) {
            //I must send a tcp message to the other clients child process that is waiting
            //for my chatting or response.
            //I should create a specialized method in this class to send a tcp message to that client
            //I shoud keep in mind the control-d character that the user may have used to end the chatting
            send_tcp_message_to_client(userInput, potentialNullPointer);
        } else {//client is giving a command to the server
            if(strncmp(MENU, userInput, sizeof(MENU)) == 0){
                print_menu();
            }else if (strncmp(LOGIN, userInput, sizeof(LOGIN)) == 0) {
                log_in();
            }else if (strncmp(LOGOUT, userInput, sizeof(LOGOUT)) == 0){
                log_out();
            }else if (strncmp(WHO, userInput, sizeof(WHO)) == 0){
                send_who_request();
            }else if (strncmp(STATUS, userInput, sizeof(STATUS)) == 0){
                
            }else if (strncmp(INVITE, userInput, sizeof(INVITE)) == 0){
                send_chat_request();
            }else if (strncmp(QUIT, userInput, sizeof(QUIT)) == 0){
                quit();
            }else{
                printf("invalid choice; try something else.\n");
            }
        }
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


ServerToClientMessage send_request(ClientToServerMessage client_to_server_message){
    
    ServerToClientMessage serverResponse;
    //send request to server
    if (sendto(udpSocket, &client_to_server_message, sizeof(client_to_server_message), 0, (struct sockaddr *)
               &serverAddress, sizeof(serverAddress)) != sizeof(client_to_server_message))
        DieWithError("sendto() sent a different number of bytes than expected\n unable to send login message");
    printf("sent the chat request.\n");
    fflush(stdout);
    //get response from server.  Response stored in the servResponse struct
    unsigned int fromSize = sizeof(fromAddr);
    if (recvfrom(udpSocket, &serverResponse, sizeof(serverResponse), 0,
                                  (struct sockaddr *) &fromAddr, &fromSize) != sizeof(serverResponse))
        DieWithError("recvfrom() failed");
    printf("passed recv\n");
    return serverResponse;
}

void log_in(){
    
    if (isLoggedIn) {//check if user already logged in
        printf("You are already logged in; to login as another user, logout first, then login again.\n");
    } else {//if not logged in, send message to server, get response
        //get username from user
        printf("Enter username: ");
        memset(username, 0, sizeof(username));
        fgets(username, (int)sizeof(username), stdin);//get input from user
        int len = (int)strnlen(username, sizeof(username));
        if(username[len - 1] == '\n'){//get rid of pesky newline character
            username[len - 1] = '\0';
        }
        
        clientToServerMessage.requestType = Login;
        strcpy(clientToServerMessage.content, username);
        //send request
        ServerToClientMessage servResponse = send_request(clientToServerMessage);
        //check if it was a success or failure
        if (servResponse.responseType == Success) {
            isLoggedIn = 1;
            printf("Login was successful.\nUsers Online:\n%s\n", servResponse.content);
        } else if(servResponse.responseType == Failure){
            printf("Login unsuccessful: %s\n", servResponse.content);
        }else{
            printf("unknown server response type.\n");
        }
        fflush(stdout);
    }
}

void log_out(){
    if (isLoggedIn == 0) {
        printf("You are not logged in.\n");
    }else{
        clientToServerMessage.requestType = Logout;
        strcpy(clientToServerMessage.content, username);//should be the same; just in case
        
        //send message to server; get response
        ServerToClientMessage servResponse = send_request(clientToServerMessage);
        if (servResponse.responseType == Success) {
            isLoggedIn = 0;
            printf("you successfully logged out as <%s>.\n", username);
        } else if(servResponse.responseType == Failure){
            printf("There was and error when logging out.\nServer Response: %s\n", servResponse.content);
        }else {
            printf("unidentified response from server.  Try to log out again.\n");
        }
    }
}

void send_who_request(){
    if (isLoggedIn == 0) {
        printf("You are not logged in; log in before sending \"who\" requst.\n");
    }else{
        clientToServerMessage.requestType = Who;
        strcpy(clientToServerMessage.content, username);//should be the same; just in case
        
        //send message to server; get response
        ServerToClientMessage servResponse = send_request(clientToServerMessage);
        if (servResponse.responseType == Success) {
            printf("Users logged in:\n%s", servResponse.content);
        }else if(servResponse.responseType == Failure){
            printf("Unable to process Who request; the server sent a failure signal.\n");
        }else{
            printf("Unidentified response from server;  Try the who request again.\n");
        }
    }
}

void send_chat_request(){
    if (isLoggedIn == 0) {
        printf("You are not logged in; please log in before sending a chat request.\n");
    }else {
        char friendName[CLIENT_USERNAME_MAX_SIZE];
        printf("enter username (max %d chars): ", CLIENT_USERNAME_MAX_SIZE - 1);
        fgets(friendName, CLIENT_USERNAME_MAX_SIZE, stdin);
        friendName[strlen(friendName) - 1] = '\0';
        
        clientToServerMessage.requestType = UserInfo;
        strcpy(clientToServerMessage.content, friendName);
        ServerToClientMessage servResponse = send_request(clientToServerMessage);
        
        if (servResponse.responseType == Failure) {
            printf("unable to start chat.\nServer Response: %s\n", servResponse.content);
            return;
        }
        printf("username: %s, port: %d", servResponse.content, servResponse.tcpPort);
        
        //at this point we know there was a successful response
        //the friend's address and port number will be in the content of the struct
        //the format will be: address portNumber.  e.g. 127.0.0.1 200
        struct sockaddr_in friendAddress;
        copy_sockaddr_in(&friendAddress, &servResponse.address);
        friendAddress.sin_family = AF_INET;
        friendAddress.sin_port = htons(servResponse.tcpPort);
        /**
         * I LEFT OFF HERE I was getting the friend address from the server...
         */
        
        //copy the friend address struct and username into the chattingBuddy in shared memory
        copy_sockaddr_in(&chattingBuddy->address, &friendAddress);
        strcpy(chattingBuddy->username, friendName);
        
        //create the tcp socket.  This socket is shared among processes.
        //this will allow the parent process to send messages to the other client's
        //child process that will be accepting tcp messages
        if ((*outgoingTCPSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            DieWithError("Socket creation failed.\n");
        }
        
        //connect with the other clients child process that is listening for a tcp connection.
        if (connect(*outgoingTCPSocket, (struct sockaddr *) &chattingBuddy->address, sizeof(chattingBuddy->address)) < 0)
            DieWithError("connect() failed");
        
        //set shared variables so that the child process knows to accept chat messages
        *hasBeenInvited = 1;
        
        
        //directly send invitation to the client
        //content contains username and tcp port seperated by a space
        ClientToClientMessage c2cMess;
        c2cMess.messageType = Invite;
        //the port that the parent process is listening on, the other program's child
        //process needs this to send messages to the right port.
        c2cMess.tcpPort = tcpPort;
        strcpy(c2cMess.usernameOfSender, username);
        //send the tcp message to the other client's child process
        sendTCPMessage(*outgoingTCPSocket, c2cMess);
    }
}

void respond_to_invitation(char *response){
    /*
     * If we are responding to a tcp invitation, the outgoing tcp socket has not been created yet
     * create the tcp socket.  This socket is shared among processes.
     * this will allow the parent process to send messages to the other client's
     * child process that will be accepting tcp messages
     */
    if ((*outgoingTCPSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("Socket creation failed.\n");
    }
    
    printf("&&&hey its %s: ", chattingBuddy->username);
    fflush(stdout);
    //connect with the other clients child process that is listening for a tcp connection.
    if (connect(*outgoingTCPSocket, (struct sockaddr *) &chattingBuddy->address, sizeof(chattingBuddy->address)) < 0)
        DieWithError("connect() failed");
    printf("after here\n");
    fflush(stdout);
    ClientToClientMessage c2cMess;
    memset(&c2cMess, 0, sizeof(c2cMess));
    if (strcpy(response, YES) == 0) {
        c2cMess.messageType = Accept;
        sendTCPMessage(*outgoingTCPSocket, c2cMess);
        *isChatting = 1;
    } else {
        c2cMess.messageType = Reject;
        sendTCPMessage(*outgoingTCPSocket, c2cMess);
        *isChatting = 0;
        close(*outgoingTCPSocket);
    }
}


/*
 * Kills the two child processes and quits the program
 */
void quit(){
    
    if (isLoggedIn) {
        printf("Logging you out...\n.");
        log_out();
        printf("successfully logged out.\n");
    }
    
    printf("quiting the process...killing all child processes.\n");
    
    //killing the child processes to ensure no zombie processes.
    kill(tcp_process_id, SIGKILL);
    kill(udp_process_id, SIGKILL);
    
    printf("Exiting program; Goodbye.\n");
    exit(0);
}

void send_tcp_message_to_client(char *message, char *potentialNullPointer){
    if (*hasResponded == 0) {
        printf("here here\n");
        fflush(stdout);
        respond_to_invitation(message);
    } else {
        ClientToClientMessage c2cMess;
        memset(&c2cMess, 0, sizeof(c2cMess));
        strcpy(c2cMess.usernameOfSender, username);
        if (potentialNullPointer == 0) {//if we received ctrl-d from the other client
            c2cMess.messageType = EndOfTransmission;
            *isChatting = 0;
            *hasBeenInvited = 0;
            *hasResponded = 1;
            sendTCPMessage(*outgoingTCPSocket, c2cMess);
            
            //now I must close the socket and kill the child process
            kill(tcp_process_id, SIGKILL);
            tcp_process_id = fork();
            if (tcp_process_id == 0) {//if we are in the child process
                enter_listening_parallel_universe(&chatListenerSocket, chattingBuddy, isChatting, hasBeenInvited, hasResponded);
            }
        }else { // regular chatting
            
            c2cMess.messageType = Chat;
            strcpy(c2cMess.content, message);
            sendTCPMessage(*outgoingTCPSocket, c2cMess);
        }
    }
}















