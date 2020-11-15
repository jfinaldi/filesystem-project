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
        fileOpen[i].filePointer = 0;
        fileOpen[i].flag = -1;
        fileOpen[i].isAllocate = FALSE;
        fileOpen[i].buffer = '\0';
        fileOpen[i].buflen = 0;
        fileOpen[i].bufIndex = 0;
        fileOpen[i].filePointer = 0;
        fileOpen[i].locationLBA = 20000;
        fileOpen[i].dataLocation = 20000;
        fileOpen[i].sizeOfFile = 0;
        fileOpen[i].numBlocks = 0;
        fileOpen[i].locationMetadata = 20000;
        fileOpen[i].isBeingUsed = 0;
        fileOpen[i].childLBA = 20000;
        fileOpen[i].entryIndex = 0;
        fileOpen[i].offsetInDataLocation = 20000;

    }
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
        if (temp -> directoryStartLocation == 20000) {
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
                fileOpen[fd].buffer = malloc(B_CHUNK_SIZE);
                if (fileOpen[fd].buffer == NULL)
                {
                    write(2, "b_open: malloc failed\n", 23);
                    return (-1);
                }
                fileOpen[fd].locationLBA = ptrOpen[curr].locationLBA;
                fileOpen[fd].childLBA = ptrOpen[curr].childLBA;
                fileOpen[fd].entryIndex = ptrOpen[curr].entryIndex;
                fileOpen[fd].dataLocation = ptrOpen[curr].dataLocation;
                fileOpen[fd].indexInDataLocation = ptrOpen[curr].dataLocation;
                strcpy(fileOpen[fd].name, ptrOpen[curr].name);
                fileOpen[fd].sizeOfFile = ptrOpen[curr].sizeOfFile;
                fileOpen[fd].numBlocks = ptrOpen[curr].numBlocks;
                fileOpen[fd].dateModifiedDirectory = ptrOpen[curr].dateModifiedDirectory;
                time(&(fileOpen[fd].dateAccessedDirectory));
                free(ptrOpen);
                ptrOpen = NULL;
                printf("found fd\n"); 
                return (fd);
            }
        }
        if (flags & O_CREAT) {
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
            if (freeIndex == -1){
                printf("no space\n"); 
                return -1;
            }
            ptrOpen[freeIndex].locationLBA = temp->directoryStartLocation;  //location of this entry in logical block
            ptrOpen[freeIndex].entryIndex = freeIndex;

            //initialize a default name for this child
            strcpy(ptrOpen[freeIndex].name, newName);

            ptrOpen[freeIndex].sizeOfFile = 20 * 512; // the number of bytes of the file data
            ptrOpen[freeIndex].numBlocks = 20;	// the number of blocks occupied by the file
            //ptrOpen[freeIndex].id = -1; //the id number for the entry

            time(&(ptrOpen[freeIndex].dateModifiedDirectory)); // date the file was last modified
            time(&(ptrOpen[freeIndex].dateAccessedDirectory)); // date the file was last accessed

            ptrOpen[freeIndex].locationMetadata = find_free_index(20); //512 file per directory
            ptrOpen[freeIndex].isBeingUsed = 1;		  //this file is currently not being used
            ptrOpen[freeIndex].type = 'f';
            LBAwrite(ptrOpen, blocks, temp->directoryStartLocation);
            fileOpen[fd].Fd = fd;
                fileOpen[fd].isAllocate = TRUE;
                fileOpen[fd].flag = flags;
                fileOpen[fd].BufferRead = malloc(B_CHUNK_SIZE);
                if (fileOpen[fd].BufferRead == NULL)
                {
                    write(2, "b_open: malloc failed\n", 23);
                    free(ptrOpen);
                    return (-1);
                }
                fileOpen[fd].BufferWrite = malloc(B_CHUNK_SIZE);
                if (fileOpen[fd].BufferWrite == NULL)
                {
                    write(2, "b_open: malloc failed\n", 23);
                    free(ptrOpen);
                    return (-1);
                }
                fileOpen[fd].locationLBA = ptrOpen[curr].locationLBA;
                fileOpen[fd].childLBA = ptrOpen[curr].childLBA;
                fileOpen[fd].entryIndex = ptrOpen[curr].entryIndex;
                fileOpen[fd].dataLocation = ptrOpen[curr].dataLocation;
                fileOpen[fd].indexInDataLocation = ptrOpen[curr].dataLocation;
                strcpy(fileOpen[fd].name, ptrOpen[curr].name);
                fileOpen[fd].sizeOfFile = ptrOpen[curr].sizeOfFile;
                fileOpen[fd].numBlocks = ptrOpen[curr].numBlocks;
                fileOpen[fd].dateModifiedDirectory = ptrOpen[curr].dateModifiedDirectory;
                time(&(fileOpen[fd].dateAccessedDirectory));
                free(ptrOpen);
                ptrOpen = NULL;
                printf("found fd"); 
                return (fd);
            return fd; 
        } else {
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
    if (fileOpen[fd].isAllocate == FALSE)
    {
        printf("This fd is already free\n");
    }
    else
    {
        //TODO Write what is left inside the buffer of FD struct
        free(fileOpen[fd].BufferWrite);
        free(fileOpen[fd].BufferRead);
        fileOpen[fd].Fd = -1;
        fileOpen[fd].isAllocate = FALSE;
    }

    //if this fd was written to, update the entry
    //if(fileOpen[fd].flag == O_WRONLY || fileOpen[fd].flag == O_RDWR
    //    || fileOpen[fd].flag == O_CREAT) 
    //{
       /* //create a dirEntry pointer, malloc dirBUfMallocSize bytes
        dirEntry *ptr = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        if (ptr == NULL)
        {
            printf("Error with malloc ln96.\n");
            return ((fdDir *)-1);
        }
        else
            printf("Malloc succeeded\n\n");

        //fill ptr buffer
        LBAread(ptr, MBR_st->dirNumBlocks, fileOpen[fd].directoryStartLocation);

        //find the entry and call update
        dirEntry* entry = ptr[fileOpen[fd].entryIndex];
        updateEntry(fileOpen[fd], entry);*/
    //}
}

