#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

int main_cursor = 0, main_cursor2 = 0; // main_cursor=메인 로그인전 커서, main_cursor2=메인 로그인후 커서
int login_cursor = 0, login_input = 0; // login_cursor=로그인화면 커서, login_input=함수signing 호출
int login = 0; // 1:sing in키를 누름 2:다시로그인 3:오류출력
int sign_cursor = 0, sign_input = 0; // sign_cursor=회원가입화면 커서, sign_input=함수signup 호출
int sign = 0; // 1:sign up키를 누름, 2:정상 가입, 3:다시계정만들기
int screen = 0; // 0:메인_로그인전 1:로그인 2:회원가입 3:메인_로그인후 4:회원정보 5:회원탈퇴 6:게임화면
int game_x = 0, game_y = 0, loc_x = 0, loc_y = 0; // 게임화면내 커서 x좌표=loc_x, y좌표=loc_y
int game_input = 0; // game_screen함수 호출
int wd_cursor = 0, wd_input = 0; // wd_cursor=회원탈퇴 화면 커서, wd_input=함수withdr 호출
int wd = 0; // 1:withdrawal 키를 누름 2:회원 탈퇴 3:다시pw입력
char *str, *str2, *name, *fname, *id, *pw, *win, *lose;

int main_screen(WINDOW *wina, WINDOW *winb){ // screen = 0 메인_로그인전
	curs_set(0); // 커서 숨기기
	int cursor2;
	if (main_cursor < 0) {
		int num = (abs(main_cursor)%3);
		cursor2 = (3 - num);
	} else
		cursor2 = main_cursor; 

        if (has_colors() == TRUE) {
                start_color();
		init_color(1, 0, 500, 500); // 옅은 파랑
		init_pair(3, 1, COLOR_WHITE);
        }
	refresh();

        mvwprintw(wina,7,28,"System Software Practice");
        mvwprintw(wina,14,67,"2016603009");
        mvwprintw(wina,16,67,"HAYOUNG MOON");
	wattron(wina, COLOR_PAIR(3));
	mvwprintw(wina,9,37,"OTHELLO");
	wattroff(wina, COLOR_PAIR(3));

	if (cursor2%3 == 0){
		wattron(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,10,"SIGN IN");
		wattroff(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,37,"SIGN UP");
		mvwprintw(winb,2,70,"EXIT");
	} else if (cursor2%3 == 1){
		mvwprintw(winb,2,10,"SIGN IN");
		wattron(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,37,"SIGN UP");
		wattroff(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,70,"EXIT");
	} else if (cursor2%3 == 2){
		mvwprintw(winb,2,10,"SIGN IN");
		mvwprintw(winb,2,37,"SIGN UP");
		wattron(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,70,"EXIT");
		wattroff(winb, COLOR_PAIR(1));
	}
        wrefresh(wina);
        wrefresh(winb);

	return 0;
}

int main_screen2(WINDOW *wina, WINDOW *winb) { // screen = 3 메인_로그인후
	curs_set(0); // 커서 숨기기
	int cursor3;
	if (main_cursor2 < 0) {
		int num2 = (abs(main_cursor2)%3);
		cursor3 = (3 - num2);
	} else
		cursor3 = main_cursor2;

	refresh();
	mvwprintw(wina,7,30,"PLAYER ID : %s",id);
	if (cursor3%3 == 0) {
		wattron(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,10,"PLAY");
		wattroff(winb, COLOR_PAIR(1));
                mvwprintw(winb,2,35,"SIGN OUT");
                mvwprintw(winb,2,60,"WITHDRAWAL");
	} else if (cursor3%3 == 1) {
		mvwprintw(winb,2,10,"PLAY");
		wattron(winb, COLOR_PAIR(1));
                mvwprintw(winb,2,35,"SIGN OUT");
		wattroff(winb, COLOR_PAIR(1));
                mvwprintw(winb,2,60,"WITHDRAWAL");
	} else if (cursor3%3 == 2) {
		mvwprintw(winb,2,10,"PLAY");
		mvwprintw(winb,2,35,"SIGN OUT");
		wattron(winb, COLOR_PAIR(1));
		mvwprintw(winb,2,60,"WITHDRAWAL");
		wattroff(winb, COLOR_PAIR(1));
	}
	wrefresh(wina);
	wrefresh(winb);

	return 0;
}

