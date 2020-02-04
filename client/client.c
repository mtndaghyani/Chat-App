#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"
#define MAX_username_char 30
#define MAX_pass_char 30
#define MAX_client_request 500
#define MAX_message_length 10000
#define MAX_server_response 100
#define MAX_channel_name 50
char auto_token[100];
void clrscr()
{
    system("@cls||clear");
}
int connect_to_server()
{

    int client_socket;
    client_socket=socket(AF_INET,SOCK_STREAM,0);
    if(client_socket==-1)
    {
        //Socket creation failed
        return -1;
    }

    struct sockaddr_in server_add;
    server_add.sin_family=AF_INET;
    server_add.sin_port=htons(12345);
    server_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    int connect_status=connect(client_socket,(struct sockaddr*)&server_add, sizeof(server_add));
    if(connect_status!=0)
    {
        //Connection failed
        return -2;
    }

    return client_socket;


}
void account_menu()
{
    int action;
    printf("1: Register\n");
    printf("2: Login\n");
    printf("3: Exit\n");
    printf("Please choose an action: \n");
    scanf("%d",&action);
    if(action==1)
    {
        regist();
    }
    else if(action==2)
    {
        login();
    }

    else if(action==3)
    {
        clrscr();
        printf("Are you sure you want to EXIT? yes/no?\n");
        char exit[3];
        scanf("%s",exit);
        if(strcmp(exit,"yes")==0)
        {
            Exit();

            return;
        }
        else
        {
            clrscr();
            account_menu();
        }
    }
    else
    {
        printf("INVALID input.Please try again\n");
        account_menu();
    }

}
void regist()
{

    char username [MAX_username_char];
    char password [MAX_pass_char];
    char password_again[MAX_pass_char];
    char status[20];

    char client_request[MAX_client_request];
    char server_response[MAX_server_response];
    int turn=0;


    printf("Enter a username: \n");
    scanf("%s",username);
    printf("Enter a password: \n");
    scanf("%s",password);
    printf("Enter your password again: \n");
    scanf("%s",password_again);
    if(strcmp(password,password_again)==0)
    {
        sprintf(client_request,"register %s %s\n",username,password);
        int server_socket=connect_to_server();

        if(server_socket==-1)
        {
            printf("Socket creation failed.\n");
            return;

        }
        else if(server_socket==-2)
        {
            printf("Connection failed.\n");
            return;
        }
        else
        {

            send(server_socket, &client_request[0], sizeof(client_request), 0);
            recv(server_socket, &server_response[0], sizeof(server_response), 0);
            cJSON *jobj = cJSON_Parse(server_response);
            char *type = cJSON_GetObjectItem(jobj, "type")->valuestring;
            printf("%s\n", type);
            if (strcmp(type, "Successful") == 0)
            {
                printf("Your account has been created successfully.\n\n");
                printf("PRESS ANY KEY TO RETURN TO ACCOUNT MENU.\n\n");
                getch();
                clrscr();
				closesocket(server_socket);
                account_menu();
            }
            else
            {
                printf("This username exists .Please try again.\n");
                closesocket(server_socket);
                regist();
            }
        }
    }

    else
    {
        printf("Doesn't match! Please try again\n ");
		//closesocket(server_socket);
        regist();
    }

}
void login()
{

    char username [MAX_username_char];
    char password [MAX_pass_char];
    char client_request[MAX_client_request];
    char server_response[MAX_server_response];
    int error;

    printf("Enter your username: \n");
    scanf("%s",username);
    printf("Enter a password: \n");
    scanf("%s",password);
    int server_socket=connect_to_server();
    if(server_socket==-1)
    {
        printf("Socket creation failed.\n");
        return;

    }
    else if(server_socket==-2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        sprintf(client_request, "login %s %s\n", username, password);
        send(server_socket, &client_request[0], sizeof(client_request), 0);
        recv(server_socket, &server_response[0], sizeof(server_response), 0);
        cJSON *jobj = cJSON_Parse(server_response);
        char *type = cJSON_GetObjectItem(jobj, "type")->valuestring;
        char *content = cJSON_GetObjectItem(jobj, "content")->valuestring;



        if (strcmp(type, "AuthToken") == 0)
        {
            strcpy(auto_token, content);
            //printf("Token :%s\n",auto_token);
            clrscr();
			closesocket(server_socket);
            main_menu();
        }
        else
        {
            printf("%s\n\n",content);
            printf("1: Back to Account Menu\n");
            printf("2: Retry\n");
            scanf("%d", &error);
            if (error == 1)
            {
				closesocket(server_socket);
                account_menu();
            }
            else if (error == 2)
            {
				closesocket(server_socket);
                login();
            }

        }


    }
}

