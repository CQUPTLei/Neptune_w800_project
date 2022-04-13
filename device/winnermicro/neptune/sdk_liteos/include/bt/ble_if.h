#ifndef __BLE_HLINK_H__
#define __BLE_HLINK_H__

/*stack releated*/
extern int InitBtStack(void);
extern int EnableBtStack(void);
extern int DisableBtStack(void);

/*gap releated*/
extern int SetDeviceName(const char *name, unsigned int len);
extern int BleSetAdvData(int advId, const BleConfigAdvData *data);
extern int BleStartAdv(int advId, const BleAdvParams *param);
extern int BleStopAdv(int advId);
extern int ReadBtMacAddr(unsigned char *mac, unsigned int len);
extern int BleGattRegisterCallbacks(BtGattCallbacks *func);
extern int BleStartAdvEx(int serverId, const StartAdvRawData rawData, BleAdvParams advParam);

/*smp releated*/
extern int BleSetSecurityIoCap(BleIoCapMode mode);
extern int BleSetSecurityAuthReq(BleAuthReqMode mode);
extern int BleGattSecurityRsp(BdAddr bdAddr, bool accept);

/*gatt server releated*/
extern int BleGattsRegister(BtUuid appUuid);
extern int BleGattsUnRegister(int serverId) ;
extern int BleGattsRegisterCallbacks(BtGattServerCallbacks *func);
extern int BleGattsDisconnect(int serverId, BdAddr bdAddr, int connId);
extern int BleGattsSetEncryption(BdAddr bdAddr, BleSecAct secAct);
extern int BleGattsAddService(int serverId, BtUuid srvcUuid, bool isPrimary, int number);
extern int BleGattsDeleteService(int serverId, int srvcHandle);
extern int BleGattsAddCharacteristic(int serverId, int srvcHandle, BtUuid characUuid,
                                         int properties, int permissions);
extern int BleGattsAddDescriptor(int serverId, int srvcHandle, BtUuid descUuid, int permissions);
extern int BleGattsStartService(int serverId, int srvcHandle);
extern int BleGattsStopService(int serverId, int srvcHandle);
extern int BleGattsSendResponse(int serverId, GattsSendRspParam *param);
extern int BleGattsSendIndication(int serverId, GattsSendIndParam *param);
extern int BleGattsStartServiceEx(int *srvcHandle, BleGattService *srvcInfo);
extern int BleGattsStopServiceEx(int srvcHandle);

/*gatt client releated*/

extern int BleGattcRegister(BtUuid appUuid);
extern int BleGattcUnRegister(int clientId);
extern int BleGattcRegisterCallbacks(BtGattClientCallbacks *func);

#endif
