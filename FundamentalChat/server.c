
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
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "server.h"

// Types ------------------------------------------------------

typedef char string[1024];

enum boolean{False, True};
enum server_states{idle_state, user_register_state, user_login_state, create_channel_state, join_channel_state, send_message_state, show_new_messages_state, show_members_state, leave_channel_state, user_logout_state};

// Constants --------------------------------------------------

const int serv_port = 12345;
const int mxlen = 1024;

// Globals ----------------------------------------------------

int serv_sock, client_sock;
struct sockaddr_in serv_socket_address;
int option = 1;

int serv_state = idle_state;

cJSON * users, * channels, * user_channels, * channel_messages, * last_read, * online;

// Main -------------------------------------------------------

int server_main()
{
    server_init();
    
    int serv_sockaddr_len = sizeof(serv_socket_address);
    string client_message, username, password;
    
    users = cJSON_CreateObject();
    channels = cJSON_CreateObject();
    user_channels = cJSON_CreateObject();
    channel_messages = cJSON_CreateObject();
    last_read = cJSON_CreateObject();
    online = cJSON_CreateObject();
    
    while(serv_state != -1) {
        memset(client_message, 0, sizeof(string));
        memset(username, 0, sizeof(string));
        memset(password, 0, sizeof(string));
        
        client_sock = accept(serv_sock, (struct sockaddr *) &serv_socket_address, (socklen_t *) &serv_sockaddr_len);
        if(client_sock < 0)
        {
            printf("Oops! Connection failed :(\n");
            return 0;
        }
        printf("\nConnected to client!\n");
        
        recv(client_sock, client_message, sizeof(string), 0);
        printf("Received message: %s\n", client_message);
        
        serv_state = get_state_from_query(client_message);
        
        switch (serv_state) {
            case user_register_state:
                user_register_task(client_message);
                break;
            
            case user_login_state:
                user_login_task(client_message);
                break;
            
            case create_channel_state:
                create_channel_task(client_message);
                break;
            
            case join_channel_state:
                join_channel_task(client_message);
                break;
            
            case send_message_state:
                send_message_task(client_message);
                break;
            
            case show_new_messages_state:
                show_new_messages_task(client_message);
                break;
            
            case show_members_state:
                show_members_task(client_message);
                break;
            
            case leave_channel_state:
                leave_channel_task(client_message);
                break;
            
            case user_logout_state:
                user_logout_task(client_message);
                break;
                
            default:
                break;
        }
    }
    
    cJSON_Delete(users);
    cJSON_Delete(channels);
    cJSON_Delete(user_channels);
    cJSON_Delete(channel_messages);
    cJSON_Delete(last_read);
    cJSON_Delete(online);
    return 0;
}

// Methods ----------------------------------------------------

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

int declare_success(int c_sock) {
    cJSON * success = cJSON_CreateObject();
    cJSON_AddStringToObject(success, "type", "Successful");
    cJSON_AddStringToObject(success, "content", "");
    
    char * res = cJSON_Print(success);
    cJSON_Delete(success);
    
    send(c_sock, res, strlen(res), 0);
    printf("Sent message: %s\n", res);
    
    free(res);
    
    return 0;
}

int throw_error(string error_message, int c_sock) {
    cJSON * error = cJSON_CreateObject();
    cJSON_AddStringToObject(error, "type", "Error");
    cJSON_AddStringToObject(error, "content", error_message);
    
    char * res = cJSON_Print(error);
    cJSON_Delete(error);
    
    send(c_sock, res, strlen(res), 0);
    printf("Sent message: %s\n", res);
    
    free(res);
    
    return 0;
}

int get_state_from_query(string query) {
    string command;
    sscanf(query, "%s", command);
    
    if(strcmp(command, "register") == 0)
        return user_register_state;
    
    if(strcmp(command, "login") == 0)
        return user_login_state;
    
    if(strcmp(command, "create") == 0)
        return create_channel_state;
    
    if(strcmp(command, "join") == 0)
        return join_channel_state;
    
    if(strcmp(command, "send") == 0)
        return send_message_state;
    
    if(strcmp(command, "refresh") == 0)
        return show_new_messages_state;
    
    if(strcmp(command, "channel") == 0)
        return show_members_state;
    
    if(strcmp(command, "leave") == 0)
        return leave_channel_state;
    
    if(strcmp(command, "logout") == 0)
        return user_logout_state;
    
    return idle_state;
}

