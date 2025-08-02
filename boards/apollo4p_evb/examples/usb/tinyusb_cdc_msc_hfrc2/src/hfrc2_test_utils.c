//*****************************************************************************
//
//! @file hfrc2_test_utils.c
//!
//! @brief tinyusb hfrc2 utilities
//!
//! @addtogroup usb_examples USB Examples
//!
//! @defgroup tinyusb_cdc_msc_hfrc2 TinyUSB CDC MSC HFRC2 Example
//! @ingroup usb_examples
//! @{
//!
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
#include "hfrc2_test_utils.h"

//
//! used to output HFRC2 clock on pin 33
//
const am_hal_gpio_pincfg_t g_gpio_pincfg_clkout33 =
{
    .GP.cfg_b.uFuncSel         = GPIO_PINCFG33_FNCSEL33_CLKOUT,
    .GP.cfg_b.eGPOutCfg        = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .GP.cfg_b.eDriveStrength   = AM_HAL_GPIO_PIN_DRIVESTRENGTH_1P0X,
    .GP.cfg_b.ePullup          = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.eGPInput         = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .GP.cfg_b.eGPRdZero        = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir          = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
    .GP.cfg_b.uSlewRate        = 0,
    .GP.cfg_b.uNCE             = 0,
    .GP.cfg_b.eCEpol           = 0,
    .GP.cfg_b.ePowerSw         = 0,
    /* The CLKOUT function does not enable input, so force it */
    .GP.cfg_b.eForceInputEn    = 1,
    .GP.cfg_b.eForceOutputEn   = 0,
    .GP.cfg_b.uRsvd_0          = 0,
    .GP.cfg_b.uRsvd_1          = 0,
};

//
//! used to output HFRC2 clock on pin 80
//
const am_hal_gpio_pincfg_t g_gpio_pincfg_clkout80 =
{
    .GP.cfg_b.uFuncSel         = GPIO_PINCFG80_FNCSEL80_CLKOUT,
    .GP.cfg_b.eGPOutCfg        = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .GP.cfg_b.eDriveStrength   = AM_HAL_GPIO_PIN_DRIVESTRENGTH_1P0X,
    .GP.cfg_b.ePullup          = AM_HAL_GPIO_PIN_PULLUP_NONE,
    .GP.cfg_b.eGPInput         = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .GP.cfg_b.eGPRdZero        = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir          = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
    .GP.cfg_b.uSlewRate        = 0,
    .GP.cfg_b.uNCE             = 0,
    .GP.cfg_b.eCEpol           = 0,
    .GP.cfg_b.ePowerSw         = 0,
    /* The CLKOUT function does not enable input, so force it */
    .GP.cfg_b.eForceInputEn    = 1,
    .GP.cfg_b.eForceOutputEn   = 0,
    .GP.cfg_b.uRsvd_0          = 0,
    .GP.cfg_b.uRsvd_1          = 0,
};

//*****************************************************************************
//
// Enable HFRC2 pin output
//
//*****************************************************************************
uint32_t hfrc2_enable_hfrc2_clock_out(am_hal_clkgen_clkout_e eClockOut, uint32_t pinNumber)
{

    am_hal_gpio_pincfg_t pinCfg = g_gpio_pincfg_clkout33 ;
    if ( pinNumber == 80 )
    {
        pinCfg = g_gpio_pincfg_clkout80;
    }
    else
    {
        pinNumber = 33 ;
    }

    if (AM_HAL_STATUS_SUCCESS != am_hal_gpio_pinconfig(pinNumber, pinCfg))
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    CLKGEN->CLKOUT_b.CKSEL = eClockOut;
    CLKGEN->CLKOUT_b.CKEN = 1;

    return AM_HAL_STATUS_SUCCESS;
}
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
