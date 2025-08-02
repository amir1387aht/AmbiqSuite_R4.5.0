//*****************************************************************************
//
//! @file pwr_timer_utils.h
//!
//! @brief This will manage the timer for the example
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup pwr_32mhz 32Mhz XTHS and HFRC2 Power Example
//! @ingroup power_examples
//! @{
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

#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#define TMR_TICKS_PER_SECOND 1024

typedef void (*tmr_callback_fcn_t)(void);

//*****************************************************************************
//
//! @brief  Init and start timer interrupt
//!
//! @note The timer is set to use XT/32 timer
//!
//! @param ui32TimerCounts    -- Number of timer counts for interrupt
//! @param pfnCallbackFcnParam  -- Function called from the timer interrupt
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t timer_init(uint32_t ui32TimerCounts, tmr_callback_fcn_t pfnCallbackFcnParam);

//*****************************************************************************
//
//! @brief  Disable timer and timer interrupt
//
//*****************************************************************************
extern void timer_disable(void);


#endif //TIMER_UTILS_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

