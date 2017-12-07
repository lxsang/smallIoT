#ifndef TTY_H
#define TTY_H
#include <stdio.h>
#ifdef USE_UART_AS_TTY
#include "../io/uart.h"
#define _getchar() serial_getc()
#define _putchar(c) serial_putc(c)
#else
#define _getchar() getchar()
#define _putchar(c) putchar(c)
#endif
#define to_left() _putchar(27);_putchar(91);_putchar(68)
#define to_right() _putchar(27);_putchar(91);_putchar(67)
#define go_up() _putchar(27);_putchar(91);_putchar(65)
#define go_down() _putchar(27);_putchar(91);_putchar(66)
#define back_space() _putchar(0x8);_putchar(0x20);_putchar(0x8)
#define HISTORY_MAX 5

enum {
	DISCARD_K,
	LEFT_K, 
	RIGHT_K,
	UP_K,
	DOWN_K
} META_KEY;

struct history_entry{
	char* cmd;
	struct history_entry* next;
	struct history_entry* prev;
};

void tty_readline(char*,int,int);
int get_meta_key(char* buff, int idx);
void insert_to_buff(char* buff, char c, int at, int length);
void remove_to_left(char* buff,int at, int length);
void remove_to_right(char* buff, int at, int number);
void tty_set_text(char* buff,char * text,int cursor);
#endif