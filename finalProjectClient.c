/*
	Create a TCP socket
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <winsock2.h>
#include <sys/stat.h>
#include <sys/types.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	
	printf("Initialised.\n");
	
	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	printf("Socket created.\n");
	
	
	server.sin_addr.s_addr = inet_addr("192.168.88.152");
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected");
	
	
	//take input and create local file to copy data to
	char *str = argv[1];
	char buf[5];
	int n;
	int fd = open (str,O_CREAT | O_WRONLY,S_IRUSR|S_IWUSR);
	n = write(s,str,strlen(str));
	int idx = 0;
	do {
		n = read (s,buf,5);
        idx += n;
        printf(".");
        fflush(stdout);
        write (fd,buf,n);
    } while (n > 0);
	
	printf("copied %s successfully from server",str);
	
	
	

	return 0;
}

//test changes