//
//  mnouhsh.c
//  
//
//  Created by Mohamed Nouh on 09/27/11.
//  
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#define INPUTSIZE 100
#define DEBUG 0

char ** arguments = {NULL}; //This is the arguments being passed, in, each seperated by a space
char buffer[INPUTSIZE]; //This is the buffer for reading input
int charNumber = 0; //Number of characters
int myStatus; //I use for wait pid

/**
 * I use this command to clean my buffer and clean my arguments.
 */
void clean_arguments()
{
    int z = 0;
    int j = 0;
    
    for(z = 0; z < sizeof(buffer); z++)
    {
        
        //                *arguments[z] = '\0';
        
        buffer[z] = '\0';
        charNumber = 0;
        
    }
    
    for(j = 0; j < sizeof(*arguments); j++)
    {
        
        arguments[j] = NULL;    
        
    }
    
    
}
/*
 * This command outputs to the screen, takes the command, the bool of background
 * background == 0(not running background), background == 1, running background.
 */

void redirect_to_screen(char **command, int background, int loc_background)
{
        
    int status;

    if(background == 0) 
    {
      
        int pid;
        int fd;
        
        pid = fork();
        
        //Parent
        if(pid > 0) {
            wait(NULL);
        }
        //Child
        else if (pid == 0) 
        {
            
            status = execvp(command[0], command);
            
            if(status == -1)
            {
                fprintf(stderr, "Command not found, please try again.\n");
                
            }
            
        }
        else 
        {
            fprintf(stderr, "Could not fork");
            exit(0);
        }
    }
    
    else if(background == 1) 
    {
        
        int j = 0;
        int z = 0;
        char * temp_command[loc_background+1];
        
        
        for(z = 0; z < (loc_background+1); z++)
        {
            
            temp_command[z] = NULL;
            
        }
        
        
        for(j = 0; j < loc_background; j++)
        {
            
            temp_command[j] = command[j];
            
            
        }
        
        int pid;
        pid = fork();
        
        
        if (!pid)
        {
            fclose(stdin); // close child's stdin
            fopen("/dev/null", "r"); // open a new stdin that is always empty
            
            fprintf(stderr, "Child Job pid = %d\n", getpid());
            
            
            status = execvp(temp_command[0], temp_command);
            
            if(status == -1)
            {
                fprintf(stderr, "Command not found, please try again.\n");
                exit(1);
            }
            
        }
        
        waitpid(pid, &myStatus, WNOHANG);
        
        
    }
    
}

/**
 *
 * This will write to a file the command executed, arg2 = file
 */

void redirect_to_file(char **command, char * arg2, int background, int loc_background) 
{
	
        int pid;
        int fd;
        
        fd = open(arg2, O_WRONLY | O_TRUNC | O_CREAT, 0777);
        if (fd == 0) {
            fprintf(stderr, "Could not open the file %s", "");
            exit(0);
        }
        pid = fork();
        
        //Parent
        if(pid > 0) {
            
            if(background == 0) {
            wait(NULL);
            }
        }
        //Child
        else if (pid == 0) {
            dup2(fd, 1);
            int status = 0;
            status = execvp(command[0], command);
            close(fd);
            
            if(status == -1)
            {
                
                fprintf(stderr, "Could not fork");
                exit(0);
                
            }
            
            
        }
        else {
            fprintf(stderr, "Could not fork");
            exit(0);
        }
    
        
    
}
/**
 * This will read from file and write to standard input
 *
 */
void redirect_from_file(char **command, char * arg2, int background, int loc_background){

        int pid;
        int in;
        
        in = open(arg2, O_RDONLY);
        if (in == 0) {
            fprintf(stderr, "Could not open the file %s", "");
            exit(0);
        }
        pid = fork();
        
        //Parent
        if(pid > 0) {
            if(background == 0) {
            wait(NULL);
            }
        }
        //Child
        else if (pid == 0) {
            dup2(in, 0);
            int status = 0;
            status = execvp(command[0], command);
            close(in);
            
            if(status == -1)
            {
                
                fprintf(stderr, "Could not fork");
                exit(0);
                
            }
            
            
        }
        else {
            fprintf(stderr, "Could not fork");
            exit(0);
        }
        
   
}

/**
 *
 *This is my parser. It handles pretty much execution of all the calls, and formating of arguments. It determines if there is a background
 *or a < or a >. 
 */

