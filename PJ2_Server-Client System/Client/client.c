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
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main_cursor = 0, main_cursor2 = 0; // main_cursor=메인 로그인전 커서, main_cursor2=메인 로그인후 커서
int login_cursor = 0, login_input = 0; // login_cursor=로그인화면 커서, login_input=함수signing 호출
int login = 0; // 1:sign in키를 누름 2:다시로그인 3:오류출력
int sign_cursor = 0, sign_input = 0; // sign_cursor=회원가입화면 커서, sign_input=함수signup 호출
int sign = 0; // 1:sign up키를 누름, 2:정상 가입, 3:다시계정만들기
int screen = 0; // 0:메인_로그인전 1:로그인 2:회원가입 3:메인_로그인후 4:회원정보 5:회원탈퇴 6:게임화면
int game_x = 0, game_y = 0, loc_x = 0, loc_y = 0; // 게임화면내 커서 x좌표=loc_x, y좌표=loc_y
int game_input = 0; // game_screen함수 호출
int wd_cursor = 0, wd_input = 0; // wd_cursor=회원탈퇴 화면 커서, wd_input=함수withdr 호출
int wd = 0; // 1:withdrawal 키를 누름 2:회원 탈퇴 3:다시pw입력
char *str, *str2, *name, *fname, *id, *pw, *win, *lose;
char *id2, *pw2, *win2, *lose2;
int info = 9, info_input = 0;
int check = 0, logout = 0;

typedef struct Checking {
	int scr_num;
	char id[20];
	char pw[20];
	char win[20];
	char lose[20];
	int check; // 0이면 OK, 1이면 not OK
} Checking;

