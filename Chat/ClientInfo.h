//
//  ClientInfo.h
//  Chat
//
//  Created by William Edward Gillespie on 3/25/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef ClientInfo_h
#define ClientInfo_h

#include <stdio.h>
#include <arpa/inet.h>
/*
 * Stores basic information about a client
 */
typedef struct {
    char username[20];
    struct sockaddr_in address;
    int tcpPort;
    int udpPort;
} Client;

void copyClient(Client *destination, Client *source);

#endif /* ClientInfo_h */
