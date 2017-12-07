#include "fento.h"
static volatile int keepRunning = 1;

static char   buff[1024];

int main (int argc, char const *argv[])
{
	printf("Enter raw mode\r\n");
	system ("/bin/stty raw");
	edit("Workspace",buff,1024,NULL,NULL);
	//tty_readline(buff,80);
	
	printf("Return to cook mode\r\n");
	 system ("/bin/stty cooked");
	 printf("\r\n%s\r\n",buff);
	 return 0;
}