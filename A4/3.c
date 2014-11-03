#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

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

    target = fopen(target_file, "w");

    if(target == NULL)
        perror("Destination file error");

    while((ch = fgetc(source)) != EOF)
        fputc(ch, target);

    printf("File copied successfully\n");

    fclose(source);
    fclose(target);

    return 0;
}