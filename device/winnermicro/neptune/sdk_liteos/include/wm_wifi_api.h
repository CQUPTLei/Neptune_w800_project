
#ifndef __WM_WIFI_API_H__
#define __WM_WIFI_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * mac transform string.CNcomment:地址转为字符串.CNend
 */
#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef MAC2STR
#define mac2str(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif

#ifndef bit
#define bit(x) (1U << (x))
#endif

/**
 * @ingroup hi_wifi_basic
 *
 * TKIP of cipher mode.CNcomment:加密方式为TKIP.CNend
 */
#define WIFI_CIPHER_TKIP                 bit(3)

/**
 * @ingroup hi_wifi_basic
 *
 * CCMP of cipher mode.CNcomment:加密方式为CCMP.CNend
 */
#define WIFI_CIPHER_CCMP                 bit(4)

/**
 * @ingroup hi_wifi_basic
 *
 * Channel numbers of 2.4G frequency.CNcomment:2.4G频段的信道数量.CNend
 */
#define WIFI_24G_CHANNEL_NUMS 14

/**
 * @ingroup hi_wifi_basic
 *
 * max interiface name length.CNcomment:网络接口名最大长度.CNend
 */
#define WIFI_IFNAME_MAX_SIZE             16

/**
 * @ingroup hi_wifi_basic
 *
 * The minimum timeout of a single reconnection.CNcomment:最小单次重连超时时间.CNend
 */
#define WIFI_MIN_RECONNECT_TIMEOUT   2

/**
 * @ingroup hi_wifi_basic
 *
 * The maximum timeout of a single reconnection, representing an infinite number of loop reconnections.
 * CNcomment:最大单次重连超时时间，表示无限次循环重连.CNend
 */
#define WIFI_MAX_RECONNECT_TIMEOUT   65535

/**
 * @ingroup hi_wifi_basic
 *
 * The minimum auto reconnect interval.CNcomment:最小自动重连间隔时间.CNend
 */
#define WIFI_MIN_RECONNECT_PERIOD    1

/**
 * @ingroup hi_wifi_basic
 *
 * The maximum auto reconnect interval.CNcomment:最大自动重连间隔时间.CNend
 */
#define WIFI_MAX_RECONNECT_PERIOD   65535

/**
 * @ingroup hi_wifi_basic
 *
 * The minmum times of auto reconnect.CNcomment:最小自动重连连接次数.CNend
 */
#define WIFI_MIN_RECONNECT_TIMES    1

/**
 * @ingroup hi_wifi_basic
 *
 * The maximum times of auto reconnect.CNcomment:最大自动重连连接次数.CNend
 */
#define WIFI_MAX_RECONNECT_TIMES   65535

/**
 * @ingroup hi_wifi_basic
 *
 * max scan number of ap.CNcomment:支持扫描ap的最多数目.CNend
 */
#define WIFI_SCAN_AP_LIMIT               32

/**
 * @ingroup hi_wifi_basic
 *
 * length of status buff.CNcomment:获取连接状态字符串的长度.CNend
 */
#define WIFI_STATUS_BUF_LEN_LIMIT        512

/**
 * @ingroup hi_wifi_basic
 *
 * Decimal only WPS pin code length.CNcomment:WPS中十进制pin码长度.CNend
 */
#define WIFI_WPS_PIN_LEN             8

/**
 * @ingroup hi_wifi_basic
 *
 * default max num of station.CNcomment:默认支持的station最大个数.CNend
 */
#define WIFI_DEFAULT_MAX_NUM_STA         6



#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of hi_wifi_api.h */
