//
//  server.h
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 1/27/20.
//  Copyright © 2020 Soroush Jahanzad. All rights reserved.
//

#ifndef server_h
#define server_h

// Types ------------------------------------------------------

typedef char string[1024];

// Method prototypes ------------------------------------------

int server_main(void);

int server_init(void);

int declare_success(int);
int throw_error(string, int);

int get_state_from_query(string);

void user_register_task(string);

void user_login_task(string);

void create_channel_task(string);

void join_channel_task(string);

void send_message_task(string);

void show_new_messages_task(string);

void show_members_task(string);

void leave_channel_task(string);

void user_logout_task(string);

#endif /* server_h */