void user_register_task(string client_message)
{
    string username, password, fname;
    
    int i = 0, j = strlen("register ");
    while(client_message[j] != ',')
    {
        username[i] = client_message[j];
        i++;
        j++;
    }
    username[i] = '\0';
    if(username[0] == '\0')
    {
        throw_error("Please enter a valid username.", client_sock);
        return;
    }
    
    j += 2;
    i = 0;
    while(client_message[j] != '\n')
    {
        password[i] = client_message[j];
        i++;
        j++;
    }
    password[i] = '\0';
    if(password[0] == '\0')
    {
        throw_error("Please enter a valid password.", client_sock);
        return;
    }
    
    //TODO: directory creation
//    struct stat st = {0};
//    if (stat("./ChatResources/Users", &st) == -1) {
    int dircreatedres = mkdir("./ChatResources", S_IRWXU);
    if(dircreatedres != 0 && errno != EEXIST)
    {
        throw_error("Directory creation failed.", client_sock);
        return;
    }
    dircreatedres = mkdir("./ChatResources/Users", S_IRWXU);
    if(dircreatedres != 0 && errno != EEXIST)
    {
        throw_error("Directory creation failed.", client_sock);
        return;
    }
//        printf("HEEEESEREREREEEEE!\n");
//    }
    
    memset(fname, 0, sizeof(fname));
    sprintf(fname, "./ChatResources/Users/%s.json", username);
    
    FILE * db = fopen(fname, "r");
    if (db != NULL) {
        throw_error("User is already existing.", client_sock);
        fclose(db);
        return;
    }
    db = fopen(fname, "w");
    if (db == NULL) {
        throw_error("Couldn't initialize the database.", client_sock);
        return;
    }
    
    cJSON * user = cJSON_CreateObject();
    cJSON_AddStringToObject(user, "username", username);
    cJSON_AddStringToObject(user, "password", password);
    
    char * res = cJSON_Print(user);
    
    fprintf(db, "%s\n", res);
    
    fclose(db);
    free(res);
    
    declare_success(client_sock);
    
    cJSON_Delete(user);
}

void user_login_task(string client_message)
{
    string username, password, fname, user, lower_username;
    
    int i = 0, j = strlen("login ");
    while(client_message[j] != ',')
    {
        username[i] = client_message[j];
        lower_username[i] = username[i];
        if(username[i] >= 'A' && username[i] <='Z')
            lower_username[i] += 'a' - 'A';
        i++;
        j++;
    }
    username[i] = '\0';
    if(username[0] == '\0')
    {
        throw_error("Please enter a valid username.", client_sock);
        return;
    }
    
    j += 2;
    i = 0;
    while(client_message[j] != '\n')
    {
        password[i] = client_message[j];
        i++;
        j++;
    }
    password[i] = '\0';
    if(password[0] == '\0')
    {
        throw_error("Please enter a valid password.", client_sock);
        return;
    }
    
    memset(fname, 0, sizeof(fname));
    sprintf(fname, "./ChatResources/Users/%s.json", username);
    
    FILE * db = fopen(fname, "r");
    if (db == NULL) {
        throw_error("User not found.", client_sock);
        return;
    }
    
    i = 0;
    char c = fgetc(db);
    while (!feof(db) && i < mxlen) {
        user[i] = c;
        c = fgetc(db);
        i++;
    }
    user[i] = '\0';
    fclose(db);
    
    if(cJSON_GetObjectItemCaseSensitive(online, lower_username) != NULL)
    {
        throw_error("User is already logged in.", client_sock);
        return;
    }
    
    cJSON * contents = cJSON_Parse(user), * real_password;
    real_password = cJSON_GetObjectItemCaseSensitive(contents, "password");
    
    if(strcmp(password, real_password->valuestring) == 0)
    {
        int token_len = 32;
        string token = "";
        string seed = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
        int n = (int) strlen(seed), r;
        do {
            srand((int)time(NULL));
            for(i = 0; i < token_len; i++)
            {
                r = rand() % n;
                token[i] = seed[r];
            }
            seed[i] = '\0';
        } while(cJSON_GetObjectItemCaseSensitive(users, token) != NULL);
        
        cJSON_AddStringToObject(users, token, username);
        cJSON_AddStringToObject(online, lower_username, token);
        
        cJSON * token_message = cJSON_CreateObject();
        
        cJSON_AddStringToObject(token_message, "type", "AuthToken");
        cJSON_AddStringToObject(token_message, "content", token);
        
        char * res = cJSON_Print(token_message);
        
        send(client_sock, res, strlen(res), 0);
        printf("Sent message: %s\n", res);
        
        free(res);
        
        cJSON_Delete(token_message);
    } else {
        throw_error("Incorrect password.", client_sock);
    }
    
    cJSON_Delete(contents);
}

