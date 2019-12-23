//
//  main.c
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 12/16/19.
//  Copyright © 2019 Soroush Jahanzad. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include "cJSON.h"

// Constants --------------------------------------------------

const int maxlen = 1024;

// Types ------------------------------------------------------

typedef char string[maxlen];

enum boolean{False, True};

// Globals ----------------------------------------------------

int state = 1;

// Method prototypes

void account_menu(void);            // State: 1
void user_register(void);           // State: 2
void user_login(void);              // State: 3
void main_menu(void);               // State: 4
void create_channel(void);          // State: 5
void join_channel(void);            // State: 6
void chat_menu(void);               // State: 7
void send_message(void);            // State: 8
void show_new_messages(void);       // State: 9
void show_channel_members(void);    // State: 10

// Main -------------------------------------------------------

int main(int argc, const char * argv[]) {
    while(state != 0)
    {
        switch (state) {
            case 1:
                account_menu();
                break;
            
            case 2:
                user_register();
                break;
            
            case 3:
                user_login();
                break;
            
            case 4:
                main_menu();
                break;
            
            case 5:
                create_channel();
                break;
            
            case 6:
                join_channel();
                break;
            
            case 7:
                chat_menu();
                break;
            
            case 8:
                send_message();
                break;
            
            case 9:
                show_new_messages();
                break;
                
            case 10:
                show_channel_members();
                break;
                
            default:
                break;
        }
        
        printf("\n");
    }
    return 0;
}

// Methods ----------------------------------------------------

void account_menu() // State: 1
{
    printf("Account menu\n");
    printf("1: Register\n");
    printf("2: Login\n");
    printf("3: Exit\n\n");
    
    int request = 0;
    scanf("%d", &request);
        
    switch (request) {
        case 1:
            state = 2;
            break;

        case 2:
            state = 3;
            break;
            
        case 3:
            state = 0;
            break;
                
        default:
            printf("Please enter a valid command!\n");
            break;
    }
}

void user_register() // State: 2
{
    string username, password;
    
    printf("Please enter your username:\n");
    scanf("%s", username);
    
    printf("Please enter your password:\n");
    scanf("%s", password);
    
    
    int success = 0;
    
    //TODO: Register
    
    if(success == 1) // Success
        printf("User successfully registered!\n");
    else if(success == 2) // ERROR
        printf("User has already registered!\n");
    else // ERROR
        printf("Oops! registration failed :(\n");
    
    state = 1;
}

void user_login() // State: 3
{
    string username, password;
    
    printf("Please enter your username:\n");
    scanf("%s", username);
    
    printf("Please enter your password:\n");
    scanf("%s", password);
    
    int success = 0;
    
    //TODO: Login
    
    if(success == 1) // successful login
        state = 4;
    else if(success == 2) // ERROR: no such user
    {
        state = 1;
        printf("No such user exists!\n");
    } else { // ERROR: incorrect data
        state = 1;
        printf("Incorrect username or password!\n");
    }
}

void main_menu() // State: 4
{
    printf("Main menu\n");
    printf("1: Create channel\n");
    printf("2: Join channel\n");
    printf("3: Logout\n\n");
    
    int request = 0;
    scanf("%d", &request);
        
    switch (request) {
        case 1:
            state = 5;
            break;

        case 2:
            state = 6;
            break;
            
        case 3:
            state = 1;
            break;
                
        default:
            printf("Please enter a valid command!\n");
            break;
    }
}

void create_channel() // State: 5
{
    string chname;
    printf("Please enter your channel name:\n");
    scanf("%s", chname);
    
    int success = 0;
    
    //TODO: Create channel
    
    if(success) // Channel created successfully
    {
        state = 7;
        printf("Channel \"%s\" was successfully created!\n", chname);
    } else { // ERROR
        state = 4;
        printf("Oops! An error occurred :(\n");
    }
}

void join_channel() // State: 6
{
    string chname;
    printf("Please enter the channel name:\n");
    scanf("%s", chname);
    
    int success = 0;
    
    //TODO: Join Channel
    
    if(success) // Joined channel successfully
    {
        state = 7;
        printf("You are now in the channel \"%s\"!\n", chname);
    } else { // ERROR
        state = 4;
        printf("Oops! An error occurred :(\n");
    }
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
            state = 8;
            break;

        case 2:
            state = 9;
            break;
            
        case 3:
            state = 10;
            break;
            
        case 4:
            state = 4;
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
}

void show_new_messages() // State: 9
{
    printf("New messages:\n");
    
    //TODO: get and print messages
}

void show_channel_members() // State: 10
{
    printf("Channel members:\n");
    
    //TODO: get and print members
}
