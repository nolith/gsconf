#ifndef GS105E_H
#define GS105E_H
#include <stdio.h>

#define GS_MODEL 0x0001
#define GS_NAME 0x0003
#define GS_MAC 0x0004
#define GS_IP 0x0006
#define GS_SUBNETMSK 0x0007
#define GS_GATEWAY 0x0008

#define GS_NEWPASSWORD 0x0009
#define GS_PASSWORD 0x000a

#define GS_FIRMWAREVER 0x000d

#define GS_PACKETSTATISTICS 0x1000

#define GS_VLAN 0x2800
#define GS_PORTSTATUS 0x0c00
#define GS_PORTDIAG 0x1c00
#define ACT_DHCP 0x000b

#define GS_VLANSETTING 0x2000


#define ACT_ADDVLAN 0x2800
#define ACT_DELVLAN 0x2c00
#define ACT_RESTART 0x0013
#define ACT_PORTMIRROR 0x5c00



#define QR_REQ 0x0101
#define QR_ANSWER 0x0102
#define QR_EXEC 0x0103
#define QR_EXECUTED 0x0104

#define MSK_PORT1 0x80
#define MSK_PORT2 0x40
#define MSK_PORT3 0x20
#define MSK_PORT4 0x10
#define MSK_PORT5 0x08


#define DHCP_ON 0x01
#define DHCP_OFF 0x00
#define DHCP_RENEW 0x02

int packetId;

struct vlan {
        int id;
        char members;
        char tag;
        struct vlan * next;
};

struct portSniffing {
        char ports;
        char output;
};

struct portStatistic {
        char portId;
        unsigned long bytesIn;
        unsigned long bytesOut;
        unsigned long errors;
        char state;
        char cableError;
        char errorDist;
};

struct gs105e_settings {
        
        char * password;
        char * model;
        
        char mac[6];
        char ip[4];
        char subnetmask[4];
        char gateway[4];
        
        char * name;
        char * swVersion;
        
        char dhcp;
        
        struct vlan * vlans;
        char vlanType;
        

        
        struct portStatistic portStatistics[5];
        
        struct portSniffing portMonitor;
        
        
};


struct gs105e_discovered {     
        int id;
        char * model;
        
        char mac[6];
        char ip[4];
        char subnetmask[4];
        char gateway[4];
        
        char * name;
        struct gs105e_discovered * next;
};

struct gs105e_discovered * gs105e_devs ;

struct gs105e_settings settings;

void debug(void);
int gs105e_setPassword(char * data);

void gs105e_init(void) ;
void makeHeader(unsigned int queryType);

void gs105e_query (void);
void gs105e_queryAll(void);

int gs105e_addVlan(int vlanId) ;
int gs105e_delVlan(int vlanId) ;
int gs105e_vlanEnable(void);
int gs105e_setVlanMembers(unsigned int vlanId, unsigned int members, unsigned int tagged);
int gs105e_discover(void);

int gs105e_setName(char * data);

int gs105e_dhcpSettings(int action);
int gs105e_cableDiagnostics(int port);
int gs105e_restart(void);
int gs105e_mirrorPorts(int outputPort, int mirrorMask);
#endif
