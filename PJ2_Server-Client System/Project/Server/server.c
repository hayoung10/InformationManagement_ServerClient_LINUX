#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

#define MAX_SOCKET 2
int sendnum = 0, screen = 0;
char *id, *pw, *win, *lose, *name, *fname;
char *gameid, *gamepw, *gamewin, *gamelose, *loginid1, *loginid2;

typedef struct Checking {
	int scr_num;
	char id[20];
        char pw[20];
	char win[20];
	char lose[20];
	int check; // 0이면 OK, 1이면 not OK
} Checking;

int nFds, nChats = 0;
int clientSockets[MAX_SOCKET];
int gconnect[MAX_SOCKET];
int listenSocket;

int getMax();
void addClient(int s, struct sockaddr_in* clientAddr);
void removeClient(int s);
int tcpListen(int host, int port, int backlog);

int signup() { // 회원가입 확인
        int fd1, exist;
        char buf[1024], *info;
        memset(name, 0x00, strlen(name)); // name 초기화
        memset(fname, 0x00, strlen(fname)); // fname 초기화
        info = (char *)malloc(sizeof(char) * 256);

        strcpy(name,"./Information/");
        strcat(name,id);
        strcat(name,".txt");
        strcpy(fname,id);
        strcat(fname,".txt");

        struct dirent *direntp;
        DIR *dirp;
        if ((dirp = opendir("Information")) == NULL)
                return 1;
        while ((direntp = readdir(dirp)) != NULL) {
                if (strcmp(direntp->d_name,fname) == 0) { // 계정존재 여부 확인
                        exist = 1;
                        break;
                } else
                        exist = 0;
        }

	if (exist == 0) {
                if ((fd1 = open(name, O_RDWR | O_CREAT | O_APPEND, 0666)) != -1) {
                        memset(info, 0x00, strlen(info)); // info 초기화
                        strcpy(info,id);
                        info[strlen(info)] = ':';
                        strcat(info,pw);
                        info[strlen(info)] = ':';
                        strcat(info,"0"); // win = 0
                        info[strlen(info)] = ':';
                        strcat(info,"0"); // lose = 0
                        info[strlen(info)] = ':';
                        if (write(fd1,info,strlen(info)) != -1)
				sendnum = 1;
                }
        } else if (exist == 1)
		sendnum = 2;

        close(fd1);
        closedir(dirp);
        free(info);

        return 0;
}

int signin() { // 로그인 확인
        int fd1, exist = 0, idcheck = 0, pwcheck = 0;
        char sbuf[1024], *check;
        struct dirent *direntp;
        DIR *dirp;

        memset(name, 0x00, strlen(name)); // name 초기화
        memset(fname, 0x00, strlen(fname)); // fname 초기화
        strcpy(name,"./Information/");
        strcat(name,id);
        strcat(name,".txt");
        strcpy(fname,id);
        strcat(fname,".txt");

        if ((dirp = opendir("Information")) == NULL) // 계정 존재 여부 확인
                return 1;
        while ((direntp = readdir(dirp)) != NULL) {
                if (strcmp(direntp->d_name,fname) == 0) {
                        exist = 1;
                        break;
                } else
                        exist = 0;
        }

	if (strcmp(loginid1,id) == 0) { // 이미 로그인한 id
		exist = 2;
		sendnum = 2;
	}

	if (strcmp(loginid2,id) == 0) { // 이미 로그인한 id
		exist = 2;
		sendnum = 2;
	}

        memset(win, 0x00, strlen(win)); // win 초기화
        memset(lose, 0x00, strlen(lose)); // lose 초기화
        if (exist == 1) { // 계정이 존재하면
                if ((fd1 = open(name, O_RDONLY)) != -1) {
                        if (read(fd1,sbuf,sizeof(sbuf)) != -1) {
                                check = strtok(sbuf,":");
                                if (strcmp(check,id) == 0) // id 확인
                                        idcheck = 1;
				check = strtok(NULL,":");
                                if (strcmp(check,pw) == 0) // pw 확인
                                        pwcheck = 1;
                                if ((idcheck == 1) && (pwcheck == 1)) {
                                        check = strtok(NULL,":");
                                        strcpy(win,check);
                                        check = strtok(NULL,":");
                                        strcpy(lose,check);
                                        sendnum = 1;
                                } else
                                        sendnum = 2;
                        }
                }
        } else
                sendnum = 2;
        close(fd1);
        closedir(dirp);

        return 0;
}

int loginname(int s, int checknum) { // 정상적으로 로그인된 계정 기억하기
	if (s == 0) {
		memset(loginid1, 0x00, strlen(loginid1));
		if (checknum == 1)
			strcpy(loginid1, id);
	} else if (s == 1) {
		memset(loginid2, 0x00, strlen(loginid2));
		if (checknum == 1)
			strcpy(loginid2, id);
	}

	return 0;
}

