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
#ifndef HI_OTA_UPDATE_H
#define HI_OTA_UPDATE_H

#include <stdbool.h>

int ota_package_verify(const unsigned char *load_addr,
	unsigned int max_load_sz);
bool is_ota_update(void);
bool is_component_in_ota_header(const char *tag);
void clear_ota_files(void);

void ota_init_var(void);
int parse_ota_info_component(const unsigned char *info_buf,
	unsigned int info_len);
bool is_higher_version(const char *pkg_version, const char *local_version);

#endif /* HI_OTA_UPDATE_H */
