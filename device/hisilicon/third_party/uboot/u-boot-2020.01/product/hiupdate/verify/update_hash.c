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
#include "update_hash.h"

#include "../../cipher/v2/api/hi_mpi_cipher.h"
#include "../update_log.h"

int update_sha256_init(void)
{
	return hi_mpi_cipher_init() == HI_SUCCESS ? 0 : -1;
}

int update_sha256_process(const unsigned char *buf, unsigned int len,
	unsigned char *out)
{
	hi_cipher_hash_attr attr;
	hi_handle handle;

	if (!buf || len == 0 || !out) {
		UPD_LOGE("invalid!");
		return -1;
	}

	attr.sha_type = HI_CIPHER_HASH_TYPE_SHA256;
	if (hi_mpi_cipher_hash_init(&attr, &handle) != HI_SUCCESS) {
		UPD_LOGE("hash init fail");
		return -1;
	}

	if (hi_mpi_cipher_hash_update(handle, (unsigned char *)buf, len) !=
		HI_SUCCESS) {
		UPD_LOGE("hash update fail");
		return -1;
	}

	if (hi_mpi_cipher_hash_final(handle, out) != HI_SUCCESS) {
		UPD_LOGE("hash final fail");
		return -1;
	}

	return 0;
}

void update_sha256_free(void)
{
	(void)hi_mpi_cipher_deinit();
}