int tcpConnect(int af, char* serverIp, unsigned short port);

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

        if (login_input == 1) {
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
	char star[strlen(str2)];
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
	char star[strlen(str2)];
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

int information(WINDOW *wina, WINDOW *winb) { // screen = 4 회원 정보
	curs_set(0); // 커서 숨기기
	if (has_colors() == TRUE) {
                start_color();
                init_color(1, 0, 500, 500); // 옅은 파랑
                init_pair(3, 1, COLOR_WHITE);
        }
        refresh();
	char *player1, *player2, *pwin1, *pwin2, *plose1, *plose2;
	char buf[1024], buf2[1024], buf3[1024];
	strcpy(buf, id2);
	strcpy(buf2, win2);
	strcpy(buf3, lose2);
	player1 = strtok(buf, ":");
	player2 = strtok(NULL, ":");
	pwin1 = strtok(buf2, ":");
	pwin2 = strtok(NULL, ":");
	plose1 = strtok(buf3, ":");
	plose2 = strtok(NULL, ":");

	mvwprintw(wina,7,10,"PLAYER1 ID : %s",player1);
        wattron(wina, COLOR_PAIR(3));
        mvwprintw(wina,9,15,"STATISTICS");
	mvwprintw(wina,9,55,"STATISTICS");
        wattroff(wina, COLOR_PAIR(3));

	int wins = atoi(pwin1);
	int loses = atoi(plose1);
	double pro;
	if ((wins+loses) == 0)
		pro = 0;
	else
		pro = ((double)(wins*100)/(double)(wins+loses));
        mvwprintw(wina,11,5,"WIN : %d / LOSE : %d (%.3lf%)",wins,loses,pro);

	if (info_input == 1) {
		mvwprintw(wina,7,50,"PLAYER2 ID : ");
		mvwprintw(wina,11,45,"WIN :    / LOSE :    (   %)");
		mvwprintw(winb,2,36,"WAITING");
	} else if (info_input == 2) {
		mvwprintw(wina,7,50,"PLAYER2 ID : %s",player2);
		int wins2 = atoi(pwin2);
		int loses2 = atoi(plose2);
		double pro2;
		if ((wins2+loses2) == 0)
			pro2 = 0;
		else
			pro2 = ((double)(wins2*100)/(double)(wins2+loses2));
		mvwprintw(wina,11,45,"WIN : %d / LOSE : %d (%.3lf%)",wins2,loses2,pro2);
		wattron(winb, COLOR_PAIR(1));
	        mvwprintw(winb,2,39,"OK");
        	wattroff(winb, COLOR_PAIR(1));
		info_input = 0;
	}
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

int main(int argc, char* argv[]) {
	initscr(); // 커서 초기화
	keypad(stdscr, TRUE);
	WINDOW *window1; // 흰색 화면
        WINDOW *window2; // 블루 화면
	int finish = 0;
	struct dirent *direntp;
	DIR *dirp;
	char *msg;
	int nFds, s;
	fd_set readFds;
	Checking buf;

	if (argc != 3) {
		printf("Usage : %s server_ip port\n", argv[0]);
		exit(1);
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
	id2 = (char *)malloc(sizeof(char) * 256);
        pw2 = (char *)malloc(sizeof(char) * 256);
        win2 = (char *)malloc(sizeof(char) * 256);
        lose2 = (char *)malloc(sizeof(char) * 256);

	s = tcpConnect(AF_INET, argv[1], atoi(argv[2]));
        if (s == -1)
                exit(1);
        nFds = s + 1;
        FD_ZERO(&readFds);

	while (finish != 1) {
	        wrefresh(window1);
    		wrefresh(window2);

		FD_SET(0, &readFds);
		FD_SET(s, &readFds);

                if (FD_ISSET(0, &readFds)) {
			if (screen == 0 && logout == 1) {
				buf.scr_num = 0;
				buf.check = 2;
				send(s, &buf, sizeof(buf), 0);
				logout = 0;
			}
			if (screen == 1 && login == 1) {
                                buf.scr_num = 1;
                                strcpy(buf.id, str);
                                strcpy(buf.pw, str2);
                                send(s, &buf, sizeof(buf), 0);
                        }
			if (screen == 2 && sign == 1) {
				buf.scr_num = 2;
				strcpy(buf.id, str);
				strcpy(buf.pw, str2);
				send(s, &buf, sizeof(buf), 0);
			
			}
			if (screen == 5 && wd == 1) {
				buf.scr_num = 5;
				strcpy(buf.id, id);
				strcpy(buf.pw, str2);
				send(s, &buf, sizeof(buf), 0);
			}
			if (screen == 4 && info == 1) {
				buf.scr_num = 4;
				buf.check = 0;
				strcpy(buf.id, id);
				strcpy(buf.pw, pw);
				strcpy(buf.win, win);
				strcpy(buf.lose, lose);
				send(s, &buf, sizeof(buf), 0);
			}
		}
                if (FD_ISSET(s, &readFds)) {
			if (screen == 1 && login == 1) {
                                int nBytes;
                                nBytes = recv(s, &buf, sizeof(Checking), 0);
                                if (buf.check == 0) {
                                        screen = buf.scr_num;
                                        memset(id, 0x00, strlen(id));
                                        memset(pw, 0x00, strlen(pw));
                                        memset(win, 0x00, strlen(win));
                                        memset(lose, 0x00, strlen(lose));
                                        strcpy(id, str);
                                        strcpy(pw, str2);
                                        strcpy(win, buf.win);
                                        strcpy(lose, buf.lose);
                                        login = 0;
                                        login_cursor = 0;
                                        login_input = 0;
					logout = 0;
                                } else if (buf.check == 1) {
                                        screen = buf.scr_num;
                                        login = 3;
                                }
                        }
			if (screen == 2 && sign == 1) {
				int nBytes;
				nBytes = recv(s, &buf, sizeof(Checking), 0);
				if (buf.check == 0) {
					screen = buf.scr_num;
					check = 0;
				} else if (buf.check == 1) {
					screen = buf.scr_num;
					check = 1;
				}
			}
			if (screen == 5 && wd == 1) {
				int nBytes;
                                nBytes = recv(s, &buf, sizeof(Checking), 0);
				if (buf.check == 0) {
					screen = buf.scr_num;
					check = 0;
					logout = 1;
				} else if (buf.check == 1) {
					screen = buf.scr_num;
					check = 1;
				}
			}
			if (screen == 4 && info == 1) {
				int nBytes;
                                nBytes = recv(s, &buf, sizeof(Checking), 0);
				memset(id2, 0x00, strlen(id2));
				memset(pw2, 0x00, strlen(pw2));
				memset(win2, 0x00, strlen(win2));
				memset(lose2, 0x00, strlen(lose2));
				strcpy(id2, buf.id);
                                strcpy(pw2, buf.pw);
                                strcpy(win2, buf.win);
                                strcpy(lose2, buf.lose);
				screen = buf.scr_num;
				if (info == 1) {
					if (buf.check == 0) {
						info_input = 2;
						info = 3;
					} else if (buf.check == 1)
						info = 2;
				}
				information(window1, window2);
			}
			if (screen == 4 && info == 2) {
				while (1) {
				int nBytes;
                                nBytes = recv(s, &buf, sizeof(Checking), 0);
					if (buf.check == 0)
						break;
				}
				if (buf.check == 0) {
                                memset(id2, 0x00, strlen(id2));
                                memset(pw2, 0x00, strlen(pw2));
                                memset(win2, 0x00, strlen(win2));
                                memset(lose2, 0x00, strlen(lose2));
                                strcpy(id2, buf.id);
                                strcpy(pw2, buf.pw);
                                strcpy(win2, buf.win);
                                strcpy(lose2, buf.lose);
                                screen = buf.scr_num;
				info_input = 2;
				info = 3;
				}
				werase(window1);
				werase(window2);
				information(window1,window2);
				wrefresh(window1);
				wrefresh(window2);
			}
                }

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
                        if (sign == 1) {
				if (check == 0) {
					sign = 2;
					sign_cursor = 0;
					sign_input = 0;
					mvwprintw(window2,5,0,">>> Welcome to OTHELLO World! (Press any key...)");
				} else if (check == 1) {
					sign = 3;
					mvwprintw(window2,5,0,">>> %s has already exist in DB! (Please any key...)",str);
				}
				wrefresh(window2);
			}
                } else if (screen == 3) {
			main_screen2(window1, window2);
		} else if (screen == 5) {
			wd_screen(window1, window2);
			if (wd == 1) {
				if (check == 0) {
					wd = 2;
					wd_cursor = 0;
					wd_input = 0;
					main_cursor = 0;
					main_cursor2 = 0;
					logout = 1;
					memset(id, 0x00, strlen(id)); // id 초기화
                        		memset(pw, 0x00, strlen(pw)); // pw 초기화
					memset(win, 0x00, strlen(win)); // win 초기화
					memset(lose, 0x00, strlen(lose)); // lose 초기화
					mvwprintw(window2,5,0,">>> Success to withdraw %s! (Please any key...)",id);
				} else if (check == 1) {
					wd = 3;
					logout = 0;
					mvwprintw(window2,5,0,">>> Wrong to input password! (Please any key...)");
				}
				wrefresh(window2);
			}
		} else if (screen == 6) {
			game_screen();
			if (game_input == 1) {
				curs_set(2); // 커서 보이기
				move(loc_x,loc_y); // 커서 이동
				refresh();
			}
		}

                if (select(nFds, &readFds, NULL, NULL, NULL) < 0)
                        exit(1);

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
					logout = 0;
					info = 1;
					info_input = 1;
					main_cursor2 = 0;
				} else if (main_cursor2%3 == 1) { // SIGN OUT 눌렀을 때
					memset(id, 0x00, strlen(id)); // id 초기화
					logout = 1;
					screen = 0;
					main_cursor = 0;
					main_cursor2 = 0;
				} else if (main_cursor2%3 == 2) { // WITHDRAWAL 눌렀을 때
					screen = 5;
					wd_cursor = 0;
					wd_input = 1;
				}
			} else if (screen == 4) { // OK 눌렀을 때
				if (info == 3) {
					info = 0;
					info_input = 0;
					screen = 6;
					game_x = 2;
					game_y = 2;
					game_input = 1;
				}
			} else if (screen == 5) {
				if (wd == 0) {
					if (wd_cursor%2 == 1) { // BACK 눌렀을 때
						screen = 3;
						logout = 0;
						wd_input = 0;
						wd_cursor = 0;
					} else if (wd_cursor%2 == 0) { // WITHDRAWAL 눌렀을 때
						wd = 1;
					}
				} else if (wd == 2) { // 정상 회원탈퇴
					screen = 0;
					logout = 1;
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
	if (finish == 1) {
		buf.check = 2;
		send(s, &buf, sizeof(buf), 0);
		close(s);
	}
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

int tcpConnect(int af, char* serverIp, unsigned short port) {
        struct sockaddr_in serverAddr;
        int s;

        if ((s = socket(af, SOCK_STREAM, 0)) < 0) { return -1; }

        bzero((char *)&serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = af;
        inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);
        serverAddr.sin_port = htons(port);

        if (connect(s, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
                return -1;

        return s;
}
