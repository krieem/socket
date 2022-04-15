#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define SERVIPADDRESS "127.0.0.1" /* loopback for testing */
#define SERVPORT 4688             /* port number */
#define BUFLEN 1024               /* buffer length */
#define QUITKEY 0x65 /* ASCII code of ESC */

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
  }

int main(int argc, char const *argv[]){
    int sockfd = 0, i=0;
    struct sockaddr_in serv_addr;
    char buffer[BUFLEN] = {0};
    char *requts = "R";
    char *endCom = "E";
    char *endCon = "OK!";
    struct timeval curTime;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        perror("Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERVIPADDRESS);
    serv_addr.sin_port = htons(SERVPORT);

    printf("\nClient connecting Server....\n");
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1){
        perror("Server started? Connection failed \n");
        exit(EXIT_FAILURE);
    }
    printf("....Connection established\n");

    while (1) { /* loop for send()/recv() */
      if ((recv(sockfd,buffer,BUFLEN-1,0)) == -1)
        perror("recv() failed ");

      if (strcmp(buffer, requts) == 0) { /* Respond to the server if R letter is received */
      printf("%2d Request from the server has been received\n",i++);
      }
      else {                             /* Terminate if else */
        send(sockfd, endCon,strlen (endCon),0);
        close(sockfd);
        printf("Server terminated!\n\n");
        return 0;
      }

      /* Time of sending */
      gettimeofday(&curTime, NULL);
      int milli = curTime.tv_usec / 1000;
      char buffer [80];
      strftime(buffer, 80, "%H:%M:%S", localtime(&curTime.tv_sec));
      char ackmsg[84] = "";
      int randomNum = rand() % 1000;
      sprintf (ackmsg, "%4d ACK from the client at  %s:%03d",randomNum , buffer, milli); /* Random Number + Message + Time */
      /* Time of sending */

      if ((send(sockfd, ackmsg,strlen (ackmsg),0)) == -1){
        perror("send failed ");
        close(sockfd);
        exit(EXIT_FAILURE);
      }
      char *dismsg = "Message has been sent successfully at: "; /* Client message */
      printf("%s %s:%03d\n\n" , dismsg, buffer, milli);
    } /* end of while loop */

    if (close(sockfd) == -1){
      perror("close socket failed ");
      exit(EXIT_FAILURE);
    }
    printf("....Client retuned!\n\n");
    return 0; /* with success */
}
