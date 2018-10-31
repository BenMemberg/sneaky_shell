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
  
  if (argc < 4){
    printf ("usage is client <ipaddr> <port> < number> \n");
    exit(1);
  }

  sd = socket(AF_INET, SOCK_STREAM, 0);

  portNumber = strtol(argv[2], NULL, 10);
  strcpy(serverIP, argv[1]);
  sprintf (buffer, "hello world from %s \n", argv[3]);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portNumber);
  server_address.sin_addr.s_addr = inet_addr(serverIP);
 
  if (connect (sd, (struct sockaddr *) &server_address, sizeof (struct sockaddr_in ))<0){
    close (sd);
    perror ("error connecting stream socket ");
    exit (1);
  }
  
  for (;;){
    rc = write (sd, buffer, strlen(buffer));
    if (rc < 0)
      perror ("write");
    printf ("sent  %d bytes\n", rc);
    sleep (3);
  }

  return 0 ; 

}
