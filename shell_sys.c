#include "shell.h"



void shell_sys(char ** argv, int elem) {
        if (elem == 1) {
                printf(" sys [show|name|password|restart] \n");
                return;
        }
        
        if (strncmp(argv[1], "show", 4) == 0) {
                printf("Name: \t\t%s\n", settings.name);
                printf("Model: \t\t%s\n", settings.model);
                printf("SW-Version:\t %s\n", settings.swVersion);
        }else if (strncmp(argv[1], "name", 4) == 0) {
                if (elem != 3) {
                        printf("Please provide a name\n");
                        return;
                }
                
                password();
                printError(gs105e_setName(argv[2]));
                
        }else if (strncmp(argv[1], "password", 4) == 0) {
        
                password();
                
                char * newPwd = copyString(getpass("New Password: "));
                if (strlen(newPwd) == 0) {
                        printf("Password musst be at least one character!\n");
                        return;
                }
                char * newPwdConf = copyString(getpass("New Password: "));
                printf("%p %p\n", settings.password, newPwd);
                if (strncmp(newPwd, newPwdConf, strlen(newPwd)) == 0 && strlen(newPwd) == strlen(newPwdConf)) {
                        int e = gs105e_setPassword(newPwd);
                        printError(e);
                        if (e != 0)
                                return;
                        settings.password = newPwd;
                }else {
                        printf("Passwords do not match!\n");
                }
        }else if (strncmp(argv[1], "restart", 4) == 0) {
                password();
                printError(gs105e_restart());
        }
        
        
}
