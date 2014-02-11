#include "shell.h"

void shell_port(char ** argv, int elem) {
        
        int n, i, m, o;
        
        if (elem == 1) {
                printf(" port [show|diag|mirror] \n");
                return;
        }
        if (strncmp(argv[1], "show", 4) == 0) {
                printf("Port\tSpeed\t\tBytes In\t\tBytes Out\n");
                for (n = 0; n< 5; n++) {
                        printf("%i\t%s\t% 8lu\t\t% 8lu\n", n, 
                        (settings.portStatistics[n].state == 0x05 ? "1000 Mbit/s" : (settings.portStatistics[n].state == 0x04 ? "100 Mbit/s" : (settings.portStatistics[n].state == 0x03 ? "10 Mbit/s":  (settings.portStatistics[n].state == 0x00 ? "No Cable": "???")))),
                        settings.portStatistics[n].bytesIn,
                        settings.portStatistics[n].bytesOut
                        );
                }
        } else if (strncmp(argv[1], "diag", 4) == 0) {
                if (elem != 3) {
                        printf("Port needed\n");
                        return;
                }
                
                n = atoi(argv[2]);
                
                if (n < 1 | n > 5) {
                        printf("Invalid Port\n");
                        return;
                }
                
                password();
                i = gs105e_cableDiagnostics((char) n);
                
                printError(i);
                if (i != 0)
                        return;
                if (settings.portStatistics[n].cableError) {
                        printf("\033[91mError detected: %s in %i m\033[0m\n", 
                        (settings.portStatistics[n].cableError == 0x03 ? "short circuit" : (settings.portStatistics[n].cableError == 0x02 ? "Only one pair Connected!": "damaged")),
                        settings.portStatistics[n].errorDist
                        );
                        
                }else{
                        if (settings.portStatistics[n].errorDist == 0 )
                                printf("\033[92mSeems no Cable is connected!?\033[0m\n");
                        else
                                printf("\033[92mCable is fine!\033[0m\n", settings.portStatistics[n].errorDist);
                }
        } else if (strncmp(argv[1], "mirror", 6) == 0) {
                
                if (elem == 3) {
                        if (strncmp(argv[2], "off", 3) == 0) {
                                password();
                                printError(gs105e_mirrorPorts(0, 0));
                                return;
                                
                        }
                }
                
                if (elem < 4) {
                        printf("port mirror output input [input.....]\n");
                        return;
                }
                
                n = atoi(argv[2]);
                
                if (n < 1 | n > 5) {
                        printf("Invalid Port %i\n", n);
                        return;
                }
                
                o = 0;
                
                for (m = 3; m < elem; m++) {
                        i = atoi(argv[m]);
                        if (i < 1 | i > 5) {
                                printf("Invalid Port %i\n", i);
                                return;
                        }
                        
                        if (i == n) {
                                printf("Output can't be input port!\n");
                                return;
                        }
                        
                        o |= (0x80 >> (i - 1));
                }
                password();
                printError(gs105e_mirrorPorts(n, o));
                
        }
}
