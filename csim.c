#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

/* === Cache Struct === */

typedef struct line {
  long long t;// tag bit
  int lru;    // lru indicator
  int v;     // valid bit
} line;

typedef line** cache;

/* === Functions === */

int hits = 0;
int misses = 0;
int evicts = 0;

/* build_cache: builds a 2D array of refs to simulate the cache */
cache build_cache(int S, int E) {

  cache C;
  if((C = malloc( S * sizeof(line*) )) == NULL) {
    fprintf(stderr, "build_cache: malloc failed\n");
    exit(1);
  }//end-if

  int i;
  int j;
  for(i = 0; i < S; i++) {
    if((C[i] = malloc( E * sizeof(line) )) == NULL) {
      fprintf(stderr, "build_cache: malloc failed\n");
      exit(1);
    }//end-if
/* set all lru and v to 0 */
    for(j = 0; j < E; j++) {
      C[i][j].lru = 0;
      C[i][j].v = 0;
    }//end-for
  }//end-for

  return C;
}


/* get_set_bits: retrieves the set index from the address */
long long get_set_bits(int s, int b, long long addr) {
  long long mask = (1 << (b+s)) - 1;
  return (addr & mask) >> b;
}

/* get_tag_bits: retrieves the tag bit from the address */
long long get_tag_bits(int s, int b, long long addr) {
  return (unsigned long long) addr >> (s+b);
}

/* incr_set: increments the lru of the heap-allocated set */
void incr_set(line* s, int E) {
  int i;
  for(i = 0; i < E; i++) /*update lru for all refs in the set*/
    (s[i].lru)++;
}

/* reference: simulates a cache reference and records the hits, misses, 
 * and evictions, and updates the cache 
 */
void reference(cache C, long long s, long long tag, int E) {

  line* set = C[s];
  
  /* check the set for hits*/
  int i;
  for(i = 0; i < E; i++) {
    if(set[i].v && set[i].t == tag) {
      hits++;
      incr_set(set, E);
      set[i].lru = 0;
      return;
    }//if-end
  }//for-end

  /* failed to hit */
  misses++;
  /* now look for an open entry */
  int max = -1;
  int mi;
  for(i = 0; i < E; i++) {
    if(!set[i].v) {
      set[i].v = 1;
      incr_set(set, E);
      set[i].lru = 0;
      set[i].t = tag;
      return;
    }//if-end
    /* While moving through, determine the lru entry */
    else if (set[i].lru > max) {
      max = set[i].lru;
      mi = i;
    }//if-end
  }//for-end

  /* failed to find an open entry */
  evicts++;
  /* update lru entry */
  incr_set(set, E);
  set[mi].lru = 0;
  set[mi].t = tag;
  return;

}

/* simulate: simulates the cache and records the hits, misses, and evicts */
void simulate(cache C, char* traces, int s, int b, int E) {
  
  FILE* file;
  char type;
  long long addr;
  int size;
  long long set;
  long long tag;

  /* open the file */
  if((file = fopen(traces, "r")) == NULL) {
    fprintf(stderr, "simulate: failed to open file\n");
    exit(1);
  }//if-end
  /* read each line and process it */
  while(fscanf(file, " %c %llx,%d",  &type, &addr, &size) == 3) {
    set = get_set_bits(s, b, addr);
    tag = get_tag_bits(s, b, addr);
    switch (type) {
      case 'M':
        hits++;
      case 'L':
      case 'S':
        printf("addr: %llx \n", addr);
        reference(C, set, tag, E);
        break;
      case 'I':
        break;
      default:
        fprintf(stderr, "simulate: invalid flag\n");
        exit(1);
    }//switch-end
  }//while-end

  return;
}

int main(int argc, char* argv[])
{
  
  int s;
  int E;
  int b;
  int S;
  char* traces;
  int op;

  while ((op = getopt(argc, argv, "s:E:b:t:")) != -1) {
    switch (op) {
      case 's':
        s = atoi(optarg);
        S = 1 << s;
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        traces = optarg;
        break;
      default:
        fprintf(stderr, "main: invalid flag\n");
        exit(1);
    }//end-switch
  }//end-while

  cache C = build_cache(S, E);
  simulate(C, traces, s, b, E);

  printSummary(hits, misses, evicts);
  return 0;
}
