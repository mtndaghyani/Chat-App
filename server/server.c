#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <stdbool.h>
#include "cJSON.c"
#include "cJSON.h"
#include "tokengen.h"
#include "fhandle.h"
#define MAX_messages_length 2000
#define MAX_client_request 500
#define MAX_server_response 100
#define MAX_users_number 100
#define MAX_channel_number 30
#define MAX_channel_members 50


//Struct of online users
struct users
{
    char username[20];
    char auto_token[33];
} ;
struct channel_members
{
    char channel_name[20];
    char auto_token[33];

};
//Array of all created channels
char channels[MAX_channel_number][20];
int users_index=0;
//Index of channels with minimum of 1 member
int channel_index=0;
//Index of all created channels
int existing_channel_index=0;

struct users online_users[MAX_users_number];
struct channel_members channel_members[MAX_channel_number];

//Function to find online username index
int find_username(char*username)
{
    int i=0;
    for(; i<=users_index; i++)
    {
        if(strcmp(online_users[i].username,username)==0)
            return i;
    }
    return -1;


}
//Check if the user is online or not
int Verify_user(char*auto_token)
{
    int i=0;
    for(; i<=users_index; i++)
    {
        if(strcmp(online_users[i].auto_token,auto_token)==0)
            return i;
    }
    return -1;

}
//Check if user has already joined a channel or not
int check_member(char auto_token[])
{
    int i=0;
    for(; i<=channel_index; i++)
    {
        if(strcmp(channel_members[i].auto_token,auto_token)==0)
            return i;
    }
    return -1;
}

