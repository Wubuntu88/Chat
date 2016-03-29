//
//  Helper.h
//  Chat
//
//  Created by  on 3/28/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#ifndef Helper_h
#define Helper_h

#include <stdio.h>
#include <arpa/inet.h>

void copy_sockaddr_in(struct sockaddr_in *destination, struct sockaddr_in *source);
int sockadddr_in_are_equal(struct sockaddr_in *first, struct sockaddr_in *second);
#endif /* Helper_h */
