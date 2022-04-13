/*
 *
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "ota_update.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <fat.h>
#include <linux/ctype.h>

#include "ota_local_info.h"
#include "ota_package_info.h"
#include "../update_log.h"
#include "../verify/verify_sign.h"

#define PACKAGE_HEADER_FILE             "infocomp.bin"

#define SIGN_DATA_LEN                   256
#define BASIC_VERSION_OFFSET            76
#define VERSION_LEN                     64
#define COMPONENT_INFO_START            176
#define COMPONENT_INFO_TYPE_SIZE        2
#define COMPONENT_INFO_HEADER_LENGTH    4
#define SINGLE_COMPONENT_INFO_LENGTH    71

#define OHOS_DEFAULT_VERSION "ohos default 1.0"

#pragma pack(1)
struct component_info {
	/* destination address */
	unsigned char addr[16];
	/* reserved: component ID */
	uint16_t id;
	/* reserved: component type */
	unsigned char type;
	/* reserved: component operation type. 0: need upgrade 1: need delete */
	unsigned char oper_type;
	/* reserved: Is Diff component */
	unsigned char is_diff;
	/* reserved: version of component */
	unsigned char version[10];
	/*
	 * reserved: size of component. if it is diff component,
	 * this mean diff compoent image size.
	 */
	unsigned int len;
	/*
	 * reserved: size of component. if it is diff component,
	 * this mean total compoent image size affer diff reduction.
	 */
	unsigned int dst_len;
	unsigned char sha_data[HASH_LEN];
};
#pragma pack()

struct component_infos {
	struct component_info table[OTA_MAX_COMPONENT_NUM];
};

static struct component_infos g_component_infos;
static unsigned int g_component_num;

static int get_ota_header_path(char *path, unsigned int len);
static int get_ota_tag_path(char *path, unsigned int len);
static void unlink_ota_files(const char *path);
static int verify_package_sign(const unsigned char *info_buf,
	unsigned int info_len);
static int ota_hash_check(const unsigned char *load_addr,
	unsigned int max_load_sz);
static int ota_img_hash_check(const unsigned char *load_addr,
	unsigned int max_load_sz, const struct component_info *info);
static bool verify_package_version(const char *pkg_version);
static int str_to_number(const char *str);

int ota_package_verify(const unsigned char *load_addr,
	unsigned int max_load_sz)
{
	char path[OTA_PATH_LEN] = {0};
	long sz;

	if (!load_addr || max_load_sz == 0) {
		UPD_LOGE("load addr or len %u invalid", max_load_sz);
		return -1;
	}

	if (get_ota_header_path(path, sizeof(path)) != 0) {
		UPD_LOGE("get header path fail");
		return -1;
	}

	sz = file_fat_read(path, (void *)load_addr, max_load_sz);
	if (sz <= SINGLE_COMPONENT_INFO_LENGTH) {
		UPD_LOGE("%s: size %ld invalid", path, sz);
		return -1;
	}

	ota_init_var();
	if (parse_ota_info_component(load_addr, sz) != 0) {
		UPD_LOGE("parse fail");
		return -1;
	}

	if (get_ota_tag_path(path, sizeof(path)) != 0) {
		UPD_LOGE("get tag path fail");
		return -1;
	}

	if (!is_component_in_ota_header(path)) {
		UPD_LOGE("ota_flag is not in ota header");
		return -1;
	}

	if (ota_hash_check(load_addr, max_load_sz) != 0) {
		UPD_LOGE("hash check fail");
		return -1;
	}
	return 0;
}

bool is_component_in_ota_header(const char *tag)
{
	char path[OTA_PATH_LEN] = {0};
	unsigned int i;

	if (!tag) {
		UPD_LOGE("tag is null");
		return false;
	}

	for (i = 0; i < g_component_num; i++) {
		if (get_ota_file_path(
			(char *)g_component_infos.table[i].addr,
			path, sizeof(path)) != 0) {
			UPD_LOGE("get package path fail");
			continue;
		}

		if (strcmp(path, tag) == 0)
			return true;
	}
	UPD_LOGE("%s: not exist", tag);
	return false;
}

