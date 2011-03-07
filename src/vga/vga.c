#include <text.h>
#include "include/vga.h"

int curPos = 0;

void scroll()
{
}

void println(char *line)
{
	int i;
	for (i = 0; line[i] != '\0'; i++)
	{
		putc(line[i]);
	}
	putc('\n');
}

void putc(char i)
{
	if (i == '\n')
	{
		int tmp = curPos/WIDTH;
		tmp++;
		curPos = curPos%WIDTH+tmp*WIDTH;
	}
	if (curPos/WIDTH >= HEIGHT)
	{
		scroll(HEIGHT-curPos/WIDTH);
	}
	char *keybuf = (char *)KEYBUF;
	keybuf[curPos] = i;
	curPos ++;
}
