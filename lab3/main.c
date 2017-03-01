/*======================================================================
|
| Name: lsdir -> CFILE: main.c
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
| -s - SINCE: optional arg that specifies the age of the oldest files to display. [An integer that refers the the number of days old] eg. 400
| -b - BEFORE: optional arg that specifies the age of the newest files. [An interger that refers to the number of days old] eg. 500
|
| Subroutines/libraries required: See include statements below.
|
| Eg output:
| ./lsdir -b 3650 -s 8000 /home/common/lab3
|
| Permissions      Inode Type UID  GID  ByteSize  Filename                           Modification Date
| -rw-r--r--    1  40991 REG  1149 1149 213904    visual_Studio_Community.exe        Fri Dec  9 10:17:00 2016
| -rw-r--r--    1  40963 REG  1149 1149 1179      C128.XTK                           Tue Jan  1 00:01:28 1980
| -rw-r--r--    1  40966 REG  1149 1149 408606    poke1.jpg                          Thu Nov  1 17:03:34 2001
| ...
| ...
|------------------------------------------------------------------*/

//# Libs
#include <dirent.h> /* Structures for directory entries */
#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Prototypes of commonly used library functions */
#include <string.h> /* Commonly used string-handling functions */
#include <sys/stat.h> /* Functions to access detailed information for files*/
#include <getopt.h> /* getopt */
#include <time.h> /* Time library */

//Allow use of boolean types :)
typedef enum { false = 0, true = !false } bool;

//# Global vars
//Struct that will hold the arguments for use by the program once grabArgs is called.
struct Arguments {
  bool b; //Was a b flag given?
  bool s; //Was a s flag given?
  int bVal; //integer that accompanies b flag. Represents the age of the oldest files from current date (# of days from date).
  int sVal; //integer that accompanies s flag. Represences the age of the youngest files from current date (# of days from date).
  char* directoryPath; //The directory path given by the user.
};  

//# Prototypes (see functions for individual explainations on functionality)
void grabArgs( int argc, char *argv[], struct Arguments *args ); 
void displayStatInfo(struct stat fileStat, char* filename);
char* getFiletype(struct stat fileStat);
char* getFilePermissions(struct stat fileStat);
char* formatDate(char* str, time_t val);

//# Functions
//----- MAIN -----
// DESC: main function of the program
// PARAMS: argc : count of # of arguments given on the commandline
//         argv : array of the given arguments from the commandline
//--------------
int main( int argc, char *argv[] ) {
  struct Arguments args; //Create the struct to hold our arguments                              
  grabArgs(argc, argv, &args); //Grab the arguments that were given on the command line. Passes in our struct `args` to be populated.

  //CHECK FOR directoryPath
  if(args.directoryPath == NULL || args.directoryPath == ""){
    printf("ERROR: No directory path given.");
    return;
  }

  //#Setup arg values to be used
  // Check that the directory path has a `/` at the end. If not. Add it.
  if(args.directoryPath[strlen(args.directoryPath)-1] != '/'){
    //No slash at end of path. Add it in for our use with files.
    strcat(args.directoryPath, "/");
  }
  int beforeDays = args.bVal;
  int sinceDays = args.sVal;

  //Time to open the directory we were given
  DIR *dirp; // pointer to a directory type                                             
  struct dirent *dp; // structure of a directory entry
  dirp = opendir(args.directoryPath); // open the directory

  //If no directory. Stop.
  if (dirp == NULL) {                                    
   printf("Could not find directory: '%s'\n", args.directoryPath); // oops open failed... bail !
   return;
  }

  //Setup the header of table
  printf("%-14s%-3s%-6s%-5s%-5s%-5s%-10s%-35s%-10s\n", "Permissions", "   ", "Inode", "Type", "UID", "GID", "ByteSize", "Filename", "Modification Date");

  //Time to read each file.
  struct stat file_status; //stat variable that will house each individual file's information as we loop
  while ((dp = readdir(dirp)) != NULL) { //While we read the directory and haven't reached the end                                 
    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue; //Skip . and .. from our listing.

    //Setup the file's full path
    char file[PATH_MAX+1];
    strcpy(file, args.directoryPath); //Add in directory
    strcat(file, dp->d_name); //Add in filename

    //Read the files information
    if(stat(file, &file_status) == 0){ //If the file's information has been gathered correctly 0=good, 1=error
        //If we were given the -b or -s flags. Do checks on the current file to see if we should display it or not.
        if(args.b || args.s) {
          //beforeDays = age of oldest files to display BEFORE.
          if(beforeDays) {
            //Display files from BEFORE beforeDays ago.
            time_t currentTime = time(0); //Current time
            double diff = difftime(currentTime, file_status.st_mtime); //Difference between current time and the file's modified time.
            int days = diff/24/3600; //Convert seconds to days
            if(days < beforeDays) {
              //DONT SHOW. TOO OLD of a file
              continue; //Go on to next file. Skip current one.
            }
          }
          //sinceDays = age of newest files to display SINCE.
          if(sinceDays) {
            //Display files from AFTER sinceDays ago.
            time_t currentTime = time(0);
            //Are the number of seconds greater then number of days to seconds?
            double diff = difftime(currentTime, file_status.st_mtime); //Difference between current time and the file's modified time
            int days = diff/24/3600; //Convert seconds to days
            if(days > sinceDays){
              //DONT SHOW. TOO NEW of a file
              continue; //Skip this file. Move along to next one
            }
          }
        }

        //IF the file hasn't been skipped display the information for the file
        displayStatInfo(file_status, dp->d_name);
    } else {
      //We couldn't load the file's information. Print an error and move along to the next file
      printf("INVALID FILE PATH: %s \n", file);
    }
  }

  closedir(dirp); // play nice and close the directory after all files have been read.

  return 0; //End of program
}

