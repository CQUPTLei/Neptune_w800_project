#ifndef __BT_DEF_H__
#define __BT_DEF_H__
#include <stdio.h>
#include <string.h>
#define OHOS_BD_ADDR_LEN 6
/* Device address */
typedef struct {
    unsigned char addr[OHOS_BD_ADDR_LEN];
} BdAddr;

/* uuid with len */
typedef struct {
    unsigned char uuidLen;
    unsigned char *uuid;
} BtUuid;

typedef enum {
    OHOS_BT_STATUS_SUCCESS,
    OHOS_BT_STATUS_FAIL,
    OHOS_BT_STATUS_NOT_READY,
    OHOS_BT_STATUS_NOMEM,
    OHOS_BT_STATUS_BUSY,
    OHOS_BT_STATUS_DONE,
    OHOS_BT_STATUS_UNSUPPORTED,
    OHOS_BT_STATUS_PARM_INVALID,
    OHOS_BT_STATUS_UNHANDLED,
    OHOS_BT_STATUS_AUTH_FAILURE,
    OHOS_BT_STATUS_RMT_DEV_DOWN,
    OHOS_BT_STATUS_AUTH_REJECTED
} BtStatus;

/* Characteristic Properties */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_BROADCAST 0x01 /* Characteristic is broadcastable */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_READ 0x02 /* Characteristic is readable */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP 0x04 /* Characteristic can be written without response */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_WRITE 0x08 /* Characteristic can be written */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY 0x10 /* Characteristic supports notification */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_INDICATE 0x20 /* Characteristic supports indication */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_SIGNED_WRITE 0x40 /* Characteristic supports write with signature */
#define OHOS_GATT_CHARACTER_PROPERTY_BIT_EXTENDED_PROPERTY 0x80 /* Characteristic has extended properties */

/* Attribute permissions */
#define OHOS_GATT_PERMISSION_READ 0x01 /* read permission */
#define OHOS_GATT_PERMISSION_READ_ENCRYPTED 0x02 /* Allow encrypted read operations */
#define OHOS_GATT_PERMISSION_READ_ENCRYPTED_MITM 0x04 /* Allow reading with man-in-the-middle protection */
#define OHOS_GATT_PERMISSION_WRITE 0x10 /* write permission */
#define OHOS_GATT_PERMISSION_WRITE_ENCRYPTED 0x20 /* Allow encrypted writes */
#define OHOS_GATT_PERMISSION_WRITE_ENCRYPTED_MITM 0x40 /* Allow encrypted writes with man-in-the-middle protection */
#define OHOS_GATT_PERMISSION_WRITE_SIGNED 0x80 /* Allow signed write operations */
#define OHOS_GATT_PERMISSION_WRITE_SIGNED_MITM = 0x100 /* Allow signed write operations with man-in-the-middle protection*/

typedef enum {
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01,
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02,
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03,
} BleAdvFilter;

/* adv data */
typedef struct {
    unsigned short advLength;
    char *advData; /* advertising data */
    unsigned short scanRspLength;
    char *scanRspData; /* scan response data */
} BleConfigAdvData;

/* BLUETOOTH CORE SPECIFICATION Version 5.2|Vol4,Part E, "HCI_LE_Set_Advertising_Parameters" */
typedef enum {
    OHOS_BLE_ADV_IND = 0x00, /* Connectable and scannable undirected advertising (default)*/
    OHOS_BLE_ADV_DIRECT_IND_HIGH = 0x01, /* Connectable high duty cycle directed advertising */
    OHOS_BLE_ADV_SCAN_IND = 0x02, /* Scannable undirected advertising*/
    OHOS_BLE_ADV_NONCONN_IND = 0x03, /* Non connectable undirected advertising */
    OHOS_BLE_ADV_DIRECT_IND_LOW  = 0x04, /* Connectable low duty cycle directed advertising */
} BleAdvType;

typedef struct {
    int minInterval; /* Minimum advertising interval for undirected and low duty cycle directed advertising */
    int maxInterval; /* Maximum advertising interval for undirected and low duty cycle directed advertising*/
    BleAdvType advType;
    unsigned char ownAddrType; /* Ref. Core spec Version 5.2|Vol 4,Part E, HCI_LE_Set_Advertising_Parameters */
    unsigned char peerAddrType;
    BdAddr peerAddr;
    int channelMap; /* channel used bit map, bit[0:2]->[37,38,39]. e.g. 0x01-only 37 used, 0x07-all used */
    BleAdvFilter advFilterPolicy;
    int txPower; /* dbm */
    int duration; /* duration for sending BLE ADV */
} BleAdvParams;

