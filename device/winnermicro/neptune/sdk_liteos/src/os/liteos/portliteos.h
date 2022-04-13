/*
 ** Copyright (c) 2020, HiHope Community.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are met:
 **
 ** 1. Redistributions of source code must retain the above copyright notice, this
 **    list of conditions and the following disclaimer.
 **
 ** 2. Redistributions in binary form must reproduce the above copyright notice,
 **    this list of conditions and the following disclaimer in the documentation
 **    and/or other materials provided with the distribution.
 **
 ** 3. Neither the name of the copyright holder nor the names of its
 **    contributors may be used to endorse or promote products derived from
 **    this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 ** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 ** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 ** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 ** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 ** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 ** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/

#ifndef PORTLITEOS_H
#define PORTLITEOS_H

#include <csi_core.h>

static inline void portEnableInterrupt( void )
{
    __enable_irq();
}

static inline void portDisableInterrupt( void )
{
    __disable_irq();
}

static inline long GetCurrentPSR (void)
{
    return __get_PSR();
}

static inline long SaveLocalPSR (void)
{
    long flags = __get_PSR();
    __disable_irq();
    return flags;
}

static inline void RestoreLocalPSR (long newMask)
{
    __asm__ __volatile__(
    "mtcr   %0, psr \n"
    :
    :"r" (newMask)
    :"memory"
    );
}

#endif
