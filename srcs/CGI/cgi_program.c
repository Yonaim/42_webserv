#include <stdio.h>

int main(int argc, char **argv, char **envp)
{
	printf("this is CGI Program ...\n");
	printf("===== META VARIABLES =====\n");

	int i = 0;
	while (envp[i])
		printf("%s\n", enwvp[i++]);

    // message_body (stdin) 테스트 추가 예정

}