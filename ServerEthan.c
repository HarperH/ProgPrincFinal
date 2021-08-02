#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILE_NAME_LENGTH 100
#define MAX_NUMBER_OF_FOLDERS 500

int init_winsock(WSADATA *ws);
int create_socket(SOCKET *s);
int bind_socket(SOCKET *unbound_socket);
int set_server_to_listen(SOCKET *socket, int number_of_devices);
int accept_socket_request(SOCKET *socket, SOCKET *client);
void navigate_directories(SOCKET *socket, SOCKET *client);
void scan_directories(char directory_path[], SOCKET *client);
int recieve_information(SOCKET *client);

int main(int argc, char **argv)
{
    SOCKET sock, client_socket;
    WSADATA winsock;
    int flag = 0;
    
    
    init_winsock(&winsock);
    create_socket(&sock);
    bind_socket(&sock);
    set_server_to_listen(&sock, 1);
    accept_socket_request(&sock, &client_socket);
    recieve_information(&client_socket);
    
    closesocket(sock);
    closesocket(client_socket);
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

int bind_socket(SOCKET *unbound_socket){
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    int flag;
    flag = bind(*unbound_socket, (SOCKADDR *) &server, sizeof(server));
    if(flag == 0){
        printf("The Socket was Successfully Bound.\n");
        return 1;
    }
    else{
        printf("An Error Occurred when Binding the Socket.\n");
        return 0;
    }
}

int set_server_to_listen(SOCKET *socket, int number_of_devices){
    
    int flag = listen(*socket, number_of_devices);
    if(flag == 0){
        printf("Server is now listening.\n");
        return 1;
    }
    else{
        printf("Server failed to start listening.\n");
        return 0;
    }
}


int accept_socket_request(SOCKET *socket, SOCKET *client){
    
    *client = accept(*socket, NULL, NULL);
    if(*client == INVALID_SOCKET){
        printf("Falied to accept socket.\n");
        return 0;
    }
    else{
        printf("Succesfully accepted socket.\n");
        return 1;
    }
}

void scan_directories(char directory_path[], SOCKET *client){
    
    printf("Directory being scanned: %s\n", directory_path);
    
    char list_files_and_folders[MAX_NUMBER_OF_FOLDERS][MAX_FILE_NAME_LENGTH];
    char *message;
    int count = 0;
    DIR *given_directory = opendir(directory_path);
    struct dirent *stream;
  
    sleep(2);
    if (given_directory == NULL)
    {
        printf("Could not open current directory" );
    }

    //Stores directory info in list of files and folders
    while((stream = readdir(given_directory)) != NULL){
        if((strcmp(stream->d_name, ".") != 0) && (strcmp(stream->d_name, "..") != 0)){
            strcpy(list_files_and_folders[count], stream->d_name);
            count++;
        }
    }
    
    printf("Number of folders in directory: %d\n", count);
    
    //Sends all the folders and files in the directory
    for(int i = 0; i < count; i++){
        send(*client, list_files_and_folders[i], 100, 0);
    }
    
    send(*client, "done", strlen("done"), 0);
    
    printf("Finished sending %s files\n", directory_path);
    closedir(given_directory);
    
}

int recieve_information(SOCKET *client){
    
    char command[2000];
    int size;
    int flag = 0;
        
    while(((size = recv(*client , command, 100 , 0)) == -1) || (flag == 0)){
            
        if(size != -1)
        {
            command[size] = '\0';
            printf("Recieved Command: %s\n", command);
            sleep(5);
            if(strcmp(command, "init") == 0){
                printf("Sending C Drive Information....\n");
                scan_directories("C:\\", client);
                printf("Finished Sending C Drive Information.\n");
                flag = 0;
            }
            else if(strcmp(command, "move") == 0){
                size = recv(*client, command, 2000, 0);
                while(size == -1){
                    size = recv(*client, command, 2000, 0);
                }
                command[size] = '\0';
                printf("Moving to directory: %s\n", command);
                sleep(5);
                scan_directories(command, client);
                flag = 0;
            }
        }
    }
}