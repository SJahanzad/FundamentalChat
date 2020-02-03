//
//  client.h
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 1/19/20.
//  Copyright © 2020 Soroush Jahanzad. All rights reserved.
//

#ifndef client_h
#define client_h

// Types ------------------------------------------------------

typedef char string[1024];

// Method prototypes ------------------------------------------

int client_main(void);

void socket_init(void); // initializes "sock" and "socket_address"
int connect_to_server(void); // creates socket and connects to server
int request_server(string); // sends message to server and returns 1 upon success, 0 upon errors or -1 upon failure of connection
void socket_shutdown(void); // closes sock

void account_menu(void);            // State: 0
void user_register(void);           // State: 1
void user_login(void);              // State: 2
void main_menu(void);               // State: 3
void create_channel(void);          // State: 4
void join_channel(void);            // State: 5
void user_logout(void);             // State: 6
void chat_menu(void);               // State: 7
void send_message(void);            // State: 8
void show_new_messages(void);       // State: 9
void show_channel_members(void);    // State: 10
void leave_channel(void);           // State: 11

void get_input(char *);
int get_int(void);

#endif /* client_h */
