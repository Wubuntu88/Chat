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
ServerToClientMessage send_request(ClientToServerMessage client_to_server_message);
void log_in();
void log_out();


#endif /* Client_h */
