//
//  main.c
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 12/16/19.
//  Copyright © 2019 Soroush Jahanzad. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include <sys/socket.h>

// Constants --------------------------------------------------

const int maxlen = 1024;
const int port = 12345;

// Types ------------------------------------------------------

typedef char string[maxlen];

enum boolean{False, True};
enum states{account_menu_state, user_register_state, user_login_state, main_menu_state, create_channel_state, join_channel_state, user_logout_state, chat_menu_state, send_message_state, show_new_messages_state, show_channel_members_state, leave_channel_state};

// Globals ----------------------------------------------------

int state = 1;

int sock;

// Method prototypes

void socket_init(void);
int send_to_server(string);

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

// Main -------------------------------------------------------

int main(int argc, const char * argv[]) {
    while(state != -1)
    {
        switch (state) {
            case account_menu_state:
                account_menu();
                break;
            
            case user_register_state:
                user_register();
                break;
            
            case user_login_state:
                user_login();
                break;
            
            case main_menu_state:
                main_menu();
                break;
            
            case create_channel_state:
                create_channel();
                break;
            
            case join_channel_state:
                join_channel();
                break;
            
            case user_logout_state:
                user_logout();
                break;
            
            case chat_menu_state:
                chat_menu();
                break;
            
            case send_message_state:
                send_message();
                break;
            
            case show_new_messages_state:
                show_new_messages();
                break;
                
            case show_channel_members_state:
                show_channel_members();
                break;
            
            case leave_channel_state:
                leave_channel();
                break;
            
            default:
                break;
        }
        
        printf("\n");
    }
    return 0;
}

// Methods ----------------------------------------------------

void account_menu() // State: 0
{
    printf("Account menu\n");
    printf("1: Register\n");
    printf("2: Login\n");
    printf("3: Exit\n\n");
    
    int request = 0;
    scanf("%d", &request);
        
    switch (request) {
        case 1:
            state = user_register_state;
            break;

        case 2:
            state = user_login_state;
            break;
            
        case 3:
            state = -1;
            break;
                
        default:
            printf("Please enter a valid command!\n");
            break;
    }
}

void user_register() // State: 1
{
    string username, password;
    
    printf("Please enter your username:\n");
    scanf("%s", username);
    
    printf("Please enter your password:\n");
    scanf("%s", password);
    
    
    int success = 0;
    
    //TODO: Register
    string request;
    sprintf(request, "register %s, %s\n", username, password);
    
    if(success == 1) // Success
        printf("User successfully registered!\n");
    else if(success == 2) // ERROR
        printf("User has already registered!\n");
    else // ERROR
        printf("Oops! registration failed :(\n");
    
    state = account_menu_state;
}

void user_login() // State: 2
{
    string username, password;
    
    printf("Please enter your username:\n");
    scanf("%s", username);
    
    printf("Please enter your password:\n");
    scanf("%s", password);
    
    int success = 0;
    
    //TODO: Login
    
    if(success == 1) // successful login
        state = main_menu_state;
    else if(success == 2) // ERROR: no such user
    {
        state = account_menu_state;
        printf("No such user exists!\n");
    } else { // ERROR: incorrect data
        state = account_menu_state;
        printf("Incorrect username or password!\n");
    }
}

void main_menu() // State: 3
{
    printf("Main menu\n");
    printf("1: Create channel\n");
    printf("2: Join channel\n");
    printf("3: Logout\n\n");
    
    int request = 0;
    scanf("%d", &request);
        
    switch (request) {
        case 1:
            state = create_channel_state;
            break;

        case 2:
            state = join_channel_state;
            break;
            
        case 3:
            state = user_logout_state;
            break;
                
        default:
            printf("Please enter a valid command!\n");
            break;
    }
}

void create_channel() // State: 4
{
    string chname;
    printf("Please enter your channel name:\n");
    scanf("%s", chname);
    
    int success = 0;
    
    //TODO: Create channel
    
    if(success) // Channel created successfully
    {
        state = chat_menu_state;
        printf("Channel \"%s\" was successfully created!\n", chname);
    } else { // ERROR
        state = main_menu_state;
        printf("Oops! An error occurred :(\n");
    }
}

void join_channel() // State: 5
{
    string chname;
    printf("Please enter the channel name:\n");
    scanf("%s", chname);
    
    int success = 0;
    
    //TODO: Join Channel
    
    if(success) // Joined channel successfully
    {
        state = chat_menu_state;
        printf("You are now in the channel \"%s\"!\n", chname);
    } else { // ERROR
        state = main_menu_state;
        printf("Oops! An error occurred :(\n");
    }
}

void user_logout() // State: 6
{
    //TODO: Logout
    
    state = account_menu_state;
}

void chat_menu() // State: 7
{
    printf("Chat menu\n");
    printf("1: Send message\n");
    printf("2: Refresh\n");
    printf("3: Show channel members\n");
    printf("4: Leave channel");
    
    int request = 0;
    scanf("%d", &request);
        
    switch (request) {
        case 1:
            state = send_message_state;
            break;

        case 2:
            state = show_new_messages_state;
            break;
            
        case 3:
            state = show_channel_members_state;
            break;
            
        case 4:
            state = leave_channel_state;
            break;
                
        default:
            printf("Please enter a valid command!\n");
            break;
    }
}

void send_message() // State: 8
{
    printf("Message:\n");
    
    int i;
    string msg;
    char current;
    
    scanf("%c", &current);
    
    for(i = 0; current != '\n' && i < maxlen; i++)
    {
        msg[i] = current;
        scanf("%c", &current);
    }
    msg[i] = '\0';
    
    //TODO: send message
    
    state = chat_menu_state;
}

void show_new_messages() // State: 9
{
    printf("New messages since last refresh:\n");
    
    //TODO: get and print messages
    
    state = chat_menu_state;
}

void show_channel_members() // State: 10
{
    printf("Channel members:\n");
    
    //TODO: get and print members
    
    state = chat_menu_state;
}

void leave_channel() // State: 11
{
    printf("Channel members:\n");
    
    //TODO: leave channel
    
    state = main_menu_state;
}

void socket_init()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
}

int send_to_server(string message)
{
    //TODO: connect to server and send message
    
    return 0;
}
