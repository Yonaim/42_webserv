#include <stdio.h>
#include <unistd.h>
#include <memory.h>

int main(int argc, char **argv, char **envp)
{
	char buff[300];
	char *buff_ptr = buff;
	int temp = 0;

	memset(buff, 0, 300);
	while ((temp = read(0, buff_ptr, 100)))
	{
		if (temp < 0)
		{
			perror("scanf");
			return (1);
		}
		buff_ptr += temp;
	}
	dprintf(2, "scan done\n");
	printf("message body: %s\n", buff);

	printf("===== META VARIABLES =====\n");
	printf("this is CGI Program ...\n");
	int i = 0;
	while (envp[i])
		printf("%s\n", envp[i++]);

	fflush(stdout);
	close(STDOUT_FILENO);
	return (0);
}
