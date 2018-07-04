#include <stdio.h>
#include <syslog.h>

extern char **environ;

int main(int argc, char *argv[])
{
    char **var;
    
    syslog(LOG_INFO, "----------------------------------------\n");
    syslog(LOG_INFO, "argv[1]=%s\n", argv[1]);
    for(var = environ; *var != NULL; ++ var)
        syslog(LOG_INFO, "env=%s\n", *var);   
    syslog(LOG_INFO, "----------------------------------------\n");

    return 0;       
}
