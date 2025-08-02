//*****************************************************************************
//
//! @file pwr_gpio_utils.h
//!
//! @brief This will manage GPIO for the example
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup pwr_mspi MSPI Power Example
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
#ifndef PWR_GPIO_UTILS_H
#define PWR_GPIO_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

//
//! Enumerate the debug pin names
//
typedef enum
{
    eDEBUG_PIN_FIRST,   //!< Keep this name it is used in a loop as the first element
    eDEBUG_PIN_RD_LOW_WRT_HI = eDEBUG_PIN_FIRST,
    eDEBUG_PIN_2,
    eDEBUG_PIN_SLEEP,   //!< pin low when in sleep, high when not sleeping
    eMAX_DEBUG_PINS
}
dbg_pin_names_e;

//*****************************************************************************
//
//! @brief      - init a debug GPIO pin
//!
//! @param debug pin number - GPIO debug is enabled when this is true
//!
//! @note       if pin number is invalid (>500), the debug pin is assumed deactivated
//!
//! @return     - Standard hal status
//!
//
//*****************************************************************************
extern uint32_t gpio_debug_init(uint32_t ui32GpioPinNUmber);

//*****************************************************************************
//
//! @brief      - init the debug GPIO pins
//!
//! @param      - pointer to an array of debug pin numbers
//!
//! @return     - Standard hal status
//!
//
//*****************************************************************************
extern uint32_t gpio_dbg_pin_init( uint32_t *pui32DbgPinTable);

#endif //PWR_GPIO_UTILS_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

