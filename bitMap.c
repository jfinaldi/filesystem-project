
#include "mfs.h"

void memory_map_init(volume_size, block_size, starting_block)
{
    int block_count = volume_size / block_size;
    int bitmap_size_in_bytes =  block_count * sizeof(_Bool);
    
    _Bool * bitmap = (_Bool*)malloc(bitmap_size_in_bytes); 
    int bitmap_size_in_blocks = bitmap_size_in_bytes/block_size; 
    if (bitmap == NULL) {
        return -1; 
    }
    for (int i = 0; i <  bitmap_size_in_blocks; i++){
        bitmap[i] = 1; 
    }
    return LBAwrite(bitmap, bitmap_size_in_blocks, starting_block); 
    return bitmap_size_in_blocks + starting_block; 
}

int find_free_index(blocks_needed, volume_size, block_size) {
    int block_count = volume_size / block_size;
    int bitmap_size_in_bytes =  block_count * sizeof(_Bool);
    int bitmap_size_in_blocks = bitmap_size_in_bytes/block_size;

    _Bool * bitmap = (_Bool*)malloc(bitmap_size_in_blocks * sizeof(_Bool)); 
    if (bitmap == NULL){
        return -1; 
    }

    LBAread(bitmap, bitmap_size_in_blocks, 0); 

    int free_blocks = 0; 
    int index = 0; 
    int result = -1;
    
    while(index < bitmap_size_in_bytes) {
        if (bitmap[index] == 0) {
            free_blocks++; 
            if (free_blocks == blocks_needed) {
                result = index - blocks_needed; 
                for (int i = result; i < index; i++){
                    bitmap[i] = 1; 
                }
                LBAwrite(bitmap, bitmap_size_in_blocks, 0); 
                return result; 
            }
        } else {
            free_blocks = 0; 
        }
        index++; 
    }
    
    printf("Out of space"); 
    return -1; 
}

