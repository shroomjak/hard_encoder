/* Exercise the actual upstream FreeModbus RTU framer, CRC, FC04/FC06 callbacks
 * through a fake serial/timer port. No STM32/ESP hardware is simulated. */
#include <assert.h>
#include <string.h>
#include "mb.h"
#include "mbport.h"
#include "mbcrc.h"
#include "snapshot_registers.h"

static eMBEventType event;
static BOOL have_event, tx_on;
static CHAR incoming;
static UCHAR sent[256];
static unsigned sent_len;
BOOL xMBPortEventInit(void) { have_event = FALSE; return TRUE; }
BOOL xMBPortEventPost(eMBEventType v) { assert(!have_event); event=v; have_event=TRUE; return TRUE; }
BOOL xMBPortEventGet(eMBEventType *v) { if (!have_event) return FALSE; *v=event; have_event=FALSE; return TRUE; }
BOOL xMBPortSerialInit(UCHAR port, ULONG baud, UCHAR bits, eMBParity parity, UCHAR stop)
{ assert(port==1 && baud==115200 && bits==8 && parity==MB_PAR_EVEN && stop==1); return TRUE; }
void vMBPortSerialEnable(BOOL rx, BOOL tx) { (void)rx; tx_on=tx; }
BOOL xMBPortSerialGetByte(CHAR *v) { *v=incoming; return TRUE; }
BOOL xMBPortSerialPutByte(CHAR v) { assert(sent_len<sizeof sent); sent[sent_len++]=(UCHAR)v; return TRUE; }
BOOL xMBPortTimersInit(USHORT t) { assert(t==35); return TRUE; }
void vMBPortTimersEnable(void) {}
void vMBPortTimersDisable(void) {}

static void send_request(UCHAR *p, unsigned n)
{
    USHORT crc=usMBCRC16(p, n);
    p[n++]=(UCHAR)crc; p[n++]=(UCHAR)(crc >> 8);
    for (unsigned i=0; i<n; ++i) { incoming=(CHAR)p[i]; pxMBFrameCBByteReceived(); }
    pxMBPortCBTimerExpired();
    assert(eMBPoll()==MB_ENOERR); /* EV_FRAME_RECEIVED */
    assert(eMBPoll()==MB_ENOERR); /* EV_EXECUTE */
    while (tx_on) pxMBFrameCBTransmitterEmpty();
    assert(eMBPoll()==MB_ENOERR); /* EV_FRAME_SENT */
}
int main(void)
{
    assert(eMBInit(MB_RTU, 1, 1, 115200, MB_PAR_EVEN, 1)==MB_ENOERR);
    assert(eMBEnable()==MB_ENOERR);
    pxMBPortCBTimerExpired();
    assert(eMBPoll()==MB_ENOERR);
    snapshot_publish(90.5f, 12, 0, 0x20, 123);
    UCHAR broadcast[16]={0,6,0,0,0,42};
    send_request(broadcast, 6);
    assert(sent_len==0); /* no reply to unit 0 */
    UCHAR read[16]={1,4,0,0,0,11};
    send_request(read, 6);
    assert(sent_len==27 && sent[0]==1 && sent[1]==4 && sent[2]==22);
    assert(sent[3]==0 && sent[4]==42 && sent[5]==0 && sent[6]==1);
    /* Decode bytes exactly as the master decodes high/low FC04 words. */
    uint32_t angle_udeg = (uint32_t)sent[7] << 24 | (uint32_t)sent[8] << 16
                          | (uint32_t)sent[9] << 8 | sent[10];
    assert(angle_udeg == 90500000U);
    assert(angle_udeg / 1000000U == 90U && angle_udeg % 1000000U == 500000U);
    assert(usMBCRC16(sent, sent_len)==0);
    sent_len=0;
    UCHAR other[16]={2,4,0,0,0,2};
    send_request(other, 6);
    assert(sent_len==0);
    return 0;
}