int login_screen(WINDOW *wina, WINDOW *winb) { // screen = 1 로그인
        if (has_colors() == TRUE) {
                start_color();
                init_color(1, 0, 500, 500); // 옅은 파랑
                init_pair(3, 1, COLOR_WHITE);
        }
        refresh();

	wattron(wina, COLOR_PAIR(3));
	mvwprintw(wina,7,35,"SIGN IN");
	wattroff(wina, COLOR_PAIR(3));
	mvwprintw(wina,9,30,"ID : ");
	mvwprintw(wina,11,24,"PASSWORD : ");

        if (login_input == 1 ) {
		login = 0;
		login_cursor = 0;
		mvwprintw(winb,2,15,"SIGN IN");
                mvwprintw(winb,2,60,"BACK");
		wrefresh(winb);
		curs_set(2); // 커서 보이기
		memset(str, 0x00, strlen(str)); // str 초기화
		memset(str2, 0x00, strlen(str2)); // str2 초기화
		echo();
                mvwgetstr(wina,9,35,str);
                int loc = 0;
		noecho();
		move(11,35);
		while ((str2[loc] = getch()) != '\n') {
			if ((str2[loc] == 127) && (loc != 0)) {
				mvwprintw(wina,11,35+loc,"\b \b");
				str2[loc] = '\0';
				loc--;
			} else {
				if ((str2[loc] == 127) && (loc == 0))
					continue;
				else
					mvwprintw(wina,11,35+loc,"*");
				loc++;
			}
			move(11,35+loc);
			wrefresh(wina);
		}
		strtok(str2,"\n");
                login_input = 2;
                login_cursor = 0;
		curs_set(0); // 커서 숨기기
        }
	char star[1024];
	memset(star,0x00,strlen(star));
	for (int i=0; i<strlen(str2); i++)
		star[i] = '*';
	if (login_input == 2) {
		int cursor = abs(login_cursor);
		if (cursor%2 == 0) {
			mvwprintw(wina,9,35,"%s",str);
			mvwprintw(wina,11,35,"%s",star);	
			wattron(winb, COLOR_PAIR(1));
			mvwprintw(winb,2,15,"SIGN IN");
			wattroff(winb, COLOR_PAIR(1));
			mvwprintw(winb,2,60,"BACK");
		} else if (cursor%2 == 1) {
			mvwprintw(wina,9,35,"%s",str);
                        mvwprintw(wina,11,35,"%s",star);
			mvwprintw(winb,2,15,"SIGN IN");
			wattron(winb, COLOR_PAIR(1));
			mvwprintw(winb,2,60,"BACK");
			wattroff(winb, COLOR_PAIR(1));
		}
	}
	wrefresh(wina);
	wrefresh(winb);

        return 0;
}

int signin() { // 로그인 확인
	int fd1, exist = 0, idcheck = 0, pwcheck = 0;
	char buf[1024], *check;
	struct dirent *direntp;
        DIR *dirp;

	memset(name, 0x00, strlen(name)); // name 초기화
	memset(fname, 0x00, strlen(fname)); // fname 초기화
	strcpy(name,"./Information/");
        strcat(name,str);
        strcat(name,".txt");
        strcpy(fname,str);
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

	memset(id, 0x00, strlen(id)); // id 초기화
	memset(pw, 0x00, strlen(pw)); // pw 초기화
	memset(win, 0x00, strlen(win)); // win 초기화
	memset(lose, 0x00, strlen(lose)); // lose 초기화
	if (exist == 1) { // 계정이 존재하면
		if ((fd1 = open(name, O_RDONLY)) != -1) {
			if (read(fd1,buf,sizeof(buf)) != -1) {
				check = strtok(buf,":");
				if (strcmp(check,str) == 0) { // id 확인
					strcpy(id,check);
					idcheck = 1;
				}
				check = strtok(NULL,":");
				if (strcmp(check,str2) == 0) { // pw 확인
					strcpy(pw,check);
					pwcheck = 1;
				}
				if ((idcheck == 1) && (pwcheck == 1)) {
					check = strtok(NULL,":");
					strcpy(win,check);
					check = strtok(NULL,":");
					strcpy(lose,check);
					screen = 3;
					login = 0;
					login_cursor = 0;
					login_input = 0;
				} else
					login = 3;
			}
		}
	} else
		login = 3;
	close(fd1);
	closedir(dirp);

	return 0;
}

