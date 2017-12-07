#ifndef XMODEM_H
#define XMODEM_H
#include <stdio.h>
#include "../io/uart.h"

#ifdef USE_UART_AS_TTY
#define _outbyte(c) serial_putc(c)
#define _inbyte(t) serial_getb(t)
#endif

int xmodemReceive(unsigned char *dest, int destsz);
int xmodemTransmit(unsigned char *src, int srcsz);
#endif