/**************************************************************
* Class:  CSC-415
* Name: Lauren Wong, Jennifer  Finaldi, Lilian Gouzeot
*       (DreamTeam)
* Student ID: N/A
* Project: Basic File System
*
* File: readWrite.c
*
* Description: This file utilizes functions to interface
*	        between the shell commands and the directory
*           model. It deals with file operations and
*           management.
*
**************************************************************/

#include "mfs.h"

int startup = 0; //Indicates that this has not been initialized
fd_struct *fileOpen = NULL;

//Method to initialize our Fd struct
void b_init()
{
    for (int i = 0; i < MAX_OPENFILE; i++)
    {
        fileOpen[i].Fd = -1;
        fileOpen[i].isAllocate = FALSE;
        fileOpen[i].flag = -1;
        fileOpen[i].flaggedForClose = FALSE;

        fileOpen[i].locationLBA = DEFAULT_LBA;
        fileOpen[i].childLBA = DEFAULT_LBA;
        fileOpen[i].entryIndex = DEFAULT_INDEX;
        fileOpen[i].dataLocation = DEFAULT_LBA;
        fileOpen[i].name[0] = '\0';
        fileOpen[i].sizeOfFile = DEFAULT_SIZE;
        fileOpen[i].numBlocks = DEFAULT_SIZE;
        fileOpen[i].locationMetadata = DEFAULT_LBA;
        fileOpen[i].extents = DEFAULT_LBA;
        fileOpen[i].numExtents = DEFAULT_SIZE;
        fileOpen[i].numExtentBlocks = DEFAULT_SIZE;
        fileOpen[i].type = DEFAULT_FILETYPE;

        fileOpen[i].filePointer = DEFAULT_INDEX;
        fileOpen[i].blockPointer = DEFAULT_INDEX;
        fileOpen[i].buffer = '\0';
        fileOpen[i].writeBuffer = '\0';
        fileOpen[i].bufIndex = DEFAULT_INDEX;
        fileOpen[i].buflen = DEFAULT_SIZE;
        fileOpen[i].lenBuffer = DEFAULT_SIZE;
        fileOpen[i].LBAInDataLocation = DEFAULT_LBA;
        fileOpen[i].offsetInDataLocation = DEFAULT_LBA;
        fileOpen[i].extentArrayPtrWrite = DEFAULT_INDEX;
        fileOpen[i].extentArrayPtrRead = DEFAULT_INDEX;
    }
}

//helper function to copy data over to fd_struct from dirEntry
int entryToFd(dirEntry* dE, int fd)
{
    if((!dE) || (fd == -1))
    {
        //printf("Error entryToFd, invalid input\n");
        return 1;
    }
    fileOpen[fd].locationLBA = getLocationLBA(dE);
    fileOpen[fd].childLBA = getChildLBA(dE);
    fileOpen[fd].entryIndex = getEntryIndex(dE);
    fileOpen[fd].dataLocation = getDataLocation(dE);
    fileOpen[fd].LBAInDataLocation = getDataLocation(dE);
    strcpy(fileOpen[fd].name, getName(dE));
    fileOpen[fd].sizeOfFile = getSizeOfFile(dE);
    fileOpen[fd].numBlocks = getNumBlocks(dE);
    fileOpen[fd].dateCreated = getDateCreated(dE);
    fileOpen[fd].dateModified = getDateModified(dE);
    time(&(fileOpen[fd].dateAccessed));
    fileOpen[fd].extents = getExtents(dE);
    fileOpen[fd].numExtents = getNumExtents(dE);
    fileOpen[fd].numExtentBlocks = getNumExtentBlocks(dE);
    fileOpen[fd].type = getType(dE);

    return 0;
}

