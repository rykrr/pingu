/* Pingu Server(Temporary Name) *
 *                              *
 * Date:   October 14, 2018     *
 * Author: Ryan Kerr            */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

int _signal = 0;

void stop(int signum) {
    _signal = 1;
    signal(SIGKILL, stop);
    signal(SIGINT, stop);
}

int init() {
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr   = htonl(INADDR_ANY),
        .sin_port   = htons(9505)
    };
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    int enable = 1;
    struct timeval timeout = {
        .tv_sec  = 5,
        .tv_usec = 0
    };
    
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    if(bind(sock, (struct sockaddr*) &addr, sizeof(addr)))
        return 0;
    
    return sock;
}

int loop(int sock, const char *host) {
   if(!sock)
       return -1;
    
    char buf[256];
    struct sockaddr_in sender;
    socklen_t sender_size;
    ssize_t read = 0;
    
    read = recvfrom(sock, buf, 255, 0, (struct sockaddr*) &sender, &sender_size);
    
    if(read > 0 && !_signal) {
        buf[read] = 0;
        
        if(buf[0] == '-' && buf[1] == '-')
            sendto(sock, host, strlen(host), 0, (struct sockaddr*) &sender, sender_size);
    }
    
    return 0;
}

char *get_host_name() {
    FILE *file = fopen("/proc/sys/kernel/hostname", "rb");
    char *host = malloc(255);
    
    if(file) {
        fscanf(file, "%s", host);
        fclose(file);
    }
    else {
        strcat(host, "noname");
    }
    
    return host;
}

int main() {
    int sock = init();
    char *host = get_host_name();
    
    //printf("%s\n", host);
    
    signal(SIGKILL, stop);
    signal(SIGINT, stop);
    
    while(!_signal && !loop(sock, host));
    
    if(host)
        free(host);
    close(sock);
}

