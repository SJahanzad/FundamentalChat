//
//  SJSON.c
//  FundamentalChat
//
//  Created by Soroush Jahanzad on 2/3/20.
//  Copyright © 2020 Soroush Jahanzad. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SJSON.h"

enum sjson_type{object_type, string_type, array_type};

const int max_len = 1024;

int sjson_IsString(sjson * data)
{
    if(data == NULL)
        return 0;
    return data->type == string_type;
}

int sjson_IsArray(sjson * data)
{
    if(data == NULL)
        return 0;
    return data->type == array_type;
}

int sjson_GetArraySize(sjson * data)
{
    if(data == NULL)
        return 0;
    return data->childsize;
}

sjson * sjson_CreateObject()
{
    sjson * object = malloc(sizeof(sjson));
    object->child = NULL;
    object->key = NULL;
    object->parent = NULL;
    object->next = NULL;
    object->prev = NULL;
    object->type = object_type;
    object->valuestring = NULL;
    object->godfather = NULL;
    return object;
}

sjson * sjson_CreateArray()
{
    sjson * array = malloc(sizeof(sjson));
    array->type = array_type;
    array->child = NULL;
    array->key = NULL;
    array->parent = NULL;
    array->next = NULL;
    array->prev = NULL;
    array->childsize = 0;
    array->valuestring = NULL;
    array->godfather = NULL;
    return array;
}

sjson * sjson_CreateString(char * content)
{
    sjson * result = malloc(sizeof(sjson));
    result->type = string_type;
    result->child = NULL;
    result->parent = NULL;
    result->next = NULL;
    result->prev = NULL;
    result->key = NULL;
    result->valuestring = malloc(strlen(content) + 1);
    result->godfather = NULL;
    strcpy(result->valuestring, content);
    return result;
}

void sjson_AddItemToAdjacent(sjson * object, string name, sjson * item)
{
    int cmp = strcmp(object->key, name);
    if(cmp == 0)
        return;
    if(cmp > 0)
    {
        if(object->next == NULL)
        {
            object->next = item;
            item->godfather = object;
            item->key = malloc(strlen(name) + 1);
            strcpy(item->key, name);
            return;
        }
        sjson_AddItemToAdjacent(object->next, name, item);
        return;
    }
    if(cmp < 0)
    {
        if(object->prev == NULL)
        {
            object->prev = item;
            item->godfather = object;
            item->key = malloc(strlen(name) + 1);
            strcpy(item->key, name);
            return;
        }
        sjson_AddItemToAdjacent(object->prev, name, item);
        return;
    }
}
void sjson_AddItemToObject(sjson * object, string name, sjson * item)
{
    if(item == NULL || object == NULL || item == object)
        return;
    item->parent = object;
    if(object->child == NULL)
    {
        object->child = item;
        item->godfather = object;
        item->key = malloc(strlen(name) + 1);
        strcpy(item->key, name);
        return;
    }
    sjson_AddItemToAdjacent(object->child, name, item);
}

void sjson_AddItemToArray(sjson * array, sjson * item)
{
    item->parent = array;
    if(array->child == NULL)
    {
        array->child = item;
        item->godfather = array;
        array->childsize = 1;
        return;
    }
    sjson * child = array->child;
    while (child->next != NULL) {
        child = child->next;
    }
    child->next = item;
    item->godfather = child;
    array->childsize++;
}


sjson * sjson_GetObjectItemCaseSensitive_Adjacent(sjson * object, string name)
{
    if (object == NULL)
        return NULL;
    int cmp = strcmp(object->key, name);
//    printf("\nfordebug: objectbeing investigated: %s\nkey : %s\nname: %s\ncmp: %d\n", sjson_Print(object), object->key, name, cmp);
    if(cmp > 0)
        return sjson_GetObjectItemCaseSensitive_Adjacent(object->next, name);
    if(cmp < 0)
        return sjson_GetObjectItemCaseSensitive_Adjacent(object->prev, name);
//    printf("\nfordebug: objectbeing investigated: %s\n\n", sjson_Print(object));
    return object;
}
sjson * sjson_GetObjectItemCaseSensitive(sjson * object, string name)
{
    return sjson_GetObjectItemCaseSensitive_Adjacent(object->child, name);
}

sjson * sjson_GetArrayItem(sjson * array, int which)
{
    sjson * child = array->child;
    while (which > 0) {
        child = child->next;
        which--;
    }
    return child;
}

void sjson_AddArrayToObject(sjson * object, string name)
{
    sjson_AddItemToObject(object, name, sjson_CreateArray());
}

void sjson_AddStringToObject(sjson * object, string name, string content)
{
    sjson_AddItemToObject(object, name, sjson_CreateString(content));
}

