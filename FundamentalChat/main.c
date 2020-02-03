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
    char state = '0';
    printf("Choose Mode:\n(c for client, s for server and e for exit)\n");
    
    while(state != 'e')
    {
        switch (state) {
            case 'c':
                client_main();
                state = 'e';
                break;

            case 's':
                server_main();
                state = 'e';
                break;

            default:
                break;
        }
        if(state != 'e')
            scanf("%c", &state);
    }
    return 0;
}
