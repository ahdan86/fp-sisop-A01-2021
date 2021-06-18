#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define SIZE 1024

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

void read_cmd(char *command_cpy){
	char *fullCommand = trimString(command_cpy);
	char command_temp[SIZE]; sprintf(command_temp, "%s", fullCommand);
	char *cmd = trimString(strtok(command_temp, " "));
	int isWrongCmd = 0;
	printf("Command Inserted : %s\n", command_cpy);

	if(!strcmp(cmd, "CREATE")){
		printf("\tCommand : %s\n", cmd);
		char *nxt_cmd = trimString(strtok(NULL, " "));
		printf("\tQuery : %s\n", nxt_cmd);

		if(!strcmp(nxt_cmd, "USER")){
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
			}
			printf("\n");
		}
		else if(!strcmp(nxt_cmd, "DATABASE")){
			char *database = trimString(strtok(NULL, " "));
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
			}
			printf("\n");
		}
		else if(!strcmp(nxt_cmd, "TABLE")){
			char *table_name = trimString(strtok(NULL, " "));
			char *all_columns = trimString(getStrBetween(fullCommand, "(", ")"));
			char *col_name, *col_type, *col_temp;
			printf("\tTable Name : %s\n", table_name);
			if(all_columns && strstr(fullCommand, ";")){
				col_temp = strtok(all_columns, ",");	
				while(col_temp ){
					//buat tabel berdasarkan kolom di sini
					col_temp = trimString(col_temp);
					col_name = trimString(getStrBetween(col_temp, NULL, " "));
					col_type = trimString(getStrBetween(col_temp, col_name, NULL));
					printf("\t\tName | Type : %s | %s\n", col_name, col_type);
					col_temp = strtok(NULL, ",");
				}
			}else isWrongCmd = 1;

			//hasil pengecekan
			if(isWrongCmd){
				printf("\tWrong Command\n");
			}
			printf("\n");
		}
	}
	else if(!strcmp(cmd, "USE")){
		char *database = trimString(strtok(NULL, " "));
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
			}
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

		if(!strcmp(nxt_cmd, "INTO")){
			char *table_name = trimString(strtok(NULL, " "));
			char *all_columns = trimString(getStrBetween(fullCommand, "(", ")"));
			char *col_value;
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
					col_value = trimString(strtok(NULL, ","));
				}
				printf("\n");
			}else isWrongCmd = 1;
		}else isWrongCmd = 1;

		//hasil pengecekan
		if(isWrongCmd){
			printf("\tWrong Command\n");
		}
		printf("\n");
	}
	else if(!strcmp(cmd, "GRANT")){
		char *nxt_cmd = trimString(strtok(NULL, " "));
		char *username, *database,*nxt_cmd2, *nxt_cmd3;
		printf("\tCommand : %s\n", cmd);

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
		}
		printf("\n");
	}
	else if(!strcmp(cmd, "DROP")){
		char *nxt_cmd = trimString(strtok(NULL, " "));
		printf("\tCommand : %s\n", cmd);
		printf("\tQuery : %s\n", nxt_cmd);

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

int main(int argc, char *argv[]){
	char *str = "CREATE USER jack IDENTIFIED BY jack123;";
	read_cmd(str);
	str = "CREATE DATABASE database1;";
	read_cmd(str);
	str = "CREATE TABLE table1 (kolom1 string, kolom2 int, kolom3 string, kolom4 int, kolom5 int);"; 
	read_cmd(str);
	str = "USE database1;"; 
	read_cmd(str);
	str = "DELETE FROM table1;"; 
	read_cmd(str);
	str = "DELETE FROM table1 WHERE kolom1='value1';"; 
	read_cmd(str);
	str = "INSERT INTO table2 ('abc', 1, 'bcd', 2);"; 
	read_cmd(str);
	str = "GRANT PERMISSION database1 INTO user1;"; 
	read_cmd(str);
	str = "DROP   DATABASE     database1;"; 
	read_cmd(str);
	str = "DROP TABLE t able1;"; 
	read_cmd(str);
	str = "DROP COLUMN    kolom1 FROM table1;"; 
	read_cmd(str);
	str = "UPDATE table1 SET kolom1='new_value1';"; 
	read_cmd(str);
	str = "UPDATE table1 SET kolom1='new_value1' WHERE kolom2='new_value2';"; 
	read_cmd(str);
	str = "SELECT * FROM table1;"; 
	read_cmd(str);
	str = "SELECT kolom1, kolom2 FROM table1;"; 
	read_cmd(str);
}
