#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// Prototypes
int hasArg(const char[], int*, char*[]);

//main
int main (int argc, char *argv[]) {
  printf("STARTING...\n");
  printf("Has Arg C: %d \n", hasArg("-C", &argc, argv)); //use the ctime() function to print out the current time. 
  printf("Has Arg Y: %d \n", hasArg("-Y", &argc, argv)); //print the current date out in YYYY-MM-DD format (e.g. 2016-02-02) 
  printf("Has Arg t: %d \n", hasArg("-t", &argc, argv)); //print out the current time of day (hh:mm:ss) e.g. 15:32:02
  printf("Has Arg f: %d \n", hasArg("-f", &argc, argv)); //print out the full time as day-of-week dd-mm-yyyy hh:mm:ss (without using ctime()) 
  exit(0);

  // time_t systemTime;
  // struct tm *timeinfo; //tm_sec, tm_min, tm_day, tm_mon etc
  // char buffer [80];

  // time(&systemTime);

  // timeinfo = localtime(&systemTime);

  // strftime(buffer, 80, "Now it's %y/%m/%d.", timeinfo);
  // puts(buffer);


  exit(0);
}

// Check if the specific argument was given
int hasArg(const char arg[], int *argc, char* argv[]){
  int paramCounter = 1;
  while(paramCounter < *argc){
    //Check as an individual params: eg. -t -Y -t -f
    if(strcmp(argv[paramCounter], arg) == 0) {
      return TRUE;
    } else {
      //Check for one string with multiple params
      if(argv[paramCounter][0] == '-') {
        int innerCount = 1;
        int strLength = strlen(argv[paramCounter]);
        while(innerCount < strLength){
          char* actualArg = (char[3]){'-', argv[paramCounter][innerCount], '\0'};
          if(strcmp(actualArg, arg) == 0) { 
            return TRUE;
          }
          innerCount++;
        }
      }
    }

    paramCounter++;
  }
  return FALSE;
}

