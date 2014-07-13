#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "parse_cl.h"

#define VERSION "0.2"

#define GRAPH_SIZE(x) ((x) * (x-1) / 2)

struct arg_t args;
static char SKAT_CARDS_A_LO[] = { 'A', '7', '8', '9', '0', 'J', 'Q', 'K' };
static char SKAT_CARDS_A_HI[] = { '7', '8', '9', '0', 'J', 'Q', 'K', 'A' };

void
init_default_args(struct arg_t* args)
{
  args->h = false;
  args->v = false;
  args->s = -1;
  args->l = -1;
  args->i = false;
  args->d = false;
  args->a = false;
  args->c = false;
  args->e = false;
}

/*
 * Provides user feed back on how to use permgraph.
 */
void
error(char* msg)
{
  if (msg) {
    fprintf(stderr, "%s\n", msg);
  }
  exit(EXIT_FAILURE);
}

/*
 * Returns number of numbered elements in permutation (by counting ,s).
 */
int
is_valid_permutation(char* perm)
{
  if (!perm) {
    return 0;
  }

  int i=0, m=1;
  char* p =perm;;
  char c = *p++;

  do
  {
    if (m)
    {
      if (c == ',')
      {
        m=0;
        i++;
      }
      else if (c < '0' || c > '9') {
        return 0;
      }
    }
    else
    {
      if (c == ',') {
        return 0;
      }
      else if (c < '0' || c > '9') {
        return 0;
      }
      m=1;
    }

    c = *p++;
  } while (c != '\0');

  if (p[-2] == ',') {
    return 0;
  }

  return i+1;
}

// 7 8 9 0 j q k a
// 1 2 3 4 5 6 7 8
// a 7 8 9 0 j q k
void
convert_skat_permutation(char* p, bool hi)
{
  char* q = p;
  do
  {
    switch(*q)
    {
      case '\0':
        break;

      case ',':
        break;

      case '0':
        *q = hi ? '4' : '5';
        break;

      case '7':
        *q = hi ? '1' : '2';
        break;

      case '8':
        *q = hi ? '2' : '3';
        break;

      case '9':
        *q = hi ? '3' : '4';
        break;

      case 'a': case 'A':
        *q = hi ? '8' : '1';
        break;

      case 'j': case 'J':
        *q = hi ? '5' : '6';
        break;

      case 'k': case 'K':
        *q = hi ? '7' : '8';
        break;

      case 'q': case 'Q':
        *q = hi ? '6' : '7';
        break;

      default:
        printf("%c\n", *q);
        error("Invalid skat permutation: must be A, 7, 8, 9, 0, J, Q or K!");
    }
  } while(*q++);
}

/*
h / help              flag        "  display this help and exit"
v / version           flag        "  output version information and exit"
s / size              int         "  size of permutation"
l / lexical           int         "  lexical permutation"
p / permutation       string      "  comma delimited permutation"
2 / second            string      "  2nd permutation to apply to first"
a / average           flag        "  print average number of edges for given # of permutations"
d / dot               flag        "  generate dot files"
i / histogram         flag        "  generate histogram"
c / cards             flag        "  use skat cards for permutation"
e / ace               flag        "  ace hi, defaults to ace low"
*/
char*
handle_arguments(int argc, char** argv, struct arg_t* args)
{
  init_default_args(args);
  Cmdline(args, argc, argv);

  if (args->v) {
    printf("permgraph version %s\n", VERSION);
  }

  uint32 size1 =0, size2;
  if (args->p)
  {
    if (args->c) {
      convert_skat_permutation(args->p, args->e);
      if (args->_2) {
        convert_skat_permutation(args->_2, args->e);
      }
    }

    if (args->l >= 1) {
      error("Can't use -p and -l simultaneously.");
    }

    size1 = is_valid_permutation(args->p);
    if (!size1) {
      error("Given permutation (-p) is not a comma delimted permutation.");
    }

    if (args->_2)
    {
      size2 = is_valid_permutation(args->_2);
      if (!size2) {
        error("Given 2nd permutation (-2) is not a comma delimted permutation.");
      }

      if (size1 != size2) {
        printf("%d, %d\n", size1, size2);
        error("Given permutation and 2nd permutation are not the same size.");
      }
    }

    if (args->s <= 0) {
      args->s = size1;
    }
    else
    {
      if (args->s != size1) {
        error("Given permutation size and specified permutation are not the same size.");
      }
    }
  }
  else
  {
    if (args->_2) {
      error("Must first specify the permtuation (-p) before -2 can be used.");
    }
    else if (args->c) {
      error("Must use -c/--card with -p/--permutation");
    }
  }

  if (args->s <= 0) {
    error("Must specify size of permutations.\n");
  }

  return argv[args->optind];
}

/*
 * Constructor to allocate and initialize permutation array.
 * NOTE: position zero contains the size, so the array is 1 based not 0 based!
 */
