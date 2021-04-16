/*************************************
* Lab 4 Exercise 3
* Name:
* Student Id: A????????
* Lab Group: B??
*************************************
Note: Duplicate the above and fill in
for the 2nd member if  you are on a team
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmalloc.h"

/**********************************************************
 * This is a "global" structure storing heap information
 * The "static" modifier restrict the structure to be
 *  visible only to functions in this file
 *********************************************************/
static heapMetaInfo hmi;


/**********************************************************
 * Quality of life helper functions / macros
 *********************************************************/
#define powOf2(E) (1 << E)

unsigned int log2Ceiling( unsigned int N )
/**********************************************************
 * Find the smallest S, such that 2^S >= N
 * S is returned
 *********************************************************/
{
    unsigned int s = 0, pOf2 = 1;

    while( pOf2 < N){
        pOf2 <<= 1;
        s++;
    }

    return s;
}


unsigned int log2Floor( unsigned int N )
/**********************************************************
 * Find the largest S, such that 2^S <= N
 * S is returned
 *********************************************************/
{
    unsigned int s = 0, pOf2 = 1;

    while( pOf2 <= N){
        pOf2 <<= 1;
        s++;
    }

    return s-1;
}

unsigned int buddyOf( unsigned int addr, unsigned int lvl )
/**********************************************************
 * Return the buddy address of address (addr) at level (lvl)
 *********************************************************/
{
    unsigned int mask = 0xFFFFFFFF << lvl;
    unsigned int buddyBit = 0x0001 << lvl;

    return (addr & mask) ^ buddyBit;
}

partInfo* buildPartitionInfo(unsigned int offset)
/**********************************************************
 * Allocate a new partInfo structure and initialize the fields
 *********************************************************/
{
    partInfo *piPtr;

    piPtr = (partInfo*) malloc(sizeof(partInfo));

    piPtr->offset = offset;
	piPtr->nextPart = NULL;

    //Buddy system's partition size is implicit
	//piPtr->size = size;

    //All available partition in buddy system is implicitly free
	//piPtr->status = FREE;

    return piPtr;
}

void printPartitionList(partInfo* piPtr)
/**********************************************************
 * Print a partition linked list
 *********************************************************/
{
	partInfo* current;
    int count = 1;

	for ( current = piPtr; current != NULL;
		current = current->nextPart){
        if (count % 8 == 0){
            printf("\t");
        }
		printf("[+%5d] ", current->offset);
        count++;
        if (count % 8 == 0){
            printf("\n");
        }
	}
    printf("\n");
}

void printHeapMetaInfo()
/**********************************************************
 * Print Heap Internal Bookkeeping Information
 *********************************************************/
{
    int i;

	printf("\nHeap Meta Info:\n");
	printf("===============\n");
	printf("Total Size = %d bytes\n", hmi.totalSize);
	printf("Start Address = %p\n", hmi.base);

    for (i = hmi.maxIdx; i >=0; i--){
        printf("A[%d]: ", i);
        printPartitionList(hmi.A[i] );
    }

}

void printHeap()
/**********************************************************
 * Print the content of the entire Heap
 *********************************************************/
{
    //Included as last debugging mechanism.
    //Print the entire heap regions as integer values.

    int* array;
    int size, i;

    size = hmi.totalSize / sizeof(int);
    array = (int*)hmi.base;

    for ( i = 0; i < size; i++){
        if (i % 4 == 0){
            printf("[+%5d] |", i);
        }
        printf("%8d",array[i]);
        if ((i+1) % 4 == 0){
            printf("\n");
        }
    }
}

void printHeapStatistic()
/**********************************************************
 * Print Heap Usage Statistics
 *********************************************************/
{
    //TODO: Task 4. Calculate and report the various statistics

    printf("\nHeap Usage Statistics:\n");
    printf("======================\n");

    printf("Total Space: %d bytes\n", hmi.totalSize);

   //Remember to preserve the message format!

    //printf("Total Space: %d bytes\n", hmi.totalSize);
    int i, freeSize=0, freePart=0;
    for(i=0; i<=hmi.maxIdx; i++){
        partInfo* part = hmi.A[i];
        while(part!=NULL){
            freePart++;
            freeSize += (1<<i);
            part = part->nextPart;
        }
    }
    printf("Total Free Partitions: %d\n", freePart);
    printf("Total Free Size: %d bytes\n", freeSize);

    printf("Total Internal Fragmentation: %d bytes\n", hmi.internalFragTotal);
}

void addPartitionAtLevel( unsigned int lvl, unsigned int offset )
/**********************************************************
 * There is just a suggested approach. You are free NOT to use this.
 *    This function adds a new free partition with "offset" at hmi.A[lvl]
 *    If buddy is found, recursively (or repeatedly) perform merging and insert
 *      at higher level
 *********************************************************/
{
    int i;
    printf("lvl=%d %i offset=%d %i\n", lvl, lvl, offset, offset);
    //for(i=0; i<10; i++)
        partInfo* old = hmi.A[lvl];
        hmi.A[lvl] = (partInfo*) malloc(sizeof(partInfo));
        hmi.A[lvl]->offset = offset;
        hmi.A[lvl]->nextPart = old;
        //printHeapMetaInfo();

        /*while(A[lvl]){
            lvl++;
        }*/
    //hmi.A[lvl] = ;
}

