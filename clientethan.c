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

#define MAX_FILE_NAME_LENGTH 100
#define MAX_NUMBER_OF_FOLDERS 500
#define BUFFERLEN 500

int init_winsock(WSADATA *ws);
int create_socket(SOCKET *s);
int connect_to_server(SOCKET *s);
void navigate_directories(SOCKET *s);
int receiveFile(char fileName[], SOCKET *socket);
char *string_concat(char *string1, char *string2);
char *string_concat_backslash(char *string1, char *string2);
int sendFile(char fileName[], SOCKET *socket);
int getFileSize(char filename[]);

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

int connect_to_server(SOCKET *s){
    
    struct sockaddr_in server;
    char ip_address[15];
    printf("What address would you like to connect to: ");
    scanf("%s", &ip_address);
    
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    
    int flag;
    flag = connect(*s, (struct sockaddr *) &server, sizeof(server));
    if(flag == 0){
        printf("Connection Successfull.\n");
        return 1;
    }
    else{
        printf("Connection Failed.\n");
        return 0;
        
    }
}

void navigate_directories(SOCKET *s){
    
    char *dir_name;
    dir_name = malloc(4);
    dir_name[0] = 'C';
    dir_name[1] = ':';
    dir_name[2] = '\\';
    dir_name[3] = '\0';
    
    
    //Displays C Drive Information
    int len;
    char *command;
    char message[MAX_FILE_NAME_LENGTH];
    command = "init";
    char list_files_and_folders[MAX_FILE_NAME_LENGTH][MAX_NUMBER_OF_FOLDERS];
    int count = 0;
    int flag = 0;
    int size;
    int selection;
    char word[MAX_FILE_NAME_LENGTH];
    char toler[MAX_FILE_NAME_LENGTH];
    
    //Sends Command to Server to request C Drive Info
    printf("Sending C Drive request to server.\n");
    send(*s, command, (int)strlen(command), 0);
    
    
    //Recieves C drive info
    while(((size = recv(*s , list_files_and_folders[count], 100 , 0)) == -1) || (flag == 0)){
        while(size != -1){
                count++;
                size = recv(*s , list_files_and_folders[count], 100 , 0);
                if(size == 4){
                    flag = 1;
                    size = -1;
                    count++;
                }
        }
        if(flag == 1){
            break;
        }
    }
    
    
    //Puts info into a list
    for(int i = 0; i < count; i++){
        if(strcmp(list_files_and_folders[i], "done") == 0){
            count = i;
            break;
        }
    }
    
    
    //Prints the C drive
    for(int i = 0; i < count; i++){
        printf("%d: %s\n", i+1, list_files_and_folders[i]);
    }
    
    
    while(selection != 6){
        printf("\n\nWhat woud you like to do.\n1: Move directories.\n2: Scan contents of a file for information.\n3: Create a file.\n"
                "4: Pull a file.\n5: Display current Directory info.\n6: Exit\n--> ");
        scanf("%d", &selection);
        
        if(selection == 1){
            
            //Tells the server we are changing directories
            command = "move";
            send(*s, command, 100, 0);
            
            //Tells the server which folder we would like to go into
            printf("Where would you like to go: (Select -1 to move up a directory): ");
            scanf("%d", &selection);
            
            
            
            if(selection != -1){
                
                //Sends this path to the server
                string_concat_backslash(dir_name, list_files_and_folders[selection-1]);
                printf("Moving to %s\n", dir_name);
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                count = 0;
                size = -1;
                flag = 0;
                printf("Waiting for file information.\n");
                //Recieves folder information from the server then populates list_files_and_folders then displays this list
                while(((size = recv(*s , list_files_and_folders[count], 100 , 0)) == -1) || (flag == 0)){
                    while(size != -1){
                            count++;
                            size = recv(*s , list_files_and_folders[count], 100 , 0);
                            if(size == 4){
                                flag = 1;
                                size = -1;
                                count++;
                            }
                    }
                    if(flag == 1){
                        break;
                    }
                }
                for(int i = 0; i < count; i++){
                    if(strcmp(list_files_and_folders[i], "done") == 0){
                        count = i;
                        break;
                    }
                }
                for(int i = 0; i < count; i++){
                    printf("%d: %s\n", i+1, list_files_and_folders[i]);
                }
                
            }
            else{
                
                //Removes the last directory name
                len = strlen(dir_name);
                for(int j = len; j > 0; j--){
                    if((dir_name[j-1] != '\\') || (j == len)){
                        dir_name[strlen(dir_name)-1] = '\0';
                    }
                    else{
                        j = 0;
                    }
                }
                
                //Sends directory to the server
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                count = 0;
                size = -1;
                flag = 0;
                printf("Waiting for file information.\n");
                //Recieves folder information from the server then populates list_files_and_folders then displays this list
                while(((size = recv(*s , list_files_and_folders[count], 100 , 0)) == -1) || (flag == 0)){
                    while(size != -1){
                            count++;
                            size = recv(*s , list_files_and_folders[count], 100 , 0);
                            if(size == 4){
                                flag = 1;
                                size = -1;
                                count++;
                            }
                    }
                    if(flag == 1){
                        break;
                    }
                }
                for(int i = 0; i < count; i++){
                    if(strcmp(list_files_and_folders[i], "done") == 0){
                        count = i;
                        break;
                    }
                }
                for(int i = 0; i < count; i++){
                    printf("%d: %s\n", i+1, list_files_and_folders[i]);
                }
            }
            
            selection = 1;
        }
        
        
        else if(selection == 2){
            printf("Exact word match or approximate word match (1 for Exact 2 for approximate) : ");
            scanf("%d", &selection);
            if(selection == 1){
                command = "scane";
                send(*s, command, 100, 0);
                printf("What file would you like to have scanned: ");
                scanf("%d", &selection);
                
                string_concat(dir_name, list_files_and_folders[selection-1]);
                printf("Sending to %s\n", dir_name);
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                len = strlen(dir_name);
                for(int j = len; j > 0; j--){
                    if((dir_name[j-1] != '\\') || (j == len)){
                        dir_name[strlen(dir_name)-1] = '\0';
                    }
                    else{
                        j = 0;
                    }
                }
                
                printf("What word would you like to search for: ");
                scanf("%s", word);
                send(*s, word, (int)strlen(word), 0);
                printf("Waiting for results...\n");
                
                size = -1;
                while(size == -1){
                    size = recv(*s, message, 100, 0);
                }
                printf("%s\n", message);
            }
            
            else{
                command = "scanr";
                send(*s, command, 100, 0);
                printf("What file would you like to have scanned: ");
                scanf("%d", &selection);
                
                string_concat(dir_name, list_files_and_folders[selection-1]);
                printf("Sending to %s\n", dir_name);
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                len = strlen(dir_name);
                for(int j = len; j > 0; j--){
                    if((dir_name[j-1] != '\\') || (j == len)){
                        dir_name[strlen(dir_name)-1] = '\0';
                    }
                    else{
                        j = 0;
                    }
                }
                
                printf("What word would you like to search for: ");
                scanf("%s", word);
                send(*s, word, (int)strlen(word), 0);
                
                printf("How many characters would you like to be consistently matching: ");
                scanf("%s", toler);
                send(*s, word, (int)strlen(word), 0);
                printf("Waiting for results...\n");
                
                size = -1;
                while(size == -1){
                    size = recv(*s, message, 100, 0);
                }
                printf("%s\n", message);
            }
        }
        
        else if(selection == 3){
            
                command = "creat";
                send(*s, command, 100, 0);
                
                printf("Enter the file path that you would like to send: ");
                scanf("%s", &message);
                
                sendFile(message, *s);
        }
        
        else if(selection == 4){
            
            command = "send";
            send(*s, command, 100, 0);
            
            
            printf("What file would you like to have sent to you: ");
            scanf("%d", &selection);
            string_concat(dir_name, list_files_and_folders[selection-1]);
            printf("Pulling %s\n", dir_name);
            send(*s, dir_name, (int)strlen(dir_name), 0);
            
            receiveFile(list_files_and_folders[selection-1], s);
        }
        
        else if(selection == 5){
            
            printf("\n\n\n");
            
            for(int i = 0; i < count; i++){
                if(strcmp(list_files_and_folders[i], "done") == 0){
                    count = i;
                    break;
                }
            }
            for(int i = 0; i < count; i++){
                printf("%d: %s\n", i+1, list_files_and_folders[i]);
            }
            
        }
    }
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
	
	
	fp = fopen(fileName,"a");
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
	
	sleep(5);
	//verify the file can be created
	if(fp == NULL){
		printf("failed to open file");
		return(0);
	} else {
		
		printf("copying file\n");		
		
		
		//receive number of incoming characters
		incFileSize=recv(*socket,filesize,8,0);
		if(incFileSize < 1){
			do{
				printf("didn't receive file size, retrying");
				sleep(1);
				len=recv(*socket,filesize,8,0);
				printf("\n%s\n",filesize);
			} while (incFileSize < 1);
		} else {
            printf("received file size");
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
			printf("len is: %d",len);
			if(len < 0){
				len = 0;
				sleep(1);
			}
			currentIdx += len;
			buffer[len] = '\0';
			fputs(buffer,fp);
			printf("copied: %s\n",buffer);

			printf("%d out of %d\n", currentIdx,lastIdx);
			if(currentIdx == lastIdx){
                break;
            }
			len=recv(*socket,buffer,BUFFERLEN,0);
			//printf("len is: %d\n",len);
	
        }
		
		//printf("last len: %d",len);
		
		printf("file copied");
		//return to to show success
		return(1);
	}
}

char *string_concat_backslash(char *string1, char *string2){
    
    int size_of_strings = strlen(string1) + strlen(string2);

    string1 = realloc (string1, size_of_strings*sizeof(char) + 2);

    memcpy(string1 + strlen(string1), string2, strlen(string2) + 1);
    
    memcpy(string1 + strlen(string1), "\\", sizeof("\\"));

    return string1;
}

char *string_concat(char *string1, char *string2){
    
    int size_of_strings = strlen(string1) + strlen(string2);

    string1 = realloc (string1, size_of_strings*sizeof(char) + 2);

    memcpy(string1 + strlen(string1), string2, strlen(string2) + 1);

    return string1;
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