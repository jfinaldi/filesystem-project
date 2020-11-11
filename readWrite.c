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
        fileOpen[i].flag = -1;
        fileOpen[i].isAllocate = FALSE;
        fileOpen[i].BufferRead = '\0';
        fileOpen[i].BufferWrite = '\0';
        fileOpen[i].buflenRead = 0;
        fileOpen[i].buflenWrite = 0;
        fileOpen[i].indexRead = 0;
        fileOpen[i].indexWrite = 0;
        fileOpen[i].locationLBA = 20000;
        fileOpen[i].dataLocation = 20000;
        fileOpen[i].sizeOfFile = 0;
        fileOpen[i].numBlocks = 0;
        fileOpen[i].locationMetadata = 20000;
        fileOpen[i].isBeingUsed = 0;
        fileOpen[i].childLBA = 20000;
        fileOpen[i].entryIndex = 0;
        fileOpen[i].indexInDataLocation = 20000;
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

    if (filename[0] == '/')
    { //Absolute path
        printf("ABSOLUTE PATH\n");
        curr = MBR_st->rootDirectoryPos;
        dirEntry *ptrOpen = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        if (ptrOpen == NULL)
        {
            write(2, "b_open: malloc failed\n", 23);
            return (-1);
        }
        LBAread(ptrOpen, MBR_st->dirNumBlocks, MBR_st->rootDirectoryPos);
        for (int n = 0; n != nbArgument; n++)
        {
            printf("Name = %s\n", ptrOpen->name);
            printf("entryIndex = %d\n", ptrOpen->entryIndex);
            printf("locationMetadata = %ld\n", ptrOpen->locationMetadata);
            printf("childLBA = %ld\n", ptrOpen->childLBA);
            found = FALSE;
            for (int i = 0; i < STARTING_NUM_DIR || found == TRUE; i++)
            {
                printf("NAME = %s, name = %s\n", ptrOpen[i].name, path_token[n]);
                if (strcmp(ptrOpen[i].name, path_token[n]) == 0)
                {
                    printf("NAME = %s, HI HELLO %d < i %ld", ptrOpen[i].name, i, ptrOpen[i].childLBA);
                    curr = i;
                    i = STARTING_NUM_DIR + 1;
                    found = TRUE;
                }
            }
            if (n != nbArgument && found == FALSE)
            {
                return (1); //directory is not exist
            }
            if (n == nbArgument)
            {
                if (found == TRUE) // IF the file exist
                {
                    if (flags == O_CREAT)
                    {
                        return (-1); //Already created
                    }
                    if (flags == O_RDWR || O_WRONLY || O_TRUNC || O_RDONLY)
                    {
                        fileOpen[fd].Fd = fd;
                        fileOpen[fd].isAllocate = TRUE;
                        fileOpen[fd].flag = flags;
                        fileOpen[fd].BufferRead = malloc(B_CHUNK_SIZE);
                        if (fileOpen[fd].BufferRead == NULL)
                        {
                            write(2, "b_open: malloc failed\n", 23);
                            return (-1);
                        }
                        fileOpen[fd].BufferWrite = malloc(B_CHUNK_SIZE);
                        if (fileOpen[fd].BufferWrite == NULL)
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
                        return (fd);
                    }
                }
                if (found == FALSE) // IF the file NOT exist
                {
                    if (flags == O_CREAT)
                    {
                        //TODO Create a file
                        return (fd);
                    }
                    if (flags == O_RDWR || O_WRONLY || O_TRUNC || O_RDONLY)
                    {
                        return (-1); // File not exist
                    }
                }
            }
            LBAread(ptrOpen, MBR_st->dirNumBlocks, ptrOpen[curr].childLBA);
        }
    }
    else
    { // relative path
        printf("RELATIVE PATH\n");
        curr = fdDirCWD->dirEntryPosition;
        dirEntry *ptrOpen = (dirEntry *)malloc(MBR_st->dirBufMallocSize);
        if (ptrOpen == NULL)
        {
            write(2, "b_open: malloc failed\n", 23);
            return (-1);
        }
        LBAread(ptrOpen, MBR_st->dirNumBlocks, fdDirCWD->dirEntryPosition);
        for (int n = 0; n != nbArgument; n++)
        {
            printf("Name = %s\n", ptrOpen->name);
            printf("entryIndex = %d\n", ptrOpen->entryIndex);
            printf("locationMetadata = %ld\n", ptrOpen->locationMetadata);
            printf("childLBA = %ld\n", ptrOpen->childLBA);
            found = FALSE;
            for (int i = 0; i < STARTING_NUM_DIR || found == TRUE; i++)
            {
                if (strcmp(ptrOpen[i].name, path_token[n]) == 0)
                {
                    printf("NAME = %s, HI HELLO %d < i %ld", ptrOpen[i].name, i, ptrOpen[i].childLBA);
                    curr = i;
                    i = STARTING_NUM_DIR + 1;
                    found = TRUE;
                }
            }
            if (n != nbArgument && found == FALSE)
            {
                return (1); //directory is not exist
            }
            if (n == nbArgument)
            {
                if (found == TRUE) // IF the file exist
                {
                    if (flags == O_CREAT)
                    {
                        return (-1); //Already created
                    }
                    if (flags == O_RDWR || O_WRONLY || O_TRUNC || O_RDONLY)
                    {
                        fileOpen[fd].Fd = fd;
                        fileOpen[fd].isAllocate = TRUE;
                        fileOpen[fd].flag = flags;
                        fileOpen[fd].BufferRead = malloc(B_CHUNK_SIZE);
                        if (fileOpen[fd].BufferRead == NULL)
                        {
                            write(2, "b_open: malloc failed\n", 23);
                            return (-1);
                        }
                        fileOpen[fd].BufferWrite = malloc(B_CHUNK_SIZE);
                        if (fileOpen[fd].BufferWrite == NULL)
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
                        return (fd);
                    }
                }
                if (found == FALSE) // IF the file NOT exist
                {
                    if (flags == O_CREAT)
                    {
                        //TODO Create a file
                        return (fd);
                    }
                    if (flags == O_RDWR || O_WRONLY || O_TRUNC || O_RDONLY)
                    {
                        return (-1); // File not exist
                    }
                }
            }
            LBAread(ptrOpen, MBR_st->dirNumBlocks, ptrOpen[curr].childLBA);
        }
    }
    return (-1); // all set
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
}

int b_write(int fd, char *buffer, int count)
{
    int n = 0;
    int returnValue = 0;

    // // check that fd is between 0 and (MAXFCBS-1)
    // if ((fd < 0) || (fd >= MAXFCBS))
    // {
    //     return (-1); //invalid file descriptor
    // }

    // if (fcbArray[fd].linuxFd == -1) //File not open for this descriptor
    // {
    //     return (-1);
    // }

    // if (fcbArray[fd].allocateBuffer == FALSE)
    // {
    //     fcbArray[fd].writeBuffer = malloc(BUFSIZE);
    //     if (fcbArray[fd].buf == NULL)
    //     {
    //         // very bad, we can not allocate our buffer
    //         close(fd);                 // close linux file
    //         fcbArray[fd].linuxFd = -1; //Free FCB
    //         return (-1);
    //     }
    //     fcbArray[fd].allocateBuffer = TRUE;
    // }

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
            bytesRead = LBAread(fileOpen[fd].BufferRead, 1, fileOpen[fd].indexInDataLocation);
            fileOpen[fd].indexInDataLocation = fileOpen[fd].indexInDataLocation + 1;
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
