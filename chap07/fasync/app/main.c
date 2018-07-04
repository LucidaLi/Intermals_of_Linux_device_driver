#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define DEVFILE     "/dev/fa_dev"
static unsigned long eflag = 1;

static void sigio_handler(int sigio)
{
    printf("Get the SIGIO signal, we exit the application!\n");
    eflag = 0;
}

static int block_sigio(void)
{
    sigset_t set, old;
    int ret;

    sigemptyset(&set);
    sigaddset(&set, SIGIO);
    sigprocmask(SIG_BLOCK, &set, &old);
    ret = sigismember(&old, SIGIO);
    return ret;
}

static void unblock_sigio(int blocked)
{
    sigset_t set;
    if(!blocked){
        sigemptyset(&set);
        sigaddset(&set, SIGIO);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
    }   
}

int main(void)
{
    int fd;
    struct sigaction sigact, oldact;
    int oflag;
    int blocked;

    blocked = block_sigio();
    
    sigemptyset(&sigact.sa_mask);
    sigaddset(&sigact.sa_mask, SIGIO);
    sigact.sa_flags = 0;
    sigact.sa_handler=sigio_handler;
    if(sigaction(SIGIO, &sigact, &oldact) < 0){
        printf("sigaction failed!\n");
        unblock_sigio(blocked);
        return -1;
    }   
    unblock_sigio(blocked);    

    fd = open(DEVFILE, O_RDWR);
    if(fd >= 0){
        fcntl(fd, F_SETOWN, getpid());
        oflag = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, oflag | FASYNC);
        printf("Do everything you want until we get a signal...\n");
        while(eflag);
        close(fd);
    } 

    return 0;       
}