void parse_arguments(char **command, int size, int total) 
{
    char * loc_element;
    int has_both = 0;
    int i = 0;
    int j = 0;
    int loc_arg = 0;
    int loc_input = 0;
    int input_found = 0;
    int output_found = 0;
    int loc_background = 0;
    int background = 0;
    
    char * quit = "exit";
    
    for(i = 0; i < total; i++) 
    {
        
        loc_element = command[i];
        
        for(j = 0; j < strlen(command[i]); j++) 
        { 
            
            if(loc_element[0] == 'e' && loc_element[1] == 'x' && loc_element[2] == 'i' && loc_element[3] == 't') {
                printf("\n\t\t\t\e[1;34mBye, Have a nice day! \e[1;34m \e[0m\n");
                exit(1);
                
            }
            
            if(loc_element[j] == '<')
            {
                
                loc_input = i;
                input_found = 1;
            }
            
            if(loc_element[j] == '>') 
            {
                
                loc_arg = i;
                output_found = 1;
                
            }
            
            
            if(loc_element[j] == '&')
            {
                
                loc_background = i;
                background = 1;
            }
            
            
        }
        
    }
    
    
    if(loc_arg == 0 && loc_input == 0) {
        
        redirect_to_screen(command, background, loc_background);
        
    }
    
    if(input_found == 1 && output_found == 1) {
        
        has_both = 1;
        
        
        if(loc_input < loc_arg) {
            
            command[loc_input] = NULL;
            
        }
        else if(loc_arg < loc_input) {
            
            command[loc_arg] = NULL;
            
            
        }
        
        int argFile = loc_input+1;
        
        char * arg1 = command[argFile];
        int argFile2 = loc_arg+1;
        
        char * arg2 = command[argFile2];
        
        
        int pid;
        int in;
        int fd;
        

        
        pid = fork();
        
        //Parent
        if(pid > 0) {
            
            if(background == 0) {
            wait(NULL);
            }
        }
        //Child
        else if (pid == 0) {
            
            
            in = open(arg1, O_RDONLY, 0664);
            if (in == 0) {
                fprintf(stderr, "Could not open the file %s", "");
                exit(0);
            }
            
            dup2(in, 0);
            
            fd = open(arg2, O_WRONLY | O_TRUNC | O_CREAT, 0777);
            if (fd == 0) {
                fprintf(stderr, "Could not open the file %s", "");
                exit(0);
            }
            
            dup2(fd, 1);
            
            int status = 0;
            status = execvp(command[0], command);
            
            if(status == -1)
            {
                
                fprintf(stderr, "Could not fork");
                exit(0);
                
            }
            
            close(in);
            close(fd);
            
            
        }
        else 
        {
            fprintf(stderr, "Could not fork");
            exit(0);
        }
   
    }
    else {
        
    if(input_found == 1 && output_found ==0)      
    {
        int argFile = loc_input+1;
        int z = 0;
        char * arg2 = command[argFile];
        int j = 0;
        
        char * temp_command[argFile];
        
        for(z = 0; z < (argFile); z++)
        {
            
            temp_command[z] = NULL;
            
        }
        
        for(j = 0; j < loc_input; j++) 
        {
         
            temp_command[j] = command[j];
            
        }
                
        char ** empty_command;
        redirect_from_file(temp_command, command[argFile], background, loc_background);
        
    }
    
        
    if(output_found == 1 && input_found == 0) 
    {
        
        int argFile = loc_arg+1;
        int z = 0;
        int j = 0;
        char * temp_command[argFile];
        
        for(z = 0; z < (argFile); z++)
        {
            
            temp_command[z] = NULL;
            
        }
        
        for(j = 0; j < loc_arg; j++) 
        {
            temp_command[j] = command[j];  
            
        }
        
        redirect_to_file(temp_command, command[argFile], background, loc_background);
        
        }
    
    }
    
}

void child_handler(int sigID)
{
    signal(SIGCHLD,child_handler); //allow SIGCHLD to catch child for next time;
    int status;
    pid_t pid;
    
    pid = wait(&status); //Child is free
    printf("\e[1;31mpid = %d , status = %d\e[0m\n", pid, status);
}


int main(int argc, char ** argv) 
{
    char input = '\0';
    int argCount = 0;
    int enterCount = 0;
    int inputIndex = 0;
    int executed = 0;
    pid_t pid;
    int status;
    char * about = {"about"};
    
    printf("\e[1;34mcs350sh> \e[1;34m \e[0m");
    
    while(input != EOF)
    {
        /*
        pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0)
            printf("\e[1;31m \e[1;31m \e[0m\n %d\n", pid);
*/
        
        signal(SIGCHLD,child_handler);
        
        input = getchar();
        
        
        if(input == '\n')
        {
        
            free(arguments);
            arguments = (char **)malloc((charNumber)*sizeof(char));
            executed = 1;
            char * arg = NULL;
            int argIndex = 0;
            argCount = 0;
            arg = strtok(buffer, " ");
            
            while(arg != NULL)
            {
                
                arguments[argIndex] = arg;
                arg = strtok(NULL, " ");
                argIndex++;
                argCount++;
                
            }
            
            char * element = arguments[0];
            enterCount++;
            
            if(element[0] == 'a' && element[1] == 'b' && element[2] == 'o' && element[3] == 'u' && element[4] == 't' ) {
                
                printf("%s", "\n\t\t\t\e[1;31mMade by Mohamed Nouh\n\t\t\tFall 2011\n\t\t\tOperating Systems \e[1;31m \e[0m\n");
                
                
            }
            else {
            
            parse_arguments(arguments, charNumber, argCount);
            }
            printf("\e[1;34mcs350sh> \e[1;34m \e[0m");
            inputIndex = 0;
            if(executed == 1) 
            {
                
                clean_arguments();
                arguments = NULL;
                executed = 0;
            }
            
            
        }
        else
        {
            
            buffer[inputIndex] = input;
            inputIndex++;
            charNumber++;
            
            
        }
        
        free(arguments); 
    }
    return 0;
    
    
}