int b_write(int fd, char *buffer, int count)
{ // this file system is limited to 20 open files at once.
    /*int n = 0;
    int returnValue = 0;

    // the two ifs below check if the file descriptor table is open or if it exists
    // check that fd is between 0 and (MAXFCBS-1) kind of like in assignment 2 we made an array of 20 structs
     if ((fd < 0) || (fd >= MAXFCBS)) //given value index does not match the limit of 20 open files.
     {
         perror("File descriptor outside range 0-20\n");
         return (-1); //invalid file descriptor
     }

     if (fd == -1) //File not open for this descriptor
     {
         perror("The file was not opened or created\n"); //file was not initilized so open was not called
         return (-1);
     }


     //TO DO: Check for Write permission still in the process

     /*
     skeleton format to test if a file is write only 
     
     if(fileOpen->flag == O_WRONLY) {
         printf("The file descriptor specified does have the O_WRITE permission\n");
         return(-1);
     }
     
     
     
     
     

     if (fileOpen[fd].isAllocate == FALSE)
     { // this portion deals with memory allocation BUFSIZE = 512
         fileOpen[fd].BufferWrite = malloc(BUFSIZE);
         if (fileOpen[fd].BufferWrite == NULL)
         {
             // very bad, we can not allocate our buffer not enought memory
             perror("ERROR IN ALLOCATION:\n");
             close(fd);                 // close linux file
             fileOpen[fd].Fd = -1;      //Free opened File
             return (-1);
         }
         fileOpen->isAllocate = TRUE;
     }
     int Total_LBA_Write_Blocks = 0; 
     int Int_Number_LBA_Writes = count/512; // get the integer number of blocks to write does not include remainder
     // due to integer division.
     int Remainder_Bytes = count - Int_Number_LBA_Writes*BUFSIZE; 
    
     if (Remainder_Bytes > 0) {
         // there was a few remaining bytes under 512 i need an extra block to fit those so add 1 block
         Total_LBA_Write_Blocks = Int_Number_LBA_Writes + 1;
     }
     else {
         //division was perfect no remainder so the count perfectly divided with no remainder
         Total_LBA_Write_Blocks = Int_Number_LBA_Writes;
     }


      //am i going to be given free lba location blocks once the file is opened or not?
      //or do i have to allocate my own memory
      //int Start_Location_Of_LBA = find_free_index(Total_LBA_Write_Blocks);
      
      fileOpen[fd].BufferWrite = buffer;

      for(int i = 0; i < Total_LBA_Write_Blocks; i++) {
          int Written_Blocks = LBAwrite(fileOpen[fd].BufferWrite, 1, fileOpen[fd].dataLocation + i);
          if (Written_Blocks == 1) { // we wrote a whole block and the return is 1 so incement by 512          
              fileOpen[fd].BufferWrite = fileOpen[fd].BufferWrite + 512;
               }
               else if (Written_Blocks == 0) { // if return of LBA return = 0
                //update eofLBA = 
                // fileOpen[Fd].offsetDataLocation = Remainder_Bytes;




               }
               else {
                   perror("The return of LBAWrite is not 0 or 1\n");
               }
               fileOpen[fd].numBlocks++;
      }
      
      
                //DO NOT RETURN THE LINUX SYSTEM CALL RETURN ASSINGMENT 5


    // if (fcbArray[fd].lenBuffer + count <= BUFSIZE) // STILL HAVE PLACE IN THE WriteBuffer
    // {
    //     for (int nbr = 0; nbr != count;)
    //     {
    //         fcbArray[fd].writeBuffer[fcbArray[fd].lenBuffer] = buffer[nbr];
    //         fcbArray[fd].lenBuffer++;
    //         nbr++;
    //     }
    // }
    // else if (fcbArray[fd].lenBuffer + count >= BUFSIZE) // NOT ENOUGHT PLACE IN THE WriteBuffer
    // {
    //     for (n = 0; fcbArray[fd].lenBuffer != BUFSIZE;)
    //     {
    //         fcbArray[fd].writeBuffer[fcbArray[fd].lenBuffer] = buffer[n];
    //         fcbArray[fd].lenBuffer++;
    //         count--;
    //         n++;
    //     }
    //     write(fcbArray[fd].linuxFd, fcbArray[fd].writeBuffer, fcbArray[fd].lenBuffer);
    //     returnValue = returnValue + fcbArray[fd].lenBuffer;
    //     fcbArray[fd].lenBuffer = 0;
    //     memset(fcbArray[fd].writeBuffer, '\0', BUFSIZE);
    //     if (fcbArray[fd].lenBuffer + count >= BUFSIZE)
    //     {
    //         for (; count >= BUFSIZE;)
    //         {
    //             for (; fcbArray[fd].lenBuffer != BUFSIZE;)
    //             {
    //                 fcbArray[fd].writeBuffer[fcbArray[fd].lenBuffer] = buffer[n];
    //                 fcbArray[fd].lenBuffer++;
    //                 count--;
    //                 n++;
    //             }
    //             write(fcbArray[fd].linuxFd, fcbArray[fd].writeBuffer, fcbArray[fd].lenBuffer);
    //             returnValue = returnValue + fcbArray[fd].lenBuffer;
    //             fcbArray[fd].lenBuffer = 0;
    //             memset(fcbArray[fd].writeBuffer, '\0', BUFSIZE);
    //         }
    //     }
    //     else
    //     {
    //         for (int a = 0; a != count;)
    //         {
    //             fcbArray[fd].writeBuffer[a] = buffer[n];
    //             fcbArray[fd].lenBuffer++;
    //             n++;
    //             a++;
    //         }
    //     }
    // }
    return (returnValue);*/
    return 1;
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
    int bytesRead;           // for our reads
    int bytesReturned;       // what we will return
    int part1, part2, part3; // holds the three potential copy lengths

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
    int remain = fileOpen[fd].buflenRead - fileOpen[fd].indexRead;
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
        memcpy(buffer, fileOpen[fd].BufferRead + fileOpen[fd].indexRead, part1);
        fileOpen[fd].indexRead = fileOpen[fd].indexRead + part1;
    }

    if (part2 > 0) //We need to read to copy more bytes to user
    {
        // Handle special case where user is asking for more than a buffer worth
        if (part2 > BUFSIZE)
        {
            int blocks = part2 / BUFSIZE; // calculate number of blocks they want
            //bytesRead = read(fcbArray[fd].linuxFd, buffer + part1, blocks * BUFSIZE);
            //LBAread returns 0 so we have to figure out another way to see how many bytes were actually read
            bytesRead = LBAread(fileOpen[fd].BufferRead, 1, fileOpen[fd].indexInDataLocation);
            fileOpen[fd].indexInDataLocation = fileOpen[fd].indexInDataLocation + 1;
            fileOpen[fd].filePointer = fileOpen[fd].filePointer + strlen(fileOpen[fd].buffer); //add the length of this buffer to fp
            part3 = bytesRead;
            part2 = part2 - part3; //part 2 is now < BUFSIZE, or file is exusted
        }

        //try to read BUFSIZE bytes into our buffer
        //bytesRead = read(fcbArray[fd].linuxFd, fcbArray[fd].buf, BUFSIZE);
        bytesRead = LBAread(fileOpen[fd].BufferRead, 1, fileOpen[fd].indexInDataLocation);
        fileOpen[fd].indexInDataLocation = fileOpen[fd].indexInDataLocation + 1;

        // error handling here...  if read fails

        fileOpen[fd].indexRead = 0;
        fileOpen[fd].buflenRead = bytesRead; //how many bytes are actually in buffer

        if (bytesRead < part2) // not even enough left to satisfy read
            part2 = bytesRead;

        if (part2 > 0) // memcpy bytesRead
        {
            memcpy(buffer + part1 + part3, fileOpen[fd].BufferRead + fileOpen[fd].indexRead, part2);
            fileOpen[fd].indexRead = fileOpen[fd].indexRead + part2;
        }
    }
    bytesReturned = part1 + part2 + part3;
    return (bytesReturned);
}

int b_seek(int fd, int offset, int whence)
{
    //reposition the file pointer
    //
    //Whence: 
        //SEEK_SET= file offset set to offset bytes
        //SEEK_CUR= file offset is set to its current location plus offset
        //SEEK_END= file offset is set to the size of the file plus offset bytes
    //this fcn returns the offset location (bytes) from the start of file

    //
    return 0;
}
