//
//  Client.c
//  Chat
//
//  Created by William Edward Gillespie on 3/26/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#include <stdio.h>
#include "Client.h"
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
    
    /* set variables that come in through parameters */
    
    /**
     * set up variables for memory sharing amoung processes
     * with inspiration from Dr. Poh's mmap.c program
     */
    
    //initialize sockets for udp listener and tcp listener
    
    //set up address for the server
    
    //welcome the user
    
    //create udp socket to send messages to the server (for login, logout, etc)
    
    /*
     * Fork processes for the child that accepts messages from the server
     * and for the child that accepts messages from the other chat client.
     * The parent process will send messages to the server (login, logout, etc)
     */
    
    
    while (1) {//loop forever
        //do stuff
    }
    return 0;
}
















