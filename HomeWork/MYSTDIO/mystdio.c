#include "mystdio.h"
#include<stdio.h>
#include<stdarg.h>
#include "softuart.h"

/* 建立自己的printf函数 --- 可以设置任意的串口上 */
void softuart_string(char *p)
{
	while (*p)
	{
		UART3_ruan_DataBuf_Push(*p++);
	}

}
void softuart_printf(const char *fmt, ...)
{
	va_list ap;
	char string[256];//访问内部拓展RAM，非访问外部RAM，不能超过内部拓展RAM大小(此处为1024)

	va_start(ap, fmt);
	vsprintf(string, fmt, ap);//此处也可以使用sprintf函数，用法差不多，稍加修改即可，此处略去
	softuart_string(string);
	va_end(ap);
}




#if 0

#include "mystdio.h"

mini_putc_callback  putc_callback = NULL;

static unsigned int
mini_strlen(const char *s)
{
	unsigned int len = 0;
	while (s[len] != '\0') len++;
	return len;
}

unsigned int mini_atou(const char *buffer)
{
	int ret = 0x00;

	while (*buffer >= '0' && *buffer <= '9')
	{
		ret = ret * 10 + *buffer - '0';
		buffer++;
	}

	return ret;
}

static unsigned int mini_itoa(long value, unsigned int radix, unsigned int uppercase, unsigned int unsig,
	char *buffer, unsigned int zero_pad, unsigned int space_zero)
{
	char	*pbuffer = buffer;
	int	negative = 0;
	unsigned int	i, len;

	/* No support for unusual radixes. */
	if (radix > 16)
		return 0;

	if (value < 0 && !unsig) {
		negative = 1;
		value = -value;
	}

	/* This builds the string back to front ... */
	do {
		int digit = value % radix;
		*(pbuffer++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
	} while (value > 0);

	for (i = (pbuffer - buffer); i < zero_pad; i++)
		*(pbuffer++) = '0';
	for (i = (pbuffer - buffer); i < space_zero; i++)
		*(pbuffer++) = ' ';

	if (negative)
		*(pbuffer++) = '-';

	*(pbuffer) = '\0';

	/* ... now we reverse it (could do it recursively but will
	 * conserve the stack space) */
	len = (pbuffer - buffer);
	for (i = 0; i < len / 2; i++) {
		char j = buffer[i];
		buffer[i] = buffer[len - i - 1];
		buffer[len - i - 1] = j;
	}

	return len;
}

static void
_putc(int ch, struct mini_buff *b)
{
	if (b != NULL)
	{
		if ((unsigned int)((b->pbuffer - b->buffer) + 1) >= b->buffer_len)
			return;
		*(b->pbuffer++) = ch;
		*(b->pbuffer) = '\0';
		return;
	}
	else
	{
		if (putc_callback) { putc_callback((unsigned char)ch); }
	}
}

static void
_puts(char *s, unsigned int len, struct mini_buff *b)
{
	if (b != NULL)
	{
		unsigned int i;

		if (b->buffer_len - (b->pbuffer - b->buffer) - 1 < len)
			len = b->buffer_len - (b->pbuffer - b->buffer) - 1;

		/* Copy to buffer */
		for (i = 0; i < len; i++)
			*(b->pbuffer++) = s[i];
		*(b->pbuffer) = '\0';

		return;
	}
	else
	{
		unsigned int i;
		for (i = 0; i < len; i++)
		{
			if (putc_callback) { putc_callback(s[i]); }
		}
	}
}

void
mini_sprintf(struct mini_buff *b, const char *fmt, va_list va)
{
#ifdef __THREADSAFE_MINI_PRINTF__
	char bf[24];
#else
	static char bf[24];
#endif
	char ch;

	while ((ch = *(fmt++)) != 0)
	{
		if (b != NULL)
		{
			if ((unsigned int)((b->pbuffer - b->buffer) + 1) >= b->buffer_len)
				break;
		}
		if (ch != '%')
			_putc(ch, b);
		else {
			char zero_pad = 0;
			char space_pad = 0;
			char *ptr;
			unsigned int len;

			ch = *(fmt++);

			/* Zero padding requested */
			if (ch == '0') {
				zero_pad = mini_atou(fmt);
				while (*fmt >= '0' && *fmt <= '9') { fmt++; }
				ch = *(fmt++);
			}
			else if (ch >= '0' && ch <= '9')
			{
				space_pad = mini_atou(--fmt);
				while (*fmt >= '0' && *fmt <= '9') { fmt++; }
				ch = *(fmt++);
			}

			switch (ch) {
			case 0:
				goto end;

			case 'u':
			case 'd':
				len = mini_itoa(va_arg(va, unsigned int), 10, 0, (ch == 'u'), bf, zero_pad, space_pad);
				_puts(bf, len, b);
				break;

			case 'x':
			case 'X':
				len = mini_itoa(va_arg(va, unsigned int), 16, (ch == 'X'), 1, bf, zero_pad, space_pad);
				_puts(bf, len, b);
				break;

			case 'l':
				len = mini_itoa(va_arg(va, unsigned long), 10, 0, (ch == 'u'), bf, zero_pad, space_pad);
				_puts(bf, len, b);
				break;

			case 'c':
				_putc((char)(va_arg(va, char)), b);
				break;

			case 's':
				ptr = va_arg(va, char*);
				_puts(ptr, mini_strlen(ptr), b);
				break;

			default:
				_putc(ch, b);
				break;
			}
		}
	}
end:
	return;
}

//void mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...)
//{
//	struct mini_buff b;
//	va_list va;
//
//	b.buffer = buffer;
//	b.pbuffer = buffer;
//	b.buffer_len = buffer_len;
//
//	va_start(va, fmt);
//	mini_sprintf(&b, fmt, va);
//	va_end(va);
//
//	return;
//}

void mini_printf_regiester_callback(mini_putc_callback cb)
{
	putc_callback = cb;
}

void mini_printf(const char *fmt, )
{
	va_list va;
	va_start(va, fmt);
	mini_sprintf(NULL, fmt, va);
	va_end(va);

	return;
}


#endif 






