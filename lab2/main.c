//---------------------------------------
// Date Format Outputter (LAB2)
//
// Name: main.c
//
// Written by: Matthew Rowlandson - February 2017
//
// Usage: ./dateformat <args>
//       eg. ./dateformat -CftY 
//       eg. ./dateformat -C -f -t -Y
//
// Params:
//    -C : prints out the current time
//    -f : prints out the full time as day-of-week dd-mm-yyyy hh:mm:ss
//    -t : prints out the current time of day (hh:mm:ss) e.g. 15:32:02
//    -Y : prints out the current date out in YYYY-MM-DD format (e.g. 2016-02-02)
//
// Description: Outputs the date in the specified format to the screen
//
//----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// Prototypes
int hasArg(const char[], int*, char*[]);
void printCurrentDate();
void printYearMonthDay();
void printTimeOfDay();
void printFullTime();

//main
int main (int argc, char *argv[]) {
  int anything = FALSE;

  //use the ctime() function to print out the current time. 
  if(hasArg("-C", &argc, argv)){
    printCurrentDate();
    anything = TRUE;
  }
  //print the current date out in YYYY-MM-DD format (e.g. 2016-02-02) 
  if(hasArg("-Y", &argc, argv)){
    printYearMonthDay();
    anything = TRUE;
  }

  //print out the current time of day (hh:mm:ss) e.g. 15:32:02
  if(hasArg("-t", &argc, argv)){
    printTimeOfDay();
    anything = TRUE;
  }

  //print out the full time as day-of-week dd-mm-yyyy hh:mm:ss (without using ctime())
  if(hasArg("-f", &argc, argv)){
    printFullTime();
    anything = TRUE;
  }

  //If no arguments.
  if(anything == FALSE){
    printf("No arguments called. Valid arguments include: %s \n", "-CftY");
  }

  exit(0); //done
}

//Prints the current date time using ctime();
void printCurrentDate()   {
  time_t rawtime = time(0);

  printf("Current local time and date: %s", ctime(&rawtime));
}

//Prints the date in format: YYYY-MM-DD
void printYearMonthDay(){
  time_t rawtime = time(0);
  struct tm *timeinfo = localtime(&rawtime);
  char buf[80];

  strftime(buf, sizeof(buf), "%Y-%m-%d", timeinfo);

  printf("YYYY-MM-DD Format: %s \n", buf);
}

//Prints the current time of the day: hh:mm:ss
void printTimeOfDay(){
  time_t rawtime = time(0);
  struct tm *timeinfo = localtime(&rawtime);
  char buf[80];

  strftime(buf, sizeof(buf), "%X", timeinfo);

  printf("Time of Day: %s \n", buf);
}

//prints out the full time as day-of-week dd-mm-yyyy hh:mm:ss
void printFullTime() {
  time_t rawtime = time(0);
  struct tm *timeinfo = localtime(&rawtime);
  char buf[80];

  strftime(buf, sizeof(buf), "%A %d-%m-%Y %X", timeinfo);

  printf("Full time: %s \n", buf);
}

// Check if the specific argument was given as an argument
int hasArg(const char arg[], int *argc, char* argv[]) {
  int paramCounter = 1;
  while(paramCounter < *argc){
    //Check as an individual params: eg. -t -Y -t -f
    if(strcmp(argv[paramCounter], arg) == 0) {
      return TRUE;
    } else {
      //Check for one string with multiple params eg -CYftz
      if(argv[paramCounter][0] == '-') {
        int innerCount = 1;
        int strLength = strlen(argv[paramCounter]);
        while(innerCount < strLength) {
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

