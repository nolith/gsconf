#include <stdio.h>
#include "socket.h"

#include "gs105e.h"
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <pwd.h>
int shellHandler = 0;




int main (int argc, char** argv) {
        init_socket();

	if (argc == 3 && !strcmp(argv[1], "-i"))
		myMac = get_mac(argv[2]);
	else
		myMac = get_mac("eth0");

        gs105e_init();

        shell();

	return 0;
}





