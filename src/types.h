
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

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t  uint32;
typedef uint64_t uint64;

typedef struct Circle
{
  struct Circle* next;
  struct Circle* prev;
  struct Circle* opp;
  bool status;
  uint32 p;
} Circle;

#endif