int withdr() { // 회원 탈퇴 확인
        int fd1, exist, idcheck = 0, pwcheck = 0, clear, status;
        char buf[1024], *check;
        struct dirent *direntp;
        DIR *dirp;
        pid_t pid;

        memset(name, 0x00, strlen(name)); // name 초기화
        memset(fname, 0x00, strlen(fname)); // fname 초기화
        strcpy(name,"./Information/");
        strcat(name,id);
        strcat(name,".txt");
        strcpy(fname,id);
        strcat(fname,".txt");

        if ((dirp = opendir("Information")) == NULL) // 계정 존재 여부 확인
                return 1;
        while ((direntp = readdir(dirp)) != NULL) {
                if (strcmp(direntp->d_name,fname) == 0) {
                        exist = 1;
                        break;
                } else
                        exist = 0;
        }

	if (exist == 1) { // 계정이 존재하면
                if ((fd1 = open(name, O_RDONLY)) != -1) {
                        if (read(fd1,buf,sizeof(buf)) != -1) {
                                check = strtok(buf,":");
                                if (strcmp(check,id) == 0) // id 확인
                                        idcheck = 1;
                                check = strtok(NULL,":");
                                if (strcmp(check,pw) == 0) // pw 확인
                                        pwcheck = 1;
					//wrefresh(winb);
                        }
                }
                if ((idcheck == 1) && (pwcheck == 1)) {
                        pid = fork();
                        if (pid == -1)
                                return 1;
                        else if (pid == 0)
                                execl("/bin/rm","rm","-rf",name,NULL);
                        else
                                wait(&status);
                        if ((dirp = opendir("Information")) == NULL) { // 계정 존재 여부 확인
                                return 1;
                        }
                        while ((direntp = readdir(dirp)) != NULL) {
                                if (strcmp(direntp->d_name,fname) == 0) {
                                        clear = 1;
                                        break;
                                } else
                                        clear = 0;
                        }
                        if (clear == 0)
				sendnum = 1;

			memset(id, 0x00, strlen(id)); // id 초기화
                        memset(pw, 0x00, strlen(pw)); // pw 초기화
                        memset(win, 0x00, strlen(win)); // win 초기화
                        memset(lose, 0x00, strlen(lose)); // lose 초기화
                } else
			sendnum = 2;
        } else if (exist == 0)
                sendnum = 2;

        close(fd1);
        closedir(dirp);

        return 0;
}

