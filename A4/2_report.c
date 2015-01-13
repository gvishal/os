#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(){
    system("perf stat -e faults ./2");
    system("perf stat -e dTLB-loads ./2");
    system("perf stat -e dTLB-loads-misses ./2");
    return 0;
}