#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
char answer[255];

/**
DONOT change the existing function definitions. You can add functions, if necessary.
*/

/**
  @brief Fork a child to execute the command using execvp. The parent should wait for the child to terminate
  @param args Null terminated list of arguments (including program).
  @return returns 1, to continue execution and 0 to terminate the MyShell prompt.
 */
int execute(char **args)
{	
	if (strcmp("exit", args[0]) == 0) {
		return 0;
	}
	pid_t pid = fork();
	if (pid == -1) {
		printf("Failed forking the child\n");
	} else if (pid == 0) {
		if (execvp(args[0], args) < 0) {
			printf("error executing command: No such file or directory\n");
		}
		exit(0);
	} else {
		wait(NULL);
	}
	return 1;
}


/**
  @brief gets the input from the prompt and splits it into tokens. Prepares the arguments for execvp
  @return returns char** args to be used by execvp
 */
char** parse(void)
{		
	char *token[1000];
	char *ptr = strtok(answer, " ");
	int counter = 0;
	while (ptr != NULL) {
		token[counter] = strdup(ptr);
		ptr = strtok(NULL, " ");
		counter++;
	}
	token[counter] = ptr;
	char ** args = malloc(sizeof(char *) * (counter + 1));
	for (int i = 0; i < counter; i++) {
		*(args + i) = token[i];
	}
	*(args + counter) = NULL;
	return args;
}


/**
   @brief Main function should run infinitely until terminated manually using CTRL+C or typing in the exit command
   It should call the parse() and execute() functions
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
	int number = 1;
	while (number == 1) {
		printf("MyShell> ");
		fgets(answer, sizeof answer, stdin);
		if (strlen(answer) != 1) {
			answer[strcspn(answer, "\n")] = 0;
			char** parsedArguments = parse();
 			number = execute(parsedArguments);
 			free(parsedArguments);
		}
 	}
 
	return EXIT_SUCCESS;
}
