#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>





int main (){
    #include <sys/time.h>

    long start = micro_time();

/*
  do some works
 */
    sleep(1);
    long end = micro_time();

// print duration in miliseconds with decimals to microsecond level
    printf("call %.3f ms\n", (end - start) / 1000.0);
}