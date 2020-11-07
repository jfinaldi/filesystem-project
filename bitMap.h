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

int memory_map_init(int starting_block);
int find_free_index(int blocks_needed);
int free_mem(int index, int count);
