//*****************************************************************************
//
//! @file pwr_gpio_utils.c
//!
//! @brief This will manage GPIO for the example
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup pwr_iom_spi SPI Power Example
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

//
//! Define max pin number.
//! @note Choose a number greater than MAX_pins of any Apollo4 that will ever exist.
//! At times, value 0xFFFF is used to denote an unused or invalid pin, so a number
//! between the actual device MAX-pin value and 0xFFFF is needed.
//
#define MAX_VALID_PIN_NUMBER 500

#include "pwr_gpio_utils.h"

//
//! pin config for GPIO debug output pins
//
const am_hal_gpio_pincfg_t g_gpio_default_output_cfg =
{
    .GP.cfg_b.uFuncSel         = 3,
    .GP.cfg_b.eGPOutCfg        = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
    .GP.cfg_b.eDriveStrength   = AM_HAL_GPIO_PIN_DRIVESTRENGTH_0P1X,
    .GP.cfg_b.ePullup          = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.eGPInput         = AM_HAL_GPIO_PIN_INPUT_NONE,
    .GP.cfg_b.eGPRdZero        = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir          = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
    .GP.cfg_b.uSlewRate        = 0,
    .GP.cfg_b.uNCE             = 0,
    .GP.cfg_b.eCEpol           = 0,
    .GP.cfg_b.ePowerSw         = 0,
    .GP.cfg_b.eForceInputEn    = 0,
    .GP.cfg_b.eForceOutputEn   = 0,
    .GP.cfg_b.uRsvd_0          = 0,
    .GP.cfg_b.uRsvd_1          = 0,
};


//*****************************************************************************
//
// Init one debug GPIO pin
//
//*****************************************************************************
uint32_t
gpio_debug_init(uint32_t ui32GpioPinNUmber)
{
    //
    // debug pin setup
    //
    if (ui32GpioPinNUmber >= MAX_VALID_PIN_NUMBER)
    {
        //
        // this pin is invalid or unallocated
        // this is not necessarily an error, so
        // return status success
        //
        return AM_HAL_STATUS_SUCCESS;
    }

    return am_hal_gpio_pinconfig(ui32GpioPinNUmber, g_gpio_default_output_cfg);
}


//*****************************************************************************
//
// Init all the debug GPIO pins
//
//*****************************************************************************
uint32_t
gpio_dbg_pin_init(uint32_t *pui32DbgPinTable)
{
    uint32_t ui32stat = 0;

    for (dbg_pin_names_e i = eDEBUG_PIN_FIRST; i < eMAX_DEBUG_PINS; i++)
    {
        ui32stat += gpio_debug_init(pui32DbgPinTable[i]);
    }
    return ui32stat;
}
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
