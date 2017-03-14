#include "includes.h"
#include "rtc_cc430.h"
#include "lpw_cc430.h"
#include "soft_timer.h"
#include "node.h"

/**
 * keep two uint32_t variables:
 * one for time base and one one time offset.
 *
 * time offset update:
 * 1. in RT1PS interrupt
 * 2. calling from set time
 *
 * time base update:
 * 1. in RTC 32-bit ovf
 * 2. calling from set time
 */
static volatile uint32_t sTimeBase = 0;
static volatile uint32_t sTimeOffset = 0;


void rtc_init(void) {
    sTimeBase = sTimeOffset = 0;

    //WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // reset RTCNT registers
    RTCTIM0 = RTCTIM1 = 0;

    // Setup RTC Timer
    RTCCTL01 = RTCTEVIE + RTCSSEL_2 + RTCTEV_3/*RTCTEV_0*/; // Counter Mode, RTC1PS, 8-bit ovf
    // overflow interrupt enable
    RTCPS0CTL = RT0PSIE + RT0PSDIV_1/*RT0PSDIV_2*/;     // ACLK, /8, start timer
    RTCPS1CTL = RT1PSIE + RT1SSEL_2 + RT1PSDIV_2/*RT1PSDIV_3*/; // out from RT0PS, /16, start timer
}

uint32_t rtc_getTimeBase(void) {
    return sTimeBase;
}

uint32_t rtc_getTimeOffset(void) {
    return sTimeOffset;
}

void rtc_setTimeBase(uint32_t aInBase) {
    BSP_CRITICAL_STATEMENT(sTimeBase = aInBase);
}

void rtc_setTimeOffset(uint32_t aInOffset) {
    bspIState_t lState;

    BSP_ENTER_CRITICAL_SECTION(lState);
    sTimeOffset = aInOffset;
    /* also set RTCNT registers */
    RTCTIM0 = (uint16_t) aInOffset;
    RTCTIM1 = (uint16_t) (aInOffset >> 16);
    BSP_EXIT_CRITICAL_SECTION(lState);
}

#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void) {
    uint32_t lTim0 = 0, lTim1 = 0;
    
    switch (__even_in_range(RTCIV, 16)) {
    case 0:
        break;                         // No interrupts
    case 2:
        break;                         // RTCRDYIFG

    case 4:                            // RTCEVIFG
        /* update time base and reset time offset */
        sTimeBase += 0x400000; // 2^22
        sTimeOffset = 0;
        break;

    case 6:
        break;                         // RTCAIFG
    case 8:
        break;                         // RT0PSIFG

    case 10:                           // RT1PSIFG
        /* update time offset */
        lTim0 = (uint32_t) RTCTIM0;
        lTim1 = ((uint32_t) RTCTIM1) << 16;
        sTimeOffset = lTim1 | lTim0;

//        /* soft timer ISR */
//        soft_ISR(sTimeOffset);
        if (-1 != gWkupTimerSlot && gNextWkup <= sTimeOffset) {
//            gNextWkup =  sTimeOffset + AWAKE_INTERVAL;
            lpw_exitSleep();
        }

        break;

    case 12:
        break;                         // Reserved
    case 14:
        break;                         // Reserved
    case 16:
        break;                         // Reserved
    default:
        break;
    }
}

// eof...
