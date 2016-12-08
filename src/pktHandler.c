#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include "pktHandler.h"

static UdpPkt *udpPkt = NULL;
static int sockFd = -1;
static uint32_t pktCnt = 0;
static struct sockaddr_in servAddr;
static uint16_t pktLength = sizeof(UdpPkt) + BYTES_OF_PKT_BODY;  /* size of struct, plus the real lenght of packet body. 8 + 1308 = 1316 */

inline static void sendUdpPkt();

/* allocate the space for packet, allocating once from the begining and subsequent packets reuse the same space */
boolean allocUdpPkt()
{
    udpPkt =  malloc(pktLength);
    return ( udpPkt != NULL);
}

/* initlize the constant contents inside the packet, initilzing only once */
void initUdpPkt()
{
    if(udpPkt)
    {
        udpPkt->pkt_token = htons(0x2309);  /* convert pkt_token to network order before sending */
        udpPkt->pkt_flag1.bit_0_1 = 3;      /* bit 0 and bit 1 set to '1' */
        udpPkt->pkt_flag1.bit_others = 0;   /* bit 3 to bit 7 set to '0' */
    }
}


void encodeUdpPkt()
{
    if(udpPkt)
    {
        uint8_t flag1;
        udpPkt->pkt_cnt = htonl(pktCnt);  /* convert pkt_cnt to network order before sending */
        udpPkt->pkt_flag1.bit_2 = (pktCnt % 2); /* least significant bit from pkt_cnt */
        memcpy(&flag1, &(udpPkt->pkt_flag1), sizeof(udpPkt->pkt_flag1));
        udpPkt->pkt_flag2 = ~flag1;    /* pkt_flag2 is the reverse order of pkt_flag1 */
        genRandomByteStream(BYTES_OF_PKT_BODY);
    }

}

/* generate random bytes stream for pkt_body, the size is 1316-8 = 1308*/
void genRandomByteStream(int lenOfStream)
{
    if(udpPkt)
    {
        srand (time (NULL));
        int i = 0;
        for (; i < lenOfStream; i++)
        {
            udpPkt->pkt_body[i] = rand () % MAX_UINT8_VALUE;
        }
    }
}

/* start sending upd packets, after sending assigned number of packets, stop sending automatically */
void startSending(const UpdStreamAttr* udpStreamAttr)
{
    sockFd = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);  /* create a upd socket */
    if(sockFd < 0)
    {
        perror("Failed to create socket : ");
        stopSending();
        return;
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(udpStreamAttr->port);
    servAddr.sin_addr.s_addr = inet_addr(udpStreamAttr->ipAddr);

    /* send first udp packet immediately */
    sendUdpPkt();

    /* calculate how many packets should be sent per second */
    double pktPerSec = (double)(udpStreamAttr->pktSpeed * 1000) / (pktLength * 8);   /* pktLength is 1316, unit of speed is kilobits */
    startPktTimer((int)((1000.0 * 1000.0) / pktPerSec));

    while(pktCnt < udpStreamAttr->pktNumber) {}

    stopSending();

}

/* callback function to handle timeout signal */
void pktTimerHandler(int sigNum)
{
    sendUdpPkt();
}

void startPktTimer(int timerLen)
{
    struct sigaction sig;
    struct itimerval pktTimer;

    memset (&sig, 0, sizeof (sig));
    sig.sa_handler = &pktTimerHandler;
    sigaction (SIGALRM, &sig, NULL);

    /* Configure the timer to expire after timerLen useconds */
    pktTimer.it_value.tv_sec = 0;
    pktTimer.it_value.tv_usec = timerLen;
    /* set the same time length for loop timer */
    pktTimer.it_interval.tv_sec = 0;
    pktTimer.it_interval.tv_usec = timerLen;

    setitimer (ITIMER_REAL, &pktTimer, NULL);
}

inline static void sendUdpPkt()
{
    encodeUdpPkt();  /* encode packet before sending */

    pktCnt++;
    traceUdpPkt();

    if(sendto(sockFd, udpPkt, pktLength, 0,
              (const struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Failed to send packet :");
        /* Do not stop sending, keep trying in next round */
    }
}

void stopSending()
{
    if(udpPkt)
    {
        free(udpPkt);
    }
    if(sockFd > 0)
    {
        close(sockFd);
    }
}

void traceUdpPkt()
{
    uint8_t *buff = (uint8_t*)udpPkt;
    if(buff)
    {
        uint16_t i = 0;
        for(; i < pktLength; i++)
        {
            printf("%02x ", buff[i]);
            if((i + 1)%16 == 0)
            {
                printf("\n");
            }
        }
    }
    /* pkt separation line ======================*/
    printf("\n================================================ \n");
}