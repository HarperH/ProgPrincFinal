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
int getFileSize(char filename[]);



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

	char fileName[20];
	strcpy(fileName,"copyThis.txt");
	
	sendFile("copyThis.txt", &new_socket);

	sleep(5);
	
	//close connection
	closesocket(s);
	WSACleanup();
	
	return 0;
}

int sendFile(char fileName[], SOCKET *socket){
	
	FILE *fp;
	FILE *fs;
	char buffer[500];
	char filesize[8];
	int fileSizeTest;
	int outFileSize;
	int currentIdx = 0;
	int lastIdx;
	int n;
	int sentBytes = 0;
	
	fp = fopen(fileName, "r");
	
	//check if file was successfully opened
	if(fp == NULL){
		printf("failed to open file");
		return(0);
		
	} else {
		
		//find size of file so other side knows how many
		//bytes to wait for
		fileSizeTest = getFileSize(fileName);
		itoa(fileSizeTest,filesize,10);
		send(*socket, filesize, 8 , 0);
		//printf("sent filesize of: %s\n",filesize);
		
		do{
			
			//get chunk of data from file and add length to idx
			fgets(buffer, 500, fp);
			n = strlen(buffer);
			currentIdx += n;
			
			//send data across stream
			sentBytes = send(*socket , buffer , strlen(buffer) , 0);
			
			//printf("sent: %d\n",sentBytes);
			//printf("sending %d out of %d, n was %d\n",currentIdx,lastIdx,n);
			
			//printf(".\n");
		} while(currentIdx < fileSizeTest);
		
		printf("sent file");
		//printf("sent %d out of %d",currentIdx,fileSizeTest);
		
		return(1);
	}
}

//count number of chars in a file
int getFileSize(char fileName[]){
	
	FILE *fp;
	char nextChar;
	int charCount = 0;
	fp = fopen(fileName, "r");
	while((nextChar = fgetc(fp)) != EOF){
		charCount++;
	}
	//printf("%d",charCount);
	return(charCount);
}