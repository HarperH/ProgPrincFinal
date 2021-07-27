#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#define MAX_FILE_NAME_LENGTH 100
#define MAX_NUMBER_OF_FOLDERS 500

int init_winsock(WSADATA *ws);
int create_socket(SOCKET *s);
int bind_socket(SOCKET *unbound_socket);
int set_server_to_listen(SOCKET *socket, int number_of_devices);
int accept_socket_request(SOCKET *socket, SOCKET *client);
void navigate_directories(SOCKET *socket, SOCKET *client);
void scan_directory(char directory_path[]);

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
    navigate_directories(&sock, &client_socket);
    
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

void navigate_directories(SOCKET *socket, SOCKET *client){
    
    //192.168.88.152
    char command[100];
    int size;
    size = recv(*client, command, 100, 0);
    if(size == 0){
        printf("Success\n");
        command[strlen(command)] = '\0';
        printf("%c", command[0]);
    }
    else{
        printf("Failed to recieve.");
    }
    
}

void scan_directories(char directory_path[]){
    
    
}