/* Local IO capability, shall be the same value defined in HCI Specification. */
typedef enum {
     OHOS_BLE_IO_CAP_OUT = 0x00, /* DisplayOnly */
     OHOS_BLE_IO_CAP_IO, /* DisplayYesNo */
     OHOS_BLE_IO_CAP_IN, /* KeyboardOnly */
     OHOS_BLE_IO_CAP_NONE, /* NoInputNoOutput */
     OHOS_BLE_IO_CAP_KBDISP, /* Keyboard display */
} BleIoCapMode;

typedef enum {
     OHOS_BLE_AUTH_NO_BOND = 0x00,
     OHOS_BLE_AUTH_BOND,
     OHOS_BLE_AUTH_REQ_MITM,
     OHOS_BLE_AUTH_REQ_SC_ONLY,
     OHOS_BLE_AUTH_REQ_SC_BOND,
     OHOS_BLE_AUTH_REQ_SC_MITM,
     OHOS_BLE_AUTH_REQ_SC_MITM_BOND
} BleAuthReqMode;

typedef enum {
    OHOS_BLE_SEC_NONE = 0x00,
    OHOS_BLE_SEC_ENCRYPT,
    OHOS_BLE_SEC_ENCRYPT_NO_MITM,
    OHOS_BLE_SEC_ENCRYPT_MITM
} BleSecAct;

/* Bluetooth 128-bit UUID */
#define OHOS_BLE_UUID_MAX_LEN 16
typedef struct {
    unsigned char uu[OHOS_BLE_UUID_MAX_LEN];
} BtGattUuid;

/* GATT ID adding instance id tracking to the UUID */
typedef struct {
    BtGattUuid uuid;
    unsigned char instId;
} BtGattId;

/* GATT Service ID also identifies the service type (primary/secondary) */
typedef struct {
    BtGattId id;
    unsigned char isPrimary;
} BtGattSrvcId;

/*
 * Buffer sizes for maximum attribute length and maximum read/write
 * operation buffer size.
 */
#define OHOS_BT_GATT_MAX_ATTR_LEN 600

/* Attribute change notification parameters */
typedef struct {
    unsigned char value[OHOS_BT_GATT_MAX_ATTR_LEN];
    BdAddr bdAddr;
    BtGattSrvcId srvcId;
    BtGattId characterId;
    unsigned short len;
    unsigned char isNotify;
} BtGattNotifyParams;

/* Buffer type for unformatted reads/writes */
typedef struct {
    unsigned char value[OHOS_BT_GATT_MAX_ATTR_LEN];
    unsigned short len;
} BtGattUnformattedValue;

/* Parameters for GATT read operations */
typedef struct {
    BtGattSrvcId srvcId;
    BtGattId characterId;
    BtGattId descriptorId;
    BtGattUnformattedValue value;
    unsigned short valueType;
    unsigned char status;
} BtGattReadParams;

/* Parameters for GATT write operations */
typedef struct {
    BtGattSrvcId srvcId;
    BtGattId characterId;
    BtGattId descriptorId;
    unsigned char status;
} BtGattWriteParams;

typedef struct {
    int reportFormat;
    int numRecords;
    int dataLen;
    unsigned char *reportData;
} BtScanReport;

typedef struct {
    int connId;
    int transId;
    BdAddr *bdAddr;
    int attrHandle;
    int offset;
    bool isLong;
} BtReqReadCbPara;

typedef struct {
    int connId;
    int transId;
    BdAddr *bdAddr;
    int attrHandle;
    int offset;
    int length;
    bool needRsp;
    bool isPrep;
    unsigned char *value;
} BtReqWriteCbPara;

/* BT-GATT Client callback structure. */
typedef void (*registerClientCallback)(int status, int clientId, BtUuid *appUuid);

typedef void (*connectClientCallback)(int connId, int status, int clientId, BdAddr *bdAddr);

typedef void (*disconnectClientCallback)(int connId, int status, int clientId, BdAddr *bdAddr);

typedef void (*searchCompleteCallback)(int connId, int status);

typedef void (*searchResultCallback)(int connId, BtGattSrvcId *srvcId);

typedef void (*getCharacteristicCallback)(int connId, int status, BtGattSrvcId *srvcId,
                                                     BtGattId *characterId, int characterProp);

