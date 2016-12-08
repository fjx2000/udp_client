#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udpClient.h"
#include "pktHandler.h"

#define VALID_PORT_NUMBER_RANGE(x)     (x > 1000 && x < 5000)
#define VALID_PKT_NUMBER_RANGE(x)      (x > 100 && x < 10000)   /* number of packets to be sent */
#define VALID_PTK_SPEED_RANGE(x)       (x > 100 && x < 10000)  /* kilobits unit, 1000 equals to 1M bits */

struct UdpStreamAttr udpStreamAttr = {"0", 0, 0, 0};

int main(int argc, char **argv)
{
    int result = 0;
    int argument;

    /* parse cmdline, try to identify auguments '-a', '-p', '-n','-s' */
    while( (argument = getopt(argc, argv, "a:p:n:s:")) != -1 )
    {
        switch(argument)
        {
        case 'a':
            strcpy(udpStreamAttr.ipAddr, optarg);
            break;
        case 'p':
            udpStreamAttr.port = atoi(optarg);
            break;
        case 'n':
            udpStreamAttr.pktNumber = atoi(optarg);
            break;
        case 's':
            udpStreamAttr.pktSpeed = atoi(optarg);
            break;
        default:
            printf("CmdLine Format Error, should be like : 'udpClient -a <target IP address> -p <target Port> -n <number of packets to be sent> -s <speed to stream in kilobits> ' ");
            return result;
        }
    }

    if(!checkCmdLineValidity())
    {
        printf("CmdLine Error, missing parameters(-a, -p, -n, -s) or value is out of range. \n");
        return result;
    }

    if(!allocUdpPkt())
    {
        return result;
    }
    initUdpPkt();
    startSending(&udpStreamAttr);

    return result;
}

boolean checkCmdLineValidity()
{
    return(isValidIpAddress(udpStreamAttr.ipAddr) && VALID_PORT_NUMBER_RANGE(udpStreamAttr.port)
           && VALID_PKT_NUMBER_RANGE(udpStreamAttr.pktNumber) && VALID_PTK_SPEED_RANGE(udpStreamAttr.pktSpeed));
}

boolean isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sockAddr;
    int result = inet_pton(AF_INET, ipAddress, &(sockAddr.sin_addr));
    return result != 0;
}
