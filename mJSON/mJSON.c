#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Don't forget to free the memory after using mJSON_GetItems ( free( mJSON* ) )
#define MAX_object_length 2000
char *mJSON_CreateObject(){
    char *ptr=malloc(MAX_object_length);
    strcpy(ptr,"{}");
    return  ptr;
}
void mJSON_AddStringtoObject(char * object,char *key,char * value){
    object[strlen(object)-1]=' ';
    char string[50];
    char format[50]="\"%s\":\"%s\" ";
    sprintf(string,format,key,value);
    strcat(object,string);
    strcat(object,"}");
}
void mJSON_AddItemtoObject(char * object,char *key,char * item){
    object[strlen(object)-1]=' ';
    char string[500];
    char format[400]="\"%s\":%s ";
    sprintf(string,format,key,item);
    strcat(object,string);
    strcat(object,"}");
}
char* mJSON_CreateArray(){
    char *ptr=malloc(MAX_object_length);
    strcpy(ptr,"[]");
    return  ptr;
}
void mJSON_AddItemtoArray(char * array,char * item){
    array[strlen(array)-1]=' ';
    char string[500];
    char format[450]="%s, ";
    sprintf(string,format,item);
    strcat(array,string);
    strcat(array,"]");
}
void mJSON_AddStringtoArray(char * array,char *string){
    char str[50];
    char format[50]="\"%s\", ";
    array[strlen(array)-1]=' ';
    sprintf(str,format,string);
    strcat(array,str);
    strcat(array,"]");
}


char * mJSON_GetObjectString(char* object,char *key){
    char * token=malloc(100);
    char * result = malloc(100);
    strcpy(token,strtok(object,":"));
    while (token) {
        if (strstr(token, key)) {
            strcpy(token, strtok(NULL, ":"));
            int counter = 0;
            int i = 0;
            int j = 0;
            while (counter < 2) {
                if (token[i] == '"') {
                    counter++;
                    i++;
                    continue;
                } else {
                    result[j] = token[i];
                    j++;
                    i++;
                }

                result[j + 1] = 0;
            }
            return result;

        }
        else
            strcpy(token, strtok(NULL, ":"));

        }
    return NULL;
}
char * mJSON_GetObjectArray(char* object,char *key){
    char * token=malloc(1000);
    char * result = malloc(1000);
    strcpy(token,strtok(object,":"));
    while (token) {
        if(strstr(token,key))
        {
            strcpy(token, strtok(NULL, ":"));
            int counter=0;
            int i=0;
            int j=0;
            while (counter <2) {
                if (token[i] == '[' || token[i] == ']')
                    counter++;
                if(token[i] == '}')
                {
                    i++;
                    continue;
                }
                result[j] = token[i];
                j++;
                i++;
                result[j + 1] = 0;
            }
            free(token);
            return result;
        }
        else
            strcpy(token, strtok(NULL, ":"));
    }
    free(token);
    return NULL;
}
char * mJSON_GetArrayItem(char* array,int index) {
    char *token = malloc(100);
    char *result = malloc(100);
    int counter = 0;
    strcpy(token, strtok(array, ","));
    while (token) {
        if (counter == index) {

            int char_counter = 0;
            int i = 0;
            int j = 0;
            while (char_counter < 2) {
                if (token[i] == '"') {
                    char_counter++;
                    i++;
                    continue;
                }
                else if(token[i] == ' ' || token[i] == '[' || token[i] == ']'){
                    i++;
                    continue;
                }
                else {
                    result[j] = token[i];
                    j++;
                    i++;
                }

                result[j + 1] = 0;
            }
            free(token);
            return result;

        } else {
            strcpy(token, strtok(NULL, ","));
            counter++;
        }
    }
    free(token);
    return NULL;
}
int mJSON_GetArraySize(char * array){
    char *token = malloc(100);
    int counter = 0;
    strcpy(token, strtok(array, ","));
    while (token) {
            strcpy(token, strtok(NULL, ","));
            counter++;
            printf("%d\n",counter);

    }
    free(token);
    return counter + 1;
}


int main() {

    

    return 0;
}
