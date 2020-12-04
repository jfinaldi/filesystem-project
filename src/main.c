/**************************************************************
* Class:  CSC-415.02
* Name: Dreamteam
* Student ID: N/A
* Project: Basic File System
*
* File: main.c
*
* Description: This is the file for the main function that
*              interacts directly with fsshell.c
*	
*
**************************************************************/
#include "mfs.h"
#include <assert.h>

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
    initGlobals();
    startPartitionSystem(argv[1], &volumeSize, &blockSize);
    MBRinit(volumeSize, blockSize, argv);

    char *cmdin;
    char *cmd;
    HIST_ENTRY *he;

    using_history();
    stifle_history(200); //max history entries

    while (1)
    {
        printf("\n %ld", fdDirCWD-> directoryStartLocation);
        printf("\n%s~", DEFAULT_USERNAME);
        if(strcmp(fdDirCWD->cwd_path, "/") != 0) 
        {        
            printf("%s", fdDirCWD->cwd_path);
        }
        cmdin = readline("$ ");
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