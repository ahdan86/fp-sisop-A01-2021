#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define SIZE_BUF 100
#define SIZE_CMD 512

int register_login(int fd, char cmd[], char id[], char password[], int isSuperUser){
    int ret_val, isFound = 0;
	char message[SIZE_BUF];
	if(isSuperUser){
		sprintf(id, "%s", "root");
		sprintf(password, "%s", "root");
	}else{
   		 //input/sending id and password
		printf("saya\n");
    	ret_val = send(fd, id, SIZE_BUF, 0);
    	ret_val = send(fd, password, SIZE_BUF, 0);
		ret_val = recv(fd, message, SIZE_BUF, 0);
	}
    printf("\e[32mUsername\n> \e[0m");
	printf("%s\n", id);

    printf("\e[32mPassword\n> \e[0m");
	printf("%s\n", password);
	printf("\n");

	//check if its terminate condition
	if(isSuperUser){
		printf("You are root!\n\n");
		return 1;
	}
	else if(!strcmp(message, "regloginsuccess\n")){
		puts(message);
		return 1;
	}
	else if(!strcmp(message, "wrongpass\n")) {
		printf("Id or Passsword doesn't match !\n\n");
		return 0;
	}
}

void see_books(int fd){
    int ret_val;
    char temp[SIZE_BUF], flag[100];
    int loop = 1;

    ret_val = recv(fd, flag, SIZE_BUF, 0);
    // printf("%s\n", flag);
    puts("");
    while(loop){
        ret_val = recv(fd, temp, SIZE_BUF, 0);
        if(strstr(temp, "done") != NULL){
            loop = 0;
            break;
        }
        printf("Nama: %s\n", temp);

        ret_val = recv(fd, temp, SIZE_BUF, 0);
        printf("Publisher: %s\n", temp);

        ret_val = recv(fd, temp, SIZE_BUF, 0);
        printf("Tahun publishing: %s\n", temp);

        ret_val = recv(fd, temp, SIZE_BUF, 0);
        printf("Esktensi file: %s\n", temp);

        ret_val = recv(fd, temp, SIZE_BUF, 0);
        printf("FilePath: %s\n\n", temp);

    }
}

