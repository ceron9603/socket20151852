#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 10000
#define BUFSIZE 100

char buffer[BUFSIZE] = "Hi, I'm Server";
//sizeof(buffer) => 100 (배열의 크기)
//strlen(buffer) => 15 (buffer에 저장된 문자열의 길이)
char rcvBuffer[BUFSIZE];

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;

	// 1. 서버 소켓 생성
	//서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP/IP 통신을 위한 서버 소켓 생성
	
	//2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로  초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	//3. 서버 소켓바인딩
	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		//바인딩 작업 실패 시, Cannot Bind 메시지 출력 후 프로그램 종료
		printf("Cannot Bind\n");
		return -1;
	}
	
	//4.listen() 함수 실행
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}

	//5. 클라이언트 요청 처리
	// 요청을 허용한 후, Hello World 메세지를 전송함
	while(1){ //무한 루프
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.
		printf("Client is connected\n");
		printf("클라이언트 접속 허용\n");
		while(1){
				n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
				printf("rcvBuffer: %s\n", rcvBuffer);
				rcvBuffer[n-1] = '\0'; //개행 문자 삭제

				if(strncasecmp(rcvBuffer, "quit", 4) == 0 || strncasecmp(rcvBuffer, "kill server", 11) == 0)
						break;
				else if(!strncasecmp(rcvBuffer, "안녕하세요", strlen("안녕하세요")))
						strcpy(buffer, "안녕하세요. 만나서 반가워요.");
				else if(!strncasecmp(rcvBuffer, "이름이 머야?", strlen("이름이 머야?")))
						strcpy(buffer, "내 이름은 홍영기야.");
				else if(!strncasecmp(rcvBuffer, "몇 살이야?", strlen("몇 살이야?")))
						strcpy(buffer, "나는 24살이야.");
				else if(!strncasecmp(rcvBuffer, "strlen ", strlen("strlen ")))
						sprintf(buffer, "문자열의 길이는 %d입니다.", strlen(rcvBuffer)-7); //-7은 공백까지 빼기
				else if(!strncasecmp(rcvBuffer, "strcmp ", strlen("strcmp "))){
						char *token;
						char *str[3];
						int idx = 0;
						token = strtok(rcvBuffer, " "); //공백을 기준으로 잘라 토큰으로 만들기
						while(token != NULL){ //널값이 될때까지 돌림
								str[idx] = token; //포인터배열에 토큰을 저장
								printf("str[%d] = %s\n", idx, str[idx]); //출력
								idx++; //인덱스 증가
								token = strtok(NULL, " "); //다음토큰을 변수에 저장
						}
						if(idx < 3)
								strcpy(buffer, "문자열 비교를 위해서는 두 문자열이 필요합니다."); //문자열이 하나만 들어올 경우
						else if(!strcmp(str[1], str[2])) //같은 문자열이면
								sprintf(buffer, "%s와 %s는 같은 문자열입니다", str[1], str[2]);
						else
								sprintf(buffer, "%s와 %s는 다른 문자열입니다", str[1], str[2]);
				}
				else if(!strncasecmp(rcvBuffer, "readfile ", strlen("readfile "))){
						char *token;
						char *str[2];
						int idx = 0;
						FILE *fp;
						token = strtok(rcvBuffer, " ");
						while(token != NULL){
								str[idx] = token;
								printf("str[%d] = %s\n", idx, str[idx]);
								idx++;
								token = strtok(NULL, " ");
						}
						if(idx < 1)
								strcpy(buffer, "읽을 파일의 이름이 필요합니다.");
						else {
								fp = fopen(str[1], "r");
								if(fp){
										while(fgets(buffer, BUFSIZE, fp)){
												write(c_socket, buffer, strlen(buffer));
										}
										continue;
								}
						}
				}
				else if(!strncasecmp(rcvBuffer, "exec ", strlen("exec "))){
						char *token;
						char *command;
						int i;
						token = strtok(rcvBuffer, " ");
						command = strtok(NULL, "\0"); //exec 뒤에 오는 모든 문자열 저장
						printf("command : %s\n", command);
						i = system(command);
						if(i == 0)
								sprintf(buffer, "[%s] command is executed\n", command);
						else if(i != 0)
								sprintf(buffer, "[%s] command is failed\n", command);
										
				}		
				else
						strcpy(buffer, "무슨 말인지 모르겠습니다.");


			
				write(c_socket, buffer, strlen(buffer));
		}
		close(c_socket);
		if(strncasecmp(rcvBuffer, "kill server", 11) == 0)
				break;
	}
	close(s_socket);
	return 0;
}

