#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "parse_cl.h"

#define VERSION "0.1"

#define GRAPH_SIZE(x) ((x) * (x-1) / 2)

struct arg_t args;

void
init_default_args(struct arg_t* args)
{
  args->h = false;
  args->v = false;
  args->s = -1;
  args->p = -1;
  args->i = false;
  args->d = false;
  args->a = false;
}

/*
 * Provides user feed back on how to use check_deck.
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
h / help              flag        "  display this help and exit"
v / version           flag        "  output version information and exit"
s / size              int         "  size of permutation"
p / permutation       int         "  lexical permutation"
i / histogram         flag        "  generate histogram"
d / dot               flag        "  generate dot files"
a / average           flag        "  print average number of edges for given # of permutations"
*/
/*
 *
 */
char*
handle_arguments(int argc, char** argv, struct arg_t* args)
{
  init_default_args(args);
  Cmdline(args, argc, argv);

  if (args->v) {
    printf("permgraph version %s\n", VERSION);
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
alloc_permutation(uint32 size)
{
  size_t perm_size = (size + 1) * sizeof(uint32);
  uint32* perm = (uint32*) malloc(perm_size * sizeof(uint32));
  if (!perm) {
    error("Can't allocate enough memory for the permutations.\n");
  }
  perm[0] = size;

  for (int i=1; i <= size; i++) {
    perm[i] = i;
  }

  return perm;
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

void
print_perm(FILE* fp, uint32* permutation)
{
  fprintf(fp, "{%u", permutation[1]);
  for (int i=2; i <= permutation[0]; i++) {
    fprintf(fp, ", %u", permutation[i]);
  }
  fprintf(fp, "}");
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
graph_to_dot(uint32* graph, uint32* permutation, char* name)
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
  fprintf(fp, "\";\n\n");
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
  uint32* permutation = alloc_permutation(size);
  uint32* graph = alloc_graph(size);
  gs = GRAPH_SIZE(size);
  hgs = gs + 1;

  if (args.i || args.a) {
    histogram = (uint32*) calloc(hgs, sizeof(uint32));
  }

  if (args.p <= 0)
  {
    uint32 numPerms = factorial(size);
    for (int i=0; i < numPerms; i++)
    {
      map_perm_to_graph(permutation, graph);
      if (args.d) {
        graph_to_dot(graph, permutation, make_name(name, i));
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
    uint32 numPerms = factorial(size);
    for (int i=0; i < numPerms; i++)
    {
      if (i == args.p)
      {
        map_perm_to_graph(permutation, graph);
        if (args.d) {
          graph_to_dot(graph, permutation, make_name(name, i));
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
