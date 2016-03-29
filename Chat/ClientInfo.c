//
//  ClientInfo.c
//  Chat
//
//  Created by William Edward Gillespie on 3/25/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#include "ClientInfo.h"
#include "SockAddrHelper.h"
#include <string.h>

void copyClient(Client *destination, Client *source){
    strcpy(destination->username, source->username);
    copy_sockaddr_in(&destination->address, &source->address);
    destination->tcpPort = source->tcpPort;
    destination->udpPort = source->udpPort;
}
