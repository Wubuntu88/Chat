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
typedef struct {
    char username[20];
    struct sockaddr_in address;
    int tcpPort;
    int udpPort;
} Client;

void moveClient(Client *destination, Client *source);

#endif /* ClientInfo_h */
