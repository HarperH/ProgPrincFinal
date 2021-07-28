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


#define BUFFERLEN 2000


int receiveFile(char fileName[], SOCKET *socket);


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
	
	
	server.sin_addr.s_addr = inet_addr("192.168.88.125");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected");
	
	char fileName[20];
	strcpy(fileName,"thisIsCopy.txt");
	
	receiveFile(fileName, &s);
	

	return 0;
}


int receiveFile(char fileName[], SOCKET *socket){
	
	FILE *fp;
	FILE *fs;
	char buffer[BUFFERLEN];
	int n;
	int len;
	
	fp = fopen(fileName,"w");
	fs = fdopen(*socket, "r");
	
	//verify the file can be created
	if(fp == NULL){
		printf("failed to open file");
		return(0);
	} else {
		
		printf("copying file\n");		
		
		
		len=recv(*socket,buffer,BUFFERLEN,0);
		if(len < 1){
			printf("got nothing");
			sleep(2);
			len=recv(*socket,buffer,BUFFERLEN,0);
			printf("\n%s\n",buffer);
		}
		
		while(len > 0){
			printf("len is: %d",len);
			buffer[len] = '\0';
			fputs(buffer,fp);
			printf("copied: %s\n",buffer);
			
			len=recv(*socket,buffer,BUFFERLEN,0);
			/*
			if(len < 1){
				printf("sleeping\n");
				sleep(2);
				len=recv(*socket,buffer,BUFFERLEN,0);
				printf("\n%s\n",buffer);
			}
			*/
		}
		
		printf("last len: %d",len);
		
		printf("file copied");
		//return to to show success
		return(1);
	}
}