void sjson_Delete_Adjacent(sjson * object)
{
    if(object == NULL) {
        return;
    }
    if(object->key != NULL)
    {
        free(object->key);
        object->key = NULL;
    }
    
    if(object->valuestring != NULL)
    {
        free(object->valuestring);
        object->valuestring = NULL;
    }
    
    if(object->parent != NULL && object->godfather == object->parent)
        object->parent->child = NULL;
    
    sjson_Delete_Adjacent(object->prev);
    sjson_Delete_Adjacent(object->next);
    sjson_Delete_Adjacent(object->child);
    free(object);
}
void sjson_Delete(sjson * object)
{
//    printf("\nfordebug: object is being deleted: %s key: %s\n", sjson_Print(object), object->key);
    if(object == NULL) {
        return;
    }
    
//    printf("\nfordebug: adjacent object prev: %s\n", sjson_Print(object->prev));
//    printf("\nfordebug: adjacent object next: %s\n", sjson_Print(object->next));
//    printf("\nfordebug: adjacent object child: %s\n", sjson_Print(object->child));
    sjson * prev = object->prev;
    sjson * next = object->next;
    sjson * parent = object->parent;
    
//    printf("\nfordebug: HERE\n");
    if(parent != NULL && object->godfather != NULL)
    {
//        printf("\nfordebug: HERE2\n");
        if(object->godfather == object->parent)
        {
            parent->child = NULL;
        }
        else
        {
//            printf("\nfordebug: HERE3\n");
            int cmp = strcmp(object->key, object->godfather->key);
            if(cmp > 0)
                object->godfather->next = NULL;
            else
                object->godfather->prev = NULL;
        }
//        printf("\nfordebug: HERE3.1\n");
        if(prev != NULL)
            sjson_AddItemToObject(parent, prev->key, prev);
//        printf("\nfordebug: HERE3.2\n");
        if(next != NULL)
            sjson_AddItemToObject(parent, next->key, next);
    }
//    printf("\nfordebug: HERE4\n");
    if(object->key != NULL)
    {
        free(object->key);
        object->key = NULL;
    }

    if(object->valuestring != NULL)
    {
        free(object->valuestring);
        object->valuestring = NULL;
    }
    sjson_Delete_Adjacent(object->child);
//    printf("\nfordebug: HERE5\n");
    free(object);
//    printf("\nfordebug: HERE6 %s\n", sjson_Print(parent));
}

void sjson_DeleteItemFromObject(sjson * object, string name)
{
    sjson * target = sjson_GetObjectItemCaseSensitive(object, name);
//    printf("\nfordebug: found object: %s\n", sjson_Print(object));
    if(target == NULL)
        return;
    sjson_Delete(target);
}

void sjson_DeleteItemFromArray(sjson * array, int which)
{
    sjson * child = array->child, * semi_parent = array;
    if(which == 0)
    {
        array->child = child->next;
        array->childsize--;
        free(child);
        return;
    }
    while (which > 0) {
        which--;
        semi_parent->childsize--;
        semi_parent = child;
        child = child->next;
    }
    semi_parent->next = child->next;
    child->next = NULL;
    free(child);
}

void sjson_PrintAssistant(sjson * object, char * res, int isfirsttobecalled)
{
    if(object == NULL)
        return;
    
    int i;

    int index, pre_index = (int)strlen(res);
    if(isfirsttobecalled != 1)
        sjson_PrintAssistant(object->prev, res, 0);
    index = (int)strlen(res);
    if(index != pre_index)
    {
        res[index] = ',';
        res[index + 1] = '\0';
    }
    pre_index = (int)strlen(res);
    if(object->parent != NULL && object->parent->type == object_type && isfirsttobecalled != 1)
    {
        if(object->key[0] != '\0')
        {
            index = (int)strlen(res);
            res[index] = '\"';
            res[index + 1] = '\0';
        }
        index = (int)strlen(res);
        for(i = 0; object->key[i] != '\0'; i++)
        {
            res[index] = object->key[i];
            index++;
        }
        res[index] = '\0';
        index = (int)strlen(res);
        res[index + 1] = '\0';
        if(i > 0)
        {
            index = (int)strlen(res);
            res[index] = '\"';
            index++;
            res[index] = ':';
            res[index + 1] = '\0';
        }
    }
    if(object->type == object_type)
    {
        index = (int)strlen(res);
        res[index] = '{';
        res[index + 1] = '\0';
        sjson_PrintAssistant(object->child, res, 0);
        index = (int) strlen(res);
        res[index] = '}';
        res[index + 1] = '\0';
    }
    if(object->type == array_type)
    {
        index = (int)strlen(res);
        res[index] = '[';
        res[index + 1] = '\0';
        sjson_PrintAssistant(object->child, res, 0);
        index = (int) strlen(res);
        res[index] = ']';
        res[index + 1] = '\0';
    }
    if(object->type == string_type)
    {
        index = (int)strlen(res);
        res[index] = '\"';
        res[index + 1] = '\0';
        index = (int)strlen(res);
        for(i = 0; object->valuestring[i] != '\0'; i++)
        {
            res[index] = object->valuestring[i];
            index++;
        }
        res[index] = '\0';
        index = (int)strlen(res);
        res[index] = '\"';
        res[index + 1] = '\0';
    }
    index = (int) strlen(res);
    if(index != pre_index)
    {
        res[index] = ',';
        res[index + 1] = '\0';
    }
    pre_index = (int) strlen(res);
    if(isfirsttobecalled != 1)
        sjson_PrintAssistant(object->next, res, 0);
    index = (int) strlen(res);
    if(index == pre_index)
    {
        res[index - 1] = '\0';
    }
}

