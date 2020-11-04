/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: bitMap.h
*
* Description: 
*	This is the header file for initDirectory
*
**************************************************************/

int memory_map_init(int starting_block, int volume_size, int block_size);
int find_free_index(int blocks_needed);