//----- grabArgs -----
// DESC: parses the arguments from the command line using get opt. Then feeds those arguments into the args struct.
// PARAMS: argc : count of # of arguments given on the commandline
//         argv : array of the given arguments from the commandline
//         args : struct of type Arguments (see global var). Holds the arguments given by the user in order to be easily accessible by the caller.
//--------------------
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
        case '?':
          printf("You gave an unrecognized parameter: -%c. \n", optopt);
          break;
        default:
          printf("Something went wrong... \n");
          break;
      }
    }
    else {
        //After we have reached the end of the optional arguments.
        //Grab the directory path from the last argument
        args->directoryPath = argv[optind];
        optind++; //End the while loop
    }
  }
}

//----- displayStatInfo -----
// DESC: Displays the information given from the stat struct in a formatted table row, (lines up with header row in main)
// PARAMS: fileStat : struct stat, holds the file information loaded in from a stat function call.
//         filename : The name of the file to display
//---------------------------
void displayStatInfo(struct stat fileStat, char* filename){
  char* filetype = getFiletype(fileStat);
  char* permissions = getFilePermissions(fileStat);
  printf("%-14s", permissions); //File permissions
  printf("%-3d", (int) fileStat.st_nlink); //print the file's hard link count
  printf("%-6ld", (long) fileStat.st_ino); // print the file's inode info
  printf("%-5s", filetype); //Prints the file's filetype
  printf("%-5ld", (long) fileStat.st_uid); //print the files owner user id
  printf("%-5ld", (long) fileStat.st_gid); //print the files owner group id
  printf("%-10lld", (long long) fileStat.st_size); // print the file size
  printf("%-35s", filename); //Name of the file
  printf("%-10s", ctime(&fileStat.st_mtime)); // last modified time
}

//----- getFiletype -----
// DESC: Parses out and returns the filetype of the given stat reference via short form for display
// PARAMS: fileStat : struct stat, holds the file information loaded in from a stat function call.
//-----------------------
char* getFiletype(struct stat fileStat){
  char* filetype;

  switch (fileStat.st_mode & S_IFMT) // parse out the file type
  {
    case S_IFREG:
      filetype = "REG";
    break;
    case S_IFDIR:
      filetype = "DIR";
      break;
    case S_IFCHR:
      filetype = "C_DEV";
    break;
    case S_IFBLK:
      filetype = "B_DEV";
    break;
    case S_IFLNK:
      filetype = "LINK";
    break;
    case S_IFIFO:
      filetype = "FIFO";
    break;
    case S_IFSOCK:
      filetype = "SOCK";
    break;
    default:
      filetype = "UNKN";
    break;
  }

  return filetype;
}

//----- getFilePermissions -----
// DESC: Parses out and returns the permissions of the given stat reference.
// PARAMS: fileStat : struct stat, holds the file information loaded in from a stat function call.
// REFERENCE: (for the different variable types) http://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html
// RETURNS: char array (STRING)
//------------------------------
char* getFilePermissions(struct stat fileStat){
  char permissions[PATH_MAX+1]; //Allocate mem

  //Type of file. Is it a link, or directory?
  if(S_ISLNK(fileStat.st_mode)){
    //Link
    strcpy(permissions, S_ISLNK(fileStat.st_mode) ? "l" : "-");
  } else {
    //Directory
    strcpy(permissions, S_ISDIR(fileStat.st_mode) ? "d" : "-");
  }

  //USER PERMS
  strcat(permissions, (fileStat.st_mode & S_IRUSR) ? "r" : "-");
  strcat(permissions, (fileStat.st_mode & S_IWUSR) ? "w" : "-");
  strcat(permissions, (fileStat.st_mode & S_IXUSR) ? "x" : "-");
  //GROUP PERMS
  strcat(permissions, (fileStat.st_mode & S_IRGRP) ? "r" : "-");
  strcat(permissions, (fileStat.st_mode & S_IWGRP) ? "w" : "-");
  strcat(permissions, (fileStat.st_mode & S_IXGRP) ? "x" : "-");
  //WORLD PERMS
  strcat(permissions, (fileStat.st_mode & S_IROTH) ? "r" : "-");
  strcat(permissions, (fileStat.st_mode & S_IWOTH) ? "w" : "-");
  strcat(permissions, (fileStat.st_mode & S_IXOTH) ? "x" : "-");

  return permissions;
}

//----- formatDate -----
// DESC: formats the given time_t date into a readable format
// PARAMS: str : The string to put the formatted date into
//         val : The time_t object holding the date information
// RETURNS: char array (STRING) [The formatted date]
//------------------------------
char* formatDate(char* str, time_t val){
  strftime(str, 40, "%d.%m.%Y %H:%M:%S", localtime(&val));
  return str;
}