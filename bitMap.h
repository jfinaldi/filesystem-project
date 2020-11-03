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

#ifndef _BITMAP_H
#define _BITMAP_H

#include "mfs.h"

int memory_map_init(int volume_size, int block_size, int starting_block);
//returns starting index of contiguous blocks needed. Sets those blocks to filled
int find_free_index(int blocks_needed, int volume_size, int block_size);

#endif