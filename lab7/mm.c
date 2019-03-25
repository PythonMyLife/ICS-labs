/*
 * This allocator uses implicit free linked list
 * and next-fit placement,which immediately free
 * idle block and has header and footer.
 * The minimum bolck is 16 bytes and blocks are 
 * aligned to 8 bytes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x) > (y)? (x) : (y))
#define MIN(x, y) ((x) > (y)? (y) : (x))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p,val) (*(unsigned int *)(p) = val)

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp))-DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

static char * heap_listp = NULL;
static char * next_p = NULL;

/* coalesce blocks and return pointer to a coalesced block */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc){
        return bp;
    }
    else if(prev_alloc && !next_alloc){
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0)); 
    }
    else if(!prev_alloc && next_alloc){
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else{
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(FTRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }

    if((next_p > (char *)bp) && (next_p < NEXT_BLKP(bp))){
        next_p = bp;
    } 

    return bp;
}

/* extend heap with free block and return a pointer to the block */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
    return coalesce(bp); 
}

/* find a suitable block with a size bigger or equal to asize */
static void *find_fit(size_t asize)
{
    /* First fit */
    //void *bp;

    //for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0;bp = NEXT_BLKP(bp)){
    //    if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
    //        return bp;
    //    }
    //}
    //return NULL;

    /* Next fit */
    char *bp = next_p;

    for(;GET_SIZE(HDRP(next_p)) > 0;next_p = NEXT_BLKP(next_p)){
        if(!GET_ALLOC(HDRP(next_p)) && (asize <= GET_SIZE(HDRP(next_p))))
            return next_p;
    }
    for(next_p = heap_listp; next_p < bp;next_p = NEXT_BLKP(next_p)){
        if(!GET_ALLOC(HDRP(next_p)) && (asize <= GET_SIZE(HDRP(next_p))))
            return next_p;
    }
    return NULL;
}

/* place the block of asize to bp(when the size of bp much bigger than asize,split it) */
static void place(void *bp,size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2*DSIZE)){ 
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp),PACK(csize-asize,0));
        PUT(FTRP(bp),PACK(csize-asize,0));
    }
    else{
        PUT(HDRP(bp),PACK(csize,1));
        PUT(FTRP(bp),PACK(csize,1));
    }

}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp,0);
    PUT(heap_listp + (1*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (2*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (3*WSIZE),PACK(0,1));
    heap_listp += (2*WSIZE);
    next_p = heap_listp;
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char *bp;

    if(heap_listp == NULL)
        mm_init();
    if(size == 0) 
        return NULL;
    /* adjust block size to include overhead and alignment reqs */
    if(size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1) ) / DSIZE);

    if((bp = find_fit(asize)) != NULL){
        place(bp,asize);
        return bp;
    }
    /* no fit found, need more memory */
    extendsize = MAX(asize,CHUNKSIZE);
    if((bp = extend_heap(extendsize/WSIZE)) == NULL){
        return NULL;
    }
    place(bp,asize);
    return bp;
    
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    if(bp == 0)
        return;
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t old_size;
    void *bp;

    if(size == 0){
        mm_free(ptr);
        return 0;
    }
    if(ptr == NULL){
        return mm_malloc(size);
    }
    if(!(bp = mm_malloc(size)))
        return 0;
    old_size = GET_SIZE(HDRP(ptr));
    if(size < old_size) old_size = size;
    memcpy(bp,ptr,old_size);

    mm_free(ptr);
    return bp;

}