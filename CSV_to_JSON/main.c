//
//  main.c
//  CVS_To_MongoDBQuery
//
//  Created by wonhee jang on 12. 2. 25..
//  Copyright (c) 2012년 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int isStringDouble(char *s);
void parseLine(char* _line, int i, cJSON* jso);

int isStringDouble(char *s) {
    size_t size = strlen(s);
    if (size == 0) return 0; // 0바이트 문자열은 숫자가 아님
    
    for (int i = 0; i < (int) size; i++) {
        if (s[i] == '.' || s[i] == '-' || s[i] == '+') continue;
        if (s[i] < '0' || s[i] > '9') return 0; // 알파벳 등이 있으면 숫자 아님
    }
    
    return 1; // 그밖의 경우는 숫자임
}


void parseLine(char* _line, int i, cJSON* json) {
    cJSON* item =  cJSON_GetArrayItem(json, i);
    char* addr = _line;
    char* field = strtok(addr, ",");
    addr += strlen(field) + 1;
    int j = 0;
    while (field) {
        if(i == 0)
            cJSON_AddItemToArray(item, cJSON_CreateString(field));
        else {
            if(isStringDouble(field)) {
                cJSON_AddItemToObject(item, cJSON_GetString(cJSON_GetArrayItem(cJSON_GetArrayItem(json, 0), j)), cJSON_CreateNumber(atof(field)));
            } else {
                cJSON_AddItemToObject(item, cJSON_GetString(cJSON_GetArrayItem(cJSON_GetArrayItem(json, 0), j)), cJSON_CreateString(field));
            }
        }
        field = strtok(addr, ",");
        if(field) {
            addr += strlen(field) + 1;
            j++;
        }
    }
}

int main (int argc, const char * argv[])
{
    if(argc != 2) {
        printf("다음과 같이 사용하세요.\nCVS_to_JSON \"cvs파일\"\n");
        return 1;
    }
    FILE* f = fopen(argv[1], "rb");
    if(f == NULL) {
        printf("%s파일이 존재하지 않습니다.\n", argv[1]);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    size_t s = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* inbuf = calloc(1, s + 1);
    fread(inbuf, s, 1, f);
    fclose(f);
    
    
    cJSON* js = cJSON_CreateArray();
    char* address = inbuf;
    char* _line = strtok(address, "\n");
    address += strlen(_line) + 1;
    int i = 0;
    char** line = calloc(1, sizeof(char*));
    
    while (_line) {
        line = realloc(line, sizeof(char**) * (i + 1));
        
        line[i] = calloc(1, strlen(_line) + 1);
        sprintf(line[i], "%s", _line);
        
        cJSON* arrin;
        if(i == 0)
            arrin = cJSON_CreateArray();
        else
            arrin = cJSON_CreateObject();
        cJSON_AddItemToArray(js, arrin);
        parseLine(line[i], i, js);
        
        _line = strtok(address, "\n");
        if(_line) {
            address += strlen(_line) + 1;
            i++;
        }
    }
    
    for(int j = 0; j < i; j++) {
        free(line[j]);
    }
    free(line);
    
    cJSON_DetachItemFromArray(js, 0);
    
    
    if(1) {
        FILE *output;
        output = popen("more", "w");
        char* code = cJSON_Print(js);
        fprintf(output, "%s\n", code);
        free(code);
        pclose(output);
    }
    
    cJSON_Delete(js);
    free(inbuf);
    
    return 0;
}