void main_menu()
{
    static int turn=0;
    int action;
    if  (turn==0)
    {
        printf("-----------------ENJOY!------------------\n\n");
    }
    printf("1: Create channel\n");
    printf("2: Join a channel\n");
    printf("3: Logout\n");
    printf("Please choose an action: \n");
    scanf("%d",&action);
    if(action==1)
    {
        clrscr();
        turn++;
        create_channel();
    }
    else if(action==2)
    {
        clrscr();
        turn++;
        join_channel();
    }
    else if(action==3)
    {
        clrscr();
        turn++;
        logout();
    }
    else
    {
        printf("INVALID input.Please try again\n");
        turn++;
        main_menu();
    }

}
void create_channel()
{
    int error;
    char client_new_channel[MAX_channel_name];
    char client_request[MAX_client_request];
    char server_response[MAX_server_response];
    printf("Enter channel name:\n");
    scanf("%s", client_new_channel);
    //printf("name is :%s\n",client_new_channel);
    //printf("tok :%s\n",auto_token);
    sprintf(client_request,"create_channel %s %s\n",client_new_channel,auto_token);
    //printf("req :%s\n",client_request);
    int server_socket = connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        send(server_socket, &client_request[0], sizeof(client_request), 0);
        recv(server_socket, &server_response[0], sizeof(server_response), 0);
        cJSON *jobj = cJSON_Parse(server_response);
        char *type = cJSON_GetObjectItem(jobj, "type")->valuestring;
        char *content = cJSON_GetObjectItem(jobj, "content")->valuestring;
        if(strcmp(type,"Successful")==0)
        {
            printf("Channel is created succesfully!\n");
            clrscr();
			closesocket(server_socket);
            chat_menu();
        }
        else
        {
            printf("Something went wrong!!\n\n");
            printf("1: Back to Main menu\n");
            printf("2: Retry\n");
            scanf("%d", &error);
            if (error == 1)
            {
                clrscr();
				closesocket(server_socket);
                main_menu();
            }
            else if (error == 2)
            {
                clrscr();
				closesocket(server_socket);
                create_channel();
            }
        }
    }
}
void join_channel()
{
    int error;
    char client_existing_channel[MAX_channel_name];
    char server_response[MAX_server_response];
    char client_request[MAX_client_request];
    printf("Enter an existing channel name:\n");
    scanf("%s",client_existing_channel);
    sprintf(client_request,"join_channel %s %s\n",client_existing_channel,auto_token);

    int server_socket=connect_to_server();
    if(server_socket==-1)
    {
        printf("Socket creation failed.\n");
        return;

    }
    else if(server_socket==-2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        send(server_socket,&client_request[0], sizeof(client_request),0);
        recv(server_socket,&server_response[0], sizeof(server_response),0);

        cJSON *jobj = cJSON_Parse(server_response);
        char *type = cJSON_GetObjectItem(jobj, "type")->valuestring;
        char *content = cJSON_GetObjectItem(jobj, "content")->valuestring;
        if(strcmp(type,"Successful")==0)
        {
            printf("You joined %s succesfully!\n",client_existing_channel);
            clrscr();
			closesocket(server_socket);
            chat_menu();
        }
        else
        {
            printf("Something went wrong!!\n\n");
            printf("1: Back to Main menu\n");
            printf("2: Retry\n");
            scanf("%d",&error);
            if(error==1)
            {
				closesocket(server_socket);
                main_menu();
            }
            else if(error==2)
            {
				closesocket(server_socket);
                join_channel();
            }
        }
    }
}
void logout()
{
    char client_request[MAX_client_request];
    char server_response[MAX_server_response];
    sprintf(client_request,"logout %s\n",auto_token);
    int server_socket=connect_to_server();
    if(server_socket==-1)
    {
        printf("Socket creation failed.\n");
        return;

    }
    else if(server_socket==-2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        send(server_socket,&client_request[0], sizeof(client_request),0);
        recv(server_socket,&server_response[0],sizeof(server_response),0);
        cJSON*jobj=cJSON_Parse(server_response);
        char *type=cJSON_GetObjectItem(jobj,"type")->valuestring;
        char *content=cJSON_GetObjectItem(jobj,"content")->valuestring;
        if(strcmp(type,"Successful")==0)
            printf("%s\n",content);
        else
        {
            printf("Something went wrong!\n");
            printf("%s\n",content);
            getch();
            clrscr();
            closesocket(server_socket);
            main_menu();

        }
        getch();
        clrscr();
		closesocket(server_socket);
        account_menu();
    }

}