int b_open(char *filename, int flags)
{
    int fd = -1;
    int returnFd;
    char *path_token[MAX_DEEP];
    char *token;
    int nbArgument = 0;
    int length = 0;
    int curr = 0;
    int found = FALSE;

    //if we didn't get a valid filename, return
    if (filename == NULL)
    {
        return -1;
    }

    if (startup == 0) // init the struct for FileDescriptor
    {
        startup = 1;
        fileOpen = malloc(sizeof(fd_struct) * MAX_OPENFILE);
        if (fileOpen == NULL)
        {
            write(2, "b_open: malloc failed\n", 23);
            return (-1);
        }
        b_init();
    }

    // -------------- Find a FD not allocated ----
    for (int i = 0; i < MAX_OPENFILE; i++)
    {
        if (fileOpen[i].Fd == -1)
        {
            fd = i;
        }
    }
    if (fd == -1)
    {
        return (1);
    }

    // -------------- Find a FD not allocated ----
    // -------------- SPLIT PATH ----------------
    length = strlen(filename);
    if (length > MAX_PATH_LENGTH)
    {
        return (1);
    }
    char pathRaw[MAX_PATH_LENGTH];
    memcpy(pathRaw, filename, length);
    token = strtok(pathRaw, "/");
    path_token[nbArgument] = token;
    while (token != NULL)
    { // Split the path into token
        path_token[nbArgument] = token;
        token = strtok(NULL, "/");
        nbArgument++;
    }
    path_token[nbArgument] = NULL;
    nbArgument = nbArgument - 1;

    if (1) 
    { //Absolute path
        char *newName = malloc(256);
        int slash = '/';
        newName = strrchr(filename, slash);
        if (newName == NULL)
        {
            newName = malloc(256);
            strcpy(newName, filename);
        }
        else
        {
            newName++;
        }

        //create a new temp directory 
        fdDir *temp = tempDirectory(filename, 0, NULL);
        if (temp->directoryStartLocation == DEFAULT_LBA)
        {
            printf("not a valid path or name. Returning -1.\n");
            return -1;
        }
        
        //create a buffer and read in entire directory start loc
        dirEntry *ptrOpen = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        int blocks = MBR_st->dirNumBlocks;
        LBAread(ptrOpen, blocks, temp->directoryStartLocation);
    
        //iterate over all entries, looking for a name match
        for (int i = 0; i < STARTING_NUM_DIR; i++)
        {
            if (strcmp(ptrOpen[i].name, newName) == 0)
            {
                //copy over all relevant data to the new open fd
                fileOpen[fd].Fd = fd;
                fileOpen[fd].isAllocate = TRUE;
                fileOpen[fd].flag = flags;
                fileOpen[fd].entryIndex = i;
                fileOpen[fd].buffer = malloc(B_CHUNK_SIZE);
                if (fileOpen[fd].buffer == NULL)
                {
                    write(2, "b_open: malloc failed\n", 23);
                    return (-1);
                }
                fileOpen[fd].writeBuffer = malloc(B_CHUNK_SIZE);
                if (fileOpen[fd].writeBuffer == NULL)
                {
                    write(2, "b_open: malloc failed\n", 23);
                    free(ptrOpen);
                    return (-1);
                }
                fileOpen[fd].locationLBA = ptrOpen[i].locationLBA;
                fileOpen[fd].childLBA = ptrOpen[i].childLBA;
                fileOpen[fd].entryIndex = ptrOpen[i].entryIndex;
                fileOpen[fd].dataLocation = ptrOpen[i].dataLocation;
                strcpy(fileOpen[fd].name, ptrOpen[i].name);
                fileOpen[fd].sizeOfFile = ptrOpen[i].sizeOfFile;
                fileOpen[fd].type = ptrOpen[i].type;
                fileOpen[fd].numBlocks = ptrOpen[i].numBlocks;
                fileOpen[fd].extents = ptrOpen[i].extents;
                fileOpen[fd].dateModified = ptrOpen[i].dateModified;
                time(&(fileOpen[fd].dateAccessed));

                free(ptrOpen);
                ptrOpen = NULL;
                return (fd);
            }
        }
        if (flags & O_CREAT)
        {
            //printf("IN CREATE\n");
            int freeIndex = -1;
            for (int i = 0; i < STARTING_NUM_DIR; i++)
            {
                if (ptrOpen[i].isBeingUsed == 0)
                {
                    //printf("FREEE INDEX: %d\n", i);
                    freeIndex = i;
                    break;
                }
            }
            if (freeIndex == -1)
            {
                //printf("no space\n");
                return -1;
            }

            //create a new directory entry
            initEntry(&ptrOpen[freeIndex]);
            ptrOpen[freeIndex].locationLBA = temp->directoryStartLocation; //location of this entry in logical block
            ptrOpen[freeIndex].entryIndex = freeIndex;
            strcpy(ptrOpen[freeIndex].name, newName);
            ptrOpen[freeIndex].isBeingUsed = 1;     
            ptrOpen[freeIndex].type = 'F';
            
            LBAwrite(ptrOpen, blocks, temp->directoryStartLocation); //write to disk
            
            //assign all relevant info open file object
            fileOpen[fd].Fd = fd;
            fileOpen[fd].isAllocate = TRUE;
            fileOpen[fd].flag = flags;
            fileOpen[fd].entryIndex = freeIndex;
            fileOpen[fd].locationLBA = temp->directoryStartLocation;
            fileOpen[fd].type = ptrOpen[freeIndex].type;
            fileOpen[fd].buffer = malloc(B_CHUNK_SIZE);
            if (fileOpen[fd].buffer == NULL)
            {
                write(2, "b_open: malloc failed\n", 23);
                free(ptrOpen);
                return (-1);
            }
            fileOpen[fd].writeBuffer = malloc(B_CHUNK_SIZE);
            if (fileOpen[fd].writeBuffer == NULL)
            {
                write(2, "b_open: malloc failed\n", 23);
                free(ptrOpen);
                return (-1);
            }

            free(ptrOpen);
            ptrOpen = NULL;
            return fd;
        }
        else
        {
            printf("file not found\n");
            free(ptrOpen);
            ptrOpen = NULL;
            return -1;
        }
    }
}

