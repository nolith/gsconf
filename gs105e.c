
#include "gs105e.h"
#include "socket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char passwordSecret[19] = {0x4e , 0x74 , 0x67 , 0x72 , 0x53 , 0x6d , 0x61 , 0x72 , 0x74 , 0x53 , 0x77 , 0x69 , 0x74 , 0x63 , 0x68 , 0x52 , 0x6f , 0x63 , 0x6b};

unsigned char * gs105e_queryData;
unsigned int gsDataLen;

struct vlan * getVlanSetting(unsigned int vlanId) {
        struct vlan *  _vlan = settings.vlans;
        
        while (_vlan != NULL) {
                if (_vlan->id == vlanId)
                        return _vlan;
                _vlan = _vlan->next;
        }

        return NULL;
}

void addVlanSetting(unsigned int vlanId, unsigned char tagged, unsigned char member) {
        struct vlan * old = settings.vlans;
        struct vlan * new = (struct vlan *) malloc (sizeof( struct vlan) );
        new->id = vlanId;
        new->tag = tagged;
        new->members = member;
        new->next = old;
        settings.vlans = new;
}

static void hexDump (unsigned char * data, int len) {
        int n;
        for (n = 0; n < len; n++) {
                if (n % 16 == 0)
                        printf("\n %06X    |",n);
                printf(" %02X", (int)data[n] & 0xFF);
        }
        printf("\n");
}
static void addData(char * data, int len) {
        gsDataLen += len;
        gs105e_queryData = (unsigned char * )realloc(gs105e_queryData, sizeof(unsigned char) * gsDataLen);
        memcpy(&gs105e_queryData[gsDataLen - len], data, len);
}
static void addQuery(int qid) {
        char queryData[] = {qid / 256, qid % 256, 0, 0};
        addData(queryData, 4);
}

void addActData(int actId, int len, char * data) {
        
        char actData[] = {actId / 256, actId % 256, len / 256, len % 256};
        addData(actData, 4);
        
        addData(data, len);
}
void gs105e_init(void) {
        packetId = 0;
        gs105e_devs = NULL;
        gs105e_queryData = NULL;
        gsDataLen = 0;
        settings.vlans = NULL;
}





char newPacketId() {
        packetId ++;
        packetId %= 256;
        return (char)packetId;
}



void makeHeader(unsigned int queryType) {
        int n;



        gs105e_queryData = (unsigned char * )realloc(gs105e_queryData, sizeof(char) * 32);
        gsDataLen = 32;
        
        
        for (n = 0; n < 32; n++)
                gs105e_queryData[n] = 0;
        
        gs105e_queryData[0] = queryType / 256;
        gs105e_queryData[1] = queryType % 256;
        
        memcpy(&gs105e_queryData[8], myMac, 6);
        if (memcmp(settings.mac, "\x00\x00\x00\x00\x00\x00", 6))
                memcpy(&gs105e_queryData[14], settings.mac, 6);
        
        memcpy(&gs105e_queryData[24], "\x4e\x53\x44\x50", 4); //Magic!! :-O
        if (settings.password != NULL && queryType == QR_EXEC) {
		char tmpPassword[strlen(settings.password)];
		for (n = 0; n < strlen(settings.password); n++)
			tmpPassword[n] = passwordSecret[n % 19] ^ settings.password[n]; 
                addActData(GS_PASSWORD, strlen(settings.password), tmpPassword);
        }
        
        
        
}


void debug(void) {
        hexDump(gs105e_queryData, gsDataLen);
}

void emptyBuffer(void) {
        char tmp[2500];
        while (recvBroadcast(tmp) > 0 ) {
                ;;
        }
}

