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
#define PORT 8886    /* port number */
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
  int addrlen = sizeof(servaddr);
  char buffer[BUFLEN] = {0};
  char *reqst = "R";
  char cmd = QUITKEY;   /* character ESC */
  bool stop = false; /* stop running  */

  
  if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,
      &optv,sizeof(optv))){
    perror("setsockopt SO_REUSEADDR failed");
    exit(EXIT_FAILURE);
  }
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,
      &optv,sizeof(optv))){
    perror("setsockopt SO_REUSEPORT failed");
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

  
  
  while (1) { /* loop for send()/recv() */
    if ((send(acptdsock, reqst, strlen(reqst) , 0 )) == -1){
      perror("send() failed ");
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    printf("%2d Sent:     %s\n",i,reqst);
    time_t sent_time = time(NULL); /* time stamp when sending the package */

    if ((recv(acptdsock,buffer,BUFLEN-1,0)) == -1)
      perror("recv() failed ");
    buffer[BUFLEN-1]=0x00;  /* force ending with '\0' */
    printf("   Received: %s\n",buffer);

    time_t recv_time = time(NULL); /* time stamp when recv the package */ 

    double diff_time = difftime(sent_time, recv_time); 
    printf("DiffTime: %f/n", diff_time); /* diff calculation between sent and recv */ 





    if ((++i) == LOOPLIMIT) /* LOOPLIMIT reached  */
      break;

    while ((kbhit()) && (!stop)) {
      cmd = getchar();
      fflush(stdout);
      if (cmd == QUITKEY)
        stop = true;
    }
    if (stop)
      break;

    sleep(PERIOD); /* unsigned int sleep(unsigned int seconds) */
  }                /* end of while loop */

  if ((write(acptdsock, &cmd, 1)) == -1){ /* write() works */
    perror("send() failed ");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (close(sockfd)== -1){
    perror("close socket failed ");
    exit(EXIT_FAILURE);
  }
  printf("....Server retuned!\n\n");
  return 0; /* with success */
}