// Interface to Close the file
void b_close(int fd)
{
    unsigned long LBA = 1;
    
    //flag this fd for close
    fileOpen[fd].flaggedForClose = TRUE;

    //file was open for read only
    if(fileOpen[fd].flag == (int) O_RDONLY) {
        //printf("༼ つ ◕_◕ ༽つ\n");
    }

    //File was open for write
    else
    { 
        LBA = getExtentLBA(fd, TRUE);
        char* temp = calloc(512, sizeof(char*)); //create a temp buffer to prep with correct bytes
        memcpy(temp, fileOpen[fd].writeBuffer, fileOpen[fd].lenBuffer);
        unsigned long blocksWritten = LBAwrite(temp, 1, LBA);
        fileOpen[fd].sizeOfFile += fileOpen[fd].lenBuffer;
        fileOpen[fd].numBlocks++;

        //create a dirEntry pointer, malloc dirBUfMallocSize bytes
        dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        if (ptr == NULL)
        {
            printf("Error with malloc ln96.\n");
            return;
        }
        
        //fill ptr buffer
        LBAread(ptr, MBR_st->dirNumBlocks, fileOpen[fd].locationLBA);

        //find the entry and call update
        dirEntry* entry = &ptr[fileOpen[fd].entryIndex];
        updateEntry(fd, entry);

        if(ptr) {
            free(ptr);
            ptr = NULL;
        }
    }

    if (fileOpen[fd].isAllocate == FALSE)
    {
        printf("This fd is already free\n");
    }
    else
    {
        free(fileOpen[fd].buffer);
        free(fileOpen[fd].writeBuffer);
        fileOpen[fd].Fd = -1;
        fileOpen[fd].isAllocate = FALSE;
        fileOpen[fd].Fd = -1;
        fileOpen[fd].isAllocate = FALSE;
        fileOpen[fd].flag = -1;
        fileOpen[fd].flaggedForClose = FALSE;

        fileOpen[fd].locationLBA = DEFAULT_LBA;
        fileOpen[fd].childLBA = DEFAULT_LBA;
        fileOpen[fd].entryIndex = DEFAULT_INDEX;
        fileOpen[fd].dataLocation = DEFAULT_LBA;
        fileOpen[fd].name[0] = '\0';
        fileOpen[fd].sizeOfFile = DEFAULT_SIZE;
        fileOpen[fd].numBlocks = DEFAULT_SIZE;
        fileOpen[fd].locationMetadata = DEFAULT_LBA;
        fileOpen[fd].extents = DEFAULT_LBA;
        fileOpen[fd].numExtents = DEFAULT_SIZE;
        fileOpen[fd].numExtentBlocks = DEFAULT_SIZE;
        fileOpen[fd].type = DEFAULT_FILETYPE;

        fileOpen[fd].filePointer = DEFAULT_INDEX;
        fileOpen[fd].blockPointer = DEFAULT_INDEX;
        fileOpen[fd].buffer = '\0';
        fileOpen[fd].writeBuffer = '\0';
        fileOpen[fd].bufIndex = DEFAULT_INDEX;
        fileOpen[fd].buflen = DEFAULT_SIZE;
        fileOpen[fd].lenBuffer = DEFAULT_SIZE;
        fileOpen[fd].LBAInDataLocation = DEFAULT_LBA;
        fileOpen[fd].offsetInDataLocation = DEFAULT_LBA;
        fileOpen[fd].extentArrayPtrWrite = DEFAULT_INDEX;
        fileOpen[fd].extentArrayPtrRead = DEFAULT_INDEX;
    }
}

