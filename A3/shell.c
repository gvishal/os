#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
 char command[100];
 int pid, status;


    while(1){	
	printf(">>");
	scanf("%s", command);
	pid = fork();
	if (pid!=0) {
		waitpid(pid, &status, WUNTRACED);
		break;
	}
	else {
		char *argv[3];
		argv[0] = command;
		argv[1] = strdup("dummy");
        	execvp(command, argv);  
		break;
	}
   }
}
