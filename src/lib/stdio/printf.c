/*
 * Andromeda
 * Copyright (C) 2011  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \AddToGroup stdio
 * @{
 */
#include <stdio.h>
#include <stdlib.h>

extern char HEX[];
extern char hex[];

int fputc(struct vfile* stream, int chr)
{
	return stream->write(stream, (char*)&chr, 1);
}

int fputs(struct vfile* stream, char* src)
{
	return stream->write(stream, src, strlen(src));
}

int sputs(char* dest, char* src)
{
        int i = 0;
        while(*src != '\0')
        {
                *dest++ = *src++;
                i++;
        }
        return i;
}

#define PRINTNUM_PADDING ' '
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
sprintnum(str, min_size, num, base, capital, sign, padding)
char* str;
size_t min_size;
int num;
int base;
bool capital;
bool sign;
char padding;
{
        if (base > 36 || base < 2)
                return -E_INVALID_ARG;
        /* If num == 0, the result is always the same, so optimize that out */
        if (num == 0)
        {
                int i = 0;
                for (; i < min_size-1; i++)
                        *(str++) = padding;
                *(str++) = '0';
                return min_size;
        }
        int32_t idx = 0;
        uint32_t unum = (uint32_t)num;
        /* If signedness is allowed, check for signedness */
        if (num < 0 && sign)
                unum = -num;

        char tmp_str[32];
        memset(tmp_str, 0, sizeof(tmp_str));

        /*
         * Convert the integer into an ascii representation
         * This unfortunately reverses the string order
         */
        for (;unum != 0; idx++)
        {
                tmp_str[sizeof(tmp_str) - idx] = (capital) ? HEX[unum % base] :
                                                               hex[unum % base];
                unum /= base;
        }
        /* If signed and negative, append the - sign */
        if (num < 0 && sign)
        {
                tmp_str[sizeof(tmp_str) - idx] = '-';
                idx++;
        }
        int ret = idx;
        /*
         * If the string doesn't cut the minimal length requirements, make it a
         * little longer by appending a couple of characters
         */
        if (idx < min_size)
        {
                int i = 0;
                for (; i < min_size - idx; i++)
                        *(str++) = padding;
                ret = min_size;
        }
        idx --;
        /*
         * Now take the temp string, reverse it and put it in the output string
         * The reversal to get the correct order again.
         */
        for (; idx >= 0; idx--)
                *(str++) = tmp_str[sizeof(tmp_str) - idx];
        return ret;
}
/**
 * \fn fprintnum
 * \brief Write integer numbers according to a format and base number.
 * \param stream
 * \brief The stream to write to
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
fprintnum(stream, min_size, num, base, capital, sign, padding)
struct vfile* stream;
size_t min_size;
int num;
int base;
bool capital;
bool sign;
char padding;
{
        int ret;
        int maxlen = 2;

        unsigned long long tmp = base;
        while( tmp < 4294967295UL )
        {
                tmp *= base;
                maxlen++;
        }

        char str[maxlen];
        ret = sprintnum(str, min_size, num, base, capital, sign, padding);
        fputs(stream,str);
        return ret;
}

/**
 * \fn fprintf
 * \brief Write a formatted string to a file
 * \param stream
 * \param fmt
 * \return The number of characters printed
 */
int fprintf(struct vfile* stream, char* fmt, ...)
{
        // No need to check (stream == NULL || fmt == NULL) since we dont rely on these variable being non zero in THIS function. vfprintf may do so, but that why vfprintf has its own check.

        va_list list;
        va_start(list, fmt);

        /* Basically let vfprintf do all the hard work! */
        int ret = vfprintf(stream, fmt, list);

        va_end(list);
        return ret;
}

/**
 * \fn vfprintf
 * \brief Print a format and several arguments to a file
 * \bug If the minimum required number of chars in an int exeeds the max of the
 * \bug type of output, these things aren't taken into account when allocating
 * \bug memory!
 * \param stream
 * \param fmt
 * \param list
 * \return The number of chars printed
 */
