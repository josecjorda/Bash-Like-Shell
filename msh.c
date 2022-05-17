/*
    Name: Jose Jordan
    ID: 1001837992
*/



// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  //pid history holds the last 15 pids, pid amount keeps track of amount of current pids
  pid_t pid_history[15] = {};
  int pidamount = 0;
  //history holds the last 15 pids, histamount keeps track of amount of current history
  char history[15][MAX_COMMAND_SIZE] = {};
  int histamount = 0;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         

    if(histamount < 15 && command_string[0]!='\n')
    {
        strcpy(history[histamount],command_string);
        histamount++;
    }
    else if(histamount==15 && command_string[0]!='\n')
    {
        char hold[MAX_COMMAND_SIZE];
        char hold2[MAX_COMMAND_SIZE];
        strcpy(hold,command_string);
        for(int x = 14; x>0;x--)
        {
            strcpy(hold2, history[x]);
            strcpy(history[x],hold);
            strcpy(hold,hold2);
        }
    }

    if(command_string[0] == '!')
    {
        int hist_index =atoi(&command_string[1]);
        strncpy(command_string,history[hist_index],MAX_COMMAND_SIZE);
    }

    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;



    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    pid_t pid = 0;

    if(token[0] == NULL)
    {
        //deals with blank inputs
    }
    else if(strcmp(token[0],"quit") ==0 || strcmp(token[0],"exit") == 0)
    {
        //exits program if quit or exit are entered
        exit(0);
    }
    else if(strcmp(token[0],"cd")==0)
    {
        //runs change directory
        chdir(token[1]);
    }
    else if(strcmp(token[0],"history")==0)
    {
        //prints history
        for(int x = 0; x<15; x++)
        {
            //checks to see if history entry is empty. if not it prints history entry
            if(strcmp(history[x],"")!=0)
            {
                printf("%d. %s",x,history[x]);
            }
        }
    }
    else if(strcmp(token[0],"pidhistory")==0 || strcmp(token[0],"showpids")== 0)
    {
        //prints pidhistory
        for(int x = 0; x<15; x++)
        {
            //checks to see if pidhistory entry is empty. if not it prints entry.
            if(pid_history[x]!=0)
            {
                printf("%d. %d\n",x,pid_history[x]);
            }
        }
    }
    else if(strcmp(token[0],"")!= 0)
    {
        int ret;
        pid = fork();
        if(pid == 0)
        {
            //child pid
            ret = execvp(token[0],&token[0]);
            if(ret ==-1)
            {
                //if exec doesn't work then command doesnt exist
                printf("Command not found.\n");
                exit(0);
            }
        }
        else
        {
            //parent waits for child
            wait(&ret);
        }
    }

    if(pidamount < 15 && pid!=0)
    {
        //adds to pid_history if there isnt already 15 pids in pidhistory
        pid_history[pidamount] = pid;
        pidamount++;
    }
    else if(pidamount == 15)
    {
        //if 15 entries removes entry 0 and moves the rest down an index
        pid_t pid_hold;
        pid_t pid_hold2;
        pid_hold = pid;
        for(int x = 14; x>0;x--)
        {
            pid_hold2 = pid_history[x];
            pid_history[x] = pid_hold;
            pid_hold = pid_hold2;
        }
    }

    free( head_ptr );

  }
  return 0;
  // e2520ca2-76f3-11ec-90d6-0242ac120003
}