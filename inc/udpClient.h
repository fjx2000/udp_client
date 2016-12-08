#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <stdint.h>

typedef uint8_t boolean;
#define TRUE  1
#define FALSE 0

typedef struct UdpStreamAttr
{
    char ipAddr[16];
    int      port;
    int      pktNumber;  /* number of packets to be sent */
    int      pktSpeed;   /* kilobits unit, 1000 equals to 1M bits */
} UpdStreamAttr;

boolean checkCmdLineValidity();
boolean isValidIpAddress(char *ipAddress);

#endif
