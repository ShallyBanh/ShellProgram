#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

/**
 * Grabs the specified interval from the commandline and prints out 
 * the date, time, load avg, and number of processes running
 */
int main(int argc, char*argv[]){
  //run in a loop
	while(1){
  char c[1000];
  FILE *fptr;
	time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  if ((fptr = fopen("/proc/loadavg", "r")) == NULL)
  {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);         
    }

  // reads text until newline 
  fscanf(fptr,"%[^\n]", c);
  fclose(fptr);

  printf ( "almonitor: \n%s", asctime (timeinfo) );
  printf ("Load Average: ");
  char str[1000];
  strcpy(str, c);
  int count = 0;
  char * pch;
  pch = strtok (str," ");
  while (pch != NULL && count < 3)
  {
  	count += 1;
  	//last one don't need comma
  	if(count == 3){
    	printf ("%s",pch);
    }
    else{
    printf ("%s,",pch);
  }
    pch = strtok (NULL, " ");
  }
  printf("\nProcesses: %s\n", pch);
  unsigned int i = atoi(argv[1]);
  sleep(i);
}

  return 0;
}