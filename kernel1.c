#include <stdio.h>
#include "kernel.h"

#define TTY_BUFF_SIZE 1024

FATFS FatFs;
DSTATUS fs_stt;
void init()
{
	serial_init();
	/* Register work area to the default drive */
	fs_stt = f_mount(&FatFs, "", 0);
}
__attribute__ ((interrupt ("IRQ"))) void interrupt_irq() {
   printf("Some IRQs happend\n" );
}

int main(int pc)
{
	init();
	char* buffer = (char*)malloc(TTY_BUFF_SIZE);
	while(1)
	{
		if(!buffer)
			printf("%s\n","Cannot alloc" );
		else
		{
			memset(buffer,'\0', TTY_BUFF_SIZE);
			while(buffer[0] != 'Y')
				tty_readline(buffer,TTY_BUFF_SIZE,1);
	
			edit("test",buffer,TTY_BUFF_SIZE, NULL, NULL);
			printf("Data: %s\n",buffer);
			fflush(stdout);
		}
	}
    return 0;
}