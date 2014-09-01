#include <unistd.h>
#include <stdio.h>

#define BUFFSIZE 8192

size_t StringLength(char *buf){
    size_t length = 0;
    while(*(buf+length))
        length++;
    return length;
}

char *StringWithNewLine(char *buf){
    char *old_buf = buf;
    while(*buf){
        buf++;
    }
    *buf = '\n';buf++;
    *buf = '\0';
    return old_buf;
}

ssize_t Print(char *buf, int new_line){
    size_t len;
    if(new_line){
        buf = StringWithNewLine(buf);
    }
    len = StringLength(buf);
    if(write(STDOUT_FILENO, buf, len) != len){
        Print("Write error", 1);
    }
}

void main(int argc,char *argv[]){
    if(argc == 1){
        printf("usage: %s ls [optional parameters...]\n", argv[0]);
    }
    else{
        char *buf;
        buf = argv[1];
        Print(buf, 0);
    }
}