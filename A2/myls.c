//myls.c
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 8192
#define BUF_SIZE 1024

#define handle_error(msg) \
        do { Print(msg, 1); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

size_t
StringLength(const char *buf)
{
    size_t length = 0;
    while(*(buf+length))
        length++;
    return length;
}

int
StringCmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
        if (*s1 == '\0')
            return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

int 
Match(char *a, char *b)
{
   int c;
   int position = 0;
   char *x, *y;
 
   x = a;
   y = b;
 
   while(*a)
   {
      while(*x==*y)
      {
         x++;
         y++;
         if(*x=='\0'||*y=='\0')
            break;         
      }   
      if(*y=='\0')
         break;
 
      a++;
      position++;
      x = a;
      y = b;
   }
   if(*a)
      return position;
   else   
      return -1;   
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
    int flag_l=0,flag_a=0,flag_h=0;
    char *options,*path;
    //for getdents
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;

    if(argc < 2){
        options = "";
        path = ".";
    }
    else if(argc > 1 && argv[1][0] == '-'){
        options = argv[1];
        if(argc == 3)path = argv[2];
        else path = ".";
    }
    else if(argc > 2 && argv[2][0] == '-'){
        options = argv[2];
        path = argv[1];
    }
    for(;*options;options++){
        if(*options == 'l')flag_l = 1;
        else if(*options == 'a')flag_a = 1;
        else if(*options == 'h')flag_h = 1;
    }

    fd = open(path, O_RDONLY | O_DIRECTORY);
    if(fd == -1)
        Print("Directory does not exist or no permissions", 1);

    for ( ; ; ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            if(!flag_a && d->d_name[0] != '.'){
                printf("%s\n", (char *) d->d_name);
            }
            else if(flag_a)
                printf("%s\n", (char *) d->d_name);
            bpos += d->d_reclen;
        }
    }
    // printf("%s %s\n",options, path);
    // Print(options, 1);
    // Print(path, 1);
}