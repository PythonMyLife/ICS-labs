/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
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
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp))-WSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

static char * heap_listp = NULL;
static char * head_block = NULL;


#define PRED_BLOCK(bp) ((char *)(bp))
#define SUCC_BLOCK(bp) ((char *)(bp) + WSIZE)
#define PRED(bp) (*(char **)(bp))
#define SUCC(bp) (*(char **)(SUCC_BLOCK(bp)))

static void pre_and_suc(void *bp)
{
    char *pred_block = PRED(bp);
    char *succ_block = SUCC(bp);

    if(pred_block == NULL){
        if(succ_block != NULL)
            PUT(PRED_BLOCK(succ_block),0);
        PUT(head_block,succ_block);
    }
    else {
        if(succ_block != NULL)
            PUT(PRED_BLOCK(succ_block),pred_block);
        PUT(SUCC_BLOCK(pred_block),succ_block);
    }
    PUT(PRED_BLOCK(bp),0);
    PUT(SUCC_BLOCK(bp),0);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc){
    }
    else if(prev_alloc && !next_alloc){
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        pre_and_suc(NEXT_BLKP(bp));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0)); 
    }
    else if(!prev_alloc && next_alloc){
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        pre_and_suc(PREV_BLKP(bp));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else{
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
        pre_and_suc(PREV_BLKP(bp));
        pre_and_suc(NEXT_BLKP(bp));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    char *next_block = GET(head_block);
    if(next_block != NULL)
        PUT(PRED_BLOCK(next_block),bp);
    PUT(SUCC_BLOCK(bp),next_block);
    PUT(head_block,bp);
    return bp;
}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(PRED_BLOCK(bp),0);
    PUT(SUCC_BLOCK(bp),0);
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
    return coalesce(bp); 
}

static void *find_fit(size_t asize)
{
    /* First fit */
    char *bp = GET(head_block);

    while(bp != NULL){
        if(GET_SIZE((HDRP(bp))) >= asize) return bp;
        bp = GET(SUCC_BLOCK(bp));
    }
    return NULL;
}

static void place(void *bp,size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    pre_and_suc(bp);

    if ((csize - asize) >= (2*DSIZE)){ /* bigger or equal in the textbook */
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp),PACK(csize-asize,0));
        PUT(FTRP(bp),PACK(csize-asize,0));
        PUT(PRED_BLOCK(bp),0);
        PUT(SUCC_BLOCK(bp),0);
        coalesce(bp);
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
    if ((heap_listp = mem_sbrk(6 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp,0);
    PUT(heap_listp + (1*WSIZE),0);
    PUT(heap_listp + (2*WSIZE),0);
    PUT(heap_listp + (3*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (4*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (5*WSIZE),PACK(0,1));

    head_block = heap_listp + (1*WSIZE);
    heap_listp += (4*WSIZE);

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

    if(size == 0) return NULL;
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
        return NULL;
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(PRED_BLOCK(bp),0);
    PUT(SUCC_BLOCK(bp),0);
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














