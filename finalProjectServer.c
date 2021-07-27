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
	
	while(1){
		
		//read file location sent by client and store it
		char incCom[COMLENGTH];
		int recCmd = read(new_socket, incCom, COMLENGTH);
		incCom[recCmd] = 0;
		printf("sending: %s",incCom);
		
		//open file at location
		int fd = open(incCom,O_RDONLY);
		
		//mark the end of the file the bring pointer back to front of file
		int fileEnd = lseek(fd,0,SEEK_END);
		lseek(fd,0,SEEK_SET);
		int chIdx = 0;
		do{
			
			//read file contents and send them through socket to client
			recCmd = read(fd,incCom,COMLENGTH);
			write(new_socket,incCom,recCmd);
			chIdx += recCmd;
		} while(chIdx < fileEnd);
		
		printf("%d/%d\n",chIdx,fileEnd);
	}
		
		


	
	
	puts("Connection accepted");
	
	
	

	//Reply to client
	message = "Hello Client , I have received your connection. But I have to go now, bye\n";
	send(new_socket , message , strlen(message) , 0);
	
	getchar();

	//close connection
	closesocket(s);
	WSACleanup();
	
	return 0;
}	


//test changes
