#include "shell.h"

void shell_vlan(char ** argv, int elem) {

        int n, m, o;
        
        if (elem == 1) {
                printf(" vlan [enable|show|add|del|assign|revoke] \n");
                return;
        }
        
        if (strncmp(argv[1], "enable", 6) == 0) {
                password();
                printError(gs105e_vlanEnable());
                return;
        }
        
        if (settings.vlanType != 4) {
                printf("Type vlan enable once to configure vlan\n");
                return;
        }
        
        if (strncmp(argv[1], "show", 4) == 0) {
                struct vlan * vl = settings.vlans;
                
                if (vl == NULL) {
                        printf("NO Vlans\n");
                        return;
                } else
                        printf("Vlans\n");
                
                while (vl != NULL) {
                        printf("%u\t", vl->id & 0xFFFF);
                        for (n = 0; n < 5; n++)
                                printf("%s\t", vl->members & (0x80 >> n) ? (vl->tag & (0x80 >> n) ? "\033[93mT\033[0m" : "\033[92mU\033[0m") : "\033[91m-\033[0m");
                        printf("\n");
                        vl = vl->next;
                }
                
        }else if (strncmp(argv[1], "add", 3) == 0) {
                if (elem != 3) {
                        printf("You need to provide a vlan ID to add!\n");
                        return;
                }
                n = atoi(argv[2]);
                if (n <= 0) {
                        printf("Invalid id!\n");
                        return;
                }
                password();
                printError(gs105e_addVlan(n));
        }else if (strncmp(argv[1], "del", 3) == 0) {
                
                if (elem != 3) {
                        printf("You need to provide a vlan ID to add!\n");
                        return;
                }
                n = atoi(argv[2]);
                
                struct vlan * vl = settings.vlans;
                
                while (vl != NULL) {
                        if (vl->id == n)
                                break;
                        vl = vl->next;
                }
                if (vl == NULL)
                        n = 0;
                
                if (n <= 0) {
                        printf("Invalid id!\n");
                        return;
                }
                password();
                printError(gs105e_delVlan(n));
        }else if (strncmp(argv[1], "assign", 6) == 0) {
                if (elem != 5) {
                        printf("vlan assign [vlan ID] [port ID] [T|U]\n");
                        return;
                }       
                int n = (atoi(argv[2]) & 0xFFF);
                struct vlan * vl = settings.vlans;
                
                while (vl != NULL) {
			printf("DEBUG %i %i\n", vl->id & 0xFFFF, n);
                        if ((vl->id & 0xFFFF) == n)
                                break;
                        vl = vl->next;

                }
                if (vl == NULL)
                        n = 0;
                
                if (n <= 0) {
                        printf("Invalid Vlan ID\n");
                        return;
                }
                
                m = atoi(argv[3]);
                
                if (m < 1 || m > 5) {
                        printf("Invalid Port ID\n");
                        return;
                }         
                o = m;
                m  = (0x80 >> ((m & 0xFF) - 1))  | (int)vl->members;
                printf("%i  %i\n", (int)vl->tag, (unsigned int)(~((unsigned char)m)));
                if (strncmp(argv[4], "T", 1) == 0) {
                        o = (int)vl->tag | (0x80 >> ((o & 0xFF) - 1));
                }else if (strncmp(argv[4], "U", 1) == 0) {
                        o = (int)vl->tag &(~((0x80 >> ((o & 0xFF) - 1))));
                }
                password();
                printError(gs105e_setVlanMembers(n, m, o));
                
        }else if (strncmp(argv[1], "revoke", 6) == 0) {
                if (elem != 4) {
                        printf("vlan revoke [vlan ID] [port ID]\n");
                        return;
                }       
                int n = atoi(argv[2]);
                struct vlan * vl = settings.vlans;
                
                while (vl != NULL) {
                        if ((vl->id & 0xFFFF) == (n & 0xFFFF))
                                break;
                        vl = vl->next;
                }
                if (vl == NULL)
                        n = 0;
                
                if (n <= 0) {
                        printf("Invalid Vlan ID\n");
                        return;
                }
                
                m = atoi(argv[3]);
                
                if (m < 1 || m > 5) {
                        printf("Invalid Port ID\n");
                        return;
                }         
                
                m  = (~(0x80 >> ((m & 0xFF) - 1)))  & (int)vl->members;
                

                o = (int)vl->tag &(~((char)m));

                password();
                printError(gs105e_setVlanMembers(n, m, o));
                
        }
}
