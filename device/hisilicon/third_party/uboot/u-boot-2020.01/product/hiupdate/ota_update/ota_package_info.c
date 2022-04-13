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
#include "ota_package_info.h"

#include <string.h>

#include <common.h>

#include "../update_log.h"

static const char *get_ota_file_name(const char *addr);

int change_to_ota_path(const char *src, char *ota_path, unsigned int len)
{
	char path[OTA_PATH_LEN] = {0};

	if (!src || !ota_path || len < OTA_PATH_LEN) {
		UPD_LOGE("para invalid");
		return -1;
	}

	if (sprintf(path, "%s/%s", OTA_ROOT_DIR, src) <= 0) {
		UPD_LOGE("sprintf path fail");
		return -1;
	}

	if (!strcpy(ota_path, path)) {
		UPD_LOGE("strcpy path fail");
		return -1;
	}
	return 0;
}

int get_ota_file_path(const char *addr, char *path, unsigned int len)
{
	char *name = NULL;

	if (!addr || !path || len < OTA_PATH_LEN) {
		UPD_LOGE("para invalid");
		return -1;
	}

	name = (char *)get_ota_file_name(addr);
	if (!name) {
		UPD_LOGE("get name fail");
		return -1;
	}

	if (sprintf(path, "%s/%s", OTA_ROOT_DIR, name) <= 0) {
		UPD_LOGE("sprintf path fail");
		return -1;
	}
	return 0;
}

static const char *get_ota_file_name(const char *addr)
{
	struct addr_to_name {
		char addr[16];
		char name[16];
	};
	static struct addr_to_name map[OTA_MAX_COMPONENT_NUM] = {
		{"ota_tag",         OTA_TAG_FILE},
		{"config",          "config"},
		{"bootloader",      "u-boot.bin"},
		{"kernel_A",        "kernel.bin"},
		{"rootfs",          "rootfs.img"},
		{"rootfs_ext4",     "rootfs_ext4.img"},
		{"reserved",        " "},
	};
	int i;

	for (i = 0; i < OTA_MAX_COMPONENT_NUM; i++) {
		if (strcmp(addr, map[i].addr) == 0)
			return map[i].name;
	}
	return NULL;
}

