#ifndef OEM_AUTH_CONFIG_H
#define OEM_AUTH_CONFIG_H
  
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int OEMReadAuthServerInfo(char* buff, unsigned int len);
char* OEMLoadKitInfos(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
