#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>

#define BUFFSIZE 8192

size_t
StringLength(const char *buf)
{
    size_t length = 0;
    while(*(buf+length))
        length++;
    printf("%zu\n", length);
    return length;
}

char 
*StringWithNewLine(char *buf)
{
    char *old_buf = buf;
    while(*buf){
        buf++;
    }
    *buf = '\n';
    buf++;
    *buf = '\0';
    return old_buf;
}

int
StringCmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
        if (*s1 == '\0')
            return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

ssize_t 
WriteStdout(char *buf)
{
    return write(STDOUT_FILENO, buf, StringLength(buf));
}

ssize_t 
Print(char *buf, int new_line)
{
    size_t len;
    len = StringLength(buf);
    if(len > 0 && write(STDOUT_FILENO, buf, len) != len){
        WriteStdout("Write error");
    }
    if(new_line){
        if(write(STDOUT_FILENO, "\n", 1) != 1){
            WriteStdout("Write error new line");
        }
    }
}

void 
main(int argc,char *argv[])
{
    if(argc < 2){
        printf("usage: %s ls [-l,-a,-h,...optional parameters]\n", argv[0]);
    }
    else{
        //Print(argv[1], 1);
        if(StringCmp(argv[1], "ls") == 0){
            Print("ls working", 1);
            int flag_l,flag_a,flag_h;
            char *options,*path;
            if(argc == 2){
                *options = "\0";
                *path = "\0";
            }
            else if(argc > 2 && argv[2][0] == '-'){
                options = argv[2];
                if(argc == 4)path = argv[3];
                else *path = "";
            }
            else if(argc > 3 && argv[3][0] == '-'){
                options = argv[3];
                path = argv[2];
            }
            printf("%s %s\n",options, path);
            // Print(options, 1);
            // Print(path, 1);
        }
        else Print("Unknown command entered", 1);
    }
}