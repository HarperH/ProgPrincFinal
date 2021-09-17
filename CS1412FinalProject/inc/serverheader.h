typedef struct
{
  char thisWord[30];
}word;


extern int init_winsock(WSADATA *ws);
extern int create_socket(SOCKET *s);
extern int bind_socket(SOCKET *unbound_socket);
extern int set_server_to_listen(SOCKET *socket, int number_of_devices);
extern int accept_socket_request(SOCKET *socket, SOCKET *client);
extern void scan_directories(char directory_path[], SOCKET *client);
extern void recieve_information(SOCKET *client);
extern int isExactMatch(char fileName[], char searchFor[]);
extern int isRoughMatch(char fileName[], char searchFor[],int characters);
extern int getWord(FILE *searchThrough, word *currentWord);
extern void makeLowercase(char word[]);
extern void cleanString(char string[]);
extern int closeWord(char keyWord[],char currentWord[],int characters);
extern int continueCheck(char keyword[],char currentWord[]);
extern int sendFile(char fileName[], SOCKET *socket);
extern int getFileSize(char filename[]);
extern int receiveFile(char fileName[], SOCKET *socket);