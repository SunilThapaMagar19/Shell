
//Mav shell
//Sunil Thapa Magar


#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 128    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell supports upto 10 arguments

#define MAX_HISTORY_SIZE 50      // Displaying the last 50 commands the user inputs

//history variables

char *history[MAX_HISTORY_SIZE];
int history_count = 0;

int main()
{

  // Shell should not terminate on Ctrl-C (SIGINT) or Ctrl-Z (SIGTSTP).
  struct sigaction act;
  
  // Zero out the sigaction struct
  memset(&act, '\0', sizeof(act));
  
  // Set the handler to ignore signals
  act.sa_handler = SIG_IGN;

  // Install the handler for SIGINT
  if (sigaction(SIGINT, &act, NULL) < 0)
  {
    perror("sigaction SIGINT");
    return 1;
  }

  // Install the handler for SIGTSTP
  if (sigaction(SIGTSTP, &act, NULL) < 0)
  {
    perror("sigaction SIGTSTP");
    return 1;
  }


  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

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

    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      token[i] = NULL;
    }

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;                                         
                                                           
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
    // \TODO Remove this for loop and replace with your shell functionality

    // int token_index  = 0;
    // for( token_index = 0; token_index < token_count; token_index ++ ) 
    // {
    //   printf("token[%d] = %s\n", token_index, token[token_index] );  
    // }

    // If user didnot enter any command, hit Enter (blank line)
    if (token[0] == NULL)
    {
      free(head_ptr);
      continue;
    }

    // save command to history (upto 50 lines)

    if(history_count < MAX_HISTORY_SIZE)
    {
      // Add until history is not full
      history[history_count] = strdup(command_string);
      history_count++;   //increment only when not full
    }
    else
    {
      // When history is full, remove the oldest command to make space for the new one

      free(history[0]);
      for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++)
      {
        history[i] = history[i + 1];
      }
      history[MAX_HISTORY_SIZE - 1] = strdup(command_string);   //new command is added to the end of the queue
    }

    // Re-runs command from history using "!#" where "#" is number
    if (token[0][0] == '!')
    {
      int cmd_num= atoi(&token[0][1]);

      // out of range number
      if (cmd_num < 1 || cmd_num > history_count)
      {
        printf("Command not found in history. \n");

        //clean up
        free(head_ptr);
        for (int i = 0; i < token_count; i++)
        {
          if(token[i] != NULL)
          {
            free(token[i]);
          }
        }
        continue;
      }
      
      //get command from history
      char *history_cmd = history[cmd_num - 1];
      strcpy(command_string, history_cmd);

      // Clean up old tokens
      free(head_ptr);
      for (int i = 0; i < token_count; i++)
        {
          if(token[i] != NULL)
          {
            free(token[i]);
          }
        }
    
      //Reset token array and Re-tokenize
      
      for (int i = 0; i < MAX_NUM_ARGUMENTS; i++) 
      { 
        token[i] = NULL; 
      }

      token_count = 0;
      working_string = strdup(command_string);
      head_ptr = working_string;
      
      while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) && 
              (token_count < MAX_NUM_ARGUMENTS))
      {
        token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
        if( strlen( token[token_count] ) == 0 )
        {
          token[token_count] = NULL;
        }
        token_count++;
      }
    }

    // Handle history command
    // Print the list of previous commands starting with 1
    if (strcmp(token[0], "history") == 0)
    {
      for (int i = 0; i < history_count; i++)
      {
        printf("[%d] %s", i + 1, history[i]); 
      }
    }

    // Handle cd command
    // Change the shell current directory
    else if (strcmp(token[0], "cd") == 0)     // Checking if the first command is "cd"
    {   
        //Check if user entered the new directory 
        if (token[1] == NULL)
        {
          printf("cd: Missing argument\n");
        }
        else
        {
            //if parent directory doesnot exist
            // chdir is used in chaning directory. It handles ".." too
            if (chdir(token[1]) != 0)
            {
              perror("cd failed");
            }
        }
    }
    // Check if the user entered "quit" or "exit"
    else if (strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
    {
      // Cleanup before exit
      for(int i = 0; i < token_count; i++)
      {
        if(token[i] != NULL) 
        {
          free(token[i]);
        }
      }

      free(head_ptr);

      //free whole history
      for(int i = 0; i < history_count; i++)
      {
        free(history[i]);
      }

      //free input buffer
      free(command_string);
      exit(0);
    }
    
    //pipe/redirection
    else
    {
      // Check for pipe character "|"
      int pipe_index = -1;
      for (int i = 0; i < token_count; i++)
      {
        if (token[i] != NULL && strcmp(token[i], "|") == 0)
        {
          pipe_index = i;
          break;
        }
      }

      // Handle piped commands "cmd1 | cmd2"
      if (pipe_index != -1)
      {
        int pfd[2];
        
        // Create the pipe with pfd[1] write end and pfd[2] read end
        if (pipe(pfd) == -1)
        {
          perror("pipe");
          free(head_ptr);
          continue;
        }

        // Split commands at pipe symbol
        token[pipe_index] = NULL;
        char **cmd1 = &token[0];
        char **cmd2 = &token[pipe_index + 1];


        // fork first child
        pid_t pid1 = fork();
        if (pid1 < 0)
        {
          perror("Fork failed");
          exit(1);
        }
        else if (pid1 == 0)
        {
          // First child: write to pipe
          close(pfd[0]); // Close read end
          dup2(pfd[1], STDOUT_FILENO); // Redirect stdout to pipe
          close(pfd[1]);


          // If exec fails, exit this child.
          if (execvp(cmd1[0], cmd1) == -1)
          {
            printf("%s: Command not found.\n", cmd1[0]);
            exit(1);
          }
        }

        // fork second child
        pid_t pid2 = fork(); 
        if (pid2 < 0)
        {
          perror("Fork failed");
          exit(1);
        }
        else if (pid2 == 0)
        {
          // Second child: read from pipe
          close(pfd[1]); // Close write end
          dup2(pfd[0], STDIN_FILENO); // Redirect stdin from pipe
          close(pfd[0]);
          
          if (execvp(cmd2[0], cmd2) == -1)
          {
            printf("%s: Command not found.\n", cmd2[0]);
            exit(1);
          }
        }

        // Parent: close both pipe ends and wait for both children
        close(pfd[0]);
        close(pfd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
    
    // if no pipe: maybe ">" redirection
    else
    {
      //Fork
      // Create a child process to run the command; the parent will wait for it to complete.
      
      pid_t pid = fork(); 

      // If fork fails, then pid will be less than 0
      if (pid < 0) 
      {
        perror("Fork failed");   //print error message
        exit(1);   //exit
      }
      // Code runs in child process
      else if (pid == 0)
      {
        for (int i = 0; i < token_count; i++)
        {

          // if there's a ">", set up redirection
          if (token[i] != NULL && strcmp(token[i], ">") == 0)
          {
            // Open the output file
            int fd = open(token[i+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd < 0){
              perror("Can't open output file");
              exit(1);
            }
            
            // Redirect stdout to the file
            dup2(fd, STDOUT_FILENO);
            close(fd);

            // Remove the > and filename from the command
            token[i] = NULL;
            break;
          }
        }
        // Child code is replaces
        // if command no exist, exit the child
        if (execvp(token[0], token) == -1)
        {
          printf("%s: Command not found.\n", token[0]);
          exit(1);
        }
      }
      else
      {
  
          wait(NULL); // Parent Wait for the child to complete
      }
    }
  }
   
    // Cleanup allocated memory
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      if( token[i] != NULL )
      {
        free( token[i] );
      }
    }
      free( head_ptr );

  }

  free( command_string );

  return 0;
}