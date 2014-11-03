#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(void){
 char command[100];
 int pid, status;
    while(1){        
        printf(">>");
        scanf("%[^\n]%*c", command);
        
        pid = fork();
        if (pid!=0) {
                waitpid(pid, &status, WUNTRACED);
                break;
        }
        else {
                char *argv[20];
                char copy[100];
                char *fil1, *fil2;
                
                char *token;
                int i = 0, j = 0;
                int fd1, fd2;
                char *pch;
                strcpy(copy, command);
                if((pch = strstr(command, ">>"))!=NULL){
                        pch[0] = '\0';
                        pch+=3;
                        strcpy(temp,pch);
                        fil2 = strtok(pch, " ");
                        fd1 = open(fil2, O_RDWR | O_CREAT , S_IRUSR | S_IWUSR);
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);
                        
}
                if((pch = strstr(copy, "<<"))!=NULL){
                        pch[0] = '\0';
                        pch+=3;
                        strcpy(temp,pch);
                        fil1 = strtok(pch, " ");
                        fd2 = open(fil1, O_RDWR , S_IRUSR | S_IWUSR);
                        dup2(fd2, STDIN_FILENO);
                        close(fd2);

                }
                if(strlen(copy) < strlen(command))
                        strcpy(command,copy);
                token = strtok(command," ");
                while( token!= NULL ){
                        argv[i] = token;
                        token = strtok(NULL," ");
                        i++;
                }
                argv[i] = NULL;
                execvp(argv[0], argv);  
        }
   }
}