#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMMANDS 200

char* handleQuotes(char* arg);

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        /* Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        /* You should start programming from here... */

        // Create an array of pipes to connect the commands
        int pipes[command_count - 1][2];
        for (int i = 0; i < command_count; i++) 
        {
            // Initialize the pipes
            if (i < command_count - 1) 
            {
                if (pipe(pipes[i]) == -1) 
                {
                    perror("pipe");
                    return 1;
                }
            }

            pid_t pid = fork();
            if (pid == -1) 
            {
                perror("fork");
                return 1;
            } 
            else if (pid == 0) 
            {
                // Child process

                // Redirect the input of the child process to the previous pipe
                if (i > 0) 
                {
                    if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) 
                    {
                        perror("dup2");
                        return 1;
                    }
                }

                // Redirect the output of the child process to the current pipe
                if (i < command_count - 1) 
                {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) 
                    {
                        perror("dup2");
                        return 1;
                    }
                }

                // Close unused file descriptors
                for (int j = 0; j < command_count - 1; j++) 
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Execute the corresponding program
                char *args[256];
                int arg_count = 0;
                char *arg = strtok(commands[i], " ");
                while (arg != NULL) 
                {
                    args[arg_count++] = handleQuotes(arg);
                    arg = strtok(NULL, " "); 

                
                }
                // printf("Executing command: %s\n", args[0]); // Print the command being executed

                // printf("Output:\n"); // Print the output of the command
                // printf("args[0]: %s\n", args[0]);

                // for (int j = 0; j < arg_count; j++) 
                // {
                //     printf("args[%d]: %s\n", j, args[j]); // Print the arguments of the command
                // }
                
                // add NULL to the end of the arguments array
                args[arg_count] = NULL;

                execvp(args[0], args);
                perror("execvp");
                return 1;
            } 
            else 
            {
                // Parent process
                
                // Close the read file descriptor of the previous pipe
                if (i > 0) 
                {
                    close(pipes[i-1][0]);
                }
                // Close the write file descriptor of the current pipe
                if (i < command_count - 1) 
                {
                    close(pipes[i][1]);
                }
            }
        }

        // Wait for all child processes to finish
        for (int i = 0; i < command_count; i++) 
        {
            wait(NULL);
        }

        // Reset the command count
        command_count = 0;
    }
    return 0;
}


char* handleQuotes(char* arg) 
{
    // If the argument starts with a quote, find the matching quote
    if (arg[0] == '"') 
    {
        // Remove the starting quote
        arg++;
        // Find the ending quote and remove it
        char *end_quote = strchr(arg, '"');
        if (end_quote != NULL) 
        {
            *end_quote = '\0';
        }
    }
    return arg;
}