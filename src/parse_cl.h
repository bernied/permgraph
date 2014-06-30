/******************************************************************************
**
** parse_cl.h
**
** Sun Jun 29 22:59:31 2014
** Linux 3.2.0-23-generic-pae (#36-Ubuntu SMP Tue Apr 10 22:19:09 UTC 2012) i686
** vagrant@precise32 (vagrant)
**
** Header file for command line parser
**
** Automatically created by genparse v0.8.7
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
  int p;
  bool i;
  bool d;
  bool a;
  int optind;
};

/* function prototypes */
void Cmdline (struct arg_t *my_args, int argc, char *argv[]);
void usage (int status, char *program_name);
