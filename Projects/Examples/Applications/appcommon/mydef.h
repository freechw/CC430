#ifndef _MY_DEF_H_
#define _MY_DEF_H_



/**
 * RF packet type
 */
typedef enum {
    RF_CMD_SYNC_REQ = 0,
    RF_CMD_SYNC_REP,
    RF_CMD_DATA_TMP,

    /* always keep it last */
    RF_CMD_END
} rf_cmd_t;


#pragma pack (1)

/* RF packet format: pkt header */
typedef struct {
    uint16_t nodeid;
    uint8_t cmd;
    uint8_t rssi;
} pkt_app_header_t;

/* RF packet format: pkt header + pkt payload */
typedef struct {
    pkt_app_header_t hdr;
    uint8_t data[MAX_APP_PAYLOAD - sizeof(pkt_app_header_t)];
} pkt_app_t;

typedef struct {
    uint32_t fTimeOffset;
    uint32_t fTimeWkup;
} app_msg_t;

#pragma pack ()


extern uint32_t gNextWkup;







#endif //_MY_DEF_H_
// eof...