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

#define MAX_FILE_NAME_LENGTH 100
#define MAX_NUMBER_OF_FOLDERS 500
#define BUFFERLEN 500


//Initializes winsock which is the socket library we are using for this program
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

//Creates the socket that will be used to transfer information to and from the server
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

//Connects the socket we created to the server 
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

//Allows us to navigate the servers directories and issue commands to the server
void navigate_directories(SOCKET *s){
    
    //Creates the dynamically allocated variable that will be sent to the server to navigate its directories
    char *dir_name;
    dir_name = malloc(4);
    dir_name[0] = 'C';
    dir_name[1] = ':';
    dir_name[2] = '\\';
    dir_name[3] = '\0';
    
    
    int len;
    char *command;
    command = "init";
    char message[MAX_FILE_NAME_LENGTH];
    char list_files_and_folders[MAX_FILE_NAME_LENGTH][MAX_NUMBER_OF_FOLDERS];
    int count = 0;
    int flag = 0;
    int size;
    int selection;
    char word[MAX_FILE_NAME_LENGTH];
    char toler[2];
    
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
    
    //Prints the C drive information that is on the server
    for(int i = 0; i < count; i++){
        if(strcmp(list_files_and_folders[i], "done") == 0){
            count = i;
            break;
        }
        printf("%d: %s\n", i+1, list_files_and_folders[i]);
    }
    
    
    //Issues commands to the server and depending on the command changes what we do inside the if statements
    while(selection != 6){
        printf("\n\nWhat woud you like to do.\n1: Move directories.\n2: Scan contents of a file for information.\n3: Create a file.\n"
                "4: Pull a file.\n5: Display current Directory info.\n6: Exit\n--> ");
        scanf("%d", &selection);
        
        //Will allow us to change directories on the server computer
        if(selection == 1){
            
            //Tells the server we are changing directories
            command = "move";
            send(*s, command, 100, 0);
            
            //Tells the server which folder we would like to go into
            printf("Where would you like to go: (Select -1 to move up a directory): ");
            scanf("%d", &selection);
            
            
            //If selection is -1 we want to move up a directory so how we adjust dir_name will change
            if(selection != -1){
                
                //Sends this path to the server
                string_concat_backslash(dir_name, list_files_and_folders[selection-1]);
                printf("Moving to %s\n", dir_name);
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                //Recieves folder information from the server then populates list_files_and_folders then displays this list
                count = 0;
                size = -1;
                flag = 0;
                printf("Waiting for file information.\n");
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
                
                //Prints the list of files and folders in the directory
                for(int i = 0; i < count; i++){
                    if(strcmp(list_files_and_folders[i], "done") == 0){
                        count = i;
                        break;
                    }
                    printf("%d: %s\n", i+1, list_files_and_folders[i]);
                }
            }
            
            //This else statement allows the user to go up a directory
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
                //Reallocates the directory size
                realloc(dir_name, strlen(dir_name)+1);
                
                //Sends directory to the server
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                //Recieves folder information from the server then populates list_files_and_folders then displays this list
                count = 0;
                size = -1;
                flag = 0;
                printf("Waiting for file information.\n");
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
                
                //Prints the directory
                for(int i = 0; i < count; i++){
                    if(strcmp(list_files_and_folders[i], "done") == 0){
                        count = i;
                        break;
                    }
                    printf("%d: %s\n", i+1, list_files_and_folders[i]);
                }
            }
            
            //Resets selection to one in case the user selected a 6 when moving around
            selection = 1;
        }
        
        //If selection is 2 we run the scan exact and scan approximate logic
        else if(selection == 2){
            
            //Requesting information on whether they want the scan exact or scan approximate file scanning algorithms
            printf("Exact word match or approximate word match (1 for Exact 2 for approximate) : ");
            scanf("%d", &selection);
            
            //If selection is 1 then we run the scan exact 
            if(selection == 1){
                
                //Sends the scan exact command to server 
                command = "scane";
                send(*s, command, 100, 0);
                
                //Requests information on what file they want to scan
                printf("What file would you like to have scanned: ");
                scanf("%d", &selection);
                
                //Appends the file to the directory name and sends it to the server
                string_concat(dir_name, list_files_and_folders[selection-1]);
                printf("Sending to %s\n", dir_name);
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                //Removes the file from the directory name
                len = strlen(dir_name);
                for(int j = len; j > 0; j--){
                    if((dir_name[j-1] != '\\') || (j == len)){
                        dir_name[strlen(dir_name)-1] = '\0';
                    }
                    else{
                        j = 0;
                    }
                }
                
                //Requests information on the word to search for then sends it to the server
                fflush(stdin);
                printf("What word would you like to search for: ");
                fgets(word, sizeof(word), stdin);
                send(*s, word, (int)strlen(word), 0);
                printf("Waiting for results...\n");
                
                //Waits to recieve the results from the server
                size = -1;
                while(size == -1){
                    size = recv(*s, message, 100, 0);
                }
                printf("%s\n", message);
            }
            
            
            //If the selection is not one then we run the scan approximate
            else{
                
                //Sends the scan approximate command to server
                command = "scanr";
                send(*s, command, 100, 0);
                
                //Requests input on what file the user wants to have scanned
                printf("What file would you like to have scanned: ");
                scanf("%d", &selection);
                
                //Appends the file to the directory name and sends it to the server
                string_concat(dir_name, list_files_and_folders[selection-1]);
                printf("Sending to %s\n", dir_name);
                send(*s, dir_name, (int)strlen(dir_name), 0);
                
                //Removes the file name from the directory name
                len = strlen(dir_name);
                for(int j = len; j > 0; j--){
                    if((dir_name[j-1] != '\\') || (j == len)){
                        dir_name[strlen(dir_name)-1] = '\0';
                    }
                    else{
                        j = 0;
                    }
                }
                
                //Requests information on what word the user wants to search for and sends it to the server
                fflush(stdin);
                printf("What word would you like to search for: ");
                fgets(word, sizeof(word), stdin);
                send(*s, word, (int)strlen(word), 0);
                
                //Requests information on the tolerance range the user would like
                printf("How many characters would you like to be consistently matching: ");
                scanf("%s", toler);
                send(*s, toler, (int)strlen(toler), 0);
                printf("Waiting for results...\n");
                
                //Waits for a reply from the server then prints the message the server sends
                size = -1;
                while(size == -1){
                    size = recv(*s, message, 100, 0);
                }
                printf("%s\n", message);
            }
        }
        
        //If the selection is 3 we will create a file on the server that was sent from the client
        else if(selection == 3){
                
                //Sends the command to create a file on the server
                command = "creat";
                send(*s, command, 100, 0);
                
                //Requests information on the file that will be sent to the server
                printf("Enter the file path that you would like to send: ");
                scanf("%s", &message);
                
                //Runs the sendfile function to send the file to the server
                sendFile(message, s);
        }
        
        //If the selection is 4 we will pull a file from the server computer
        else if(selection == 4){
            
            //Sends the send command to the server
            command = "send";
            send(*s, command, 100, 0);
            
            //Requests information on the file that we want to be sent to us then sends this information to the server
            printf("What file would you like to have sent to you: ");
            scanf("%d", &selection);
            string_concat(dir_name, list_files_and_folders[selection-1]);
            printf("Pulling %s\n", dir_name);
            send(*s, dir_name, (int)strlen(dir_name), 0);
            
            //Runs the receive file function that will recieve the file from the server
            receiveFile(list_files_and_folders[selection-1], s);
        }
        
        //If the selection is 5 we will display the current directory again
        else if(selection == 5){
            
            printf("\n\n\n");
            
            //We rerun the logic used to display the directory again
            for(int i = 0; i < count; i++){
                if(strcmp(list_files_and_folders[i], "done") == 0){
                    count = i;
                    break;
                }
                printf("%d: %s\n", i+1, list_files_and_folders[i]);
            }
            
        }
        
        else if(selection == 6){
            command = "exit";
            send(*s, command, 100, 0);
            
        }
    }
}

int receiveFile(char fileName[], SOCKET *socket){
	
	FILE *fp;
	char buffer[BUFFERLEN];
	char filesize[8];
	int incFileSize;
	int n=0;
	int len;
	int currentIdx = 0;
	int lastIdx;
	
	
	fp = fopen(fileName,"a");
	
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
	char buffer[500];
	char filesize[8];
	int fileSizeTest;
	int currentIdx = 0;
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
		printf("sent filesize of: %s\n",filesize);
		
		do{
			
			//get chunk of data from file and add length to idx
			fgets(buffer, 500, fp);
			n = strlen(buffer);
			currentIdx += n;
			
			//send data across stream
			sentBytes = send(*socket , buffer , strlen(buffer) , 0);
			
			printf("sent: %d\n",sentBytes);
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