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


#define MAX_FILE_NAME_LENGTH 100
#define MAX_NUMBER_OF_FOLDERS 500
#define COMLENGTH 30
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

//Creates the socket that will be used to transfer information to and from the client
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

//Binds the socket we created to port 8888
int bind_socket(SOCKET *unbound_socket){
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    int flag;
    flag = bind(*unbound_socket, (SOCKADDR *) &server, sizeof(server));
    if(flag == 0){
        printf("The Socket was Successfully Bound.\n");
        return 1;
    }
    else{
        printf("An Error Occurred when Binding the Socket.\n");
        return 0;
    }
}

//Sets the server to listen for incoming connections
int set_server_to_listen(SOCKET *socket, int number_of_devices){
    
    int flag = listen(*socket, number_of_devices);
    if(flag == 0){
        printf("Server is now listening.\n");
        return 1;
    }
    else{
        printf("Server failed to start listening.\n");
        return 0;
    }
}

//Accepts a socket request and creates a new socket with the client
int accept_socket_request(SOCKET *socket, SOCKET *client){
    
    *client = accept(*socket, NULL, NULL);
    if(*client == INVALID_SOCKET){
        printf("Falied to accept socket.\n");
        return 0;
    }
    else{
        printf("Succesfully accepted socket.\n");
        return 1;
    }
}

//Scans directories for files then sends this information to the client
void scan_directories(char directory_path[], SOCKET *client){
    
    printf("Directory being scanned: %s\n", directory_path);
    
    char list_files_and_folders[MAX_NUMBER_OF_FOLDERS][MAX_FILE_NAME_LENGTH];
    int count = 0;
    DIR *given_directory = opendir(directory_path);
    struct dirent *stream;
    
    //Sleeps to ensure that we are in sync with the client
    sleep(2);
    if (given_directory == NULL)
    {
        printf("Could not open current directory" );
    }

    //Stores directory info in list of files and folders
    while((stream = readdir(given_directory)) != NULL){
        if((strcmp(stream->d_name, ".") != 0) && (strcmp(stream->d_name, "..") != 0)){
            strcpy(list_files_and_folders[count], stream->d_name);
            count++;
        }
    }
    
    printf("Number of folders in directory: %d\n", count);
    
    //Sends all the folders and files in the directory
    for(int i = 0; i < count; i++){
        send(*client, list_files_and_folders[i], 100, 0);
    }
    send(*client, "done", strlen("done"), 0);
    
    printf("Finished sending %s files\n", directory_path);
    closedir(given_directory);
    
}

