#include <stdio.h>
#include <string.h>

void file_handle(char*auto_token,char* file_name)
{
    char str[1000];
	char line[40];
    FILE*file=fopen(file_name,"r");
    FILE*temp=fopen("temp.txt","w");
    while(fgets(str,sizeof(str),file)!=NULL){

		sprintf(line,"%s\n",auto_token);
        if(strcmp(str,line)==0){
            continue;
        }

        fputs(str,temp);
    }

    fclose(file);
    fclose(temp);
    file=fopen(file_name,"w");
    temp=fopen("temp.txt","r");
    while(fgets(str,sizeof(str),temp)!=NULL){
        fputs(str,file);
    }

    fclose(file);
    fclose(temp);
    remove("temp.txt");
}
