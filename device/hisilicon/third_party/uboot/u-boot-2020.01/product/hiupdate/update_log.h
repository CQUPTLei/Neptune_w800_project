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
#ifndef HI_UPDATE_LOG_H
#define HI_UPDATE_LOG_H

#include <stdio.h>

#define UPD_LOGI(fmt, args...) printf("update: "fmt"\n", ##args)
#define UPD_LOGE(fmt, args...) printf("update: [Error]"fmt"\n", ##args)

#endif /* HI_UPDATE_LOG_H */
