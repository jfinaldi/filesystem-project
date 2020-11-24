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

    startPartitionSystem(argv[1], &volumeSize, &blockSize);
    MBRinit(volumeSize, blockSize, argv);
    // b_open("/foo/dir/test.txt", O_WRONLY | O_RDONLY);
    // exit(0);
    // int okay = b_open("/beep/helloFile", O_CREAT); 
	// int okay2 = b_open("/beep/helloFile2", O_RDONLY); 
	// int check1 = b_open("/beep/helloFile", O_RDONLY); 
	// int check2 = b_open("/beep/helloFile2", O_RDONLY);
	//  printf("check1 %d, check2 %d", check1, check2);
    char *cmdin;
    char *cmd;
    HIST_ENTRY *he;

    using_history();
    stifle_history(200); //max history entries
    
    //START TESTS
    //SWITCH TO 0 AFTER HAVING INITIALIZED A SAMPLE VOLUME 
    if (0) {
        printf("Starting test Initial Volume:\n"); 
        printf("initial root directry:\n"); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n");
        
        printf("root Q:\n"); 
        char* test1[]={
        "cd", "Q" 
        };
        cmd_cd(2, test1); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("root QQ:\n"); 
        char* test2[]={
        "cd", "QQ" 
        };
        cmd_cd(2, test2); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("md relative:\n"); 
        char* test3[]={
        "md", "relativeTest" 
        };
        cmd_md(2, test3); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("md absolute:\n"); 
        char* test4[]={
        "md", "/absoluteTest" 
        };
        cmd_md(2, test4); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("cd absolute:\n"); 
        char* test5[]={
        "cd", "/absoluteTest" 
        };
        cmd_cd(2, test5); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/") == 0);
        printf("ok: \n\n") ;

        printf("cd relative with QQ:\n"); 
        char* test6[]={
        "cd", "QQ/relativeTest" 
        };
        cmd_cd(2, test6);
        printf("%s\n", fdDirCWD->cwd_path);  
        assert(strcmp(fdDirCWD->cwd_path, "/relativeTest/") == 0);
        printf("ok\n\n") ;

        printf("md absolute inside other directory from other directory:\n"); 
        char* test7[]={
        "md", "/absoluteTest/absoluteChild" 
        };
        cmd_md(2, test7);
        printf("%s\n", fdDirCWD->cwd_path);  
        assert(strcmp(fdDirCWD->cwd_path, "/relativeTest/") == 0);
        printf("ok\n\n") ;

        printf("cd into absolute inside other directory from other directory:\n"); 
        char* test8[]={
        "cd", "/absoluteTest/absoluteChild" 
        };
        cmd_cd(2, test8); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/") == 0);
        printf("ok\n\n") ;

        printf("cd QQ/QQ\n"); 
        char* test9[]={
        "cd", "QQ/QQ" 
        };
        cmd_cd(2, test9); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("md relative 3 deep:\n"); 
        char* test10[]={
        "md", "absoluteTest/absoluteChild/relativeGrandchild" 
        };
        cmd_md(2, test10); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("cd into grandchild relative:\n"); 
        char* test11[]={
        "cd", "absoluteTest/absoluteChild/relativeGrandchild" 
        };
        cmd_cd(2, test11); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeGrandchild/") == 0);
        printf("ok\n\n") ;

        printf("md relative cousin:\n"); 
        char* test12[]={
        "md", "QQ/relativeCousin" 
        };
        cmd_md(2, test12); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeGrandchild/") == 0);
        printf("ok\n\n") ;

        printf("cd into cousin absolute:\n"); 
        char* test13[]={
        "cd", "/absoluteTest/absoluteChild/relativeCousin" 
        };
        cmd_cd(2, test13); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/") == 0);
        printf("ok\n\n") ;

        printf("md 4Deep:\n"); 
        char* test14[]={
        "md", "4Deep" 
        };
        cmd_md(2, test14); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/") == 0);
        printf("ok\n\n") ;

        printf("cd into 4Deep:\n"); 
        char* test15[]={
        "cd", "4Deep" 
        };
        cmd_cd(2, test15); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/4Deep/") == 0);
        printf("ok\n\n") ;

        printf("cd Q:\n"); 
        char* test16[]={
        "cd", "Q" 
        };
        cmd_cd(2, test16); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/4Deep/") == 0);
        printf("ok\n\n") ;

        printf("cd QQ:\n"); 
        char* test17[]={
        "cd", "QQ" 
        };
        cmd_cd(2, test17); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/") == 0);
        printf("ok\n\n") ;
        
    
    } else if (0){
        printf("Starting test:\n"); 
        printf("initial root directry:\n"); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n");
        
        printf("root Q:\n"); 
        char* test1[]={
        "cd", "Q" 
        };
        cmd_cd(2, test1); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("root QQ:\n"); 
        char* test2[]={
        "cd", "QQ" 
        };
        cmd_cd(2, test2); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        
        printf("cd absolute:\n"); 
        char* test5[]={
        "cd", "/absoluteTest" 
        };
        cmd_cd(2, test5); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/") == 0);
        printf("ok: \n\n") ;

        printf("cd relative with QQ:\n"); 
        char* test6[]={
        "cd", "QQ/relativeTest" 
        };
        cmd_cd(2, test6);
        printf("%s\n", fdDirCWD->cwd_path);  
        assert(strcmp(fdDirCWD->cwd_path, "/relativeTest/") == 0);
        printf("ok\n\n") ;

        printf("cd into absolute inside other directory from other directory:\n"); 
        char* test8[]={
        "cd", "/absoluteTest/absoluteChild" 
        };
        cmd_cd(2, test8); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/") == 0);
        printf("ok\n\n") ;

        printf("cd QQ/QQ\n"); 
        char* test9[]={
        "cd", "QQ/QQ" 
        };
        cmd_cd(2, test9); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/") == 0);
        printf("ok\n\n") ;

        printf("cd into grandchild relative:\n"); 
        char* test11[]={
        "cd", "absoluteTest/absoluteChild/relativeGrandchild" 
        };
        cmd_cd(2, test11); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeGrandchild/") == 0);
        printf("ok\n\n") ;

        printf("cd into cousin absolute:\n"); 
        char* test13[]={
        "cd", "/absoluteTest/absoluteChild/relativeCousin" 
        };
        cmd_cd(2, test13); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/") == 0);
        printf("ok\n\n") ;

        printf("cd into 4Deep:\n"); 
        char* test15[]={
        "cd", "4Deep" 
        };
        cmd_cd(2, test15); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/4Deep/") == 0);
        printf("ok\n\n") ;

        printf("cd Q:\n"); 
        char* test16[]={
        "cd", "Q" 
        };
        cmd_cd(2, test16); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/4Deep/") == 0);
        printf("ok\n\n") ;

        printf("cd QQ:\n"); 
        char* test17[]={
        "cd", "QQ" 
        };
        cmd_cd(2, test17); 
        printf("%s\n", fdDirCWD->cwd_path); 
        assert(strcmp(fdDirCWD->cwd_path, "/absoluteTest/absoluteChild/relativeCousin/") == 0);
        printf("ok\n\n") ;
    }
    
    //END TESTS
    while (1)
    {
       //printf("\ncurr block: %ld\n", fdDirCWD->directoryStartLocation);
        printf("\n%s~", DEFAULT_USERNAME);
        if(strcmp(fdDirCWD->cwd_path, "/") != 0) 
        {        
            printf("%s", fdDirCWD->cwd_path);
        }
        cmdin = readline("$ ");
        /*
        printf("\ncurr block: %ld\n", fdDirCWD->directoryStartLocation);
        printf("\nCWD: %s ", fdDirCWD->cwd_path);
        cmdin = readline(" > ");*/
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