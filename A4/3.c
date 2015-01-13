#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void sudo_pw1(){
    char password[] = "hello",command[100];
    // strcpy(command, password);
    system("echo hello | sudo -S chmod -R 755 /home/vg/work/IIITH/OS/A4/test");
}

void sudo_pw2(){
    char password[] = "hello",command[100];
    // strcpy(command, password);
    system("echo hello | sudo -S chmod -R 110 /home/vg/work/IIITH/OS/A4/test");
}

int 
main(int argc,char *argv[]){
    char ch,*source_file, target_file[100];
    FILE *source, *target;
    source_file = argv[1];
    source = fopen(source_file, "r");

    if(source == NULL){
        perror("Source file error");
    }

    strcpy(target_file,"/home/vg/work/IIITH/OS/A4/test/");

    strcat(target_file, source_file);

    // sudo_pw1();
    target = fopen(target_file, "w");

    if(target == NULL){
        perror("Destination file error");
        exit(1);
    }

    while((ch = fgetc(source)) != EOF)
        fputc(ch, target);

    printf("File copied successfully\n");

    fclose(source);
    fclose(target);

    // sudo_pw2();

    return 0;
}