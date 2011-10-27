#ifndef _SPECTRANET_H
#define _SPECTRANET_H
#include "tcp.h"
#include <stdio.h>

#define FALSE 0
#define TRUE 1

extern int check_internet_connection(void);
extern void connect_to_internet(char *username,char *password);

#endif
