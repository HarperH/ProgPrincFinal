/*
	Bind socket to port 8888 on localhost
*/
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <winsock2.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define COMLENGTH 30

#pragma comment(lib,"ws2_32.lib") //Winsock Library


int sendFile(char fileName[], SOCKET *socket);



int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s , new_socket;
	struct sockaddr_in server , client;
	int c;
	char *message;

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
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
	}
	
	puts("Bind done");

	//Listen to incoming connections
	listen(s , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	
	c = sizeof(struct sockaddr_in);
	new_socket = accept(s , (struct sockaddr *)&client, &c);
	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d" , WSAGetLastError());
	}
	
	puts("Connection accepted");
	
	//Reply to client
	//message = "Hello Client , I have received your connection. But I have to go now, bye\n";
	//send(new_socket , message , strlen(message) , 0);
	
	//getchar();

	char fileName[20];
	strcpy(fileName,"copyThis.txt");
	
	sendFile("copyThis.txt", &new_socket);


	//close connection
	closesocket(s);
	WSACleanup();
	
	return 0;
}

int sendFile(char fileName[], SOCKET *socket){
	
	FILE *fp;
	FILE *fs;
	char buffer[20];
	int currentIdx = 0;
	int lastIdx;
	int n;
	
	//printf("activated sendFile");
	
	fp = fopen(fileName, "r");
	//printf("opened file");
	
	fs = fdopen(*socket, "w");
	//printf("socket link up");
	
	//connect to designated file
	if(fp == NULL){
		printf("failed to open file");
		return(0);
	} else {
		printf("sending file, buffer length %d\n",strlen(buffer));
		//find last index position then return to start of file
		fseek(fp,0,SEEK_END);
		lastIdx = ftell(fp);
		fseek(fp,0,SEEK_SET);
		
		do{
			
			//Sleep(2000);
			
			//get chunk of data from file and add length to idx
			fgets(buffer, 20, fp);
			n = strlen(buffer);
			currentIdx += n;
			
			//send data across stream
			send(*socket , buffer , strlen(buffer) , 0);
			//write (*socket, buffer, strlen(buffer));
			
			printf("sending %d out of %d, n was %d\n",currentIdx,lastIdx,n);
			
			//printf(".\n");
		} while(currentIdx < lastIdx);
		
		printf("sent %d out of %d",currentIdx,lastIdx);
	}
}