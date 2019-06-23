#include <stdio.h>
#define sys_call() {__asm__ __volatile__ ("swi 0x900000+223\n\t");} while(0)

int main(void) {
	sys_call();
	return 0;
}
