/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Display
 * @{
 *
 * @brief Defines driver functions of the display module.
 *
 * The driver functions provided for the GUI include the layer, hardware acceleration, memory, and callback functions.
 *
 * @since 1.0
 * @version 1.0
 */


/**
 * @file display_gralloc.h
 *
 * @brief Declares the driver functions for memory.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef DISPLAY_GRALLOC_H
#define DISPLAY_GRALLOC_H
#include "display_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines pointers to the memory driver functions.
 */
typedef struct {
    /**
     * @brief Allocates memory based on the parameters passed by the GUI.
     *
     * The allocated memory can be classified into shared memory, memory with cache, and memory without cache.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to allocate.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*AllocMem)(GrallocBuffer *buffer);

    /**
     * @brief Releases memory.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to release.
     *
     * @since 1.0
     * @version 1.0
     */
    void (*FreeMem)(GrallocBuffer *buffer);

    /**
     * @brief Maps memory to memory without cache in the process's address space.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    void *(*Mmap)(GrallocBuffer *buffer);

    /**
     * @brief Maps memory to memory with cache in the process's address space.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    void *(*MmapCache)(GrallocBuffer *buffer);

    /**
     * @brief Unmaps memory, that is, removes any mappings in the process's address space.
     *
     * @param buffer Indicates the pointer to the buffer of the memory to unmap.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Unmap)(GrallocBuffer *buffer);

    /**
     * @brief Flushes data from the cache to memory and invalidates the data in the cache.
     *
     * @param buffer Indicates the pointer to the buffer of the cache to flush.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*FlushCache)(GrallocBuffer *buffer);

    /**
     * @brief Flushes data from the cache mapped via {@link Mmap} to memory and invalidates the data in the cache.
     *
     * @param buffer Indicates the pointer to the buffer of the cache to flush.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*FlushMCache)(GrallocBuffer *buffer);
} GrallocFuncs;

/**
 * @brief Initializes the memory module to obtain the pointer to functions for memory operations.
 *
 * @param funcs Indicates the double pointer to functions for memory operations. Memory is allocated automatically when
 * you initiate the memory module initialization, so you can simply use the pointer to gain access to the functions.
 *
 * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t GrallocInitialize(GrallocFuncs **funcs);

/**
 * @brief Deinitializes the memory module to release the memory allocated to the pointer to functions for memory
 * operations.
 *
 * @param funcs Indicates the pointer to functions for memory operations.
 *
 * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t GrallocUninitialize(GrallocFuncs *funcs);

#ifdef __cplusplus
}
#endif
#endif
/** @} */
