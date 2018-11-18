#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//#define MAXCLIENTS 10  //NEW
int main()
{
  int sd; /* socket descriptor */
  int connected_sd; /* socket descriptor */
  int rc; /* return code from recvfrom */
  struct sockaddr_in server_address;
  struct sockaddr_in from_address;
  char buffer[100];
  int flags = 0;
  socklen_t fromLength;
  //int clientSDList[MAXCLIENTS] = {0}; // NEW
  int clientSD = 0;
  fd_set socketFDS; // NEW
  int maxSD = 0;//NEW
  int i;

  sd = socket (AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(24000);
  server_address.sin_addr.s_addr = INADDR_ANY;

  if((bind (sd, (struct sockaddr *)&server_address, sizeof(server_address))) != 0){
    printf("socket bind failed...\n");
    exit(0);
  } else printf("Socket successfully binded..\n");

  if((listen (sd, 5)) != 0){
    printf("Listen failed...\n");
    exit(0);
  } else printf("Server listening..\n");

  maxSD = sd;
  for(;;){
    memset (buffer, 0, 100);
    FD_ZERO(&socketFDS);
    FD_SET(sd, &socketFDS); //Sets the bit for the initial SD

    rc = select (maxSD+1, &socketFDS, NULL, NULL, NULL); // NEW block until something arrives

    if (FD_ISSET(sd, &socketFDS)){
	    connected_sd = accept (sd, (struct sockaddr *) &from_address, &fromLength);
      if (connected_sd < 0) {
          printf("server acccept failed...\n");
          exit(0);
      } else printf("server acccept the client...\n");
      printf("Client connected with descriptor: %i\n", connected_sd);
      clientSD = connected_sd;
      FD_SET(clientSD, &socketFDS);
      if (clientSD > maxSD)
        maxSD = clientSD;
	    if (FD_ISSET(clientSD, &socketFDS)){
        printf("Entered read stage\n");
	      rc = read(clientSD, &buffer, 100);
	      if (rc == 0 ){ //  the client disconnected
          printf("client disconnected\n");
          FD_CLR(clientSD, &socketFDS);
	        close (clientSD); // close the socket

	      }
	      else
	        printf ("rc from read  %d received the following %s\n", rc, buffer);
	    }// NEW

    }
  }// NEW
  return 0;
}
