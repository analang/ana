#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <stddef.h>

#include "arena.h"

#define COMO_MEM_DEBUG(x, ...)

static inline size_t ana_round_up(size_t sz, size_t al)
{
  size_t x = sz;

  while(x % al != 0)
    x++;

  return x;
}

static ana_block *ana_block_new_sized(size_t size)
{
  ana_block *ar = malloc(sizeof(*ar));

  if(!ar)
    return NULL;

  ar->size = size;
  ar->next = NULL;
  ar->offset = 0;
  ar->mem = malloc(size);
  
  assert(ar->mem);
  memset(ar->mem, 0, size);



  if((uintptr_t)ar->mem % COMO_ARENA_ALIGNMENT != 0)
    ar->mem += 8 - (uintptr_t)ar->mem % COMO_ARENA_ALIGNMENT;

  return ar;
}

static ana_block *ana_block_new(void)
{
  return ana_block_new_sized(COMO_ARENA_BLOCK_SIZE);
}

ana_arena *ana_arena_new_sized(size_t sz)
{
  ana_arena *ar = malloc(sizeof(*ar));

  if(!ar)
  {
    COMO_MEM_DEBUG("warning: failed to acquire arena\n");
    return NULL;
  }

  ar->tblocks = 1;
  ar->block = ana_block_new_sized(sz);

  if(!ar->block)
  {
    COMO_MEM_DEBUG("warning: failed to acquire block allocation\n");
    free(ar);
    return NULL;
  }

  return ar;
}

ana_arena *ana_arena_new(void)
{
  ana_arena *ar = malloc(sizeof(*ar));

  if(!ar)
  {
    COMO_MEM_DEBUG("warning: failed to acquire arena\n");
    return NULL;
  }

  ar->tblocks = 1;
  ar->block = ana_block_new();

  if(!ar->block)
  {
    COMO_MEM_DEBUG("warning: failed to acquire block allocation\n");
    free(ar);
    return NULL;
  }

  return ar;
}


char *ana_arena_alloc(ana_arena *a, size_t sz)
{  
  size_t rounded = sz;

  while(rounded % COMO_ARENA_ALIGNMENT != 0)
    rounded++;

  if(rounded + a->block->offset > a->block->size)
  {
    if(1)
    {
      COMO_MEM_DEBUG("warning: ana_arena_alloc not enough space"
        " for %zu more bytes\n", rounded);

      COMO_MEM_DEBUG("ana_arena_alloc: offset is %zu\n", 
        a->block->offset);

      COMO_MEM_DEBUG("info: searching for a suitable block size\n");
    }
    
    ana_block *block = a->block;

    COMO_MEM_DEBUG("searching for a block suitable for a %zu byte "
      "allocation\n", rounded);

    while(block)
    {
      COMO_MEM_DEBUG("checking block %p's capability to satisfy a "
        "%zu block allocation, where offset is %zu inside a block of %zu bytes "
        "with %zu free bytes therefore\n", 
        (void *)block, rounded, 
        block->offset, 
        block->size, 
        block->size - block->offset);

      if(rounded + block->offset <= block->size) 
      {
        COMO_MEM_DEBUG("...yes\n");

        block->offset += rounded;

        char *addr = (char *)((((char *)block->mem) + block->offset) - rounded);

        return addr;
      }

      block = block->next;
    }

    COMO_MEM_DEBUG("...no\n");
    /* Condition isn't neccessary */
    if(!block)
    {
      if(1)
      {
        COMO_MEM_DEBUG("didn't find a block size capable of a %zu "
          "byte allocation, requesting a new block from OS\n", rounded);
      }

      ana_block *next = ana_block_new_sized(sz < COMO_ARENA_BLOCK_SIZE 
        ? COMO_ARENA_BLOCK_SIZE: sz);

      if(!next) 
      {
        if(0)
        {
          COMO_MEM_DEBUG("ana_arena_alloc: warning failed to "
            "aquire next block\n");
        }

        return NULL;
      }

      next->offset += rounded;
      next->next = a->block;
   
   /* TODO, this currently has a gaping hole. I'm not going to 
      ever get to the original block again probably, since an allocation it 
      might be best to use a doubly linked list, to properly fill up each block,
      and then, when the block IS filled to a multiple of our average allocation
      size, put it in the back. This might be overkill for my current needs
      It's possibile to caclulate fragmentation for each block now.

      Algorithm idea:
      On each allocation of a new block, use the leftover bytes to add to a
      scratch block
    */
      a->tblocks++;
      a->block = next;


      char *addr = (char *)((((char *)next->mem) + next->offset) - rounded);     

      COMO_MEM_DEBUG("returned address is %p\n", (void *)addr);

      return addr;
    }
  }

  a->block->offset += rounded;

  char *addr = (char *)((((char *)a->block->mem) + a->block->offset) - rounded);

  if((uintptr_t)addr % COMO_ARENA_ALIGNMENT != 0)
  {
    addr += 8 - (uintptr_t)addr % COMO_ARENA_ALIGNMENT;
  }

  return addr;
}

void ana_arena_stats(FILE *fp, ana_arena *a)
{
  (void)fp;

  size_t totalbytes  = 0;
  size_t totalblocks = 0;
  size_t totalused = 0;
  
  ana_block *block  = a->block;
  
  while(block)
  {
    size_t fragmented = block->size - block->offset;
    
    printf("block %p was fragmented by %zu bytes\n", 
      (void *)block, fragmented);

    totalbytes += block->size;
    totalused += block->offset;
    totalblocks++;
    block = block->next;
  }

  COMO_MEM_DEBUG("arena at %p with %zu blocks totalling %zu bytes, with %zu "
    "bytes in use\n",(void *)a, totalblocks, totalbytes, totalused);
}

void ana_arena_free(ana_arena *arena)
{
  ana_block *block = arena->block;

  while(block) 
  {
    ana_block *next = block->next;
  
    free(block->mem);
    free(block);

    block = next;
  }

  free(arena);
}