int getIdPass(int length, char *str[], char id[], char pass[]){
   if(length != 5) return 0;
   if(!strcmp(str[1], "-u") && !strcmp(str[3], "-p")){
      sprintf(id, "%s", str[2]);
      sprintf(pass, "%s", str[4]);
	  return 1;
   }
   return 0;
}

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
        char temp[SIZE_BUF]; 
		sprintf(temp, "[INI_HANYA_HIASAN]%s", str);
        return getStrBetween(temp, "[INI_HANYA_HIASAN]", PATTERN2);
    }else if(PATTERN2 == NULL){
        char temp[SIZE_BUF]; 
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

void read_cmd(int fd, char *command_cpy, char idNow[], char passNow[]){
	char *fullCommand = trimString(command_cpy);
	char command_temp[SIZE_CMD]; sprintf(command_temp, "%s", fullCommand);
	char *cmd = trimString(strtok(command_temp, " "));
	int isWrongCmd = 0;
	printf("Command Inserted : %s\n", command_cpy);

	if(!strcmp(cmd, "CREATE")){
		printf("\tCommand : %s\n", cmd);
		char *nxt_cmd = trimString(strtok(NULL, " "));
		printf("\tQuery : %s\n", nxt_cmd);
		int ret_val = send(fd, "create", SIZE_BUF, 0);

		if(!strcmp(nxt_cmd, "USER")){
			ret_val = send(fd, "user", SIZE_BUF, 0);
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
				printf("\tUsername : %s\n", id);
				printf("\tPassword : %s\n", pass);
				ret_val = send(fd, id, SIZE_BUF, 0);
				ret_val = send(fd, pass, SIZE_BUF, 0);
			}
			char message[SIZE_BUF];
			ret_val = recv(fd, message, SIZE_BUF, 0);
			printf("%s", message);
			printf("\n");
		}
		else if(!strcmp(nxt_cmd, "DATABASE")){
			char *database = trimString(strtok(NULL, " "));
			ret_val = send(fd, "database", SIZE_BUF, 0);
			if(database){
				if(strstr(database, ";")){
					// bikin database
					database = removeSemicolon(database);
				}
				else isWrongCmd = 1;
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}else{
				printf("\tDatabase : %s\n", database);
				ret_val = send(fd, idNow, SIZE_BUF, 0);
				ret_val = send(fd, passNow, SIZE_BUF, 0);
				ret_val = send(fd, database, SIZE_BUF, 0);
			}
			char message[SIZE_BUF];
			ret_val = recv(fd, message, SIZE_BUF, 0);
			printf("%s", message);
			printf("\n");
		}
		else if(!strcmp(nxt_cmd, "TABLE")){
			char *table_name = trimString(strtok(NULL, " "));
			char *all_columns = trimString(getStrBetween(fullCommand, "(", ")"));
			char *col_name, *col_type, *col_temp;
			int ret_val = send(fd, "table", SIZE_BUF, 0);
			ret_val = send(fd, table_name, SIZE_BUF, 0);
			printf("\tTable Name : %s\n", table_name);

			if(all_columns && strstr(fullCommand, ";")){
				col_temp = strtok(all_columns, ",");	
				char *message = "not done";
				while(col_temp ){
					//buat tabel berdasarkan kolom di sini
					col_temp = trimString(col_temp);
					col_name = trimString(getStrBetween(col_temp, NULL, " "));
					col_type = trimString(getStrBetween(col_temp, col_name, NULL));
					printf("\t\tName | Type : %s | %s\n", col_name, col_type);
					col_temp = strtok(NULL, ",");

					ret_val = send(fd, message, SIZE_BUF, 0);
					ret_val = send(fd, col_name, SIZE_BUF, 0);
					ret_val = send(fd, col_type, SIZE_BUF, 0);
				}
				ret_val = send(fd, "done", SIZE_BUF, 0);
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}
			char message[SIZE_BUF];
			ret_val = recv(fd, message, SIZE_BUF, 0);
			printf("%s", message);
			printf("\n");
		}
	}
	else if(!strcmp(cmd, "USE")){
		char *database = trimString(strtok(NULL, " "));
		int ret_val = send(fd, "use", SIZE_BUF, 0);
			if(database){
				if(strstr(database, ";")){
					// bikin database
					database = removeSemicolon(database);
				}
				else isWrongCmd = 1;
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}else{
				printf("\tDatabase : %s\n", database);
				ret_val = send(fd, database, SIZE_BUF, 0);
				ret_val = send(fd, idNow, SIZE_BUF, 0);
				ret_val = send(fd, passNow, SIZE_BUF, 0);
			}
			char message[SIZE_BUF];
			int isPermitted;
			ret_val = recv(fd, message, SIZE_BUF, 0);
			printf("CWD is : %s\n", message);
		printf("\n");
	}
	else if(!strcmp(cmd, "DELETE")){
		char *nxt_cmd = strtok(NULL, " ");
		int isWhere = 0;
		printf("\tCommand : %s\n", cmd);
		if(!strcmp(nxt_cmd, "FROM")){
			char *table, *nxt_cmd2, *col_value, *col_name;
			table = trimString(strtok(NULL, " "));
			if(strstr(table, ";")){
				//delete table doang ga ada kondisi where
				table = removeSemicolon(table);
			}else{
				nxt_cmd2 = trimString(strtok(NULL, " "));
				if(!strcmp(nxt_cmd2, "WHERE")){
					isWhere = 1;
					col_name = trimString(strtok(NULL, "="));
					col_value = trimString(strtok(NULL, " "));
					
					if(col_name && strstr(col_value, ";")){
						col_value = removeSemicolon(col_value);
					}else isWrongCmd = 1;
					
					col_value = removeStrQuotes(col_value);
				}else isWrongCmd = 1;
			}

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}else{
				printf("\tTable : %s\n", table);
				if(isWhere){
					printf("\t\tColumn : %s\n", col_name);
					printf("\t\tValue : %s\n", col_value);
				}
			}
			printf("\n");
		}
	}
	else if(!strcmp(cmd, "UPDATE")){
		char *table = trimString(strtok(NULL, " "));
		int isWhere = 0;
		printf("\tCommand : %s\n", cmd);
		if(table){
			char *nxt_cmd = trimString(strtok(NULL, " "));
			printf("\tQuery : %s\n", nxt_cmd);
			char *nxt_cmd2, *col_value, *col_name, *col_value2, *col_name2;
			//cek kalo format udah sesuai
			if(!strcmp(nxt_cmd, "SET")){
				col_name = trimString(strtok(NULL, "="));
				if(col_name){ 
					if(col_name){
						col_value = trimString(strtok(NULL, " "));
						if(strstr(col_value, ";")){
							//update user  di sini yang tanpa kondisi
							col_value = removeSemicolon(col_value);
							col_value = removeStrQuotes(col_value);


						}else{
							nxt_cmd2 = trimString(strtok(NULL, " "));
							if(nxt_cmd2 != NULL && !strcmp(nxt_cmd2, "WHERE")){
								col_name2 = trimString(strtok(NULL, "="));
								isWhere = 1;
								if(col_name){
									col_value2 = trimString(strtok(NULL, " "));
									if(strstr(col_value2, ";")){
										//update user disini yang ada kondisinya
										col_value2 = removeSemicolon(col_value2);
										col_value2 = removeStrQuotes(col_value);
									}
								}else isWrongCmd = 1;
							}else isWrongCmd = 1;
						}
					}else isWrongCmd = 1;
				}else isWrongCmd = 1;
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}else{
				printf("\tTable : %s\n", table);
				printf("\t\tColumn : %s\n", col_name);
				printf("\t\tValue : %s\n", col_value);
				if(isWhere){
					printf("\t\t\tColumn2 : %s\n", col_name2);
					printf("\t\t\tValue2 : %s\n", col_value2);
				}
			}
			printf("\n");
		}
	}
	else if(!strcmp(cmd, "INSERT")){
		char *nxt_cmd = trimString(strtok(NULL, " "));
		printf("\tCommand : %s\n", cmd);
		int ret_val = send(fd, "insert", SIZE_BUF, 0);

		if(!strcmp(nxt_cmd, "INTO")){
			char *table_name = trimString(strtok(NULL, " "));
			char *all_columns = trimString(getStrBetween(fullCommand, "(", ")"));
			char *col_value;
			char *message = "not done";
			ret_val = send(fd, table_name, SIZE_BUF, 0);

			if(all_columns && strstr(fullCommand, ";")){
				printf("\tQuery : %s\n", nxt_cmd);
				printf("\tTable Name : %s\n", table_name);
				printf("\tALL COLUMN : %s\n", all_columns);
				
				col_value = strtok(all_columns, ",");
				while(col_value){
					//buat tabel berdasarkan kolom di sini
					col_value = trimString(col_value);
					col_value = removeStrQuotes(col_value);
					printf("\t\tValue: %s\n", col_value);
					ret_val = send(fd, message, SIZE_BUF, 0);
					ret_val = send(fd, col_value, SIZE_BUF, 0);

					col_value = trimString(strtok(NULL, ","));
				}
				ret_val = send(fd, "done", SIZE_BUF, 0);
			}else isWrongCmd = 1;
		}else isWrongCmd = 1;

		//hasil pengecekan
		if(isWrongCmd){
			printf("\tWrong Command\n");
		}
		char message[SIZE_BUF];
		ret_val = recv(fd, message, SIZE_BUF, 0);
		printf("%s", message);
		printf("\n");
	}
	else if(!strcmp(cmd, "GRANT")){
		char *nxt_cmd = trimString(strtok(NULL, " "));
		char *username, *database,*nxt_cmd2, *nxt_cmd3;
		printf("\tCommand : %s\n", cmd);
		int ret_val = send(fd, "grant", SIZE_BUF, 0);

		if(!strcmp(nxt_cmd, "PERMISSION")){
			database = trimString(strtok(NULL, " "));
			printf("\tQuery : %s\n", nxt_cmd);
			//cek kalo format udah sesuai
			if(database){
				nxt_cmd2 = trimString(strtok(NULL, " "));
				if(!strcmp(nxt_cmd2, "INTO")){ 
					username = trimString(strtok(NULL, " "));
					if(strstr(username, ";")){
						//grant permissionnya di sini ya
						username = removeSemicolon(username);

					}else isWrongCmd = 1;
				}else isWrongCmd = 1;
			}else isWrongCmd = 1;
		}else isWrongCmd = 1;
		
		//hasil pengecekan
		if(isWrongCmd){
			printf("\tWrong Command\n");
		}else{
			printf("\tDatabase : %s\n", database);
			printf("\tUsername: %s\n", username);
			ret_val = send(fd, database, SIZE_BUF, 0);
			ret_val = send(fd, username, SIZE_BUF, 0);
		}
		char message[SIZE_BUF];
		ret_val = recv(fd, message, SIZE_BUF, 0);
		printf("%s", message);
		printf("\n");
	}
	else if(!strcmp(cmd, "DROP")){
		char *nxt_cmd = trimString(strtok(NULL, " "));
		printf("\tCommand : %s\n", cmd);
		printf("\tQuery : %s\n", nxt_cmd);
		int ret_val = send(fd, "drop", SIZE_BUF, 0);

		if(!strcmp(nxt_cmd, "TABLE")){
			char *table = trimString(strtok(NULL, " "));
			if(table){
				if(strstr(table, ";")){
					// drop tabel
					table = removeSemicolon(table);
				}
				else isWrongCmd = 1;
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(!isWrongCmd){
				printf("\tTable name : %s\n", table);
			}
		}
		else if(!strcmp(nxt_cmd, "DATABASE")){
			char *database = trimString(strtok(NULL, " "));
			if(database){
				if(strstr(database, ";")){
					// drop database
					database = removeSemicolon(database);
				}
				else isWrongCmd = 1;
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(!isWrongCmd){
				printf("\tDatabase : %s\n", database);
			}
		}
		else if(!strcmp(nxt_cmd, "COLUMN")){
			char *col_name = trimString(strtok(NULL, " "));
			if(col_name){
				char *nxt_cmd2 = trimString(strtok(NULL, " "));
				printf("\tQuery : %s\n", nxt_cmd);
				char *table_name;
				//cek kalo format udah sesuai
				if(!strcmp(nxt_cmd2, "FROM")){
					table_name = trimString(strtok(NULL, " "));
					if(table_name){ 
						if(strstr(table_name, ";")){
							//ngapain di sini buat perintah
							table_name = removeSemicolon(table_name);


						}else isWrongCmd = 1;
					}else isWrongCmd = 1;
				}else isWrongCmd = 1;

				//hasil pengecekan
				if(!isWrongCmd){
					printf("\tTable : %s\n", table_name);
					printf("\t\tColumn : %s\n", col_name);
				}
			}
		}else isWrongCmd = 1;

		//hasil pengecekan
		if(isWrongCmd){
			printf("\tWrong Command\n\n");
		}
		printf("\n");
	}

	//yang select masih bug kalo format string tidak sesuai :(, kalo mau ngetes yang ini
	//disesuaiin format yakkkkk
	else if(!strcmp(cmd, "SELECT")){
		char *all_column = getStrBetween(fullCommand, "SELECT ", "FROM");
		char *table;
		int isWhere = 0;
		printf("\tCommand : %s\n", cmd);

		if(all_column){
			char *nxt_cmd = trimString(getStrBetween(fullCommand, all_column, " "));
			printf("\tQuery : %s\n", nxt_cmd);
			if(!strcmp(nxt_cmd, "FROM")){
				if(!strcmp(all_column, "*")){
					// select * noh from table
					table = trimString(getStrBetween(fullCommand, "FROM ", ";"));
					printf("\tAll column : %s\n", all_column);
				}else if(all_column){
					//ini loop kolom
					table = trimString(getStrBetween(fullCommand, "FROM ", ";"));
					printf("\tAll column : %s\n", all_column);
					char *temp = trimString(strtok(all_column, ","));
					while (temp != NULL){
						printf("\t\tColumn : %s\n", temp);
						temp = trimString(strtok(NULL, ","));
					}
				}else
					isWrongCmd = 1;
			}else
				isWrongCmd = 1;
			
			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}else{
				printf("\tTable name : %s\n", table);
			}
			printf("\n");
		}
	}
	else printf("Wrong command\n");
}

