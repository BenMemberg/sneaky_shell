#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXCLIENTS 10  //NEW
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
  int clientSDList[MAXCLIENTS] = {0}; // NEW
  fd_set socketFDS; // NEW
  int maxSD = 0;//NEW
  int i;
  
  sd = socket (AF_INET, SOCK_STREAM, 0);
  
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(24000);
  server_address.sin_addr.s_addr = INADDR_ANY;

  bind (sd, (struct sockaddr *)&server_address, sizeof(server_address));

  listen (sd, 5);
  maxSD = sd; // NEW
  for(;;){// NEW
    memset (buffer, 0, 100);
    FD_ZERO(&socketFDS);// NEW
    FD_SET(sd, &socketFDS); //NEW - sets the bit for the initial SD
    for (i=0;i<MAXCLIENTS;i++){ //NEW
      if (clientSDList[i]>0){ //NEW
	      FD_SET(clientSDList[i], &socketFDS); //NEW
	      if (clientSDList[i] > maxSD) //NEW
	        maxSD = clientSDList[i]; //NEW
      }
    }
    rc = select (maxSD+1, &socketFDS, NULL, NULL, NULL); // NEW block until something arrives

    if (FD_ISSET(sd, &socketFDS)){ // NEW
	    connected_sd = accept (sd, (struct sockaddr *) &from_address, &fromLength);
	    for (i=0;i<MAXCLIENTS;i++) { //NEW
	      if (clientSDList[i] ==0){ //NEW
	        clientSDList [i] = connected_sd; //NEW
	        break; // NEW
	      }
      }
      for (i=0;i<MAXCLIENTS;i++) { // NEW
	      if (FD_ISSET(clientSDList[i], &socketFDS)){ //NEW
	        rc = read(clientSDList[i], &buffer, 100);//NEW
	        if (rc == 0 ){ // NEW - the client disconnected
	          close (clientSDList[i]); // NEW close the socket
	          clientSDList[i] = 0; // reuse the slot
	        }
	        else
	          printf ("rc from read  %d received the following %s\n", rc, buffer);
	      }// NEW
      }
    }
  }// NEW
  return 0;
}
