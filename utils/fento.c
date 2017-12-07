#include "fento.h"

// the screen buffer
static struct editor_data* editor;
static int editing = 0;
// default commit action
static void ed_commit(struct editor_data* ed)
{
	char text[10];
	int j;
	sprintf(text,"Commit?");
	ed_question(text, 10);
	if(text[0] == 'Y'|| text[0] == 'y')
	{
		edit_end();
		editor->stat = ED_COMMITED;
	}
}
void edit_end()
{
	editing = 0;
}
// default exit action
static void ed_exit(struct editor_data* ed)
{
	char text[10];
	int j;
	sprintf(text,"Commit?");
	ed_question(text, 10);
	if(text[0] == 'Y'|| text[0] == 'y')
	{
		editor->stat = ED_COMMITED;
	}
	else // just quit
	{
		editor->stat = ED_DISCARD;
	}
	edit_end();
}

static void go_to_line(int y)
{
	int line = y;
	int i,len;
	if( y < 0) line=0;
	if(y >= ED_ROWS) line=ED_ROWS-1;
	if(line > editor->line_cnt) return;
	if(line == editor->y) return;
	len = strlen(editor->screen[line]);
	if(line > editor->y)
	{
		for(editor->y; editor->y < line ; editor->y++)
		{
			go_down();
		}
	}
	else //if(line < editor->y)
	{
		for(editor->y; editor->y > line ; editor->y--)
		{
			go_up();
		}
	}
	// go to cols
	if(editor->x >= len)
	{
		for(editor->x;editor->x > len; editor->x--)
		{
			to_left();
		}
	}
}

static void ed_new_line()
{
	int i,len, old_cur;
	int space_line;
	if(editor->line_cnt == ED_ROWS -1) return; // no more line available
	editor->line_cnt++;
	len = strlen(editor->screen[editor->y]);
	old_cur = editor->x;
	//if(old_cur != len)
	//	remove_to_right(NULL,old_cur,len-old_cur);
	if(editor->y == editor->line_cnt - 1)
	{
		// make new line
		go_to_line(editor->y+1);
	}
	else
	{
		// expand line
		space_line = editor->y+1;
		for(i = editor->line_cnt; i > space_line; i--)
		{
			go_to_line(i);
			tty_set_text(editor->screen[i],editor->screen[i-1],editor->x);
			editor->x = strlen(editor->screen[i]);
		}
		go_to_line(space_line);
	}
	memset(editor->screen[editor->y],'\0', strlen(editor->screen[editor->y]));
	if(old_cur != len)
	{
		// cut the remainder then put to new line
		for(i = 0; i < len - old_cur;i++)
			editor->screen[editor->y][i] = editor->screen[editor->y-1][old_cur + i];
		editor->screen[editor->y-1][old_cur] = '\0';
		/*if(c == 0x0D|| c == 0x0A)
		{
			editor->screen[editor->y-1][old_cur] = '\0';
			editor->screen[editor->y-1][old_cur+1] = '\n';
		}
		else
			editor->screen[editor->y-1][old_cur] = '\0';*/
		
	}
	tty_set_text(editor->screen[editor->y], NULL,editor->x);	
	//move cursor to the left
	for(i=0; i<= len-old_cur;i++)
		{ to_left();}
	editor-> x = 0;
}

