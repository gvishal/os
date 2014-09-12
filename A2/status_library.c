#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define handle_error(msg) \
        do { printf("%s\n", msg); exit(EXIT_FAILURE); } while (0)

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
    int i;
    if(argc == 1){
        handle_error("No pid specified");
    }
    else if(argc >= 2){
        pid = argv[1];
        if(!CheckInt(pid))
            handle_error("Pid should be an integer");
    }
    strcat(process_file, pid);
    strcat(process_file, "/status");

    Getdetails(process_file, (char *)process_info);

    for(i=0; i<30; i++){
        if(Match(process_info[i], "State") > -1)
            printf("%s\n", process_info[i]);
        if(Match(process_info[i], "PPid") > -1)
            printf("%s\n", process_info[i]);
        if(Match(process_info[i], "Threads") > -1)
            printf("%s\n", process_info[i]);
    }

    return 0;
}