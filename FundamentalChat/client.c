//
//  client.c
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 1/19/20.
//  Copyright © 2020 Soroush Jahanzad. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "cJSON.c"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

// Types ------------------------------------------------------

typedef char string[1024];

enum boolean{False, True};
enum states{account_menu_state, user_register_state, user_login_state, main_menu_state, create_channel_state, join_channel_state, user_logout_state, chat_menu_state, send_message_state, show_new_messages_state, show_channel_members_state, leave_channel_state};

// Constants --------------------------------------------------

const int maxlen = 1024;
const int request_digit_limit = 3;
const int port = 12345;

// Globals ----------------------------------------------------

int state = account_menu_state;

struct sockaddr_in socket_address;
int sock;

string token;

// Main -------------------------------------------------------

int client_main() {
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
    printf("3: Exit\n");
    
    int request = get_int();
        
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
    get_input(username);
    
    printf("Please enter your password:\n");
    get_input(password);
    
    string message;
    memset(message, 0, sizeof(string));
    sprintf(message, "register %s, %s\n", username, password);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = account_menu_state;
        return;
    }
    int success = request_server(message);
    socket_shutdown();
    
    if(success == 1)
    {
        printf("User successfully registered!\n");
    }
    else if(success == -1)
    {
        printf("Oops! An unknown error caused the registration to fail :(\n");
        // Other error options are "This username is already taken.\n"
        // or "The username or password you have entered is invalid.\n"
    }

    state = account_menu_state;
}

void user_login() // State: 2
{
    string username, password;
    
    printf("Please enter your username:\n");
    get_input(username);
    
    printf("Please enter your password:\n");
    get_input(password);
    
    string message;
    sprintf(message, "login %s, %s\n", username, password);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = account_menu_state;
        return;
    }
    send(sock, message, strlen(message), 0);
    
    memset(message, 0, sizeof(string));
    recv(sock, message, sizeof(string), 0);
    
    socket_shutdown();
    
    cJSON * result = cJSON_Parse(message), * data;
    
    data = cJSON_GetObjectItemCaseSensitive(result, "type");
    
    if(cJSON_IsString(data))
    {
        if(strcmp(data->valuestring, "AuthToken") == 0)  // Successful login
        {
            data = cJSON_GetObjectItemCaseSensitive(result, "content");
            if(cJSON_IsString(data))
            {
                memset(token, 0, sizeof(string));
                for(int i = 0; i < maxlen && data->valuestring[i] != '\0'; i++)
                    token[i] = data->valuestring[i];
                printf("Welcome, %s! :)\n", username);
                cJSON_Delete(result);
                state = main_menu_state;
                return;
            }
        }
        
        data = cJSON_GetObjectItemCaseSensitive(result, "content");
        if(cJSON_IsString(data))
        {
            if(strcmp(data->valuestring, "Wrong password.") == 0) // ERROR: incorrect data
            {
                printf("Wrong password!\n");
                cJSON_Delete(result);
                state = account_menu_state;
                return;
            }
            if(strcmp(data->valuestring, "Username is not valid.") == 0) // ERROR: no such user
            {
                printf("Invalid username!\n");
                cJSON_Delete(result);
                state = account_menu_state;
                return;
            }
        }
    }
    
    // If the function execution gets here, an unidentified error has occurred.
    printf("Oops! Login failed :(\n");
    cJSON_Delete(result);
    state = account_menu_state;
}

void main_menu() // State: 3
{
    printf("Main menu\n");
    printf("1: Create channel\n");
    printf("2: Join channel\n");
    printf("3: Logout\n");
    
    int request = get_int();
        
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
    get_input(chname);
    
    string message;
    sprintf(message, "create channel %s, %s\n", chname, token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = main_menu_state;
        return;
    }
    int success = request_server(message);
    socket_shutdown();
    
    if(success == 1)
    {
        printf("Channel \"%s\" was successfully created!\n", chname);
        state = chat_menu_state;
    }
    else if(success == -1)
    {
        printf("Oops! An unknown error prevented the user from creating the channel :(\n");
        state = main_menu_state;
    } else {
        state = main_menu_state;
    }
}

void join_channel() // State: 5
{
    string chname;
    printf("Please enter the channel name:\n");
    get_input(chname);
    
    string message;
    sprintf(message, "join channel %s, %s\n", chname, token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = main_menu_state;
        return;
    }
    int success = request_server(message);
    socket_shutdown();
    
    if(success == 1)
    {
        printf("You are now in the channel \"%s\"!\n", chname);
        state = chat_menu_state;
    }
    else if(success == -1)
    {
        printf("Oops! An unknown error prevented the user from joining the channel :(\n");
        state = main_menu_state;
    } else {
        state = main_menu_state;
    }
}

void user_logout() // State: 6
{
    string message;
    sprintf(message, "logout %s\n", token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = main_menu_state;
        return;
    }
    int success = request_server(message);
    socket_shutdown();
    
    if(success == 1)
    {
        printf("Successfully logged out.\n");
        state = account_menu_state;
    }
    else if(success == -1)
    {
        printf("Oops! Logout failed :(\n");
        state = main_menu_state;
    } else {
        state = main_menu_state;
    }
}

void chat_menu() // State: 7
{
    printf("Chat menu\n");
    printf("1: Send message\n");
    printf("2: Refresh\n");
    printf("3: Show channel members\n");
    printf("4: Leave channel\n");
    
    int request = get_int();
        
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
    
    string msg;
    get_input(msg);
    
    string message;
    sprintf(message, "send %s, %s\n", msg, token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = chat_menu_state;
        return;
    }
    int success = request_server(message);
    socket_shutdown();
    
    if(success == 1)
        printf("Message sent successfully.\n");
    else if(success == -1)
        printf("Oops! Message failed to send :(\n");
    state = chat_menu_state;
}

