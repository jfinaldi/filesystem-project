/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: main.c
*
* Description: Main driver for file system assignment.
*
* Make sure to set the #defined on the CMDxxxx_ON from 0 to 1 
* when you are ready to test that feature
*
**************************************************************/

#include "mfs.h"

MBRstruct *MBR_st = NULL;
fdDir *fdDirCWD = NULL;

int main(int argc, char *argv[])
{

    uint64_t volumeSize;
    uint64_t blockSize;
    char *filename;
    int MBR = 0;

    if (argc > 3)
    {
        filename = argv[1];
        volumeSize = atoll(argv[2]);
        blockSize = atoll(argv[3]);
    }
    else
    {
        printf("Usage: ./fsshell volumeFileName volumeSize blockSize\n");
        return (-1);
    }
    MBR_st = malloc(512);
    fdDirCWD = malloc(sizeof(fdDir));

    if (MBR_st == NULL)
    {
        printf("ERROR: malloc main\n");
        return (1);
    }

    startPartitionSystem(argv[1], &volumeSize, &blockSize);
    MBRinit(volumeSize, blockSize, argv);
    // b_open("/foo/dir/test.txt", O_WRONLY | O_RDONLY);
    // exit(0);
    /*int okay = b_open("/beep/helloFile", O_CREAT); 
	int okay2 = b_open("/beep/helloFile2", O_RDONLY); 
	int check1 = b_open("/beep/helloFile", O_RDONLY); 
	int check2 = b_open("/beep/helloFile2", O_RDONLY);
	 printf("check1 %d, check2 %d", check1, check2);*/
    char *cmdin;
    char *cmd;
    HIST_ENTRY *he;

    using_history();
    stifle_history(200); //max history entries

    while (1)
    {

        printf("\ncurr block: %ld\n", fdDirCWD->directoryStartLocation);
        printf("\nCWD: %s ", fdDirCWD->cwd_path);
        cmdin = readline(" > ");
#ifdef COMMAND_DEBUG
        printf("%s\n", cmdin);
#endif

        cmd = malloc(strlen(cmdin) + 30);
        strcpy(cmd, cmdin);
        free(cmdin);
        cmdin = NULL;

        if (strcmp(cmd, "exit") == 0)
        {
            free(cmd);
            cmd = NULL;
            // exit while loop and terminate shell
            break;
        }

        if ((cmd != NULL) && (strlen(cmd) > 0))
        {
            he = history_get(history_length);
            if (!((he != NULL) && (strcmp(he->line, cmd) == 0)))
            {
                add_history(cmd);
            }
            processcommand(cmd);
        }

    } // end while
    closePartitionSystem();
    free(cmd);
    cmd = NULL;

    //deallocate our dirItemInfo pointer
    if (fdDirCWD->dirItemInfo)
        free(fdDirCWD->dirItemInfo);
    if (fileOpen != NULL)
        free(fileOpen);
    fdDirCWD->dirItemInfo = NULL;

    //deallocate our CWD struct
    if (fdDirCWD)
    {
        free(fdDirCWD);
        fdDirCWD = NULL;
    }
}