void regist(int client_socket,char *username,char *password)
{
    char user[30];
    char server_response[MAX_server_response];
    cJSON*jobj=cJSON_CreateObject();

    sprintf(user,"Database/Users/%s.txt",username);
    FILE*file=fopen(user,"r");
    if(file==NULL)
    {
        cJSON_AddStringToObject(jobj,"type","Successful");
        cJSON_AddStringToObject(jobj,"content","");
        strcpy(server_response,cJSON_Print(jobj));
        fclose(file);
        file=fopen(user,"w");
        fprintf(file,password);


    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","This username  already exists.");
        strcpy(server_response,cJSON_Print(jobj));

    }

    printf("response: %s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);
    fclose(file);
    free(server_response);


}
void login(int client_socket,char*username,char*password)
{
    char auto_token[32];
    char user[30];
    char*saved_pswd[30];
    char server_response[MAX_server_response];
    cJSON*jobj=cJSON_CreateObject();

    sprintf(user,"Database/Users/%s.txt",username);
    FILE*file=fopen(user,"r");
    fgets(saved_pswd,sizeof(saved_pswd),file);

    if(file!=NULL)
    {

        if(strcmp(saved_pswd,password)==0)
        {
            printf("index is:%d\n",find_username(username));
            if(find_username(username)==-1)
            {
                cJSON_AddStringToObject(jobj,"type","AuthToken");
                strcpy(auto_token,generator());
                cJSON_AddStringToObject(jobj,"content",auto_token);


                strcpy(server_response,cJSON_Print(jobj));
                strcpy(online_users[users_index].username,username);
                strcpy(online_users[users_index].auto_token,auto_token);
                users_index++;
            }
            else
            {
                cJSON_AddStringToObject(jobj,"type","Error");
                cJSON_AddStringToObject(jobj,"content","You have already logged in!");
                strcpy(server_response,cJSON_Print(jobj));
            }

        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","Wrong password!");
            strcpy(server_response,cJSON_Print(jobj));
        }

    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","This username doesn't exist.");
        strcpy(server_response,cJSON_Print(jobj));

    }
    printf("response :%s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);
    free(server_response);

}
void create_channel(int client_socket,char *channel_name,char *auto_token)
{
    char channel_folder_add[30];
    char channel_members_add[50];
    char channel_messages_add[50];
    char default_message[200];


    char server_response[MAX_server_response];

    cJSON*jobj=cJSON_CreateObject();
    cJSON*jmessages=cJSON_CreateObject();
    cJSON*jarr=cJSON_CreateArray();
    sprintf(channel_folder_add,"Database/Channels/%s",channel_name);
    printf("folder :%s\n",channel_folder_add);
    sprintf(channel_members_add,"Database/Channels/%s/Members.txt",channel_name);
    sprintf(channel_messages_add,"Database/Channels/%s/Messages.txt",channel_name);
    FILE*file_members;
    FILE*file_messages;
    if (Verify_user(auto_token)!=-1)
    {
        if(mkdir(channel_folder_add)==0)
        {

            cJSON_AddStringToObject(jobj,"type","Successful");
            cJSON_AddStringToObject(jobj,"content","");
            strcpy(server_response,cJSON_Print(jobj));
            //Make a default message.Useful in "receive_message"
            cJSON_AddStringToObject(jmessages,"type","List");
            cJSON_AddItemToObject(jmessages,"content",jarr);
            strcpy(default_message,cJSON_Print(jmessages));
            //---------------------------------------------
            file_members=fopen(channel_members_add,"w");
            file_messages=fopen(channel_messages_add,"w");
            fputs(default_message,file_messages);
            strcpy(channels[existing_channel_index],channel_name);
            existing_channel_index++;
            fclose(file_members);
            fclose(file_messages);
            join_channel(client_socket,channel_name,auto_token);
            free(default_message);


        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","This channel is already existed.");
            strcpy(server_response,cJSON_Print(jobj));

        }
    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed");
        strcpy(server_response,cJSON_Print(jobj));
    }

    printf("response: %s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);

    free(server_response);


}
void join_channel(int client_socket,char *channel_name,char *auto_token)
{
    char server_response[MAX_server_response];
    char channel_folder_add[30];
    char channel_members_add[50];
    FILE*members;
    FILE*joined_users;
    int index=check_member(auto_token);
    cJSON*jobj=cJSON_CreateObject();
    sprintf(channel_folder_add,"Database/Channels/%s",channel_name);
    sprintf(channel_members_add,"Database/Channels/%s/Members.txt",channel_name);

    if(Verify_user(auto_token)!=-1)
    {
        if(mkdir(channel_folder_add))
        {
            if(index==-1)
            {
                cJSON_AddStringToObject(jobj,"type","Successful");
                cJSON_AddStringToObject(jobj,"content","You joined the channel successfully.");
                strcpy(server_response,cJSON_Print(jobj));
                members=fopen(channel_members_add,"a");
                fputs(auto_token,members);
                fputs("\n",members);
                strcpy(channel_members[channel_index].channel_name,channel_name);
                strcpy(channel_members[channel_index].auto_token,auto_token);
                channel_index++;
                fclose(members);


            }
            else
            {
                cJSON_AddStringToObject(jobj,"type","Error");
                cJSON_AddStringToObject(jobj,"content","You have already joined another channel!");
                strcpy(server_response,cJSON_Print(jobj));
            }

        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","This channel doesn't exist.");
            strcpy(server_response,cJSON_Print(jobj));
            rmdir(channel_folder_add);

        }

    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed");
        strcpy(server_response,cJSON_Print(jobj));
    }
    printf("response: %s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);
    free(server_response);


}
void receive_message(int client_socket,char*message,char*auto_token)
{
    char server_response[MAX_server_response];
    char messages_list[MAX_messages_length];
    char channel_messages_add[50];
    char previous_message[500]="";
    char line[100];

    int index=check_member(auto_token);
    cJSON*jobj=cJSON_CreateObject();
    cJSON*jarr;
    cJSON*jmessages=cJSON_CreateObject();
    cJSON*jitem=cJSON_CreateObject();
    FILE*file;

    if(Verify_user(auto_token)!=-1)
    {
        if(index!=-1)
        {

            cJSON_AddStringToObject(jobj,"type","Successful");
            cJSON_AddStringToObject(jobj,"content","Message received.");
            strcpy(server_response,cJSON_Print(jobj));

            sprintf(channel_messages_add,"Database/Channels/%s/Messages.txt",channel_members[index].channel_name);
            file=fopen(channel_messages_add,"r");
            while(fgets(line,sizeof(line),file)!=NULL)
            {
                strcat(previous_message,line);
            }

            fclose(file);
            printf("previous : %s\n",previous_message);
            cJSON*jprevious=cJSON_Parse(previous_message);
            jarr=cJSON_GetObjectItem(jprevious,"content");

            cJSON_AddStringToObject(jitem,"sender",online_users[Verify_user(auto_token)].username);
            cJSON_AddStringToObject(jitem,"content",message);
            cJSON_AddItemToArray(jarr,jitem);
            cJSON_AddStringToObject(jmessages,"type","List");
            cJSON_AddItemToObject(jmessages,"content",jarr);
            strcpy(messages_list,cJSON_Print(jmessages));


            file=fopen(channel_messages_add,"w");
            fputs(messages_list,file);
            fputs("\n",file);
            fclose(file);
            printf("messages: %s\n",messages_list);


        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","You haven't joined a channel yet!");
            strcpy(server_response,cJSON_Print(jobj));
        }

    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed.");
        strcpy(server_response,cJSON_Print(jobj));

    }
    printf("server response: %s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);
    free(server_response);


}
void refresh(int client_socket,char*auto_token)
{

    char server_response[MAX_messages_length];
    char channel_messages_add[50];
    cJSON*jobj=cJSON_CreateObject();
    int index=check_member(auto_token);
    FILE*messages_list;
    sprintf(channel_messages_add,"Database/Channels/%s/Messages.txt",channel_members[index].channel_name);
    if(Verify_user(auto_token)!=-1)
    {

        char c;
        FILE*messages_list=fopen(channel_messages_add,"r");
        c=fgetc(messages_list);
        int i=0;
        while(c!=EOF)
        {
            server_response[i]=c;
            c=fgetc(messages_list);
            i++;
        }
        server_response[i]=0;

        fclose(messages_list);





    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed.");
        strcpy(server_response,cJSON_Print(jobj));
    }

    send(client_socket,server_response,sizeof(server_response),0);
    printf("response :%s\n",server_response);
    free(server_response);


}
void show_members(int client_socket,char*auto_token)
{

    char server_response[MAX_messages_length];
    char channel_members_add[50];
    char line[33];
    cJSON*jmembers=cJSON_CreateObject();
    cJSON*jarr=cJSON_CreateArray();
    cJSON*jobj=cJSON_CreateObject();
    int index=check_member(auto_token);
    FILE*members_list;
    if(Verify_user(auto_token)!=-1)
    {
        if(index!=-1)
        {
            cJSON_AddStringToObject(jobj,"type","Successful");
            cJSON_AddStringToObject(jobj,"content","");
            strcpy(server_response,cJSON_Print(jobj));

            sprintf(channel_members_add,"Database/Channels/%s/Members.txt",channel_members[index].channel_name);
            members_list=fopen(channel_members_add,"r");
            while(fgets(line,sizeof(line),members_list)!=NULL)
            {
                int i=Verify_user(line);
                if(i==-1)
                {
                    continue;
                }
                else
                {
                    strcpy(line,online_users[i].username);
                    cJSON*jstr=cJSON_CreateString(line);
                    cJSON_AddItemToArray(jarr,jstr);
                }
            }
            cJSON_AddStringToObject(jmembers,"type","List");
            cJSON_AddItemToObject(jmembers,"content",jarr);
            strcpy(server_response,cJSON_Print(jmembers));

        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","You haven't joined a channel yet!");
            strcpy(server_response,cJSON_Print(jobj));
        }
    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed.");
        strcpy(server_response,cJSON_Print(jobj));
    }

    printf("%s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);
    free(server_response);




}
void search_member(int client_socket,char * member, char *auto_token)
{
    char server_response[MAX_messages_length];
    char channel_members_add[50];
    char line[33];
    char user[50];
    int result=0;
    cJSON*jobj=cJSON_CreateObject();
    int index=check_member(auto_token);
    FILE*members_list;
    if(Verify_user(auto_token)!=-1)
    {
        if(index!=-1)
        {

            sprintf(channel_members_add,"Database/Channels/%s/Members.txt",channel_members[index].channel_name);
            members_list=fopen(channel_members_add,"r");
            while(fgets(line,sizeof(line),members_list)!=NULL)
            {
                int i=Verify_user(line);
                if(i==-1)
                {
                    continue;
                }
                else {
                    strcpy(user,online_users[i].username);
                    if(strcmp(user,member)==0)
                        result = 1;
                }
            }
            if(result){
                cJSON_AddStringToObject(jobj,"type","Successful");
                cJSON_AddStringToObject(jobj,"content","Member exists.");
                strcpy(server_response,cJSON_Print(jobj));
            }
            else{
                cJSON_AddStringToObject(jobj,"type","Successful");
                cJSON_AddStringToObject(jobj,"content","Member NOT found.");
                strcpy(server_response,cJSON_Print(jobj));
            }

        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","You haven't joined a channel yet!");
            strcpy(server_response,cJSON_Print(jobj));
        }
    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed.");
        strcpy(server_response,cJSON_Print(jobj));
    }

    printf("%s\n",server_response);
    send(client_socket,server_response,sizeof(server_response),0);
    free(server_response);
}

void leave_channel(int client_socket,char *auto_token)
{

    char server_response[MAX_server_response];
    char channel_members_add[50];
    cJSON*jobj=cJSON_CreateObject();
    if(Verify_user(auto_token)!=-1)
    {
        int index=check_member(auto_token);
        if(index!=-1)
        {
            sprintf(channel_members_add,"Database/Channels/%s/Members.txt",channel_members[index].channel_name);
            //Delete the token in members list
            file_handle(auto_token,channel_members_add);
            strcpy(channel_members[index].channel_name,"");
            strcpy(channel_members[index].auto_token,"");
            cJSON_AddStringToObject(jobj,"type","Successful");
            cJSON_AddStringToObject(jobj,"content","You have left the channel.");
            strcpy(server_response,cJSON_Print(jobj));

        }
        else
        {
            cJSON_AddStringToObject(jobj,"type","Error");
            cJSON_AddStringToObject(jobj,"content","You haven't joined a channel yet!");
            strcpy(server_response,cJSON_Print(jobj));

        }
    }

    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed.");
        strcpy(server_response,cJSON_Print(jobj));
    }
    send(client_socket,server_response,sizeof(server_response),0);
    printf("response: %s\n",server_response);
    free(server_response);


}
void logout(int client_socket,char*auto_token)
{
    char server_response[MAX_server_response];
    cJSON*jobj=cJSON_CreateObject();
    int index=Verify_user(auto_token);
    if(index!=-1)
    {
        strcpy(online_users[index].auto_token,"");
        strcpy(online_users[index].username,"");
        cJSON_AddStringToObject(jobj,"type","Successful");
        cJSON_AddStringToObject(jobj,"content","You have logged out successfully.");
        strcpy(server_response,cJSON_Print(jobj));

    }
    else
    {
        cJSON_AddStringToObject(jobj,"type","Error");
        cJSON_AddStringToObject(jobj,"content","Verification failed.");
        strcpy(server_response,cJSON_Print(jobj));
    }
    send(client_socket,server_response,sizeof(server_response),0);
    printf("response :%s",server_response);
    free(server_response);


}
void Exit()
{
    char channel_members_add[50];
    char channel_messages_add[50];
    int i=0;
    for(; i<existing_channel_index; i++)
    {
        printf("Channel %d is %s\n",i+1,channels[i]);
        sprintf(channel_members_add,"Database/Channels/%s/Members.txt",channels[i]);
        sprintf(channel_members_add,"Database/Channels/%s/Messages.txt",channels[i]);
        FILE*members=fopen(channel_members_add,"w");
        FILE*messages=fopen(channel_messages_add,"w");
        fclose(members);
        fclose(messages);

    }




}
int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;

    }
    //---------------------------NETWORK HANDLING-----------------------------------------------
    int server_socket;
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1)
        printf("Socket creation failed.\n");
    else
        printf("Socket created successfully.\n");
    struct sockaddr_in server_add,client_add;
    server_add.sin_family=AF_INET;
    server_add.sin_port=htons(12345);
    server_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    int bind_server=bind(server_socket,(struct sockaddr*)&server_add,sizeof(server_add));
    if(bind_server !=0)
        printf("Binding failed.\n");
    else
        printf("Successfully bounded.\n");
    //-----------------------------------Creating Database-------------------------------------
    mkdir("Database");
    mkdir("Database/Channels");
    mkdir("Database/Users");

    //--------------------------------------EXECUTION-------------------------------------------
    cJSON*jarr=cJSON_CreateArray();
    int client_socket;
    char client_request[MAX_client_request];
    //Variables related to client request
    char command[20];
    char keyword_1[50];
    char keyword_2[50];
    char message[MAX_messages_length];




    while(true)
    {
        if ((listen(server_socket, 5)) != 0)
        {
            printf("Listen failed...\n");
            return 0;
        }

        // Accept the data packet from client and verify
        client_socket = accept(server_socket, NULL,NULL);
        if (client_socket < 0)
        {
            printf("Server accceptance failed...\n");
            return 0;
        }
        else
        {

            recv(client_socket,client_request,sizeof(client_request),0);

            sscanf(client_request,"%s",command);
            if(strcmp(command,"register")==0)
            {
                sscanf(client_request,"%*s %s %s\n",keyword_1,keyword_2);
                regist(client_socket,keyword_1,keyword_2);
            }
            else if(strcmp(command,"login")==0)
            {
                sscanf(client_request,"%*s %s %s\n",keyword_1,keyword_2);
                login(client_socket,keyword_1,keyword_2);

            }
            else if(strcmp(command,"create_channel")==0)
            {
                sscanf(client_request,"%*s %s %s\n",keyword_1,keyword_2);
                create_channel(client_socket,keyword_1,keyword_2);

            }
            else if(strcmp(command,"join_channel")==0)
            {
                sscanf(client_request,"%*s %s %s\n",keyword_1,keyword_2);
                join_channel(client_socket,keyword_1,keyword_2);
            }
            else if(strcmp(command,"send")==0)
            {
                sscanf(client_request,"%*s  %s %[^#]s\n",keyword_1,message);

                receive_message(client_socket,message,keyword_1);
            }
            else if(strcmp(command,"refresh")==0)
            {
                sscanf(client_request,"%*s %s\n",keyword_1);
                refresh(client_socket,keyword_1);
            }
            else if(strcmp(command,"channel_members")==0)
            {
                sscanf(client_request,"%*s %s\n",keyword_1);
                show_members(client_socket,keyword_1);
            }
			else if(strcmp(command,"search")==0)
            {
                sscanf(client_request,"%*s %s %s\n",keyword_1,keyword_2);
                search_member(client_socket,keyword_1,keyword_2);
            }
            else if(strcmp(command,"leave")==0)
            {
                sscanf(client_request,"%*s %s\n",keyword_1);
                leave_channel(client_socket,keyword_1);

            }
            else if(strcmp(command,"logout")==0)
            {

                sscanf(client_request,"%*s %s\n",keyword_1);
                logout(client_socket,keyword_1);
            }
            //else if(strcmp(command,"Exit")==0)
            //{

            //  break;


            //}

        }

    }
    //Exit();


    return 0;
}
