#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	char command[100],copy[100],infile[50],outfile[50];
 	int pid,status,fd_in,fd_out,i;

    while(1){	
		printf(">>");
		// scanf("%s", command);
		scanf("%[^\n]%*c", command);
		//printf("%s\n", command);
		strcpy(copy, command);
		// printf("%s\n", copy);
		pid = fork();
		if (pid!=0) {
			waitpid(pid, &status, WUNTRACED);
			break;
		}
		else {

			char *argv[20],*r,*t;
			// argv[0] = command;
			if((r = strstr(command, "<<")) != NULL){
				r[0] = '\0';
				r += 3;
				t = strtok(r , " ");
				fd_in = open(t, O_RDONLY, S_IRUSR | S_IWUSR);
				dup2(fd_in, STDIN_FILENO);
				// close(fd_in);
			}

			if((r = strstr(copy, ">>")) != NULL){
				r[0] = '\0';
				r += 3;
				t = strtok(r , " ");
				fd_out = open(t, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
				dup2(fd_out, STDOUT_FILENO);
				// close(fd_out);
			}

			if(strlen(copy) < strlen(command))
				strcpy(command, copy);
			
			r = strtok(command, " ");
			i = 0;
			while(r != NULL){
				argv[i] = r;
				r = strtok(NULL, " ");
				i++;
			}
			argv[i] = '\0';
			//printf("%s %s\n", argv[0], argv[1]);
	        execvp(argv[0], argv);  
			break;
	}
   }
}
