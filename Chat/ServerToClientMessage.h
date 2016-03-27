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

typedef struct {
    char *content;
    enum {Success, Failure} responseType;
} ServerToClientMessage;

#endif /* ServerToClientMessage_h */
