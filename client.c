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
  //int portNumber;
  char serverIP[29];
  int rc = 0;

  //checks number of command args
  if (argc < 2){
    printf ("usage is client <ipaddr> \n");
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
  //portNumber = strtol(argv[2], NULL, 10);
  strcpy(serverIP, argv[1]);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(24000);
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
      rc = read(sd, buffer, 200);
      if (rc == 0 ){
        printf("Server has closed\n");
        close (sd); // close the socket
        break;
      }
      //emulates shell
      printf("[~%s]$ ", buffer);
      n = 0;
      memset(buffer, 0, BUFFSIZE);
      //gets command string from user
      while ((buffer[n++] = getchar()) != '\n');
      //checks to see if user wants to exit shell
      if ((strncmp(buffer, "exit", 4)) == 0) {
          printf("Client Sneakily Exiting...\n");
          break;
      }
      int lastChar = strlen(buffer) - 1;
      if (buffer[lastChar] == '\n'){
        buffer[lastChar] = '\0';
      }
      //sends command to sever
      write(sd, buffer, sizeof(buffer));
      //clears buffer to use for recieving
      memset(buffer, 0, BUFFSIZE);
      //reads input sent from server about command
      rc = read(sd, buffer, sizeof(buffer));
      if (rc == 0 ){
        printf("Server has closed\n");
        close (sd); // close the socket
        break;
      }
      //prints message from server
      printf("\n%s\n", buffer);
  }
  return 0;
}
