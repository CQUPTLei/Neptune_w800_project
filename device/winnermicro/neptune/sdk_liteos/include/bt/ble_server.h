#ifndef __BLE_SERVER_H__
#define __BLE_SERVER_H__

extern void ble_server_init();

extern int ble_server_alloc(BleGattService *srvcinfo);

extern int ble_server_free(int server_if);

#endif