int game() {
	int waiting = 0;
	for (int i = 0; i < nChats; i++)
		if (gconnect[i] == 2)
			waiting++;
	if (waiting == 1) {
		memset(gameid, 0x00, strlen(gameid));
		memset(gamepw, 0x00, strlen(gamepw));
		memset(gamewin, 0x00, strlen(gamewin));
		memset(gamelose, 0x00, strlen(gamelose));
		strcpy(gameid, id);
		gameid[strlen(gameid)] = ':';
		strcpy(gamepw, pw);
		gamepw[strlen(gamepw)] = ':';
		strcpy(gamewin, win);
		gamewin[strlen(gamewin)] = ':';
		strcpy(gamelose, lose);
		gamelose[strlen(gamelose)] = ':';
		sendnum = 2;
	} else if (waiting == 2) {
		strcat(gameid,id);
		gameid[strlen(gameid)] = ':';
		strcat(gamepw,pw);
		gamepw[strlen(gamepw)] = ':';
		strcat(gamewin,win);
		gamewin[strlen(gamewin)] = ':';
		strcat(gamelose,lose);
		gamelose[strlen(gamelose)] = ':';
		sendnum = 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	struct sockaddr_in clientAddr;
	int i, j, nBytes, acceptSocket;
	int addrLen = sizeof(struct sockaddr_in);
	fd_set readFds;
	Checking buf;
	DIR *dirp;

	id = (char *)malloc(sizeof(char) * 256);
        pw = (char *)malloc(sizeof(char) * 256);
        win = (char *)malloc(sizeof(char) * 256);
        lose = (char *)malloc(sizeof(char) * 256);
	name = (char *)malloc(sizeof(char) * 256);
	fname = (char *)malloc(sizeof(char) * 256);
	gameid = (char *)malloc(sizeof(char) * 256);
	gamepw = (char *)malloc(sizeof(char) * 256);
	gamewin = (char *)malloc(sizeof(char) * 256);
	gamelose = (char *)malloc(sizeof(char) * 256);
	loginid1 = (char *)malloc(sizeof(char) * 256);
	loginid2 = (char *)malloc(sizeof(char) * 256);
	memset(loginid1, 0x00, strlen(loginid1));
	memset(loginid2, 0x00, strlen(loginid2));

	if (argc != 2) {
		printf("Usage : %s port\n", argv[0]);
		exit(1);
	}
	listenSocket = tcpListen(INADDR_ANY, atoi(argv[1]), 5);

        if ((dirp = opendir("Information")) == NULL) { // Information 디렉토리가 없으면 생성하기
                mkdir("./Information",0775);
        }

	while (1) {
		FD_ZERO(&readFds);
                FD_SET(listenSocket, &readFds);
                for (i=0; i<nChats; i++) { FD_SET(clientSockets[i], &readFds); }
		nFds = getMax() + 1;

		if (select(nFds, &readFds, NULL, NULL, NULL) < 0) { exit(1); }
		if (FD_ISSET(listenSocket, &readFds)) {
			acceptSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
                        if (acceptSocket == -1) { exit(1); }
			addClient(acceptSocket, &clientAddr);
		}

		for (i = 0; i < nChats; i++) {
                        if (FD_ISSET(clientSockets[i], &readFds)) {
                                nBytes = recv(clientSockets[i], &buf, sizeof(Checking), 0);
                                if (nBytes == 0) {
                                        removeClient(i);
                                        continue;
                                }
				memset(id, 0x00, strlen(id));
				memset(pw, 0x00, strlen(pw));
				memset(win, 0x00, strlen(win));
				memset(lose, 0x00, strlen(lose));
				
				strcpy(id, buf.id);
				strcpy(pw, buf.pw);
				strcpy(win, buf.win);
				strcpy(lose, buf.lose);
				if (buf.scr_num == 0) {
					screen = 0;
					if (buf.check == 2)
						loginname(i, 2);
				} else if (buf.scr_num == 1) {
					screen = 1;
					signin();
				} else if (buf.scr_num == 2) {
					screen = 2;
					signup();
				} else if (buf.scr_num == 5) {
					screen = 5;
					withdr();
				} else if (buf.scr_num == 4) {
					screen = 4;
					gconnect[i] = 2;
					game();
				}

        	                if (sendnum == 1) {
                	                buf.check = 0;
					strcpy(buf.id, id);
                                	strcpy(buf.pw, pw);
                        	        if (screen == 1) {
                                        	strcpy(buf.win, win);
	                                        strcpy(buf.lose, lose);
        	                                buf.scr_num = 3;
						loginname(i, 1);
                	                        printf("SIGN IN: %s\n", id);
                        	        } else if (screen == 2) {
						buf.scr_num = 2;
						printf("SIGN UP: %s\n", id);
					} else if (screen == 5) {
						buf.scr_num = 5;
					} else if (screen == 4) {
						buf.scr_num = 4;
	                                        strcpy(buf.id, gameid);
                	                        strcpy(buf.win, gamewin);
                        	                strcpy(buf.lose, gamelose);
					}
	                        } else if (sendnum == 2) {
	                                buf.check = 1;
					strcpy(buf.id, id);
                                        strcpy(buf.pw, pw);
					if (screen == 1) {
						buf.scr_num = 1;
					} else if (screen == 2) {
						buf.scr_num = 2;
					} else if (screen == 5) {
						buf.scr_num = 5;
					} else if (screen == 4) {
						buf.scr_num = 4;
	                                        strcpy(buf.id, gameid);
                	                        strcpy(buf.win, gamewin);
                        	                strcpy(buf.lose, gamelose);
					}
				}
				if (sendnum == 1 && screen == 4) {
					for (j = 0; j < nChats; j++)
						send(clientSockets[j], &buf, sizeof(buf), 0);
				} else
                                	send(clientSockets[i], &buf, sizeof(buf), 0);
                        }
                }
	}
	free(id);
	free(pw);
	free(win);
	free(lose);
        free(name);
        free(fname);
	return 0;
}

int getMax() {
        int max = listenSocket;
        int i;
        for (i = 0; i < nChats; i++)
                if (clientSockets[i] > max)
                        max = clientSockets[i];
        return max;
}

void addClient(int s, struct sockaddr_in* clientAddr) {
        char buffer[20];
        inet_ntop(AF_INET, &clientAddr->sin_addr, buffer, sizeof(buffer));
        clientSockets[nChats] = s;
	gconnect[nChats] = 1;
        nChats++;
	printf("CONNECT: %d\n",nChats);
}

void removeClient(int s) {
        close(clientSockets[s]);

        if (s != nChats - 1) {
                clientSockets[s] = clientSockets[nChats - 1];
		gconnect[s] = gconnect[nChats - 1];
	}
        nChats--;
        printf("The user has logged out - %d users remaining.\n", nChats);
}

int tcpListen(int host, int port, int backlog) {
        int sd;
        struct sockaddr_in serverAddr;

        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("Failed to create a socket.");
                exit(1);
        }

        bzero((char *)&serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(host);
        serverAddr.sin_port = htons(port);

        if (bind(sd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
                perror("Failed to bind.");
                exit(1);
        }

        listen(sd, backlog);
	return sd;
}
