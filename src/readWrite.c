/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: readWrite.c
*
* Description: 
*	This is the read write fonction.
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
        printf("Error entryToFd, invalid input\n");
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

    printf("%d\n", flags);
    // if (flags == O_RDONLY)
    // {
    //     printf("HERE\n");
    // }

    if (filename == NULL)
    {
        return (EXIT_FAILURE);
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
        printf("%s ", path_token[nbArgument]);
        printf(" %d\n", nbArgument);
        nbArgument++;
    }
    path_token[nbArgument] = NULL;
    // -------------- SPLIT PATH ----------------
    printf("NbArgument = %d\n", nbArgument);

    //! CHECK IF GOOD ????
    nbArgument = nbArgument - 1;
    //! CHECK IF GOOD ????

    printf("NbArgument = %d\n", nbArgument);

    if (1)
    { //Absolute path

        /*****FIX THIS MEMORY LEAK*********/
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
        /**********************************/

        printf("new name %s\n", newName);
        fdDir *temp = tempDirectory(filename, 0);
        if (temp->directoryStartLocation == 20000)
        {
            printf("not a valid path or name\n");
            return -1;
        }
        printf("HELLLO, dir start loc: %ld\n", temp->directoryStartLocation);
        dirEntry *ptrOpen = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        int blocks = MBR_st->dirNumBlocks;
        LBAread(ptrOpen, blocks, temp->directoryStartLocation);
        int fount = -1;
        for (int i = 0; i < STARTING_NUM_DIR; i++)
        {
            if (strcmp(ptrOpen[i].name, newName) == 0)
            {
                printf("found %s %s\n", newName, ptrOpen[i].name);
                fileOpen[fd].Fd = fd;
                fileOpen[fd].isAllocate = TRUE;
                fileOpen[fd].flag = flags;
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
                /*fileOpen[fd].locationLBA = ptrOpen[curr].locationLBA;
                fileOpen[fd].childLBA = ptrOpen[curr].childLBA;
                fileOpen[fd].entryIndex = ptrOpen[curr].entryIndex;
                fileOpen[fd].dataLocation = ptrOpen[curr].dataLocation;
                fileOpen[fd].LBAInDataLocation = ptrOpen[curr].dataLocation;
                strcpy(fileOpen[fd].name, ptrOpen[curr].name);
                fileOpen[fd].sizeOfFile = ptrOpen[curr].sizeOfFile;
                fileOpen[fd].numBlocks = ptrOpen[curr].numBlocks;
                fileOpen[fd].dateModified = ptrOpen[curr].dateModified;
                time(&(fileOpen[fd].dateAccessed));*/

                //copy data from dirEntry to fd_struct
                if(entryToFd(&ptrOpen[curr], fd) != 0){
                    printf("ERROR copying entry data in b_open ln209\n");
                    return -1;
                }
                free(ptrOpen);
                ptrOpen = NULL;
                printf("found fd\n");
                return (fd);
            }
        }
        if (flags & O_CREAT)
        {
            printf("IN CREATE\n");
            int freeIndex = -1;
            for (int i = 0; i < STARTING_NUM_DIR; i++)
            {
                if (ptrOpen[i].isBeingUsed == 0)
                {
                    printf("FREEE INDEX: %d\n", i);
                    freeIndex = i;
                    break;
                }
            }
            if (freeIndex == -1)
            {
                printf("no space\n");
                return -1;
            }

            /**********I THINK ALL THIS COULD BE AN initEntry() CALL************/
            ptrOpen[freeIndex].locationLBA = temp->directoryStartLocation; //location of this entry in logical block
            ptrOpen[freeIndex].entryIndex = freeIndex;

            //initialize a default name for this child
            strcpy(ptrOpen[freeIndex].name, newName);

            ptrOpen[freeIndex].sizeOfFile = 20 * 512; // the number of bytes of the file data
            ptrOpen[freeIndex].numBlocks = 20;        // the number of blocks occupied by the file
            //ptrOpen[freeIndex].id = -1; //the id number for the entry

            time(&(ptrOpen[freeIndex].dateModified)); // date the file was last modified
            time(&(ptrOpen[freeIndex].dateAccessed)); // date the file was last accessed

            //ptrOpen[freeIndex].locationMetadata = find_free_index(20); //512 file per directory
            ptrOpen[freeIndex].isBeingUsed = 1;                        //this file is currently not being used
            ptrOpen[freeIndex].type = 'f';
            /*******************************************************************/

            LBAwrite(ptrOpen, blocks, temp->directoryStartLocation);
            fileOpen[fd].Fd = fd;
            fileOpen[fd].isAllocate = TRUE;
            fileOpen[fd].flag = flags;
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

            /*********CREATE HELPER copyToFD() ***************/
            /*
            fileOpen[fd].locationLBA = ptrOpen[curr].locationLBA;
            fileOpen[fd].childLBA = ptrOpen[curr].childLBA;
            fileOpen[fd].entryIndex = ptrOpen[curr].entryIndex;
            fileOpen[fd].dataLocation = ptrOpen[curr].dataLocation;
            fileOpen[fd].LBAInDataLocation = ptrOpen[curr].dataLocation;
            strcpy(fileOpen[fd].name, ptrOpen[curr].name);
            fileOpen[fd].sizeOfFile = ptrOpen[curr].sizeOfFile;
            fileOpen[fd].numBlocks = ptrOpen[curr].numBlocks;
            fileOpen[fd].dateModified = ptrOpen[curr].dateModified;
            time(&(fileOpen[fd].dateAccessed));*/
            /*************************************************/

            //copy data from dirEntry to fd_struct
            if(entryToFd(&ptrOpen[curr], fd) != 0){
                printf("ERROR copying entry data in b_open ln209\n");
                return -1;
            }

            free(ptrOpen);
            ptrOpen = NULL;
            printf("found fd");
            return fd;
        }
        else
        {
            printf("file not found");
            free(ptrOpen);
            ptrOpen = NULL;
            return -1;
        }
    }
}