uint32*
alloc_permutation(uint32 size, bool init)
{
  uint32* perm =NULL;
  if (init)
  {
    size_t perm_size = (size + 1) * sizeof(uint32);
    perm = (uint32*) malloc(perm_size * sizeof(uint32));

    for (int i=1; i <= size; i++) {
      perm[i] = i;
    }
  }
  else {
    perm = (uint32*) calloc(size + 1, sizeof(uint32));
  }

  if (!perm) {
    error("Can't allocate enough memory for the permutations.\n");
  }
  perm[0] = size;
  return perm;
}

void
print_perm(FILE* fp, uint32* permutation)
{
  fprintf(fp, "{%u", permutation[1]);
  for (int i=2; i <= permutation[0]; i++) {
    fprintf(fp, ", %u", permutation[i]);
  }
  fprintf(fp, "}");
}

void
check_permutation(uint32* permutation, int i, uint32 v)
{
  if (permutation[i] != 0) {
    error("Invalid permutation: value has already been assigned.");
  }
  if (v > permutation[0]) {
    print_perm(stdout, permutation);
    error("Invalid permutation: value is larger then given permtuation size.");
  }
  if (v <= 0) {
    error("Invalid permutation: value is negative.");
  }
}

/*
 * Assumes the string is in a comma delimted format and has been "validated".
 */
void
str_to_permutation(char* str, uint32* permutation, bool print)
{
  char* p =str;
  char c = *p++;
  int i=1;
  uint32 v =0;

  while (c != '\0')
  {
    if (print) print_perm(stdout, permutation);
    if (c == ',')
    {
      check_permutation(permutation, i, v);
      permutation[i] = v;
      i++;
      v = 0;
    }
    else
    {
      v *= 10;
      v += c - '0';
    }

    c = *p++;
  }
  check_permutation(permutation, i, v);
  permutation[i] = v;
  if (print) print_perm(stdout, permutation);

  uint32* inv_perm = alloc_permutation(permutation[0], false);
  for (i=1; i <= permutation[0]; i++)
  {
    if (permutation[i] == 0) {
      error("Invalid permutation: one of the values has not been assigned.");
    }
    if (inv_perm[permutation[i]] != 0) {
      error("Invalid permutation: one of the values is repeating.");
    }
    inv_perm[permutation[i]] = i;
  }

  free(inv_perm);
}

void
apply_permutation(uint32* src, uint32* dst)
{
  if (src[0] != dst[0]) {
    error("Internal error: attempted to apply permutation with wrong size.");
  }

   uint32* p = alloc_permutation(src[0], false);
   for (int i=1; i <= src[0]; i++) {
    p[i] = dst[src[i]];
   }

   for (int i=1; i <= dst[0]; i++) {
    dst[i] = p[i];
   }

   free(p);
}

uint32*
alloc_graph(uint32 nodes)
{
  size_t num_verts = GRAPH_SIZE(nodes); // square symetric matrix ignoring diagonal
  uint32* graph = (uint32*) calloc(num_verts, sizeof(uint32));
  if (!graph) {
    error("Can't allocate enough memory for the graph.\n");
  }

  return graph;
}

/* taken from http://www.dreamincode.net/code/snippet4206.htm */
/*
 * To generate a permuation we use the following algorithm...
 */
int
lex_permute(uint32 *array, int size)
{
  int l, r, tmp;
  if (size < 2)
    return 0;
  l = size - 2;
  while (l >= 0 && array[l] > array[l+1])
    --l;

  if (l < 0)
    return 0;
  r = size - 1;
  while (array[r] < array[l])
    --r;
  tmp = array[l];
  array[l] = array[r];
  array[r] = tmp;
  r = size - 1;
  ++l;
  while (l < r) {
    tmp = array[l];
    array[l++] = array[r];
    array[r--] = tmp;
  }
  return 1;
}

uint32
factorial(uint32 x)
{
  uint32 r = 1;
  for (int i=2; i <= x; i++) {
    r *= i;
  }
  return r;
}

uint32
LT(uint32 src, uint32 dst)
{
  if (src < dst) {
    uint32 tmp = src;
    src = dst;
    dst = tmp;
  }
  return (src * (src-1) / 2) + dst;
}

uint32*
invert_permutation(uint32* permutation)
{
  uint32* x = permutation;
  uint32 n = permutation[0];
  int i, j, m;

  for (i = 1; i <= n; ++i) {
    x[i] = -x[i] - 1;
  }
  for (m = n; m > 0; --m)
  {
    for (i = m, j = x[m]; j > 0; i = j, j = x[j]);
    x[i] = x[-j - 1], x[-j - 1] = m;
  }
  return permutation;
}