void create_channel_task(string client_message)
{
    string channel_name, token, fname;
    
    int k = (int) strlen(client_message);
    while (client_message[k] != ',' && k >= 0) {
        k--;
    }
    
    int i = 0, j = strlen("create channel ");
    while(j != k)
    {
        channel_name[i] = client_message[j];
        i++;
        j++;
    }
    channel_name[i] = '\0';
    if(channel_name[0] == '\0')
    {
        throw_error("Please enter a valid channel name.", client_sock);
        return;
    }
    
    j += 2;
    i = 0;
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }
    
    int dircreatedres = mkdir("./ChatResources", S_IRWXU);
    if(dircreatedres != 0 && errno != EEXIST)
    {
        throw_error("Directory creation failed.", client_sock);
        return;
    }
    dircreatedres = mkdir("./ChatResources/Channels", S_IRWXU);
    if(dircreatedres != 0 && errno != EEXIST)
    {
        throw_error("Directory creation failed.", client_sock);
        return;
    }
    
    if(cJSON_GetObjectItemCaseSensitive(user_channels, token) != NULL)
    {
        throw_error("User is already in a channel.", client_sock);
        return;
    }
        
    memset(fname, 0, sizeof(fname));
    sprintf(fname, "./ChatResources/Channels/%s.json", channel_name);
        
    FILE * db = fopen(fname, "r");
    if (db != NULL) {
        throw_error("Channel is already existing.", client_sock);
        fclose(db);
        return;
    }
    db = fopen(fname, "w");
    if (db == NULL) {
        throw_error("Couldn't initialize the database.", client_sock);
        return;
    }
    
    cJSON * channel = cJSON_CreateObject(), * arr;
    cJSON_AddArrayToObject(channel, "messages");
    cJSON_AddStringToObject(channel, "name", channel_name);
    
    cJSON_AddArrayToObject(channel_messages, channel_name);
    
    char * res = cJSON_Print(channel);
    fprintf(db, "%s", res);
    free(res);
    
    cJSON_Delete(channel);
    fclose(db);
    
    channel = cJSON_CreateObject();
    arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateString(user->valuestring));
    cJSON_AddStringToObject(channel, "type", "List");
    cJSON_AddItemToObject(channel, "content", arr);
    cJSON_AddItemToObject(channels, channel_name, channel);
    
    cJSON_AddStringToObject(user_channels, token, channel_name);
    cJSON_AddStringToObject(last_read, user->valuestring, "0");
    
    declare_success(client_sock);
}

