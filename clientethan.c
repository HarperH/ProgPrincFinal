#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define MAX_FILE_NAME_LENGTH 100
#define MAX_NUMBER_OF_FOLDERS 500

int init_winsock(WSADATA *ws);
int create_socket(SOCKET *s);
int connect_to_server(SOCKET *s);
int send_commands_to_server(SOCKET *s);

int main(int argc, char **argv)
{
    
    
    /*printf("Which would you like to go to: ");
    scanf("%d", &selection);
    realloc(dir_name, sizeof(dir_name)+strlen(list_files_and_folders[selection-1])+1);
    strcat(dir_name, list_files_and_folders[selection-1]);
    
    printf("%s\n",dir_name);
    
    given_directory = opendir(dir_name);
  
    if (given_directory == NULL)
    {
        printf("\nCould not open current directory: %s\n",strerror(errno));
    }
    
    count = 0;
    
    while((stream = readdir(given_directory)) != NULL){
        if((strcmp(stream->d_name, ".") != 0) && (strcmp(stream->d_name, "..") != 0)){
            strcpy(list_files_and_folders[count], stream->d_name);
            count++;
        }
    }
    
    for(int i = 0; i < count; i++){
        printf("%d: %s\n", i+1, list_files_and_folders[i]);
    }
    
    closedir(given_directory);*/
    
    
    
    
    SOCKET sock;
    WSADATA winsock;
    
    
    init_winsock(&winsock);
    create_socket(&sock);
    connect_to_server(&sock);
    send_commands_to_server(&sock);
    
    closesocket(sock);
    WSACleanup();
    
    
    
    
	return 0;
}


int init_winsock(WSADATA *ws){
    
    int init_success;
    
    //Initializes Winsock to version 2.2
    init_success = WSAStartup(MAKEWORD(2,2), ws);
    if(init_success == 0){
        printf("Winsock Initialization Successfull.\n");
        return 1;
    }
    else{
        printf("Winsock Initialization Failed.\n");
        printf("Error code: %d", WSAGetLastError());
        return 0;
    }
    
}

int create_socket(SOCKET *s){
    
    //Will store the address information
    struct addrinfo address_information;
    ZeroMemory(&address_information, sizeof(address_information));
    address_information.ai_family = AF_INET;
    address_information.ai_socktype = SOCK_STREAM;
    address_information.ai_protocol = IPPROTO_TCP;
    
    
    *s = socket(address_information.ai_family, address_information.ai_socktype, address_information.ai_protocol);
    if(*s == INVALID_SOCKET){
        printf("Couldn't create socket.\n");
        return 0;
    }
    else{
        printf("Socket Was Successfully Created.\n");
        return 1;
    }
}

int connect_to_server(SOCKET *s){
    
    struct sockaddr_in server;
    char ip_address[15];
    printf("What address would you like to connect to: ");
    scanf("%s", &ip_address);
    
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    
    int flag;
    flag = connect(*s, (struct sockaddr *) &server, sizeof(server));
    if(flag == 0){
        printf("Connection Successfull.\n");
        return 1;
    }
    else{
        printf("Connection Failed.\n");
        return 0;
        
    }
}

/*void navigate_directories(){
    
    char dir_name[] = "C:\\";
    char list_files_and_folders[MAX_NUMBER_OF_FOLDERS][MAX_FILE_NAME_LENGTH];
    int count = 0;
    int selection;
    DIR *given_directory = opendir(dir_name);
    struct dirent *stream;
  
    if (given_directory == NULL)
    {
        printf("Could not open current directory" );
    }
    
    while((stream = readdir(given_directory)) != NULL){
        if((strcmp(stream->d_name, ".") != 0) && (strcmp(stream->d_name, "..") != 0)){
            strcpy(list_files_and_folders[count], stream->d_name);
            count++;
        }
    }
    
    //Lists all the folders and files in the C drive
    for(int i = 0; i < count; i++){
        printf("%d: %s\n", i+1, list_files_and_folders[i]);
    }
    
    
    printf("What would you like to do.\n1:  ")
}*/

int send_commands_to_server(SOCKET *s){
    
    printf("Displaying the contents of the C drive.\n");
    //char dir_name[] = "C:\\";
    char *command = "What the hell.";
    int error = send(*s, command, (int) strlen(command), 0);
    if(error == 0){
        printf("Success\n");
    }
    else{
        printf("Failed to send\n");
    }
    
    /*int selection;
    printf("What woud you like to do.\n1: Move directories.\n2: Scan contents of a file for information.\n3: Create a file.\n4: Pull a file.");
    scanf("%d", &selection);*/
    
    
    
}
