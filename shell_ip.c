#include "shell.h"
void shell_ip(char ** argv, int elem) {

        int n;
        if (elem == 1) {
                printf(" ip [show|dhcp|set] \n");
                return;
        }
        
        if (strncmp(argv[1], "show", 2) == 0) {
                
                printf("IP:\t\t");
                printIp(settings.ip);
                printf("DHCP:\t\t%s\n", settings.dhcp == 0x01 ? "ON":"OFF");
                printf("Subnetmask:\t");
                printIp(settings.subnetmask);
                printf("Gateway:\t");
                printIp(settings.gateway);
                printf("\n");
                return;
        }
        if (strncmp(argv[1], "dhcp", 4) == 0) {
                if (elem == 2) {
                        printf("ip dhcp [on|off|renew]\n");
                        printf("         on: Turn on DHCP\n");
                        printf("         off: Turn off DHCP, current IP will be taken\n");
                        printf("         renew: renew IP-Address via DHCP (turns on DHCP too)\n");
                }else {
                        password();
                        if (strncmp(argv[2], "on", 2) == 0) {
                                printError(gs105e_dhcpSettings(DHCP_ON));
                        } else if (strncmp(argv[2], "off", 3) == 0) {
                                printError(gs105e_dhcpSettings(DHCP_OFF));
                        } else if (strncmp(argv[2], "renew", 5) == 0) {
                                printError(gs105e_dhcpSettings(DHCP_RENEW));
                        }
                        
                }
        }
}
