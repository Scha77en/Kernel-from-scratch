#ifndef TYPES_H
#define TYPES_H

typedef unsigned char		u8;
typedef signed char		s8;
typedef unsigned short		u16;
typedef signed short		s16;
typedef unsigned int		u32;
typedef signed int		s32;
typedef unsigned long long	u64;
typedef signed long long	s64;

typedef unsigned char	_bool;
#define true	1;
#define false	0;

typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_end(v)      __builtin_va_end(v)

#endif
