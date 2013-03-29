#include <stdio.h>
#include "socket.h"

#include "gs105e.h"
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <pwd.h>
int shellHandler = 0;




int main (int argc, char** argv) {
        int n;
        
        init_socket();
        
        char* device = "eth0";
        if(argc > 1)
                device = argv[1];
        myMac = get_mac(device);
        
        gs105e_init();

        shell();
}





