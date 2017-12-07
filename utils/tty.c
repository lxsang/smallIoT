#include "tty.h"

static struct history_entry * tty_history_first = 0 ;
static struct history_entry* current_history  = 0;
static int history_cnt = 0;

int get_meta_key(char* buff, int idx)
{
	char c,c1;
	c = (char) _getchar();
	switch(c)
	{
		case 91://arrow key
		c1 = (char) _getchar();
		if(c1 == 68) return LEFT_K;
		if(c1 == 67) return RIGHT_K;
		if(c1 == 65) return UP_K;
		if(c1 == 66) return DOWN_K;
		buff[idx] = c1;
		_putchar(c);
		_putchar(c1);
		break;
		
		case 32 ... 90:
		case 92 ... 255:
		_putchar(c);
		buff[idx] = c;
		break;
		
		default:
		_putchar(c);
		break;
	}
	return DISCARD_K;
}
void insert_to_buff(char* buff, char c, int at, int length)
{
	int i;
	for(i = length+1; i > at; i--)
		buff[i] = buff[i-1];
	buff[at] = c;
	// echo back
	//for(i = at+1; i <= length; i++) _putchar(0x08);//back space
	for(i = at; i < length+1; i++) _putchar(buff[i]);
	for(i = at+1; i < length+1; i++){to_left();}
}
void remove_to_left(char* buff,int at, int length)
{
	int i;
	for(i = at-1; i < length-1; i++)
		buff[i] = buff[i+1];
	for(i = at-1; i < length - 1; i++) _putchar(buff[i]);
	buff[length-1] = '\0';
	_putchar(0x20);
	for(i = at; i <= length; i++){to_left();}
}
void remove_to_right(char* buff, int at, int number)
{
	if(buff)
		buff[at+1] = '\0';
	for(int i=0;i<number;i++)
	{
		_putchar(0x20);
	}
	for(int i=0;i<number;i++)
	{
		to_left();
	}
}
void tty_set_text(char* buff,char * text,int cursor)
{
	int i, new_cur;
	int len = strlen(buff);
	// put cursor to the end
	new_cur = cursor;
	for(i=cursor; i< len;i++) {to_right(); new_cur++;};
	// remove to left
	for(i=0;i< new_cur;i++)
	{
		back_space();
	}
	// display new string
	if(text)
	{
		memset(buff,'\0', len);
		memcpy(buff,text,strlen(text));
		//strcpy(buff,text);
	}
	for( i = 0; i < strlen(buff); i++)
		_putchar(buff[i]);
}
static void tty_history_register(const char* cmd)
{
	struct history_entry* tmp = 0;
	int len;
	len = strlen(cmd);
	if(!tty_history_first) // init
	{
		tty_history_first =  (struct history_entry*)malloc(sizeof (struct history_entry));
		tty_history_first->next = 0;
		tty_history_first->prev = 0;
		tty_history_first->cmd = (char*)malloc(len+1);
		memset(tty_history_first->cmd, '\0', len+1);
		strcpy(tty_history_first->cmd,cmd);
	}
	else
	{
		// insert one
		tmp =  (struct history_entry*)malloc(sizeof(struct history_entry));
		tmp->cmd = (char*)malloc(len+1);
		memset(tmp->cmd, '\0', len + 1);
		strcpy(tmp->cmd,cmd);
		tty_history_first->prev = tmp;
		tmp->next = tty_history_first;
		tmp->prev = 0;
		tty_history_first = tmp;
		if(history_cnt < HISTORY_MAX)
			history_cnt++;
		else // remove last
		{
			for(tmp = tty_history_first; tmp && tmp->next != 0; tmp = tmp->next);
			if(tmp->prev) tmp->prev->next =0;
			tmp->prev = 0;
			free(tmp);
		}
	}
}
void tty_readline(char* buff,int length, int nl)
{
	int i=0;
	memset(buff,'\0',length);
	char c;
	int meta = DISCARD_K;
	int cur = 0;
	while(i < length)
	{
		c = (char) _getchar();
		//printf("%d\n",c);
		if(c == 0x0D || c == 0x0A || i == length - 1)
		{
			buff[i] = '\0';
			if(nl)
			{
				_putchar(0x0D);
		    	_putchar(0x0A);
			}
			if(i>0) 
			{
				tty_history_register(buff);
				current_history = tty_history_first;
			}
			break;
		} else
		{
			switch(c)
			{
				case 8:// backspace
				case 127:
				if(cur>0){
					if(cur >= i)
					{
						_putchar(0x08);
						_putchar(0x20);
						_putchar(0x08);
						buff[i] = '\0';
					}
					else
					{
						_putchar(0x08);
						remove_to_left(buff,cur,i);
					}
					cur--;
					i--;
				}
				
				break;
				
				case 27: //ESC
				meta = get_meta_key(buff,i);
				
				if(meta == LEFT_K){
					cur--;
					if(cur >= 0) {to_left();}
					else{cur = 0;}
					
				}
				else if(meta == RIGHT_K){
					cur++;
					if(cur <= i) {to_right();}
					else{cur = i;}
				} else if(meta == UP_K || meta == DOWN_K)
				{
					if(current_history)
					{
						tty_set_text(buff,current_history->cmd,cur);
						cur = i = strlen(current_history->cmd);
						if(meta == UP_K && current_history->next) current_history = current_history->next;
						else if(meta == DOWN_K && current_history->prev) current_history = current_history->prev;
					}
				}
				break;
				
				case 32 ... 126:
				case 128 ... 255:
				
				if(cur == i)
				{
					cur = i;
					meta = DISCARD_K;
					_putchar(c);
					buff[i]  = c;
				}
				else
					insert_to_buff(buff,c,cur,i);
				cur++;
				i++;
				break;
				
				default:
				//_putchar(c);
				break;
			}
		}
	}
}