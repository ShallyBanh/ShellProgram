/**
 * Shally Banh
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/stat.h>


static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

/**
 * Starts the clock 
 */
void start_clock(){
    st_time = times(&st_cpu);
}


/**
 * Ends the clock and calculates the time passed since start_clock() was called
 */
void end_clock(){
  en_time = times(&en_cpu);

  printf("\nTotal Time: %ld, User Time %ld, System Time %ld, Child User Time %ld, Child System Time %ld\n",
    (en_time - st_time),
    (en_cpu.tms_utime - st_cpu.tms_utime),
    (en_cpu.tms_stime - st_cpu.tms_stime),
    (en_cpu.tms_cutime - st_cpu.tms_cutime),
    (en_cpu.tms_cstime - st_cpu.tms_cstime));
}

/**
 * Prints the current working directory when the user enters "pwd" in a1shell
 */
void our_pwd(){
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL){
       printf("Current Working Directory: %s\n", cwd);
  }
  else{
    //print to standard out that we can't get the current directory
    printf("Cannot get current directory: Error occurred is %s\n", strerror(errno));
  }

}

/**
 * Changes the current working directory to the pathname specififed.
 * @param pathname
 */
void our_cd(char* pathname){
  //PATH_MAX is defined in # include <limit.h>
  char path[PATH_MAX];
  char *expansion;
  char *token;
  memset(path, 0, PATH_MAX);

  //if it's an absolute path we need to append this to the beginning since
  //strtok will strip it in the next line
  if(pathname[0] == '/'){
    strcat(path, "/");
  }
  token = strtok (pathname,"/");
  while (token != NULL)
  {
    //we have an enivronment variable so have to handle differently
    if(token[0] == '$'){
      //gets rid of the '$' char at the beginning
      token++;
      expansion = getenv(token);

      //Cannot find the expansion variable
      if(expansion == NULL){
        printf("Invalid Expansion Variable: $%s\n", token);
        return;
      }

      strcat(path, expansion);
    }

    else{
      //add the token to our path
      strcat(path, token);
    }

    token = strtok (NULL, "/");
    //There's still more of the path to go through so add "/" char
    if(token != NULL){
      strcat(path, "/");
    }
  }

  if (chdir(path) == -1){
    printf("Error occured when trying to change the current working directory using chdir(): Error occured is: %s\n", strerror(errno));
  }
}
/**
 * Exits the program
 */
void our_done(){
  exit(EXIT_SUCCESS);
}

/**
 * Prints the current file mode creation mask, S_IRXWU, S_IRXWG, S_IRXWO in octal.
 */
void our_umask(){
  mode_t mask = umask(0);
  //umask always succeeds so we don't have to do an error check
  umask(mask);
  printf("Unmask: %04o, S_IRXWU: %04o, S_IRXWG: %04o, S_IRXWO: %04o\n", mask , S_IRWXU, S_IRWXG, S_IRWXO);

}

/**
 * Executes the command given in bash. A child is forked to execute the command
 * @param commandToRun
 */
void our_cmd(char* commandToRun){
  int status;
  //start timing 
  start_clock();
  pid_t pid = fork();

  //error whilg calling fork()
  if(pid == -1){
    printf( "Failed to fork: Error ocurred is: %s\n", strerror(errno) );
  }

  //child process
  else if(pid == 0 ){
    //Failed to execute the command via bash
    if(execl("/bin/bash", "bash", "-c", commandToRun, (char *) 0) == -1){
      printf( "Faied to execute command [%s] via bash: Error ocurred is: %s\n", commandToRun, strerror(errno) );
    }
    kill(pid, SIGTERM);
    exit(0);
  }

  //parent process
  else {
    //wait for child process to terminate
    waitpid(pid, &status, 0);
    //stop timing
    end_clock();
  }

}


/**
 * Helper function to help grab the user input from stdin
 */
void getText(char *userInput, int size){
    fgets(userInput, sizeof(char) * size, stdin);
    sscanf(userInput, "%[^\n]", userInput);
}


int main(int argc, char*argv[]){
  int status;

  if (argv[1] == NULL || argv[1] == ""){
    printf("Error: no interval entered. Please enter an interval\n");
    exit(0);
  }

  struct rlimit r1;
  char * commandlineToken;
  //units of seconds = this equates to 10 mins
  r1.rlim_cur = 10 * 60;
  //set the user limit to 10 mins
  setrlimit(RLIMIT_CPU, &r1); 


  pid_t parentPid = getpid();
  pid_t pid = fork();

  //error forking
  if (pid == -1){
    printf( "Failed to fork: Error ocurred is: %s\n", strerror(errno) );
  } 

  //child process
  else if(pid == 0) {
    //try to run a1monitor
    if( execve("a1monitor", argv, NULL) == -1){
      printf( "Faied to run a1monitor: Error ocurred is: %s\n", strerror(errno) );
    }
    _exit(EXIT_FAILURE);   
  }

  //parent process
  else{
    //let the child print out it's a1monitor message first so it doesn't look so messy on start up
    sleep(1);
    while(1){
      char userInput[80];
      char findCommand[80];
     
      printf("alshell: ");
      //Get the user input from stdin
      getText(userInput, 80);
      //Create a copy of the userInput 
      strcpy(findCommand, userInput);
      commandlineToken = strtok (findCommand," ");

      if(strcmp(commandlineToken, "pwd") == 0){
       our_pwd();
      }
      else if(strcmp(commandlineToken, "done") == 0){
       kill(pid, SIGTERM);
       //wait til child kills itself to avoid any zombies
       waitpid(pid, &status, 0);
       our_done();
      }
      else if(strcmp(commandlineToken, "cd") == 0){
        commandlineToken = strtok (NULL, " ");
        //check if there's a pathname to change to 
        if( commandlineToken != NULL ){
          our_cd(commandlineToken);
        }
      }
      else if(strcmp(commandlineToken, "umask") == 0){
        our_umask();
      }
      else{
        our_cmd(userInput);
      }
    }
  }

  return 0;
}
