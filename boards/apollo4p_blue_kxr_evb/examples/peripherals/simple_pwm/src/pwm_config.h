//*****************************************************************************
//
//! @file pwm_config.h
//!
//! @brief PWM config example code
//!
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

#ifndef PWM_CONFIG
#define PWM_CONFIG

#include "am_mcu_apollo.h"

typedef struct
{
    //
    //! pad number
    //! pad is disabled if this value > 0xFFFF
    //
    uint32_t ui32PadNumber1;
    //
    //! pad number
    //! pad is disabled if this value > 0xFFFF
    //
    uint32_t ui32PadNumber2;

    uint32_t ui32TimerNumber;
    //
    //! scaled freq number, this value is the frequency multiplies by the scale factor below
    //
    uint32_t ui32PwmFreq_x128;
    //
    //! frequency scale factor
    //
    uintptr_t ui32FreqFractionalScaling;

    //
    //! percent duty cycle (0-100)
    //
    uint32_t ui32DC_x100;

    //
    // clock mode used for PWM
    //
    am_hal_timer_clock_e ePwmClk;

} pwm_setup_t;


//***************************************************************************************
//
//! @brief will setup pwm
//!
//! @param[in] pwmSetup  pointer to struct containing params
//!
//! @return standard hal status
//
//***************************************************************************************
extern uint32_t pwm_setupPwm( pwm_setup_t *pwmSetup ) ;





#endif //PWM_CONFIG
