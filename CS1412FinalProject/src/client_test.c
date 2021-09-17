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
#include "clientheader.h"

int main(int argc, char **argv)
{
    
    SOCKET sock;
    WSADATA winsock;
    
    
    init_winsock(&winsock);
    create_socket(&sock);
    connect_to_server(&sock);
    navigate_directories(&sock);
    
    closesocket(sock);
    WSACleanup();
    
	return 0;
}