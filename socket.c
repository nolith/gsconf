#ifdef OSX
#include <sys/types.h>
#include <sys/sysctl.h>
#include <net/if_dl.h>
#endif
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
#include <errno.h>

int sock;


struct sockaddr_in sockAddr;

#ifdef LINUX
unsigned char * linux_get_mac(const char *intf) {
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
#endif

#ifdef OSX
unsigned char * bsd_get_mac(const char *intf) {
        int32_t     mib[6];
        size_t     len;
        char      *buf;
        unsigned char   *mac;
        struct if_msghdr  *ifm;
        struct sockaddr_dl  *sdl;

        mib[0] = CTL_NET;
        mib[1] = AF_ROUTE;
        mib[2] = 0;
        mib[3] = AF_LINK;
        mib[4] = NET_RT_IFLIST;
        if ((mib[5] = if_nametoindex(intf)) == 0) {
                perror("if_nametoindex error");
                exit(2);
        }

        if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
                perror("sysctl 1 error");
                exit(3);
        }

        if ((buf = malloc(len)) == NULL) {
                perror("malloc error");
                exit(4);
        }

        if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
                perror("sysctl 2 error");
                exit(5);
        }

        ifm = (struct if_msghdr *)buf;
        sdl = (struct sockaddr_dl *)(ifm + 1);
        mac = (unsigned char *)LLADDR(sdl);
        return mac;
}
#endif

unsigned char * get_mac(const char *intf) {
#ifdef LINUX
        return linux_get_mac(intf);
#endif
#ifdef OSX
        return bsd_get_mac(intf);
#endif
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


int sendBroadcast(unsigned char * data, unsigned int len) {
        
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
        socklen_t sa_len = sizeof(sa_in);
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
        socklen_t sa_len = sizeof(sa_in);
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


