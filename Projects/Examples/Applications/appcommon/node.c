#include <stdbool.h>
#include "node.h"
#include "bsp.h"
#include "rtc_cc430.h"
#include "lpw_cc430.h"
#include "task_scheduler.h"
#include "soft_timer.h"

#if defined(END_DEVICE)
#include "tmpsnr/tmpsnr.h"

#elif defined(ACCESS_POINT)
#include "wrkstn/wrkstn.h"

#else
#error ERROR: Must define a proper node type.
#endif


/**************************************************
 * PUBLIC FUNCTIONS
 *************************************************/
bool node_init(void)
{
    /* hardware-related initialization */
    BSP_Init();
    rtc_init();
    lpw_init();

    /* software-related initialization */
    task_pool_init();
    soft_initTimers();

    /* register RF msg processors */
#if defined(END_DEVICE)
    tmpsnr_registerMsgProcessor();

#elif defined(ACCESS_POINT)
    wrkstn_registerMsgProcessor();

#endif // defined

    return true;
}
