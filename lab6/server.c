#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define SERVER_ADDRESS      "127.0.0.1" //localhost (where this server's IP is)
#define PORT_NUM             25328 //Our unique port

//Prototypes
int error(char* where, char* msg);

int main(int argc, char *argv[]) {
  int server_socket;
  int peer_socket;
  socklen_t       sock_len;
  ssize_t len;
  struct sockaddr_in      server_addr;
  struct sockaddr_in      peer_addr;
  int fd;
  int sent_bytes = 0;
  char file_size[256];
  struct stat file_stat;
  off_t offset; //file offset
  int remain_data;

  /* Create server socket */
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    fprintf(stderr, "Error creating socket --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Zeroing server_addr struct */
  memset(&server_addr, 0, sizeof(server_addr));
  /* Construct server_addr struct */
  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_ADDRESS, &(server_addr.sin_addr));
  server_addr.sin_port = htons(PORT_NUM);

  /* Bind */
  if ((bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) == -1) {
    fprintf(stderr, "Error on bind --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Listening to incoming connections */
  if ((listen(server_socket, 5)) == -1) {
    fprintf(stderr, "Error on listen --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  sock_len = sizeof(struct sockaddr_in);
  /* Accepting incoming peers */
  peer_socket = accept(server_socket, (struct sockaddr *)&peer_addr, &sock_len);
  if (peer_socket == -1) {
    fprintf(stderr, "Error on accept --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "Accepted connection --> %s\n", inet_ntoa(peer_addr.sin_addr));

  //Start of filepath recieve
  char filepathBuffer[BUFSIZ];
  int filePathlength = recv(peer_socket, filepathBuffer, BUFSIZ, 0); //Recieve the filepath from the client
  if(filePathlength < 0){
    fprintf(stderr, "No file path recieved from client...");
    exit(EXIT_FAILURE);
  } else {
    fprintf(stdout, "File to retrieve for client: %s \n", filepathBuffer);
  }
  //Done recieving file path

  //Open the requested file
  fd = open(filepathBuffer, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Error opening file --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Get file stats */
  if (fstat(fd, &file_stat) < 0) {
    fprintf(stderr, "Error fstat --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "File Size: \n%d bytes\n", file_stat.st_size);

  sprintf(file_size, "%d", file_stat.st_size);

  /* Sending file size to client to give them the full size to expect */
  len = send(peer_socket, file_size, sizeof(file_size), 0);
  if (len < 0) {
    fprintf(stderr, "Error on sending file --> %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  offset = 0;
  remain_data = file_stat.st_size;
  /* Sending file data */
  while (((sent_bytes = sendfile(peer_socket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)) {
    fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
    remain_data -= sent_bytes;
    fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
  }

  printf("Server done sending data... \n");

  //Recieve the termination message from client
  char terminationMessage[4];
  int termRecieved = recv(peer_socket, terminationMessage, 4, 0); //Recieve the termination message from client
  if(termRecieved > 0){
    printf("Server: Recieved Termination message (%s) from client. Terminating server ... \n", terminationMessage);
    close(peer_socket);
    close(server_socket);
  }else {
    printf("Issue recieving termination message... (%d) \n", strcmp(terminationMessage, "TERM"));
  }

  return 0;
}

//Our error function for formatting our errors to be consistant accross the board. (copied from client script)
int error(char* where, char* msg){
  printf("Server Error: (%s) => %s \n", where, msg);
  exit(EXIT_FAILURE); //Exit with a failure message [EXIT_FAILURE is similar to using 1]
}