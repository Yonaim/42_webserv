#include <cstdlib>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("usage: %s var1 var2 ... path/to/cgi/script\n", argv[0]);
		return (1);
	}

	int env_size = argc - 2;
	char **env = static_cast<char **>(malloc(sizeof(char *) * (env_size + 1)));
	env[env_size] = NULL;
	for (int i = 0; i < env_size; ++i)
		env[i] = strdup(argv[i + 1]);
	printf("==========env==========\n");
	for (int i = 0; env[i]; ++i)
		printf("%s\n", env[i]);
	printf("=======================\n");
	if (execve(argv[argc - 1], NULL, env) < 0)
		perror("");
	return (0);
}
