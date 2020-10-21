#ifndef _DIRENTRY_H
#define _DIRENTRY_H

typedef struct DirectoryEntry 
{
	unsigned long rootDir; // location of first root directory  
	unsigned long id; // id of the file
	unsigned long dataLocation; // location where file data starts 
	char name[256]; // the name of the file
	short permission[3]; // three-digit code for rwx, owner/group/all, 0-7
	unsigned int owner; // the user ID of the owner of the file
	uint64_t size;   // the number of bytes of the file data
	unsigned long numBlocks; // the number of blocks occupied by the file
	unsigned long parentID;   // # of block where parent directory is located
	short dateModifiedDirectory;  // date the file was last modified
	short dateAccessedDirectory;  // date the file was last accessed
	unsigned long locationMetadata; //512 file per directory
} dirEntry;

#endif