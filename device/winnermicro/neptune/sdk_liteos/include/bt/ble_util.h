#ifndef __BLE_UTIL_H__
#define __BLE_UTIL_H__
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bt_def.h"
#include "wm_bt_def.h"

#define BLE_IF_DBG 1

#ifndef BLE_IF_DBG
#define BLE_IF_DBG 0
#endif

#if BLE_IF_DBG
#define BLE_IF_DEBUG(fmt, ...)  \
    do{\
        if(1) \
            printf("%s(L%d): " fmt, __FUNCTION__, __LINE__,  ## __VA_ARGS__); \
    }while(0)
#define BLE_IF_PRINTF(fmt, ...)  \
    do{\
        if(1) \
            printf(fmt, ## __VA_ARGS__); \
    }while(0)    
#else
#define BLE_IF_DEBUG(param, ...)
#define BLE_IF_PRINTF(param, ...)
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

extern void app_to_btif_uuid(tls_bt_uuid_t *p_dest, BtUuid *p_src);

extern void btif_to_app_uuid(BtUuid *p_dest, tls_bt_uuid_t *p_src);

extern uint16_t btif_uuid128_to_uuid16(tls_bt_uuid_t *uuid);
tls_bt_uuid_t * btif_uuid16_to_uuid128(uint16_t uuid16);

#endif
