# InformationManagement_ServerClient_LINUX
2018.12.16
###### Project assignment 2
Project assignment 1 + Server/Client System
###### 프로젝트 과제1
리눅스에서 서버/클라이언트로 회원 정보 관리 및 화면 동작 구성
- 서버/클라이언트 통신
  - IP 주소와 포트번호로 접속
- 파일 또는 디렉토리 입출력으로 회원 정보 관리

### 실행 환경
> Ubuntu 16.04.5 LTS  
	ncurses 6.1  
	C 언어 / GCC 컴파일러  
	시스템 호출 함수  

### 프로젝트 구조
- 최상위 디렉토리 Project 안에 서버 관련 디렉토리 Server, 클라이언트 관련 디렉토리 Client가 있음
  - 디렉토리 Server 내에 서버 코드 server.c가 있음
  - 디렉토리 Client 내에 클라이언트 코드 client.c가 있음
- 서버
  - 회원가입, 로그인, 회원 탈퇴 등의 회원관리
  - 회원 정보 업데이트
  - 클라이언트는 최대 2개까지 접속할 수 있음
- 클라이언트
  - 서버와의 통신을 통한 회원 가입, 로그인, 회원 탈퇴
  - 다른 플레이어 대기
  - 서버와의 통신을 통한 진행
  
### 기본 동작
- 코드 실행 시 Server 디렉토리 내에 Information 디렉토리가 생성되어 text파일로 회원 정보를 저장하게 됨
- 커서가 버튼에 위치할 경우에, 좌우 방향키 입력으로 버튼 간 커서 이동
  - 커서는 좌우로 계속 움직일 수 있음 
- Enter 키를 입력하면, 현재 커서에 따라 동작
- 계정명과 비밀번호는 Enter 키를 입력할 때까지 입력 받음
- 서버
  - 프로그램 실행 시에, 포트번호를 입력 받아야 함
- 클라이언트
  - 프로그램 실행 시에, 서버의 IP 주소와 포트번호를 입력 받아야 함

### 회원 정보 관리 및 화면간 동작
- 화면
> 메인 - 로그인 전  
> 메인 - 로그인 후  
> 회원 가입  
> 로그인  
> 회원 탈퇴  
> 회원 정보  
> 게임 화면  
- 링크의 README.md 파일을 참고
[link](https://github.com/hayoung10/InformationManagement_ServerClient_LINUX/blob/master/PJ1_Member%20Information%20Management/README.md)

------
## 프로그램 실행 EXAMPLES
- Server
``` linux
gcc server.c -o server.out
./server.out 5000
```
- Client1
``` linux
gcc client.c -o client.out -lncurses
./client.out 127.0.0.1 5000
```
- Client2
``` linux
./client.out 127.0.0.1 5000
```

## Screenshots
##### 서버 출력  
![server](/Screenshots/PJ2/Server.JPG)

##### 클라이언트 출력
- 현재 접속한 사용자가 1명인 경우
![client1](/Screenshots/PJ2/player1.JPG)

- 현재 접속한 사용자가 2명인 경우
![client2](/Screenshots/PJ2/player2.JPG)
