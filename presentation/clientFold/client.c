#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFSIZE 10000

int main(int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server_address;
  char buffer[BUFFSIZE];
  char shellDir[BUFFSIZE];
  int portNumber;
  char serverIP[29];
  int rc = 0;

  //checks number of command args
  if (argc < 3){
    printf ("usage is client <ipaddr> <port number> \n");
    exit(1);
  }

  // creates socket connection
  sd = socket(AF_INET, SOCK_STREAM, 0);
  // socket varification
  if (sd == -1) {
      printf("Failed to create socket conection...\n");
      exit(1);
  }
  else
      printf("Successfully created socket connection..\n");
  portNumber = strtol(argv[2], NULL, 10);
  strcpy(serverIP, argv[1]);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portNumber);
  server_address.sin_addr.s_addr = inet_addr(serverIP);

  // checks connetion between client and server
  if (connect (sd, (struct sockaddr *) &server_address, sizeof (struct sockaddr_in ))<0){
    close (sd);
    perror ("error connecting stream socket ");
    exit (1);
  } else printf("Successfully Connected..\n");

  int n;
  for (;;) {
      //clears memory buffer
      memset(buffer, 0, BUFFSIZE);
      //memset(shellDir,0,80);
      //reads input sent from server about working dir
      read(sd, buffer, sizeof(buffer));
      int dirLen = strlen(buffer);
      char dirStr[200];
      strncpy(dirStr, buffer, dirLen-1);
      //emulates shell
      printf("[~%s]$ ", dirStr);
      n = 0;
      //gets command string from user
      while ((buffer[n++] = getchar()) != '\n');
      //checks to see if user wants to exit shell
      if ((strncmp(buffer, "exit", 4)) == 0) {
          printf("Client Sneakily Exiting...\n");
          break;
      }
      //sends command to sever
      write(sd, buffer, sizeof(buffer));
      //clears buffer to use for recieving
      memset(buffer, 0, BUFFSIZE);
      //reads input sent from server about command
      read(sd, buffer, sizeof(buffer));
      //test print statements
      //sprintf(shellDir, "nunderwood@nunderwood-VirtualBox~/Downloads/sshell");
      //sprintf(buffer, "/home/nunderwood/Downloads/sshell");
      //prints message from server
      printf("\n%s\n", buffer);
  }
  return 0;
}