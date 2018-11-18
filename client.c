#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server_address;
  char buffer[100];
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
  //bzero(&servaddr, sizeof(servaddr));
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
  } else printf("Sneakily Connected..\n");

  int n;
  for (;;) {
      //clears memory buffer
      memset(buffer,0,100);
      //emulates shell
      printf("$ ");
      n = 0;
      //gets command string from user
      while ((buffer[n++] = getchar()) != '\n');
      //checks to see if user wants to exit shell
      if ((strncmp(buffer, "exit", 4)) == 0) {
          printf("Client Sneakiliy Exiting...\n");
          break;
      }
      //sends command to sever
      write(sd, buffer, sizeof(buffer));
      //clears buffer to use for recieving
      memset(buffer,0,100);
      //reads input sent from server
      read(sd, buffer, sizeof(buffer));
      //prints message from server
      printf("$  %s", buffer);
      //prints new line for continued input commands
      printf("\n");
  }

  return 0;
}
