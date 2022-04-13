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
#include <string.h>

#include "../../cipher/v2/api/hi_mpi_cipher.h"

#include "../update_log.h"
#include "update_public_key.h"

int update_sign_rsa_verify(const unsigned char *data, unsigned int data_len,
	unsigned char *sign, unsigned int sign_len)
{
	int ret;
	hi_cipher_rsa_verify rsa;
	hi_cipher_verify_data verify_data;

	if (!data || data_len == 0 || !sign || sign_len == 0) {
		UPD_LOGE("invalid");
		return -1;
	}

	memset(&rsa, 0, sizeof(rsa));
	memset(&verify_data, 0, sizeof(hi_cipher_verify_data));

	verify_data.in = data;
	verify_data.in_len = data_len;
	verify_data.sign = sign;
	verify_data.sign_len = sign_len;

	rsa.scheme = HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256;
	rsa.pub_key.n = get_ota_pub_key_N(&rsa.pub_key.n_len);
	rsa.pub_key.e = get_ota_pub_key_E(&rsa.pub_key.e_len);
	if (!rsa.pub_key.n || !rsa.pub_key.e) {
		UPD_LOGE("public key invalid");
		return -1;
	}

	UPD_LOGI("verify sign start...");
	ret = hi_mpi_cipher_rsa_verify(&rsa, &verify_data);
	if (ret != HI_SUCCESS) {
		UPD_LOGE("verify sign fail!");
		return -1;
	}
	UPD_LOGI("verify sign ok");
	return 0;
}

int update_rsa_init(void)
{
	return hi_mpi_cipher_init() == HI_SUCCESS ? 0 : -1;
}

void update_rsa_free(void)
{
	(void)hi_mpi_cipher_deinit();
}