void gs105e_query (void) {
        emptyBuffer();
        gs105e_queryData[23] = newPacketId();
        
        if (gs105e_queryData[gsDataLen - 4] != 0xFF || gs105e_queryData[gsDataLen - 3] != 0xFF || gs105e_queryData[gsDataLen - 2] != 0x00 || gs105e_queryData[gsDataLen - 1] != 0x00)
                addData("\xFF\xFF\x00\x00", 4);
        sendBroadcast(gs105e_queryData, gsDataLen);
        
}

static unsigned long toLong(char * data) {
        int n;
        unsigned long a = 0;
        for (n = 0; n < 8; n++) {
                a <<= 8;
                a |= ((unsigned long)data[n]) & 0xFF;
        }
        return a;
}

static unsigned int toUInt4(char * data) {
        int n;
        unsigned int a = 0;
        for (n = 0; n < 4; n++) {
                a <<= 8;
                a |= ((unsigned int )data[n]) & 0xFF;
        }
        return a;
}

static unsigned int toUInt(char * data) {
        int n;
        unsigned int a = 0;
        for (n = 0; n < 2; n++) {
                a <<= 8;
                a |= ((unsigned int )data[n]) & 0xFF;
        }
        return a;
}

void gs105e_interpret_slice(unsigned int ID, char * data, int len) {
        int mLen , tmp;
        struct portStatistic *p;
        switch (ID) {
                case GS_MODEL:;
                        
                        mLen = (strstr(data, " ") - data) + 1;
                        settings.model = realloc(settings.model, sizeof(char) * mLen);
                        memcpy(settings.model, data, mLen);
                        break;
                case GS_NAME:;
                        mLen= (strchr(data, 0x00) - data) + 1;
                        settings.name = realloc(settings.name, sizeof(char) * mLen);
                        memcpy(settings.name, data, mLen);
                        break;
                case GS_MAC:;
                        memcpy(settings.mac, data, 6);
                        break;
                case GS_IP:;
                        memcpy(settings.ip, data, 4);
                        break;
                case GS_SUBNETMSK:;
                        memcpy(settings.subnetmask, data, 4);
                        break;
                case GS_GATEWAY:;
                        memcpy(settings.gateway, data, 4);
                        break;
                case GS_FIRMWAREVER:;
                        settings.swVersion = realloc(settings.swVersion, sizeof(char) * len);
                        memcpy(settings.swVersion, data, len);
                        break;
                case GS_PACKETSTATISTICS:;
                        tmp = (unsigned int)data[0] - 1;
                        if (tmp > 5)
                                return;
                        p = &settings.portStatistics[tmp];
                        p->portId = tmp + 1;
                        p->bytesIn = toLong(&data[1]);
                        p->bytesOut = toLong(&data[9]);
                        p->errors = toLong(&data[41]);
                        break;
                case GS_VLAN:;
                        unsigned int vlanId = toUInt(data);
                        unsigned char tagged = (unsigned char) data[3];
                        unsigned char memberOf = (unsigned char) data[2];
                        struct vlan * _vlan = getVlanSetting(vlanId);
                        
                        if (_vlan == NULL) {
                                addVlanSetting(vlanId, tagged, memberOf);
                        }else {
                             _vlan->members = memberOf;
                             _vlan->tag = tagged;
                                
                        }
                        
                        
                        break;
                case GS_PORTSTATUS:;
                        unsigned int portId = (unsigned int) data[0];
                        unsigned char state = (unsigned char) data[1];
                        p = &settings.portStatistics[portId - 1];
                        p->state = state;
                        break;
                case GS_PORTDIAG:;
                        p = &settings.portStatistics[(unsigned)data[0]];
                        p->cableError = (char)toUInt4(&data[1]);
                        p->errorDist = (char)toUInt4(&data[5]);
                        break;
                case ACT_DHCP:;
                        settings.dhcp = data[1] & 0x03;
                        break;
                case GS_VLANSETTING:;
                        settings.vlanType = data[0];
                        break;
        }
}

