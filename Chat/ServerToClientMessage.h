//
//  ServerToClientMessage.h
//  Chat
//
//  Created by William Edward Gillespie on 3/26/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef ServerToClientMessage_h
#define ServerToClientMessage_h

#include <stdio.h>

#include "Constants.h"

typedef struct {
    enum {Success, Failure} responseType;
    char content[SERVER_MESSAGE_SIZE];
    int tcpPort;
    struct sockaddr_in address;
} ServerToClientMessage;

#endif /* ServerToClientMessage_h */
