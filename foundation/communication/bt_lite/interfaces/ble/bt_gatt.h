
#ifndef __BT_GATT_H__
#define __BT_GATT_H__
/*
 * draft for openAPI.
 */
#include "bt_def.h"

/*
 * @brief Initialize the Bluetooth protocol stack
 * @param[in] void
 * @return 0-success, other-fail
 */
int InitBtStack(void);

/*
 * @brief Bluetooth protocol stack enable
 * @param[in] void
 * @return 0-success, other-fail
 */
int EnableBtStack(void);

/*
 * @brief Bluetooth protocol stack disable
 * @param[in] void
 * @return 0-success, other-fail
 */
int DisableBtStack(void);

/*
 * @brief set this device's name for friendly
 * @param[in] <name> device name
 * @param[in] <len> length
 * @return 0-success, other-fail
 */
int SetDeviceName(const char *name, unsigned int len);

/*
 * @brief gatt client register, callback return clientId
 * @param[in] <appUuid> specified by upper layer
 * @return 0-success, other-fail
 */
int BleGattcRegister(BtUuid appUuid);

/*
 * @brief gatt client deregister
 * @param[in] <clientId> client Id
 * @return 0-success, other-fail
 */
int BleGattcUnRegister(int clientId);

/*
 * @brief set advertising data
 * @param[in] <advId> specified by upper layer
 * @param[in] <data> adv data or scan response
 * @return 0-success, other-fail
 */
int BleSetAdvData(int advId, const BleConfigAdvData *data);

/*
 * @brief start ble advertising
 * @param[in] <advId> specified by upper layer
 * @param[in] <param> ble advertising param list
 * @return 0-success, other-fail
 */
int BleStartAdv(int advId, const BleAdvParams *param);

/*
 * @brief stop ble advertising
 * @param[in] <advId> specified by upper layer
 * @return 0-success, other-fail
 */
int BleStopAdv(int advId);

/*
 * @brief set security IO capability
 * @param[in] <mode> BleIoCapMode
 * @return 0-success, other-fail
 */
int BleSetSecurityIoCap(BleIoCapMode mode);

/*
 * @brief set security authority
 * @param[in] <mode> BleAuthReqMode
 * @return 0-success, other-fail
 */
int BleSetSecurityAuthReq(BleAuthReqMode mode);

/*
 * @brief The device accept or reject the connection initiator. 
 * @param[in] <bdAddr> initiator's address
 * @param[in] <accept> 0-reject, 1-accept
 * @return 0-success, other-fail
 */
int BleGattSecurityRsp(BdAddr bdAddr, bool accept);

/*
 * @brief Cancel connection with remote device
 * @param[in] <serverId> server interface id
 * @param[in] <bdAddr>   remote address
 * @param[in] <connId>   connection index.
 * @return 0-success, other-fail
 */
int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId);

/*
 * @brief Set the encryption level of the data transmission channel during connection
 * @param[in] <bdAddr> remote address
 * @param[in] <secAct> BleSecAct
 * @return 0-success, other-fail
 */
int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct);

/*
 * @brief gatt server register, callback return serverId
 * @param[in] <appUuid> specified by upper layer
 * @return 0-success, other-fail
 */
int BleGattsRegister(BtUuid appUuid);

/*
 * @brief gatt server deregister
 * @param[in] <clientId> server Id
 * @return 0-success, other-fail
 */
int BleGattsUnRegister(int serverId);

/*
 * @brief add service
 * @param[in] <serverId>  server interface id
 * @param[in] <srvcUuid>  service uuid and uuid length
 * @param[in] <isPrimary> is primary or secondary service.
 * @param[in] <number>    service characther attribute number.
 * @return 0-success, other-fail
 */
int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number);

/*
 * @brief delete service
 * @param[in] <serverId>   server interface id
 * @param[in] <srvcHandle> service handle
 * @return 0-success, other-fail
 */
int BleGattsDeleteService(int serverId, int srvcHandle);

/*
 * @brief add characteristic
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle plus offset
 * @param[in] <characUuid>  characteristic uuid and uuid length
 * @param[in] <properties>  e.g. (GATT_CHARACTER_PROPERTY_BIT_BROADCAST | GATT_CHARACTER_PROPERTY_BIT_READ)
 * @param[in] <permissions> e.g. (GATT_PERMISSION_READ | GATT_PERMISSION_WRITE)
 * @return 0-success, other-fail
 */
int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                                         int properties, int permissions);

/*
 * @brief add descriptor
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @param[in] <descUuid>    descriptor uuid and uuid length
 * @param[in] <permissions> e.g. (GATT_PERMISSION_READ | GATT_PERMISSION_WRITE)
 * @return 0-success, other-fail
 */
int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions);

/*
 * @brief start service
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @return 0-success, other-fail
 */
int BleGattsStartService(int serverId, int srvcHandle);

/*
 * @brief start service
 * @param[in] <serverId>    server interface id
 * @param[in] <srvcHandle>  service handle
 * @return 0-success, other-fail
 */
int BleGattsStopService(int serverId, int srvcHandle);

/*
 * @brief send gatt read/write response.
 * @param[in] <serverId> server interface id
 * @param[in] <GattsSendRspParam> response param
 * @return 0-success, other-fail
 */
int BleGattsSendResponse(int serverId, GattsSendRspParam *param);

/*
 * @brief send gatt read response.
 * @param[in] <serverId> server interface id
 * @param[in] <GattsSendRspParam> response param
 * @return 0-success, other-fail
 */
int BleGattsSendIndication(int serverId, GattsSendIndParam *param);

/*
 * @brief read bt mac address
 * @param[in] <mac> mac addr
 * @param[in] <len> addr length
 * @return 0-success, other-fail
 */
int ReadBtMacAddr(unsigned char *mac, unsigned int len);

/*
 * @brief Callback invoked for gatt client function
 * @param[in] <BtGattClientCallbacks> Callback funcs
 * @return 0-success, other-fail
 */
int BleGattcRegisterCallbacks(BtGattClientCallbacks *func);

/*
 * @brief Callback invoked for gatt server function
 * @param[in] <BtGattServerCallbacks> Callback funcs
 * @return 0-success, other-fail
 */
int BleGattsRegisterCallbacks(BtGattServerCallbacks *func);

/*
 * @brief Callback invoked for gatt common function
 * @param[in] <BtGattCallbacks> Callback funcs
 * @return 0-success, other-fail
 */
int BleGattRegisterCallbacks(BtGattCallbacks *func);

/* ==========================================================
 * begin: Not described in the development manual.
 * ==========================================================
 */
int BleStartAdvEx(int serverId, const StartAdvRawData rawData, BleAdvParams advParam);

int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo);

int BleGattsStopServiceEx(int srvcHandle);
/* ==========================================================
 * end: Not described in the development manual.
 * ==========================================================
 */

#endif

