#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <unistd.h>
#include "socket.h"

#include "gs105e.h"
#include <stdlib.h>
#include <string.h>

#include <pwd.h>

void shell_ip(char ** argv, int elem);
char * copyString(const char * data);

int shell(void);
void password(void) ;
void printError(int errCode);
void printIp(char * data);
void shell_port(char ** argv, int elem);
void shell_sys(char ** argv, int elem);
void shell_vlan(char ** argv, int elem);

#endif
