/*
    Author: Matthew Rowlandson
    Date: 2017-04-04
    Purpose: Client side code for connecting to a serverside socket and recieving file information and then writing to a file
    Compiler: gcc o -c client.c
    Running: ./c
*/
#include <stdlib.h> //std lib
#include <stdio.h> //Std input output 
#include <sys/types.h> // types
#include <sys/socket.h> //Socket library
#include <arpa/inet.h> //network lib
#include <unistd.h> // File lib
#include <netinet/in.h> //another network lib
#include <errno.h> //EXIT_FAILURE const
#include <string.h> //string lib

//Allow use of boolean types :)
typedef enum { false = 0, true = !false } bool;

#define SERVER_ADDRESS      "127.0.0.1" //localhost (where the server socket to connect to is)
#define PORT_NUM             25328 //Our unique port
#define OUTPUT_FILENAME      "outputfile"

//Struct for our arguments
struct Arguments {
  bool f; //Was a file path given?
  char* filepath;
}; 

//Prototypes
void grabArgs( int argc, char *argv[], struct Arguments *args ); 
int error(char* where, char* msg);

//Main
int main(int argc, char *argv[]) {
  //PROGRAM MAIN VARS
  int clientSocket; //client side socket 
  struct sockaddr_in remote_addr; //struct type of sockaddr_in
  char buffer[BUFSIZ]; //Create our buffer (with size of built in constant BUFSIZ from stdio.h. Usually 4096? But depends apparently)
  int remainingData = 0; //The data left to write
  ssize_t len; //length of the data we just recieved


  struct Arguments args; //Create the struct to hold our arguments {f: bool, filepath: char* }                             
  grabArgs(argc, argv, &args); //Grab the arguments that were given on the command line. Passes in our struct `args` to be populated.

  //Check that we got the proper val
  if(args.f == false){
    error("grabArgs", "Missing -f filename");
    return -1;
  }

  //zero the memory of the struct
  memset(&remote_addr, 0, sizeof(remote_addr));

  //Setup the address to connect to
  remote_addr.sin_family = AF_INET; //type of address
  inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr)); //setup the adrress
  remote_addr.sin_port = htons(PORT_NUM); //set the port

  //Create out client side socket (to recieve the data)
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    error("ClientSocket create", "Error creating socket...");
  }

  //Connect the client side socket to the server side socket
  if (connect(clientSocket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1) {
    error("Socket Connect", "Error connecting to server...");
  }

  //Send message to server asking for the file
  send(clientSocket, args.filepath, sizeof(args.filepath), 0);

  //Recieve the file information (size)
  recv(clientSocket, buffer, BUFSIZ, 0);

  //Open the file to write the file output to
  FILE *receivedFile = fopen(OUTPUT_FILENAME, "w");
  if (receivedFile == NULL) {
    error("Open File", "Issue opening file to write to...");
  }

  remainingData = atoi(buffer); //Set total file size

  //Recieve each chunk of the file and while we still have data remaining. Write it to the file
  while (((len = recv(clientSocket, buffer, BUFSIZ, 0)) > 0) && (remainingData > 0)){
    fwrite(buffer, sizeof(char), len, receivedFile);
    remainingData -= len;
    fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remainingData);
  }
  fclose(receivedFile); //Close the file we wrote to
  close(clientSocket); //Close the socket. We are done

  return 0; //done
}

//Our error function for formatting our errors to be consistant accross the board.
int error(char* where, char* msg){
  printf("Error: (%s) => %s \n", where, msg);
  exit(EXIT_FAILURE); //Exit with a failure message [EXIT_FAILURE is similar to using 1]
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
    if((c = getopt(argc, argv, "f:")) != -1) {
      //Option arguments
      switch(c) {
        case 'f':
          args->f = true;
          args->filepath = optarg;
          break;
        case '?':
          printf("You gave an unrecognized parameter: -%c. \n", optopt);
          break;
        default:
          printf("Something went wrong... \n");
          break;
      }
    }
  }
}