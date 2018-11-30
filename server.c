#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFSIZE 10000

// string tracking the shell's working directory
char workDir[200];
// string tracking the previous working directory
char prevDir[200];

void runCmd(char *cmd, char *buff){
  char command[500] = "cd ";
  strcat(command, workDir);
  strcat(command, " && ");
  strcat(command, cmd);
  FILE* cmdOut = popen(command, "r");
  if(cmdOut == NULL){
    perror("popen");
    exit(1);
  }
  int n=0;
  char *buffStart = buff;
  while(fgets(buff, BUFFSIZE, cmdOut) != NULL){
    puts(buff);
    n = strlen(buff);
    buff = buff+n;
    if(buffStart+BUFFSIZE < buff){
      buff=buffStart;
    }
  }
  //buff = buffStart;
  pclose(cmdOut);
}

void checkCmd(char* cmd, char*buff){
  // define strings for client strings to match
  char cd[2] = "cd";
  char upDir[2] = "..";
  char rootDir[1] = "~";
  char explDir[1] = "/";
  char blankLine[1] = "\0";
  char backDir[1] = "-";
  // parse first two client command tokens
  char * cmdType = strtok(cmd, " ");
  char * cmdCrit = strtok(NULL, " ");
  // parse client command tokens
  if (cmdType == NULL){

    printf("Client entered a blank line command\n");

  } else if (strncmp(cd, cmdType, 2) == 0){
    // parse second token of the cd command
    if ( (cmdCrit == NULL) || (strncmp(rootDir, cmdCrit, 1) == 0) ){

      printf("change wd to root directory\n");
      // save previous wd in prevDir
      memset(prevDir, 0, 200);
      strcpy(prevDir, workDir);
      // clear workDir to set to root folder string
      memset(workDir, 0, 200);
      runCmd("pwd", workDir);
      int lastChar = strlen(workDir) - 1;
      if (workDir[lastChar] == '\n'){
        workDir[lastChar] = '\0';
      }

    } else if (strncmp(upDir, cmdCrit, 2) == 0){
      printf("change wd up a directory\n");
      // save previous wd in prevDir
      memset(prevDir, 0, 200);
      strcpy(prevDir, workDir);
      // get total length of wd
      int dirLen = strlen(workDir);
      //
      char * rem = strrchr(workDir, '/');
      //
      int remLen = strlen(rem);
      //
      int newDirLen = dirLen - remLen;
      //
      workDir[newDirLen] = '\0';

    } else if (strncmp(explDir, cmdCrit, 1) == 0){

      printf("Explicit workDir: %s\n", cmdCrit);
      // save previous wd in prevDir
      memset(prevDir, 0, 200);
      strcpy(prevDir, workDir);
      // copy explicit directory declaration into workDir
      strcpy(workDir, cmdCrit);

    } else if(strncmp(backDir, cmdCrit, 1) == 0){
      // command to go lo the last visited directory
      char temp[200];
      // save current wd into temp
      memset(temp, 0, 200);
      strcpy(temp, workDir);
      // save prevDir into workDir
      memset(workDir, 0, 200);
      strcpy(workDir, prevDir);
      // save previous dir into prevDir from temp
      memset(prevDir, 0, 200);
      strcpy(prevDir, temp);

    } else {

      printf("Subdirectory: %s\n", cmdCrit);
      // save previous wd in prevDir
      memset(prevDir, 0, 200);
      strcpy(prevDir, workDir);
      // add subdirectory name on to wd
      strcat(workDir, "/");
      strcat(workDir, cmdCrit);

    }
    char * cmdConcat = strtok(NULL, " ");
    char and[2] = "&&";
    if ( (cmdConcat != NULL) && (strncmp(and, cmdConcat, 2) == 0) ){

      char * rawRemCmd = strstr(workDir, "&&");
      char * remCmd = &rawRemCmd[2];
      checkCmd(remCmd, buff);

    }
  } else {
    runCmd(cmd, buff);
  }
}

int main()
{
  int sd; /* socket descriptor */
  int connected_sd; /* socket descriptor */
  int rc; /* return code from recvfrom */
  struct sockaddr_in server_address;
  struct sockaddr_in from_address;
  char clientCmd[BUFFSIZE];
  //char wdBuff[BUFFSIZE];
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

  // get working directory to send to client
  runCmd("pwd", workDir);
  int lastChar = strlen(workDir) - 1;
  if (workDir[lastChar] == '\n'){
    workDir[lastChar] = '\0';
  }
  strcpy(prevDir, workDir);

  maxSD = sd;
  for(;;){
    // clear relevent buffers
    memset (clientCmd, 0, BUFFSIZE);
    memset (cmdMsg, 0, BUFFSIZE);
    FD_ZERO(&socketFDS);
    FD_SET(sd, &socketFDS); //Sets the bit for the initial SD

    rc = select (maxSD+1, &socketFDS, NULL, NULL, NULL); // NEW block until something arrives

    if (FD_ISSET(sd, &socketFDS)){
	    connected_sd = accept (sd, (struct sockaddr *) &from_address, &fromLength);
      if (connected_sd < 0) {
          printf("Server connection acccept failed...\n");
          exit(0);
      } else printf("Server acccepted the client...\n");
      //printf("Client connected with descriptor: %i\n", connected_sd);
      clientSD = connected_sd;
      FD_SET(clientSD, &socketFDS);
      if (clientSD > maxSD)
        maxSD = clientSD;
	    if (FD_ISSET(clientSD, &socketFDS)){
        for(;;){
          memset (clientCmd, 0, BUFFSIZE);
          //memset (wdBuff, 0, BUFFSIZE);
          memset (cmdMsg, 0, BUFFSIZE);
          // write dir to client socket
          write(clientSD, workDir, 200);
          memset (clientCmd, 0, BUFFSIZE);
          // read client comand off socket
	        rc = read(clientSD, clientCmd, BUFFSIZE);
          // check if the client disconnected
	        if (rc == 0 ){
            printf("Client has disconnected\n");
            FD_CLR(clientSD, &socketFDS);
	          close (clientSD); // close the socket
            break;
	        }
          // run client command and get the terminal return
          checkCmd(clientCmd, cmdMsg);
          //write the command return to the client socket
          write(clientSD, cmdMsg, BUFFSIZE);
          printf ("Client issued command: [%s] to the terminal\n", clientCmd);
	        printf ("Terminal responded with:\n %s\n", cmdMsg);
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
