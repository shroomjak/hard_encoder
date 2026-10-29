/*
 * Интеграционный тест FreeModbus RTU и прикладных регистров STM32 (UTF-8).
 * Запуск из корня: sh tests/host/run_rtu_test.sh
 * Требуются sh и cc с поддержкой C99; плата и ESP-IDF/IAR не нужны.
 *
 * Собираем настоящий vendored FreeModbus, CRC16 и обработчики регистров.
 * Подменён только аппаратный порт: запрос вводится побайтно, событие таймера
 * завершает кадр, а байты ответа собираются в sent[].
 * Проверяем broadcast SNAP без ответа, адресный FC04 и порядок байтов угла,
 * CRC ответов, игнорирование чужого ID, запрет broadcast калибровки,
 * адресный запуск/STOP, исключения 03 и 06, чтение LIVE.
 * Зачем: прямого вызова callback недостаточно — проверяем путь от RTU-кадра
 * через адресацию/диспетчер до прикладного действия и ответного кадра.
 * Не проверяются реальные t3.5, USART/IRQ, DE/RE, электрические коллизии,
 * повреждённые входные CRC и все возможные некорректные кадры.
 * Успех — тихое завершение с кодом 0; ошибка — assert или ошибка сборки.
 */
#include <assert.h>
#include <string.h>
#include "mb.h"
#include "mbport.h"
#include "mbcrc.h"
#include "snapshot_registers.h"
extern int start_angk_cal;

