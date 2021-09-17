#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "serverheader.h"


int main(int argc, char **argv)
{
    SOCKET sock, client_socket;
    WSADATA winsock;
    
    
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