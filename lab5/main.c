/*
  Name: Lab5: Signal Handling
  Author: Matthew Rowlandson
  Date: March 19th 2017
  Purpose: Required to write a C program that contains three signal handler functions
*/

//Libraries
#include <signal.h> //The signal library
#include <stdlib.h> //standard library
#include <stdio.h> //printf Standard InputOutput Library
#include <unistd.h> //pid, cwd, fork

//Global Vars
static int const MAX_COUNT = 5; //Max number of sigints to recieve before removing the sigint handler
int sigintCounter = 0; //Used to show how many sigint signals recieved
pid_t child_pid; //child process id

//Prototypes
void sigintHandler(int sigint);
void sigquitHandler();
void siguserHandler();

//Main
//Listen for the three different types of sigints (SIGINT, SIGQUIT, SIGUSR1)
int main(int argc, char argv) {
  if(!child_pid) {
    //Parent functionality

    //Handlers
    signal(SIGINT, sigintHandler);//ctrl-c
    signal(SIGQUIT, sigquitHandler); //ctrl-slash
    signal(SIGUSR1, siguserHandler); //user defined

    //Forever loop while we wait for signals
    do {
      printf("Wait for another signal ... \n");
      pause(); //wait
    } while(1);
  }
}

//SIGINT HANDLER (ctrl-c)
// Param: sigint the number emitted (1-3) : not used in this
void sigintHandler(int sigint){
  sigintCounter++;//increment the counter

  //Check if we reached the total number of sigint counts
  if(sigintCounter >= MAX_COUNT){
    //Done
    printf("SIGINT: MAX has been exceeded\n");
    signal(SIGINT, SIG_DFL); //use the default sigint handler
  } else {
    printf("This is the %d time you pressed cntl-c\n", sigintCounter);
    signal(SIGINT, sigintHandler); //readd the custom handler
  }
}

//SIGQUIT HANDLER (ctrl-\)
void sigquitHandler(){
  //Fork the process and create a child process
  if(!(child_pid = fork())){
    // Child: Print message
    printf("I am the child(%d) and I am sending a signal to parent(%d)\n", getpid(), getppid());
    // Child: KILL() to parent
    kill(getppid(), SIGUSR1); //send signal
    exit(0); //child process exit
  } else {
    //Parent message
    printf("Parent(%d): I have a child %d\n", getpid(), child_pid);
  }
}

//SIGUSR1 (sent by child process)
void siguserHandler(){
  printf("Parent(%d): Child sent signal. We are finished\n", getpid());
  exit(0); //Exit parent process
}