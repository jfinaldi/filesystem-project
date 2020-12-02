/**************************************************************
* Class:  CSC-415.02
* Name: Jennifer Finaldi (Dreamteam)
* Student ID: N/A
* Project: Basic File System
*
* File: globals.c
*
* Description: 
*	This file stores all global constant variables needed
*	throughout the filesystem.
*
**************************************************************/
#include "mfs.h"

int BLOCK_SIZE;
int B_CHUNK_SIZE;
int BUFSIZE;
int MAX_PATH_LENGTH;
int TRUE;
int FALSE;
int MAX_OPENFILE;
int MAXFCBS;
int MAX_DEEP;
int EXTENT_MULTIPLIER;
int EXTENT_START_BLOCKS;
int EXTENT_MAX_ELEMENTS;
int PRE_ALLOCATE_BLOCK;
int DEFAULT_INDEX;
int DEFAULT_SIZE;
int DEFAULT_LBA;
char DEFAULT_FILETYPE;
char* DEFAULT_USERNAME;

void initGlobals()
{
    BLOCK_SIZE = 512;
    B_CHUNK_SIZE = 512;
    BUFSIZE = 512;
    MAX_PATH_LENGTH = 512;
    TRUE = 1;
    FALSE = 0;
    MAX_OPENFILE = 20;
    MAXFCBS = 20;
    MAX_DEEP = 50;
    EXTENT_MULTIPLIER = 2;
    EXTENT_START_BLOCKS = 20;
    EXTENT_MAX_ELEMENTS = 64;
    PRE_ALLOCATE_BLOCK = 4;
    DEFAULT_INDEX = 0;
    DEFAULT_SIZE = 0;
    DEFAULT_LBA = 20000;
    DEFAULT_FILETYPE = 'z';
    DEFAULT_USERNAME = "BobsAirPods@sfsu-DreamTeamFS:";
}