static eMBEventType event;
static BOOL have_event, tx_on;
static CHAR incoming;
static UCHAR sent[256];
static unsigned sent_len;
BOOL xMBPortEventInit(void) { have_event = FALSE; return TRUE; }
BOOL xMBPortEventPost(eMBEventType v) { assert(!have_event); event=v; have_event=TRUE; return TRUE; }
BOOL xMBPortEventGet(eMBEventType *v) { if (!have_event) return FALSE; *v=event; have_event=FALSE; return TRUE; }
static USHORT last_t35_50us;
BOOL xMBPortSerialInit(UCHAR port, ULONG baud, UCHAR bits, eMBParity parity, UCHAR stop)
{ assert(port==1 && (baud==115200 || baud==230400) && bits==8 && parity==MB_PAR_EVEN && stop==1); return TRUE; }
void vMBPortSerialEnable(BOOL rx, BOOL tx) { (void)rx; tx_on=tx; }
BOOL xMBPortSerialGetByte(CHAR *v) { *v=incoming; return TRUE; }
BOOL xMBPortSerialPutByte(CHAR v) { assert(sent_len<sizeof sent); sent[sent_len++]=(UCHAR)v; return TRUE; }
BOOL xMBPortTimersInit(USHORT t) { last_t35_50us = t; return TRUE; }
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
    assert(last_t35_50us==35);
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
    /* Calibration is unicast only, even with valid CRC. */
    UCHAR broadcast_cal[16]={0,6,0,1,0,0x14};
    send_request(broadcast_cal, 6);
    assert(sent_len==0 && start_angk_cal==0);
    UCHAR unicast_cal[16]={1,6,0,1,0,0x14};
    send_request(unicast_cal, 6);
    assert(sent_len==8 && sent[1]==6 && start_angk_cal==1);
    sent_len=0;
    UCHAR busy[16]={1,6,0,1,0,0x13};
    send_request(busy, 6);
    assert(sent_len==5 && sent[1]==0x86 && sent[2]==6);
    sent_len=0;
    UCHAR invalid[16]={1,6,0,1,0,0x15};
    send_request(invalid, 6);
    assert(sent_len==5 && sent[1]==0x86 && sent[2]==3);
    sent_len=0;
    UCHAR stop[16]={1,6,0,1,0,0x10};
    send_request(stop, 6);
    assert(sent_len==8 && !start_angk_cal);
    sent_len=0;
    UCHAR live[16]={1,4,1,0,0,3};
    send_request(live, 6);
    assert(sent_len==11 && sent[1]==4 && sent[2]==6);
    assert(sent[3]==0 && sent[4]==0 && sent[6]==1 && sent[7]==0xca && sent[8]==1);
    assert(usMBCRC16(sent, sent_len)==0);
    sent_len=0;

    /* ISR fast-latch test: broadcast SNAP latches at TIM2 t3.5 expiry BEFORE eMBPoll,
       so a frame published between ISR and superloop eMBPoll cannot skew the frozen angle! */
    assert(eMBDisable()==MB_ENOERR);
    assert(eMBInit(MB_RTU, 1, 1, 230400, MB_PAR_EVEN, 1)==MB_ENOERR);
    assert(last_t35_50us==4); /* 200 us = 4 * 50 us at 230400 8E1 */
    assert(eMBEnable()==MB_ENOERR);
    pxMBPortCBTimerExpired();
    assert(eMBPoll()==MB_ENOERR);

    snapshot_publish(135.25f, 54, 0, 0x00, 500);
    UCHAR fast_snap[16]={0,6,0,0,0,99};
    USHORT crc=usMBCRC16(fast_snap, 6);
    fast_snap[6]=(UCHAR)crc; fast_snap[7]=(UCHAR)(crc >> 8);
    for (unsigned i=0; i<8; ++i) { incoming=(CHAR)fast_snap[i]; pxMBFrameCBByteReceived(); }
    pxMBPortCBTimerExpired(); /* TIM2 ISR runs here — must freeze 135.25 deg immediately! */
    snapshot_publish(270.75f, 108, 0, 0x00, 501); /* newer frame arrives BEFORE eMBPoll */
    assert(eMBPoll()==MB_ENOERR);
    assert(eMBPoll()==MB_ENOERR);
    assert(eMBPoll()==MB_ENOERR);

    UCHAR read2[16]={1,4,0,0,0,11};
    send_request(read2, 6);
    uint32_t frozen_udeg = (uint32_t)sent[7] << 24 | (uint32_t)sent[8] << 16
                           | (uint32_t)sent[9] << 8 | sent[10];
    assert(sent[4]==99 && frozen_udeg == 135250000U);
    sent_len=0;

    /* Back-to-back bus traffic without intervening superloop eMBPoll():
       1) Broadcast SNAP(100) arrives -> TIM2 ISR fires,
       2) Foreign Head #2 request + 27-byte reply pass on bus -> TIM2 ISR fires,
       3) Own Head #1 FC04 arrives -> ONLY now main() calls eMBPoll()!
       Note: mock xMBPortEventPost has capacity 1 (assert(!have_event)), so this
       also proves zero stale EV_FRAME_RECEIVED events were queued! */
    snapshot_publish(315.5f, 126, 0, 0x00, 600);
    UCHAR snap100[16]={0,6,0,0,0,100};
    crc=usMBCRC16(snap100, 6);
    snap100[6]=(UCHAR)crc; snap100[7]=(UCHAR)(crc >> 8);
    for (unsigned i=0; i<8; ++i) { incoming=(CHAR)snap100[i]; pxMBFrameCBByteReceived(); }
    pxMBPortCBTimerExpired();

    UCHAR foreign_reply[32]={2,4,22, 0,100, 0,1, 0,0,0,0, 0,1, 0,0, 0,0, 0,0, 0,0, 0,0, 0,80};
    crc=usMBCRC16(foreign_reply, 25);
    foreign_reply[25]=(UCHAR)crc; foreign_reply[26]=(UCHAR)(crc >> 8);
    for (unsigned i=0; i<27; ++i) { incoming=(CHAR)foreign_reply[i]; pxMBFrameCBByteReceived(); }
    pxMBPortCBTimerExpired();

    UCHAR own_read[16]={1,4,0,0,0,11};
    send_request(own_read, 6);
    assert(sent_len==27 && sent[0]==1 && sent[4]==100);
    frozen_udeg = (uint32_t)sent[7] << 24 | (uint32_t)sent[8] << 16
                  | (uint32_t)sent[9] << 8 | sent[10];
    assert(frozen_udeg == 315500000U);
    return 0;
}
