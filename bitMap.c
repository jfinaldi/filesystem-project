
int *bitmap;
char *buffer;
int volume_size; 
int bits = 8; 

void memory_map_init(int block_size, int block_count)
{
    bitmap = (int*)calloc((block_count) / sizeof(int) * bits + (block_count % sizeof(int) * bits), sizeof(int) * bits);
}

bool is_free(int index)
{
    int map_index = index / sizeof(int) * bits;
    int bit_position = index % sizeof(int) * bits;
    int flag = 1 << bit_position; 
    return !(flag & bitmap[map_index]);
}

void set_bit_to_filled(int index)
{
    int map_index = index / sizeof(int) * bits;
    int bit_position = index % sizeof(int) * bits;
    int flag = 1 << bit_position; 
    bitmap[map_index] = bitmap[map_index] | flag; 
}

void set_bit_to_free(int index)
{
    int map_index = index / sizeof(int) * bits;
    int bit_position = index % sizeof(int) * bits;
    int flag = 1 << bit_position; 
    flag = ~flag; 
    bitmap[map_index] = bitmap[map_index] | flag; 
}
int find_free(int blocks_needed) {
    int curr_free_contiguous = 0; 
    for (int index = 0; index < volume_size; index++) {
        if (is_free) {
            curr_free_contiguous++; 
            if (curr_free_contiguous == blocks_needed) {
                return index - blocks_needed;
            }
        } else {
            curr_free_contiguous = 0; 
        }
    }
    return -1; 
}