//Recieves commands from the client to do various things
void recieve_information(SOCKET *client){
    
    char command[2000];
    char filepath[MAX_FILE_NAME_LENGTH];
    char word_search[MAX_FILE_NAME_LENGTH];
    int match = 10;
    int size;
    int tolerance;
    int flag = 0;
    
    //We wait to recieve a command from the server and depending on the command we will do different things
    while(((size = recv(*client , command, 100 , 0)) == -1) || (flag == 0)){
            
        if(size != -1)
        {
            
            //Once we recieve a command we null terminate it and print it 
            command[size] = '\0';
            printf("Recieved Command: %s\n", command);
            
            //We wait in order to synchronise the timings of both the server and client
            sleep(5);
            
            //If the client sends the init command we will send the client the c drive
            if(strcmp(command, "init") == 0){
                
                memset(command, 0, sizeof(command));
                printf("Sending C Drive Information....\n");
                scan_directories("C:\\", client);
                printf("Finished Sending C Drive Information.\n");
            }
            
            //If the client sends the move command we will wait to recieve the directory to move to
            else if(strcmp(command, "move") == 0){
                
                memset(filepath, 0, sizeof(filepath));
                size = recv(*client, filepath, 2000, 0);
                while(size == -1){
                    size = recv(*client, filepath, 2000, 0);
                }
                filepath[size] = '\0';
                printf("Moving to directory: %s\n", filepath);
                sleep(5);
                scan_directories(filepath, client);
            }
            
            //If the client sends the scane command we will send the client the scan exact results
            else if(strcmp(command, "scane") == 0){
                
                //Recieves the filepath in filepath
                memset(filepath, 0, sizeof(filepath));
                size = recv(*client, filepath, 2000, 0);
                while(size == -1){
                    size = recv(*client, filepath, 2000, 0);
                }
                printf("Recieved the file path to the text file: %s\n", filepath);
                
                //Recieves the word to search for in word_search
                memset(word_search, 0, sizeof(word_search));
                size = recv(*client, word_search, 2000, 0);
                while(size == -1){
                    size = recv(*client, word_search, 2000, 0);
                }
                printf("Recieved the word: %s\n", word_search);
                
                //Runs the isExactMatch function to determine if the word is in use or not
                match = isExactMatch(filepath, word_search);
                printf("%d\n", match);
                memset(command, 0, sizeof(command));
                sleep(3);
                
                //Depending on the result we send a message back to the client to tell them the result
                if(match == 1){
                     send(*client, "The word is in the file.", 100, 0);
                }
                else{
                    send(*client, "The word is not in the file.", 100, 0);
                }
            }
            
            //If the client sends the scanr command we run the scan approximate logic
            else if(strcmp(command, "scanr") == 0){
            
                //We change command to the file path of the file we will scan
                memset(filepath, 0, sizeof(filepath));
                size = recv(*client, filepath, 2000, 0);
                while(size == -1){
                    size = recv(*client, filepath, 2000, 0);
                }
                printf("Recieved the file path to the text file: %s\n", filepath);
                
                //We take in the word we are searching for 
                memset(word_search, 0, sizeof(word_search));
                size = recv(*client, word_search, 2000, 0);
                while(size == -1){
                    size = recv(*client, word_search, 2000, 0);
                }
                printf("Recieved the word: %s\n", word_search);
                
                //We reset command then take in the tolerance 
                //we will accept for when searching and then convert this string to an int
                memset(command, 0, sizeof(command));
                size = recv(*client, command, 2000, 0);
                while(size == -1){
                    size = recv(*client, command, 2000, 0);
                }
                tolerance = atoi(command);
                printf("Recieved the tolerance: %d\n", tolerance);
                
                //We then run the isRoughMatch functionthen depending on the results we will send a message back to the client
                match = isRoughMatch(filepath, word_search, tolerance);
                sleep(3);
                if(match == 1){
                     send(*client, "There was a word found that was close.", 100, 0);
                }
                else{
                    send(*client, "There was not a word found that was close.", 100, 0);
                }
            }
            
            //If the client sends the send command we will run the logic to send a file
            else if(strcmp(command, "send") == 0){
                
                //We recieve the file path being sent to us in file path then run the send file function
                memset(filepath, 0, sizeof(filepath));
                size = recv(*client, filepath, 2000, 0);
                while(size == -1){
                    size = recv(*client, filepath, 2000, 0);
                }
                printf("sendfile activated");
                sendFile(filepath, client);
            }
            
            //If the client sends the creat command we run the recieve file function 
            else if(strcmp(command, "creat") == 0){ 
                
                //This will create a file called recieved_file.txt in the path C:\User\Public\Documents from information sent by the client
                receiveFile("C:\\Users\\Public\\Documents\\received_file.txt" ,client);
            }
            
            //If we recieve the exit command we will end the program
            else if(strcmp(command, "exit") == 0){
                
                flag = 1;
            }
        }
    }
}


//returns 1 if word(searchFor) found in file
int isExactMatch(char fileName[], char searchFor[])
{
    FILE *searchThrough = fopen(fileName,"r");

    if (searchThrough == NULL)
    {
		printf("Couldn't open file\n");
        return -1;
	}

    /*make the word that you are looking for all lowercase so it will match the lowercase words as they come through*/
    char keyWord[30];
    strcpy(keyWord,searchFor);
    makeLowercase(keyWord);
    cleanString(keyWord);

    //for the word that will be compared
    word *currentWord = (word*) malloc(sizeof(word));

    do
    {
        //scan the word
        getWord(searchThrough,currentWord);

        //if the file is empty stops
        if(currentWord->thisWord[0]=='\0')
        {
        break;
        }

        //makes the word compareable
        cleanString(currentWord->thisWord);
        makeLowercase(currentWord->thisWord);
    
        printf("Current Word=%s\n",currentWord->thisWord);

        //compare the word, change if match
        if(strcmp(keyWord,currentWord->thisWord)==0)
        {
        printf("Your requested word was found exactly.\n");
        return 1;
        break;
        }


    }while(1);

    fclose(searchThrough);

    return 0;
}

