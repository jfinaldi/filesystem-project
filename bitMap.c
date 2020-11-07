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
    
    int block_count = MBR_st -> totalBlockLBA;
    int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st ->blockSize) + 1;

    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st ->blockSize, sizeof(_Bool));
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
    int block_count = MBR_st -> totalBlockLBA;
    int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st -> blockSize) + 1;
    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st -> blockSize, sizeof(_Bool));
    if (bitmap == NULL)
    {
        return -1;
    }
    LBAread(bitmap, bitmap_size_in_blocks, MBR_st ->freeSpacePos);
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
                result = index - blocks_needed;
                for (int i = result; i < index; i++)
                {
                    bitmap[i] = 1;
                }
                LBAwrite(bitmap, bitmap_size_in_blocks, MBR_st ->freeSpacePos);
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
    return (50);
}
int free_mem(int index, int count) {
    int block_count = MBR_st -> totalBlockLBA;
    int bitmap_size_in_bytes = block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes / MBR_st -> blockSize) + 1;
    _Bool *bitmap = (_Bool *)calloc(bitmap_size_in_blocks * MBR_st -> blockSize, sizeof(_Bool));
    if (bitmap == NULL)
    {
        return -1;
    }
    LBAread(bitmap, bitmap_size_in_blocks, MBR_st ->freeSpacePos);
    for (int i = index; i < index + count; index++) {
        bitmap[i] = 0; 
    }
    LBAwrite(bitmap, bitmap_size_in_blocks, MBR_st ->freeSpacePos);
}