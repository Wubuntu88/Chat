//
//  SockAddrHelper.c
//  Chat
//
//  Created by  on 3/28/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#include <arpa/inet.h>

/*
 * Compares the sockaddr_ins for equality
 * @return 1 if they are equal, 0 if they are not equal
 */
int sockadddr_in_are_equal(struct sockaddr_in *first, struct sockaddr_in *second){
    if (first->sin_family == second->sin_family &&
        first->sin_addr.s_addr == second->sin_addr.s_addr &&
        first->sin_port == second->sin_port) {
        return 1;
    }else{
        return 0;
    }
}

void copy_sockaddr_in(struct sockaddr_in *destination, struct sockaddr_in *source){
    destination->sin_family = source->sin_family;
    destination->sin_addr.s_addr = source->sin_addr.s_addr;
    destination->sin_port = source->sin_port;
}

