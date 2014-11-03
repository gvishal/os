#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>

int main(){
    pid_t pid=0;
    unsigned int i=0,flag=1;
    while(flag){
        pid = fork();
        if(pid<0){
            perror("Child Proc. not created");
            flag = 0;
        }
        else if(pid == 0){
            wait(getppid());
            _exit(0);
        }
        else
            i++;
    }
    printf("%u\n", i);
    return 0;
}