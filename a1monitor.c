/**
 * Shally Banh
 * 1424763
 * Cmput379 Assign 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/**
 * Open the "/proc/loadavg" file
 * and places the content of the file to loadAvgAndProcesses 
 *
 * @param loadAvgAndProcesses
 */
void getLoadAverageAndProcesses(char (*loadAvgAndProcesses)[]){
  FILE *filePtr;
  filePtr = fopen("/proc/loadavg", "r");
  if (filePtr == NULL){
    printf("Error opening \"/proc/loadavg\" : Error occured is : %s\n", strerror(errno) );
    // Program exits if file pointer returns NULL.
    exit(EXIT_FAILURE);         
  }

  // reads text until newline 
  fscanf(filePtr,"%[^\n]", *loadAvgAndProcesses);
  
  fclose(filePtr);

}

/**
 * Reads the contentOfLoadAvgFile and prints out the load averages and total processes running
 *
 * @param contentOfLoadAvgFile
 */
void printOutLoadAverageAndProcesses(char (*contentOfLoadAvgFile)[]){
  int count = 0;
  char * token;
  printf ("Load Average: ");
  token = strtok (*contentOfLoadAvgFile," ");
  while (token != NULL && count < 3)
  {
    count += 1;
    //last one don't need comma
    if(count == 3){
      printf ("%s",token);
    }
    else{
    printf ("%s, ",token);
  }
    token = strtok (NULL, " ");
  }
  printf("\nProcesses: %s\n", token);

}

/**
 * Grabs the specified interval from the commandline and prints out 
 * the date, time, load avg, and number of processes running
 */
int main(int argc, char*argv[]){
  //run in a loop
	while(1){
    //grabs our time info
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    //grab content from the "/proc/loadavg" file
    char contentOfLoadAvgFile[1000];
    getLoadAverageAndProcesses(&contentOfLoadAvgFile);

    printf ( "almonitor: \n%s", asctime (timeinfo) );
    printOutLoadAverageAndProcesses(&contentOfLoadAvgFile);

    //sleep the process based in the interval entered by the user
    unsigned int intervalToSleep = atoi(argv[1]);
    sleep(intervalToSleep);
  }

  return 0;
}