int sign_screen(WINDOW *wina, WINDOW *winb) { // screen = 2 회원가입
        if (has_colors() == TRUE) {
                start_color();
                init_color(1, 0, 500, 500); // 옅은 파랑
                init_pair(3, 1, COLOR_WHITE);
        }
        refresh();

	wattron(wina, COLOR_PAIR(3));
        mvwprintw(wina,7,35,"SIGN UP");
        wattroff(wina, COLOR_PAIR(3));
        mvwprintw(wina,9,30,"ID : ");
        mvwprintw(wina,11,24,"PASSWORD : ");

        if (sign_input == 1 ) {
		sign = 0;
		sign_cursor = 0;
                mvwprintw(winb,2,15,"SIGN UP");
                mvwprintw(winb,2,60,"BACK");
                wrefresh(winb);
                curs_set(2); // 커서 보이기
		memset(str, 0x00, strlen(str)); // str 초기화
		memset(str2, 0x00, strlen(str2)); // str2 초기화
		echo();
		mvwgetstr(wina,9,35,str);
		int loc = 0;
		noecho();
		move(11,35);
		while ((str2[loc] = getch()) != '\n') {
			if ((str2[loc] == 127) && (loc != 0)) {
				mvwprintw(wina,11,35+loc,"\b \b");
				str2[loc] = '\0';
				loc--;
			} else {
				if ((str2[loc] == 127) && (loc == 0))
						continue;
				else
					mvwprintw(wina,11,35+loc,"*");
				loc++;
			}
			move(11,35+loc);
			wrefresh(wina);
		}
		strtok(str2,"\n");
                sign_input = 2;
                sign_cursor = 0;
		curs_set(0); // 커서 숨기기
	}
	char star[1024];
	memset(star,0x00,strlen(star));
	for (int i=0; i < strlen(str2); i++)
                        star[i] = '*'; 
	if (sign_input == 2) {
		int cursor = abs(sign_cursor);
                if (cursor%2 == 0) {
			mvwprintw(wina,9,35,"%s",str);
			mvwprintw(wina,11,35,"%s",star);
                        wattron(winb, COLOR_PAIR(1));
                        mvwprintw(winb,2,15,"SIGN UP");
                        wattroff(winb, COLOR_PAIR(1));
                        mvwprintw(winb,2,60,"BACK");
                } else if (cursor%2 == 1) {
			mvwprintw(wina,9,35,"%s",str);
                        mvwprintw(wina,11,35,"%s",star);
                        mvwprintw(winb,2,15,"SIGN UP");
                        wattron(winb, COLOR_PAIR(1));
                        mvwprintw(winb,2,60,"BACK");
                        wattroff(winb, COLOR_PAIR(1));
                }
        }
	wrefresh(wina);
        wrefresh(winb);

	return 0;
}

int signup(WINDOW *winb) { // 회원가입 확인
	int fd1, exist;
	char buf[1024], *info;
	memset(name, 0x00, strlen(name)); // name 초기화
	memset(fname, 0x00, strlen(fname)); // fname 초기화
	info = (char *)malloc(sizeof(char) * 256);

	strcpy(name,"./Information/");
	strcat(name,str);
	strcat(name,".txt");
	strcpy(fname,str);
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
			strcpy(info,str);
	                info[strlen(info)] = ':';
        	        strcat(info,str2);
                	info[strlen(info)] = ':';
	                strcat(info,"0"); // win = 0
        	        info[strlen(info)] = ':';
                	strcat(info,"0"); // lose = 0
			info[strlen(info)] = ':';
			if (write(fd1,info,strlen(info)) != -1) {
				sign = 2;
				sign_cursor = 0;
				sign_input = 0;
				mvwprintw(winb,5,0,">>> Welcome to OTHELLO World! (Press any key...)");
			}
		}
	} else if (exist == 1) {
		sign = 3;
		mvwprintw(winb,5,0,">>> %s has already exist in DB! (Please any key...)",str);
	}
	close(fd1);
	closedir(dirp);
	wrefresh(winb);
	free(info);

	return 0;
}

