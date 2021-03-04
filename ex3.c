/*************************************
* Lab 2 Exercise 3
* Name: Yen Jun Hung
* Student Id: A0208749R
* Lab Group: B14
*************************************
* Name: Chieu Le Heng
* Student Id: A0209943X
* Lab Group: B02
*************************************
Note: Duplicate the above and fill in
for the 2nd member if  you are on a team
--------------------------------------
Warning: Make sure your code works on
compute cluster node (Linux on x86)
*************************************/

#include <stdio.h>
#include <fcntl.h>      //For stat()
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/wait.h>   //for waitpid()
#include <unistd.h>     //for fork(), wait()
#include <string.h>     //for string comparison etc
#include <stdlib.h>     //for malloc()


char** split( char* input, char* delimiter, int maxTokenNum, int* readTokenNum )
//Assumptions:
//  - the input line is a string (i.e. with NULL character at the end)
//  - the delimiter is a string of possible delimiters, each delimiter is single chracter
//Behavior:
//  - Split up to and include maxTokenNum from the input string
//Return: Tokenized substrings as array of strings
//        readTokenNum stores the total number of tokens
//Note:
//  - Must use the freeTokenArray to free memory after use!
{
    char** tokenStrArr;
    char* tStart;   //start of token
    int i, strSize;

    //allocate token array, each element is a char*
    tokenStrArr = (char**) malloc(sizeof(char*) * maxTokenNum );

    //Nullify all entries
    for (i = 0; i < maxTokenNum; i++){
        tokenStrArr[i] = NULL;
    }

    //Look for next token as separated by the delimiter
    tStart = strtok(input, delimiter);

    i = 0;
    while (i < maxTokenNum && tStart){

        strSize = strlen(tStart);

        //Allocate space for token string. +1 for null terminator
        tokenStrArr[i] = (char*) malloc(sizeof(char) * (strSize + 1) );

        strcpy(tokenStrArr[i], tStart);

        i++;
        tStart = strtok(NULL, delimiter);
    }

    //return number of token read
    *readTokenNum = i;

    return tokenStrArr;
}

void freeTokenArray(char** strArr, int size)
{
    int i;

    //Free each string first
    for (i = 0; i < size; i++){
        if (strArr[i])
            free( strArr[i] );
        strArr[i] = NULL;
    }

    //Free the entire array
    free( strArr );

    //Note: Caller still need to set the strArr parameter to NULL
    //      afterwards
}


int main()
{
    char **cmdLineArgs;
    char path[20] = ".";  //default search path
    char command[20] = ".";
    char userInput[121];
    pid_t child_Pids[11] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; //one extra just in case
    int child_Counter = 0;
    int tokenNum;
    int background_Process = 0;
    int wait_Correctly = 0;
    int status;
    struct stat stats;

    //read user input
    printf("YWIMC > ");

    //read up to 120 characters from user input
    fgets(userInput, 120, stdin);

    //split is a helper function to "tokenize" the input
    //see "stringTokenizer.c" program for more sample usage
    cmdLineArgs = split( userInput, " \n", 7, &tokenNum );

    //At this point you have the user input split neatly into token in cmdLineArg[]

    while ( strcmp( cmdLineArgs[0], "quit") != 0 ){

        //Figure out which command the user want and implement below

        //clear the list of pids of children who are finished
        for (int i = 0;i<10;i++){
            if (!child_Pids[i]){
                waitpid(child_Pids[i], &status, WNOHANG);
                if (WIFEXITED(status))child_Pids[i] = NULL;
                break;
            }
        }
        //Figure out which command the user want and implement below
        if (( strcmp( cmdLineArgs[0], "showpath") == 0 )){
            printf("%s\n",path);
        }

        else if (( strcmp( cmdLineArgs[0], "setpath") == 0 )){
            strcpy(path,cmdLineArgs[1]);
        }

        else if (( strcmp( cmdLineArgs[0], "wait") == 0 )){
            wait_Correctly = 0;
            for (int i = 0;i<10;i++){
                    if (strtoll(cmdLineArgs[1],NULL,10) == child_Pids[i]){
                        waitpid(child_Pids[i], NULL, 0);
                        child_Pids[i] = NULL;
                        wait_Correctly = 1;
                        break;
                    }
                }
            if (wait_Correctly == 0){
            printf("%ld is not a valid child pid\n",strtoll(cmdLineArgs[1],NULL,10));
            }
        }
        else if (( strcmp( cmdLineArgs[0], "pc") == 0 )){
            printf("Unwaited Child Processes:\n");
            for (int i = 0;i<10;i++){
                    if (child_Pids[i] !=NULL){
                        printf("%ld\n",child_Pids[i]);
                    }
                }
        }

        else if (( strcmp( cmdLineArgs[0], "result") == 0 )){
            printf("%d\n",status);
        }

        else{
            strcpy(command,path);
            strcat(command,"/");
            strcat(command,cmdLineArgs[0]);
            for (int i = 0; i< 7;i++){
                if (!cmdLineArgs[i]){
                    background_Process = 0;
                    if(strcmp(cmdLineArgs[i-1],"&")==0) background_Process = 1;
                    break;
                }
            }


            if (stat(command,&stats)==0){
                for (int i = 0;i<10;i++){
                    if (!child_Pids[i]) child_Counter = i;
                }
                child_Pids[child_Counter] = fork();
                if (child_Pids[child_Counter] == 0){

                    execv(command,cmdLineArgs);
                    exit(3);
                }
                else{
                    if (background_Process == 1) printf("Child %d in background\n",child_Pids[child_Counter]);
                   if (background_Process == 0){
                   waitpid(0, &status, 0);
                   child_Pids[child_Counter] = NULL;
                   }
                }
            }
            else{
                printf("\"%s\" not found \n",command);
            }
        }
        //Prepare for next round input

        //Clean up the token array as it is dynamically allocated
        freeTokenArray(cmdLineArgs, tokenNum);

        printf("YWIMC > ");
        fgets(userInput, 120, stdin);
        cmdLineArgs = split( userInput, " \n", 7, &tokenNum );
    }


    printf("Goodbye!\n");

    //Clean up the token array as it is dynamically allocated
    freeTokenArray(cmdLineArgs, tokenNum);


    return 0;

}
