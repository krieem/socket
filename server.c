#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>
#include <pthread.h>
#include <sys/time.h>

#define requestLimit 256
#define responseLimit 256
#define SA struct sockaddr
#define SA_IN struct sockaddr_in
#define backlog 10

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int exitFlag = 0;

void error(int errorCode);
void * handle_connection(void* p_client_socket);

void error(int errorCode) {
    switch (errorCode) {
    case 0 :
        fprintf(stdout, "Usage: server <hostname> <port>\n");
        exit(0);
        break;
    case 1 :
        fprintf(stdout, "Error creating socket\n");
        exit(1);
        break;
    case 2 :
        fprintf(stdout, "Error binding to socket\n");
        exit(2);
        break;
    case 3 :
        fprintf(stdout, "Error listening on socket\n");
        exit(3);
        break;
    case 4 :
        fprintf(stdout, "Error accepting connection\n");
        exit(4);
        break;
    case 5 :
        fprintf(stdout, "Read error\n");
        exit(5);
        break;
    case 6 :
        fprintf(stdout, "Invalid exit ack\n");
        exit(6);
        break;
    }
}

void * handle_connection(void* p_client_socket) {
    int client_socket = *((int*)p_client_socket);
    free(p_client_socket);
    char request[requestLimit];
    char response[responseLimit];
    int msgsize;
    size_t bytes_read;
    struct timespec before;
    struct timespec after;

    while (1) {
        //TODO - clear request
        msgsize = 0;
        bytes_read = 0;
        sprintf(request, "Request\n");
        write(client_socket, request, strlen(request));

        clock_gettime(CLOCK_MONOTONIC, &before);

        while((bytes_read = read(client_socket, response+msgsize, sizeof(response)-msgsize-1)) >= 0 ) {
            msgsize += bytes_read;
            if (msgsize > responseLimit-1 || response[msgsize-1] == '\n') break;
        }
        clock_gettime(CLOCK_MONOTONIC, &after);
        if (bytes_read < 0) {
            error(5);
        }
        response[msgsize-1] = 0;
        long secElapsed = (long)(after.tv_sec - before.tv_sec);
        double nanoElapsed = (double)(after.tv_nsec - before.tv_nsec);
        printf("\033[s\r\033[2AResponse: %s RTT: %ld seconds, %lf nanoseconds     \033[u", response, secElapsed, nanoElapsed);
        fflush(stdout);
        sleep(3);

        if (exitFlag == 1) {
            msgsize = 0;
            bytes_read = 0;
            sprintf(request, "Exit\n");
            write(client_socket, request, strlen(request));

            while((bytes_read = read(client_socket, response+msgsize, sizeof(response)-msgsize-1)) >= 0 ) {
                msgsize += bytes_read;
                if (msgsize > responseLimit-1 || response[msgsize-1] == '\n') break;
            }
            response[msgsize-1] = 0;
            if (strncmp(response, "OK", strlen(response)) == 0) {
                sprintf(request, "OK\n");
                write(client_socket, request, strlen(request));
            } else {
                //invalid exit ack
            } 
            break;
        }
    }
    //close(client_socket);
    printf("closing connection\n");
    return NULL;
}


int main(int argc, char* argv[]) {
    int server_socket;
    int client_socket;
    int addr_size;
    int port_number;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    if (argc != 3) {
        error(0);
    }

    port_number = atoi(argv[2]);


    if ((server_socket = socket(AF_INET, SOCK_STREAM , 0)) < 0) {
        error(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    if (bind(server_socket,(SA*)&server_addr, sizeof(server_addr)) < 0) {
        error(2);
    }

    if (listen(server_socket, backlog) < 0) {
        error(3);
    }

    printf("Waiting for connection\n\n");
    fflush(stdout);
    addr_size = sizeof(SA_IN);
    if ((client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size)) < 0) {
        error(4);
    }
    printf("Connected\n\n");
    fflush(stdout);
    pthread_t t;
    int *pclient = malloc(sizeof(int));
    *pclient = client_socket;
    pthread_create(&t, NULL, handle_connection, pclient);
    printf("Response: %s RTT: %ld seconds, %lf nanoseconds   \r\033[2B", " ", (long)0, (double)0);
    fflush(stdout);
    while (1) {
        //scan for user input
        printf("Enter Command> ");
        char str[10];
        scanf("%10s",str);
        if (strncmp(str, "exit", strlen(str)) == 0) {
            pthread_mutex_lock(&lock);
            exitFlag = 1;
            pthread_mutex_unlock(&lock);
            pthread_join(t, NULL);
            break;
        } else {
            //ignore unknown command
        }
    }    
    close(client_socket);
    return 0;
}