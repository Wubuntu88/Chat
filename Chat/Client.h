//
//  Client.h
//  Chat
//
//  Created by William Edward Gillespie on 3/26/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef Client_h
#define Client_h

#include <stdio.h>
#include "ServerToClientMessage.h"
#include "ClientToServerMessage.h"

void print_menu();
void log_in();
ServerToClientMessage* send_request(ClientToServerMessage client_to_server_message);

#endif /* Client_h */
