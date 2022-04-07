#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define SERVIPADDRESS "127.0.0.1" /* loopback for testing */
#define SERVPORT 8886             /* port number */
#define BUFLEN 1024               /* buffer length */
#define QUITKEY 0x65 /* ASCII code of ESC */


int main(int argc, char const *argv[]){
    time_t now =time(NULL);
    int sockfd = 0, i=0;
    struct sockaddr_in serv_addr;
    char buffer[BUFLEN] = {0};
    char *ackmsg = "ACK from client local time";
    char requts = 'R';
    char *string_now = ctime(&now);
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
      buffer[BUFLEN-1] = 0x00;    /* force ending with '\0' */
      if (buffer[0] == QUITKEY)   /* prepare termination */
        break;
      if (buffer[0] == requts)
      printf("%2d Request Received...\n",i++);
      else
      printf("%2d Received: Uknowing request",i++);

      
      if ((send(sockfd,ackmsg,strlen(ackmsg),0)) == -1){
        perror("send failed ");
        close(sockfd);
        exit(EXIT_FAILURE);
      }
      printf("   Sent:     %s\n", ackmsg);
    } /* end of while loop */

    if (close(sockfd) == -1){
      perror("close socket failed ");
      exit(EXIT_FAILURE);
    }
    printf("....Client retuned!\n\n");
    return 0; /* with success */
}