char * sjson_Print(sjson * object)
{
    char * res = malloc(2 * max_len);
    res[0] = '\0';
    sjson_PrintAssistant(object, res, 1);
    return res;
}

sjson * sjson_Parse(const char * const source)
{
    if(source == NULL || source[0] == '\0')
        return NULL;
    sjson * res;
    int n = (int)strlen(source);
    n -= 1;
    if(source[0] == '{')
    {
        string name;
        res = sjson_CreateObject();
        int i = 0;
        while(source[i] != '}' && i < n)
        {
            while (source[i] != '\"') {
                i++;
            }
            i++;
            int j = 0;
            while (source[i] != '\"') {
                name[j] = source[i];
                j++;
                i++;
            }
            name[j] = '\0';
            i++;
            char protagontist, antagonist = '\0';
            while (source[i] != '{' && source[i] != '[' && source[i] != '\"') {
                i++;
            }
            protagontist = source[i];
            int net_appearance = 1;
            if(protagontist == '\"')
            {
                string secondary;
                j = 0;
                i++;
                while (source[i] != '\"') {
                    secondary[j] = source[i];
                    i++;
                    j++;
                }
                secondary[j] = '\0';
                i++;
                sjson_AddItemToObject(res, name, sjson_CreateString(secondary));
                net_appearance = 0;
            }
            else if(protagontist == '{')
                antagonist = '}';
            else if(protagontist == '[')
                antagonist = ']';
            else
                return res;
            string secondary;
            j = 0;
            while(net_appearance > 0)
            {
                secondary[j] = source[i];
                i++;
                j++;
                if(source[i] == protagontist)
                    net_appearance++;
                if(source[i] == antagonist)
                    net_appearance--;
            }
            secondary[j] = antagonist;
            secondary[j + 1] = '\0';
            i++;
            sjson * temp = sjson_Parse(secondary);
            if(temp != NULL)
                sjson_AddItemToObject(res, name, temp);
        }
    }
    else if(source[0] == '[')
    {
        res = sjson_CreateArray();
        if(source[1] == ']')
            return res;
        int i = 0, j;
        while(source[i] != ']' && i < n)
        {
            char protagontist, antagonist = '\0';
            i++;
            while (source[i] != '{' && source[i] != '[' && source[i] != '\"') {
                i++;
            }
            protagontist = source[i];
            int net_appearance = 1;
            if(protagontist == '\"')
            {
                string secondary;
                j = 0;
                j++;
                i++;
                while (source[i] != '\"') {
                    secondary[j] = source[i];
                    i++;
                    j++;
                }
                secondary[j] = '\0';
                i++;
                sjson_AddItemToArray(res, sjson_CreateString(secondary));
                net_appearance = 0;
            }
            else if(protagontist == '{')
                antagonist = '}';
            else if(protagontist == '[')
                antagonist = ']';
            else
                return res;
            string secondary;
            j = 0;
            while(net_appearance > 0)
            {
                secondary[j] = source[i];
                i++;
                j++;
                if(source[i] == protagontist)
                    net_appearance++;
                if(source[i] == antagonist)
                    net_appearance--;
            }
            secondary[j] = antagonist;
            secondary[j + 1] = '\0';
            i++;
            sjson * temp = sjson_Parse(secondary);
            if(temp != NULL)
                sjson_AddItemToArray(res, temp);
        }
    }
    else if(source[0] == '\"')
    {
        string content;
        int i = 1;
        while (source[i] != '\"' && i < n) {
            content[i - 1] = source[i];
            i++;
        }
        content[i - 1] = '\0';
        res = sjson_CreateString(content);
    }
    else
        res = NULL;
    return res;
}
