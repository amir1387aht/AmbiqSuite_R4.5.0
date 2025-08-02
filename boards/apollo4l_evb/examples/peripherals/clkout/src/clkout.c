//*****************************************************************************
//
//! @file clkout.c
//!
//! @brief This example demonstrates the Apollo4 Family CLKOUT feature.
//!
//! @addtogroup peripheral_examples Peripheral Examples
//
//! @defgroup clkout Clock Out Example
//! @ingroup peripheral_examples
//! @{
//!
//! Purpose: This example enables CLKOUT, configures a pin to output the CLKOUT
//! signal and sets up a GPIO interrupt to count the number of low-to-high
//! transitions of CLKOUT.
//! The transitions are counted in the ISR in order to toggle an LED about
//! once per second.
//!
//! A logic analyzer can be attached to the pin specified by CLKOUT_PIN to
//! observe the CLKOUT signal.
//!
//! Additional Information:
//! Printing takes place over the SWO/ITM at 1M Baud.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Local definitions
//
//*****************************************************************************

//*****************************************************************************
//
// CLKOUT pin configuration
//
//*****************************************************************************
#define LED_NUM         0
//
// LED duration, time in seconds
//
#define LED_DURATION    1
#define CLKOUT_PIN      33

//
// Use LFRC, always enabled and will allow deepsleep.
//
#define CLKOUT_FREQHZ   1024
#define CLKOUT_CKSEL    AM_HAL_CLKGEN_CLKOUT_LFRC

#define AM_HAL_GPIO_PINCFG_CLKOUT_33                                          \
    {                                                                         \
        .GP.cfg_b.uFuncSel         = GPIO_PINCFG33_FNCSEL33_CLKOUT,           \
        .GP.cfg_b.eGPOutCfg        = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,          \
        .GP.cfg_b.eDriveStrength   = AM_HAL_GPIO_PIN_DRIVESTRENGTH_1P0X,      \
        .GP.cfg_b.ePullup          = AM_HAL_GPIO_PIN_PULLUP_NONE,             \
        .GP.cfg_b.eGPInput         = AM_HAL_GPIO_PIN_INPUT_ENABLE,            \
        .GP.cfg_b.eGPRdZero        = AM_HAL_GPIO_PIN_RDZERO_READPIN,          \
        .GP.cfg_b.eIntDir          = AM_HAL_GPIO_PIN_INTDIR_LO2HI,            \
        .GP.cfg_b.uSlewRate        = 0,                                       \
        .GP.cfg_b.uNCE             = 0,                                       \
        .GP.cfg_b.eCEpol           = 0,                                       \
        .GP.cfg_b.ePowerSw         = 0,                                       \
        /* The CLKOUT function does not enable input, so force it */          \
        .GP.cfg_b.eForceInputEn    = 1,                                       \
        .GP.cfg_b.eForceOutputEn   = 0,                                       \
        .GP.cfg_b.uRsvd_0          = 0,                                       \
        .GP.cfg_b.uRsvd_1          = 0,                                       \
    }

const am_hal_gpio_pincfg_t am_hal_gpio_pincfg_clkout = AM_HAL_GPIO_PINCFG_CLKOUT_33;

#if CLKOUT_PIN <= 31
#define GPIO_IRQn       GPIO0_001F_IRQn
#elif CLKOUT_PIN <= 63
#define GPIO_IRQn       GPIO0_203F_IRQn
#elif CLKOUT_PIN <= 95
#define GPIO_IRQn       GPIO0_405F_IRQn
#else
#error Selected CLKOUT_PIN is not handled.
#endif

//*****************************************************************************
//
// GPIO ISR
//
//*****************************************************************************
static uint32_t g_ui32clkoutcnt = 0;

void
am_gpio0_203f_isr(void)
{
    uint32_t ui32IntStatus;

    //
    // Clear the GPIO interrupt via HAL functions
    //
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    if ( ++g_ui32clkoutcnt >= ( CLKOUT_FREQHZ * LED_DURATION) )
    {
        g_ui32clkoutcnt = 0;

        //
        // Toggle the LED.
        //
#if (AM_BSP_NUM_LEDS > 0)
        am_devices_led_toggle(am_bsp_psLEDs, LED_NUM);
#endif
    }

} // am_gpio0_203f_isr()

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32IntStatus;
    uint32_t ui32ClkoutPinnum = CLKOUT_PIN;

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#if (AM_BSP_NUM_LEDS > 0)
    //
    // Configure the LEDs.
    //
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);

    //
    // Turn the LEDs off, but initialize the LED on so user will see something.
    //
    for (int ix = 0; ix < AM_BSP_NUM_LEDS; ix++)
    {
        am_devices_led_off(am_bsp_psLEDs, ix);
    }

    am_devices_led_on(am_bsp_psLEDs, LED_NUM);
#endif // AM_BSP_NUM_LEDS

    //
    // Clear the GPIO Interrupt (write to clear).
    //
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO_IRQn, ui32IntStatus);
    AM_CRITICAL_END

    //
    // Enable the GPIO interrupt for the CLKOUT pin.
    //
    am_hal_gpio_interrupt_control(AM_HAL_GPIO_INT_CHANNEL_0,
                                  AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
                                  (void *)&ui32ClkoutPinnum);

    //
    // Enable GPIO interrupts to the NVIC.
    //
#if (AM_BSP_NUM_LEDS > 0)
    NVIC_SetPriority(GPIO_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(GPIO_IRQn);
#endif

    //
    // Enable interrupts to the core.
    //
    am_hal_interrupt_master_enable();

    //
    // Initialize the printf interface for ITM output
    //
    if ( am_bsp_debug_printf_enable() )
    {
        // Cannot print - so no point proceeding
        while(1);
    }

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("CLKOUT example...\n");

    //
    // Setting CLKOUT at the desired frequency.
    //
    CLKGEN->CLKOUT_b.CKSEL = CLKOUT_CKSEL;
    CLKGEN->CLKOUT_b.CKEN  = CLKGEN_CLKOUT_CKEN_EN;

    //
    // Configure pin for CLKOUT (Freq as set above).
    //
    if ( am_hal_gpio_pinconfig(CLKOUT_PIN, am_hal_gpio_pincfg_clkout) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Error configuring pin for CLKOUT.\n");
        am_bsp_debug_printf_disable();
        while(1);
    }

    //
    // We are done printing.
    // Disable debug printf messages on ITM.
    //
    am_util_stdio_printf("  CLKOUT signal is available on pin %d at %d Hz.\n", CLKOUT_PIN, CLKOUT_FREQHZ);
    am_util_stdio_printf("  LED %d will toggle approximately every %d second.\n", LED_NUM, LED_DURATION);

    am_util_stdio_printf("Done with prints. Entering while loop for deepsleep.\n");
    am_bsp_debug_printf_disable();

    while (1)
    {
        //
        // Go to Deep Sleep until wakeup.
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

