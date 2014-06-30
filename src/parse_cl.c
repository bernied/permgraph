/******************************************************************************
**
** parse_cl.c
**
** Sun Jun 29 22:59:31 2014
** Linux 3.2.0-23-generic-pae (#36-Ubuntu SMP Tue Apr 10 22:19:09 UTC 2012) i686
** vagrant@precise32 (vagrant)
**
** C file for command line parser
**
** Automatically created by genparse v0.8.7
**
** See http://genparse.sourceforge.net for details and updates
**
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "parse_cl.h"

static struct option const long_options[] =
{
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {"size", required_argument, NULL, 's'},
  {"permutation", required_argument, NULL, 'p'},
  {"histogram", no_argument, NULL, 'i'},
  {"dot", no_argument, NULL, 'd'},
  {"average", no_argument, NULL, 'a'},
  {NULL, 0, NULL, 0}
};

/*----------------------------------------------------------------------------
**
** Cmdline ()
**
** Parse the argv array of command line parameters
**
**--------------------------------------------------------------------------*/

void Cmdline (struct arg_t *my_args, int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int errflg = 0;

  my_args->h = false;
  my_args->v = false;
  my_args->i = false;
  my_args->d = false;
  my_args->a = false;

  optind = 0;
  while ((c = getopt_long (argc, argv, "hvs:p:ida", long_options, &optind)) != - 1)
    {
      switch (c)
        {
        case 'h':
          my_args->h = true;
          usage (EXIT_SUCCESS, argv[0]);
          break;

        case 'v':
          my_args->v = true;
          break;

        case 's':
          my_args->s = atoi (optarg);
          break;

        case 'p':
          my_args->p = atoi (optarg);
          break;

        case 'i':
          my_args->i = true;
          break;

        case 'd':
          my_args->d = true;
          break;

        case 'a':
          my_args->a = true;
          break;

        default:
          usage (EXIT_FAILURE, argv[0]);

        }
    } /* while */

  if (errflg)
    usage (EXIT_FAILURE, argv[0]);

  if (optind >= argc)
    my_args->optind = 0;
  else
    my_args->optind = optind;
}

/*----------------------------------------------------------------------------
**
** usage ()
**
** Print out usage information, then exit
**
**--------------------------------------------------------------------------*/

void usage (int status, char *program_name)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, "Try `%s --help' for more information.\n",
            program_name);
  else
    {
      printf ("\
Usage: %s [OPTION]... [FILE]\n\
\n\
  -h, --help              display this help and exit\n\
  -v, --version           output version information and exit\n\
  -s, --size              size of permutation\n\
  -p, --permutation       lexical permutation\n\
  -i, --histogram         generate histogram\n\
  -d, --dot               generate dot files\n\
  -a, --average           print average number of edges for given # of permutations\n\
\n", program_name);
    }
  exit (status);
}
