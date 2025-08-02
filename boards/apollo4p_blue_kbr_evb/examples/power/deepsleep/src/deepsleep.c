//*****************************************************************************
//
//! @file deepsleep.c
//!
//! @brief Example demonstrating how to enter deepsleep.
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup deepsleep Deepsleep Example
//! @ingroup power_examples
//! @{
//!
//! Purpose: This example configures the device to go into a deep sleep mode.
//! Once in sleep mode the device has no ability to wake up. This example is
//! merely to provide the opportunity to measure deepsleep current without
//! interrupts interfering with the measurement.
//! <br>
//!   1) Initialize memory for performance and low power<br>
//!   2) Place device into deepsleep<br>
//!   3) Measure current draw<br>
//!
//! This example uses a button press upon power up to allow the user to retain<br>
//! the minimum amount of DTCM. If not pressed, no DTCM will be retained in<br>
//! deepsleep.<br>
//! <br>
//! The example begins by printing out a banner announcement message through<br>
//! the UART, which is then completely disabled for the remainder of execution.<br>
//! <br>
//! Text is output to the UART at 115,200 BAUD, 8 bit, no parity.<br>
//! Please note that text end-of-line is a newline (LF) character only.<br>
//! Therefore, the UART terminal must be set to simulate a CR/LF.<br>
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "am_devices_button.h"

static bool
button_config_and_check()
{
    uint32_t ui32PinState = 0;

    //
    // Enable the button pin.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, g_AM_BSP_GPIO_BUTTON0);

    //
    // Read the pin state. If the pin is in its normal (unpressed) state, set
    // its "state" counter to zero.
    //
    am_hal_gpio_state_read(AM_BSP_GPIO_BUTTON0, AM_HAL_GPIO_INPUT_READ, &ui32PinState);

    //
    // Disable the button pin.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_BUTTON0, am_hal_gpio_pincfg_disabled);

    //
    // Check to see if the button is "pressed" according to our GPIO reading.
    //
    return (ui32PinState != 1);
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    //
    // This variable will default to button pressed.
    // In the case of not buttons, this will default to not retain DTCM.
    //
    bool bRawButtonPressed = false;

#if AM_BSP_NUM_BUTTONS
    bRawButtonPressed = button_config_and_check();
#endif

    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eCacheCfg    = AM_HAL_PWRCTRL_CACHE_NONE,
        .bRetainCache = false,
#if defined(AM_PART_APOLLO4L)
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_32K,
#else
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_8K,
#endif
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_NONE,
        .bEnableNVM0  = true,
        .bRetainNVM0  = false
    };

    am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg =
    {
        .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDSP     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_NONE
    };

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    am_hal_pwrctrl_dsp_memory_config_t    DSPMemCfg =
    {
        .bEnableICache      = false,
        .bRetainCache       = false,
        .bEnableRAM         = false,
        .bActiveRAM         = false,
        .bRetainRAM         = false
    };
#endif

    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Deepsleep Example\n");

    //
    // To minimize power during the run, disable the UART.
    //
    am_bsp_uart_printf_disable();

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

#if defined (AM_PART_APOLLO4B)
    //
    // Power down Crypto.
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);

    //
    // Disable all peripherals
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL, 0);

#elif defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    //
    // Power down Crypto.
    //
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
#endif

    //
    // Disable XTAL in deepsleep
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);

#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    //
    // For SiP packages, put the BLE Controller in reset.
    //
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN,   am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_DEVICES_BLECTRLR_RESET_PIN

    if (bRawButtonPressed)
    {
#if defined(AM_PART_APOLLO4L)
        McuMemCfg.eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_32K;
#else
        McuMemCfg.eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_8K;
#endif
    }

    //
    // Update memory configuration to minimum.
    //
    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);
#if defined(AM_PART_APOLLO4B)
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP0, &DSPMemCfg);
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP1, &DSPMemCfg);
#elif defined(AM_PART_APOLLO4P)
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP0, &DSPMemCfg);
#else // defined(AM_PART_APOLLO4L)
    //
    // Apollo4 Lite does not have extended memory
    //
#endif

    while (1)
    {
        //
        // Go to Deep Sleep and stay there.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
