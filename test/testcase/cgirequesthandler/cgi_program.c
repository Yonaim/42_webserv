#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp)
{
	char buff[300];
	char *buff_ptr = buff;
	int temp = 0;

	while (1)
	{
		temp = read(STDIN_FILENO, buff_ptr, 100);
		if (temp < 0)
		{
			perror("scanf");
			return (1);
		}
		if (temp == 0)
		{
			buff[buff_ptr - buff] = '\0';
			break;
		}
		buff_ptr += temp;
	}
	/// debug
	dprintf(2, "\n======== CGI Program ========\n");
	dprintf(2, "\n- Message Body:\n\t%s\n\n", buff);
	dprintf(2, "- Meta Variables:\n");

	int i = 0;
	while (envp[i])
		dprintf(2, "\t%s\n", envp[i++]);
	fflush(stderr);

	// response message
	printf("\n======== CGI Program ========\n");
	printf("\n- Message Body: %s\n", buff);
	printf("- Meta Variables:\n");

	i = 0;
	while (envp[i])
		printf("\t%s\n", envp[i++]);

	fflush(stdout);
	return (0);
}
