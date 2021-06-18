#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

#define DATA_BUFFER 50
#define MAX_CONNECTIONS 10 
#define SUCCESS_MESSAGE "Your message delivered successfully"
#define LOGIN_MESSAGE "Id and Password is sent\n"
#define SIZE 100

char *path = "/home/ahdan/FP/fp2/server/USER/akun.txt";
char *path_mk_database = "/home/ahdan/FP/fp2/server/databases/";
char cwd[256];

int create_tcp_server_socket() {
    struct sockaddr_in saddr;
    int fd, ret_val;

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Initialize the socket address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    saddr.sin_addr.s_addr = INADDR_ANY; 

    /* Step2: bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

int check_IdPassDatabase(char id[], char pass[], char database[]){
    char line[512];
    const char delim[5] = "[,]";
	char *tempId, *tempPass, *tempDatabase;

    FILE *fp = fopen(path, "r");
	while(fgets(line, 512, fp)){
		char *newline = strchr( line, '\n' ); //getrid god dang newline
		if ( newline )
			*newline = 0;
		tempId = strtok(line, delim);
		tempPass = strtok(NULL, delim);
        tempDatabase = strtok(NULL, delim);

        if(tempDatabase){
            if(!strcmp(tempId, id) && !strcmp(tempPass, pass) && !strcmp(tempDatabase, database)){
                return 1;
            }
        }
		
	}
	fclose(fp);
	return 0;
}

int check_IdPassword(char id[], char password[], char cmd[]){
	char line[512];
	const char delim[5] = "[,]";
	char *tempId, *tempPass;

	FILE *fp = fopen(path, "r");
	while(fgets(line, 512, fp)){
		char *newline = strchr( line, '\n' ); //getrid god dang newline
		if ( newline )
			*newline = 0;
		tempId = strtok(line, delim);
		tempPass = strtok(NULL, delim);

		if(!strcmp(cmd, "register")){
			if(!strcmp(tempId, id))
				return 1;
		}else{
			if(!strcmp(tempId, id) && !strcmp(tempPass, password))
				return 1;
		}
	}
	fclose(fp);
	return 0;
}

void register_login(int all_connections_i, char cmd[], char id[], char password[], 
                    int *userLoggedIn, int all_connections_serving ){
    int ret_val;
    int status_val;
    if(!strcmp(cmd, "register")) {
        ret_val = recv(all_connections_i, id, SIZE, 0);
        ret_val = recv(all_connections_i, password, SIZE, 0);
        if(check_IdPassword(id, password, cmd)) {
            status_val = send(all_connections_serving,
                    "userfound\n", SIZE, 0);
        } else {
            *userLoggedIn = 1;
            FILE *app = fopen(path, "a+");
            fprintf(app, "%s[,]%s[,]\n", id, password);
            fclose(app);
            status_val = send(all_connections_serving,
                    "regloginsuccess\n", SIZE, 0);
        }
    }else if(!strcmp(cmd, "login")) {
        ret_val = recv(all_connections_i, id, SIZE, 0);
        ret_val = recv(all_connections_i, password, SIZE, 0);
        if(!check_IdPassword(id, password, cmd))
            status_val = send(all_connections_serving,
                    "wrongpass\n", SIZE, 0);
        else {
            *userLoggedIn = 1;
            status_val = send(all_connections_serving,
                    "regloginsuccess\n", SIZE, 0);
        }
    }
}

char *strrev(char *str)
{
    char *p1, *p2;

    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

void see_books(int rcv_clt){
	// char *publisher, *tahun, *filePath, 
	// 		*nameFile, *ekstensi;
	// char  line[512], filePathDir[SIZE], tempName[SIZE];
	// const char tab[2] = "\t";
	// int status_val;

	// FILE *fp = fopen("files.tsv", "r");
	// status_val = send(rcv_clt, "not-done", SIZE, 0);

	// while(fgets(line, 512, fp)){
	// 	char *newline = strchr( line, '\n' ); //getrid god dang newline
	// 	if ( newline )
	// 		*newline = 0;

	// 	filePath = strtok(line, tab);
	// 	publisher = strtok(NULL, tab);
	// 	tahun = strtok(NULL, tab);

	// 	sprintf(filePathDir, "%s", filePath);
	// 	getDir(filePathDir, tempName);
	// 	strrev(tempName);
	// 	nameFile = strtok(tempName, ".");
	// 	ekstensi = strtok(NULL, ".");

	// 	printf("Nama: %s\n", nameFile);
	// 	printf("Publisher: %s\n", publisher);
	// 	printf("Tahun publishing: %s\n", tahun);
	// 	printf("Esktensi file: %s\n", ekstensi);
	// 	printf("FilePath: %s\n\n", filePath);

	// 	status_val = send(rcv_clt, nameFile, SIZE, 0);
	// 	status_val = send(rcv_clt, publisher, SIZE, 0);
	// 	status_val = send(rcv_clt, tahun, SIZE, 0);
	// 	status_val = send(rcv_clt, ekstensi, SIZE, 0);
	// 	status_val = send(rcv_clt, filePath, SIZE, 0);
	// }
	// fclose(fp);
	// status_val = send(rcv_clt, "done", SIZE, 0);
}

void create(int all_connections_i, int isSuperUser){
    char query[SIZE];
    int ret_val = recv(all_connections_i, query, SIZE, 0);
    // printf("query %s\n", query);
    
    if(!strcmp(query, "user")){
        char id[SIZE], pass[SIZE];
        ret_val = recv(all_connections_i, id, SIZE, 0);
        ret_val = recv(all_connections_i, pass, SIZE, 0);
        printf("%s -> %s %s\n", query, id, pass);
        if(isSuperUser){
            FILE *app = fopen(path, "a+");
            fprintf(app, "%s[,]%s[,]\n", id, pass);
            fclose(app);
            ret_val = send(all_connections_i, "user created\n", SIZE, 0);
        }else
            ret_val = send(all_connections_i, "you are not root user\n", SIZE, 0);
        
    } else if(!strcmp(query, "database")){
        char database[SIZE], id[SIZE], pass[SIZE], temp[SIZE];
        int ret_val;
        ret_val = recv(all_connections_i, id, SIZE, 0);
        ret_val = recv(all_connections_i, pass, SIZE, 0);
        ret_val = recv(all_connections_i, database, SIZE, 0);
        sprintf(temp, "%s%s", path_mk_database, database);
        mkdir(temp, 0777);
        FILE *app = fopen(path, "a+");
        fprintf(app, "%s[,]%s[,]%s[,]\n", id, pass, database);
        fclose(app);
        ret_val = send(all_connections_i, "database created!\n", SIZE, 0);
    } else if(!strcmp(query, "table")){
        char table[SIZE], col_name[SIZE], col_type[SIZE], message[SIZE], tempMkTable[512];
        int ret_val;
        ret_val = recv(all_connections_i, table, SIZE, 0);
        sprintf(tempMkTable, "%s/%s", cwd, table);
        printf("table %s\n", tempMkTable);
        FILE *fp = fopen(tempMkTable, "w+");

        while(1){
            ret_val = recv(all_connections_i, message, SIZE, 0);
            if(!strcmp(message, "done"))
                break;
            ret_val = recv(all_connections_i, col_name, SIZE, 0);
            ret_val = recv(all_connections_i, col_type, SIZE, 0);
            printf("DEBUG --- %s : %s %s\n", message, col_type, col_name);
            fprintf(fp, "%s %s[,]", col_name, col_type);
        }
        fprintf(fp, "\n");
        fclose(fp);
        ret_val = send(all_connections_i, "Table created!\n", SIZE, 0);
    }
}

void grant(int all_connectionss_i, int isSuperUser){
    char database[SIZE], username[SIZE], line[512];
    int ret_val, isFound = 0;
    const char delim[5] = "[,]";
    char *tempId, *tempPass;

    ret_val = recv(all_connectionss_i, database, SIZE, 0);
    ret_val = recv(all_connectionss_i, username, SIZE, 0);

    if(isSuperUser){
        FILE *fp = fopen(path, "a+");
        while(fgets(line, 512, fp)){
            char *newline = strchr( line, '\n' ); //getrid god dang newline
            if ( newline )
                *newline = 0;
            if(strstr(line, username)){
                isFound = 1;
                tempId = strtok(line, delim);
                tempPass = strtok(NULL, delim);
                if(!check_IdPassDatabase(tempId, tempPass, database)){
                    fprintf(fp,"%s[,]%s[,]%s[,]\n", tempId, tempPass, database);
                }else
                    break;
            }
        }
	    fclose(fp);
        if(!isFound)
            ret_val = send(all_connectionss_i, "Permission not granted(not found)\n", SIZE, 0);
        else
            ret_val = send(all_connectionss_i, "Permission granted\n", SIZE, 0);

    }else
       ret_val = send(all_connectionss_i, "Permission not granted(not root user)\n", SIZE, 0);  
}

void use(int all_connections_i, int isSuperUser){
    char id[SIZE], pass[SIZE], database[SIZE];
    int ret_val, isPermitted = 0;

    ret_val = recv(all_connections_i, database, SIZE, 0);
    ret_val = recv(all_connections_i, id, SIZE, 0);
    ret_val = recv(all_connections_i, pass, SIZE, 0);

    // printf("debug -> %s %s %s\n", id, pass, database);
    if(!isSuperUser){
        if(check_IdPassDatabase(id, pass, database)){
            sprintf(cwd, "%s%s", path_mk_database, database);
            isPermitted = 1;
        }
    }else{
        isPermitted = 1;
        sprintf(cwd, "%s%s", path_mk_database, database);
    }
    printf("CWD --- %s\n", cwd);
    if(isPermitted)
        ret_val = send(all_connections_i, cwd, SIZE, 0);
    else
        ret_val = send(all_connections_i, "Permission is not found\n", SIZE, 0);
}

void dropTable(int all_connections_i, int isSuperUser){
    char table[SIZE];
    char temp[SIZE];
    printf("drop cwd = %s\n",cwd);
    if(!strcmp(cwd,"/home/ahdan/FP/fp2/server/databases/"))
    {
        int retvalFailed = send(all_connections_i, "Belum ada DB yang di USE\n", SIZE, 0);
        return;
    }
    strcpy(temp, cwd);
    int flag =0;
    int retval = recv(all_connections_i, table, SIZE, 0);
    printf("drop table = %s\n",table);
    DIR *d;
    struct dirent *dir;
    d = opendir(cwd);
    if(d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(!strcmp(dir->d_name,".")||!strcmp(dir->d_name,".."))
            {
                continue;
            }

            else if(!strcmp(dir->d_name,table))
            {
                flag = 1;
                strcat(temp,"/");
                strcat(temp,dir->d_name);
                printf("Tabel to Drop = %s\n",temp);
                remove(temp);
                break;
            }
        }
    }
    if(flag == 1)
    {
        int retval2=send(all_connections_i, "DROP Table Success\n", SIZE, 0);
    }
    else{
        int retval2=send(all_connections_i, "Table Not Found!\n", SIZE, 0);
    }
}

void dropColumn(int all_connections_i, int isSuperUser)
{
    int tag=0;
    char table[SIZE];
    char column[SIZE];
    char temp[SIZE];
    char temp1[SIZE];
    printf("cwd = %s\n",cwd);
    strcpy(temp,cwd);
    strcpy(temp1,cwd);
    if(!strcmp(cwd,"/home/ahdan/FP/fp2/server/databases/"))
    {
        int retvalFailed = send(all_connections_i, "Belum ada DB yang di USE\n", SIZE, 0);
        return;
    }
    strcat(temp,table);
    strcat(temp1,"temp");
    int retvalTable = recv(all_connections_i, table, SIZE, 0);
    int retvalColumn = recv(all_connections_i, column, SIZE, 0);
    
    FILE *tableChoose = fopen(temp, "r+");
    FILE *temp2 = fopen(temp1,"w+");
    char line[252], *temp3;
    char temp4[252];
    int i=1;
    while(fgets(line, 252, tableChoose)!=0)
    {
        if(strstr(line, column))
        {
            temp3 = strtok(line, "[,]");
            if(strstr(temp3,column))
            {
                tag=1;
                break;
            }
            while(temp3!=NULL){
                printf("%s\n",temp3);
                temp3 = strtok(NULL, "[,]");
                if(strstr(temp3,column))
                {
                    printf("%s\n",temp3);
                    i++;
                    tag=1;
                    break;
                }
                i++;
            }
            printf("i = %d\n",i);
        }
    }
    if(tag==0)
    {
        int retvalMSG=send(all_connections_i,"Tidak ada column ditemukan",SIZE, 0);
        return;
    }
    rewind(tableChoose);
    while(fgets(temp4, 252, tableChoose)!=0)
    {
        char *newline = strchr(temp4,'\n');
        if(newline)
            *newline=0;
        int j=1;
        temp3 = strtok(temp1, "[,]");
        char *delimit = "[,]";
        printf("temp3 = %s\n",temp3);
        while(temp3 != NULL)
        {
            if(j!=i)
            {
                printf("temp3 = %s\n",temp3);
                fprintf(temp2,"%s%s", temp3, delimit);
                temp3 = strtok(NULL, "[,]");
            }
            else{
                temp3 = strtok(NULL, "[,]");
            }
            j++;
        }
        fprintf(temp2,"\n");
    }

    fclose(tableChoose);
    fclose(temp2);
    int retvalMsg = send(all_connections_i,"Column berhasil di drop",SIZE,0);
}

int deleteTxt(char filename[])
{
    char temp1[SIZE];
    strcpy(temp1, "/home/ahdan/FP/fp2/server/USER/");
    strcat(temp1, "temp.txt");
    FILE *tsv = fopen(path, "r+");
    FILE *tmp = fopen(temp1, "w+");
    char temp[256], line[256];

	while(fgets(line, 256, tsv) != 0){
        if(sscanf(line, "%255[^\n]", temp) != 1) 
			break;
        if(strstr(temp, filename) == 0) 
            fprintf(tmp, "%s\n", temp);
    }

    while(fgets(line, 256, tsv) != 0){
        if(sscanf(line, "%255[^\n]", temp) != 1) break;
        fprintf(tsv, "%s\n", temp);
    }
    remove(path);
    rename(temp1, path);

    fclose(tmp);
    fclose(tsv);
    return 0;  
}

void dropDB(int all_connections_i, int isSuperUser){
    int flag = 0;
    char database[SIZE];
    char idNow[SIZE];
    char passNow[SIZE];
    int retvalDB = recv(all_connections_i, database, SIZE, 0);
    int retvalId = recv(all_connections_i, idNow, SIZE, 0);
    int retvalPass = recv(all_connections_i, passNow, SIZE, 0);

    if(!check_IdPassDatabase(idNow,passNow,database) && !isSuperUser)
    {
        int retvalMsg = send(all_connections_i, "Tidak ada permission\n", SIZE, 0);
        return;
    }
    else{
        char temp[SIZE];
        strcpy(temp, path_mk_database);
        strcat(temp, database);
        int flag=0;
        DIR *d;
        struct dirent *dir;
        d = opendir(path_mk_database);
        while ((dir = readdir(d)) != NULL)
        {
            if(dir->d_type==DT_DIR)
            {
                if(!strcmp(dir->d_name, database))
                {
                    flag=1;
                    printf("DB to Drop = %s\n",temp);
                    remove(temp);
                    break;
                }
            }
        }

        int check = deleteTxt(database);
    }
    if(flag == 1)
    {
        int retval2=send(all_connections_i, "DROP DB Success\n", SIZE, 0);
    }
    else{
        int retval2=send(all_connections_i, "DB Not Found!\n", SIZE, 0);
    }
}

void insert(int all_connections_i, int isSuperUser){
    char table[SIZE], col_value[SIZE], message[SIZE], tempMkTable[512];
    int ret_val;
    ret_val = recv(all_connections_i, table, SIZE, 0);
    sprintf(tempMkTable, "%s/%s", cwd, table);
    printf("table %s\n", tempMkTable);
    FILE *fp = fopen(tempMkTable, "a+");

    while(1){
        ret_val = recv(all_connections_i, message, SIZE, 0);
        if(!strcmp(message, "done"))
            break;
        ret_val = recv(all_connections_i, col_value, SIZE, 0);
        printf("DEBUG --- %s : %s\n", message, col_value);
        fprintf(fp, "%s[,]", col_value);
    }
    fprintf(fp, "\n");
    fclose(fp);
    ret_val = send(all_connections_i, "Table inserted!\n", SIZE, 0);
}


int main (int argc, char* argv[]) {
    fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, i, serving = 1, isSuperUser = 0;
    int ret_val, ret_val1, ret_val2, ret_val3, ret_val4, status_val;
    char message[SIZE], id[SIZE], password[SIZE], cmd[SIZE];
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    int all_connections[MAX_CONNECTIONS];
    
    //make necessary files
	if(access(path, F_OK ) != 0 ) {
		FILE *fp = fopen(path, "w+");
		fclose(fp);
	} 
    if(access("files.tsv", F_OK ) != 0 ) {
		FILE *fp = fopen("files.tsv", "w+");
		fclose(fp);
	}
	if(access("running.log", F_OK ) != 0 ) {
		FILE *fp = fopen("running.log", "w+");
		fclose(fp);
	} 
    /* Get the socket server fd */
    server_fd = create_tcp_server_socket(); 
    if (server_fd == -1) {
        fprintf(stderr, "Failed to create a server\n");
        return -1; 
    }   

    /* Initialize all_connections and set the first entry to server fd */
    for (i=0;i < MAX_CONNECTIONS;i++) {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

	printf("\nServer is running....\n\n");
    int userLoggedIn = 0;
    while (1) {
        FD_ZERO(&read_fd_set);
        /* Set the fd_set before passing it to the select call */
        for (i=0;i < MAX_CONNECTIONS;i++) {
            if (all_connections[i] >= 0) {
                FD_SET(all_connections[i], &read_fd_set);
            }
        }

        /* Invoke select() and then wait! */
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events */
        if (ret_val >= 0 ) {
            /* Check if the fd with event is the server fd */
            if (FD_ISSET(server_fd, &read_fd_set)) { 
                /* accept the new connection */
                new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    printf("Accepted a new login try:\n\n");
                    for (i=0;i < MAX_CONNECTIONS;i++) {
                        if (all_connections[i] < 0) {
                            all_connections[i] = new_fd;
                            if(i != serving) {
                                ret_val1 = send(all_connections[i], "wait",  SIZE, 0);
                            } else {
                                ret_val1 = send(all_connections[i], "serve",  SIZE, 0);
                            }
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
                ret_val--;
                if (!ret_val) continue;
            } 

            /* Check if the fd with event is a non-server fd */
            // step2
            for (i=1;i < MAX_CONNECTIONS;i++) {
                if ((all_connections[i] > 0) &&
                    (FD_ISSET(all_connections[i], &read_fd_set))) {
                    // read command from client 
                    ret_val3 = recv(all_connections[i], cmd, sizeof(cmd), 0);

                    printf("Returned fd is %d [index, i: %d]\n", all_connections[i], i);
                    printf("Command : %s\n", cmd);
                    //check if client terminate
                    if (ret_val1 == 0 || ret_val2 == 0 || ret_val3 == 0 ) {
                        printf("Id of the user now : %s\n", id);
                        printf("Password of the user now : %s\n", password);
                        printf("Closing connection for fd:%d\n\n", all_connections[i]);
                        close(all_connections[i]);
                        all_connections[i] = -1; /* Connection is now closed */

                        //make another client wait while a client is loggein
                        while(1) {
                            if(serving == 9) {
                                serving = 1;
                                break;
                            }
                            if(all_connections[serving + 1] != -1) {
                                serving++;
                                break;
                            }
                            serving++;
                        }
                        userLoggedIn = 0;
                        if(all_connections[serving] != -1)
                            status_val = send(all_connections[serving], "serve",  SIZE, 0);
                    } 
                    if (ret_val3 > 0) {
                        // signing up
                        if(!strcmp(cmd, "login"))
                              register_login(all_connections[i], cmd, id, password, &userLoggedIn, 
                                            all_connections[serving] );
                        else if(!strcmp(cmd, "login root")){
                            // BUAT BEDAIN DIA MAKE ROOT ATAU NGGA MAKE VARIABEL 
                            //iSuperUser Y
                            userLoggedIn = 1;
                            isSuperUser = 1;
                            sprintf(id, "%s", "root");
                            sprintf(password, "%s", "root");
                        }
                        // other command
                        else {
                            if(userLoggedIn) {
                                printf("Kamu berhak mengakses command\n\n");
								//KALO MAU BUAT PERINTAH DISINI GES
                                //INI DAH ADA CONTOH FUNGSI SEE 
                                //KALO BINGUNG TANYA AJA Y

                                //create command
                                if(!strcmp(cmd, "create")){
									create(all_connections[serving], isSuperUser);
                                }
                                //grant command
                                else if(!strcmp(cmd, "grant")){
									grant(all_connections[serving], isSuperUser);
                                }
                                //use command
                                else if(!strcmp(cmd, "use")){
                                    use(all_connections[serving], isSuperUser);
                                }
                                //insert command
                                else if(!strcmp(cmd, "insert")){
                                    insert(all_connections[serving], isSuperUser);
                                }
                                else if(!strcmp(cmd, "drop")){
                                    char cmdLanjut[SIZE];
                                    int retvalDrop = recv(all_connections[serving],cmdLanjut, SIZE, 0);
                                    if(!strcmp(cmdLanjut,"table"))
                                    {
                                        dropTable(all_connections[serving], isSuperUser);
                                    }
                                    else if( !strcmp(cmdLanjut,"database"))
                                    {
                                        dropDB(all_connections[serving], isSuperUser);
                                    }
                                    else if( !strcmp(cmdLanjut,"column"))
                                    {
                                        dropColumn(all_connections[serving], isSuperUser);
                                    }
                                }

                            } else {
								//user not login
                                status_val = send(all_connections[serving],
                                        "notlogin\n", SIZE, 0);
                                continue;
                            }
                        }
                        
                        printf("Id of the user now : %s\n", id);
                        printf("Password of the user now : %s\n\n", password);
                    } 
                    if (ret_val1 == -1 || ret_val3 == -1) {
                        printf("recv() failed for fd: %d [%s]\n", 
                            all_connections[i], strerror(errno));
                        break;
                    }
                }
                ret_val1--;
                ret_val2--;
                ret_val3--;
                if (!ret_val1) continue;
                if (!ret_val2) continue;
                if (!ret_val3) continue;
            }
		} 
    }

    /* Last step: Close all the sockets */
    for(i=0;i < MAX_CONNECTIONS;i++) {
        if (all_connections[i] > 0) {
            close(all_connections[i]);
        }
    }

    return 0;
}