int main (int argc, char* argv[]) {
    struct sockaddr_in saddr;
    int fd, ret_val;
    struct hostent *local_host; /* need netdb.h for this */
    char message[SIZE_BUF],  cmd[SIZE_BUF], id[SIZE_BUF], password[SIZE_BUF];
	

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", hstrerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);
    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "connect failed [%s]\n", hstrerror(errno));
        close(fd);
        return -1;
    }

    //TERIMA MSG SERVE / WAIT
    ret_val = recv(fd, message, SIZE_BUF, 0);
    // puts(message);
    while(strcmp(message, "wait") == 0) {
        printf("\e[31mServer is full!\e[0m\n");
        ret_val = recv(fd, message, SIZE_BUF, 0);
    }

    int commandTrue = 0, isSuperUser = 0;
    while(1) {
        // sign up user
        while(!commandTrue) {
			if(!getuid()){
				//super user
				isSuperUser = 1;
				commandTrue = 1;
				sprintf(cmd, "%s", "login root");
				register_login(fd, cmd, id, password, isSuperUser);
				ret_val = send(fd, cmd, SIZE_BUF, 0);
			}else{
				//pengguna remeh temeh
				sprintf(cmd, "%s", "login");
				ret_val = send(fd, cmd, SIZE_BUF, 0);
				if(!getIdPass(argc, argv, id, password)){
					//salah format pas run file
					printf("\nWrong argument!\n");
					return 0;
				} else if(!strcmp(cmd, "login")) {
					//login biasa
					if(register_login(fd, cmd, id, password, isSuperUser))
						commandTrue = 1;
					else
						return 0;
				} else {
					//error handling, gatau, mager ganti kodingan yang lama nanti repot
					//udah pake aja yang penting jalan asw
					ret_val = recv(fd, message, SIZE_BUF, 0);
					if(!strcmp(message, "notlogin\n")) {
						printf("TOLONG REGISTER/ LOGIN DULU Y\n\n");
						return 0;
					} else {
						commandTrue = 1;
					}
				}
			}

        }

        // other command
        while(1){
            printf("\e[32mInsert Command \n>\e[0m ");
			scanf("%[^\n]%*c",cmd);
			read_cmd(fd,cmd, id, password);
		}
        sleep(2);
        if(commandTrue) break;
    }
    printf("\e[31mKAMU KELUAR DARI BELENGGU CLIENT, BYE BYE!\e[0m\n\n");

    /* Last step: close the socket */
    close(fd);
    return 0;
} 
