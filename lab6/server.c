/*
    Author: Matthew Rowlandson
    Date: 2017-04-04
    Purpose: Server side code for recieving a file name, and passing the file data over to connected clients via a socket.
    Compiler: gcc o -s server.c
    Running: ./s& //Runs as a process, independent from the console
*/

#include <stdio.h> //standard io lib
#include <sys/types.h> //types lib 
#include <sys/socket.h> //socket lib
#include <stdlib.h> // standard lib
#include <errno.h> //error number library
#include <fcntl.h> // file control library
#include <sys/sendfile.h> // send file library
#include <string.h> //string lib
#include <arpa/inet.h> // ipaddress library
#include <unistd.h> // POSIX operating system API
#include <netinet/in.h> //another network library
#include <sys/stat.h> // file stat library (for accessing file information)

#define SERVER_ADDRESS      "127.0.0.1" //localhost (where this server's IP is)
#define PORT_NUM             25328 //Our unique port
#define BLOCK_SIZE           512 //number of bytes per iteration

//Prototypes
int error(char* where, char* msg);

int main(int argc, char *argv[]) {
  //Main vars
  //  Socket vars
  int serverSocket; //the server socket
  int peerSocket; // the peer socket (currently connected client)
  socklen_t socketLength; //number of connections
  struct sockaddr_in serverAddress; //server socket address
  struct sockaddr_in peerAddress; //connected client socket address

  //program vars
  ssize_t len; //length of the packet data being sent to the connected client
  int fd; //File descriptor
  int sentBytes = 0; //number of bytes being sent
  char fileSize[256]; //Size of the file
  struct stat fileStat; //file information
  off_t offset; //file offset
  int remainingData; //data left to send to client

  //Create the server socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    error("Socket Creation", "Error creating socket");
  }

  //Setup the server address
  memset(&serverAddress, 0, sizeof(serverAddress)); //Set the memory for the server address
  serverAddress.sin_family = AF_INET; //type of address
  inet_pton(AF_INET, SERVER_ADDRESS, &(serverAddress.sin_addr)); // Setup the ipaddress
  serverAddress.sin_port = htons(PORT_NUM); //Add in the port number

  //Bind the server socket to the server address
  if ((bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr))) == -1) {
    error("Socket Bind", "Error on bind");
  }

  //Listen on the server socket for up to 5 incoming connections
  if ((listen(serverSocket, 5)) == -1) {
    error("Socket Listen", "Error on listen");
  }

  //Loop
  while(1) {
    socketLength = sizeof(struct sockaddr_in);
    //Accept a client connection
    peerSocket = accept(serverSocket, (struct sockaddr *)&peerAddress, &socketLength);
    if (peerSocket == -1) {
      error("Socket Accept", "Error on accept");
    }
    fprintf(stdout, "Server: Accepted connection --> %s\n", inet_ntoa(peerAddress.sin_addr));

    //Start of filepath recieve
    char filepathBuffer[BUFSIZ];
    int filePathlength = recv(peerSocket, filepathBuffer, BUFSIZ, 0); //Recieve the filepath from the client
    if(filePathlength < 0){
      error("Missing Filepath", "No file path recieved from client...");
    } else {
      fprintf(stdout, "Server: File to retrieve for client: %s \n", filepathBuffer);
    }
    //Done recieving file path

    //Open the requested file
    fd = open(filepathBuffer, O_RDONLY);
    if (fd == -1) {
      error("File Open", "Error opening file");
    }

    //Get the file size of the requested file
    if (fstat(fd, &fileStat) < 0) {
      error("File Stat", "Error on filestat");
    }

    fprintf(stdout, "Server: File Size: %d bytes\n", fileStat.st_size);
    sprintf(fileSize, "%d", fileStat.st_size); //copy over the filesize value

    //Sending file size to client to give them the full size to expect
    len = send(peerSocket, fileSize, sizeof(fileSize), 0);
    if (len < 0) {
      error("File Send", "Error on sending file");
    }

    offset = 0; //fileoffset
    remainingData = fileStat.st_size; //remaining file data to send (size of file)
    //Send the file data in packets of BUFSIZE
    int dataSizeToSend = BLOCK_SIZE; // 256
    while (remainingData > 0) {
      sentBytes = sendfile(peerSocket, fd, &offset, dataSizeToSend);

      fprintf(stdout, "Server: sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sentBytes, offset, remainingData);
      remainingData -= sentBytes;
      //If the size of the data block is too large. Make it smaller for the last bit of data
      if(remainingData < dataSizeToSend) {
        printf("New data size to send: %d \n", dataSizeToSend);
        dataSizeToSend = remainingData;
      }
    }

    printf("Server: Done sending data... \n");
  }
}

//Our error function for formatting our errors to be consistant accross the board. (copied from client script)
int error(char* where, char* msg){
  printf("Server Error: (%s) => %s \n", where, msg);
  exit(EXIT_FAILURE); //Exit with a failure message [EXIT_FAILURE is similar to using 1]
}