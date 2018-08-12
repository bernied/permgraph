/******************************************************************************
**
** parse_cl.h
**
** Sat Aug 11 18:57:21 2018
** Darwin 16.7.0 (Darwin Kernel Version 16.7.0: Wed Oct  4 00:17:00 PDT 2017; root:xnu-3789.71.6~1/RELEASE_X86_64) x86_64
** bernied@ptah.local. (Bernhard Damberger)
**
** Header file for command line parser
**
** Automatically created by genparse v0.9.3
**
** See http://genparse.sourceforge.net for details and updates
**
******************************************************************************/

#include <stdio.h>
#include "types.h"

/* customized structure for command line parameters */
struct arg_t
{
  bool h;
  bool v;
  int s;
  int l;
  bool r;
  char * p;
  char * _2;
  bool a;
  bool d;
  bool i;
  bool c;
  bool n;
  int optind;
};

/* function prototypes */
void Cmdline (struct arg_t *my_args, int argc, char *argv[]);
void usage (int status, char *program_name);
