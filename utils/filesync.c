#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "xmodem.h"
#define error_message printf
static int serial_fp = 0;

void _outbyte(int c)
{
	char _c = (char)(c&0xFF);
	write (serial_fp, &_c,1);
}
int _inbyte(unsigned int timeout)// in millisecond
{
	char c;
	int delay = timeout << 4;

	while (read(serial_fp,&c,1) == 0) {
		usleep(60); /* 60 us * 16 = 960 us (~ 1 ms) */
		if (timeout) {
			if (--delay == 0) return -2;
		}
	}
	return (int)c&0x0FF;
}

int set_interface_attribs (int fd, int speed, int parity)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		error_message ("error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
	// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
	// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		error_message ("error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}

void set_blocking (int fd, int should_block)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		error_message ("error %d from tggetattr", errno);
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 0;            // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
		error_message ("error %d setting term attributes", errno);
}


int main (int argc, char const *argv[])
{
	char *portname;
	char* filename;
	char buff[1024];
	int size = 0;
	if(argc != 3)
	{
		printf("USAGE: %s [port] [file]\n", argv[0]);
		return 0;
	}
	portname = argv[1];
	filename = argv[2];
	FILE* fp = fopen(filename,"rb");
	if(!fp)
	{
		error_message("error when opening :%s \n", filename);
		return 0;
	}
	printf("OPenning serial\n");
	serial_fp = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

	if (serial_fp < 0)
	{
		error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
		return 0;
	}
	printf("OPen serial success\n");
	set_interface_attribs (serial_fp, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (serial_fp, 0);                // set no blocking
	printf("Sync file name\n");
	xmodemTransmit (filename, sizeof(filename));
	//send file content
	printf("Send file content\n");
	while((size = fread(buff,1,sizeof(buff), fp))>0)
	{
		printf("transmit %d\n", size);
		xmodemTransmit (buff, size);
	}	
	printf("Send done\n");
	return 0;
}