#include "includes.h"

static char *myStrDup (char *str) 
{
    char *other = malloc (strlen (str) + 1);
    if (other != NULL)
        strcpy (other, str);
    return other;
}


// List | Sort Function
void multiExec(char * buf, int sendSocket)
{
    // Set up pipes to read and write to
    int pipes[2];
    if(pipe(pipes) == -1) 
    {
        perror("Pipe failed");
        exit(1);
    }

    // Child Process for ls function
    if (fork() == 0)       
    {
        // Set output to pipe
        close(STDOUT_FILENO);  
        dup(pipes[1]);         
        close(pipes[0]);       
        close(pipes[1]);

        // Set up argument list for execvp
        char* prog1[] = {"ls", 0};

        execvp(prog1[0], prog1);
        perror("execvp of ls failed");
        exit(1);
    }

    // Child process for sort function
    if (fork() == 0)            
    {
        // Set input to pipe
        close(STDIN_FILENO);   
        dup(pipes[0]);         
        close(pipes[1]); 
        close(pipes[0]);

        // Set up argument list for execvp
        char* prog2[] = {"sort", 0};

        // Set outputs to client
        dup2(sendSocket, STDOUT_FILENO);
		dup2(sendSocket, STDERR_FILENO);

        execvp(prog2[0], prog2);
        close(sendSocket);
        perror("execvp of sort failed");
        exit(1);
    }

    close(pipes[0]);
    close(pipes[1]);
    wait(0);
    wait(0);

  	close(sendSocket);
}