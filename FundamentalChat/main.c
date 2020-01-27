//
//  main.c
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 12/16/19.
//  Copyright © 2019 Soroush Jahanzad. All rights reserved.
//

#include <stdio.h>
#include "client.h"
#include "server.h"

// Main -------------------------------------------------------

int main(int argc, const char * argv[]) {
    char c;
    printf("Choose Mode:\n(c for client and s for server)\n");
    scanf("%c", &c);
    if(c == 'c')
        client_main();
    if(c == 's')
        server_main();
    return 0;
}