partInfo* removePartitionAtLevel(unsigned int lvl)
/**********************************************************
 * There is just a suggested approach. You are free NOT to use this.
 *    This function remove a free partition at hmi.A[lvl]
 *    Perform the "upstream" search if this lvl is empty AND perform
 *      the repeated split from higher level back to this level.
 *
 * Return NULL if cannot find such partition (i.e. cannot sastify request)
 * Return the Partition Structure if found.
 *********************************************************/
{
    int initLvl = lvl, i;

    while(hmi.A[lvl] == NULL ){ // is empty
        if(lvl==hmi.maxIdx) return NULL;
        lvl++;
    }
    //printf("initLvl: %d     lvl: %d\n", initLvl, lvl);

    int addr, initOffset = hmi.A[lvl]->offset;

    addr = hmi.A[lvl]->offset + (1<<(lvl-1));
      //printf("start: %d     %d\n", hmi.A[lvl]->offset, addr);
      //printHeapMetaInfo();
    hmi.A[lvl] = hmi.A[lvl]->nextPart;

    //puts("\nhi\n");
    //printHeapMetaInfo();

    for(i=lvl-1; i>=initLvl; i--){
        //if(i!=lvl){
            partInfo* old = hmi.A[i];

            hmi.A[i] = malloc(sizeof(partInfo));
            hmi.A[i]->offset = addr;
            hmi.A[i]->nextPart = old;
            addr = (1<<(i-1));

            //printf("i=%d     %d %d\n", i, hmi.A[i]->offset, addr);
            //break;
            //printHeapMetaInfo();
        //}

    }

    /*if(lvl==8){
      puts("special");
      partInfo* old = hmi.A[initLvl];
    partInfo* ans = malloc(sizeof(partInfo));
    ans->offset = initOffset;
    ans->nextPart = old;
    printf("final: %d %p\n", ans->offset, ans->nextPart);
    printHeapMetaInfo();

    return NULL;
    }*/

    partInfo* old = hmi.A[initLvl];
    partInfo* ans = malloc(sizeof(partInfo));
    ans->offset = initOffset;
    ans->nextPart = old;
    printf("final: %d %p\n", ans->offset, ans->nextPart);
    //printHeapMetaInfo();

    return ans;
}

int setupHeap(int initialSize)
/**********************************************************
 * Setup a heap with "initialSize" bytes
 *********************************************************/
{
	void* base;

	base = sbrk(0);
	if(	sbrk(initialSize) == (void*)-1){
		printf("Cannot set break! Behavior undefined!\n");
		return 0;
	}

    hmi.base = base;

	  hmi.totalSize = initialSize;
    hmi.internalFragTotal = 0;

    //TODO: Task 1. Setup the rest of the bookkeeping info:
    //       hmi.A <= an array of partition linked list
    //       hmi.maxIdx <= the largest index for hmi.A[]
    //
    int max = 0, product = 1;
    for(max=0; product!=hmi.totalSize; max++, product*=2);
    //printf("max: %d\ntotal size: %d\n", max, hmi.totalSize);

    hmi.A = (partInfo**) malloc(sizeof (partInfo*) * (max+1));   //change this!
    int i;
    hmi.A[max] = malloc(sizeof(partInfo));
    hmi.A[max]->offset = 0;
    hmi.A[max]->nextPart = NULL;

    for(i=0; i<max; i++){
        /*if(hmi.totalSize%2){
            hmi.A[i] = malloc(sizeof(partInfo));
            hmi.A[i]->offset = 0;
            hmi.A[i]->nextPart = NULL;
        }else */
        hmi.A[i] = NULL;
        /*printf("%d %d\n", hmi.totalSize%2, hmi.totalSize/2);
        hmi.totalSize /= 2;*/
    }
    hmi.maxIdx = max; //change this!
    hmi.internalFragTotal = 0;

    return 1;
}


void* mymalloc(int size)
/**********************************************************
 * Mimic the normal "malloc()":
 *    Attempt to allocate a piece of free heap of (size) bytes
 *    Return the memory addres of this free memory if successful
 *    Return NULL otherwise
 *********************************************************/
{
    //TODO: Task 2. Implement the allocation using buddy allocator
    int max = 0, product = 1;
    for(max=0; product<size; max++, product*=2);
    //printf("max: %d\ntotal size: %d\n", max, size);
    hmi.internalFragTotal += product - size;
    //hmi.A[6];
    //for(int i=0; i<10; i++)hmi.A[i] = 10;
    return removePartitionAtLevel(max);

}

void myfree(void* address, int size)
/**********************************************************
 * Mimic the normal "free()":
 *    Attempt to free a previously allocated memory space
 *********************************************************/
{
    //TODO: Task 3. Implement the de allocation using buddy allocator
    int max = 0, product = 1;
    for(max=0; product<size; max++, product*=2);
    hmi.internalFragTotal -= product - size;

    partInfo* part = (partInfo*)address;

    unsigned int addr = part->offset;
    printf("max: %d;  size: %d;  address: %p %d\n", max, size, address, address-hmi.base);
    printf("%i %d %p\n", (unsigned int)(part->offset), part->offset, part->nextPart);

    return addPartitionAtLevel(max, part->offset);
}
