/*======================================================================
|
| Name: main.c
|
| Written by: Matthew Rowlandson - February 2017
|
| Purpose: Similar to the `ls` command. This code lists the filenames contained in a target directory specified
| on the command line.
|
| Compile: gcc -o lsdir main.c
|
| usage: ./lsdir [directory path]
|
| Description of parameters:
| [directory path] - optional parameter that lists the directory to be processed. (otherwise cwd is used).
| -s - optional arg that specifies the age of the oldest files to display. [An integer that refers the the number of days old] eg. 3
| -b - optional arg that specifies the age of the newest files. [An interger that refers to the number of days old] eg. 1
|
| Subroutines/libraries required: See include statements below.
|
| Eg output:
| ./lsdir  -b 3650 -s 8000 /home/common/lab3
|
| inode  Type  UID  GID  ByteSize  Filename   Modification date
| 40963  REG  1149  1149   1179    C128.XTK   Tue Jan 1 00:01:28 1980
| ...
| ...
|
|------------------------------------------------------------------*/

//# Libs
#include <dirent.h> /* Structures for directory entries */
#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Prototypes of commonly used library functions */
#include <string.h> /* Commonly used string-handling functions */
#include <sys/stat.h> /* Functions to access detailed information for files*/
#include <getopt.h> /* getopt */

//Allow use of booleans :D
typedef enum { false = 0, true = !false } bool;

//# Global vars
struct Arguments {
  bool b;
  bool s;
  int bVal;
  int sVal;
  char* directoryPath;
};  

//# Prototypes
void grabArgs( int argc, char *argv[], struct Arguments *args );

//# Functions

//main function of the program
int main( int argc, char *argv[] ) {
  //Get arguments (-b 1 -s 0 [directoryPath]) using getOpt.
  //  Feed them back into our struct
  struct Arguments args;
  grabArgs(argc, argv, &args);

  //Debug
  printf("has b? %d val: %d \n", args.b, args.bVal);
  printf("has s? %d val: %d \n", args.s, args.sVal);
  printf("directoryPath: %s \n", args.directoryPath);
  //

  return 0;
}

//grabArgs, Grabs the arguments given and populates them in a struct args
void grabArgs(int argc, char *argv[], struct Arguments *args) {
  int c;
  extern char *optarg;
  while (optind < argc) {
    //Loop through the argments with get opt
    if((c = getopt(argc, argv, "s:b:")) != -1) {
      //Option arguments
      switch(c) {
        case 's':
            args->s = true;
            args->sVal = atoi(optarg);
            break;
        case 'b':
            args->b = true;
            args->bVal = atoi(optarg);
            break;
        default:
          break;
      }
    }
    else {
        //After we have reached the end of the optional arguments.
        //Grab the directory path from the last argument
        printf("DirectoryPath: %s\n", argv[optind]);
        args->directoryPath = argv[optind];
        optind++; //End the while loop
    }
  }
}