int b_write(int fd, char *buffer, int count)
{
    //printf("I am in b_write...\n");
    int n = 0;
    int returnValue = 0;

    //we only perform write if the proper flags allow it
    if (fileOpen[fd].flag == O_RDWR || O_WRONLY || O_TRUNC)
    {
        // check that fd is between 0 and (MAXFCBS-1)
        if ((fd < 0) || (fd >= MAXFCBS))
        {
            return (-1); //invalid file descriptor
        }

        if (fileOpen[fd].Fd == -1) //File not open for this descriptor
        {
            return (-1);
        }

        if (fileOpen[fd].isAllocate == FALSE) // NOT ALLOCATE
        {
            return (-1);
        }

        if (fileOpen[fd].lenBuffer + count <= BUFSIZE) // STILL HAVE PLACE IN THE WriteBuffer
        {
            for (int nbr = 0; nbr != count;)
            {
                fileOpen[fd].writeBuffer[fileOpen[fd].lenBuffer] = buffer[nbr];
                fileOpen[fd].lenBuffer++;
                nbr++;
            }
        }
        else if (fileOpen[fd].lenBuffer + count >= BUFSIZE) // NOT ENOUGHT PLACE IN THE WriteBuffer
        {
            for (n = 0; fileOpen[fd].lenBuffer != BUFSIZE;)
            {
                fileOpen[fd].writeBuffer[fileOpen[fd].lenBuffer] = buffer[n];
                fileOpen[fd].lenBuffer++;
                count--;
                n++;
            }
            
            unsigned long LBA = getExtentLBA(fd, TRUE); //get the next available LBA block to write to
            fileOpen[fd].extentArrayPtrWrite++; //increment the extent array index
            LBAwrite(fileOpen[fd].writeBuffer, 1, LBA); //write to disk
            fileOpen[fd].filePointer += BUFSIZE; //increment the file pointer in bytes read
            fileOpen[fd].numBlocks++; //increment the blocks by one
            fileOpen[fd].sizeOfFile += BUFSIZE; //increment the size of file
            returnValue = returnValue + fileOpen[fd].lenBuffer;
            fileOpen[fd].lenBuffer = 0;
            memset(fileOpen[fd].writeBuffer, '\0', BUFSIZE);
            if (fileOpen[fd].lenBuffer + count >= BUFSIZE)
            {
                for (; count >= BUFSIZE;)
                {
                    for (; fileOpen[fd].lenBuffer != BUFSIZE;)
                    {
                        fileOpen[fd].writeBuffer[fileOpen[fd].lenBuffer] = buffer[n];
                        fileOpen[fd].lenBuffer++;
                        count--;
                        n++;
                    }
                    unsigned long LBA = getExtentLBA(fd, TRUE); //get the next available LBA block to write to
                    fileOpen[fd].extentArrayPtrWrite++; //increment the extent array index
                    LBAwrite(fileOpen[fd].writeBuffer, 1, LBA); //write to disk
                    fileOpen[fd].filePointer += BUFSIZE; //increment the file pointer by bytes read
                    fileOpen[fd].sizeOfFile += BUFSIZE; //increment the size of file
                    fileOpen[fd].numBlocks++;
                    returnValue = returnValue + fileOpen[fd].lenBuffer;
                    fileOpen[fd].lenBuffer = 0;
                    memset(fileOpen[fd].writeBuffer, '\0', BUFSIZE);
                }
            }
            else
            {
                for (int a = 0; a != count;)
                {
                    fileOpen[fd].writeBuffer[a] = buffer[n];
                    fileOpen[fd].lenBuffer++;
                    n++;
                    a++;
                }
            }
        }
    }
    return (returnValue);
}

