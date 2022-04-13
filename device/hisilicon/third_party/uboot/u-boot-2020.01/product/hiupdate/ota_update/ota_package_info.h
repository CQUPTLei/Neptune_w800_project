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
#ifndef HI_OTA_PACKAGE_INFO_H
#define HI_OTA_PACKAGE_INFO_H

#define OTA_ROOT_DIR                    "/update"
#define OTA_TAG_FILE                    "OTA.tag"
#define OTA_PATH_LEN                    32
#define OTA_MAX_COMPONENT_NUM           7

int change_to_ota_path(const char *src, char *ota_path, unsigned int len);
int get_ota_file_path(const char *addr, char *path, unsigned int len);

#endif /* HI_OTA_PACKAGE_INFO_H */