void show_new_messages() // State: 9
{
    string message;
    sprintf(message, "refresh %s\n", token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = chat_menu_state;
        return;
    }
    send(sock, message, strlen(message), 0);

    string response;
    memset(response, 0, sizeof(string));
    recv(sock, response, sizeof(string), 0);
    socket_shutdown();
    
    cJSON * result = cJSON_Parse(response), * data, * item, * value;

    data = cJSON_GetObjectItemCaseSensitive(result, "type");
    
    if(cJSON_IsString(data) && strcmp(data->valuestring, "List") == 0)
    {
        data = cJSON_GetObjectItemCaseSensitive(result, "content");
        if(cJSON_IsArray(data))
        {
            int n = cJSON_GetArraySize(data);
            if(n == 0)
                printf("No new messages since last refresh.\n");
            else
                printf("New messages since last refresh:\n");
            for(int i = 0; i < n; i++)
            {
                item = cJSON_GetArrayItem(data, i);
                printf("\n");
                value = cJSON_GetObjectItemCaseSensitive(item, "sender");
                printf("%s:\n", value->valuestring);
                value = cJSON_GetObjectItemCaseSensitive(item, "content");
                printf("%s\n", value->valuestring);
            }
        }
    } else {
        printf("Oops! An error occurred :(\n");
    }
    
    cJSON_Delete(result);
    
    state = chat_menu_state;
}

void show_channel_members() // State: 10
{
    string message;
    sprintf(message, "channel members %s\n", token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = chat_menu_state;
        return;
    }
    send(sock, message, strlen(message), 0);

    string response;
    memset(response, 0, sizeof(string));
    recv(sock, response, sizeof(string), 0);
    socket_shutdown();
    
    cJSON * result = cJSON_Parse(response), * data, * item;

    data = cJSON_GetObjectItemCaseSensitive(result, "type");
    
    if(cJSON_IsString(data) && strcmp(data->valuestring, "List") == 0)
    {
        data = cJSON_GetObjectItemCaseSensitive(result, "content");
        if(cJSON_IsArray(data))
        {
            int n = cJSON_GetArraySize(data);
            printf("Channel members:\n");
            for(int i = 0; i < n; i++)
            {
                item = cJSON_GetArrayItem(data, i);
                printf("%s\n", item->valuestring);
            }
        }
    } else {
        printf("Oops! An error occurred :(\n");
    }
    
    cJSON_Delete(result);
    
    state = chat_menu_state;
}

void leave_channel() // State: 11
{
    string message;
    sprintf(message, "leave %s\n", token);
    
    int connected = connect_to_server();
    if(connected < 0)
    {
        state = chat_menu_state;
        return;
    }
    int success = request_server(message);
    socket_shutdown();
    
    if(success == 1)
    {
        printf("Successfully leaved the channel.\n");
        state = main_menu_state;
    }
    else if(success == -1)
    {
        printf("Oops! Failed to leave the channel :(\n");
        state = chat_menu_state;
    } else {
        state = chat_menu_state;
    }
}

void socket_init()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("Oops! Socket creation failed :(\n");
        return;
    }
    
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
}

int connect_to_server() // creates socket and connects to server
{
    socket_init();
    if(sock < 0)
    {
        printf("Socket creation failed :(\n");
        return -1;
    }
    
    int connected = connect(sock, (struct sockaddr *) &socket_address, sizeof(socket_address));
    if(connected < 0)
    {
        printf("Oops! Connection failed :(\n");
        socket_shutdown();
    }
    
    return connected;
}

int request_server(string message)
{
    send(sock, message, strlen(message), 0);

    string response;
    memset(response, 0, sizeof(string));
    recv(sock, response, sizeof(string), 0);

    socket_shutdown();

    cJSON * result = cJSON_Parse(response), * data;

    data = cJSON_GetObjectItemCaseSensitive(result, "type");

    int success = 0;
    if(cJSON_IsString(data))
    {
        if(strcmp(data->valuestring, "Successful") == 0)
            success = 1;
        else
        {
            data = cJSON_GetObjectItemCaseSensitive(result, "content");
            if(cJSON_IsString(data))
                printf("%s\n", data->valuestring);
            else
                success = -1;
        }
    } else {
        success = -1;
    }

    cJSON_Delete(result);
    return success;
}

void socket_shutdown()
{
    shutdown(sock, SHUT_RDWR);
}

void get_input(char * buffer)
{
    int i = 0;
    char current;
    
    scanf("%c", &current);
    while (current == '\n')
    {
        scanf("%c", &current);
    }
    for(i = 0; current != '\n' && i < maxlen; i++)
    {
        buffer[i] = current;
        scanf("%c", &current);
    }
    buffer[i] = '\0';
}

int get_int()
{
    int result = 0, i = 0, has_ch_or_is_longer_than_expected = 0;
    char current;
    
    scanf("%c", &current);
    while (current == '\n')
    {
        scanf("%c", &current);
    }
    while(current != '\n')
    {
        if(current >= '0' && current <= '9' && i < request_digit_limit)
        {
            result *= 10;
            result += (current - '0');
            i++;
        } else {
            has_ch_or_is_longer_than_expected = 1;
        }
        scanf("%c", &current);
    }
    if(has_ch_or_is_longer_than_expected)
        result = 0;
    return result;
}
