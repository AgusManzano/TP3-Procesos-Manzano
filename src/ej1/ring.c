#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{	
	int pid, n;
	int buffer[1];


	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
	


	/* Parsing of arguments */
	n = atoi(argv[1]);
	int c = atoi(argv[2]);
	int s = atoi(argv[3]);
	
	printf("n: %i, c: %i, s: %i\n", n, c, s);

	printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, c, s);	
	
	int pipes[n][2]; // Array of pipes
	
	// Create pipes
	for (int i = 0; i < n; i++) {
		if (pipe(pipes[i]) == -1) {
			perror("Error creating pipe");
			exit(1);
		}
	}
	
	// Create child processes
	for (int i = 0; i < n; i++) 
	{
    pid = fork();

    if (pid < 0) {
        perror("Error creating child process");
        exit(1);

    } 
	else if (pid == 0) 
	{
		// Child process
		close(pipes[i][1]); // Close write end of the pipe

		if (i == s) 
		{
			// This is the process that will send the character
			buffer[0] = c;
			printf("Proceso %i enviará el caracter %i al siguiente proceso\n", i, buffer[0]);
			buffer[0]++;
		} 
		else 
		{
			// Other processes
			while (read(pipes[i][0], buffer, sizeof(buffer)) == 0) 
			{
				// Wait for something to be written to the pipe
			}
			printf("Proceso %i recibió el caracter %i y lo enviará al siguiente proceso\n", i, buffer[0]);
			buffer[0]++;
		}

		close(pipes[i][0]); // Close read end of the pipe
		write(pipes[(i + 1) % n][1], buffer, sizeof(buffer)); // Write to next process
		close(pipes[(i + 1) % n][1]); // Close write end of the pipe

		// Close the read and write ends of all pipes
		for (int j = 0; j < n; j++) 
		{
			close(pipes[j][0]);
			close(pipes[j][1]);
		}

		exit(0);
	}
	}
	return 0;
}



