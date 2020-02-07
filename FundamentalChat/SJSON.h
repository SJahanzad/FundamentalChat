//
//  SJSON.h
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 2/3/20.
//  Copyright © 2020 Soroush Jahanzad. All rights reserved.
//

#ifndef SJSON_h
#define SJSON_h

#include <stdio.h>

typedef char string[1024];

typedef struct sjson {
    char * key;
    struct sjson * parent;
    struct sjson * child;
    struct sjson * prev;
    struct sjson * next;
    struct sjson * godfather;
    char * valuestring;
    int type;
    int childsize;
    char * spare;
} sjson;

int sjson_IsString(sjson * data);

int sjson_IsArray(sjson * data);

int sjson_GetArraySize(sjson * data);

sjson * sjson_CreateObject(void);

sjson * sjson_CreateArray(void);

sjson * sjson_CreateString(char *);

void sjson_AddItemToAdjacent(sjson *, string, sjson *);
void sjson_AddItemToObject(sjson *, string, sjson *);

void sjson_AddItemToArray(sjson * array, sjson * item);

sjson * sjson_GetObjectItemCaseSensitive_Adjacent(sjson * object, string name);
sjson * sjson_GetObjectItemCaseSensitive(sjson * object, string name);

sjson * sjson_GetArrayItem(sjson * array, int which);

void sjson_AddArrayToObject(sjson * object, string name);

void sjson_AddStringToObject(sjson * object, string name, string content);

void sjson_Delete_Adjacent(sjson * object);
void sjson_Delete(sjson * object);

void sjson_DeleteItemFromObject(sjson * object, string name);

void sjson_DeleteItemFromArray(sjson * array, int which);

void sjson_PrintAssistant(sjson * object, char * res, int isfirsttobecalled);
char * sjson_Print(sjson * object);

sjson * sjson_Parse(const char * const source);

#endif /* SJSON_h */
