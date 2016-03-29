//
//  ClientToClientMessage.h
//  Chat
//
//  Created by  on 3/29/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef ClientToClientMessage_h
#define ClientToClientMessage_h

#include <stdio.h>

#include "Constants.h"

typedef struct {
    enum {Success, Failure} responseType;
    char content[CLIENT_TO_CLIENT_MESSAGE_SIZE];
} ClientToClientMessage;

#endif /* ClientToClientMessage_h */
