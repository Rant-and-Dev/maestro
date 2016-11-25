#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>

char* argv0;

int usage(int exitcode)
{
	fprintf(stderr, "usage: %s command parameters\n", argv0);
	exit(exitcode);
}

int main(int argc, char* argv[])
{
	char** newargv;
	int status;
	argv0 = argv[0];
	argc--; argv++;

	if(!argc)
	{
		usage(1);
	}

	struct child {
		int in[2];
		int out[2];
	} child;

	if(pipe(child.in) < 0)
		perror("pipe");
	if(pipe(child.out) < 0)
		perror("pipe");

	pid_t pid;

	posix_spawn_file_actions_t action;
	posix_spawn_file_actions_init(&action);
	posix_spawn_file_actions_adddup2(&action, child.in[0], STDIN_FILENO);
	posix_spawn_file_actions_addclose(&action, child.in[0]);
	posix_spawn_file_actions_adddup2(&action, child.out[1], STDOUT_FILENO);
	posix_spawn_file_actions_addclose(&action, child.out[1]);

	if(posix_spawnp(&pid, argv[0], &action, NULL, argv, NULL) < 0)
	{
		// TODO: manage errors here.
	}

	int i;
	char c[2];
	c[1] = '\0';
	for(i = 0; i < 5; i++)
	{
		c[0] = '0' + i;
		write(child.in[1], "echo ", 5);
		write(child.in[1], c, 1);
		write(child.in[1], "\n", 1);
		c[0] = '\0';
		read(child.out[0], c, 1);
		printf("Output was: %s\n", c);
		read(child.out[0], c, 1); // read the newline
	}

	write(child.in[1], "exit\n", 5);

	waitpid(pid, &status, 0);

	printf("%s exited with status %d\n", argv[0], status);
	return 0;
}