static bool is_ota_tag_valid(const char *path)
{
	char buf[64] = {0}; /* 32bytes for max in OTA_TAG_FILE */
	const char *info = "package_type:ota";
	const int len = strlen(info);
	long sz = file_fat_read(path, (void *)buf, sizeof(buf));

	if (sz < len) {
		UPD_LOGE("%s: size %ld invalid", path, sz);
		return false;
	}

	if (strncmp(info, buf, len) != 0) {
		UPD_LOGE("%s info invalid", path);
		return false;
	}
	return true;
}

/* package_type:ota */
bool is_ota_update(void)
{
	char path[OTA_PATH_LEN] = {0};

	if (get_ota_tag_path(path, sizeof(path)) != 0) {
		UPD_LOGE("get tag fail");
		return false;
	}

	return is_ota_tag_valid(path);
}

void clear_ota_files(void)
{
	char path[OTA_PATH_LEN] = {0};
	unsigned int i;

	printf("\nupdate: clear unused file:");
	for (i = 0; i < g_component_num; i++) {
		if (get_ota_file_path(
			(char *)g_component_infos.table[i].addr,
			path, sizeof(path)) != 0) {
			UPD_LOGE("get package path fail");
			continue;
		}
		unlink_ota_files(path);
	}

	if (get_ota_tag_path(path, sizeof(path)) == 0 &&
		fat_exists(path) &&
		is_ota_tag_valid(path)) {
		unlink_ota_files(path);
	}

	if (get_ota_header_path(path, sizeof(path)) == 0) {
		unlink_ota_files(path);
	}
}

static void unlink_ota_files(const char *path)
{
	if (!fat_exists(path))
		return;
	(void)fat_unlink(path);
	UPD_LOGI("unlink %s", path);
}

static int get_ota_header_path(char *path, unsigned int len)
{
	if (change_to_ota_path(PACKAGE_HEADER_FILE, path, len) != 0) {
		UPD_LOGE("sprintf %s %d fail", PACKAGE_HEADER_FILE, len);
		return -1;
	}
	return 0;
}

static int get_ota_tag_path(char *path, unsigned int len)
{
	if (change_to_ota_path(OTA_TAG_FILE, path, len) != 0) {
		UPD_LOGE("sprintf %s %d fail", OTA_TAG_FILE, len);
		return -1;
	}
	return 0;
}

void ota_init_var(void)
{
	memset(&g_component_infos, 0, sizeof(g_component_infos));
	g_component_num = 0;
}

static int verify_package_sign(const unsigned char *info_buf,
	unsigned int info_len)
{
	int ret;
	unsigned int header_len;
	unsigned char sign[SIGN_DATA_LEN];
	unsigned char *info_header = NULL;

	if (info_len <= SIGN_DATA_LEN || info_len >  2 * 1024 * 1024) {
		UPD_LOGE("info invalid");
		return -1;
	}

	header_len = info_len - SIGN_DATA_LEN;
	memcpy(sign, info_buf + header_len, SIGN_DATA_LEN);
	info_header = malloc(header_len);
	if (!info_header) {
		UPD_LOGE("malloc fail");
		return -1;
	}
	memcpy(info_header, info_buf, header_len);

	ret = verify_sign(info_header, header_len, sign, sizeof(sign));
	if (ret != 0)
		UPD_LOGE("verify sign fail");
	free(info_header);
	return ret;
}

/* info_component = header + header_signaure */
int parse_ota_info_component(const unsigned char *info_buf,
	unsigned int info_len)
{
	uint16_t all_component_size;
	unsigned int pos;
	unsigned int i;

	if (verify_package_sign(info_buf, info_len) != 0) {
		UPD_LOGE("verify package fail");
		return -1;
	}

	if (!verify_package_version(
		(char *)(info_buf + BASIC_VERSION_OFFSET))) {
		UPD_LOGE("verify version fail");
		return -1;
	}

	memcpy(&all_component_size,
		info_buf + COMPONENT_INFO_START + COMPONENT_INFO_TYPE_SIZE,
		sizeof(all_component_size));
	g_component_num = all_component_size / SINGLE_COMPONENT_INFO_LENGTH;
	if (g_component_num > OTA_MAX_COMPONENT_NUM) {
		UPD_LOGE("image num %d too large", g_component_num);
		return -1;
	}
	pos = COMPONENT_INFO_START + COMPONENT_INFO_HEADER_LENGTH;
	for (i = 0; i < g_component_num; i++) {
		memcpy(g_component_infos.table + i,
			info_buf + pos,
			sizeof(g_component_infos.table[i]));
		pos += SINGLE_COMPONENT_INFO_LENGTH;
		UPD_LOGI("%d, name %s", i + 1, g_component_infos.table[i].addr);
	}
	return 0;
}

