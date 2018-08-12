#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "parse_cl.h"

#define VERSION "0.4"
#define FILE_NAME_SIZE 256
#define GRAPH_SIZE(x) ((x) * (x-1) / 2)

struct arg_t args;

void
init_default_args(struct arg_t* args)
{
  args->h = false;
  args->v = false;
  args->s = -1;
  args->l = -1;
  args->r = false;
  args->i = false;
  args->d = false;
  args->a = false;
  args->c = false;
  args->n = false;
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
    if (args->l >= 1 || args->r) {
      error("Can't use -p, -l or -r simultaneously.");
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
  }

  if (args->l >= 1 && args->r) {
      error("Can't use -p, -l or -r simultaneously.");
  }

  if (args->s <= 0) {
    error("Must specify size of permutations.\n");
  }

  if (args->r)
  {
    unsigned int seed = time(NULL);
    srand(seed);
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
    error("Unable to allocate memory for the permutations!\n");
  }
  perm[0] = size;
  return perm;
}

void
print_perm(FILE* fp, uint32* permutation)
{
  fprintf(fp, "(%u", permutation[1]);
  for (int i=2; i <= permutation[0]; i++) {
    fprintf(fp, ", %u", permutation[i]);
  }
  fprintf(fp, ")");
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
 * Random number n where i <= n <= j
 */
uint32
rand_uint32(uint32 i, uint32 j)
{
  assert(i > 0); // permutation arrays are 1 based
  assert(j >= i);

  uint32 d = j - i + 1;  // 1 <= d <= j

  // get unbiased random value
  uint32 r, rm = RAND_MAX - ((RAND_MAX+1L) % (uint64)d);
  do {
    r = rand();
  } while (r > rm);      // 0 <= r <= rm

  uint32 x = r % d;      // 0 <= x <= d - 1
  uint32 n = x + i;      // i <= n <= j

  assert(n >= i);
  assert(n <= j);

  return n;
}

void
randomize_permutation(uint32* permutation)
{
  uint32 size = permutation[0], tmp;
  for (uint32 i = 1; i <= size-1; i++)
  {
      uint32 j = rand_uint32(i, size);
      tmp = permutation[i];
      permutation[i] = permutation[j];
      permutation[j] = tmp;
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
    error("Unable to allocate memory for the graph!\n");
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
graph_to_dot(uint32* graph, uint32* permutation, char* name)
{
  char file_name[FILE_NAME_SIZE];
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
print_node(FILE* fp, Circle* c)
{
    fprintf(fp, "%p\t%u\t%p\t%p\t%p\t%s\n",
                                    c,
                                    c->p,
                                    c->next,
                                    c->prev,
                                    c->opp,
                                    c->status ? "prime" : "");
}

void
print_circle(FILE* fp, Circle* circle)
{
  Circle* c = circle;
  do
  {
    print_node(fp, c);
    c = c->next;
  } while (c && c != circle);
}

void
map_perm_to_circle(uint32* permutation, Circle* circle)
{
  Circle *curr, *n;
  Circle *prev = circle;

    // Initialize circle w/ prime values
  for (int i=permutation[0]; i > 0 ; i--)
  {
    curr = (Circle*) malloc(sizeof(Circle));
    if (!curr) {
      error("Unable to allocate memory for circle node!");
    }
    curr->p = permutation[i];
    curr->status = true;
    curr->opp = NULL;
    curr->prev = prev;
    prev->next = curr;
    prev = curr;
  }
  curr->next = circle;
  circle->prev = curr;

  bool* skipped = (bool*) calloc(permutation[0]+1, sizeof(bool));
  if (!skipped) {
    error("Unable to allocate memory for circle skip detection!");
  }

    // Initialize the non-prime fields
  for (uint32 i=1; i <= permutation[0]; i++)
  {
    if (skipped[i] == false)
    {
      // skip to right before i
      while(curr != circle)
      {
        skipped[curr->p] = true;
        curr = curr->prev;
        if (curr->p == i) {
          break;
        }
      }
    }

    // draw node
    n = (Circle*) malloc(sizeof(Circle));
    if (!n) {
      error("Unable to allocate node for circle!");
    }
    n->p = i;
    n->status = false;
    n->opp = NULL;
    n->prev = curr->prev;
    n->next = curr;
    curr->prev->next = n;
    curr->prev = n;
    skipped[i] = true;
    curr = n;
  }

  // assign "opp" values to its corresponding opposite
  // This is a stupid O(N^2) algorithm. But its good enough.
  for (int i=1; i <= permutation[0]; i++)
  {
    curr = circle;
    n = NULL;
    while (curr)
    {
      if (curr->p == i)
      {
        n = curr;
        break;
      }
      curr = curr->next;
    }

    curr = curr->next;
    while (curr)
    {
      if (curr->p == i)
      {
        n->opp = curr;
        curr->opp = n;
        break;
      }
      curr = curr->next;
    }
  }
}

void
circle_to_gv(Circle* circle, uint32* permutation, char* name)
{
  Circle *c, *d;
  uint32 size = permutation[0];
  char file_name[FILE_NAME_SIZE];

  sprintf(file_name, "%s.gv", name);
  FILE* fp = fopen(file_name, "w+");
  if (fp == NULL) {
    error("Unable to open file.\n");
  }

  fprintf(fp, "graph %s {\n", name);
  fprintf(fp, "  labelloc=\"t\";\n");
  fprintf(fp, "  label=\"");
  print_perm(fp, permutation);
  fprintf(fp, "\";");

  c = circle->prev;
  do
  {
    if (c->status) {
      fprintf(fp, "p%u [shape=\"point\",xlabel=\"%u'\"];\n", c->p, c->p);
    }
    else {
      fprintf(fp, "n%u [shape=\"point\",xlabel=\"%u\"];\n", c->p, c->p);
    }
    c = c->prev;
  } while (c && c != circle);

  fprintf(fp, "\n");
  c = circle->prev;
  while (c && c != circle) // LAMb: this is broken!
  {
    fprintf(fp, "%s%u -- ", c->status ? "p" : "n", c->p);
    c = c->prev;
  }
  c = circle->prev; // LAMb: fix this as above!
  fprintf(fp, "%s%u", c->status ? "p" : "n", c->p);
  fprintf(fp, " [style=\"solid\",splines=\"curved\"];\n");

  fprintf(fp, "\n");
  for (uint32 i=1; i <= size; i++) {
    fprintf(fp, "n%u -- p%u;\n", i, i); // LAMb: replace w/ opp check loop
  }
/*
  c = circle->next;
  bool
  do
  {
    if (c->p != c->next->p) {
      fprintf(fp, "  n%u -- p%u;\n", c->p, c->p);
    }
  } while (c && c != circle);
*/
  fprintf(fp, "\n}\n");
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

Circle*
perm_to_circle_to_gv(uint32* permutation, char* name)
{
  Circle* circle = (Circle*) calloc(1, sizeof(Circle));
  if (!circle) {
    error("Unable to allocate memory for Circle graph!");
  }
  map_perm_to_circle(permutation, circle);
  circle_to_gv(circle, permutation, name);

  return circle;
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
  char name[FILE_NAME_SIZE] = "permutation";
  char cname[FILE_NAME_SIZE] = "circle";
  char inv_name[FILE_NAME_SIZE] = "permutation_inv";
  char inv_cname[FILE_NAME_SIZE] = "circle_inv";
  int len = strlen(name);
  int clen = strlen(cname);
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
    if (!histogram) {
      error("Unable to allocate memory for histogram!");
    }
  }

  if (args.p || args.r)
  {
    if (args.r)
    {
      permutation = alloc_permutation(size, true);
      randomize_permutation(permutation);
    }
    else if (args.p)
    {
      permutation = alloc_permutation(size, false);
      str_to_permutation(args.p, permutation, false);
    }
    else {
      error("Missing a permutation: expected -p or -r to be set!");
    }
    if (args._2)
    {
      uint32* p2 = alloc_permutation(size, false);
      str_to_permutation(args._2, p2, false);
      apply_permutation(p2, permutation);
      free(p2);
    }

    map_perm_to_graph(permutation, graph);
    if (args.d) {
      graph_to_dot(graph, permutation, name);
    }
    if (args.c) {
      perm_to_circle_to_gv(permutation, cname);
    }

    if (histogram) {
      count_edges(graph, histogram, size);
    }

    if (args.n)
    {
      if (args.d)
      {
        uint32* igraph = alloc_graph(size);
        invert_permutation(permutation);
        map_perm_to_graph(permutation, igraph);
        graph_to_dot(igraph, permutation, inv_name);
      }
      if (args.c) {
        perm_to_circle_to_gv(permutation, inv_cname);
      }
    }

  }
  else if (args.l <= 0)
  {
    if (size > 7) {
      error("Can not generate more then 6! files.");
    }
    permutation = alloc_permutation(size, true);
    uint32 numPerms = factorial(size);
    for (int i=0; i < numPerms; i++)
    {
      map_perm_to_graph(permutation, graph);
      if (args.d) {
        graph_to_dot(graph, permutation, make_name(name, i));
      }
      if (args.c) {
        perm_to_circle_to_gv(permutation, make_name(cname, i));
      }
      if (histogram) {
        count_edges(graph, histogram, size);
      }
      name[len] = '\0';
      cname[clen] = '\0';
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
          graph_to_dot(graph, permutation, make_name(name, i));
        }
        if (args.c) {
          perm_to_circle_to_gv(permutation, make_name(cname, i));
        }

        if (args.n)
        {
          uint32* igraph = alloc_graph(size);
          invert_permutation(permutation);
          if (args.d)
          {
            map_perm_to_graph(permutation, igraph);
            graph_to_dot(igraph, permutation, make_name(inv_name, i));
          }
          if (args.c) {
            perm_to_circle_to_gv(permutation, make_name(inv_cname, i));
          }
        }

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

  // We don't do any memory free'ing since this is a small application
  // and it doesn't use much memory.
  exit(result);
}
