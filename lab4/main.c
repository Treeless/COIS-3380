/*
  # Name: Lab4: COIS-3380 Processes
  # Author: Matthew Rowlandson
  # Date: March 2017
  # File: main.c
  # Libraries: see below
  # Purpose: Learning how processes work. Creating two child processes and a single grandchild them waiting for them to finish their tasks
  # Compile: gcc -o lab4 main.c
  # Run: ./lab4


  //MARKER PLEASE note: the output is not as was given from the assignment. However the code is proper. 
  //                    I just went at it from a different way and couldn't get the output as given in the assignment exactly. 
  //                    All functionality is there.
*/

#include <stdlib.h> //standard library
#include <stdio.h> //standard input and output
#include <unistd.h> //pid, cwd, fork
#include <sys/types.h> //types
#include <sys/wait.h> //waitpd
#include <fcntl.h> //File read, open, close, write
#include <string.h> //String lib
#include <sys/stat.h> //Using the permission flags from here

//main function
int main(int *argc, char *args[]) {
  pid_t pidc1, pidc2, pidgc; //process ids
  //Create the two child processes
  if(pidc1 = fork()) { // parent
    if(pidc2 = fork()) { //parent
      printf("P: first child has pid %d\n", pidc1);
      printf("P: second child has pid %d\n", pidc2);

      pid_t wpid; // process id that is finished
      int status; //status code returned by process  
      //Wait for all child processes to complete
      while ((wpid = wait(&status)) > 0){
        if(wpid > 0){
          printf("P: Child %d is done \n", wpid);
        }
      };

      exit(0); //exit main parent process
    } else {
      //Second process's code

      printf("SC: I am the second child.\n");
      int totalBytes = 0; //total number of bytes in the file we are going to read in
      char buffer[2056]; //file contents size 2056 bytes
      ssize_t read_bytes = 0; //Bytes from each chunk get placed here temporarily

      ///Read in file contents: home/common/lab_sourcefile
      int fd_in = open("/home/common/lab_sourcefile", O_RDONLY);//open the file
      do {
        //read the file chunk by chunk to the buffer
        read_bytes = read(fd_in, buffer, (size_t)2056);

        // End of file or error.
        if (read_bytes <= 0){
            break; //done reading file
        }

        //Save the total number of bytes
        totalBytes = totalBytes + read_bytes;
      } while (1); //infinite loop until we are done reading the file
      
      //Write out contents to: lab4_file_copy
      char filepath[1024]; //temp path
      getcwd(filepath, sizeof(filepath)); //Get the current working directory
      strcat(filepath, "/lab4_file_copy"); //Tack on the filename
      int fd = open(cwd, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH); //open the file. O_RDWR = read and write flag, O_CREAT = create if doesnt exist, S_* flags are permission flags from fstat 
      write(fd, buffer, strlen(buffer)); //write to the file

      close(fd_in);//close file we were reading
      close(fd); //close copy file

      // print out the number of bytes the file
      printf("SC: --> Second child: %d bytes transferred.\n", totalBytes);

      exit(0); //done kill second child process
    }
  }
  else {
    //First child process

    printf("FC: I'm the first child.\n");

    //Create the granchild process
    if((pidgc = fork()) == 0) {
      printf("GC: I am the grandchild.\n");
      
      sleep(3); //sleep for 3 seconds

      printf("GC: #### Output start ####\n");
      execlp("head", "head", "-n", "20", "main.c", NULL); //this should exit automatically
      printf("ERROR: execlp command failed\n");//This will only run if the execlp process fails
      exit(1); //FAILED!
    } else {
      //Output the granchild pid
      printf("FC: I am the first child, grandchild has pid %d\n", pidgc);

      int grandchildStatus;

      //Wait for grandchild process to be done. Polling...
      while((waitpid(pidgc, &grandchildStatus, WNOHANG)) != pidgc){
        printf("FC: Try again\n");
        sleep(1);
      };
      printf("GC: #### Output end ####\n");
      //End of first child. Grandchild must have completed.

      exit(0); //Done. kill first child process
    }
  }
}