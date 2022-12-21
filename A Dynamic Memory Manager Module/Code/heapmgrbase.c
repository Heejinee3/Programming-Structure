/*--------------------------------------------------------------------*/
/* heapmgrbase.c                                                      */
/* Author: Donghwi Kim, KyoungSoo Park                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "chunkbase.h"

#define FALSE 0
#define TRUE  1

enum {
   MEMALLOC_MIN = 1024,
};

/* gFreeHead: point to first chunk in the free list */
static Chunk_T gFreeHead = NULL;

/* gHeapStart, gHeapEnd: start and end of the heap area.
 * gHeapEnd will move if you increase the heap */
static void *gHeapStart = NULL, *gHeapEnd = NULL;

#ifndef NDEBUG
/* ChunkValidityCheck:
 * Validity check for entire data structures for chunks. Note that this
 * is basic sanity check, and passing this test does not guarantee the
 * integrity of your code. 
 * Returns 1 on success or 0 (zero) on failure. 
 */
static int
CheckHeapValidity(void)
{
   Chunk_T w;

   if (gHeapStart == NULL) {
      fprintf(stderr, "Uninitialized heap start\n");
      return FALSE;
   }

   if (gHeapEnd == NULL) {
      fprintf(stderr, "Uninitialized heap end\n");
      return FALSE;
   }

   if (gHeapStart == gHeapEnd) {
      if (gFreeHead == NULL)
         return 1;
      fprintf(stderr, "Inconsistent empty heap\n");
      return FALSE;
   }

   for (w = (Chunk_T)gHeapStart; 
        w && w < (Chunk_T)gHeapEnd;
        w = ChunkGetNextAdjacent(w, gHeapStart, gHeapEnd)) {

      if (!ChunkIsValid(w, gHeapStart, gHeapEnd)) 
         return 0;
   }

   for (w = gFreeHead; w; w = ChunkGetNextFreeChunk(w)) {
      Chunk_T n;

      if (ChunkGetStatus(w) != CHUNK_FREE) {
         fprintf(stderr, "Non-free chunk in the free chunk list\n");
         return 0;
      }

      if (!ChunkIsValid(w, gHeapStart, gHeapEnd))
         return 0;

      n = ChunkGetNextAdjacent(w, gHeapStart, gHeapEnd);
      if (n != NULL && n == ChunkGetNextFreeChunk(w)) {
         fprintf(stderr, "Uncoalesced chunks\n");
         return 0;
      }
   }
   return TRUE;
}
#endif

/*--------------------------------------------------------------*/
/* SizeToUnits:
 * Returns capable number of units for 'size' bytes. 
 */
/*--------------------------------------------------------------*/
static int
SizeToUnits(int size)
{
  return (size + (CHUNK_UNIT-1))/CHUNK_UNIT;
}
/*--------------------------------------------------------------*/
/* GetChunkFromDataPtr:
 * Returns the header pointer that contains data 'm'. 
 */
/*--------------------------------------------------------------*/
static Chunk_T
GetChunkFromDataPtr(void *m)
{
  return (Chunk_T)((char *)m - CHUNK_UNIT);
}
/*--------------------------------------------------------------------*/
/* InitMyHeap: 
 * Initialize data structures and global variables for
 * chunk management. 
 */
/*--------------------------------------------------------------------*/
static void
InitMyHeap(void)
{
   /* Initialize gHeapStart and gHeapEnd */
   gHeapStart = gHeapEnd = sbrk(0);
   if (gHeapStart == (void *)-1) {
      fprintf(stderr, "sbrk(0) failed\n");
      exit(-1);
   }
}
/*--------------------------------------------------------------------*/
/* MergeChunk:
 * Merge two adjacent chunks and return the merged chunk.
 * Returns NULL on error. 
 */
