/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <io.h>
#include <text.h>
#include "include/out.h"

int reloc(int loc_x, int loc_y);
struct curPos cursor;
const uint8_t col = 0x07;

void textInit()
{
	scroll(VGA_HEIGHT);
	cursor.x = 0;
	cursor.y = 0;
	cursor.tabwidth = 8;
}

void scroll(unsigned char lines)
{
	int x, y;
	unsigned short *keybuf = (unsigned short *) KEYBUF;
	for (y = 0; y < VGA_HEIGHT - lines; y++)
	{
		for (x = 0; x < VGA_WIDTH; x++)
		{
			keybuf[x + y * VGA_WIDTH] =
					    keybuf[x + (y + lines) * VGA_WIDTH];
		}
	}
	for (; y < VGA_HEIGHT; y++)
	{
		for (x = 0; x < VGA_WIDTH; x++)
		{
			keybuf[x + y * VGA_WIDTH] = ((col << 8) | ' ');
		}
	}
	if (lines >= VGA_HEIGHT)
	{
		cursor.x = 0;
		cursor.y = 0;
		return;
	}
	cursor.y -= lines;
}

void println(char *line)
{
	printf(line);
	putc('\n');
}

void printf(char *line, ...)
{
        int i;
        va_list list;
        va_start(list, line);
        for (i = 0; line[i] != '\0'; i++)
        {
                if (line[i] != '%')
                {
                        putc(line[i]);
                }
                else
                {
                        i++;
                        if (line[i] == '\0')
                        {
                                putc('%');
                                break;
                        }
                        switch (line[i])
                        {
                        case 'd':
                                printDecimalNum(va_arg(list, double), 10);
                                break;
                        case 'i':
                                printNum(va_arg(list, unsigned int), 10, TRUE, FALSE);
                                break;
                        case 'u':
                                printNum(va_arg(list, unsigned int), 10, FALSE, FALSE);
                                break;
                        case 'x':
                                printNum(va_arg(list, unsigned int), 16, FALSE, FALSE);
                                break;
                        case 'X':
                                printNum(va_arg(list, unsigned int), 16, FALSE, TRUE);
                                break;
                        case 'c':
                                putc((char) va_arg(list, unsigned int));
                                break;
                        case 's':
                                printf(va_arg(list, char*));
                                break;
                        case '%':
                                putc('%');
                                break;
                        default:
                                break;
                        }
                }
        va_end(list);
        }
}

char hex[36] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c',
			  'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
char HEX[36] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
			  'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

int atoi(char* str)
{
	int i = 0;
	int idx = 0;
	while (str[i] != '\0' && idx < 0x20000000)
	// (idx < 0x20000000) = overload prevention
	{
		if (str[i] >= 0x30 && str[i] <= 0x39)
		{
			if (0xffffffff - (idx * 10) > (str[i] - 0x30))
			//overload prevention
				idx = idx * 10 + (str[i] - 0x30);
			else
				break;
			i++;
		}
	}
	if (str[0] == '-')
		idx *= -1;
	return idx;
}

int formatInt(void* buffer, int num, unsigned int base, boolean sInt, boolean capital)
{
	if (base > 36 || base < 2)
		return 0;
	int count = 0;
	if ((num < 0) && sInt) //if number is negative print '-' and print absolue value of number (witch is -num)
	{
		*((char*) buffer++) = '-';
		count++;
		num = -num;
	}
	unsigned int i = base, numBase = num / base, buf;
	if (numBase > 0)
	{
		while (i <= numBase)
		{
			i *= base;
			count++;
		}
	}
	else
	{
		count--;
		i = 1;
	}
	for (; i >= 1; i /= base)
	{
		buf = (unsigned int) (num / i);
		*((char*) buffer++) = (capital) ? HEX[buf] : hex[buf];
		num -= buf*i;
	}
	return count + 2;
}

char* itoa(unsigned int index, char* buffer, unsigned int base)
{
	int len = formatInt(buffer, index, base, (base == 10) ? TRUE : FALSE, FALSE); // only signed if base = 10
	buffer[len] = '\0';
	return buffer;
}

