#ifndef OEM_AUTH_RESULT_STORAGE_H
#define OEM_AUTH_RESULT_STORAGE_H

#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

bool OEMIsOverTemperatureLimit(void);
bool OEMIsResetFlagExist(void);
int OEMCreateResetFlag(void);
int OEMDeleteResetFlag(void);
bool OEMIsAuthStatusExist(void);
int OEMWriteAuthStatus(const char* data, unsigned int len);
int OEMReadAuthStatus(char* buffer, unsigned int bufferLen);
int OEMDeleteAuthStatus(void);
int OEMGetAuthStatusFileSize(unsigned int* len);
bool OEMIsTicketExist(void);
int OEMWriteTicket(const char* data, unsigned int len);
int OEMReadTicket(char* buffer, unsigned int bufferLen);
int OEMDeleteTicket(void);
int OEMGetTicketFileSize(unsigned int* len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
