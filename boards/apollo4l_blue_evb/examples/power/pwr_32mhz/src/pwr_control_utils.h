//*****************************************************************************
//
//! @file pwr_pwrControl_utils.h
//!
//! @brief Set power mode(s) for the example
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup pwr_32mhz 32Mhz XTHS and HFRC2 Power Example
//! @ingroup power_examples
//! @{
//!
//
//*****************************************************************************
//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef PWRCONTROL_UTILS_H
#define PWRCONTROL_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

//*****************************************************************************
//
//! @brief set the power modes for this example
//
//*****************************************************************************
uint32_t pwrctrl_set_low_power(void);

//*****************************************************************************
//
//! @brief Fill buffer with incrementing bytes ( 0-255_
//!
//! This function fills the given array with incrementing bits for the given size.
//!
//! @param pui8Array Pointer to the array to fill
//! @param i32Size Size of the array in bytes
//
//*****************************************************************************
extern void pwrctrl_fill_buff_incrementing_bytes(uint8_t *pui8Array, int32_t i32Size);
//*****************************************************************************
//
//! @brief Fill buffer with alternating bits 0xA5
//!
//! This function fills the given array with alternating bits for the given size.
//! Each two bytes alternate bits  so this will work well with octal or smaller transfers
//! With a hex transfer, the pattern is the same and output pins won't change
//!
//! @param pui8Array Pointer to the array to fill (longword-aligned)
//! @param ui32TransferBitWidth the (m)spi bit width 1,2,4,8,16
//! @param i32Size Size of the array in bytes (multiple of 4)
//
//*****************************************************************************
extern void pwrctrl_fill_buff_alternating_bits(uint8_t *pui8Array,
                                               uint32_t ui32TransferBitWidth,
                                               int32_t i32Size);

//*****************************************************************************
//
//! @brief fill a buffer with a repeating set of random values
//!
//! @param pui8Array   pointer to destination for random value
//! @param i32Size    number of bytes in destination
//
//*****************************************************************************
void pwrctrl_fill_buff_random(uint8_t *pui8Array, int32_t i32Size);

#endif //PWR_PWRCONTROL_UTILS_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
