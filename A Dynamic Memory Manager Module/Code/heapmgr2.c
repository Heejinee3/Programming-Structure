/*--------------------------------------------------------------------*/
/* heapmgr2.c                                                         */
/* Name: Heejin Jeong                                                 */
/* Student ID: 20170616                                               */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "chunk.h"

#define FALSE 0
#define TRUE  1

enum {
   MEMALLOC_MIN = 1024,
};

/* gFreeHead: each element points to first chunk in the free list */
static Chunk_T gFreeHead[1024];

/* gHeapStart, gHeapEnd: start and end of the heap area.
 * gHeapEnd will move if you increase the heap 
 */
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
      if (gFreeHead[0] == NULL)
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
/* UnitsToIndex:
 * Returns index that corresponds for units. 
 */
/*--------------------------------------------------------------*/
static int
UnitsToIndex(int units)
{   
   int index;

   if (units <= 1023){
      index = units-1;
   }
   else{
      index = 1023;
   }
   return index;
}

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
   gFreeHead[0] = NULL;
   /* Initialize gHeapStart and gHeapEnd */
   gHeapStart = gHeapEnd = sbrk(0);
   if (gHeapStart == (void *)-1) {
      fprintf(stderr, "sbrk(0) failed\n");
      exit(-1);
   }
}

/*--------------------------------------------------------------------*/
/* LeftMergeChunk:
 * Merge two adjacent chunks and return the merged chunk.
 * Left chunk is freed and right chunk is allocated.
 * Returns NULL on error. 
 */
/*--------------------------------------------------------------------*/
static Chunk_T
LeftMergeChunk(Chunk_T c1, Chunk_T c2)
{
   /* c1 and c2 must be be adjacent */
   assert (c1 < c2 && ChunkGetNextAdjacent(c1, gHeapStart, gHeapEnd) == c2);
   assert (ChunkGetStatus(c1) == CHUNK_FREE);
   assert (ChunkGetStatus(c2) == CHUNK_IN_USE);

   Chunk_T prev = ChunkGetPrevFreeChunk(c1);
   int index = UnitsToIndex(ChunkGetUnits(c1));

   /* adjust the next and previous chunks of c1 */
   /* adjust the units, status, the next pointer of c1 */
   ChunkSetUnits(c1, ChunkGetUnits(c1) + ChunkGetUnits(c2) + 2);
   if (prev != NULL){
      ChunkSetNextFreeChunk(prev, ChunkGetNextFreeChunk(c1));
   }
   else {
      gFreeHead[index] = ChunkGetNextFreeChunk(c1);
   }
   if (ChunkGetNextFreeChunk(c1) != NULL){
       ChunkSetPrevFreeChunk(ChunkGetNextFreeChunk(c1), prev);
   }
   ChunkSetNextFreeChunk(c1, NULL);
   ChunkSetStatus(c1, CHUNK_IN_USE);
   return c1;
}

/*--------------------------------------------------------------------*/
/* RightMergeChunk:
 * Merge two adjacent chunks and return the merged chunk.
 * Right chunk is freed and left chunk is allocated.
 * Returns NULL on error. 
 */
