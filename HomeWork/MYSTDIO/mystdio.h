#ifndef _MYSTDIO_H_
#define _MYSTDIO_H_
#include "stm8l15x.h"


void softuart_string(char *p);
void softuart_printf(const char *fmt, ...);



#endif 


#if 0
#define __ENABLE_MINI_PRINTF__
//#define __THREADSAFE_MINI_PRINTF__

struct mini_buff {
	char *buffer, *pbuffer;
	unsigned int buffer_len;
};

#ifdef __ENABLE_MINI_PRINTF__
#define my_ptf			mini_printf		
#define my_snptf		mini_snprintf
#else
#define my_ptf			printf				
#define my_snptf		snprintf
#endif

typedef void(*mini_putc_callback) (unsigned char c);
void mini_printf(const char *fmt, ...);
unsigned int mini_atou(const char *buffer);
void mini_sprintf(struct mini_buff *b, const char *fmt, va_list va);
void mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);
void mini_printf_regiester_callback(mini_putc_callback cb);

#endif



