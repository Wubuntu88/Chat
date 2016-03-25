//
//  ClientToServerMessage.h
//  Chat
//
//  Created by  on 3/25/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef ClientToServerMessage_h
#define ClientToServerMessage_h

#include <stdio.h>

typedef struct {
    /* content has diferent meanings based on the enum
     Login: username
     Logout: username
     Who: nothing
     Request Chat: other user client wants to chat with
     Status: not sure yet
     */
    char *content;
    int udpPort;
    int tcpPort;
    enum {Login, Logout, Who, RequestChat, Status} requestType;
}ClientToServerMessage;

#endif /* ClientToServerMessage_h */