/*--------------------------------------------------------------------*/
static Chunk_T
RightMergeChunk(Chunk_T c1, Chunk_T c2)
{
   /* c1 and c2 must be be adjacent */
   assert (c1 < c2 && ChunkGetNextAdjacent(c1, gHeapStart, gHeapEnd) == c2);
   assert (ChunkGetStatus(c1) == CHUNK_IN_USE);
   assert (ChunkGetStatus(c2) == CHUNK_FREE);

   Chunk_T prev = ChunkGetPrevFreeChunk(c2);
   int index = UnitsToIndex(ChunkGetUnits(c2));

   /* adjust the next and previous chunks of c2 */
   /* adjust the units, status, the previous pointer of c2 */
   ChunkSetUnits(c1, ChunkGetUnits(c1) + ChunkGetUnits(c2) + 2);
   if (prev != NULL){
      ChunkSetNextFreeChunk(prev, ChunkGetNextFreeChunk(c2));
   }
   else {
      gFreeHead[index] = ChunkGetNextFreeChunk(c2);
   }
   if (ChunkGetNextFreeChunk(c2) != NULL){
      ChunkSetPrevFreeChunk(ChunkGetNextFreeChunk(c2), prev);
   }
   ChunkSetPrevFreeChunk(c2, NULL);
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
   assert (c >= (Chunk_T)gHeapStart && c <= (Chunk_T)gHeapEnd);
   assert (ChunkGetStatus(c) == CHUNK_FREE);
   assert (ChunkGetUnits(c) > units + 2);

   Chunk_T c2;
   int all_units = ChunkGetUnits(c);
   Chunk_T prev = ChunkGetPrevFreeChunk(c);
   int index = UnitsToIndex(all_units);
   Chunk_T prevgFreeHead;

   if (prev != NULL){
      ChunkSetNextFreeChunk(prev, ChunkGetNextFreeChunk(c));
   }
   else {
      gFreeHead[index] = ChunkGetNextFreeChunk(c);
   }   
   if (ChunkGetNextFreeChunk(c) != NULL){
      ChunkSetPrevFreeChunk(ChunkGetNextFreeChunk(c), prev);
   }

   /* adjust the size of the first chunk */
   ChunkSetUnits(c, all_units - units - 2);
   index = UnitsToIndex(ChunkGetUnits(c));
   prevgFreeHead = gFreeHead[index];
   gFreeHead[index] = c;
   ChunkSetNextFreeChunk(c, prevgFreeHead);
   if (prevgFreeHead != NULL){
       ChunkSetPrevFreeChunk(prevgFreeHead, c);
   }
   ChunkSetPrevFreeChunk(c, NULL);

   /* prepare for the second chunk */
   c2 = ChunkGetNextAdjacent(c, gHeapStart, gHeapEnd);
   ChunkSetUnits(c2, units);
   ChunkSetStatus(c2, CHUNK_IN_USE);
   ChunkSetNextFreeChunk(c2, NULL);
   ChunkSetPrevFreeChunk(c2, NULL);

   return c2;
}

/*--------------------------------------------------------------------*/
/* InsertChunk:
 * Insert a chunk, 'c', into the head of the free chunk list. 
 * 'c' will be merged with the adjacent chunk if possible.
 * The status of 'c' is set to CHUNK_FREE
 */
/*--------------------------------------------------------------------*/
static Chunk_T
InsertChunk(Chunk_T c)
{
   assert (ChunkGetStatus(c) != CHUNK_FREE);
   assert (ChunkGetUnits(c) >= 1);

   Chunk_T prev = ChunkGetPrevAdjacent(c, gHeapStart, gHeapEnd);
   Chunk_T next = ChunkGetNextAdjacent(c, gHeapStart, gHeapEnd); 
   Chunk_T prevgFreeHead;
   int index;   

   if (prev != NULL){
      if (ChunkGetStatus(prev) == CHUNK_FREE) {
         c = LeftMergeChunk(prev, c);
      }
   }
   if (next != NULL){
      if (ChunkGetStatus(next) == CHUNK_FREE) {
         c = RightMergeChunk(c, next);
      }
   }
   
   ChunkSetStatus(c, CHUNK_FREE);
   index = UnitsToIndex(ChunkGetUnits(c));
   prevgFreeHead = gFreeHead[index];
   gFreeHead[index] = c;
   ChunkSetNextFreeChunk(c, prevgFreeHead);
   if (prevgFreeHead != NULL){
       ChunkSetPrevFreeChunk(prevgFreeHead, c);
   }
   
   return c;
}

/*--------------------------------------------------------------------*/
/* RemoveChunkFromList:
 * Removes 'c' from the free chunk list. 'prev' should be the previous
 * free chunk of 'c' or NULL if 'c' is the first chunk.
 */
