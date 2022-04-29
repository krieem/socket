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
#define configLimit 256
#define SA struct sockaddr
#define backlog 10

int exitFlag = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void error(int errorCode);
void get_next_line(int sockfd, char request[]);
void handle_connection(int sockfd);
void * check_for_input();

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


void get_next_line(int sockfd, char response[]) {
    int msgsize = 0;
    size_t bytes_read = 0;
    memset(response, 0, responseLimit);
     
    while((bytes_read = read(sockfd, response+msgsize, responseLimit-msgsize-1)) >= 0 ) {
        msgsize += bytes_read;
        if (msgsize > responseLimit-1 || response[msgsize-1] == '\n') {
            break;
        } 
    }

    if (bytes_read < 0) {
        error(5);
    }

    // Null terminate request
    response[msgsize-1] = 0;
}


void handle_connection(int sockfd) {
    char request[requestLimit];
    char response[responseLimit];
    struct timespec before;
    struct timespec after;
    time_t secElapsed;
    long nanoElapsed;


    while (1) {
        if (exitFlag) {
            memset(request, 0, requestLimit);
            sprintf(request, "Exit\n");
            write(sockfd, request, strlen(request));
            
            get_next_line(sockfd, response);
           
            if (strncmp(response, "OK", strlen(response)) == 0) {
                memset(request, 0, requestLimit);
                sprintf(request, "OK\n");
                write(sockfd, request, strlen(request));

            } else {
                error(6);
            } 

            pthread_mutex_lock(&lock);
            exitFlag = 0;
            pthread_mutex_unlock(&lock);
            break;
        }
       
        sprintf(request, "Request\n");

        clock_gettime(CLOCK_MONOTONIC, &before);
        write(sockfd, request, strlen(request));
        get_next_line(sockfd, response);
        clock_gettime(CLOCK_MONOTONIC, &after);
       
        secElapsed = after.tv_sec - before.tv_sec;
        nanoElapsed = after.tv_nsec - before.tv_nsec;

        printf("\r\33[1AResonse: %s RTT: %lld.%.9ld Seconds  \r\033[1B\033[35C", response, (long long)secElapsed, nanoElapsed);
        fflush(stdout);
        sleep(3);
    } // while

    printf("closing connection\n");
}

void * check_for_input () {
    while (1) {
        char str[10];
        printf("\rPress e then enter to exit server>");
        fflush(stdout);
        scanf("%10s",str);
        if (strncmp(str, "e", 1) == 0) {
            pthread_mutex_lock(&lock);
            exitFlag = 1;
            pthread_mutex_unlock(&lock);
            break;
        } else {
            //ignore unknown command
        }
    }  
    while (exitFlag == 1) {
        
    }
}


struct sockaddr_in import_config() {
    char fileName[] = "config.txt";
    char config[configLimit];
    struct sockaddr_in serv_addr;
    FILE *fd;
    int msgsize = 0;
    int port_number;

    memset(&serv_addr, 0, sizeof(serv_addr));
    fd = fopen(fileName, "r");
    if (fd == NULL) {
        error(0); //invalid config
    }

    memset(config, 0, configLimit);
    fseek(fd, 0, SEEK_SET);

    // Get IP
    fgets(config, configLimit, fd);
    msgsize = strlen(config);

    if (msgsize < 0) {
        error(0); //invalid config
    } 
    // Null terminate request
    config[msgsize-1] = 0;

    if (inet_pton(AF_INET, config, &serv_addr.sin_addr) <= 0) {
        error(0); //invalid config
    }

    // Get Port
    memset(config, 0, configLimit);
    fgets(config, configLimit, fd);
    msgsize = strlen(config);

    if (msgsize < 0) {
        error(0); //invalid config
    } 
    // Null terminate request
    config[msgsize-1] = 0;

    port_number = atoi(config);
    if (port_number == 0) {
        error(0); //invalid config
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);

    fclose(fd);
    return serv_addr;
}

int main(int argc, char* argv[]) {
    int server_socket;
    int client_socket;
    int addr_size;
    int port_number;
    int reuse_port = 1;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    
    if (argc != 3) {
        server_addr = import_config();
    } else {

         if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
            //invalid ip
        }
     
        port_number = atoi(argv[2]);
        server_addr.sin_family = AF_INET;
        //server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port_number);
    }

    

    if ((server_socket = socket(AF_INET, SOCK_STREAM , 0)) < 0) {
        error(1);
    }

    if (bind(server_socket,(SA*)&server_addr, sizeof(server_addr)) < 0) {
        error(2);
    }

    if (listen(server_socket, backlog) < 0) {
        error(3);
    }

    printf("Waiting for connection\n");
    addr_size = sizeof(struct sockaddr_in);

    if ((client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size)) < 0) {
        error(4);
    }

    printf("Connected\n");
    printf("Press e then enter to exit server >\n");
    // Create UI thread
    pthread_t t;
    pthread_create(&t, NULL, check_for_input, NULL);
    
    handle_connection(client_socket);
    
    // Wait for UI thread
    pthread_join(t, NULL);  
    close(client_socket);
    return 0;
}