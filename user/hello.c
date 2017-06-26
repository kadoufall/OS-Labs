// hello, world
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	
	int i;
	for (i = 3; i >= 1; i--) {
		int re = fork_priority(i);
		if (re == 0) {
			cprintf("get %x \n", i);
			int j;
			for (j = 0; j < 3; j++) {
				cprintf("yielding %x \n", i);
				sys_yield();
			}
			break;
		}
	}
	
	/*
	cprintf("hello, world\n");
	cprintf("i am environment %08x\n", thisenv->env_id);
	*/
}
