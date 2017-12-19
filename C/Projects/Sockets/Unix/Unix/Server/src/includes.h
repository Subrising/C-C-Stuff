#ifndef INC_H
#define INC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/utsname.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string.h>

void lsExec(char *, int);
void lsPipes(char *, int);
void multiExec(char *, int);
void multiGet(char *, int);
int make_server_socket();
int process_request(int);
void reap(int);

#endif