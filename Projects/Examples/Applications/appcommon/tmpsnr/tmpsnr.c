//#ifdef TMPSNR_NODE
#include <stdio.h>

#include "cc430f5137.h"
#include "bsp.h"
#include "mrfi.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "nwk_frame.h"
#include "nwk.h"
#include "rtc_cc430.h"
#include "lpw_cc430.h"
#include "soft_timer.h"
#include "task_scheduler.h"
#include "utils.h"
#include "../mydef.h"
#include "tmpsnr.h"


#define AWAKE_INTERVAL (5000)
#define AWAKE_PERIOD   (AWAKE_INTERVAL / 2)


typedef void (*msg_processor_t)(pkt_app_t*);

static msg_processor_t sMsgProcessor[RF_CMD_END];

//void processMessage(linkID_t aInLid, uint8_t aInMsg, uint8_t aInLen);
void tmpsnr_processSyncReq(pkt_app_t *);
void tmpsnr_processSyncRep(pkt_app_t *);
void tmpsnr_processTmpData(pkt_app_t *);


void tmpsnr_registerMsgProcessor(void)
{
    sMsgProcessor[RF_CMD_SYNC_REQ] = tmpsnr_processSyncReq;
    sMsgProcessor[RF_CMD_SYNC_REP] = tmpsnr_processSyncRep;
    sMsgProcessor[RF_CMD_DATA_TMP] = tmpsnr_processTmpData;
}

void tmpsnr_task_readFrame(uint16_t arg) {
    log(LOG_DEBUG, "enter into readFrame");

    /* Have we received a frame on one of the ED connections?
     * No critical section -- it doesn't really matter much if we miss a poll
     */
    pkt_app_t lPkt = {0};
    uint8_t len = 0;
    linkID_t lid = (linkID_t)arg;

    /* process all frames waiting */
    if (SMPL_SUCCESS != SMPL_Receive(lid, (uint8_t*)&lPkt, &len)) {
        log(LOG_WARNING, "tmpsnr_task_readFrame: failed to read a frame from link %u",
                (uint32_t)lid);
        return;
    }

    if (lPkt.hdr.cmd >= RF_CMD_END) {
        log(LOG_WARNING, "tmpsnr_task_readFrame: unknown msg type %u read from link %u",
                (uint32_t)lPkt.hdr.cmd,
                (uint32_t)lid);
        return;
    }

    sMsgProcessor[lPkt.hdr.cmd](&lPkt);
}

void tmpsnr_task_addDevice(uint16_t arg) {
    log(LOG_DEBUG, "link to an AP");
}

void tmpsnr_task_sleep(uint16_t arg) {
    bsp_turn_off_green_led();
    bsp_turn_off_red_led();
    lpw_enterSleep();
}

void tmpsnr_tmr_sleepISR(uint16_t arg) {
    post_task(tmpsnr_task_sleep, 0);
}

void tmpsnr_tmr_awakeISR(uint16_t arg) {
    lpw_exitSleep();

    /* actions after wakeup */
    post_task(tmpsnr_task_mytask, arg);

    /* awake for 3s */
    soft_setTimer(AWAKE_PERIOD, tmpsnr_tmr_sleepISR, 0);

    /* wake up again after 10s */
    soft_setTimer(AWAKE_INTERVAL, tmpsnr_tmr_awakeISR, 0);
    gNextWkup += AWAKE_INTERVAL;
}

void tmpsnr_task_mytask(uint16_t arg) {
    bsp_turn_on_green_led();
    bsp_turn_on_red_led();

    /* send time stamp to peers */
    log(LOG_DEBUG, "time = %u, next wkup = %u",
            (uint32_t)rtc_getTimeOffset(),
            (uint32_t)gNextWkup);
}

void tmpsnr_processSyncReq(pkt_app_t *aInPkt)
{
    log(LOG_DEBUG, "enter into processSyncReq");
}

void tmpsnr_processSyncRep(pkt_app_t *aInPkt)
{
    log(LOG_DEBUG, "enter into processSyncRep");

    app_msg_t *lMsg = (app_msg_t*)(aInPkt->data);
    log(LOG_DEBUG, "RF_CMD_SYNC_REP: time %u, wkup %u......",
            (uint32_t)(lMsg->fTimeOffset),
            (uint32_t)(lMsg->fTimeWkup));

    /* set rtc time */
    rtc_setTimeOffset(lMsg->fTimeOffset);
    gNextWkup = lMsg->fTimeWkup;

    /* sleep */
    soft_setTimer(lMsg->fTimeWkup - lMsg->fTimeOffset, tmpsnr_tmr_awakeISR, 0);
    post_task(tmpsnr_task_sleep, 0);

    return;
}

void tmpsnr_processTmpData(pkt_app_t *aInPkt)
{
    log(LOG_DEBUG, "enter into processTmpData");
}
//#endif //TMPSNR_NODE
// eof...