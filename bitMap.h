



void memory_map_init(int volume_size, int block_size, int starting_block)
bool[] get_bitmap(int bitmap_size_in_blocks);
//bool bit_is_free(int index);
// void set_bit_to_filled(int index);
// void set_bit_to_free(int index);
int find_free_index(int blocks_needed, int volume_size, int block_size)