void join_channel_task(string client_message)
{
    string channel_name, token, fname;
    
    int k = (int) strlen(client_message);
    while (client_message[k] != ',' && k >= 0) {
        k--;
    }
    
    int i = 0, j = strlen("join channel ");
    while(j != k)
    {
        channel_name[i] = client_message[j];
        i++;
        j++;
    }
    channel_name[i] = '\0';
    if(channel_name[0] == '\0')
    {
        throw_error("Please enter a valid channel name.", client_sock);
        return;
    }
    
    j += 2;
    i = 0;
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }
    
    if(cJSON_GetObjectItemCaseSensitive(user_channels, token) != NULL)
    {
        throw_error("User is already in a channel.", client_sock);
        return;
    }
    
    memset(fname, 0, sizeof(fname));
    sprintf(fname, "./ChatResources/Channels/%s.json", channel_name);
    
    FILE * db = fopen(fname, "r");
    if (db == NULL) {
        throw_error("Channel not found.", client_sock);
        return;
    }
    string channel_content;
    i = 0;
    char c = fgetc(db);
    while (!feof(db) && i < mxlen - 1) {
        channel_content[i] = c;
        c = fgetc(db);
        i++;
    }
    fclose(db);
    channel_content[i] = '\0';
    cJSON * channel_json =  cJSON_Parse(channel_content);
    cJSON * messages = cJSON_GetObjectItemCaseSensitive(channel_json, "messages");
    
    char * res = cJSON_Print(messages);
    cJSON_AddItemToObject(channel_messages, channel_name, cJSON_Parse(res));
    free(res);
    cJSON_Delete(channel_json);
    
    cJSON * channel = cJSON_GetObjectItemCaseSensitive(channels, channel_name);
    if(channel == NULL)
    {
        channel = cJSON_CreateObject();
        cJSON * arr = cJSON_CreateArray();
        cJSON_AddStringToObject(channel, "type", "List");
        cJSON_AddItemToObject(channel, "content", arr);
        cJSON_AddItemToObject(channels, channel_name, channel);
    }
    
    cJSON_AddItemToArray(cJSON_GetObjectItemCaseSensitive(channel, "content"), cJSON_CreateString(user->valuestring));
    cJSON_AddStringToObject(user_channels, token, channel_name);
    cJSON_AddStringToObject(last_read, user->valuestring, "0");
    
    declare_success(client_sock);
}

void send_message_task(string client_message)
{
    string message, token, fname;
    
    int k = (int) strlen(client_message);
    while (client_message[k] != ',' && k >= 0) {
        k--;
    }
    
    int i = 0, j = strlen("send ");
    while(j != k)
    {
        message[i] = client_message[j];
        i++;
        j++;
    }
    message[i] = '\0';
    if(message[0] == '\0')
    {
        throw_error("Please enter a valid channel name.", client_sock);
        return;
    }
    
    j += 2;
    i = 0;
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }
    
    cJSON * channel = cJSON_GetObjectItemCaseSensitive(user_channels, token);
    if(channel == NULL)
    {
        throw_error("User is not in a channel.", client_sock);
        return;
    }
    
    memset(fname, 0, sizeof(fname));
    sprintf(fname, "./ChatResources/Channels/%s.json", channel->valuestring);
    
    FILE * db = fopen(fname, "r");
    if (db == NULL) {
        throw_error("Channel not found.", client_sock);
        return;
    }
    string channel_content;
    i = 0;
    char c = fgetc(db);
    while (!feof(db) && i < mxlen) {
        channel_content[i] = c;
        c = fgetc(db);
        i++;
    }
    channel_content[i] = '\0';
    fclose(db);
    
    cJSON * channel_json =  cJSON_Parse(channel_content);
    
    cJSON * original_messages = cJSON_GetObjectItemCaseSensitive(channel_json, "messages");
    
    cJSON * message_json = cJSON_CreateObject();
    cJSON_AddStringToObject(message_json, "sender", user->valuestring);
    cJSON_AddStringToObject(message_json, "content", message);
    
    cJSON_AddItemToArray(original_messages, message_json);
    
    char * res = cJSON_Print(channel_json);
    db = fopen(fname, "w");
    if (db == NULL) {
        throw_error("Channel not found.", client_sock);
        return;
    }
    fprintf(db, "%s", res);
    fclose(db);
    free(res);
    
    message_json = cJSON_CreateObject();
    cJSON_AddStringToObject(message_json, "sender", user->valuestring);
    cJSON_AddStringToObject(message_json, "content", message);
    cJSON_AddItemToArray(cJSON_GetObjectItemCaseSensitive(channel_messages, channel->valuestring), message_json);
    
    cJSON_Delete(channel_json);
    
    declare_success(client_sock);
}

