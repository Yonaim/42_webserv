#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp)
{
dprintf(STDOUT_FILENO, "\nthis is CGI Program ...\n");
dprintf(STDOUT_FILENO, "===== META VARIABLES =====\n");
// dprintf(STDOUT_FILENO, "===== META VARIABLES =====\n");

// write(1, "this is cgi program\n", 21);

// char buffer[100];
// fgets(buffer, 100, stdin);
// dprintf(2, "stdin: %s\n", buffer);

int i = 0;
while (envp[i])
	dprintf(STDOUT_FILENO, "%s\n", envp[i++]);

dprintf(2, "dprintf: ===== META VARIABLES =====\n");

i = 0;
while (envp[i])
	dprintf(2, "dprintf: %s\n", envp[i++]);

// message_body (stdin) 테스트 추가 예정
close(STDOUT_FILENO);
return (0);
}
