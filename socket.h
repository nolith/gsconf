void init_socket();
unsigned char * get_mac(const char *intf);

unsigned char * myMac;

int recvBroadcast(char * data);
int recvBroadcast_tout(char * data);
int sendBroadcast(unsigned char * data, unsigned int len);

#define PACKET_BUFFER 2500