void show_new_messages_task(string client_message)
{
    string token;
    
    int k = (int) strlen(client_message);
    while (client_message[k] != ',' && k >= 0) {
        k--;
    }
    
    int i = 0, j = strlen("refresh ");
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }
    
    cJSON * channel = cJSON_GetObjectItemCaseSensitive(user_channels, token);
    if(channel == NULL)
    {
        throw_error("User is not in a channel.", client_sock);
        return;
    }
    
    cJSON * messages = cJSON_GetObjectItemCaseSensitive(channel_messages, channel->valuestring);
    char * unread_duplicate = cJSON_Print(messages);
    cJSON * unread_messages = cJSON_CreateObject();
    cJSON_AddStringToObject(unread_messages, "type", "List");
    cJSON_AddItemToObject(unread_messages, "content", cJSON_Parse(unread_duplicate));
    free(unread_duplicate);
    cJSON * arr = cJSON_GetObjectItemCaseSensitive(unread_messages, "content");
    int n = atoi(cJSON_GetObjectItemCaseSensitive(last_read, user->valuestring)->valuestring);
    while (n > 0) {
        cJSON_DeleteItemFromArray(arr, 0);
        n--;
    }
    if(cJSON_GetObjectItemCaseSensitive(unread_messages, "content") == NULL)
        cJSON_AddArrayToObject(unread_messages, "content");
    
    cJSON_DeleteItemFromObject(last_read, user->valuestring);
    string last_read_num;
    sprintf(last_read_num, "%d", cJSON_GetArraySize(messages));
    cJSON_AddStringToObject(last_read, user->valuestring, last_read_num);
    char * res = cJSON_Print(unread_messages);
    
    cJSON_Delete(unread_messages);
    
    send(client_sock, res, strlen(res), 0);
    printf("Sent message: %s\n", res);
    
    free(res);
}

void show_members_task(string client_message)
{
    string token;

    int i = 0, j = strlen("channel members ");
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }
    
    cJSON * channel = cJSON_GetObjectItemCaseSensitive(user_channels, token);
    if(channel == NULL)
    {
        throw_error("User is not in a channel.", client_sock);
        return;
    }
    cJSON * channel_members = cJSON_GetObjectItemCaseSensitive(channels, channel->valuestring);
    if(channel_members == NULL)
    {
        channel_members = cJSON_CreateObject();
        cJSON * arr = cJSON_CreateArray();
        cJSON_AddItemToArray(arr, cJSON_CreateString(user->valuestring));
        cJSON_AddStringToObject(channel_members, "type", "List");
        cJSON_AddItemToObject(channel_members, "content", arr);
        cJSON_AddItemToObject(channels, channel->valuestring, channel_members);
    }
    char * res = cJSON_Print(channel_members);
    send(client_sock, res, strlen(res), 0);
    printf("Sent message: %s\n", res);
    free(res);
}

void leave_channel_task(string client_message)
{
    string token;

    int i = 0, j = strlen("leave ");
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }

    cJSON * channel = cJSON_GetObjectItemCaseSensitive(user_channels, token);
    if(channel == NULL)
    {
        throw_error("User is not in a channel.", client_sock);
        return;
    }
    cJSON * arr = cJSON_GetObjectItemCaseSensitive(channels, channel->valuestring);
    arr = cJSON_GetObjectItemCaseSensitive(arr, "content");
    int n = cJSON_GetArraySize(arr);
    i = 0;
    while (i < n && strcmp(cJSON_GetArrayItem(arr, i)->valuestring, cJSON_GetObjectItemCaseSensitive(users, token)->valuestring) != 0) {
        i++;
    }
    if(i == n)
    {
        throw_error("User is not in a channel.", client_sock);
        return;
    }
    
    cJSON_DeleteItemFromObject(channel_messages, channel->valuestring);
    cJSON_DeleteItemFromArray(arr, i);
    cJSON_DeleteItemFromObject(user_channels, token);
    
    declare_success(client_sock);
}

void user_logout_task(string client_message)
{
    string token, lower_username;
    
    int i = 0, j = strlen("logout ");
    while(client_message[j] != '\n')
    {
        token[i] = client_message[j];
        i++;
        j++;
    }
    token[i] = '\0';
    cJSON * user = cJSON_GetObjectItemCaseSensitive(users, token);
    if(user == NULL || token[0] == '\0')
    {
        throw_error("Invalid token.", client_sock);
        return;
    }
    for(i = 0; token[i] != '\0' && i < mxlen - 1; i++)
    {
        lower_username[i] = user->valuestring[i];
        if(user->valuestring[i] >= 'A' && user->valuestring[i] <='Z')
            lower_username[i] += 'a' - 'A';
    }
    lower_username[i] = '\0';
    
    cJSON_DeleteItemFromObject(online, lower_username);
    cJSON_DeleteItemFromObject(users, token);
    
    declare_success(client_sock);
}