typedef void (*getDescriptorCallback)(int connId, int status, BtGattSrvcId *srvcId,
                                               BtGattId *characterId, BtGattId *descriptorId);

typedef void (*getIncludedServiceCallback)(int connId, int status, BtGattSrvcId *srvcId,
                                                       BtGattSrvcId *includeSrvcId);

typedef void (*registerNotificationCallback)(int connId, int registered, int status,
                                                          BtGattSrvcId *srvcId, BtGattId *characterId);

typedef void (*notifyCallback)(int connId, BtGattNotifyParams *pData);

typedef void (*readCharacteristicCallback)(int connId, int status, BtGattReadParams *pData);

typedef void (*writeCharacteristicCallback)(int connId, int status, BtGattWriteParams *pData);

typedef void (*executeWriteCallback)(int connId, int status);

typedef void (*readDescriptorCallback)(int connId, int status, BtGattReadParams *pData);

typedef void (*writeDescriptorCallback)(int connId, int status, BtGattReadParams *pData);

typedef void (*configureMtuCallback)(int connId, int status, int mtu);

typedef struct {
    registerClientCallback registerClientCb;
    connectClientCallback openCb;
    disconnectClientCallback closeCb;
    searchCompleteCallback searchCompleteCb;
    searchResultCallback searchResultCb;
    getCharacteristicCallback getCharacteristicCb;
    getDescriptorCallback getDescriptorCb;
    getIncludedServiceCallback getIncludedServiceCb;
    registerNotificationCallback registerNotificationCb;
    notifyCallback notifyCb;
    readCharacteristicCallback readCharacteristicCb;
    writeCharacteristicCallback writeCharacteristicCb;
    readDescriptorCallback readDescriptorCb;
    writeDescriptorCallback writeDescriptorCb;
    executeWriteCallback executeWriteCb;
    configureMtuCallback configureMtuCb;
} BtGattClientCallbacks;

typedef void (*registerServerCallback)(int status, int serverId, BtUuid *appUuid);

typedef void (*connectServerCallback)(int connId, int serverId, BdAddr *bdAddr);

typedef void (*disconnectServerCallback)(int connId, int serverId, BdAddr *bdAddr);

typedef void (*serviceAddCallback)(int status, int serverId, BtUuid *uuid, int srvcHandle);

typedef void (*includeServiceAddCallback)(int status, int serverId, int srvcHandle, int includeSrvcHandle);

typedef void (*characteristicAddCallback)(int status, int serverId, BtUuid *uuid,
                                                     int srvcHandle, int characteristicHandle);

typedef void (*descriptorAddCallback)(int status, int serverId, BtUuid *uuid,
                                               int srvcHandle, int descriptorHandle);

typedef void (*serviceStartCallback)(int status, int serverId, int srvcHandle);

typedef void (*serviceStopCallback)(int status, int serverId, int srvcHandle);

typedef void (*serviceDeleteCallback)(int status, int serverId, int srvcHandle);

typedef void (*requestReadCallback)(BtReqReadCbPara readCbPara);

typedef void (*requestWriteCallback)(BtReqWriteCbPara writeCbPara);

typedef void (*responseConfirmationCallback)(int status, int handle);

typedef void (*indicationSentCallback)(int connId, int status);

typedef void (*mtuChangeCallback)(int connId, int mtu);

typedef struct {
    registerServerCallback registerServerCb;
    connectServerCallback connectServerCb;
    disconnectServerCallback disconnectServerCb;
    serviceAddCallback serviceAddCb;
    includeServiceAddCallback includeServiceAddCb;
    characteristicAddCallback characteristicAddCb;
    descriptorAddCallback descriptorAddCb;
    serviceStartCallback serviceStartCb;
    serviceStopCallback serviceStopCb;
    serviceDeleteCallback serviceDeleteCb;
    requestReadCallback requestReadCb;
    requestWriteCallback requestWriteCb;
    responseConfirmationCallback responseConfirmationCb;
    indicationSentCallback indicationSentCb;
    mtuChangeCallback mtuChangeCb;
} BtGattServerCallbacks;

typedef void (*scanResultCallback)(BdAddr *bda, int rssi, unsigned char *advData);

typedef void (*readRemoteRssiCallback)(int clientId, BdAddr* bdAddr, int rssi, int status);

typedef void (*listenCallback)(int status, int serverId);

typedef void (*scanFilterCfgCallback)(int action, int clientId, int status, int filtType, int avblSpace);

