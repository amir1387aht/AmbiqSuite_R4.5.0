//*****************************************************************************
//
//! @file dtcm_retention.c
//!
//! @brief DTCM Variable Retention through reset
//!
//! @addtogroup memory_examples Memory Examples
//
//! @defgroup dtcm_retention DTCM Variable Retention
//! @ingroup memory_examples
//! @{
//!
//! Purpose: This example shows how to modify and reserve some TCM space to be able
//! to save variables through a reset
//!
//! This example sets aside a 256 word TCM space using INFO0_SECURITY_SRAM_RESV.<br>
//! It also writes data to DTCM and uses a variable through reset to count the<br>
//! number of resets. It also checks the data section for corruption or loss.<br>
//!
//! Additional Information:
//! The host example uses the ITM SWO to let the user know progress and <br>
//! status of the demonstration.  The SWO is configured at 1M baud.<br>
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

#include <string.h>
#include "am_memory_map.h"

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define DTCM_DATA_SIZE_IN_WORDS  0x400
#define DTCM_DATA_SIZE_IN_BYTES (DTCM_DATA_SIZE_IN_WORDS)
#define DTCM_RESERVED_SPACE_START (TCM_BASEADDR + TCM_MAX_SIZE - DTCM_DATA_SIZE_IN_BYTES)

//
//! Amount of SRAM Reserved space for DTCM memory Retention
//
static uint32_t ui32Info0[1] = { DTCM_DATA_SIZE_IN_WORDS };

uint32_t ui32Info0ReadBack[1];

#define INFO0_READ      0

typedef struct
{
    uint32_t counter;
    uint32_t my_data[5];
} my_struct;

my_struct *lcl_data = (my_struct *)(DTCM_RESERVED_SPACE_START);

const uint32_t data_src[5] = { 0, 1, 2, 3, 4 };

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    int32_t i32ReturnCode;

    am_util_delay_ms(4000);

    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eCacheCfg    = AM_HAL_PWRCTRL_CACHE_ALL,
        .bRetainCache = true,
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_384K,
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_384K,
        .bEnableNVM0  = true,
        .bRetainNVM0  = true
    };

    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();


    //
    // Initialize the peripherals for this board.
    //
    am_bsp_low_power_init();

    //
    // Enable printing through the ITM interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_printf("DTCM Retention Through Reset Example\n");
    am_util_stdio_printf("Location of lcl_data: 0x%08X\n\n", &lcl_data->counter);

    //
    // Do we need to program SRAM_RESV?
    //
    if (AM_HAL_STATUS_SUCCESS != am_hal_mram_info_read(INFO0_READ, AM_REG_INFO0_SECURITY_SRAM_RESV_O / 4, (sizeof(ui32Info0) / sizeof(uint32_t)), &ui32Info0ReadBack[0]))
    {
        am_util_stdio_printf("ERROR: INFO0 Read Back failed\n");
    }

    if (ui32Info0ReadBack[0] != DTCM_DATA_SIZE_IN_WORDS)
    {
        //
        // Set the SRAM RESV data size to DTCM_DATA_SIZE_IN_WORDS
        //
        am_util_stdio_printf("Programming SRAM Reserved in INFO0\n");
        i32ReturnCode = am_hal_mram_info_program(AM_HAL_MRAM_INFO_KEY,
                                                 &ui32Info0[0],
                                                 AM_REG_INFO0_SECURITY_SRAM_RESV_O / 4,
                                                 1 );

        //
        // Check for an error from the HAL.
        //
        if (i32ReturnCode != AM_HAL_STATUS_SUCCESS)
        {
            am_util_stdio_printf("MRAM program Info0 - i32ReturnCode = 0x%x.\n", i32ReturnCode);
            while(1);
        }

        memset((void *)lcl_data, 0, sizeof(my_struct));
    }

    if (0 != memcmp((void *)&data_src, (void *)lcl_data->my_data, sizeof(data_src)))
    {
        am_util_stdio_printf("A %d: %d, %d, %d, %d, %d\n", lcl_data->counter,
                              lcl_data->my_data[0], lcl_data->my_data[1], lcl_data->my_data[2], lcl_data->my_data[3], lcl_data->my_data[4]);

        // The first time through, initialize the reserved data.
        am_util_stdio_printf(" Initializing the array and counter...\n");
        memcpy((void *)&lcl_data->my_data, (void *)&data_src, sizeof(data_src));
        lcl_data->counter = 0;

        am_util_stdio_printf("B %d: %d, %d, %d, %d, %d\n", lcl_data->counter,
                              lcl_data->my_data[0], lcl_data->my_data[1], lcl_data->my_data[2], lcl_data->my_data[3], lcl_data->my_data[4]);
    }

    //
    // Verify that the reserved data didn't change.
    // Note that the first time through it will compare.
    //

    //
    //! Increase Counter
    //
    lcl_data->counter++;

    //
    //! Check Data retention
    //
    if (0 != memcmp((void *)&data_src, (void *)lcl_data->my_data, sizeof(data_src)))
    {
        am_util_stdio_printf("Error in DTCM Retention", i32ReturnCode);
        am_util_stdio_printf("%d: %d, %d, %d, %d, %d\n", lcl_data->counter,
                              lcl_data->my_data[0], lcl_data->my_data[1], lcl_data->my_data[2], lcl_data->my_data[3], lcl_data->my_data[4]);
    }
    else
    {
        if (lcl_data->counter == 1)
        {
            am_util_stdio_printf("Match! Data in TCM is correct on the first iteration.\n");
        }
        else
        {
            am_util_stdio_printf("Match! Data in TCM was retained through the reset as expected.\n");
        }

    }

    if ( lcl_data->counter < 5 )
    {
        am_util_stdio_printf("Device reset; count=%d.\n", lcl_data->counter);

        //
        // Verify that ITM is done printing
        //
        am_hal_itm_not_busy();

        //
        // Reset the device
        //
        am_hal_reset_control(AM_HAL_RESET_CONTROL_SWPOR, 0);
        while(1);
    }

    //
    // Loop forever while sleeping.
    //
    am_util_stdio_printf("\nAll done, going to sleep now.\n");
    am_bsp_debug_printf_disable();

    //
    // Clear the INFO0 SRAM RESV space so the example can run again.
    //
    ui32Info0[0] = 0x000;
    i32ReturnCode = am_hal_mram_info_program(AM_HAL_MRAM_INFO_KEY,
                                                 &ui32Info0[0],
                                                 AM_REG_INFO0_SECURITY_SRAM_RESV_O / 4,
                                                 1 );
    //
    // Check for an error from the HAL.
    //
    if (i32ReturnCode != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("MRAM Erase Info0 SRAM RESV Failed - i32ReturnCode = 0x%x.\n", i32ReturnCode);
    }

    while (1)
    {
        //
        // Go to Deep Sleep.
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