static int ota_hash_check(const unsigned char *load_addr,
	unsigned int max_load_sz)
{
	unsigned int i;

	for (i = 0; i < g_component_num; i++) {
		if (ota_img_hash_check(load_addr, max_load_sz,
			&g_component_infos.table[i]) != 0) {
			UPD_LOGE("ota pre-check fail");
			return -1;
		}
	}
	return 0;
}

static int ota_img_hash_check(const unsigned char *load_addr,
	unsigned int max_load_sz, const struct component_info *info)
{
	char path[OTA_PATH_LEN];
	long len;
	unsigned char hash[HASH_LEN] = {0};

	if (get_ota_file_path((char *)info->addr, path,
		sizeof(path)) != 0) {
		UPD_LOGE("get image path fail");
		return -1;
	}

	if (!fat_exists(path)) {
		UPD_LOGE("%s: not exist", path);
		return -1;
	}

	len = file_fat_read(path, (void *)load_addr, max_load_sz);
	if (len <= 0 || info->len != len) {
		UPD_LOGE("file len %ld != info->len %d", len, info->len);
		return -1;
	}

	UPD_LOGI("%s: check hash, file len %ld", path, len);
	if (calc_image_hash(load_addr, len, hash, sizeof(hash)) != 0) {
		UPD_LOGE("get hash fail");
		return -1;
	}

	if (memcmp(hash, info->sha_data, HASH_LEN) != 0) {
		UPD_LOGE("compare fail");
		return -1;
	}

	UPD_LOGI("check hash ok!");
	return 0;
}

static bool verify_package_version(const char *pkg_version)
{
	const char *local_version = get_local_version();

	if (!local_version || strlen(local_version) <= 0 ||
		!pkg_version || strlen(pkg_version) <= 0) {
		UPD_LOGE("version invalid");
		return false;
	}

        if (strcmp(OHOS_DEFAULT_VERSION, local_version) == 0) {
	        UPD_LOGI("default version, verify version ok");
		return true;
       	}

	if (!is_higher_version(pkg_version, local_version)) {
		UPD_LOGE("compare fail, pkg version %s <= local_version %s",
			pkg_version, local_version);
		return false;
	}

	UPD_LOGI("verify version number ok");
	return true;
}

bool is_higher_version(const char *pkg_version, const char *local_version)
{
	char pkg_buf[VERSION_LEN] = {0};
	char local_buf[VERSION_LEN] = {0};
	char *local_head = NULL;
	char *pkg_head = NULL;
	char *local_tail = NULL;
	char *pkg_tail = NULL;
	int pkg_num;
	int local_num;

	if (strcmp(pkg_version, local_version) == 0) {
		UPD_LOGE("verify version number fail, same version");
		return false;
	}

	if (!strcpy(pkg_buf, pkg_version) ||
		!strcpy(local_buf, local_version)) {
		UPD_LOGE("strcpy version number fail");
		return false;
	}

	pkg_head = strstr(pkg_buf, " ");
	local_head = strstr(local_buf, " ");
	if (!pkg_head || !local_head ||
		pkg_head - pkg_buf != local_head - local_buf ||
		strncmp(pkg_buf, local_buf, local_head - local_buf) != 0) {
		UPD_LOGE("verify version number fail, title invalid");
		return false;
	}

	while (pkg_head && local_head) {
		pkg_tail = strstr(pkg_head, ".");
		local_tail = strstr(local_head, ".");
		if ((!pkg_tail && local_tail) ||
			(pkg_tail && !local_tail)) {
			UPD_LOGE("verify version number fail, invalid tags");
			return false;
		}

		pkg_num = str_to_number(pkg_head);
		local_num = str_to_number(local_head);
		if (pkg_num < local_num) {
			UPD_LOGE("verify version number fail, invalid number");
			return false;
		}

		if (pkg_num > local_num)
			return true;

		pkg_head = pkg_tail + 1;
		local_head = local_tail + 1;
	}
	UPD_LOGE("verify version number fail");
	return false;
}

static int str_to_number(const char *str)
{
	int number = 0;
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == ' ')
			continue;
		if (str[i] == '.')
			return number;
		if (isdigit(str[i]))
			number = 10 * number + (str[i] - '0');
	}
	return number;
}