static void ed_remove_line()
{
	int cur_len, prev_len, tmp,i;
	if(editor->x > 0) return;
	if(editor->y == 0) return;
	// check the size of the current 
	//line with the previous line
	cur_len = strlen(editor->screen[editor->y]);
	prev_len = strlen(editor->screen[editor->y-1]);
	if(cur_len+prev_len > ED_COLS - 1)
	{
		// concat a sub string to the prev. line:
		// search for a space near the cut point
		tmp = ED_COLS - prev_len - 1;
		while(tmp>0 && editor->screen[editor->y][tmp] != 0x20) tmp--;
		if(tmp<=0) return; // not found
		// update buffer
		for(i=0; i < tmp; i++)
		{
			editor->screen[editor->y-1][prev_len+i] =  editor->screen[editor->y][i];
		}
		editor->screen[editor->y-1][prev_len+tmp] = '\0';
		for(i=tmp; i < cur_len; i++)
		{
			editor->screen[editor->y][i-tmp] =  editor->screen[editor->y][i];
		}
		editor->screen[editor->y][cur_len-tmp] = '\0';
		tty_set_text(editor->screen[editor->y],NULL,editor->x);
		editor->x = strlen(editor->screen[editor->y]);
		remove_to_right(NULL,cur_len-tmp,tmp);
		go_to_line(editor->y-1);
		tty_set_text(editor->screen[editor->y],NULL,editor->x);
		
	}
	else
	{
		// concat the current string to
		// the prev line then delete
		// the current line and put the cursor
		// to the end of the line
		strcat(editor->screen[editor->y-1],editor->screen[editor->y]);
		tmp = editor->y - 1;
		for(i=editor->y; i< editor->line_cnt;i++)
		{
			tty_set_text(editor->screen[i],editor->screen[i+1],editor->x);
			editor->x = strlen(editor->screen[i]);
			go_to_line(i+1);
		}
		//go_to_line(editor->line_cnt);
		tty_set_text(editor->screen[editor->line_cnt], "",editor->x);
		editor->line_cnt--;
		go_to_line(tmp);
		tty_set_text(editor->screen[tmp],0,editor->x);
	}
	editor->x = prev_len;
	for(i=prev_len; i < strlen(editor->screen[editor->y]);i++)
	{
		to_left();
	}
}

static int ed_error(int code)
{
	printf("ERROR: Init\n");
	return code;
}

