#include "shell.h"


unsigned int countTokens(char * data, char * deli) {
        unsigned int n = 0;
        char * lp;
        
        char * da = malloc(sizeof(char) * strlen(data));
        strcpy(da, data);
        
        lp = strtok(da, deli);
        while (lp != NULL) {
                lp = strtok(NULL, deli);
                n++;
        }
        
        return n;
}

unsigned int split(char *** _data, char * text, char * deli) {

        
        
        unsigned int elements = countTokens(text, deli);
        
        *_data = malloc( sizeof(char *) * (elements + 1));
        
        char ** data = (char **)*_data;
        
        char * lp = strtok(text, deli);
        
        unsigned int n = 0;
        
        data[elements - 1] = NULL;
        
        while (lp != NULL) {
                data[n] = (char*)malloc( sizeof(char *) * (strlen(lp) + 1));
                strcpy(data[n], lp);
                data[n][strlen(lp)] = 0;
                lp = strtok(NULL, deli);
                n++;
        }
        return elements;
}

void printIp(char * data) {
        printf("\033[92m%u.%u.%u.%u\033[0m\n",data[0]&0xff, data[1]&0xff, data[2]&0xff, data[3]&0xff);
}
void printError(int errCode) {
        switch (errCode) {
                case 0x00:;
                        printf("\033[92mSuccess!\033[0m\n");
                        gs105e_queryAll();
                        break;
                case -1:;
                        printf("\033[91mSwitch does not answer!\033[0m\n");
                        break;
                case 0x07:;
                        printf("\033[91mWrong Password, try again!\033[0m\n");
                        settings.password = NULL;
                        break;
                default:;
                        printf("\033[91mUups, unknown Error!?\033[0m\n");
                        break;
        }
}

void shell_set(char ** argv, int elem) {

        if (strncmp(argv[1], "name", 4) == 0) {
                if (elem != 3) {
                        printf("set name [name]\n");
                        return;
                }
                printError(gs105e_setName(argv[2]));
        }
}

char * copyString(const char * data) {
        char * ret = (char * )malloc(strlen(data) + 1);
        memcpy(ret, data, strlen(data) + 1);
        ret[strlen(data)] = 0;
        return ret;
}

void password(void) {
        if (settings.password == NULL) {
               printf("\033[91mWarning: As the protocol of the switch wants it that way,"
			" all configuration packets are send as broadcasts."
			" Even though Netgear is 'encrypting' the password, the encryption used,"
			" is one step away from plaintext. To be safe, use a dedicated VLAN"
			" to manage the switch.\033[0m\n");
                settings.password = copyString((char *)getpass("Password: "));
        }
}




int shell (void) {

        char cmd[300];
        int elem = 0;
        char **argv;
        int n;
        
        struct gs105e_discovered * ddev;
        
        char * dev = "nodev";
        
        n = gs105e_discover();
        printf("Discovered \033[92m%i\033[0m devices\n", n);

        ddev = gs105e_devs;

        printf("ID\tName\t\tMAC\t\tIP\n");

        while (ddev != NULL) {
                printf("%i\t%s\t\t%X:%X:%X:%X:%X:%X\t%u.%u.%u.%u\n", ddev->id, ddev->name, ddev->mac[0]&0xff, ddev->mac[1]&0xff, ddev->mac[2]&0xff, ddev->mac[3]&0xff, ddev->mac[4]&0xff, ddev->mac[5]&0xff, ddev->ip[0]&0xff, ddev->ip[1]&0xff, ddev->ip[2]&0xff, ddev->ip[3]&0xff);
                ddev = ddev->next;
        }

        if (n == 1) {
                printf("only one switch, selecting 1\n");

                memcpy(settings.mac, gs105e_devs->mac, 6);
                gs105e_queryAll();
                dev = gs105e_devs->name;
        }
        
        while (1) {
                printf("\033[96mgs (\033[93m%s\033[96m)# \033[0m", dev);
                cmd[0] = 0;
                fflush(0);
                if (fgets(cmd, 300, stdin) == NULL) {   //STRG + D
                                printf("\nExiting ...\n");
                                return 0;
                        
                }
                
                
                
                cmd[strlen(cmd) - 1] = 0;
                
                //Check for an empty command line
                if (strlen(cmd) == 0) {
                        continue;
                }
                
                
                //Check for the exit Command
                if (strncmp(cmd, "exit", 4) == 0 && strlen(cmd) == 4)
                        return 0;
                
                elem = split(&argv, cmd, " ");
                

                if (strncmp(argv[0], "help", 4) == 0) {
                        printf("Available commands: \n");
                        printf("[discover|select|nodev|ip|vlan|port|sys] \n");
                }

                if (strncmp(argv[0], "discover", 8) == 0) {
                        n = gs105e_discover();
                        printf("Discovered \033[92m%i\033[0m devices\n", n);
                        
                        ddev = gs105e_devs;
                        
                        printf("ID\tName\t\tMAC\t\tIP\n");
                        
                        while (ddev != NULL) {
                                printf("%i\t%s\t\t%X:%X:%X:%X:%X:%X\t%u.%u.%u.%u\n", ddev->id, ddev->name, ddev->mac[0]&0xff, ddev->mac[1]&0xff, ddev->mac[2]&0xff, ddev->mac[3]&0xff, ddev->mac[4]&0xff, ddev->mac[5]&0xff, ddev->ip[0]&0xff, ddev->ip[1]&0xff, ddev->ip[2]&0xff, ddev->ip[3]&0xff);
                                ddev = ddev->next;
                        }
                        
                        if (n == 1) {
                                printf("only one switch, selecting 1\n");
                                
                                memcpy(settings.mac, gs105e_devs->mac, 6);
                                gs105e_queryAll();
                                dev = gs105e_devs->name;
                        }
                }
                

                
                if (strncmp(argv[0], "select", 6) == 0 && elem == 2) {
                        n = atoi(argv[1]);
                        if (n == 0){
                                printf("Please select a valid ID\n");
                                continue;
                        }
                        ddev = gs105e_devs;
                        while (ddev != NULL) {
                                if (n == ddev->id)
                                        break;
                                ddev = ddev->next;
                        }
                        
                        if (ddev == NULL){
                                printf("Please select a valid ID\n");
                                continue;
                        }
                        
                        memcpy(settings.mac, ddev->mac, 6);
                        gs105e_queryAll();
                        dev = ddev->name;
                        
                }

                if (strncmp(dev, "nodev", 6) == 0){
                        printf("Discover and select device first!\n");
                        continue;
                }
                                
                if (strncmp(argv[0], "ip", 2) == 0) {
                        shell_ip(argv, elem);
                }
                if (strncmp(argv[0], "vlan", 4) == 0) {
                        shell_vlan(argv, elem);
                }
                if (strncmp(argv[0], "port", 4) == 0) {
                        shell_port(argv, elem);
                }
                
                if (strncmp(argv[0], "sys", 3) == 0) {
                        shell_sys(argv, elem);
                }
        }
}

