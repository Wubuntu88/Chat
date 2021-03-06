//
//  Constants.h
//  Chat
//
//  This file contains some constants used throughout the programs
//
//  Created by William Edward Gillespie on 3/26/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef Constants_h
#define Constants_h
/* constant commands a client can issue */
#define MENU "menu"
#define LOGIN "login"
#define LOGOUT "logout"
#define WHO "who"
#define INVITE "invite"
#define QUIT "quit"

/* constants for accepting or rejecting chat requests*/
#define YES "yes"
#define NO "no"

/* Sizes of types of messages the client can send */
#define CLIENT_COMMAND_SIZE 20
#define CLIENT_MESSAGE_SIZE 500
#define CLIENT_USERNAME_MAX_SIZE 10

#define SERVER_MESSAGE_SIZE 600

//Message size for client to client message
#define CLIENT_TO_CLIENT_MESSAGE_SIZE 500

//size is 16: 127:000:000:001 + null terminator
#define SIZE_OF_ADDRESS 16

/* max number of users that a server can handle */
#define MAX_USERS 25

#endif /* Constants_h */






























