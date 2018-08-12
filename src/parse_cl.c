/******************************************************************************
**
** parse_cl.c
**
** Sat Aug 11 18:57:21 2018
** Darwin 16.7.0 (Darwin Kernel Version 16.7.0: Wed Oct  4 00:17:00 PDT 2017; root:xnu-3789.71.6~1/RELEASE_X86_64) x86_64
** bernied@ptah.local. (Bernhard Damberger)
**
** C file for command line parser
**
** Automatically created by genparse v0.9.3
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
  {"lexical", required_argument, NULL, 'l'},
  {"random", no_argument, NULL, 'r'},
  {"permutation", required_argument, NULL, 'p'},
  {"second", required_argument, NULL, '2'},
  {"average", no_argument, NULL, 'a'},
  {"dot", no_argument, NULL, 'd'},
  {"histogram", no_argument, NULL, 'i'},
  {"circle", no_argument, NULL, 'c'},
  {"invert", no_argument, NULL, 'n'},
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
  my_args->r = false;
  my_args->p = NULL;
  my_args->_2 = NULL;
  my_args->a = false;
  my_args->d = false;
  my_args->i = false;
  my_args->c = false;
  my_args->n = false;

  optind = 0;
  while ((c = getopt_long (argc, argv, "hvs:l:rp:2:adicn", long_options, &optind)) != - 1)
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

        case 'l':
          my_args->l = atoi (optarg);
          break;

        case 'r':
          my_args->r = true;
          break;

        case 'p':
          my_args->p = optarg;
          break;

        case '2':
          my_args->_2 = optarg;
          break;

        case 'a':
          my_args->a = true;
          break;

        case 'd':
          my_args->d = true;
          break;

        case 'i':
          my_args->i = true;
          break;

        case 'c':
          my_args->c = true;
          break;

        case 'n':
          my_args->n = true;
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
Usage: %s [OPTION]...\n\
\n\
  -h, --help              display this help and exit\n\
  -v, --version           output version information and exit\n\
  -s, --size              size of permutation\n\
  -l, --lexical           lexical permutation\n\
  -r, --random            random permutation\n\
  -p, --permutation       comma delimited permutation\n\
  -2, --second            2nd permutation to apply to first\n\
  -a, --average           print average number of edges for given # of permutations\n\
  -d, --dot               generate dot files\n\
  -i, --histogram         generate histogram\n\
  -c, --circle            generate circle graph\n\
  -n, --invert            include inverted version of permutation\n\
\n", program_name);
    }
  exit (status);
}
