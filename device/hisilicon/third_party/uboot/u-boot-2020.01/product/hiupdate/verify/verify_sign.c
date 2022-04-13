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
#include "verify_sign.h"

#include <stdio.h>

#include "../update_log.h"
#include "update_rsa.h"
#include "update_hash.h"

int calc_image_hash(const unsigned char *data, unsigned int data_len,
	unsigned char *hash, unsigned int hash_len)
{
	int ret;

	if (!data || !hash || (data_len == 0) || (hash_len < HASH_LEN)) {
		UPD_LOGE("param invalid");
		return -1;
	}

	if (update_sha256_init() != 0) {
		UPD_LOGE("init fail");
		return -1;
	}

	ret = update_sha256_process(data, data_len, hash);
	update_sha256_free();
	if (ret != 0) {
		UPD_LOGE("hash fail");
		return -1;
	}

	return 0;
}

int verify_sign(const unsigned char *data, unsigned int data_len,
	unsigned char *sign, unsigned int sign_len)
{
	int ret;

	if (!data || data_len == 0 || !sign || sign_len == 0) {
		UPD_LOGE("invalid");
		return -1;
	}

	if (update_rsa_init() != 0) {
		UPD_LOGE("init fail");
		return -1;
	}

	ret = update_sign_rsa_verify(data, data_len, sign, sign_len);
	update_rsa_free();

	return ret;
}

