//*****************************************************************************
//
//! @file prime.c
//!
//! @brief This example computes the number of primes in a given integer value.
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup prime Prime Example
//! @ingroup power_examples
//! @{
//!
//! Purpose: This example consists of a non-optimized, brute-force routine for<br>
//! computing the number of prime numbers between 1 and a given value, N. The<br>
//! routine uses modulo operations to determine whether a value is prime or not.<br>
//! While obviously not optimal, it is very useful for exercising the core.<br>
//! <br>
//! For this example, N is 1000000, for which the answer is 78498.<br>
//! <br>
//! Additional Information:
//! The goal of this example is to measure current consumption while the core<br>
//! is working to compute the answer. Power and energy can then be derived<br>
//! knowing the current and run time.<br>
//! <br>
//! The example prints an initial banner to the UART port.  After each prime<br>
//! loop, it enables the UART long enough to print the answer, disables the<br>
//! UART and starts the computation again.<br>
//! <br>
//! Text is output to the UART at 115,200 BAUD, 8 bit, no parity.<br>
//! Please note that text end-of-line is a newline (LF) character only.<br>
//! Therefore, the UART terminal must be set to simulate a CR/LF.<br>
//! <br>
//! @note For minimum power, disable the printing by setting PRINT_UART to 0.
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
#include "am_hal_global.h"

//*****************************************************************************
//
// Set PRINT_UART to 1 to print a banner message and the solution to the
// number of primes.
// Note that the UART is disabled while the computations are in progress.
//
//*****************************************************************************
#define PRINT_UART    1

//*****************************************************************************
//
// Number of primes to count.
//
//*****************************************************************************
//
// Set target to 100000 (for which the correct answer is 9592).
//
#define NUM_OF_PRIMES_IN 1000000
#define EXP_PRIMES 78498

//*****************************************************************************
//
// Minimize power
//
//*****************************************************************************
void
min_power_settings(void)
{
    //
    // Low power settings.
    //

#if defined(AM_PART_APOLLO4B)
    //
    // Note that this differs slightly from the Apollo4b HAL setting of this
    // field to 0x3E. The low bit is DAXI CLKGATE.
    //
    CLKGEN->MISC_b.CLKGENMISCSPARES = 0x3F;
#endif // AM_PART_APOLLO4B

    //
    // Disable the XTAL.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC, 0);

    //
    // Turn off the voltage comparator.
    //
    VCOMP->PWDKEY = _VAL2FLD(VCOMP_PWDKEY_PWDKEY, VCOMP_PWDKEY_PWDKEY_Key);

    //
    // Configure the MRAM for low power mode
    //
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;

#if defined(AM_PART_APOLLO4B)
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 0;
#elif defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
#endif

#if defined (AM_PART_APOLLO4B)
    //
    // Power down Crypto.
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);

    //
    // Disable all peripherals
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL, 0);

    //
    // Disable XTAL
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
#elif defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
#endif

    //
    // Additional Power Settings
    //
    MCUCTRL->PWRSW0_b.PWRSWVDDMDSP0DYNSEL = 0;    // Bit 18
    MCUCTRL->PWRSW0_b.PWRSWVDDMDSP1DYNSEL = 0;    // Bit 21
    MCUCTRL->PWRSW0_b.PWRSWVDDMLDYNSEL    = 0;    // Bit 24

} // min_power_settings()

//*****************************************************************************
//
// Count the number of primes in 0-N range
//
//*****************************************************************************

unsigned int
primeNumber( unsigned int n )
{
    unsigned int count = 1, prime;

    for ( unsigned int i = 3; i < n ; i++ )
    {
        prime = 1;
        for ( unsigned int j = 2; j * j <= i; j++ )
        {
            if ( (i % j) == 0 )
            {
                prime = 0;
                break;
            }
        }
        if ( prime )
        {
            count++;
        }
    }

    return count;
}

//*****************************************************************************
//
// Main Function.
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Result;

    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eCacheCfg    = AM_HAL_PWRCTRL_CACHE_ALL,
        .bRetainCache = false,
#if defined(AM_PART_APOLLO4L)
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_32K,
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_32K,
#else
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_8K,
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_8K,
#endif
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

    //
    // Configure the board for low power operation.
    // Note that for Apollo4, this call will set the default memory
    // configuration, which will be reconfigured as desired afterwards.
    //
    am_bsp_low_power_init();

    //
    // Other power optimizations
    //
    min_power_settings();

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Update memory configuration to minimum.
    //
    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);

#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    //
    // For SiP packages, put the BLE Controller in reset.
    //
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN,   am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_DEVICES_BLECTRLR_RESET_PIN

#if (PRINT_UART == 1)
    am_bsp_uart_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Ambiq Micro 'prime' example.\n");

    //
    // Brief description
    //
    am_util_stdio_printf("Measure power while computing the number of prime numbers up to %d.\n\n", NUM_OF_PRIMES_IN);

    //
    // Print the compiler version.
    //
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
    am_util_stdio_printf("HAL Compiler:    %s\n", g_ui8HALcompiler);
    am_util_stdio_printf("HAL SDK version: %d.%d.%d\n",
                         g_ui32HALversion.s.Major,
                         g_ui32HALversion.s.Minor,
                         g_ui32HALversion.s.Revision);
    am_util_stdio_printf("HAL compiled with %s-style registers\n",
                         g_ui32HALversion.s.bAMREGS ? "AM_REG" : "CMSIS");

    am_util_stdio_printf("\nEntering prime calculation loop...\n");

    //
    // To minimize power during the run, disable the UART.
    //
    am_hal_delay_us(10000);
    am_bsp_uart_printf_disable();
#endif // PRINT_UART

    //
    // Call the prime function
    //
    while(1)
    {
        //
        // Determine the number of primes for the given value.
        //
        ui32Result = primeNumber(NUM_OF_PRIMES_IN);

#if (PRINT_UART == 1)
        //
        // Print the result
        //
        am_bsp_uart_printf_enable();

        if ( ui32Result == EXP_PRIMES )
        {
            am_util_stdio_printf("Pass: number of primes for %d is %d.\n", NUM_OF_PRIMES_IN, ui32Result);
        }
        else
        {
            am_util_stdio_printf("ERROR: Invalid result. Expected number of primes = %d, got %d.\n", EXP_PRIMES, ui32Result);
        }

        am_hal_delay_us(10000);
        am_bsp_uart_printf_disable();

        //
        // Flush the cache so the computations will begin in a consistent state
        //
        am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_MRAM_CACHE_INVALIDATE, 0);

#endif // PRINT_UART
    }
}
