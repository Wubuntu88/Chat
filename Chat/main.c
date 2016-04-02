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
    
    while (1) {
        char userInput[10];
        memset(userInput, 0, sizeof(userInput));
        
        //if the user types ctrl-d, fgets will return a null pointer (i.e. 0)
        printf("enter something: ");
        char *potentialNullPointer = fgets(userInput, (int)sizeof(userInput), stdin);
        if (potentialNullPointer == 0) {
            printf("hey\n");
            fflush(stdout);
            clearerr(stdin);
        }
        //eliminates newline character when user gives input
        int len = (int)strnlen(userInput, sizeof(userInput));
        if(userInput[len - 1] == '\n'){
            userInput[len - 1] = '\0';
        }
        if (strcmp(userInput, "quit") == 0) {
            break;
        }
    }
    printf("exited\n");
    /*
    char str[20];
    printf("enter something: ");
    char* ptr = fgets(str, sizeof(str), stdin);
    if (ptr == 0) {
        printf("null pointer\n");
    }
    
    printf("len: %lu", strlen(str));
     */
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
