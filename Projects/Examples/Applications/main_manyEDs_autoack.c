/*----------------------------------------------------------------------------
 *  Demo Application for SimpliciTI
 *
 *  L. Friedman
 *  Texas Instruments, Inc.
 *---------------------------------------------------------------------------- */

/**********************************************************************************************
 Copyright 2007-2009 Texas Instruments Incorporated. All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights granted under
 the terms of a software license agreement between the user who downloaded the software,
 his/her employer (which must be your employer) and Texas Instruments Incorporated (the
 "License"). You may not use this Software unless you agree to abide by the terms of the
 License. The License limits your use, and you acknowledge, that the Software may not be
 modified, copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio frequency
 transceiver, which is integrated into your product. Other than for the foregoing purpose,
 you may not use, reproduce, copy, prepare derivative works of, modify, distribute,
 perform, display or sell this Software and/or its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 揂S IS�
 WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY
 WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
 IN NO EVENT SHALL TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE
 THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY
 INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST
 DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY
 THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.
 **************************************************************************************************/
#include <stdio.h>
#include "bsp.h"
#include "mrfi.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"

#include "app_remap_led.h"

#include "Si705x.h"
#include "task_scheduler.h"
#include "utils.h"
#include "appcommon/mydef.h"
#include "appcommon/node.h"
#include "appcommon/tmpsnr/tmpsnr.h"

#ifndef APP_AUTO_ACK
#error ERROR: Must define the macro APP_AUTO_ACK for this application.
#endif

/* callback handler */
static uint8_t sCB(linkID_t);
static void linkTo(void);

static linkID_t sLinkID1 = 0;

#define SPIN_ABOUT_A_SECOND   NWK_DELAY(1000)
#define SPIN_ABOUT_A_QUARTER_SECOND   NWK_DELAY(250)

/* How many times to try a Tx and miss an acknowledge before doing a scan */
#define MISSES_IN_A_ROW  2

void main(void) {
    /* init the node */
#if 1
    node_init();
#else
    BSP_Init();
    /* init task pool */
    task_pool_init();
    rtc_init();
    lpw_init();
    soft_initTimers();
#endif

    /* If an on-the-fly device address is generated it must be done before the
     * call to SMPL_Init(). If the address is set here the ROM value will not
     * be used. If SMPL_Init() runs before this IOCTL is used the IOCTL call
     * will not take effect. One shot only. The IOCTL call below is conformal.
     */
#ifdef I_WANT_TO_CHANGE_DEFAULT_ROM_DEVICE_ADDRESS_PSEUDO_CODE
    {
        addr_t lAddr;

        createRandomAddress(&lAddr);
        SMPL_Ioctl(IOCTL_OBJ_ADDR, IOCTL_ACT_SET, &lAddr);
    }
#endif /* I_WANT_TO_CHANGE_DEFAULT_ROM_DEVICE_ADDRESS_PSEUDO_CODE */

    /* Keep trying to join (a side effect of successful initialization) until
     * successful. Toggle LEDS to indicate that joining has not occurred.
     */
    while (SMPL_SUCCESS != SMPL_Init(sCB)) {
        bsp_toggle_red_led();
        bsp_toggle_green_led();
        SPIN_ABOUT_A_QUARTER_SECOND;
        log(LOG_DEBUG, "trying to join a network...");
    }
    /* LEDs on solid to indicate successful join. */
    bsp_turn_on_green_led();
    bsp_turn_on_red_led();

    /* Unconditional link to AP which is listening due to successful join. */
    linkTo();

    while (1) {
        task_scheduler();
    }
}

static void linkTo() {
    /* Keep trying to link... */
    while (SMPL_SUCCESS != SMPL_Link(&sLinkID1)) {
        bsp_toggle_green_led();
        bsp_toggle_red_led();
        SPIN_ABOUT_A_SECOND;
        log(LOG_DEBUG, "trying to link to the AP...");
    }

    /* Turn off LEDs. */
    bsp_turn_off_green_led();
    bsp_turn_off_red_led();

    /* turn on RX. default is RX off. */
    SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_RXON, 0);
    log(LOG_DEBUG, "linked!!!");

//	/* sleep until button press... */
//	SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SLEEP, 0);

//    button = 0;
//	while (1) {
//	    SPIN_ABOUT_A_SECOND;
//
////		/* Send a message when either button pressed */
////		if (BSP_BUTTON1()) {
////			SPIN_ABOUT_A_QUARTER_SECOND; /* debounce... */
////			/* Message to toggle LED 1. */
////			button = 1;
////		} else if (BSP_BUTTON2()) {
////			SPIN_ABOUT_A_QUARTER_SECOND; /* debounce... */
////			/* Message to toggle LED 2. */
////			button = 2;
////		}
//	    button++;
//	    if (button >2) button = 0;#define AWAKE_INTERVAL (5000)

//		if (button) {
//			uint8_t noAck;
//			smplStatus_t rc;
//
//			/* get radio ready...awakens in idle state */
//			SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_AWAKE, 0);
//
//			/* Set TID and designate which LED to toggle */
//			msg[1] = ++sTid;
//			msg[0] = (button == 1) ? 1 : 2;
//			done = 0;
//			while (!done) {
//				noAck = 0;
//
//				/* Try sending message MISSES_IN_A_ROW times looking for ack */
//				for (misses = 0; misses < MISSES_IN_A_ROW; ++misses) {
//					if (SMPL_SUCCESS
//							== (rc = SMPL_SendOpt(sLinkID1, msg, sizeof(msg),
//							SMPL_TXOPTION_ACKREQ))) {
//						/* Message acked. We're done. Toggle LED 1 to indicate ack received. */
//						toggleLED(1);
//						break;
//					}
//					if (SMPL_NO_ACK == rc) {
//						/* Count ack failures. Could also fail becuase of CCA and
//						 * we don't want to scan in this case.
//						 */
//						noAck++;
//					}
//				}
//				if (MISSES_IN_A_ROW == noAck) {
//					/* Message not acked. Toggle LED 2. */
//					toggleLED(2);
//#ifdef FREQUENCY_AGILITY
//					/* Assume we're on the wrong channel so look for channel by
//					 * using the Ping to initiate a scan when it gets no reply. With
//					 * a successful ping try sending the message again. Otherwise,
//					 * for any error we get we will wait until the next button
//					 * press to try again.
//					 */
//					if (SMPL_SUCCESS != SMPL_Ping(sLinkID1)) {
//						done = 1;
//					}
//#else
//					done = 1;
//#endif  /* FREQUENCY_AGILITY */
//				} else {
//					/* Got the ack or we don't care. We're done. */
//					done = 1;
//				}
//			}
//
//			/* radio back to sleep */
//			SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SLEEP, 0);
//		}
//	}
}

static uint8_t sCB(linkID_t lid) {
    if (lid) {
        post_task(tmpsnr_task_readFrame, (uint16_t) lid);
    } else {
        post_task(tmpsnr_task_addDevice, 0);
    }

    /* leave frame to be read by application. */
    return 0;
}
