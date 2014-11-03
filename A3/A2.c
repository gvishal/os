#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/resource.h>

#define ull unsigned long long
#define INF 100000
#define MS 1000000
int main(){
    ull i=0,j=0,count;
    double time_taken;
    struct rusage usage;
    while( i < INF){
        j = 0;
        while(j < INF)
            j++;
        i++;
    }
    getrusage(RUSAGE_SELF, &usage);
    time_taken = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
    time_taken += (double)(usage.ru_utime.tv_usec + usage.ru_stime.tv_usec)/MS;
    count = usage.ru_nivcsw;
    printf("Time Slice(seconds): %lf\n", time_taken/count);
    printf("Time Slice(milli seconds): %lf\n", time_taken/count*1000.0);
    return 0;
}