void
map_perm_to_graph(uint32* permutation, uint32* graph)
{
  // there is an edge  ij for any two indices i and j for which i < j and p[i] > p[j]
  invert_permutation(permutation);
  uint32 size = permutation[0];
  memset(graph, 0, GRAPH_SIZE(size)*sizeof(uint32));
  for (int i=0; i < size; i++)
  {
    for (int j=i+1; j < size; j++)
    {
      // i < j && p[i] > p[j]
      if (i < j && permutation[i+1] > permutation[j+1]) {
        graph[LT(i, j)] = 1;
      }
    }
  }
  invert_permutation(permutation);
}

char*
make_name(char* name, int i)
{
  int len = strlen(name);
  sprintf(name+len, "%03d", i);
  return name;
}

void
graph_to_dot(uint32* graph, uint32* permutation, char* name, bool skat, bool hi)
{
  char file_name[100];
  uint32 size = permutation[0];

  sprintf(file_name, "%s.dot", name);
  FILE* fp = fopen(file_name, "w+");
  if (fp == NULL) {
    error("Unable to open file.\n");
  }

  fprintf(fp, "graph %s {\n", name);
  fprintf(fp, "  labelloc=\"t\";\n");
  fprintf(fp, "  label=\"");
  print_perm(fp, permutation);
  fprintf(fp, "\";");

  if (skat)
  {
    char* cards = hi ? SKAT_CARDS_A_HI : SKAT_CARDS_A_LO;
    for (int i=0; i < size; i++)
    {
      if (cards[i] == '0') {
        fprintf(fp, "  %d [ label=\"10\" ];\n", i+1);
      }
      else {
        fprintf(fp, "  %d [ label=\"%c\" ];\n", i+1, cards[i]);
      }
    }
  }
  fprintf(fp, "\n\n");

  for (int i=0; i < size; i++)
  {
    for (int j=i; j < size; j++)
    {
      if (i != j && graph[LT(i, j)] != 0) {
        fprintf(fp, "  %d -- %d;\n", i+1, j+1);
      }
    }
  }
  fprintf(fp, "}\n");

  fclose(fp);
}

void
count_edges(uint32* graph, uint32* histogram, uint32 size)
{
  uint32 gs = GRAPH_SIZE(size);
  uint32 s =0;
  for (int i=0; i < gs; i++) {
    if (graph[i] == 1) {
      s++;
    }
  }
  histogram[s]++;
}

/*
 *
 */
int
main(int argc, char** argv)
{
  int result;
  struct stat sb;
  char* file_name;
  char name[100] = "permutation";
  int len = strlen(name);
  uint32* histogram =NULL;
  uint32 gs, hgs;

  // Handle arguments
  file_name = handle_arguments(argc, argv, &args);

  int size = args.s;
  uint32* permutation;
  uint32* graph = alloc_graph(size);
  gs = GRAPH_SIZE(size);
  hgs = gs + 1;

  if (args.i || args.a) {
    histogram = (uint32*) calloc(hgs, sizeof(uint32));
  }

  if (args.p)
  {
    permutation = alloc_permutation(size, false);
    str_to_permutation(args.p, permutation, false);
    if (args._2)
    {
      uint32* p2 = alloc_permutation(size, false);
      str_to_permutation(args._2, p2, false);
      apply_permutation(p2, permutation);
      free(p2);
    }

    map_perm_to_graph(permutation, graph);
    if (args.d) {
      graph_to_dot(graph, permutation, name, args.c, args.e);
    }
    if (histogram) {
      count_edges(graph, histogram, size);
    }
  }
  else if (args.l <= 0)
  {
    if (size > 7) {
      error("Can not generate more then 7! files.");
    }
    permutation = alloc_permutation(size, true);
    uint32 numPerms = factorial(size);
    for (int i=0; i < numPerms; i++)
    {
      map_perm_to_graph(permutation, graph);
      if (args.d) {
        graph_to_dot(graph, permutation, make_name(name, i), args.c, args.e);
      }
      if (histogram) {
        count_edges(graph, histogram, size);
      }
      name[len] = '\0';
      lex_permute(permutation+1, size);
    }
  }
  else
  {
    permutation = alloc_permutation(size, true);
    uint32 numPerms = factorial(size);
    for (int i=0; i < numPerms; i++)
    {
      if (i == args.l)
      {
        map_perm_to_graph(permutation, graph);
        if (args.d) {
          graph_to_dot(graph, permutation, make_name(name, i), args.c, args.e);
        }
        name[len] = '\0';
        break;
      }
      lex_permute(permutation+1, size);
    }
  }

  if (args.i)
  {
    for (int i=0; i < hgs; i++) {
      printf("%i\t", i);
    }
    printf("\n");
    for (int i=0; i < hgs; i++) {
      printf("%i\t", histogram[i]);
    }
    printf("\n");
  }

  if (args.a)
  {
    uint64 sum =0, count =0;
    for (int i=0; i < hgs; i++) {
      sum += histogram[i] * i;
      count += histogram[i];
    }
    double d = sum / (double) count;
    printf("average: %llu, %llu, %f\n", sum, count, d);
  }

  if (histogram) free(histogram);
  free(permutation);
  free(graph);
  exit(result);
}