int isRoughMatch(char fileName[], char searchFor[], int characters)
{
    FILE *searchThrough = fopen(fileName,"r");

    if (searchThrough == NULL)
	{
		printf("Couldn't open file\n");
        return -1;
	}

    /*make the word that you are looking for all lowercase so it will match the lowercase words as they come through*/
    char keyWord[30];
    strcpy(keyWord,searchFor);
    makeLowercase(keyWord);
    cleanString(keyWord);

    //for the word that will be compared
    word *currentWord = (word*) malloc(sizeof(word));

    do
    {
        //scan the word
        getWord(searchThrough,currentWord);
    
        //if the file is empty stops
        if(currentWord->thisWord[0]=='\0')
        {
            break;
        }

        //makes the word compareable
        makeLowercase(currentWord->thisWord);
        cleanString(currentWord->thisWord);

        //printf("Current word: %s\n",currentWord.thisWord);

        //compare the word, change if match
        if(strcmp(keyWord,currentWord->thisWord)==0)
        {
            printf("Your requested word was found exactly.\n");
            return 1;
            break;
        }
        if(closeWord(keyWord,currentWord->thisWord,characters)==1)
        {
            printf("There was a close match to your word.\n");
            return 1;
            break;
        }


    }while(1);

    fclose(searchThrough);

    return 0;
}

//makes words all lowercase
void makeLowercase(char word[])
{
    int strLength= strlen(word);

    for(int i=0; i<strLength; i++)
    {
        word[i] = tolower(word[i]);
    }
}

//makes the string into a all lowercase no punctuation or space word.
void cleanString(char string[])
{
    int charsAdded=0;
    char compare;
    for(int i=0;i<strlen(string);i++)
    {
        compare=string[i];
        if(('A'<=compare && compare<='Z') || ('a'<=compare && compare<='z'))
        {
        string[i] = tolower(compare);
        string[charsAdded]=compare;
        charsAdded++;
        }
    }
    string[charsAdded]='\0';
}

//reads a word from the file
int getWord(FILE *searchThrough, word *currentWord)
{

    word placeHolder;
    int spaceIncountered=0;
    int charsAdded=0;

    char currentCharacter;
    currentCharacter=fgetc(searchThrough);

    if(currentCharacter==EOF)
    {
        //printf("adding blank\n");
        placeHolder.thisWord[0]='\0';
        strcpy(currentWord->thisWord, placeHolder.thisWord);
        return 0;
    }
  
    while( currentCharacter != EOF && spaceIncountered==0)
    { 

        if(currentCharacter==32 || currentCharacter==10)
        {
        //printf("SPACE INCOUNTERED\n");
        spaceIncountered=1;
        }
        // if space and first disregarded
        if(spaceIncountered==1 && charsAdded==0)
        {
            //printf("Reseting spaces added\n");
            spaceIncountered=0;
        }
        else if(spaceIncountered==0)
        {
            //printf("Adding: %c\n",currentCharacter);
            placeHolder.thisWord[charsAdded]=currentCharacter;
            charsAdded++;
        }
        if(spaceIncountered==0)
        {
            currentCharacter= fgetc(searchThrough);
        }
    }

    placeHolder.thisWord[charsAdded]='\0';
    strcpy(currentWord->thisWord, placeHolder.thisWord);
    return 0;
}

//Sees if a word has (character) many characters matching
int closeWord(char keyWord[],char currentWord[],int characters)
{
    //printf("New word: %s\n",currentWord);
    int mostConsecutive=0;
    int currentConsecutive=0;

    for(int i=0;i< strlen(keyWord); i++)
    {
        for(int j=0;j<strlen(currentWord);j++)
        {
            if(keyWord[i]==currentWord[j])
            {
                currentConsecutive=continueCheck(&keyWord[i],&currentWord[j]);
                if(currentConsecutive>mostConsecutive)
                {
                    mostConsecutive=currentConsecutive;
                }
            }
        }
    }

    if(mostConsecutive>=characters)
    {
        return 1;
    }

    return 0;
}

//takes a matching character and sees how many match after that
int continueCheck(char keyword[],char currentWord[])
{
    int matchingChars=1;
    int max=0;
    if(strlen(keyword)>strlen(currentWord))
    {
        max=strlen(currentWord);
    }
    else
    {
        max=strlen(keyword);
    }
    for(int i=1; i<max; i++)
    {
        if(keyword[i]==currentWord[i])
        {
            matchingChars++;
        }
        else
        {
            return matchingChars;
        }
    }
    return matchingChars;
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