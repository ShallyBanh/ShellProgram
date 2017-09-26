#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>
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

  printf("Total Time: %ld, User Time %ld, System Time %ld, Child User Time %ld, Child System Time %ld\n",
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
   char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL){
       printf("Current Working Directory: %s\n", cwd);
    }
   else{
      //print to standard error that we can't get the current directory
      fprintf(stderr, "getcwd() error");
   }

}

/**
 * @param pathname[char*]
 * Changes the current working directory to the pathname specififed.
 */
void our_cd(char* pathname){
  char path[1024];
  char *expansion;
  char *token;
  memset(path, 0, 1024);
  token = strtok (pathname,"/");
  while (token != NULL)
  {
    //we have an enivronment variable so have to handle differently
    if(token[0] == '$'){
      token++;
      expansion = getenv(token);
      strcat(path, expansion);
    }
    else{
      strcat(path, token);
    }
    token = strtok (NULL, "/");
    if(token != NULL){
      strcat(path, "/");
    }
  }
	printf("pathname is : %s\n", path);
	chdir(pathname);
}
/**
 * Exits the program and cleans up all threads
 */
void our_done(pid_t pid){
  kill(pid, SIGTERM);
  exit(EXIT_SUCCESS);
}

/**
 * Prints the current file mode creation mask, S_IRXWU, S_IRXWG, S_IRXWO in octal.
 */
void our_umask(){
  mode_t mask = umask(0);
  umask(mask);
  mode_t s_irwxu = umask(S_IRWXU);
  umask(s_irwxu);
  mode_t s_irwxg = umask(S_IRWXG);
  umask(s_irwxg);
  mode_t s_irwxo = umask(S_IRWXO);
  umask(s_irwxo);
 // , umask S_IRXWU: %u, umask S_IRXWG: %u, umask S_IRXWO: %u
  //,umask S_IRXWU: %o, umask S_IRXWG: %o, umask S_IRXWO: %o\n" mask , s_irwxu, s_irwxg, s_irwxo
  printf("Unmask: %o,umask S_IRXWU: %o, umask S_IRXWG: %o, umask S_IRXWO: %o\n", mask , s_irwxu, s_irwxg, s_irwxo);

}

/**
 * @param commandToRun[char*]
 * Executes the command given in bash. A child is forked to execute the command
 */
void our_cmd(char* commandToRun){
  int status;
  //start timing 
  start_clock();
  pid_t pid = fork();

  //error occured forking
  if(pid == -1){
    //print to standard error
    fprintf( stderr, "failed to fork()\n");
  }

  //child process
  else if(pid ==0){
    execl("/bin/bash", "bash", "-c", commandToRun, (char *) 0);
    kill(pid, SIGTERM);
    exit(0);
  }

  //parent process
  else {
    //wait for any child process to terminate
    waitpid(-1, &status, 0);
    //stop timing
    end_clock();
  }

}


/**
 * Helper function to help grab the user input from stdin
 */
void getText(char *variable, int size){
    fgets(variable, sizeof(char) * size, stdin);
    sscanf(variable, "%[^\n]", variable);
}


int main(int argc, char*argv[]){

  struct rlimit r1;
  char * commandlineToken;
  //units of seconds = this equates to 10 mins
  r1.rlim_cur = 10 * 60;
  //set th user limit to 10 mins
  setrlimit(RLIMIT_CPU, &r1); 
  pid_t parent = getpid();
  pid_t pid = fork();

  //error forking
  if (pid == -1){
    fprintf( stderr, "failed to fork()\n");
  } 

  //child process
  else if(pid == 0) {
    //run a1monitor
    execve("a1monitor.o", argv, NULL);
    _exit(EXIT_FAILURE);   // exec never returns
  }

  //parent process
  else{
    while(1){
      printf("alshell: ");
      char userInput[80];
      char findCommand[80];
     
      //Get the user input from stdin
		  getText(userInput, 80);
      //Create a copy of the userInput 
      strcpy(findCommand, userInput);
      commandlineToken = strtok (findCommand," ");

      if(strcmp(commandlineToken, "pwd") == 0){
    	 our_pwd();
      }
      else if(strcmp(commandlineToken, "done") == 0){
    	 kill(pid, SIGKILL);
    	 our_done(parent);
      }
      else if(strcmp(commandlineToken, "cd") == 0){
        //now contains the pathname to change the directory to
        commandlineToken = strtok (NULL, " ");
        if(commandlineToken != NULL){
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