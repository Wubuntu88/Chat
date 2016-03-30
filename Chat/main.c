//
//  main.c
//  Chat
//
//  Created by  on 3/25/16.
//  Copyright © 2016 EMU. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    char str[20];
    printf("enter something: ");
    char* ptr = fgets(str, sizeof(str), stdin);
    if (ptr == 0) {
        printf("null pointer\n");
    }
    
    printf("len: %lu", strlen(str));
    /*
    int makeMillis = 1000;
    int numberMillis = 300;
    int n = makeMillis * numberMillis;
    for (int i = 1; i<=10; i++)
    {
        usleep(n);
        printf("Hello, World!\n");
        fflush(stdout);
    }
     */
    return 0;
}
