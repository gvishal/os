#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define handle_error(msg) \
        do { Print(msg, 1); exit(EXIT_FAILURE); } while (0)

size_t
Strlen(const char *buf)
{
    size_t length = 0;
    while(*(buf+length))
        length++;
    return length;
}

int
Strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
        if (*s1 == '\0')
            return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

char *
Strcpy(char *s1, const char *s2)
{
    char *s = s1;
    while ((*s++ = *s2++) != 0)
    ;
    return (s1);
}

char *
Strcat(char *s1, const char *s2)
{
    Strcpy(&s1[Strlen(s1)], s2);
    return s1;
}

ssize_t 
WriteStdout(char *buf)
{
    return write(STDOUT_FILENO, buf, Strlen(buf));
}

void 
Print(char *buf, int new_line)
{
    size_t len;
    len = Strlen(buf);
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
Getdetails(const char *process_file, char *process_info){
    register int n;
    int fd,i=0,pos=0;
    char buf[2];
    fd = open(process_file, O_RDONLY);
    if(fd == -1)
        handle_error("No such process");
    while((n = read(fd, buf, 1)) > 0){
        if(i<30 && buf[0] == '\n'){
            *((process_info + i*100) + pos) ='\0';
            i++;
            pos=0;
        }
        else if(i<30){
            *((process_info + i*100) + pos) = buf[0];
            pos++;
            }
        if(buf[0] == EOF)
            break;
    }
    return;
}

int Atoi(char *buf){
    int x,len;
    len = Strlen(buf);
    for( ; *buf; buf++)
        x = x*10 + (int)(*buf - '0');
    return x;
}

int CheckInt(char *buf){
    int x = buf[0] - '0';
    if(x >= 0 && x <=9)
        return 1;
    return 0;
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

int main(int argc, char *argv[]){
    char *pid;
    char process_file[100] = "/proc/";
    char process_info[30][100];
    if(argc == 1){
        handle_error("No pid specified");
    }
    else if(argc >= 2){
        pid = argv[1];
        if(!CheckInt(pid))
            handle_error("Pid should be an integer");
    }
    Strcat(process_file, pid);
    Strcat(process_file, "/status");

    Getdetails(process_file, (char *)process_info);

    Print(process_info[1], 1);
    Print(process_info[5], 1);

    if(Match(process_info[11], "Threads") > -1)
        Print(process_info[11], 1);
    else
        Print(process_info[23], 1);

    return 0;
}