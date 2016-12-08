#ifndef PKT_HANDLER_H
#define PKT_HANDLER_H

#include <stdint.h>
#include <time.h>
#include "udpClient.h"

#define BYTES_OF_PKT_BODY        1308  /* 1316 -8 */
#define MAX_UINT8_VALUE           256
#define LOCAL_HOST               "127.0.0.1"

typedef struct
{
    uint32_t pkt_cnt;
    uint16_t pkt_token;

    struct
    {
        uint8_t bit_0_1     : 2;
        uint8_t bit_2       : 1;
        uint8_t bit_others : 5;

    } pkt_flag1;

    uint8_t pkt_flag2;
    uint8_t pkt_body[]; /* empty array for pkt_body, make it flexible for variable length packet */
} UdpPkt;

void encodeUdpPkt();
boolean allocUdpPkt();
void initUdpPkt();
void genRandomByteStream(int lenOfStream);
void startSending(const UpdStreamAttr* udpStreamAttr);
void stopSending();
void traceUdpPkt();
void pktTimerHandler(int sigNum);
void startPktTimer(int timerLen);

#endif