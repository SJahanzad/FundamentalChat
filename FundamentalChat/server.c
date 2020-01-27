//
//  server.c
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 1/27/20.
//  Copyright © 2020 Soroush Jahanzad. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "server.h"

// Constants --------------------------------------------------

const int serv_port = 12345;

// Globals ----------------------------------------------------

int serv_sock, client_sock;
struct sockaddr_in serv_socket_address;
int option = 1;

// Main -------------------------------------------------------

int server_main()
{
    server_init();
    
    int serv_sockaddr_len = sizeof(serv_socket_address);
    string client_message;
    
    while (1) {
        memset(client_message, 0, sizeof(string));
        
        client_sock = accept(serv_sock, (struct sockaddr *) &serv_socket_address, (socklen_t *) &serv_sockaddr_len);
        printf("%d\n", client_sock);
        if(client_sock < 0)
        {
            printf("Oops! Connection failed :(\n");
            return 0;
        }
        printf("Connected!\n");
        
        recv(client_sock, client_message, sizeof(string), 0);
        printf("received: %s\n", client_message);
        send(client_sock, "Hiya", sizeof("Hiya"), 0);
        printf("Message sent\n");
    }
    return 0;
}

int server_init() {
    
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sock < 0)
    {
        printf("Oops! Socket creation failed :(\n");
        return 0;
    }
    else {
        printf("Socket created successfully!\n");
    }
    
    int options_set = setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if(options_set < 0)
    {
        printf("Oops! Setting socket address option failed :(\n");
        return 0;
    }
    
    options_set = setsockopt(serv_sock, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option));
    if(options_set < 0)
    {
        printf("Oops! Setting socket port option failed :(\n");
        return 0;
    }
    printf("Socket options were set successfully!\n");
    
    serv_socket_address.sin_family = AF_INET;
    serv_socket_address.sin_port = htons(serv_port);
    serv_socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int binded = bind(serv_sock, (struct sockaddr *) &serv_socket_address, sizeof(serv_socket_address));
    if(binded < 0)
    {
        printf("Oops! Socket binding failed :(\n");
        return 0;
    }
    else {
        printf("Socket binded successfully!\n");
    }
    
    int listening = listen(serv_sock, 8);
    if(listening < 0)
    {
        printf("Oops! Socket failed to listen :(\n");
        return 0;
    }
    else {
        printf("Server Listening on port %d...\n", serv_port);
    }
    
    return 0;
}

// Methods ----------------------------------------------------

int declare_success() {
    // TODO: Declare success
    return 0;
}

int throw_error() {
    // TODO: Throw error
    return 0;
}