// Interface to Close the file
void b_close(int fd)
{
    //flag this fd for close
    fileOpen[fd].flaggedForClose = TRUE;

    //if this fd was written to, update the entry
    if(fileOpen[fd].flag == O_WRONLY || fileOpen[fd].flag == O_RDWR
        || fileOpen[fd].flag == O_CREAT)
    {
        //create a dirEntry pointer, malloc dirBUfMallocSize bytes
        dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        if (ptr == NULL)
        {
            printf("Error with malloc ln96.\n");
            return;
        }
        else
            printf("Malloc succeeded\n\n");

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
        //TODO Write what is left inside the buffer of FD struct
        free(fileOpen[fd].buffer);
        free(fileOpen[fd].buffer);
        fileOpen[fd].Fd = -1;
        fileOpen[fd].isAllocate = FALSE;
    }
}

int b_write(int fd, char *buffer, int count)
{
    int n = 0;
    int returnValue = 0;

    //ADD CALL TO ADD EXTENT IF WE HAVE NONE, OR UPDATE IF WE'RE OUT

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
            // write(fileOpen[fd].Fd, fileOpen[fd].writeBuffer, fileOpen[fd].lenBuffer);
            LBAwrite(fileOpen[fd].writeBuffer, 1, fileOpen[fd].dataLocation);
            // IS dataLocation the correct variable(where the data begin) ?
            //TODO ALLOCATE THE BLOCK AND WRITE THE DATA WITH LBAWRITE WHEN EXEED THE DEFAULT PREALLOCATE BLOCK FOR THE FILE
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
                    // write(fileOpen[fd].Fd, fileOpen[fd].writeBuffer, fileOpen[fd].lenBuffer);
                    LBAwrite(fileOpen[fd].writeBuffer, 1, fileOpen[fd].dataLocation);
                    // IS dataLocation the correct variable(where the data begin) ?
                    // IS THE 1 a corect varrible ?
                    //TODO ALLOCATE THE BLOCK AND WRITE THE DATA WITH LBAWRITE WHEN EXEED THE DEFAULT PREALLOCATE BLOCK FOR THE FILE
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

//Method to get a free FCB element
int b_getFCB()
{
    // for (int i = 0; i < MAXFCBS; i++)
    // {
    //     if (fcbArray[i].linuxFd == -1)
    //     {
    //         fcbArray[i].linuxFd = -2; // used but not assigned
    //         return i;                 //Not thread safe
    //     }
    // }
    return (-1); //all in use
}

// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR

// Interface to write a buffer

// Interface to read a buffer
int b_read(int fd, char *buffer, int count)
{
    printf("b_read:\n");
    printf("------------------------------\n");
    if (!(fileOpen[fd].flag & O_RDONLY) && !(fileOpen[fd].flag & O_RDWR))
    {
        printf("------------------------------\n");
        perror("Error: We do not have read permissions.\n");
        return -1;
    }
    int bytesRead;           // for our reads
    int bytesReturned;       // what we will return
    int part1, part2, part3; // holds the three potential copy lengths
    int prevBufSize = 0;     // caches the previous size of buffer to calculate bytesRead

    if (startup == 0)
        b_init(); //Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS))
    {
        printf("------------------------------\n");
        return (-1); //invalid file descriptor
    }

    if (fileOpen[fd].Fd == -1) //File not open for this descriptor
    {
        printf("------------------------------\n");
        return -1;
    }

    // number of bytes available to copy from buffer
    int remain = fileOpen[fd].buflen - fileOpen[fd].bufIndex;
    part3 = 0;           //only used if count > BUFSIZE
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
    }

    if (part2 > 0) //We need to read to copy more bytes to user
    {
        // Handle special case where user is asking for more than a buffer worth
        if (part2 > BUFSIZE)
        {
            int blocks = part2 / BUFSIZE; // calculate number of blocks they want
            //bytesRead = read(fcbArray[fd].linuxFd, buffer + part1, blocks * BUFSIZE);
            LBAread(fileOpen[fd].buffer, 1, fileOpen[fd].LBAInDataLocation);
            printf("prevBufSize: %d\n", prevBufSize);
            bytesRead = strlen(fileOpen[fd].buffer) - prevBufSize; //subtract prev buf size to get what we just read
            prevBufSize += bytesRead;                              //update the new previous buffer size for next time
            printf("bytesRead: %d\n", bytesRead);
            printf("prevBufSize: %d\n", prevBufSize);
            fileOpen[fd].LBAInDataLocation += 1; //increment data location by 1 LBA block
            fileOpen[fd].filePointer += bytesRead; //add the length of this buffer to fp
            //fileOpen[fd].bufIndex += bytesRead; //update bufIndex
            part3 = bytesRead;
            part2 = part2 - part3; //part 2 is now < BUFSIZE, or file is exusted
        }

        //try to read BUFSIZE bytes into our buffer
        LBAread(fileOpen[fd].buffer, 1, fileOpen[fd].bufIndex);
        printf("prevBufSize: %d\n", prevBufSize);
        bytesRead = strlen(fileOpen[fd].buffer) - prevBufSize; // cur size of buffer - what it used to be
        prevBufSize += bytesRead;                              // update the new previous buffer size for next time
        printf("bytesRead: %d\n", bytesRead);
        printf("prevBufSize: %d\n", prevBufSize);
        fileOpen[fd].LBAInDataLocation += 1;
        fileOpen[fd].filePointer += bytesRead; // update the file pointer
        //fileOpen[fd].bufIndex += bytesRead;                    // update the buffer index

        // error handling here...  if read fails

        fileOpen[fd].bufIndex = 0;
        fileOpen[fd].buflen = bytesRead; //how many bytes are actually in buffer

        if (bytesRead < part2) // not even enough left to satisfy read
            part2 = bytesRead;

        if (part2 > 0) // memcpy bytesRead
        {
            memcpy(buffer + part1 + part3, fileOpen[fd].buffer + fileOpen[fd].bufIndex, part2);
            fileOpen[fd].bufIndex = fileOpen[fd].bufIndex + part2;
        }
    }
    bytesReturned = part1 + part2 + part3;
    printf("returning bytesReturned = %d\n", bytesReturned);
    printf("------------------------------\n");
    return (bytesReturned);
}

