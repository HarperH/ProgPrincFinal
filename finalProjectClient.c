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


#define BUFFERLEN 500


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
	//fflush(stdout);
	char fileName[20];
	strcpy(fileName,"thisIsCopy.txt");
	
	receiveFile(fileName, &s);
	

	return 0;
}


int receiveFile(char fileName[], SOCKET *socket){
	
	FILE *fp;
	FILE *fs;
	char buffer[BUFFERLEN];
	char filesize[8];
	int incFileSize;
	int n=0;
	int j=0;
	int len;
	int currentIdx = 0;
	int lastIdx;
	char *readyCheck;
	int flag;
	int checkLen;
	
	
	fp = fopen(fileName,"w");
	fs = fdopen(*socket, "r");
	/*
	while(flag == 0){
		checkLen = recv(*socket, readyCheck, 100,0);
		readyCheck[checkLen] = '\0';
		send(*socket, "readyrecv", strlen("readyrecv"),0);
		if(strcmp(readyCheck,"readysend")==0){
			flag = 1;
		}
	}
	*/
	
	//sleep(5);
	//verify the file can be created
	if(fp == NULL){
		printf("failed to open file");
		return(0);
	} else {
		
		printf("copying file\n");		
		
		
		//receive number of incoming characters
		incFileSize=recv(*socket,filesize,BUFFERLEN,0);
		if(incFileSize < 1){
			do{
				printf("didn't receive file size, retrying");
				sleep(1);
				len=recv(*socket,filesize,BUFFERLEN,0);
				printf("\n%s\n",filesize);
			} while (incFileSize < 1);
		} else {
			lastIdx = atoi(filesize);
			//printf("received file size of: %d\n",lastIdx);
		}
		
		sleep(1);
		len = recv(*socket,buffer,BUFFERLEN,0);
		//printf("len is: %d",len);
		//printf("%d,%s\n",len,buffer);
		while(len < 0 && n < 3){
			n++;
			printf("transfer failed, retrying\n");
			sleep(1);
			len=recv(*socket,filesize,BUFFERLEN,0);
		}
		
		while(currentIdx < lastIdx){

			//n++;
			//printf("len is: %d",len);
			if(len < 0){
				len = 0;
				sleep(1);
			}
			currentIdx += len;
			buffer[len] = '\0';
			fputs(buffer,fp);
			//printf("copied: %s\n",buffer);

			//printf("%d out of %d\n", currentIdx,lastIdx);
			
			len=recv(*socket,buffer,BUFFERLEN,0);
			//printf("len is: %d\n",len);
		}
		
		//printf("last len: %d",len);
		
		printf("file copied");
		//return to to show success
		return(1);
	}
}