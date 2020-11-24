/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: bitMap.c
*
* Description: 
*	This is the header file for initDirectory
*
**************************************************************/

#include "mfs.h"

int memory_map_init(int starting_block)
{

    int block_count = MBR_st->totalBlockLBA;
    int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st->blockSize) + 1;

    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st->blockSize, sizeof(_Bool));
    if (bitmap == NULL)
    {
        return -1;
    }
    for (int i = 0; i < bitmap_size_in_blocks; i++)
    {
        bitmap[i] = 1;
    }
    LBAwrite(bitmap, bitmap_size_in_blocks, starting_block);
    free(bitmap);
    int result = bitmap_size_in_blocks + starting_block;
    return result;
}

int find_free_index(int blocks_needed)
{
    int block_count = MBR_st->totalBlockLBA;
    int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st->blockSize) + 1;
    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st->blockSize, sizeof(_Bool));
    if (bitmap == NULL)
    {
        return -1;
    }
    printf("FREESPACE POS %ld", MBR_st->blockSize);
    LBAread(bitmap, bitmap_size_in_blocks, MBR_st->freeSpacePos);
    int free_blocks = 0;
    int index = 0;
    int result = -1;

    while (index < bitmap_size_in_bytes)
    {
        if (bitmap[index] == 0)
        {
            free_blocks++;
            if (free_blocks == blocks_needed)
            {
                result = index - blocks_needed + 1;
                for (int i = result; i <= index; i++)
                {
                    bitmap[i] = 1;
                }
                LBAwrite(bitmap, bitmap_size_in_blocks, MBR_st->freeSpacePos);
                return result;
            }
        }
        else
        {
            free_blocks = 0;
        }
        index++;
    }

    printf("Out of space");
    return (-1);
}
int free_mem(int index, int count)
{

    int block_count = MBR_st->totalBlockLBA;
    int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st->blockSize) + 1;
    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st->blockSize, sizeof(_Bool));
    if (bitmap == NULL)
    {
        return -1;
    }
    LBAread(bitmap, bitmap_size_in_blocks, MBR_st->freeSpacePos);
    for (int i = index; i <= index + count; i++)
    {
        //printf("HLEELEO?");
        bitmap[i] = 0;
    }
    LBAwrite(bitmap, bitmap_size_in_blocks, MBR_st->freeSpacePos);
}
int defrag(int start)
{

    
    int block_count = MBR_st->totalBlockLBA;
    // int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_bytes = 19000;
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st->blockSize) + 1;
    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st->blockSize, sizeof(_Bool));
    printf("START DEGRAG %d", bitmap_size_in_bytes); 
    if (bitmap == NULL)
    {
        return -1;
    }
    LBAread(bitmap, bitmap_size_in_blocks, MBR_st->freeSpacePos);
    int index = start; 
    int fragStart = -1; 
    int fragEnd = -1;
    int fragSize = 0;  
    while (index < bitmap_size_in_bytes - fragSize)
    {
        
        if (bitmap[index] == 0)
        {
            if (fragStart < 0) {
                fragStart = index; 
            }
        }
        else if (bitmap[index] == 1)
        {
            if (fragStart > 0 && fragEnd < 0) {
                printf("WHAT?? %d", bitmap[index]); 
                fragEnd = index - 1; 
                fragSize = fragEnd - fragStart + 1 ;
                int index2 = fragStart; 
                while (index2 <= fragEnd)
                {
                    bitmap[index2] = bitmap[index2 + fragSize - 1];
                    index2++; 
                }
                
                
            }
        }
        if (fragStart > 0) {
            bitmap[index] = bitmap[index + fragSize - 1]; 
        }
        index++;
    }
    // while (index < bitmap_size_in_bytes)
    // {
    //     bitmap[index] = 0; 
    //     index++;
    // }
    if (fragEnd < 0) {
        printf("DONE DEFRAGGING"); 
        return 0; 
    }
    printf("START END, %d, %d", fragStart, fragEnd); 
    defrag_helper(NULL, fragSize - 1, fragStart);
    int toMove = bitmap_size_in_bytes - fragEnd;
    printf("TO MOVE %d", toMove);  
    void *moveBuffer = malloc(toMove * MBR_st -> blockSize);
    if (moveBuffer == NULL) {
        printf("PROBLEM WITH MALLOC"); 
    }
    LBAread(moveBuffer, toMove, fragEnd); 
    LBAwrite(moveBuffer, toMove, fragStart); 
    LBAwrite(bitmap, bitmap_size_in_blocks, MBR_st->freeSpacePos);
    //defrag(fragStart); 
}

int defrag_helper(dirEntry *startdE, int offset, int fragStart)
{
    //set toRemove to be empty 
    dirEntry *entryBuff = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
    int blocks = MBR_st->dirNumBlocks;
    int originalChild = 0;
    if (startdE == NULL) {
        LBAread(entryBuff, blocks, MBR_st -> rootDirectoryPos);
    } else {
        originalChild = startdE->childLBA;
        LBAread(entryBuff, blocks, originalChild);
    }
    for (int i = 0; i < STARTING_NUM_DIR; i++) {
        if (entryBuff[i].isBeingUsed == 1) {
            if (i > 1) {
                defrag_helper(&entryBuff[i], offset, fragStart);
            } else {
                if (entryBuff[i].locationMetadata > fragStart) {
                    entryBuff[i].locationMetadata = entryBuff[i].locationMetadata - offset; 
                } 
                
            }
            if (entryBuff[i].childLBA > fragStart) {
                entryBuff[i].childLBA = entryBuff[i].childLBA - offset;
            }
            
            if (entryBuff[i].locationLBA > fragStart) {
                    entryBuff[i].locationLBA = entryBuff[i].locationLBA - offset; 
            }
            
            
        }
    }
    if (startdE == NULL) {
        LBAwrite(entryBuff, blocks, MBR_st -> rootDirectoryPos);
    } else {
        LBAwrite(entryBuff, blocks, originalChild);
    }
}