int gs105e__receive(void) {
        char data[2500];
        int len = recvBroadcast(data);
        int n;
        
        unsigned int id;
        unsigned int slLen;
        
        
//        if (len > 0) 
//                hexDump(data, len);
        if (len == -1) {
//                printf("Received No or Invalid Packet\n");
                return -1;
        }
        if (memcmp(&data[8], myMac, 6) || data[0] != 0x01 || (!(data[1] == 0x02 || data[1] == 0x04))) {
                return -1;
        }
        
        for (n = 32; n < len; ) {
               id = (data[n] * 256)  + data[n + 1];
               slLen = (data[n+2] * 256)  + data[n + 3];
               
               gs105e_interpret_slice(id, &data[n + 4], slLen);
               n += slLen + 4;
        }
        return 0;

        
}

int gs105e_receive(void) {
        if (gs105e__receive() == -1) {
                gs105e_query();
                return gs105e_receive();
        }
        return 0;
}


int gs105e_actRecv(void) {
        char data[2500];
        int len = recvBroadcast_tout(data);
        int n;
        
        unsigned int id;
        unsigned int slLen;
        
        
//        if (len > 0) 
//                hexDump(data, len);
        if (len == -1) {
//                printf("Received No or Invalid Packet\n");
                return -1;
        }
        if (memcmp(&data[8], myMac, 6) || data[0] != 0x01 || (!(data[1] == 0x02 || data[1] == 0x04))) {
                gs105e_actRecv();
                return -1;
        }
        
        
        
        if (data[2] == 0) {
                for (n = 32; n < len; ) {
                        id = (data[n] * 256)  + data[n + 1];
                        slLen = (data[n+2] * 256)  + data[n + 3];

                        gs105e_interpret_slice(id, &data[n + 4], slLen);
                        n += slLen + 4;
                }
                return data[2];
        }
	return -1;
}
int gs105e_act() {
        gs105e_query();
        int n = gs105e_actRecv();
        if (n < 0){
                gs105e_query();
                n = gs105e_actRecv();
        }
        return n;
}


int gs105e_addVlan(int vlanId) {
        makeHeader(QR_EXEC);
        
        char data[4] = {vlanId / 256, vlanId % 256, 0, 0};
        
        addActData(ACT_ADDVLAN, 4, data);
 
        return gs105e_act();
        
}

int gs105e_setVlanMembers(unsigned int vlanId, unsigned int members, unsigned int tagged) {
        char data[4] = {0, vlanId & 0xFF, (char)members & 0xFF, (char)tagged & 0xFF};
        makeHeader(QR_EXEC);
        addActData(GS_VLAN, 4, data);
        return gs105e_act();
        
}


int gs105e_delVlan(int vlanId) {
        makeHeader(QR_EXEC);
        
        char data[2] = {vlanId / 256, vlanId % 256};
        
        addActData(ACT_DELVLAN, 2, data);
 
        return gs105e_act();
        
}
int gs105e_restart(void) {
        makeHeader(QR_EXEC);
        
        char data[1] = {1};
        
        addActData(ACT_RESTART, 1, data);
 
        return gs105e_act();
        
}

int gs105e_mirrorPorts(int outputPort, int mirrorMask) {
        makeHeader(QR_EXEC);
        char data[3] = {(char)outputPort, 0, (char) mirrorMask};
        
        addActData(ACT_PORTMIRROR, 3, data);
        return gs105e_act();
}

int gs105e_cableDiagnostics(int port) {
        makeHeader(QR_REQ);
        char data[1] = {port};
        
        addActData(GS_PORTDIAG, 1, data);
        return gs105e_act();
}

int gs105e_setName(char * data) {
        makeHeader(QR_EXEC);
        
        
        addActData(GS_NAME, strlen(data), data);
        return gs105e_act();
}

int gs105e_setPassword(char * data) {
        int n;
	makeHeader(QR_EXEC);
        
        char tmpPasswd[strlen(data)];
	for (n = 0; n < strlen(data); n++)
		tmpPasswd[n] = data[n] ^ passwordSecret[n % 19];
        addActData(GS_NEWPASSWORD, strlen(data), tmpPasswd);
        return gs105e_act();
}

