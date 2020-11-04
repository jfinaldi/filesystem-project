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
int volume_size = 0;
int block_size = 0; 
int starting_block = 0; 
int memory_map_init(start, volumeSize, blockSize)
{
    volume_size = volumeSize; 
    starting_block = start; 
    block_size = blockSize; 
    int block_count = volume_size / block_size;
    int bitmap_size_in_bytes =  block_count * sizeof(_Bool);
    
    
    int bitmap_size_in_blocks = (bitmap_size_in_bytes/block_size) + 1; 

    _Bool * bitmap = (_Bool*)calloc(bitmap_size_in_blocks * block_size, sizeof(_Bool)); 
    if (bitmap == NULL) {
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

int find_free_index(blocks_needed) {
    int block_count = volume_size / block_size;
    int bitmap_size_in_bytes =  block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = (bitmap_size_in_bytes/block_size) + 1;

    _Bool * bitmap = (_Bool*)calloc(bitmap_size_in_blocks * block_size, sizeof(_Bool)); 
    if (bitmap == NULL){
        return -1; 
    }
    printf("hello?"); 
    LBAread(bitmap, bitmap_size_in_blocks, starting_block); 

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
                LBAwrite(bitmap, bitmap_size_in_blocks, starting_block); 
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