int information(WINDOW *wina, WINDOW *winb) { // screen = 4 회원 정보
	curs_set(0); // 커서 숨기기
	if (has_colors() == TRUE) {
                start_color();
                init_color(1, 0, 500, 500); // 옅은 파랑
                init_pair(3, 1, COLOR_WHITE);
        }
        refresh();
	mvwprintw(wina,7,30,"PLAYER ID : %s",id);
        wattron(wina, COLOR_PAIR(3));
        mvwprintw(wina,9,35,"STATISTICS");
        wattroff(wina, COLOR_PAIR(3));

	int wins = atoi(win);
	int loses = atoi(lose);
	double pro;
	if ((wins+loses) == 0)
		pro = 0;
	else
		pro = ((double)(wins*100)/(double)(wins+loses));
        mvwprintw(wina,11,25,"WIN : %d / LOSE : %d (%.3lf%)",wins,loses,pro);
	wattron(winb, COLOR_PAIR(1));
	mvwprintw(winb,2,39,"OK");
	wattroff(winb, COLOR_PAIR(1));

	wrefresh(wina);
	wrefresh(winb);

	return 0;
}

int wd_screen(WINDOW *wina, WINDOW *winb) { // screen = 5 회원탈퇴
	if (has_colors() == TRUE) {
                start_color();
                init_color(1, 0, 500, 500); // 옅은 파랑
                init_pair(3, 1, COLOR_WHITE);
        }
        refresh();

	wattron(wina, COLOR_PAIR(3));
	mvwprintw(wina,7,35,"WITHDRAWAL");
	wattroff(wina, COLOR_PAIR(3));
	mvwprintw(wina,9,30,"PLAYER ID : %s",id);
	mvwprintw(wina,11,24,"PASSWORD : ");
	
	if (wd_input == 1) {
		wd = 0;
		wd_cursor = 0;
		mvwprintw(winb,2,15,"WITHDRAWAL");
		mvwprintw(winb,2,60,"BACK");
		wrefresh(winb);
		curs_set(2); // 커서 보이기
		echo();
		memset(str2, 0x00, strlen(str2)); // str2 초기화
                mvwgetstr(wina,11,35,str2);
                wd_input = 2;
                curs_set(0); // 커서 숨기기
	}
	if (wd_input == 2) {
		int cursor = abs(wd_cursor);
		if (cursor%2 == 0) {
			mvwprintw(wina,11,35,"%s",str2);
			wattron(winb, COLOR_PAIR(1));
	        	mvwprintw(winb,2,15,"WITHDRAWAL");
        		wattroff(winb, COLOR_PAIR(1));
			mvwprintw(winb,2,60,"BACK");
		} else if (cursor%2 == 1) {
			mvwprintw(wina,11,35,"%s",str2);
			mvwprintw(winb,2,15,"WITHDRAWAL");
			wattron(winb, COLOR_PAIR(1));
			mvwprintw(winb,2,60,"BACK");
			wattroff(winb, COLOR_PAIR(1));
		}
	}
	wrefresh(wina);
	wrefresh(winb);

	return 0;
}

int withdr(WINDOW *winb) { // 회원 탈퇴 확인
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
                                if ((strcmp(pw,str2)==0)&&(strcmp(check,str2)==0)) // pw 확인
                                        pwcheck = 1;
				wrefresh(winb);
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
			if (clear == 0) {
				wd = 2;
				wd_cursor = 0;
				wd_input = 0;
				main_cursor = 0;
				main_cursor2 = 0;
                                mvwprintw(winb,5,0,">>> Success to withdraw %s! (Please any key...)",id);
			}
			memset(id, 0x00, strlen(id)); // id 초기화
                        memset(pw, 0x00, strlen(pw)); // pw 초기화
                        memset(win, 0x00, strlen(win)); // win 초기화
                        memset(lose, 0x00, strlen(lose)); // lose 초기화
		} else {
			mvwprintw(winb,5,0,">>> Wrong to input password! (Please any key...)");
                	wd = 3;
		}
        } else if (exist == 0)
		return 1;

	close(fd1);
	closedir(dirp);
	wrefresh(winb);

	return 0;
}

