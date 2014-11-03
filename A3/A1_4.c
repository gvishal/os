#include <stdio.h>
#include <sys/resource.h>

int main()
{
    struct rlimit rl;
    getrlimit(RLIMIT_NPROC, &rl);
    printf("Soft limit: %d\n", rl.rlim_cur);
    printf("Hard limit: %d\n", rl.rlim_max);
}