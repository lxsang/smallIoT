#include <stdio.h>
#include "kernel.h"

#define TTY_BUFF_SIZE 512

FATFS FatFs;
DSTATUS fs_stt;
void init()
{
	//setbuf(stdout, NULL);
	//setvbuf(stdout, NULL, _IONBF, 0);
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
    
    //char str1[20];
    //read(, str1,16);
    //printf("Entered Name %d: %s\n",stdin ,str1);
    //char c = serial_getc();
    //printf("init image\n");
    pinMode(16,OUTPUT);
    digitalWrite(16,HIGH);
    FILE *fp;
    object firstProcess;
    char *p, buffer[1024], cmd[1024];
    char* code;

    initMemoryManager();

    strcpy(buffer, "image.im");
    p = buffer;
    fp = fopen(p, "r");

	if (fp == NULL) {
		sysError("cannot open image", p);
		exit(1);
    }
    imageRead(fp);
    initCommonSymbols();
    digitalWrite(16,LOW);
	while(vm_execute(globalSymbol("systemProcess"),15000));
    return 0;
}