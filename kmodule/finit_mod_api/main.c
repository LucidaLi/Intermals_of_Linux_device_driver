#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>

int main(void)
{
	int fd, rt;

	fd = open("/home/dennis/workspace/Linux/book/kmodule/A_mod/demodev.ko", O_RDONLY);
	if (fd < 0) {
		perror("open file failed");
		return -1;
	}
	
	rt = syscall(273, fd, NULL, 0);
	if (rt != 0) {
		perror("load kernel module failed");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