static int screen_init(char* text)
{
	int i,j;
	if(!editor)
	{
		editor = (struct editor_data*)malloc(sizeof(struct editor_data));
#ifndef STATIC_SCREEN
		editor->screen = (char**)malloc(ED_ROWS*sizeof(char*));
		if(!editor->screen) return ed_error(ED_INIT_ROWS_FAIL);
		for(i=0;i< ED_ROWS;i++)
		{
			if(!(editor->screen[i] = (char*)malloc(ED_COLS)))
				return ed_error(ED_INIT_COLS_FAIL);
		}
#endif
		editor->commit_evt = ed_commit;
		editor->exit_evt = ed_exit;
		editor->x = 0;
		editor->y=0;
		editor->line_cnt=0;
		editor->stat = ED_COMMITED;
	}
	for(i = 0; i< ED_ROWS;i++)
		memset(editor->screen[i],'\0',ED_COLS);
	// draw editor area
	// first line is the header
#ifdef  ADV_FORMAT
	printf("\033[100;97;1m");
	j = (ED_COLS - strlen(text))/2;
	for(i=0; i < j; i++)
		printf(" ");
	printf("%s",text);
	for(i=0; i < j; i++)
		printf(" ");
	printf("\033[0m\r\n");
	fflush(stdout);
#endif
	// make the rows
	for(i=0;i< ED_ROWS+1; i++)
	{
		_putchar(0x0D);
	    _putchar(0x0A);
		editor->y++;
	}
	// go back to top
	go_to_line(0);
	return ED_OK;
}
static void screen_destroy()
{
	if(editor)
	{
#ifndef STATIC_SCREEN
		int i;
		if(editor->screen)
		{
			for(i=0; i < ED_ROWS; i++)
				if(editor->screen[i]) free(editor->screen[i]);
			free(editor->screen);
		}
#endif
		free(editor);
		editor = 0;
	}
}
void ed_question(char* buff, int length)
{
	int i,len;
	for(i = editor->y; i <= ED_ROWS;i++)
	{
		go_down();
	}
	for(i=editor->x; i> 0; i--)
	{
		to_left();
	}
	printf("%s",buff);
	len = strlen(buff);
	fflush(stdout);
	memset(buff,'\0',length);
	tty_readline(buff, length,0);
	len += strlen(buff);
	for(i=0; i < len; i++)
	{
		back_space();
	}
	for(i = editor->y; i <= ED_ROWS;i++)
	{
		go_up();
	}
	for(i=0; i < editor->x ; i++)
	{
		to_right();
	}
	
}
static void edit_line()
{
	int i=0;
	int j;
	//memset(buff,'\0',ED_COLS);
	char c;
	int meta = DISCARD_K;
	int line;
	while(editing)
	{
		line = editor->y;
		i = strlen(editor->screen[editor->y]);
		c = (char) _getchar();
		switch(c)
		{
			case 8:// backspace
			case 127:
			if(editor->x>0){
				if(editor->x >= i)
				{
					back_space();
					editor->screen[line][i-1] = '\0';
				} 
				else
				{
					_putchar(0x08);
					remove_to_left(editor->screen[line],editor->x,i);
				}
				editor->x--;
				//i--;
			}else
			{
				ed_remove_line();
				//i = strlen(editor->screen[editor->y]);
			}
			editor->stat = ED_EDITED;
			break;
			
			case 0x0D:
			case 0x0A:
			ed_new_line();
			editor->stat = ED_EDITED;
			break;
			
			case 9: // tab - insert two space
				if(i != ED_COLS-2)
				{
					editor->stat = ED_EDITED;
					editor->screen[line][i]  = 0x20;
					editor->screen[line][i+1]  = 0x20;
					_putchar(0x20);
					_putchar(0x20);
					//i+=2;
					editor->x+=2;
				}
				else
				{
					
				}
			break;
			
			case 19: //ctrl +s
			if(editor->commit_evt) 
			{
				editor->stat = ED_COMMITED;
				editor->commit_evt(editor);
			}
			break;
			
			case 24: //ctrl +x
			if(editor->exit_evt) editor->exit_evt(editor);
			break;
			
			case 27: //ESC
			meta = get_meta_key(editor->screen[line],i);
			
			if(meta == LEFT_K){
				editor->x--;
				if(editor->x >= 0) {to_left();}
				else{editor->x = 0;}
				
			}
			else if(meta == RIGHT_K){
				editor->x++;
				if(editor->x <= i) {to_right();}
				else{editor->x = i;}
			} else if(meta == UP_K||meta == DOWN_K)
			{
				if(meta == UP_K)
					go_to_line(editor->y-1);
				else
					go_to_line(editor->y+1);
				//i = strlen(editor->screen[editor->y]);
			}
			break;
			
			case 32 ... 126:
			case 128 ... 255:
			if(i == ED_COLS-1)
			{
				
			}
			else
			{
				editor->stat = ED_EDITED;
				if(editor->x == i)
				{
					editor->x = i;
					meta = DISCARD_K;
					_putchar(c);
					editor->screen[line][i]  = c;
				}
				else
					insert_to_buff(editor->screen[line],c,editor->x,i);
				editor->x++;
				//i++;
			}
			break;
			
			default:
			//_putchar(c);
			break;
		}
	}
}
static int ed_get_text(char* buff, int length)
{
	int i,j, len,tmp;
	if(!buff) return 0;
	if(editor->line_cnt == 0 && strlen(editor->screen[0]) == 0) return 0;
	len = 0;
	for(i = 0; i<= editor->line_cnt; i++)
	{
		tmp = strlen(editor->screen[i]);
		if(tmp> 0)
		{
			for(j=0;j<tmp;j++)
				buff[len+j] = (char)editor->screen[i][j];
			len += tmp;
			//buff[len] = '\n';
		}
	}
	return len;
}
int edit(char* title,char *buff,int length,void (*commit_evt)(struct editor_data*),void (*exit_evt)(struct editor_data*))
{
	editing = 1;
	clear();
	int len;
	screen_init(title);
	if(commit_evt) editor->commit_evt = commit_evt;
	if(exit_evt) editor->exit_evt = exit_evt;
	edit_line();
	if(buff && editor->stat == ED_COMMITED)
	{
		memset(buff,'\0', length);
		len = ed_get_text(buff, length);
	}
	screen_destroy();
	clear();
	return len;
}