int gs105e_dhcpSettings(int action) {
        makeHeader(QR_EXEC);
        char data[1] = {(char)action};
        
        addActData(ACT_DHCP, 1, data);
        return gs105e_act();
}

int gs105e_vlanEnable(void) {
        makeHeader(QR_EXEC);
        char data[1] = {4};
        
        addActData(GS_VLANSETTING, 1, data);
        return gs105e_act();        
}

void addDiscoveredDevice(int id) {
        
        struct gs105e_discovered * ddev = gs105e_devs;
        
        if (ddev == NULL) {
                ddev = (struct gs105e_discovered *)malloc(sizeof(struct gs105e_discovered));
                gs105e_devs = ddev;
                ddev->next = NULL;
        }else {
                while (ddev != NULL) {
                        if (memcmp(ddev->mac, settings.mac, 6)) {
                                ddev = ddev->next;
                                continue;
                        }
                        
                        break;
                }
                
        }
        
        if (ddev == NULL) {
                
                ddev = gs105e_devs;
                gs105e_devs = (struct gs105e_discovered *)malloc(sizeof(struct gs105e_discovered));
                printf("%p\n", gs105e_devs);
                printf("%p\n", ddev);
                gs105e_devs->next = ddev;
                ddev = gs105e_devs;
                printf("%p\n", ddev);
                
        }        
        
        printf("%p\n", ddev->name);
        ddev->name = (char *)malloc(sizeof(char) * strlen(settings.name));
        printf("DEBUG 1\n");
        memcpy(ddev->name, settings.name, strlen(settings.name) + 1);
        memcpy(ddev->mac, settings.mac, 6);
        
        memcpy(ddev->ip, settings.ip, 4);
        memcpy(ddev->gateway, settings.gateway, 4);
        ddev->model = (char *)malloc(sizeof(char) * strlen(settings.model));
        memcpy(ddev->model, settings.model, strlen(settings.model) + 1);
        
        ddev->id = id;
        
}


int gs105e_discover(void) {
        int n = 0;
        struct gs105e_discovered * ddev = gs105e_devs;
        struct gs105e_discovered * ddev_T;
        while (ddev != NULL) {
                ddev_T = ddev;
                ddev = ddev->next;
                free(ddev_T);
        }
        gs105e_devs = NULL;
        
        makeHeader(QR_REQ);
        addQuery(GS_MODEL);
        addQuery(GS_NAME);
        addQuery(GS_MAC);
        addQuery(GS_IP);
        addQuery(GS_SUBNETMSK);
        addQuery(GS_GATEWAY);
        
        gs105e_query ();
        gs105e_query ();
        while (gs105e__receive() == 0) {
                printf("DEGUB DEVICE DISCOVERED\n");
                addDiscoveredDevice(n + 1);
                
                n++;
        }
        return n;
    
}


void gs105e_queryAll(void) {


        
        if (settings.vlans != NULL) {
                struct vlan * vl = settings.vlans;
                struct vlan * vl_T;
                while (vl != NULL) {
                        vl_T = vl;
                        vl = vl->next;
                        free(vl_T);
                }
                settings.vlans = NULL;
        }
                
        makeHeader(QR_REQ);
        addQuery(GS_MODEL);
        addQuery(GS_NAME);
        addQuery(GS_MAC);
        addQuery(GS_IP);
        addQuery(GS_SUBNETMSK);
        addQuery(GS_GATEWAY);
        addQuery(GS_PACKETSTATISTICS);
        addQuery(GS_FIRMWAREVER);
        addQuery(ACT_DHCP);
        addQuery(GS_PORTSTATUS);        
        addQuery(GS_VLAN);
        addQuery(GS_VLANSETTING);
        
        gs105e_query ();
         gs105e_receive();       
}
