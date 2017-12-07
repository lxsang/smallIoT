#ifndef FENTO_H
#define FENTO_H

#include "tty.h"
#define clear() printf("\033[H\033[J");fflush(stdout)
#define ED_ROWS 20
#define ED_COLS 50 
#define ADV_FORMAT

struct editor_data
{
#ifdef STATIC_SCREEN
	char screen[ED_ROWS][ED_COLS];
#else
	char **screen;
#endif
	void (*commit_evt)(struct editor_data* );
	void (*exit_evt)(struct editor_data* );
	char* current_buff;
	int x;
	int y;
	int line_cnt;
	int stat;
};

enum {
	ED_OK,
	ED_INIT_ROWS_FAIL,
	ED_INIT_COLS_FAIL,
	ED_EDITED,
	ED_COMMITED,
	ED_DISCARD
} ED_STAT;
int edit(char* title,char* buff, int length,void (*commit_evt)(struct editor_data*),void (*exit_evt)(struct editor_data*));
//int ed_get_text(char*);
void edit_end();
void ed_question(char*,int);
#endif