int vfprintf(struct vfile* stream, char* fmt, va_list list)
{
        /* Check the preconditions first. */
        if (stream == NULL || fmt == NULL)
                return 0;
        if (stream->write == NULL)
                return 0;

        int num = 0;
        char padding = PRINTNUM_PADDING;
        char* lastWritePosition = fmt;

        /* Itterate through the string to put every character in place. */
        for (; *fmt != '\0'; fmt++, num++)
        {
                /* If formatted? */
                if (*fmt == '%')
                {
                        /* Interpret the format numbering. */
                        int pre  = 0;
                        int post = 0;
                        bool dotted = false;
                        for (; (*(fmt + 1) >= '0' &&
                                *(fmt + 1) <= '9') ||
                                *(fmt + 1) == '.';
                                fmt++)
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
                                        if (pre == 0 && *(fmt+1) == '0')
                                        {
                                                padding = '0';
                                                continue;
                                        }
                                        pre *= 10;
                                        pre += (*(fmt+1) - '0');
                                }
                        }
                        if (pre == 0)
                                pre = 1;
                        if (post == 0)
                                post = 1;
                        int inc = 0;
                        /* Print the part of the format in between the last % and this one. */
                        num += stream->write(stream, lastWritePosition, (unsigned long)fmt - (unsigned long)lastWritePosition - 1);
                        /* Now finally choose the type of format. */
                        switch(*(++fmt))
                        {
                        case 'x': /* Print lower case hex numbers */
                                lastWritePosition = fmt + 1;
                                inc = fprintnum(stream, pre,
                                                         (int)va_arg(list, int),
                                                                             16,
                                                                          false,
                                                                          false,
                                                                       padding);
                                break;
                        case 'X': /* Print upper case hex numbers */
                                lastWritePosition = fmt + 1;
                                inc = fprintnum(stream, pre,
                                                         (int)va_arg(list, int),
                                                                             16,
                                                                           true,
                                                                          false,
                                                                       padding);
                                break;
                        case 'f': /* Print floats (not yet supported) */
                                lastWritePosition = fmt + 1;
                                break;
                        case 'i': /* Print unsigned decimals */
                                lastWritePosition = fmt + 1;
                                inc = fprintnum(stream, pre,
                                                         (int)va_arg(list, int),
                                                                             10,
                                                                          false,
                                                                          false,
                                                                       padding);
                                break;
                        case 'd': /* Print signed decimals */
                                lastWritePosition = fmt + 1;
                                inc = fprintnum(stream, pre,
                                                         (int)va_arg(list, int),
                                                                             10,
                                                                          false,
                                                                           true,
                                                                       padding);
                                break;
                        case 'c': /* Print character */
                                lastWritePosition = fmt + 1;
                                inc = fputc(stream, va_arg(list, int));
                                break;
                        case 's': /* Print string of characters */
                                lastWritePosition = fmt + 1;
                                inc = fputs(stream, va_arg(list, char*));
                                break;
                        default: /* Undefined, just print fmt */
                                lastWritePosition = fmt;
                                continue;
                        }
                        /*
                         * Update the looping info.
                         * The -1 below compensates for the increment by the for
                         * loop.
                         */
                        num += inc - 1;
                        padding = PRINTNUM_PADDING;
                }
        }

        num += stream->write(stream, lastWritePosition, (unsigned long)fmt - (unsigned long)lastWritePosition );

        return num;
}

/**
 * \fn sprintf
 * \brief Print a format to string
 * \param str
 * \param fmt
 * \return The number of characters succesfully printed
 */
int sprintf(char* str, char* fmt, ...)
{
        // No need to check (str == NULL || fmt == NULL) since we dont rely on these variable being non zero in THIS function. vfprintf may do so, but that why vfprintf has its own check.

        /* Set up the variable argument list for vsprintf */
        va_list list;
        va_start(list, fmt);

        /* Now actually do your job */
        int ret = vsprintf(str, fmt, list);

        /* Destroy the variable argument list */
        va_end(list);
        return ret;
}

/**
 * \fn vsprintf
 * \brief Print a format to string using granted variable argument list
 * \param string
 * \param fmt
 * \param list
 * \brief The list of arguments to use
 * \return The number of characters succesfully printed
 */
int vsprintf(char* str, char* fmt, va_list list)
{
        /* Check the preconditions first. */
        if (str == NULL || fmt == NULL)
                return 0;
        int num = 0;
        char padding = PRINTNUM_PADDING;

        /* Itterate through the string to put every character in place. */
        for (; *fmt != '\0'; fmt++, str++, num++)
        {
                /* If formatted? */
                if (*fmt == '%')
                {
                        /* Interpret the format numbering. */
                        int pre  = 0;
                        int post = 0;
                        bool dotted = false;
                        for (; (*(fmt + 1) >= '0' &&
                                *(fmt + 1) <= '9') ||
                                *(fmt + 1) == '.';
                                fmt++)
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
                                        if (pre == 0 && *(fmt+1) == '0')
                                        {
                                                padding = '0';
                                                continue;
                                        }
                                        pre *= 10;
                                        pre += (*(fmt+1) - '0');
                                }
                        }
                        if (pre == 0)
                                pre = 1;
                        if (post == 0)
                                post = 1;
                        int inc = 0;
                        /* Now finally choose the type of format. */
                        switch(*(++fmt))
                        {
                        case 'x': /* Print lower case hex numbers */
                                inc = sprintnum(str, pre,
                                                         (int)va_arg(list, int),
                                                                             16,
                                                                          false,
                                                                          false,
                                                                       padding);
                                break;
                        case 'X': /* Print upper case hex numbers */
                                inc = sprintnum(str, pre,
                                                         (int)va_arg(list, int),
                                                                             16,
                                                                           true,
                                                                          false,
                                                                       padding);
                                break;
                        case 'f': /* Print floats (not yet supported) */
                                break;
                        case 'i': /* Print unsigned decimals */
                                inc = sprintnum(str, pre,
                                                         (int)va_arg(list, int),
                                                                             10,
                                                                          false,
                                                                          false,
                                                                       padding);
                                break;
                        case 'd': /* Print signed decimals */
                                inc = sprintnum(str, pre,
                                                         (int)va_arg(list, int),
                                                                             10,
                                                                          false,
                                                                           true,
                                                                       padding);
                                break;
                        case 'c': /* Print character */
                                inc = 1;
                                *str = (char)va_arg(list, int);
                                break;
                        case 's': /* Print string of characters */
                                inc = sputs(str, va_arg(list, char*));
                                break;
                        default: /* Undefined, just print fmt */
                                *str = *fmt;
                                continue;
                        }
                        /*
                         * Update the looping info.
                         * The -1 below compensates for the increment by the for
                         * loop.
                         */
                        num += inc - 1;
                        str += inc - 1;
                        padding = PRINTNUM_PADDING;
                }
                /* Else just copy the character over. */
                else
                        *str = *fmt;
        }

        return num;
}

/** @} \file */

