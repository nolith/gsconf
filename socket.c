#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include "socket.h"
#include <sys/select.h>
int sock;


struct sockaddr_in sockAddr;

unsigned char * get_mac(const char *intf) {
        unsigned char * mac = (unsigned char *) malloc(sizeof(unsigned char *) * 6);
        struct ifreq iface;
        unsigned char i;
        
        int tmpSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        
        
        //How to get the MAC of the Interface: http://www.geekpage.jp/en/programming/linux-network/get-macaddr.php
        iface.ifr_addr.sa_family = AF_INET;
        strncpy(iface.ifr_name, intf, IFNAMSIZ-1);
        ioctl(tmpSock, SIOCGIFHWADDR, &iface);
        close(tmpSock);
        
        for (i = 0; i < 6; i++)
                mac[i] = (unsigned char)iface.ifr_hwaddr.sa_data[i];
        
        return mac;
}

void init_socket() {
        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        
        if (sock == -1) {
                printf("Could not create Socket\n");
                exit(1);
        }
        
        
        
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        sockAddr.sin_port = htons(63321);
        
        if (bind(sock, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
                printf("Could not bind to socket\n");
                exit(1);
        }
        
        
}


int sendBroadcast(char * data, unsigned int len) {
        
        int bcp = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &bcp, sizeof(bcp)) < 0) {
                printf("Could not broadcast data\n");
                exit(1);
        }
        
        struct sockaddr_in bcAddr;
        
        bcAddr.sin_family = AF_INET;
        bcAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
        bcAddr.sin_port = htons(63322);
        
        return sendto(sock, data, len, 0, (struct sockaddr *) &bcAddr, sizeof(bcAddr));
        
          
}

int recvBroadcast(char * data) {

        struct sockaddr_in sa_in;
        int sa_len = sizeof(sa_in);
        struct 	timeval tout;
        tout.tv_sec = 0;
        tout.tv_usec = 500000;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        int s = select(sock + 1, &fds, NULL, NULL, &tout);
        if (s <= 0)
                return -1;
        return recvfrom(sock, data, PACKET_BUFFER, 0,(struct sockaddr * )&sa_in, &sa_len);
        
}


int recvBroadcast_tout(char * data) {

        struct sockaddr_in sa_in;
        int sa_len = sizeof(sa_in);
        struct 	timeval tout;
        tout.tv_sec = 5;
        tout.tv_usec = 0;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);
        int s = select(sock + 1, &fds, NULL, NULL, &tout);
        if (s <= 0)
                return -1;
        return recvfrom(sock, data, PACKET_BUFFER, 0,(struct sockaddr * )&sa_in, &sa_len);
        
}