int formatDouble(void* buffer, double num, unsigned int base, boolean capital, boolean scientific)
{
	if (base > 36 || base < 2)
		return 0;

	int precision = 0, // persents number after e (or E) when printing scientific.
		count = formatInt(buffer, (int) num, base, TRUE, capital); // print part before dot.

	if (scientific)
	{
		precision = count - 1;
		if (precision != 0) // if we should print scientific AND we have more than 1 digit before dot.
		{
			memcpy((char*) buffer + 2, (char*) buffer + 1, count); // all digits after first digit are moved 1 place to left. (e.g. 12345 becomes 122345)
			*((char*) (buffer + 1)) = '.'; // replace seccond digit with a dot (e.g. 122345 becomes 1.2345)
			count++;
		}
	}
	buffer += count;
	double decimals = num - (double) ((int) num); // get all decimals of the number.
	if (decimals == 0) // check for xx.0
	{
		if (scientific)
		{
			*((char*) buffer++) = (capital) ? 'E' : 'e'; //print exx or Exx
			if (precision >= 0)
				*((char*) buffer++) = '+'; //print leading + (minus is printed by formatInt)
			count += formatInt(buffer, precision, base, TRUE, capital); //print precision
		}
		return count; // appears to be an integer, so we are done now.
	}
	precision += decimals;


	if (precision == 0) // this is 0 for non scientific AND for scientific, but without printed dot.
		*((char*) buffer++) = '.';

	for (; decimals > 0; count++) //loop through all decimals
	{
		decimals *= 10; // move current decimal befor to left side of the dot.
		*((char*) buffer++) = (capital) ? HEX[(int) decimals] : hex[(int) decimals]; //print decimal.
		decimals -= (double) ((int) decimals); //decimal is printed, remove it!
	}

	if (scientific)
	{
		*((char*) buffer++) = (capital) ? 'E' : 'e'; //print exx or Exx
		if (precision >= 0)
			*((char*) buffer++) = '+'; //print leading + (minus is printed by formatInt)
		count += 1 + formatInt(buffer, precision, base, TRUE, capital); //print precision
	}

	return count + 1;
}

char* dtoa(double index, char* buffer, unsigned int base)
{
	int len = formatDouble(buffer, index, base, FALSE, FALSE);
	buffer[len] = '\0';
	return buffer;
}

void printNum(int index, unsigned int base, boolean sInt, boolean capital)
{
	char buf[32];
	memset(buf, '\0', 32);
	int i = 0;

	if (base > 36 || base < 2)
		return;
	if (index == 0)
		putc('0');

	if (index < 0 && sInt)
	{
		putc('-');
		index *= -1;
	}
	unsigned int uIndex = (unsigned int) index;
	for (; uIndex != 0; i++)
	{
		buf[31 - i] = (capital) ? HEX[uIndex % base] : hex[uIndex % base];
		uIndex /= base;
	}
	for (i--; i >= 0; i--)
	{
		putc(buf[31 - i]);
	}
}

void printDecimalNum(double index, unsigned int base)
{
	char buf[64];
	memset(buf, '\0', 64);
	int i = 0;
	int count = strlen(dtoa(index, buf, base));
	for (; i < count; i++)
	{
		putc(buf[i]);
	}
}

/*
void putc(unsigned char i)
{
  uint16_t *keybuf = (uint16_t*) KEYBUF;
  boolean noPrint = FALSE;
  if (i == '\n')
  {
    cursor.y++;
    cursor.x = 0;
    noPrint = TRUE;
  }

  if (i == '\b')
  {
    noPrint = true;
    if (cursor.x == 0)
    {
      cursor.y--;
      cursor.x = 79;
    }

    else
    {
      cursor.x -= 1;
      keybuf[(cursor.x + cursor.y * VGA_WIDTH)-1] = ((OL_WHITE_TXT << 8) | ' ');
    }
  }

  if (i == '\t')
  {
    int i;
    if (cursor.x % cursor.tabwidth == 0)
    {
      putc(' ');
    }
    for (i = 0; i < cursor.x % cursor.tabwidth; i++)
    {
      putc(' ');
    }
    noPrint = TRUE;
  }
  if (cursor.x + 1 > VGA_WIDTH)
  {
    cursor.x %= VGA_WIDTH;
    cursor.y += 1;
  }
  if (cursor.y >= VGA_HEIGHT)
  {
    scroll(cursor.y % VGA_HEIGHT + 1);
  }
  if (!noPrint)
  {
    cursor.x++;
    unsigned short chr = (unsigned short) (col << 8) | (unsigned short) (i & 0x00FF);
    keybuf[cursor.x - 1 + cursor.y * VGA_WIDTH] = chr;
  }
  reloc(cursor.x, cursor.y);
}
 */

void putc(uint8_t c)
{
	uint16_t *vidmem = (uint16_t*) KEYBUF;
	uint32_t i = (cursor.y * VGA_WIDTH) + cursor.x;
	switch (c)
	{
	case '\0':
		break;
	case '\n':
		cursor.x = 0;
		cursor.y++;
		break;
	case '\b':
		if (cursor.x == 0)
		{
			cursor.y--;
			cursor.x = 79;
		}
		else
		{
			cursor.x -= 1;
			vidmem[i - 1] = ((OL_WHITE_TXT << 8) | ' ');
		}
		break;
	case '\t':
		cursor.x += (cursor.tabwidth - cursor.x % cursor.tabwidth);
		if (cursor.x >= VGA_WIDTH)
		{
			cursor.x = 0;
			cursor.y++;
			if (cursor.y >= VGA_HEIGHT)
			{
				scroll(1);
			}
		}
		break;
	default:
		vidmem[i] = ((OL_WHITE_TXT << 8) | c);
		cursor.x += 1;
		if (cursor.x >= VGA_WIDTH)
		{
			cursor.y += (cursor.x / VGA_WIDTH);
			cursor.x %= VGA_WIDTH;
		}
		break;
	}

	if (cursor.y >= VGA_HEIGHT)
	{
		scroll(cursor.y % VGA_HEIGHT + 1);
	}
	reloc(cursor.x, cursor.y);
}