// Interface to read a buffer
int b_read(int fd, char *buffer, int count)
{
    //printf("in read"); 
    if (!(fileOpen[fd].flag == O_RDONLY||O_RDWR))
    {
        //printf("------------------------------\n");
        perror("Error: We do not have read permissions.\n");
        return -1;
    }
    int bytesRead;           // for our reads
    int bytesReturned;       // what we will return
    int part1, part2, part3; // holds the three potential copy lengths
    unsigned long LBA = 0;   // variable that stores the next LBA position to read from


    if (startup == 0)
        b_init(); //Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return (-1); //invalid file descriptor
    }

    if (fileOpen[fd].Fd == -1) //File not open for this descriptor
    {
        return -1;
    }
    
    // number of bytes available to copy from buffer
    int remain = fileOpen[fd].buflen - fileOpen[fd].bufIndex;
    part3 = 0;
    if(fileOpen[fd].blockPointer >= (fileOpen[fd].numBlocks )) //we left off at the end of the second to last block
    {
        //printf("should be here"); 
        int lastChunk = fileOpen[fd].sizeOfFile % BLOCK_SIZE;
        lastChunk = lastChunk - fileOpen[fd].bufIndex;
        if (count < lastChunk) { 
            memcpy(buffer, fileOpen[fd].buffer + (fileOpen[fd].bufIndex % BLOCK_SIZE) , count);
            fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + count;
            return count; 
        } else { 
            //printf("LAST CHUNK??? %d\n", lastChunk); 
            memcpy(buffer, fileOpen[fd].buffer + (int) (fileOpen[fd].bufIndex % BLOCK_SIZE), lastChunk);
            fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + lastChunk;
            return lastChunk; 
        }
    }           //only used if count > BUFSIZE
    if (remain >= count) //we have enough in buffer
    {
        part1 = count; // completely buffered
        part2 = 0;
    }
    else
    {
        part1 = remain; //spanning buffer (or first read)
        part2 = count - remain;
    }

    if (part1 > 0) // memcpy part 1
    {
        memcpy(buffer, fileOpen[fd].buffer + fileOpen[fd].bufIndex, part1);
        fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + part1;
        if (fileOpen[fd].bufIndex == 512) {
            fileOpen[fd].bufIndex = 0;
            if (part2 == 0){
                LBA = getExtentLBA(fd, FALSE);
                fileOpen[fd].extentArrayPtrRead++;
                LBAread(fileOpen[fd].buffer, 1, LBA);
                fileOpen[fd].blockPointer++;
                fileOpen[fd].filePointer += bytesRead;  
            } 
        } 
    }

     if (part2 > 0) //We need to read to copy more bytes to user
    {
        // Handle special case where user is asking for more than a buffer worth
        while (part2 > BLOCK_SIZE)
        {
            //int blocks = part2 / BUFSIZE; // calculate number of blocks they want
            LBA = getExtentLBA(fd, FALSE);
            fileOpen[fd].extentArrayPtrRead++;
            LBAread(fileOpen[fd].buffer, 1, LBA);
            fileOpen[fd].buflen = 512;
            fileOpen[fd].blockPointer++; 
            if(fileOpen[fd].blockPointer >= (fileOpen[fd].numBlocks )) //we left off at the end of the second to last block
            {
                int lastChunk = fileOpen[fd].sizeOfFile % BLOCK_SIZE;
                lastChunk = lastChunk - fileOpen[fd].bufIndex;
                //printf("%d", fileOpen[fd].bufIndex % BLOCK_SIZE);
                if (part2 < lastChunk) { 
                    memcpy(buffer + part1 + part3, fileOpen[fd].buffer + (fileOpen[fd].bufIndex % BLOCK_SIZE) , part2);
                    fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + part2;
                    //printf("HELLLOOO, %s", buffer);
                    return count; 
                } else {
                    //printf("hello?? %d", (fileOpen[fd].bufIndex % BLOCK_SIZE)); 
                    memcpy(buffer + part1 + part3, fileOpen[fd].buffer + (int) (fileOpen[fd].bufIndex % BLOCK_SIZE), lastChunk);
                    fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + lastChunk;
                    //buffer[lastChunk + 1] = '\0';
                    //printf("HELLLOOO, %s", buffer); 
                    return part1 + part3 + lastChunk; 
                }
            }
            memcpy(buffer + part1 + part3, fileOpen[fd].buffer + fileOpen[fd].bufIndex, BLOCK_SIZE);
            bytesRead = BLOCK_SIZE;
            
            fileOpen[fd].filePointer += bytesRead;                 //add the length of this buffer to fp
            fileOpen[fd].bufIndex = 0;                    // update the buffer index
            part3 += bytesRead;
            part2 = part2 - bytesRead; //part 2 is now < BUFSIZE, or file is exusted
            
            
        }

        //try to read BUFSIZE bytes into our buffer
        
        if(fileOpen[fd].blockPointer == fileOpen[fd].numBlocks){
            bytesRead = 0;
        }
        else if (fileOpen[fd].bufIndex == 0 || fileOpen[fd].bufIndex == 512){
            LBA = getExtentLBA(fd, FALSE);
            fileOpen[fd].extentArrayPtrRead++;
            LBAread(fileOpen[fd].buffer, 1, LBA);
            fileOpen[fd].blockPointer++;
            
            if(fileOpen[fd].blockPointer >= (fileOpen[fd].numBlocks)) //we left off at the end of the second to last block
            {
                int lastChunk = fileOpen[fd].sizeOfFile % BLOCK_SIZE;
                lastChunk = lastChunk - fileOpen[fd].bufIndex;
                //printf("%d", fileOpen[fd].bufIndex % BLOCK_SIZE);
                if (part2 < lastChunk) { 
                    memcpy(buffer + part1 + part3, fileOpen[fd].buffer + (fileOpen[fd].bufIndex % BLOCK_SIZE) , part2);
                    fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + part2;
                    //printf("HELLLOOO, %s", buffer);
                    return count; 
                } else {
                    //printf("hello?? %d", (fileOpen[fd].bufIndex % BLOCK_SIZE)); 
                    memcpy(buffer + part1 + part3, fileOpen[fd].buffer + (int) (fileOpen[fd].bufIndex % BLOCK_SIZE), lastChunk);
                    fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + lastChunk;
                    //buffer[lastChunk + 1] = '\0';
                    //printf("HELLLOOO, %s", buffer); 
                    return part1 + part3 + lastChunk; 
                }
            }
            else bytesRead = BLOCK_SIZE;
            fileOpen[fd].filePointer += bytesRead; // update the file pointer
            //fileOpen[fd].bufIndex += bytesRead;    // update the buffer index


            fileOpen[fd].bufIndex = 0;
            fileOpen[fd].buflen = bytesRead;
        }
         //how many bytes are actually in buffer

        if (bytesRead < part2) // not even enough left to satisfy read
            part2 = bytesRead;

        if (part2 > 0) // memcpy bytesRead
        {
            memcpy(buffer + part1 + part3, fileOpen[fd].buffer + fileOpen[fd].bufIndex, part2);
            fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + part2;
            //printf("%d", fileOpen[fd].bufIndex); 
        }
    }
    bytesReturned = part1 + part2 + part3;
    return (bytesReturned);
}

int b_seek(int fd, off_t offset, int whence)
{
    //reposition the file pointer
    switch (whence)
    {
    //SEEK_SET= file offset set to offset bytes
    case SEEK_SET:
        fileOpen[fd].filePointer = fileOpen[fd].dataLocation + offset; //reposition fp to dataLocation + offset
        return fileOpen[fd].filePointer; //return fp

    //SEEK_CUR= file offset is set to its current location plus offset
    case SEEK_CUR:
        fileOpen[fd].filePointer += offset; //fp += offset
        return fileOpen[fd].filePointer; //return fp;

    //SEEK_END= file offset is set to the size of the file plus offset bytes
    case SEEK_END:
        fileOpen[fd].filePointer = fileOpen[fd].sizeOfFile + offset; //position fp to the end of file
        return fileOpen[fd].filePointer; //return fp;
    default:
        perror("Error: invalid value of whence for b_seek.\n");
        return -1;
    }
    return 0;
}


