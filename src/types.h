#ifndef __TYPES__
#define __TYPES__

#include <sys/types.h>
#include <ctype.h>

#ifndef bool
typedef enum bool_t
{
  false = 0, true
} bool;
#endif

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;
// typedef uint8_t  uint8;
// typedef uint32_t  uint32;
// typedef uint8_t uint64;


#endif