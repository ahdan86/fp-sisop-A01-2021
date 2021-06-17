#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#define PORT 8080
#define SIZE 1024

typedef struct account{
    char id[1024];
    char pass[1024];
}akun;

char *trimString(char *str)
{
    if(str == NULL) return NULL;
	size_t len = strlen(str);

    while(isspace(str[len - 1])) --len;
    while(*str && isspace(*str)) ++str, --len;

    return strndup(str, len);
}

char *getStrBetween(char *str, char *PATTERN1, char *PATTERN2){
    if(PATTERN1 == NULL){
        char temp[SIZE]; 
		sprintf(temp, "[INI_HANYA_HIASAN]%s", str);
        return getStrBetween(temp, "[INI_HANYA_HIASAN]", PATTERN2);
    }else if(PATTERN2 == NULL){
        char temp[SIZE]; 
		sprintf(temp, "%s[INI_HANYA_HIASAN]", str);
        return getStrBetween(temp, PATTERN1, "[INI_HANYA_HIASAN]");
    }

    // printf("\tPATTERN1 : [%s]\n", PATTERN1);
    // printf("\tPATTERN2 : [%s]\n", PATTERN2);

    char *target = NULL;
    char *start, *end;

    if ( start = strstr( str, PATTERN1 ) ){
        start += strlen( PATTERN1 );
        if ( end = strstr( start, PATTERN2 ) ){
            target = ( char * )malloc( end - start + 1 );
            memcpy( target, start, end - start );
            target[end - start] = '\0';
        }
    }
    if(target == NULL){
        return NULL;
    }else{
        return target;
    }
}

char* removeStrQuotes(char* str){
    if(str[0] == '\'' && str[strlen(str)-1] == '\''){
        return getStrBetween(str, "'", "'");
    }else if(str[0] == '"' && str[strlen(str)-1] == '"'){
        return getStrBetween(str, "\"", "\"");
    }else if(str[0] == '`' && str[strlen(str)-1] == '`'){
        return getStrBetween(str, "`", "`");
    }else{
        return str;
    }
}

char* removeSemicolon(char *str){
	if(str == NULL);
	if(str[strlen(str)-1] == ';')
		return getStrBetween(str, NULL, ";");
	return str;
}

void read_cmd(char *command_cpy, bool isSudo, int sock){
    char *fullCommand = trimString(command_cpy);
	char command_temp[SIZE]; sprintf(command_temp, "%s", fullCommand);
	char *cmd = trimString(strtok(command_temp, " "));
	int isWrongCmd = 0;
	printf("Command Inserted : %s\n", command_cpy);

	if(!strcmp(cmd, "CREATE")){
		printf("\tCommand : %s\n", cmd);
		char *nxt_cmd = trimString(strtok(NULL, " "));
		printf("\tQuery : %s\n", nxt_cmd);

		if(!strcmp(nxt_cmd, "USER") && isSudo){
			char *id = trimString(strtok(NULL, " "));
			char *pass, *nxt_cmd2, *nxt_cmd3;
			//cek kalo format udah sesuai
			if(id && strcmp(id, "IDENTIFIED")){
				nxt_cmd2 = trimString(strtok(NULL, " "));
				if(!strcmp(nxt_cmd2, "IDENTIFIED")){ 
					nxt_cmd3 = trimString(strtok(NULL, " "));
					if(!strcmp(nxt_cmd3, "BY")){
						//bikin user baru di sini
						pass = trimString(strtok(NULL, " "));
						if(strstr(pass, ";"))
							pass = removeSemicolon(pass);
						else isWrongCmd = 1;
					}else isWrongCmd = 1;
				}else isWrongCmd = 1;
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}else{
				akun newAkun;
                printf("\tUsername : %s\n", id);
                strcpy(newAkun.id,id);
				printf("\tPassword : %s\n", pass);
                strcpy(newAkun.pass,pass);
                send(sock, (void *)&newAkun, sizeof(newAkun), 0);
			}
			printf("\n");
		}
    }
}

int main(int argc, char* argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    bool command = false;
    int status;
    status=getuid();
    while(true)
    {
        if(status==0)
        {
            // printf("ini root\n");
            char *str=malloc(1024);
            scanf("%[^\n]%*c",str);
            read_cmd(str,true,sock);
            free(str);
        }
        else{
            // printf("ini bukan\n");
            //username -> argv[2]

        }
    }
}