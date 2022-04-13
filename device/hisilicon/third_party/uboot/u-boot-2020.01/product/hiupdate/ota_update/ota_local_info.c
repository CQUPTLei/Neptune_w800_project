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

const char *get_local_version(void)
{
#if defined(CONFIG_TARGET_HI3516EV200) || \
	defined(CONFIG_TARGET_HI3516DV300) || \
	defined(CONFIG_TARGET_HI3518EV300)
#define LOCAL_VERSION "ohos default 1.0" /* increase: every release version */
#else
#define LOCAL_VERSION ""
#endif

	return LOCAL_VERSION;
}

