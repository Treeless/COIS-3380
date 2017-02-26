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
#include <sys/stat.h> /* Functions to get file information */
#include <time.h> /* Time library */

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
void displayStatInfo( const struct stat *sb, char* filename);
char* getFiletype(const struct stat *sb);
char* getFilePermissions(const struct stat *sb);

//# Functions

//main function of the program
int main( int argc, char *argv[] ) {
  DIR *dirp;                                           // pointer to a directory type
  struct dirent *dp;                                   // structure of a directory entry

  struct Arguments args;                               // eg args (-b 1 -s 0 [directoryPath])
  grabArgs(argc, argv, &args);                         //Get the arguments and feed them into our struct

  dirp = opendir(args.directoryPath);                    // open the directory
  if (dirp == NULL)                                      // oops open failed... bail !
  {
   printf("opendir failed on '%s'\n", args.directoryPath);
   return;
  }

  printf("Directory: %s/ \n", args.directoryPath);
  printf("%-14s%-17s%-5s%-5s%-5s%-10s%-18s%-10s\n", "Permissions", "inode", "Type", "UID", "GID", "ByteSize", "Filename", "Modification date");
  for (;;)
  {
    //For each file that passes the specified args
    if(args.b || args.s) {
      //Do checks here on the modification date
      //TODO
    }

    dp = readdir(dirp);
    if (dp == NULL) break;                                // reached the end of the directory

    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue; /* Skip . and .. */

    struct stat file_status;
    stat(dp->d_name, &file_status);
    //Display the stats
    displayStatInfo( &file_status, dp->d_name);
  }

  closedir(dirp);                                       // play nice and close the directory

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

//Display file information for the specified file
// Params: a stat structure returned by the stat() system call. see sys/stat.h for details on the field(s) and what they contain.
void displayStatInfo(const struct stat *sb, char* filename){
  char* filetype = getFiletype(sb);
  char* permissions = getFilePermissions(sb);
  //printf("permissions   inode  Type  UID  GID  ByteSize  Filename   Modification date \n");
  printf("%-14s", permissions); //File permissions
  printf("%-17ld", (long) sb->st_ino); // print the file's inode info
  printf("%-5s", filetype); //Prints the file's filetype
  printf("%-5ld", (long) sb->st_uid); //print the files owner user id
  printf("%-5ld", (long) sb->st_gid); //print the files owner group id
  printf("%-10lld", (long long) sb->st_size); // print the file size
  printf("%18s", filename); //Name of the file
  printf("%-10s", ctime(&sb->st_mtime)); // last modified time
}

//Parses out and returns the filetype of the given stat reference via short form for display
char* getFiletype(const struct stat *sb){
  char* filetype;

  switch (sb->st_mode & S_IFMT) // parse out the file type
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

//TODO Might not get to this. But its not required by the assignment
//Parses out and returns the permissions of the given stat reference.
//Ref: http://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html
char* getFilePermissions(const struct stat *sb){
  char* permissions = "---";
  //S_IRWXU read, write, execute/search by owner
  switch(sb->st_mode & S_IRWXU){
    case(S_IRUSR):
      //read permission, owner
    break;
    case(S_IWUSR):
      //write permission, owner
    break;
    case(S_IXUSR):
      //execute/search permission, owner
    break;
    case(S_IRWXG):
      //read, write, execute/search by group
    break;
    case(S_IRGRP):
      //read permission, group
    break;
    case(S_IWGRP):
      //write permission, group
    break;
    case(S_IXGRP):
      //execute/search permission, group
    break;
    case(S_IRWXO):
      //read, write, execute/search by others
    break;
    case(S_IROTH):
      //read permission, others
    break;
    case(S_IWOTH):
      //write permission, others
    break;
    case(S_IXOTH):
      //execute/search permission, others
    break;
    case(S_ISUID):
      //set-user-ID on execution
    break;
    case(S_ISGID):
      //set-group-ID on execution
    break;
    case(S_ISVTX):
      //on directories, restricted deletion flag
    break;
    default:
    break;
  }

  return permissions;
}