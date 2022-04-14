#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#define PORT 4688    /* port number */
#define BUFLEN 1024  /* buffer length */
#define PERIOD 3     /* in seconds */
#define LOOPLIMIT 10 /* loop testing send()/recv() */
#define QUITKEY 0x65 /* ASCII code of 'e' */

int kbhit(void){
    static bool initflag = false;
    static const int STDIN = 0;

    if (!initflag) {
        struct termios term;
        tcgetattr(STDIN,&term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN,TCSANOW,&term);
        setbuf(stdin,NULL);
        initflag = true;
    }
    int nbbytes;
    ioctl(STDIN,FIONREAD,&nbbytes);
    return nbbytes;
}

int main(int argc, char const *argv[]){
  int sockfd, acptdsock, optv=1,i=0;
  struct sockaddr_in servaddr;
  struct timeval curTime;
  int addrlen = sizeof(servaddr);
  int diff;
  char buffer[BUFLEN] = {0};
  char *reqst = "R";
  char *endCom = "E";
  char cmd = QUITKEY;   /* character ESC */
  bool stop = false; /* stop running  */


  if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd,(struct sockaddr *)&servaddr,
      sizeof(servaddr)) == -1){
    perror("bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd,3) == -1){
    perror("listen failed");
    exit(EXIT_FAILURE);
  }
  time_t now = time(NULL);
  char *string_time = ctime(&now);
  printf("%s", string_time);
  printf("\nServer waiting connection....\n");
  if ((acptdsock=accept(sockfd,(struct sockaddr *)&servaddr,
        (socklen_t*)&addrlen)) == -1){
    perror("accept() tried but not succeded, keep trying...");
    exit(EXIT_FAILURE);
  }

  do { /* loop for send()/recv() */
    ++i;
    if ((send(acptdsock, reqst, strlen(reqst) , 0 )) == -1){
      perror("send() failed ");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    gettimeofday(&curTime, NULL);
    int sntmilli = curTime.tv_usec / 1000;
    char sentTime[4] = "";
    sprintf(sentTime, "%03d", sntmilli);

    printf("%2d Sending request to the client: %s\n",i,reqst);

    if ((recv(acptdsock,buffer,BUFLEN-1,0)) == -1)
      perror("recv() failed ");
      buffer[BUFLEN-1]=0x00;  /* force ending with '\0' */
      printf("   Message from the client: %s\n",buffer);
      gettimeofday(&curTime, NULL);
      int revmilli = curTime.tv_usec / 1000;
      char rcvTime[4] = "";
      sprintf(rcvTime, "%03d", revmilli);
      if( revmilli>sntmilli )
        diff=revmilli-sntmilli;
      else
        diff=sntmilli-revmilli;
      printf("   Round trip time: %3d Millisecond \n \n", diff );

      while ((kbhit()) && (!stop)){
        cmd = getchar();
        fflush(stdout);
        if (cmd == QUITKEY)
          stop = true;
      }
      sleep(PERIOD);
  }
  while (!stop);
  send(acptdsock, endCom, strlen(endCom) , 0 );
  printf("Termination request has been sent to the client\n\n");
  char bufferEnd[BUFLEN] = {0};
  recv(acptdsock,bufferEnd,BUFLEN-1,0);
  printf("Client terminated: %s \n",bufferEnd);

    sleep(PERIOD); /* unsigned int sleep(unsigned int seconds) */               /* end of while loop */

  if ((write(acptdsock, &cmd, 1)) == -1){ /* write() works */
    perror("send() failed ");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (close(sockfd)== -1){
    perror("close socket failed ");
    exit(EXIT_FAILURE);
  }
  printf("Server terminated!\n");
  return 0; /* with success */
}