/*--------------------------------------------------------------------*/
static Chunk_T
MergeChunk(Chunk_T c1, Chunk_T c2)
{
   /* c1 and c2 must be be adjacent */
   assert (c1 < c2 && ChunkGetNextAdjacent(c1, gHeapStart, gHeapEnd) == c2);
   assert (ChunkGetStatus(c1) == CHUNK_FREE);
   assert (ChunkGetStatus(c2) == CHUNK_FREE);

   /* adjust the units and the next pointer of c1 */
   ChunkSetUnits(c1, ChunkGetUnits(c1) + ChunkGetUnits(c2) + 1);
   ChunkSetNextFreeChunk(c1, ChunkGetNextFreeChunk(c2));
   return c1;
}
/*--------------------------------------------------------------------*/
/* SplitChunk:
 * Split 'c' into two chunks s.t. the size of one chunk is 'units' and
 * the size of the other chunk is (original size - 'units' - 1).
 * returns the chunk with 'units'
 * Returns the data chunk. */
/*--------------------------------------------------------------------*/
static Chunk_T 
SplitChunk(Chunk_T c, int units)
{
   Chunk_T c2;
   int all_units;

   assert (c >= (Chunk_T)gHeapStart && c <= (Chunk_T)gHeapEnd);
   assert (ChunkGetStatus(c) == CHUNK_FREE);
   assert (ChunkGetUnits(c) > units + 1); /* assume chunk with header unit */
   
   /* adjust the size of the first chunk */
   all_units = ChunkGetUnits(c);
   ChunkSetUnits(c, all_units - units - 1);

   /* prepare for the second chunk */
   c2 = ChunkGetNextAdjacent(c, gHeapStart, gHeapEnd);
   ChunkSetUnits(c2, units);
   ChunkSetStatus(c2, CHUNK_IN_USE);
   ChunkSetNextFreeChunk(c2, NULL);

   return c2;
}
/*--------------------------------------------------------------------*/
/* InsertChunk:
 * Insert a chunk, 'c', into the head of the free chunk list. 
 * 'c' will be merged with the first chunk if possible.
 * The status of 'c' is set to CHUNK_FREE
 */
/*--------------------------------------------------------------------*/
static void
InsertChunk(Chunk_T c)
{
   assert (ChunkGetUnits(c) >= 1);

   ChunkSetStatus(c, CHUNK_FREE);

   /* If the free chunk list is empty, chunk c points to itself. */
   if (gFreeHead == NULL) {
      gFreeHead = c;
      ChunkSetNextFreeChunk(c, NULL);
   }
   else {
      assert (c < gFreeHead);
      ChunkSetNextFreeChunk(c, gFreeHead);
      if (ChunkGetNextAdjacent(c, gHeapStart, gHeapEnd) == gFreeHead) 
         MergeChunk(c, gFreeHead);
      gFreeHead = c;
   }
}

/* InsertChunkAfter:
 * Insert a chunk, 'c', to the free chunk list after 'e' which is
 * already in the free chunk list. After the operation, 'c' will be the
 * next element of 'e' in the free chunk list.  
 * 'c' will be merged with neighbor free chunks if possible.
 * It will return 'c' (after the merge operation if possible).
 */
/*--------------------------------------------------------------------*/
static Chunk_T
InsertChunkAfter(Chunk_T e, Chunk_T c)
{
   Chunk_T n;

   assert (e < c);
   assert (ChunkGetStatus(e) == CHUNK_FREE);
   assert (ChunkGetStatus(c) != CHUNK_FREE);

   ChunkSetNextFreeChunk(c, ChunkGetNextFreeChunk(e));
   ChunkSetNextFreeChunk(e, c);
   ChunkSetStatus(c, CHUNK_FREE);

   /* see if c can be merged with e */
   if (ChunkGetNextAdjacent(e, gHeapStart, gHeapEnd) == c) 
      c = MergeChunk(e, c);
   
   /* see if we can merge with n */
   n = ChunkGetNextAdjacent(c, gHeapStart, gHeapEnd);
   if (n != NULL && ChunkGetStatus(n) == CHUNK_FREE)
      c = MergeChunk(c, n);

   return c;
}
/*--------------------------------------------------------------------*/
/* RemoveChunkFromList:
 * Removes 'c' from the free chunk list. 'prev' should be the previous
 * free chunk of 'c' or NULL if 'c' is the first chunk
 */
