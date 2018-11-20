#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFSIZE 256

void runCmd(char *cmd, char *buff){
  FILE* cmdOut = popen(cmd, "r");
  if(cmdOut == NULL){
    perror("popen");
    exit(1);
  }
  fgets(buff, BUFFSIZE, cmdOut);
  pclose(cmdOut);
}

int main()
{
  int sd; /* socket descriptor */
  int connected_sd; /* socket descriptor */
  int rc; /* return code from recvfrom */
  struct sockaddr_in server_address;
  struct sockaddr_in from_address;
  char clientCmd[BUFFSIZE];
  char wdBuff[BUFFSIZE];
  char cmdMsg[BUFFSIZE];
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

  FD_ZERO(&socketFDS);
  FD_SET(sd, &socketFDS); // Sets the bit for the initial SD
  maxSD = sd;
  for(;;){

    rc = select (maxSD+1, &socketFDS, NULL, NULL, NULL); // block until something arrives

    if (FD_ISSET(sd, &socketFDS)){
	    connected_sd = accept (sd, (struct sockaddr *) &from_address, &fromLength);
      if (connected_sd < 0) {
          printf("server acccept failed...\n");
          exit(0);
      } else printf("server acccept the client...\n");
      clientSD = connected_sd;
      FD_SET(clientSD, &socketFDS);
      if (clientSD > maxSD)
        maxSD = clientSD;
	    if (FD_ISSET(clientSD, &socketFDS)){
        for(;;){
          memset (clientCmd, 0, BUFFSIZE);
          memset (wdBuff, 0, BUFFSIZE);
          memset (cmdMsg, 0, BUFFSIZE);
          // get working directory to send to client
          runCmd("pwd", wdBuff);
          // write dir to client socket
          //printf("pre dir write\n");
          write(clientSD, wdBuff, BUFFSIZE);
          //printf("post dir write\n");
          // read client comand off socket
          //printf("pre client cmd read\n");
  	      rc = read(clientSD, clientCmd, BUFFSIZE);
          //printf("post client cmd read\n");
          // run client command and get the terminal return
          runCmd(clientCmd, cmdMsg);
          //write the command return to the client socket
          //printf("pre cmd return write\n");
          write(clientSD, cmdMsg, BUFFSIZE);
          //printf("post cmd return write\n");

  	      if (rc == 0 ){ //  the client disconnected
            printf("client disconnected\n");
            FD_CLR(clientSD, &socketFDS);
  	        close (clientSD); // close the socket
  	      }
  	      else {
            printf ("client issued command: %s\n", clientCmd);
  	        printf ("terminal responded with: %s\n", cmdMsg);
          }
        }

	    }else {
        printf ("Client socket not set in socketFDS\n");
      }

    } else {
      printf ("Listening socket not set in socketFDS\n");
    }
  }
  close(sd);
  return 0;
}
