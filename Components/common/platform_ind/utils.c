#include "includes.h"
#include "utils.h"
#include "uart_intfc_cc430.h"

#define MAX_DBG_MSG (64)
#define PREFIX_LOG_DEBUG    "[DBUG] "
#define PREFIX_LOG_INFO     "[INFO] "
#define PREFIX_LOG_WARNING  "[WARN] "
#define PREFIX_LOG_ERROR    "[ERRR] "

static logLevel_t LOG_LEVEL = LOG_DEBUG;

/*************************************************
 * public functions
 ************************************************/
uint8_t log(logLevel_t level, char *fmt, ...) {
    if (level < LOG_LEVEL) {
        return 0;
    }

    char lMsg[MAX_DBG_MSG + 1] = { 0 };
    uint8_t lMsgLen = 0;

    /* msg prefix */
    switch (level) {
    case LOG_DEBUG:
        strncpy(lMsg, PREFIX_LOG_DEBUG, strlen(PREFIX_LOG_DEBUG));
        lMsgLen += strlen(PREFIX_LOG_DEBUG);
        break;
    case LOG_INFO:
        strncpy(lMsg, PREFIX_LOG_INFO, strlen(PREFIX_LOG_INFO));
        lMsgLen += strlen(PREFIX_LOG_INFO);
        break;

    case LOG_WARNING:
        strncpy(lMsg, PREFIX_LOG_WARNING, strlen(PREFIX_LOG_WARNING));
        lMsgLen += strlen(PREFIX_LOG_WARNING);
        break;

    case LOG_ERROR:
        strncpy(lMsg, PREFIX_LOG_ERROR, strlen(PREFIX_LOG_ERROR));
        lMsgLen += strlen(PREFIX_LOG_ERROR);
        break;

    default:
        return 0;
    }

    /* msg content */
#if 0
    va_list ap;
    va_start(ap, fmt);
    int lWritten = vsnprintf(lMsg + lMsgLen, MAX_DBG_MSG - lMsgLen, fmt, ap);
    if (lWritten >= 0) {
        lMsgLen += lWritten;
    }
    va_end(ap);

#else
    char *src = fmt;
    char *dst = lMsg + lMsgLen;

    uint8_t lFsmState = 0;
//    bool lbDone = false;
    va_list ap;
    va_start(ap, fmt);
    while (1) {
        char ch = *src++;

        /* fsm to parse format string */
        switch (lFsmState) {
        case 0: {
            if ('\0' == ch) {
                /**dst = ch;*/
                lFsmState = 1;
            } else if ('%' == ch) {
                lFsmState = 2;
            } else {
                *dst++ = ch;
            }
        }
            break;

        case 1: {
//            lbDone = true;
        }
            break;

        case 2: {
            if ('\0' == ch) {
                /**dst++ = '%';*/
                /**dst = ch;*/
                lFsmState = 1;
            }/* else if ('%' == ch) {
             *dst++ = ch;
             }*/else if ('d' == ch) {
                int32_t val = va_arg(ap, int32_t);
                dst += my_itoa(val, dst, 10);
                lFsmState = 0;
            } else if ('u' == ch) {
                uint32_t val = va_arg(ap, uint32_t);
                dst += my_utoa(val, dst, 10);
                lFsmState = 0;
            } else if ('x' == ch) {
                uint32_t val = va_arg(ap, uint32_t);
                dst += my_utoa(val, dst, 16);
                lFsmState = 0;
            } else {
                *dst++ = ch;
                lFsmState = 0;
            }
        }
            break;

        default:
            break;
        }

        if ((dst - lMsg) >= (MAX_DBG_MSG - 2)) {
            lMsg[MAX_DBG_MSG - 2] = '\r';
            lMsg[MAX_DBG_MSG - 1] = '\n';
            lMsg[MAX_DBG_MSG] = '\0';
            lMsgLen = MAX_DBG_MSG;
            break;
        } else if (1 == lFsmState) {
            /* lFsmState is the terminate state */
            *dst++ = '\r';
            *dst++ = '\n';
            *dst = '\0';
            lMsgLen = dst - lMsg;
            break;
        }

#if 0
        if (lbDone) {
            *dst++ = '\r';
            *dst++ = '\n';
            *dst = '\0';
            lMsgLen = dst - lMsg;
            break;
        } else if ( (dst-lMsg) >= (MAX_DBG_MSG-2) ) {
            lMsg[MAX_DBG_MSG-2] = '\r';
            lMsg[MAX_DBG_MSG-1] = '\n';
            lMsg[MAX_DBG_MSG] = '\0';
            lMsgLen = MAX_DBG_MSG;
            break;
        }
#endif
    }
    va_end(ap);
#endif

    tx_send_wait(lMsg, lMsgLen);
    return lMsgLen;
}

// convert integer to string, according to the base
uint8_t my_itoa(int32_t aInNum, char *aOutStr, uint8_t aInBase) {
    bool bNeg = false;
    uint8_t i, len = 0;
    uint8_t val[10]; // 10 is large enough for 32-bit int

    // sign
    if (aInNum < 0) {
        bNeg = true;
        aInNum = -aInNum;
        len++;
    }

    // get each digit
    do {
        val[len++] = aInNum % aInBase;
        aInNum /= aInBase;
    } while (aInNum > 0);

    // to string
    if (bNeg)
        *aOutStr++ = '-';
    if (10 == aInBase) {
        for (i = len; i > 0; i--) {
            *aOutStr++ = val[i - 1] + '0';
        }
    } else {
        for (i = len; i > 0; i--) {
            switch (val[i - 1]) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                *aOutStr++ = val[i - 1] + '0';
                break;

            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                *aOutStr++ = val[i - 1] - 10 + 'A';
                break;

            }
        }
    }

    return len;
}

uint8_t my_utoa(uint32_t aInNum, char *aOutStr, uint8_t aInBase) {
//    bool bNeg = false;
    uint8_t i, len = 0;
    uint8_t val[10]; // 10 is large enough for 32-bit int

//    // sign
//    if (aInNum < 0) {
//        bNeg = true;
//        aInNum = -aInNum;
//    }

    // get each digit
    do {
        val[len++] = aInNum % aInBase;
        aInNum /= aInBase;
    } while (aInNum > 0);

    // to string
//    if (bNeg) *aOutStr++ = '-';
    if (10 == aInBase) {
        for (i = len; i > 0; i--) {
            *aOutStr++ = val[i - 1] + '0';
        }
    } else {
        for (i = len; i > 0; i--) {
            switch (val[i - 1]) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                *aOutStr++ = val[i - 1] + '0';
                break;

            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                *aOutStr++ = val[i - 1] - 10 + 'A';
                break;

            }
        }
    }

    return len;
}

/************************************************
 * private functions
 ************************************************/

// eof...
