extern int init_winsock(WSADATA *ws);
extern int create_socket(SOCKET *s);
extern int connect_to_server(SOCKET *s);
extern void navigate_directories(SOCKET *s);
extern int receiveFile(char fileName[], SOCKET *socket);
extern char *string_concat(char *string1, char *string2);
extern char *string_concat_backslash(char *string1, char *string2);
extern int sendFile(char fileName[], SOCKET *socket);
extern int getFileSize(char filename[]);