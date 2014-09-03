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
#include <time.h>
#include <limits.h>
//#include "loc_time.h"

#define BUFFSIZE 8192
#define BUF_SIZE 1024

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define handle_error(msg) \
        do { Print(msg, 1); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

struct detail {
    char permissions[11];
    int st_nlink;
    int st_uid;
    char uname[100];
    int st_gid;
    char gname[100];
    off_t st_size;//long int
};

#define INT_DIGITS 19       /* enough for 64 bit integer */

char 
*itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;   /* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {            /* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

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
    return write(STDOUT_FILENO, buf, Strlen(buf));
}

ssize_t 
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

char
*Getname(int id,char *name){
    int fd,n,i=0,pos=0;
    char passwd_file[] = "/etc/passwd";
    char info[4][100];
    char buf[2];
    char *cid = itoa(id);
    // printf("%s\n", cid);
    fd = open(passwd_file, O_RDONLY);
    if(fd == -1)
        handle_error("Directory does not exist or no permissions");
    while((n = read(fd, buf, 1)) > 0){
        //printf("%c", buf[0]);
        if(i<4 && buf[0] == ':'){
            info[i][pos]='\0';
            i++;
            pos=0;
        }
        else if(buf[0] == '\n'){
            pos=0;
            i=0;
            if(Strcmp(cid, info[2]) == 0){
                name = info[0];
                return name;
            }
        }
        else if(i<4){
            info[i][pos] = buf[0];
            pos++;
            }
        if(buf[0] == EOF)
            break;
    }
    return "None";
}

char* readable_fs(double size/*in bytes*/,char *buf) {
    int i = 0;
    const char* units[] = {"", "K", "M", "G", "T", "P", "E", "Z", "Y"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f%s", i, size, units[i]);
    return buf;
}

//Epoch to date time
typedef struct
{
    unsigned char second; // 0-59
    unsigned char minute; // 0-59
    unsigned char hour;   // 0-23
    unsigned char day;    // 1-31
    unsigned char month;  // 1-12
    unsigned char year;   // 0-99 (representing 2000-2099)
}
date_time_t;

static unsigned short days[4][12] =
{
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};

static char *bfmt[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec",
};

void epoch_to_date_time(date_time_t* date_time,unsigned int epoch)
{
    date_time->second = epoch%60; epoch /= 60;
    date_time->minute = epoch%60; epoch /= 60;
    date_time->hour   = epoch%24; epoch /= 24;

    unsigned int years = epoch/(365*4+1)*4; epoch %= 365*4+1;

    unsigned int year;
    for (year=3; year>0; year--)
    {
        if (epoch >= days[year][0])
            break;
    }

    unsigned int month;
    for (month=11; month>0; month--)
    {
        if (epoch >= days[year][month])
            break;
    }

    date_time->year  = years+year;
    date_time->month = month+1;
    date_time->day   = epoch-days[year][month]+1;
}

void
Detail(char *details, struct stat sb, int flag_h){
    int de=0;
    // if(S_ISDIR(sb.st_mode)){
    //     details[de++] = 'd';
    // }
    // else if(S_ISLNK(sb.st_mode)){
    //     details[de++] = 'l';
    // }
    // else if(S_ISREG(sb.st_mode)){
    //     details[de++] = '-';
    // }
    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  details[de++] = 'b';break;
    case S_IFCHR:  details[de++] = 'c';break;
    case S_IFLNK:  details[de++] = 'l';break;
    case S_IFDIR:  details[de++] = 'd';break;
    case S_IFIFO:  details[de++] = 'p';break;
    case S_IFREG:  details[de++] = '-';break;
    case S_IFSOCK: details[de++] = 's';break;
    default:       details[de++] = '-';break;
    }
    if(sb.st_mode & S_IRUSR){
        details[de++] = 'r';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IWUSR){
        details[de++] = 'w';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IXUSR){
        details[de++] = 'x';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IRGRP){
        details[de++] = 'r';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IWGRP){
        details[de++] = 'w';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IXGRP){
        details[de++] = 'x';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IROTH){
        details[de++] = 'r';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IWOTH){
        details[de++] = 'w';
    }
    else details[de++] = '-';
    if(sb.st_mode & S_IXOTH){
        details[de++] = 'x';
    }
    else details[de++] = '-';

    details[de++] = ' ';
    details[de] = '\0';

    Strcat(&details[de], itoa(sb.st_nlink));
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';

    char name[100];
    Strcat(&details[de], Getname(sb.st_uid, name));
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';
    de = Strlen(details);   
    
    Strcat(&details[de], Getname(sb.st_gid, name));
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';
    de = Strlen(details);

    char hsize[10];
    if(flag_h)
        Strcat(&details[de], readable_fs(sb.st_size,hsize));
    else 
        Strcat(&details[de], itoa(sb.st_size));
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';
    de = Strlen(details);

    date_time_t date_time;
    char s[20];
    epoch_to_date_time(&date_time, sb.st_mtime + 60*330 + 24*60*60);//UTC to IST
    
    Strcat(&details[de], bfmt[date_time.month - 1]);
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';
    de = Strlen(details);

    Strcat(&details[de], itoa(date_time.day));
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';
    de = Strlen(details);

    Strcat(&details[de], itoa(date_time.hour));
    de = Strlen(details);
    details[de++] = ':';
    details[de] = '\0';
    de = Strlen(details);

    Strcat(&details[de], itoa(date_time.minute));
    de = Strlen(details);
    details[de++] = ' ';
    details[de] = '\0';
    de = Strlen(details);
}

