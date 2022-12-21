/*--------------------------------------------------------------------*/
/* chunkbase.c                                                        */
/* Author: Donghwi Kim, KyoungSoo Park                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "chunkbase.h"

struct Chunk {
   Chunk_T next;       /* Pointer to the next chunk in the free chunk list */
   int units;          /* Capacity of a chunk (chunk units) */
   int status;         /* CHUNK_FREE or CHUNK_IN_USE */
};

/*--------------------------------------------------------------------*/
int
ChunkGetStatus(Chunk_T c)
{
   return c->status;
}
/*--------------------------------------------------------------------*/
void
ChunkSetStatus(Chunk_T c, int status)
{
   c->status = status;
}
/*--------------------------------------------------------------------*/
Chunk_T
ChunkGetNextFreeChunk(Chunk_T c)
{
  return c->next;
}
/*--------------------------------------------------------------------*/
void
ChunkSetNextFreeChunk(Chunk_T c, Chunk_T next)
{
   c->next = next;
}
/*--------------------------------------------------------------------*/
int
ChunkGetUnits(Chunk_T c)
{
   return c->units;
}
/*--------------------------------------------------------------------*/
void
ChunkSetUnits(Chunk_T c, int units)
{
   c->units = units;
}
/*--------------------------------------------------------------------*/
Chunk_T
ChunkGetNextAdjacent(Chunk_T c, void* start, void* end)
{
   Chunk_T n;
   
   assert((void *)c >= start);

   /* Note that a chunk consists of one chunk unit for a header and
    * many chunk units for data. */
   n = c + c->units + 1;
   
   /* If 'c' is the last chunk in memory space, then return NULL. */
   if ((void *)n >= end)
      return NULL;
   
   return n;
}

#ifndef NDEBUG
/*--------------------------------------------------------------------*/
int 
ChunkIsValid(Chunk_T c, void *start, void *end)
/* Return 1 (TRUE) iff c is valid */
{
   assert(c != NULL);
   assert(start != NULL);
   assert(end != NULL);

   if (c < (Chunk_T)start)
      {fprintf(stderr, "Bad heap start\n"); return 0; }
   if (c >= (Chunk_T)end)
      {fprintf(stderr, "Bad heap end\n"); return 0; }
   if (c->units == 0)
      {fprintf(stderr, "Zero units\n"); return 0; }
   return 1;
}

/*--------------------------------------------------------------------*/
void
ChunkPrint(void *start, void *end, Chunk_T freeHead)
{
   Chunk_T w, p = NULL;
   
   printf("heap: %p ~ %p\n", start, end);
   printf("free chunk head: %p\n", (void *)freeHead);
   
   printf("free chain\n");
   for (w = freeHead; w; w = w->next) {
      if (p == w) {
         printf("%p->next == %p, weird!\n", 
                (void *)p, (void *)w); exit(0);
      }
      printf("[%p: %d units]\n", (void *)w, w->units);
      p = w;
      
      if (w->next == freeHead)
         break;
   }
   
   printf("mem\n");
   for (w = (Chunk_T)start;
        w < (Chunk_T)end;
        w = (Chunk_T)((char *)w + (w->units + 1) * CHUNK_UNIT))
     printf("[%p (%c): %d units]\n", 
	    (void *)w, w->next ? 'F' : 'U', w->units);
}
#endif
