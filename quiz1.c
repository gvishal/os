#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>

int main(){fork(); fork(); fork(); printf("YES ");return 0;}