int b_seek(int fd, off_t offset, int whence)
{
    printf("\nb_seek:\n");
    printf("------------------------------\n");
    printf("filePointer set to: %ld\n", fileOpen[fd].filePointer);
    //reposition the file pointer
    switch (whence)
    {
    //SEEK_SET= file offset set to offset bytes
    case SEEK_SET:
        fileOpen[fd].filePointer = fileOpen[fd].dataLocation + offset; //reposition fp to dataLocation + offset
        printf("filePointer set to: %ld\n", fileOpen[fd].filePointer);
        printf("------------------------------\n");
        return fileOpen[fd].filePointer; //return fp

    //SEEK_CUR= file offset is set to its current location plus offset
    case SEEK_CUR:
        fileOpen[fd].filePointer += offset; //fp += offset
        printf("filePointer set to: %ld\n", fileOpen[fd].filePointer);
        printf("------------------------------\n");
        return fileOpen[fd].filePointer; //return fp;

    //SEEK_END= file offset is set to the size of the file plus offset bytes
    case SEEK_END:
        fileOpen[fd].filePointer = fileOpen[fd].sizeOfFile + offset; //position fp to the end of file
        printf("filePointer set to: %ld\n", fileOpen[fd].filePointer);
        printf("------------------------------\n");
        //If the above line breaks: fp = dataLocation + sizeOfFile + offset
        return fileOpen[fd].filePointer; //return fp;
    default:
        perror("Error: invalid value of whence for b_seek.\n");
        printf("------------------------------\n");
        return -1;
    }
    return 0;
}