void chat_menu()
{
    int action;
    printf("Choose an action:\n\n");
    printf("1: Send Message\n");
    printf("2: Refresh\n");
    printf("3: Members List\n");
    printf("4 :Search members\n");
    printf("5: Leave the channel\n");
    printf("6: \Back to main menu \n");
    scanf("%d",&action);
    if(action==1)
    {
        clrscr();


        send_message();
    }
    else if(action==2)
    {
        clrscr();

        refresh();

    }
    else if(action==3)
    {
        clrscr();

        members_list();

    }
    else if (action == 4){
        clrscr();

        search_member();
    }
    else if (action==5)
    {
        clrscr();

        leave_channel();

    }
    else if(action==6){
        clrscr();
        main_menu();
    }

    else
    {
        printf("INVALID input.Please try again\n");
        chat_menu();

    }
}
void send_message()
{
    int s=1;
    int index=1;
    char character;
    char client_message[MAX_message_length-200];
    char server_response[MAX_server_response];
    char client_request[MAX_message_length];
    printf("Enter Your Message:\n");
    char temp;
    scanf("%c",&temp);
    scanf("%[^\n]s",client_message);

    int server_socket = connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        sprintf(client_request,"send %s %s#\n",auto_token,client_message);
        send(server_socket,&client_request[0],sizeof(client_request),0);
        recv(server_socket,&server_response[0], sizeof(server_response),0);
        cJSON *jobj = cJSON_Parse(server_response);
        char *type = cJSON_GetObjectItem(jobj, "type")->valuestring;
        char *content = cJSON_GetObjectItem(jobj, "content")->valuestring;

        if(strcmp(type,"Successful")==0)
        {
            printf("Your message has been sent.\n\n");
			closesocket(server_socket);
            chat_menu();
        }
        else
        {
            printf("Something went wrong.Please try again.\n\n");
			closesocket(server_socket);
            chat_menu();
        }

    }
}
void refresh()
{
    char server_response[MAX_message_length];
    char client_request[MAX_client_request];
    int server_socket = connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        sprintf(client_request,"refresh %s\n",auto_token);
        send(server_socket,&client_request[0], sizeof(client_request),0);
        recv(server_socket,&server_response[0],MAX_message_length,0);
        cJSON* jobj=cJSON_Parse(server_response);
        cJSON* content=cJSON_GetObjectItem(jobj,"content");
        int arr_size=cJSON_GetArraySize(content);
        int i;
        for (i=0; i<arr_size; i++)
        {
            cJSON* name=cJSON_GetArrayItem(content,i);
            char *sender=cJSON_GetObjectItem(name,"sender")->valuestring;
            if(strcmp(sender,"server")!=0)
            {
                char *pm=cJSON_GetObjectItem(name,"content")->valuestring;
                printf("%s sent %s\n",sender,pm);
            }
        }
            printf("Press any key to go back to chat menu.\n\n");
            getch();
			closesocket(server_socket);
            chat_menu();
        }
    }

void members_list()
{
    char server_response[MAX_message_length];
    char client_request[MAX_client_request];
    int server_socket = connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        sprintf(client_request,"channel_members %s\n",auto_token);
        send(server_socket,&client_request[0], sizeof(client_request),0);
        recv(server_socket,&server_response[0],MAX_message_length,0);
        cJSON* jobj=cJSON_Parse(server_response);
        cJSON* members=cJSON_GetObjectItem(jobj,"content");
        int arr_size=cJSON_GetArraySize(members);
        int i;
        for (i=0; i<arr_size; i++)
        {
            char* name=cJSON_GetArrayItem(members,i)->valuestring;
            printf("%d :%s\n",i+1,name);
        }
        printf("Press any key to go back to chat menu.\n\n");
        getch();
        clrscr();
		closesocket(server_socket);
        chat_menu();
    }
}
void search_member(){
    char server_response[MAX_server_response];
    char client_request[MAX_client_request];
    char member[MAX_username_char];
    printf("Enter a username: \n");
    scanf("%s",member);
    int server_socket=connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        sprintf(client_request,"search %s %s\n",member,auto_token);;
        send(server_socket,&client_request[0], sizeof(client_request),0);
        recv(server_socket,&server_response[0],sizeof(server_response),0);
        cJSON*jobj=cJSON_Parse(server_response);
        char *content=cJSON_GetObjectItem(jobj,"content")->valuestring;
        printf("%s\n",content);
        printf("Press any key to go back to chat menu.\n\n");
        getch();
        clrscr();
		closesocket(server_socket);
        chat_menu();
    }


}
void leave_channel()
{

    char server_response[MAX_server_response];
    char client_request[MAX_client_request];
    int server_socket=connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        sprintf(client_request,"leave %s\n",auto_token);
        send(server_socket,&client_request[0], sizeof(client_request),0);
        recv(server_socket,&server_response[0],sizeof(server_response),0);
        cJSON*jobj=cJSON_Parse(server_response);
        char *content=cJSON_GetObjectItem(jobj,"content")->valuestring;
        printf("%s\n",content);
        printf("Press any key to go back to main menu.\n\n");
        getch();
		closesocket(server_socket);
        main_menu();
    }
}
void Exit(){
    char server_response[MAX_message_length];
    char client_request[MAX_client_request];
    int server_socket=connect_to_server();
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        return;
    }
    else if (server_socket == -2)
    {
        printf("Connection failed.\n");
        return;
    }
    else
    {
        strcpy(client_request,"Exit\n");
        send(server_socket,client_request,sizeof(client_request),0);
        closesocket(server_socket);
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
    printf("****WELCOME TO MEPHA CHAT ! PRESS ANY KEY TO START CHATTING.****\n");
    getch();
    clrscr();
    account_menu();
    //    closesocket(client_socket);

    return 0;
}