int game_screen() { // screen = 6 게임화면
	WINDOW *window3; // 게임 흰색 화면
	WINDOW *window4; // 게임 파란색 화면
	int x, y;
	if (game_x < 0) {
		int num = (abs(game_x)%6);
		x = (6 - num);
	}
	else
		x = game_x;
	if (game_y < 0) {
		int num = (abs(game_y)%6);
		y = (6 - num);
	}
	else
		y = game_y;
	loc_x = (6+2*(x%6)); // 게임 커서 x좌표 설정
	loc_y = (19+4*(y%6)); // 게임 커서 y좌표 설정

	if (has_colors() == TRUE) {
                start_color();
                init_pair(1, COLOR_BLUE, COLOR_WHITE);
                init_pair(2, COLOR_WHITE, COLOR_BLUE);
        }
	refresh();

	window3 = newwin(24,60,0,0);
	window4 = newwin(24,20,0,60);
        wbkgd(window3, COLOR_PAIR(1));
        wbkgd(window4, COLOR_PAIR(2));
	mvwprintw(window3,5,17,"+---+---+---+---+---+---+");
	mvwprintw(window3,6,17,"|   |   |   |   |   |   |");
	mvwprintw(window3,7,17,"+---+---+---+---+---+---+");
        mvwprintw(window3,8,17,"|   |   |   |   |   |   |");
	mvwprintw(window3,9,17,"+---+---+---+---+---+---+");
        mvwprintw(window3,10,17,"|   |   | O | X |   |   |");
	mvwprintw(window3,11,17,"+---+---+---+---+---+---+");
        mvwprintw(window3,12,17,"|   |   | X | O |   |   |");
	mvwprintw(window3,13,17,"+---+---+---+---+---+---+");
        mvwprintw(window3,14,17,"|   |   |   |   |   |   |");
	mvwprintw(window3,15,17,"+---+---+---+---+---+---+");
        mvwprintw(window3,16,17,"|   |   |   |   |   |   |");
	mvwprintw(window3,17,17,"+---+---+---+---+---+---+");

	if (game_input == 1) { // 커서가 게임보드에 있는 경우
		mvwprintw(window4,18,6,"EXIT");
		initscr();
	} else if (game_input == 2) { // 커서가 EXIT 버튼에 있는 경우
		curs_set(0); // 커서 숨기기
		wattron(window4,COLOR_PAIR(1));
		mvwprintw(window4,18,6,"EXIT");
		wattroff(window4,COLOR_PAIR(1));
	}
	wrefresh(window3);
	wrefresh(window4);

	return 0;
}

