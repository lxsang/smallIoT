#include "uart.h"
void serial_init()
{
    unsigned int ra;
    PUT32(AUX_ENABLES,1);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_IIR_REG,0xC6);
    PUT32(AUX_MU_BAUD_REG,270);

    ra=GET32(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    ra&=~(7<<15); //gpio15
    ra|=2<<15;    //alt5
    PUT32(GPFSEL1,ra);

    PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,(1<<14)|(1<<15));
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,0);
	PUT32(AUX_MU_CNTL_REG,3);
}
void serial_putc ( unsigned int c )
{
    if(c==0x0A) serial_putc(0x0D);
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x20) break;
    }
    PUT32(AUX_MU_IO_REG,c);
}
void serial_write_hex(unsigned int d)
{
    serial_hex(d);
    serial_putc(0x0D);
    serial_putc(0x0A);
}
void serial_hex(unsigned int d)
{
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        serial_putc(rc);
        if(rb==0) break;
    }
    serial_putc(0x20);
}
void serial_write(char* text, int len)
{
	int r;
	for(r=0;r<len;r++) serial_putc(text[r]);
}
unsigned int serial_getc()
{
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x01) break;
    }
    return GET32(AUX_MU_IO_REG);
}
int serial_getb(unsigned int timeout)// in millisecond
{
	unsigned short c;
	int delay = timeout << 4;

	while ((GET32(AUX_MU_LSR_REG)&0x01) == 0) {
		usleep(60); /* 60 us * 16 = 960 us (~ 1 ms) */
		if (timeout) {
			if (--delay == 0) return -2;
		}
	}

	return (GET32(AUX_MU_IO_REG)) & 0x0FF;
}
int serial_read(char* buff, int length)
{
    int i;
    char c;
	for(i = 0; i < length; i++)
	{
        c  = (char) serial_getc();
        //serial_putc(c);
        if(c == '\r')
        {
            buff[i] = '\r';
            buff[i+1] = '\n';
            return i+2;
        }
        buff[i] = c;
    }
    return i;
}