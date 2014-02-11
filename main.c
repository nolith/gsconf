#include <stdio.h>
#include "socket.h"

#include "gs105e.h"
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <pwd.h>
int shellHandler = 0;




int main () {
        int n;
        
        init_socket();
        
        myMac = get_mac("eth0");
        
        gs105e_init();

        shell();
}





