/* Pingu (Temporary Name)       *
 *                              *
 * Date:   October 14, 2018     *
 * Author: Ryan Kerr            */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <errno.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int _signal = 0;

void stop(int signum) {
    _signal = 1;
}

int init() {
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr   = htonl(INADDR_ANY),
        .sin_port   = htons(9500)
    };
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    int enable = 1;
    struct timeval timeout = {
        .tv_sec  = 2,
        .tv_usec = 0
    };
    
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    if(bind(sock, (struct sockaddr*) &addr, sizeof(addr)))
        return 0;
    
    struct sockaddr_in bcast = {
        .sin_family = AF_INET,
        .sin_addr   = htonl((unsigned int) 0xFFFFFFFF),
        .sin_port   = htons(9505)
    };
    
    ssize_t sent;
    sent = sendto(sock, "--", 2, 0, (struct sockaddr*) &bcast, sizeof(bcast));
    
    return sock;
}

int loop(int sock) {
   if(!sock)
       return -1;
    
    char buf[256];
    struct sockaddr_in sender;
    socklen_t sender_size = sizeof(sender);
    
    ssize_t read = 0;
    read = recvfrom(sock, buf, 255, 0, (struct sockaddr*) &sender, &sender_size);
    
    if(read > 0) {
        buf[read] = 0;
        
        printf("%-20s ", buf);
        
        unsigned int addr = ntohl(sender.sin_addr.s_addr);
        for(int i=0; i<4; i++)
            printf("%d%c", (addr >> ((3-i)*8))&0xFF, i==3? '\n' : '.');
        return 0;
    }
    
    return 1;
}

int main() {
    int sock = init();
    
    if(errno) {
        fprintf(stderr, "Broadcast Error: %s\n", strerror(errno));
        return 0;
    }
    
    printf("%-20s %s\n", "Host", "Address");
    printf("-----------------------------------\n");
    
    signal(SIGKILL, stop);
    signal(SIGINT, stop);
    
    while(!_signal && !loop(sock));
    
    if(sock)
        close(sock);
}

