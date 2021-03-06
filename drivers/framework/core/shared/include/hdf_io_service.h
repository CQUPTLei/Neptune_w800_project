/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_IO_SERVICE_H
#define HDF_IO_SERVICE_H

#include "hdf_io_service_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEV_NODE_PATH "/dev/hdf/"
#define DEV_PATH "/dev/"
#define DEV_MGR_NODE "dev_mgr"
#define MAX_MODE_SIZE 0777
#define DEV_NODE_PATH_MODE 0755
#define HDF_WRITE_READ _IOWR('b', 1, struct HdfSBuf)
#define HDF_READ_DEV_EVENT _IOR('b', 2, struct HdfSBuf)
#define HDF_LISTEN_EVENT_START _IO('b', 3)
#define HDF_LISTEN_EVENT_STOP _IO('b', 4)
#define HDF_LISTEN_EVENT_WAKEUP _IO('b', 5)
#define HDF_LISTEN_EVENT_EXIT _IO('b', 6)

typedef enum {
    DEVMGR_LOAD_SERVICE = 0,
    DEVMGR_UNLOAD_SERVICE,
    DEVMGR_GET_SERVICE,
} DevMgrCmd;

struct HdfWriteReadBuf {
    int cmdCode;
    size_t writeSize;     // bytes to write
    size_t writeConsumed; // bytes consumed by driver (for ERESTARTSYS)
    uintptr_t writeBuffer;
    size_t readSize; // bytes to read
    size_t readConsumed; // bytes consumed by driver (for ERESTARTSYS)
    uintptr_t readBuffer;
};

struct HdfIoService *HdfIoServicePublish(const char *serviceName, uint32_t mode);
void HdfIoServiceRemove(struct HdfIoService *service);

struct HdfIoService *HdfIoServiceAdapterObtain(const char *serviceName);
void HdfIoServiceAdapterRecycle(struct HdfIoService *service);
struct HdfIoService *HdfIoServiceAdapterPublish(const char *serviceName, uint32_t mode) __attribute__((weak));
void HdfIoServiceAdapterRemove(struct HdfIoService *service) __attribute__((weak));
int32_t HdfLoadDriverByServiceName(const char *serviceName);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* HDF_IO_SERVICE_H */