int main(void) {
	initscr(); // 커서 초기화
	keypad(stdscr, TRUE);
	WINDOW *window1; // 흰색 화면
        WINDOW *window2; // 블루 화면
	int finish = 0;
	struct dirent *direntp;
	DIR *dirp;

	if ((dirp = opendir("Information")) == NULL) { // Information 디렉토리가 없으면 생성하기
		mkdir("./Information",0775);
	}
	
        if (has_colors()==TRUE) {
                start_color();
                init_pair(1, COLOR_BLUE, COLOR_WHITE);
                init_pair(2, COLOR_WHITE, COLOR_BLUE);
        }
        refresh();

        window1 = newwin(18,80,0,0);
        window2 = newwin(6,80,18,0);
        wbkgd(window1, COLOR_PAIR(1));
        wbkgd(window2, COLOR_PAIR(2));

	str = (char *)malloc(sizeof(char) * 256);
	str2 = (char *)malloc(sizeof(char) * 256);
	name = (char *)malloc(sizeof(char) * 256);
	fname = (char *)malloc(sizeof(char) * 256);
	id = (char *)malloc(sizeof(char) * 256);
	pw = (char *)malloc(sizeof(char) * 256);
	win = (char *)malloc(sizeof(char) * 256);
	lose = (char *)malloc(sizeof(char) * 256);

	while (finish != 1) {
	        wrefresh(window1);
    		wrefresh(window2);

		if (screen == 0)
                        main_screen(window1, window2);
                else if (screen == 1) {
                        login_screen(window1, window2);
			if (login == 3) {
				mvwprintw(window2,5,0,">>> Wrong to login! (Press any key...)");
				wrefresh(window2);
				login = 2;
				}
		} else if (screen == 2) {
                        sign_screen(window1, window2);
                        if (sign == 1)
                                signup(window2);
                } else if (screen == 3) {
			main_screen2(window1, window2);
		} else if (screen == 4) {
			information(window1, window2);
		} else if (screen == 5) {
			wd_screen(window1, window2);
			if (wd == 1)
				withdr(window2);
		} else if (screen == 6) {
			game_screen();
			if (game_input == 1) {
				curs_set(2); // 커서 보이기
				move(loc_x,loc_y); // 커서 이동
			}
		}

                switch (getch()) {
                case KEY_LEFT:
                        if (screen == 0)
                                main_cursor--;
                        else if (screen == 1) {
				if (login_input == 2)
                                	login_cursor--;
				if (login == 2) // 다시 로그인
                                        login_input = 1;
                        } else if (screen == 2) {
                                if (sign_input == 2)
                                        sign_cursor--;
                                if (sign == 3) // 다시 회원가입
                                        sign_input = 1;
                                else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
				}
			} else if (screen == 3) {
				main_cursor2--;
			} else if (screen == 5) {
				if (wd_input == 2)
					wd_cursor--;
				if (wd == 3) // 다시 pw 입력
					wd_input = 1;
				else if (wd == 2) { // 정상 회원탈퇴
					screen = 0;
					wd = 0;
				}
			} else if (screen == 6) {
				if (game_input == 1) // 커서가 게임보드에 있는 경우
					game_y--;
			} break;
                case KEY_RIGHT:
                        if (screen == 0)
                                main_cursor++;
                        else if (screen == 1) {
				if (login_input == 2)
                                	login_cursor++;
				if (login == 2) // 다시 로그인
		                	login_input = 1;
                        } else if (screen == 2) {
                                if (sign_input == 2)
                                        sign_cursor++;
                                if (sign == 3) // 다시 회원가입
                                        sign_input = 1;
                                else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
                                }
                        } else if (screen == 3) {
				main_cursor2++;
			} else if (screen == 5) {
				if (wd_input == 2)
					wd_cursor++;
				if (wd == 3) // 다시 pw 입력
                                        wd_input = 1;
				else if (wd == 2) { // 정상 회원탈퇴
					screen = 0;
                                        wd = 0;
				}
			} else if (screen == 6) {
				if (game_input == 1) // 커서가 게임보드에 있는 경우
					game_y++;
			} break;
		case KEY_UP:
			if (screen == 1) {
                                if (login == 2) // 다시 로그인
                                        login_input = 1;
                        } else if (screen == 2) {
                                if (sign == 3) // 다시 회원가입
                                        sign_input = 1;
                                else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
                                }
                        } else if (screen == 5) {
                                if (wd == 3) // 다시 pw 입력
                                        wd_input = 1;
                                else if (wd == 2) { // 정상 회원탈퇴
                                        screen = 0;
                                        wd = 0;
                                }
                        } else if (screen == 6) {
				if (game_input == 1) // 커서가 게임보드에 있는 경우
					game_x--;
			} break;
		case KEY_DOWN:
			if (screen == 1) {
                                if (login == 2) // 다시 로그인
                                        login_input = 1;
                        } else if (screen == 2) {
                                if (sign == 3) // 다시 회원가입
                                        sign_input = 1;
                                else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
                                }
                        } else if (screen == 5) {
                                if (wd == 3) // 다시 pw 입력
                                        wd_input = 1;
                                else if (wd == 2) { // 정상 회원탈퇴
                                        screen = 0;
                                        wd = 0;
                                }
                        } else if (screen == 6) { // 커서가 게임보드에 있는 경우
				if (game_input == 1)
					game_x++;
			} break;
		case 103:
			if (screen == 1) {
                                if (login == 2) // 다시 로그인
                                        login_input = 1;
                        } else if (screen == 2) {
                                if (sign == 3) // 다시 회원가입
                                        sign_input = 1;
                                else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
                                }
                        } else if (screen == 5) {
                                if (wd == 3) // 다시 pw 입력
                                        wd_input = 1;
                                else if (wd == 2) { // 정상 회원탈퇴
                                        screen = 0;
                                        wd = 0;
                                }
                        } else if (screen == 6) {
				if (game_input == 2) // 커서가 EXIT 버튼에 있는 경우
					game_input = 1;
			} break;
		case 120:
			if (screen == 1) {
                                if (login == 2) // 다시 로그인
                                        login_input = 1;
                        } else if (screen == 2) {
                                if (sign == 3) // 다시 회원가입
                                        sign_input = 1;
                                else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
                                }
                        } else if (screen == 5) {
                                if (wd == 3) // 다시 pw 입력
                                        wd_input = 1;
                                else if (wd == 2) { // 정상 회원탈퇴
                                        screen = 0;
                                        wd = 0;
                                }
                        } else if (screen == 6) {
                                if (game_input == 1) // 커서가 게임보드에 있는 경우
                                        game_input = 2;
			} break;
		case 10:
			if (screen == 0) {
				if (main_cursor%3 == 0) { // SIGN IN 눌렀을 때
					screen = 1;
					login_input = 1;
				} else if (main_cursor%3 == 1) { // SIGN UP 눌렀을 때
					screen = 2;
					sign_input = 1;
				} else if (main_cursor%3 == 2) // EXIT 눌렀을 때
					finish = 1;
			} else if (screen == 1) {
				if (login == 0) { 
					if (login_cursor%2 == 1) { // BACK 눌렀을 때
						screen = 0;
						login_input = 0;
						login_cursor = 0;
					} else if (login_cursor%2 == 0) { // SIGN IN 눌렀을 때
						login = 1;
						signin();
					}
				} else if (login == 2) // 다시 로그인
                			login_input = 1;
			} else if (screen == 2) {
				if (sign == 0) {
					if (sign_cursor%2 == 1) { // BACK 눌렀을 때
						screen = 0;
						sign_input = 0;
						sign_cursor = 0;
					} else if (sign_cursor%2 == 0) // SIGN UP 눌렀을 때
						sign = 1;
				} else if (sign == 2) { // 정상 회원가입
                                        screen = 0;
                                        sign = 0;
                                } else if (sign == 3) // 다시 회원가입
					sign_input = 1;
			} else if (screen == 3) {
				if (main_cursor2%3 == 0) { // PLAY 눌렀을 때
					screen = 4;
					main_cursor2 = 0;
				} else if (main_cursor2%3 == 1) { // SIGN OUT 눌렀을 때
					memset(id, 0x00, strlen(id)); // id 초기화
					screen = 0;
					main_cursor = 0;
					main_cursor2 = 0;
				} else if (main_cursor2%3 == 2) { // WITHDRAWAL 눌렀을 때
					screen = 5;
					wd_cursor = 0;
					wd_input = 1;
				}
			} else if (screen == 4) { // OK 눌렀을 때
				screen = 6;
				game_x = 2;
				game_y = 2;
				game_input = 1;
			} else if (screen == 5) {
				if (wd == 0) {
					if (wd_cursor%2 == 1) { // BACK 눌렀을 때
						screen = 3;
						wd_input = 0;
						wd_cursor = 0;
					} else if (wd_cursor%2 == 0) { // WITHDRAWAL 눌렀을 때
						wd = 1;
					}
				} else if (wd == 2) { // 정상 회원탈퇴
					screen = 0;
					wd = 0;
				} else if (wd == 3) // 다시 pw 입력
					wd_input = 1;
			} else if (screen == 6) {
				if (game_input == 2) { // 커서가 EXIT 버튼에 있는 경우
					screen = 3;
					game_input = 0;
					game_x = 0;
					game_y = 0;
				}
			} break;
		default:
			if (screen == 1) { 
				if (login == 2) // 다시 로그인
	                		login_input = 1;
			} else if (screen == 2) {
				if (sign == 2) { // 정상 회원가입
					screen = 0;
					sign = 0;
				} else if (sign == 3) // 다시 회원가입
					sign_input = 1;
			} else if (screen == 5) {
				if (wd == 3) // 다시 pw 입력
					wd_input = 1;
				else if (wd == 2) { // 정상 회원탈퇴
					screen = 0;
					wd = 0;
				}
			} break;
		}
		werase(window1);
		wrefresh(window1);

		werase(window2);
		wrefresh(window2);
	}
	endwin();

	free(str);
	free(str2);
	free(id);
	free(pw);
	free(win);
	free(lose);
	free(name);
	free(fname);
	getch();

	return 0;
}