typedef void (*scanFilterParamCallback)(int action, int clientId, int status, int avblSpace);

typedef void (*scanFilterStatusCallback)(int enable, int clientId, int status);

typedef void (*advEnableCallback)(int advId, int status);

typedef void (*advUpdateCallback)(int advId, int status);

typedef void (*advDataCallback)(int advId, int status);

typedef void (*advDisableCallback)(int advId, int status);

typedef void (*congestionCallback)(int connId, bool congested);

typedef void (*batchscanCfgStorageCallback)(int clientId, int status);

typedef void (*batchscanEnableDisableCallback)(int action, int clientId, int status);

typedef void (*batchscanReportsCallback)(int clientId, int status, BtScanReport scanReport);

typedef void (*batchscanThresholdCallback)(int clientId);

typedef void (*trackAdvEventCallback)(int clientId, int filtIndex, int addrType, BdAddr *bda, int advState);

typedef void (*securityRespondCallback)(BdAddr *bdAddr);

typedef struct {
    advEnableCallback advEnableCb; /* start adv */
    advDisableCallback advDisableCb; /* stop adv */
    advDataCallback advDataCb;
    advUpdateCallback advUpdateCb;
    securityRespondCallback securityRespondCb;
    scanResultCallback scanResultCb;
    readRemoteRssiCallback readRemoteRssiCb;
    listenCallback listenCb;
    scanFilterCfgCallback scanFilterCfgCb;
    scanFilterParamCallback scanFilterParamCb;
    scanFilterStatusCallback scanFilterStatusCb;
    congestionCallback congestionCb;
    batchscanCfgStorageCallback batchscanCfgStorageCb;
    batchscanEnableDisableCallback batchscanEnbDisableCb;
    batchscanReportsCallback batchscanReportsCb;
    batchscanThresholdCallback batchscanThresholdCb;
    trackAdvEventCallback trackAdvEventCb;
} BtGattCallbacks;

typedef struct {
    int connectId; /* connection index */
    int status; /* read/write character status */
    int attrHandle; /* attribute handle */
    int valueLen;
    char *value; /* response data from host */
} GattsSendRspParam;

typedef struct {
    int connectId; /* connection index */
    int attrHandle; /* attribute handle */
    int confirm; /* confirm true for indication, false for notify */
    int valueLen; /* response data from host */
    char *value;
} GattsSendIndParam;

/* ==========================================================
 * begin: Not described in the development manual.
 * ==========================================================
 */
typedef struct {
    unsigned char *advData;
    unsigned int advDataLen;
    unsigned char *rspData;
    unsigned int rspDataLen;
} StartAdvRawData;

typedef int (*BleGattServiceRead)(unsigned char *buff, unsigned int *len);

typedef int (*BleGattServiceWrite)(unsigned char *buff, unsigned int len);

typedef int (*BleGattServiceIndicate)(unsigned char *buff, unsigned int len);

typedef enum {
    OHOS_BLE_ATTRIB_TYPE_SERVICE = 0,
    OHOS_BLE_ATTRIB_TYPE_CHAR,
    OHOS_BLE_ATTRIB_TYPE_CHAR_VALUE,
    OHOS_BLE_ATTRIB_TYPE_CHAR_CLIENT_CONFIG,
    OHOS_BLE_ATTRIB_TYPE_CHAR_USER_DESCR,
} BleAttribType;

typedef enum {
    OHOS_UUID_TYPE_NULL = 0,
    OHOS_UUID_TYPE_16_BIT,
    OHOS_UUID_TYPE_32_BIT,
    OHOS_UUID_TYPE_128_BIT,
} UuidType;

typedef struct {
    BleGattServiceRead read;
    BleGattServiceWrite write;
    BleGattServiceIndicate indicate;
} BleGattOperateFunc;

typedef struct {
    BleAttribType attrType;
    unsigned char permission; /* e.g. (GATT_PERMISSION_READ | GATT_PERMISSION_WRITE) */
    UuidType uuidType;
    unsigned char uuid[OHOS_BLE_UUID_MAX_LEN];
    unsigned char *value;
    unsigned char valLen;
    unsigned char properties; /* e.g. (GATT_CHARACTER_PROPERTY_BIT_BROADCAST | GATT_CHARACTER_PROPERTY_BIT_READ) */
    BleGattOperateFunc func;
} BleGattAttr;

typedef struct {
    unsigned int attrNum;
    BleGattAttr *attrList;
} BleGattService;

#endif