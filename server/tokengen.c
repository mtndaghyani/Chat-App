#include <stdio.h>
#include <stdlib.h>
#include <time.h>
char characters[10]={'!','@','#','$','%','&','*','(',')','_'};
char*generator() {
    srand((unsigned int)(time(NULL)));
    char* token=malloc(33);
    for (int i = 0; i < 32; i+=4 ) {
        token[i]=48+rand()%10;
        token[i+1]='A' +(rand()%26);
        token[i+2]='a'+(rand()%26);
        token[i+3]=characters[rand()%10];

    }
    token[32]=0;
    return token;
}
