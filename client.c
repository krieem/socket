#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define requestLimit 256
#define responseLimit 256
#define SA struct sockaddr

void error(int errorCode);

void error(int errorCode) {
    switch (errorCode) {
    case 0 :
        fprintf(stdout, "Usage: client <hostname> <port>\n");
        exit(0);
        break;
    case 1 :
        fprintf(stdout, "Error creating socket\n");
        exit(1);
        break;
    case 2 :
        fprintf(stdout, "Hostname error\n");
        exit(2);
        break;
    case 3 :
        fprintf(stdout, "Connection failed\n");
        exit(3);
        break;  
    case 4 :
        fprintf(stdout, "Write error\n");
        exit(4);
        break;
    case 5 :
        fprintf(stdout, "Read error\n");
        exit(5);
        break;
    }
}

int main(int argc, char* argv[]) {
    int sockfd;
    int port_number;
    int sendBytes;
    int msgsize = 0;
    size_t bytes_read;
    struct sockaddr_in serv_addr;
    struct timeval currentTime;
    time_t t;
    struct tm *info;
    char request[requestLimit];
    char response[responseLimit];

    if (argc != 3) {
        error(0);
    }

    port_number = atoi(argv[2]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);
    
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        error(2);
    }

    if (connect(sockfd, (SA *)&serv_addr, sizeof(serv_addr)) < 0) {
        error(3);
    }

    while (1) {
        msgsize = 0;
        bytes_read = 0;
        sendBytes = 0;
        while((bytes_read = read(sockfd, request+msgsize, sizeof(request)-msgsize-1)) >= 0 ) {
            msgsize += bytes_read;
            if (msgsize > requestLimit-1 || request[msgsize-1] == '\n') break;
        }

        if (bytes_read < 0) {
            error(5);
        }
        request[msgsize-1] = 0;

        if (strncmp(request, "Request", strlen(request)) == 0) {
            printf("%s\n", request);
            gettimeofday(&currentTime, NULL);
            t = currentTime.tv_sec;
            info = localtime(&t);
            sprintf(response,"ACK at %d:%d:%d:%ld %d\n", info->tm_hour, info->tm_min, info->tm_sec, (currentTime.tv_usec/1000), rand());
            sendBytes = strlen(response);
            if (write(sockfd, response, sendBytes) != sendBytes) {
                error(4);
            }
        } else if (strncmp(request, "Exit", strlen(request)) == 0) {
            msgsize = 0;
            bytes_read = 0;
            sendBytes = 0;
            printf("%s\n", request);
            sprintf(response,"OK\n");
            sendBytes = strlen(response);
            if (write(sockfd, response, sendBytes) != sendBytes) {
                error(4);
            }
            while((bytes_read = read(sockfd, request+msgsize, sizeof(request)-msgsize-1)) >= 0 ) {
                msgsize += bytes_read;
                if (msgsize > requestLimit-1 || request[msgsize-1] == '\n') break;
            }

            if (bytes_read < 0) {
                error(5);
            }
            request[msgsize-1] = 0;
            if (strncmp(request, "OK", strlen(request)) == 0) {
                close(sockfd);
                return 0;
            } else {
                //invalid exit acknowledgment
            }
        }
    }
    return 0;
}

