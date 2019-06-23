#include <linux/unistd.h>
#include <sys/syscall.h>

int main() {
	syscall(223);
	return 0;
}