/*--------------------------------------------------------------------*/
static void
RemoveChunkFromList(Chunk_T c)
{
   assert (ChunkGetStatus(c) == CHUNK_FREE);

   Chunk_T prev = ChunkGetPrevFreeChunk(c);
   int index;
   
   index = UnitsToIndex(ChunkGetUnits(c));
   if (prev != NULL){
      ChunkSetNextFreeChunk(prev, ChunkGetNextFreeChunk(c));
   }
   else {
      gFreeHead[index] = ChunkGetNextFreeChunk(c);
   }   
   if (ChunkGetNextFreeChunk(c) != NULL){
      ChunkSetPrevFreeChunk(ChunkGetNextFreeChunk(c), prev);
   }

   ChunkSetNextFreeChunk(c, NULL);
   ChunkSetStatus(c, CHUNK_IN_USE);
   ChunkSetPrevFreeChunk(c, NULL);

}

/*--------------------------------------------------------------------*/
/* AllocateMoreMemory: 
 * Allocate a new chunk and return it.
 * This function also performs chunk merging with previous free chunk 
 * if possible after allocating a new chunk. 
*/
/*--------------------------------------------------------------------*/
static Chunk_T
AllocateMoreMemory(int units)
{
   Chunk_T c;

   if (units < MEMALLOC_MIN)
      units = MEMALLOC_MIN;
   
   /* Note that we need to allocate two units for header and footer. */
   c = (Chunk_T)sbrk((units + 2) * CHUNK_UNIT);
   if (c == (Chunk_T)-1)
      return NULL;
   
   gHeapEnd = sbrk(0);
   ChunkSetUnits(c, units);
   ChunkSetNextFreeChunk(c, NULL);
   ChunkSetPrevFreeChunk(c, NULL);
   ChunkSetStatus(c, CHUNK_IN_USE);
   
   /* Insert the newly allocated chunk 'c' to the free list. */
   c = InsertChunk(c);
 
   assert(CheckHeapValidity());
   return c;
}

/*--------------------------------------------------------------*/
/* HeapMgr_malloc:
 * Dynamically allocate a memory capable of holding size bytes. 
 * Substitute for GNU malloc().                                 
 */
/*--------------------------------------------------------------*/
void *
HeapMgr_malloc(size_t size)
{
   static int isInit = FALSE;
   Chunk_T c;
   int units;
   int index;
   
   if (size <= 0)
      return NULL;
   
   if (isInit == FALSE) {
      InitMyHeap();
      isInit = TRUE;
   }

   /* see if everything is OK before doing any operations */
   assert(CheckHeapValidity());

   units = SizeToUnits(size);
   index = UnitsToIndex(units);

   for (c = gFreeHead[index]; 
      index< 1023; 
      c = gFreeHead[index]) {
   
      if (c == NULL){
         index++;
         continue;
      }      
      if (ChunkGetUnits(c) > units + 2) 
         c = SplitChunk(c, units);
      else
         RemoveChunkFromList(c);
         
      assert(CheckHeapValidity());
      return (void *)((char *)c + CHUNK_UNIT);
   }

   for (c = gFreeHead[index]; 
        c != NULL; 
        c = ChunkGetNextFreeChunk(c)) {

      if (ChunkGetUnits(c) >= units) {
         if (ChunkGetUnits(c) > units + 2) 
            c = SplitChunk(c, units);
         else
            RemoveChunkFromList(c);
      
         assert(CheckHeapValidity());
         return (void *)((char *)c + CHUNK_UNIT);
      }
   }

   /* allocate new memory */
   c = AllocateMoreMemory(units);
   if (c == NULL) {
     assert(CheckHeapValidity());
     return NULL;
   }

   assert(ChunkGetUnits(c) >= units);

   if (ChunkGetUnits(c) > units + 2) 
      c = SplitChunk(c, units);
   else 
      RemoveChunkFromList(c);
      
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
   Chunk_T c;
   
   if (m == NULL)
      return;

   /* check everything is OK before freeing 'm' */
   assert(CheckHeapValidity());

   /* get the chunk header pointer from m */
   c = GetChunkFromDataPtr(m);
   assert (ChunkGetStatus(c) != CHUNK_FREE);
   
   c = InsertChunk(c);

   /* double check if everything is OK */
   assert(CheckHeapValidity());
}
