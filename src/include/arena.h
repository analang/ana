#ifndef COMO_ARENA_H
#define COMO_ARENA_H

/*
 * Use this arena for tasks, don't use it for the GC, as it makes debugging
   almost impossible with valgrind. This is currently only used for the parser, and AST
   construction.

   TODO:
   Implement an average allocation tracking metric
 */

#define COMO_ARENA_BLOCK_SIZE 4096
#define COMO_ARENA_ALIGNMENT 8

struct _ana_block;

typedef struct _ana_arena
{
  size_t tblocks;
  struct _ana_block *block;
} ana_arena;

typedef struct _ana_block {
  struct _ana_block *next;
  size_t offset;
  size_t size;
  void *mem;
} ana_block;

ana_arena *ana_arena_new(void);
ana_arena *ana_arena_new_sized(size_t sz);
char *ana_arena_alloc(ana_arena *a, size_t sz);
void ana_arena_free(ana_arena *arena);
void ana_arena_stats(FILE *fp, ana_arena *a);


#endif