/* chunk.h
 * Authors: Donghwi Kim, KyoungSoo Park
 */

#ifndef _CHUNK_H_
#define _CHUNK_H_

/* 
   each chunk has a header, data units, and a footer.  each header (and
   a footer) in a chunk has a pointer to the next/previous chunk in the
   free list. the size of a chunk is in the number of units, but it
   does not include the unit for a header and a footer. For example,
   if a chunk has 5 units, its actual size is 7 units since it has
   one header and one footer. 
*/

typedef struct Chunk *Chunk_T;

enum {
   CHUNK_FREE,
   CHUNK_IN_USE,
};

enum {
   CHUNK_UNIT = 16, /* 16 = sizeof(struct Chunk) */
};

/* ChunkGetUnits:
 * Returns the size of a chunk, 'c', in terms of the number of chunk units. */
int
ChunkGetUnits(Chunk_T c);

/* ChunkSetUnits:
 * Sets the current size to units */
void
ChunkSetUnits(Chunk_T c, int units);

/* ChunkGetStatus:
 * Returns a chunk's status which shows whether the chunk is in use or free.
 * Return value is either CHUNK_IN_USE or CHUNK_FREE. */
int
ChunkGetStatus(Chunk_T c);

/* ChunkSetStatus:
 * Set the status of the chunk, 'c'. 
 * status can be either CHUNK_FREE or CHUNK_IN_USE */
void
ChunkSetStatus(Chunk_T c, int status);

/* ChunkGetNextFreeChunk:
 * Returns the next free chunk of 'c' in the free chunk list.
 * Returns NULL if 'c' is the last free chunk in the list. */
Chunk_T
ChunkGetNextFreeChunk(Chunk_T c);

/* ChunkSetNextFreeChunk:
 * Sets the next chunk of 'c' to 'next' */
void
ChunkSetNextFreeChunk(Chunk_T c, Chunk_T next);

/* ChunkGetPrevFreeChunk:
 * Returns the previous free chunk of 'c' in the free chunk list.
 * Returns NULL if 'c' is the first free chunk in the list. */
Chunk_T
ChunkGetPrevFreeChunk(Chunk_T c);

/* ChunkSetPrevreeChunk:
 * Sets the previous chunk of 'c' to 'prev' */
void
ChunkSetPrevFreeChunk(Chunk_T c, Chunk_T prev);

/* ChunkGetPrevAdjacent:
 * Returns the previous adjacent chunk to 'c' in memory space.
 * start is the pointer to the start of the Heap
 * end is the the pointer to the end of the Heap
 * Returns NULL if 'c' is the first chunk in memory space. */
Chunk_T
ChunkGetPrevAdjacent(Chunk_T c, void* start, void *end);

/* ChunkGetNextAdjacent:
 * Returns the next adjacent chunk to 'c' in memory space.
 * start is the pointer to the start of the Heap
 * end is the the pointer to the end of the Heap
 * Returns NULL if 'c' is the last chunk in memory space. */
Chunk_T
ChunkGetNextAdjacent(Chunk_T c, void *start, void *end);


#ifndef NDEBUG
/* ChunkIsValid:
 * Checks the validity of a chunk, 'c'. Returns 1 if OK otherwise 0.
 * start is the pointer to the start of the Heap
 * end is the the pointer to the end of the Heap
 */
int 
ChunkIsValid(Chunk_T c, void *start, void *end);

/* ChunkPrint:
 * Print out entire data sturucture for chunks. This function will print
 * all chunks in free chunks list and in memory. 
 * start is the pointer to the start of the Heap
 * end is the the pointer to the end of the Heap
 * freeHead is the head pointer of the free chunk list
 */
void
ChunkPrint(void *start, void *end, Chunk_T freeHead);

#endif /* NDEBUG */

#endif /* _CHUNK_H_ */


