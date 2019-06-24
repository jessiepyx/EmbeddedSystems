#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ALLCHAR "abcdefghijklmnopqrstuvwxyz"

int main()
{
	int fd = open("/dev/matrix", O_WRONLY | O_NONBLOCK);
	if (fd < 0)
	{
		printf("ERROR: Cound not open matrix device!\n");
		return -1;
	}
	write(fd, ALLCHAR, strlen(ALLCHAR) + 1);
	return 0;
}