void 
main(int argc,char *argv[])
{   
    int flag_l=0,flag_a=0,flag_h=0,print_true=0;
    char *options,*path;
    //for getdents
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;
    //for stat
    struct stat sb;
    char details[1024];
    int de;

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
    else if(argc > 1 && argv[1][0] != '-'){
        options = "";
        path = argv[1];
    }

    for( ; *options; options++){
        if(*options == 'l')flag_l = 1;
        else if(*options == 'a')flag_a = 1;
        else if(*options == 'h')flag_h = 1;
    }

    fd = open(path, O_RDONLY | O_DIRECTORY);
    if(fd == -1)
        handle_error("Directory does not exist or no permissions");

    for ( ; ; ) {
        char complete_path[100];

        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            print_true = 0;
            complete_path[0] = '\0';
            Strcat(complete_path, path);
            if(path[0] != '.'){
                Strcat(complete_path, "/");
                // printf("%s\n", complete_path );
                Strcat(complete_path, d->d_name);
            }
            else{
                Strcpy(complete_path, d->d_name);
            }
            //printf("%s\n", complete_path );

            if (lstat(complete_path, &sb) == -1)
                handle_error("stat");

            if(!flag_a && d->d_name[0] != '.'){
                print_true = 1;
                // if(flag_l){
                //     Detail(details, sb, flag_h);
                //     printf("%s ", details);
                // }
                // if(S_ISDIR(sb.st_mode)){
                //     printf(ANSI_COLOR_YELLOW);
                //     printf("%s\n", (char *) d->d_name);
                //     printf(ANSI_COLOR_RESET);
                // }
                // else
                //     printf("%s\n", (char *) d->d_name);
            }
            else if(flag_a){
                print_true = 1;
                //same as above;
            }
            if(print_true){

                if(flag_l){
                    Detail(details, sb, flag_h);
                    printf("%s ", details);
                }
                if(S_ISDIR(sb.st_mode)){
                    printf(ANSI_COLOR_YELLOW);
                    printf("%s\n", (char *) d->d_name);
                    printf(ANSI_COLOR_RESET);
                }
                else if(S_ISLNK(sb.st_mode)){
                    char linkname[sb.st_size + 1];
                    ssize_t r;
                    printf(ANSI_COLOR_CYAN);
                    printf("%s -> ",(char *)d->d_name);
                    printf(ANSI_COLOR_RESET);

                    r = readlink(complete_path, linkname, sb.st_size + 1);
                    if (r < 0)
                       handle_error("lstat");
                    if (r > sb.st_size)
                       handle_error("symlink increased in size ");
                    linkname[sb.st_size] = '\0';
                    printf("%s\n",linkname);
                }
                else if(S_ISSOCK(sb.st_mode)){
                    printf(ANSI_COLOR_MAGENTA);
                    printf("%s\n", (char *) d->d_name);
                    printf(ANSI_COLOR_RESET);
                }
                else
                    printf("%s\n", (char *) d->d_name);
            }

            bpos += d->d_reclen;
        }
    }
}