/*--------------------------------------------------------------------*/
static void
RemoveChunkFromList(Chunk_T prev, Chunk_T c)
{
   assert (ChunkGetStatus(c) == CHUNK_FREE);
   
   if (prev == NULL)  
      gFreeHead = ChunkGetNextFreeChunk(c);
   else 
      ChunkSetNextFreeChunk(prev, ChunkGetNextFreeChunk(c));

   ChunkSetNextFreeChunk(c, NULL);
   ChunkSetStatus(c, CHUNK_IN_USE);
}
/* AllocateMoreMemory: 
 * Allocate a new chunk which is capable of holding 'units' chunk
 * units in memory by increasing the heap, and return the new
 * chunk. 'prev' should be the last chunk in the free list.
 * This function also performs chunk merging with "prev" if possible
 * after allocating a new chunk. 
*/
/*--------------------------------------------------------------------*/
static Chunk_T
AllocateMoreMemory(Chunk_T prev, int units)
{
   Chunk_T c;

   if (units < MEMALLOC_MIN)
      units = MEMALLOC_MIN;
   
   /* Note that we need to allocate one more unit for header. */
   c = (Chunk_T)sbrk((units + 1) * CHUNK_UNIT);
   if (c == (Chunk_T)-1)
      return NULL;
   
   gHeapEnd = sbrk(0);
   ChunkSetUnits(c, units);
   ChunkSetNextFreeChunk(c, NULL);
   ChunkSetStatus(c, CHUNK_IN_USE);
   
   /* Insert the newly allocated chunk 'c' to the free list.
    * Note that the list is sorted in ascending order. */
   if (gFreeHead == NULL) 
      InsertChunk(c);
   else 
      c = InsertChunkAfter(prev, c);
 
   assert(CheckHeapValidity());
   return c;
}
/* HeapMgr_malloc:
 * Dynamically allocate a memory capable of holding size bytes. 
 * Substitute for GNU malloc().                                 
 */
/*--------------------------------------------------------------*/
void *
HeapMgr_malloc(size_t size)
{
   static int isInit = FALSE;
   Chunk_T c, prev, pprev;
   int units;
   
   if (size <= 0)
      return NULL;
   
   if (isInit == FALSE) {
      InitMyHeap();
      isInit = TRUE;
   }

   /* see if everything is OK before doing any operations */
   assert(CheckHeapValidity());

   units = SizeToUnits(size);

   pprev = NULL;
   prev = NULL;

   for (c = gFreeHead; 
        c != NULL; 
        c = ChunkGetNextFreeChunk(c)) {

      if (ChunkGetUnits(c) >= units) {
         if (ChunkGetUnits(c) > units + 1) 
            c = SplitChunk(c, units);
         else
            RemoveChunkFromList(prev, c);
         
         assert(CheckHeapValidity());
         return (void *)((char *)c + CHUNK_UNIT);
      }
      pprev = prev;
      prev = c;
   }
   
   /* allocate new memory */
   c = AllocateMoreMemory(prev, units);
   if (c == NULL) {
     assert(CheckHeapValidity());
     return NULL;
   }
   assert(ChunkGetUnits(c) >= units);

   /* if c was merged with prev, pprev becomes prev */
   if (c == prev)
      prev = pprev;

   if (ChunkGetUnits(c) > units + 1) 
      c = SplitChunk(c, units);
   else 
      RemoveChunkFromList(prev, c);
      
   assert(CheckHeapValidity());
   return (void *)((char *)c + CHUNK_UNIT);
}

/*--------------------------------------------------------------*/
/* HeapMgr_free:
 * Releases dynamically allocated memory. 
 * Substitute for GNU free().                                   */
/*--------------------------------------------------------------*/
void
HeapMgr_free(void *m)
{
   Chunk_T c, w, prev;
   
   if (m == NULL)
      return;

   /* check everything is OK before freeing 'm' */
   assert(CheckHeapValidity());

   /* get the chunk header pointer from m */
   c = GetChunkFromDataPtr(m);
   assert (ChunkGetStatus(c) != CHUNK_FREE);
   
   prev = NULL;

   /* traverse the free chunk list to find the
      right location for 'c' */
   for (w = gFreeHead;
        w != NULL;
        w = ChunkGetNextFreeChunk(w)) {
      if (c < w)
         break;
      prev = w;
   }

   /* insert the new free chunk into the free list */
   if (prev == NULL)
      InsertChunk(c);
   else
      InsertChunkAfter(prev, c);

   /* double check if everything is OK */
   assert(CheckHeapValidity());
}
