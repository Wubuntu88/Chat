//
//  ClientToClientMessage.h
//  Chat
//
//  Created by  on 3/30/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef ClientToClientMessage_h
#define ClientToClientMessage_h

#include <stdio.h>
#include "Constants.h"

typedef struct {
    enum {Invite, Reject, Accept, Chat, EndOfTransmission} messageType;
    int tcpPort;
    char usernameOfSender[CLIENT_USERNAME_MAX_SIZE];
    char content[CLIENT_TO_CLIENT_MESSAGE_SIZE];
} ClientToClientMessage;

#endif /* ClientToClientMessage_h */