#define PRINTNUM_PADDING '0'
/**
 * \fn sprintnum
 * \brief Write integer numbers according to a format and base number.
 * \param str
 * \brief The string to write to
 * \param min_size
 * \brief How many chars must this fill?
 * \param num
 * \brief What number do we print?
 * \param base
 * \brief The base number for the printed number
 * \param capital
 * \brief Can we use capitals in the text?
 * \param sign
 * \brief Is the integer signed or not?
 * \return The number of chars printed.
 */
int
sprintnum(char* str, size_t min_size, int num, int base, bool capital, bool sign)
{
        if (base > 36 || base < 2)
                return -E_INVALID_ARG;
        /** If num == 0, the result is always the same, so optimize that out */
        if (num == 0)
        {
                int i = 0;
                for (; i < min_size-1; i++)
                {
                        *(str++) = PRINTNUM_PADDING;
                }
                *(str++) = '0';
                return min_size;
        }
        int32_t idx = 0;
        uint32_t unum = (uint32_t)num;
        /** If signedness is allowed, check for signedness */
        if (num < 0 && sign)
                unum = -num;

        char tmp_str[32];
        memset(tmp_str, 0, sizeof(tmp_str));

        /**
         * Convert the integer into an ascii representation
         * This unfortunately reverses the string order
         */
        for (;unum != 0; idx++)
        {
                tmp_str[sizeof(tmp_str) - idx] = (capital) ? HEX[unum % base] :
                                                               hex[unum % base];
                unum /= base;
        }
        /** If signed and negative, append the - sign */
        if (num < 0 && sign)
        {
                tmp_str[sizeof(tmp_str) - idx] = '-';
                idx++;
        }
        int ret = idx;
        /**
         * If the string doesn't cut the minimal length requirements, make it a
         * little longer by appending a couple of characters
         */
        if (idx < min_size)
        {
                int i = 0;
                for (; i < min_size - idx; i++)
                        *(str++) = PRINTNUM_PADDING;
                ret = min_size;
        }
        idx --;
        /**
         * Now take the temp string, reverse it and put it in the output string
         * The reversal to get the correct order again.
         */
        for (; idx >= 0; idx--)
                *(str++) = tmp_str[sizeof(tmp_str) - idx];
        return ret;
}

/**
 * \fn sprintf
 * \brief Print a format to a string
 * \param str
 * \brief The string to print to
 * \param fmt
 * \brief The format
 * \param ...
 * \brief Random arguments which have to match the format
 * \return The number of characters succesfully printed
 */
int sprintf(char* str, char* fmt, ...)
{
        /** Check the preconditions first. */
        if (str == NULL || fmt == NULL)
                return 0;
        int num = 0;
        va_list list;
        va_start(list, fmt);

        /** Itterate through the string to put every character in place. */
        for (; *fmt != '\0'; fmt++, str++, num++)
        {
                /** If formatted? */
                if (*fmt == '%')
                {
                        /** Interpret the format numbering. */
                        int pre  = 0;
                        int post = 0;
                        bool dotted = false;
                        for (; *(fmt + 1) >= '0' && *(fmt + 1) <= '9' ||
                                                       *(fmt + 1) == '.'; fmt++)
                        {
                                if (*(fmt + 1) == '.')
                                {
                                        dotted = true;
                                        continue;
                                }
                                if (dotted)
                                {
                                        post *= 10;
                                        post += (*(fmt+1) - '0');
                                }
                                else
                                {
                                        pre *= 10;
                                        pre += (*(fmt+1) - '0');
                                }
                        }
                        if (pre == 0)
                                pre = 1;
                        if (post == 0)
                                post = 1;
                        int inc = 0;
                        /** Now finally choose the type of format. */
                        switch(*(++fmt))
                        {
                        case 'x':
                                inc = sprintnum(str, pre, (int)va_arg(list, int), 16, false, false);
                                break;
                        case 'X':
                                inc = sprintnum(str, pre, (int)va_arg(list, int), 16, true, false);
                                break;
                        case 'f':
                                break;
                        case 'i':
                                inc = sprintnum(str, pre, (int)va_arg(list, int), 10, false, false);
                                break;
                        case 'd':
                                inc = sprintnum(str, pre, (int)va_arg(list, int), 10, false, true);
                                break;
                        case 'c':
                                inc = 1;
                                *str = (char)va_arg(list, int);
                                break;
                        case 's':
                                inc = sprintf(str, va_arg(list, char*));
                                break;
                        default:
                                *str = *fmt;
                                continue;
                        }
                        /**
                         * Update the looping info.
                         * The -1 below compensates for the increment by the for
                         * loop.
                         */
                        num += inc - 1;
                        str += inc - 1;
                }
                /** Else just copy the character over. */
                else
                        *str = *fmt;
        }

        va_end(list);
        return num;
}

int reloc(int loc_x, int loc_y)
{
	/*
	* Set the new cursor position
	*/
	unsigned short location = loc_y * VGA_WIDTH + loc_x;
	outb(0x3D4, 14);
	outb(0x3D5, location >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, location);
	return 0;
}
