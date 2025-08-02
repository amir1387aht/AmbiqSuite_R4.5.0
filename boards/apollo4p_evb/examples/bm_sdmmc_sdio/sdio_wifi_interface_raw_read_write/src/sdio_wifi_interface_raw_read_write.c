//*****************************************************************************
//
//! @file sdio_wifi_interface_raw_read_write.c
//!
//! @brief sdio wifi interface read and write example.
//!
//! @addtogroup bm_sdmmc_sdio_examples SDIO Examples
//!
//! @defgroup sdio_wifi_interface_raw_read_write SD Card WiFi Interface Read/Write Example
//! @ingroup bm_sdmmc_sdio_examples
//! @{
//!
//! Purpose: This example demonstrates how to blocking DMA read & write
//!          APIs with WiFi device.
//!
//! Additional Information:
//! To enable debug printing, add the following project-level macro definitions.
//!
//! AM_DEBUG_PRINTF
//!
//! When defined, debug messages will be sent over ITM/SWO at 1M Baud.
//!
//! Note that when these macros are defined, the device will never achieve deep
//! sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.
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

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_devices_sdio_rs9116.h"

//#define SDIO_TIMING_SCAN
#define RSI_PING_BUFFER_ADDR 0x18000
#define RSI_BLOCK_SIZE       256
#define BLK_NUM 16
#define BUF_LEN RSI_BLOCK_SIZE*BLK_NUM

AM_SHARED_RW uint8_t ui8RdBufSSRAM[BUF_LEN] AM_BIT_ALIGNED(128);
AM_SHARED_RW uint8_t ui8WrBufSSRAM[BUF_LEN] AM_BIT_ALIGNED(128);

void check_if_data_match(uint8_t *pui8RdBuf, uint8_t *pui8WrBuf, uint32_t ui32Len)
{
    uint32_t i;
    for (i = 0; i < ui32Len; i++)
    {
        if (pui8RdBuf[i] != pui8WrBuf[i])
        {
            am_util_stdio_printf("pui8RdBuf[%d] = %d and pui8WrBuf[%d] = %d\n", i, pui8RdBuf[i], i, pui8WrBuf[i]);
            break;
        }
    }

    if (i == ui32Len)
    {
        am_util_stdio_printf("data matched\n");
    }

}

//
// customer provides their load function
//
bool custom_load_sdio_calib(uint8_t ui8TxRxDelays[2])
{
    //
    // Load ui8TxRxDelays from the somewhere - for example non-volatile memory ...
    // here simply hardcoding these values.
    //
    ui8TxRxDelays[0] = 0;
    ui8TxRxDelays[1] = 0;

    //
    // Return false not to force the calibration
    //
    return false;
}

#ifdef SDIO_TIMING_SCAN
//
// Reset RS9116 device
//
uint32_t sdio_wifi_device_reset()
{
    am_bsp_sdio_pins_enable(AM_HAL_HOST_BUS_WIDTH_4);

    am_bsp_sdio_reset();

    return true;
}
#endif


//*****************************************************************************
//
// SDIO ISRs.
//
//*****************************************************************************
void am_sdio_isr(void)
{
    uint32_t ui32IntStatus;

    am_hal_sdhc_intr_status_get(pSdhcCardHost->pHandle, &ui32IntStatus, true);
    am_hal_sdhc_intr_status_clear(pSdhcCardHost->pHandle, ui32IntStatus);
    am_hal_sdhc_interrupt_service(pSdhcCardHost->pHandle, ui32IntStatus);
}


int
main(void)
{
    uint32_t ui32Status;

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    //
    // Enable printing to the console.
    //
    am_bsp_itm_printf_enable();

    am_util_stdio_printf("\nApollo4 sdio raw block read write example\n");

    //
    // initialize the test read and write buffers
    //
    for (int i = 0; i < BUF_LEN; i++)
    {
        ui8WrBufSSRAM[i] = i % 256;
        ui8RdBufSSRAM[i] = 0x0;
    }

    uint8_t ui8TxRxDelays[2];

    bool bValid = custom_load_sdio_calib(ui8TxRxDelays);
    if (bValid)
    {
        rsi_sdio_apply_timing(ui8TxRxDelays);
    }
 #ifdef SDIO_TIMING_SCAN
    else
    {
        am_util_stdio_printf("Wait for SDIO Timing Scan......\n");
        ui32Status = rsi_sdio_timing_scan(AM_HAL_SDHC_CARD_HOST, AM_HAL_HOST_UHS_NONE, 48000000,
                                         AM_HAL_HOST_BUS_WIDTH_4, (uint8_t *)ui8RdBufSSRAM,
                                      RSI_PING_BUFFER_ADDR, 2, AM_HAL_HOST_BUS_VOLTAGE_1_8,
                                                    ui8TxRxDelays, sdio_wifi_device_reset);
        if ( ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            am_util_stdio_printf("No Timing Scan Window, example fail\n");
            return false;
        }
        else
        {
            am_util_stdio_printf("SDIO TX delay - %d, RX Delay - %d\n", ui8TxRxDelays[0], ui8TxRxDelays[1]);
        }
    }
#endif

    //
    // initialize the SDIO wifi mcu
    //
    ui32Status = rsi_mcu_sdio_init();
    if ( ui32Status != AM_DEVICES_SDIO_RS9116_STATUS_SUCCESS )
    {
        am_util_debug_printf("RS9116 SDIO init fail. Status=0x%x\n", ui32Status);
        return AM_DEVICES_SDIO_RS9116_STATUS_ERROR;
    }

    //
    //write 1 byte to rs9116
    //
    am_util_stdio_printf("\n======== Test 1 byte write and read to rs9116 ========\n");

    rsi_sdio_writeb(RSI_PING_BUFFER_ADDR, 1, (uint8_t *)ui8WrBufSSRAM);
    memset((void *)ui8RdBufSSRAM, 0x0, 1);
    rsi_sdio_readb(RSI_PING_BUFFER_ADDR, 1, (uint8_t *)ui8RdBufSSRAM);

    //
    // check if block data match or not
    //
    check_if_data_match((uint8_t *)ui8RdBufSSRAM, (uint8_t *)ui8WrBufSSRAM, 1);
    am_util_stdio_printf("============================================================\n\n");

    //
    // write 64 bytes to rs9116
    //
    am_util_stdio_printf("\n======== Test 64 bytes write and read to rs9116 ========\n");

    rsi_sdio_writeb(RSI_PING_BUFFER_ADDR, 64, (uint8_t *)ui8WrBufSSRAM);
    memset((void *)ui8RdBufSSRAM, 0x0, 64);
    rsi_sdio_readb(RSI_PING_BUFFER_ADDR, 64, (uint8_t *)ui8RdBufSSRAM);

    //
    // check if block data match or not
    //
    check_if_data_match((uint8_t *)ui8RdBufSSRAM, (uint8_t *)ui8WrBufSSRAM, 64);
    am_util_stdio_printf("============================================================\n\n");

    //
    // write 2*1024 bytes to sdio card
    //
    am_util_stdio_printf("\n========Test 2K bytes write and read to rs9116 ========\n");

    rsi_sdio_write_multiple((uint8_t *)ui8WrBufSSRAM, RSI_PING_BUFFER_ADDR, BLK_NUM / 2);
    memset((void *)ui8RdBufSSRAM, 0x0, BUF_LEN / 2);
    rsi_sdio_read_multiple((uint8_t *)ui8RdBufSSRAM, RSI_PING_BUFFER_ADDR, BLK_NUM / 2);

    //
    // check if block data match or not
    //
    check_if_data_match((uint8_t *)ui8RdBufSSRAM, (uint8_t *)ui8WrBufSSRAM, BUF_LEN / 2);
    am_util_stdio_printf("============================================================\n\n");

    //
    // SSRAM: write 4*1024 bytes to sdio card
    //
    am_util_stdio_printf("\n========Test 4K bytes write and read to rs9116 ========\n");

    rsi_sdio_write_multiple((uint8_t *)ui8WrBufSSRAM, RSI_PING_BUFFER_ADDR, BLK_NUM);
    memset((void *)ui8RdBufSSRAM, 0x0, BUF_LEN);
    rsi_sdio_read_multiple((uint8_t *)ui8RdBufSSRAM, RSI_PING_BUFFER_ADDR, BLK_NUM);

    //
    // check if block data match or not
    //
    check_if_data_match((uint8_t *)ui8RdBufSSRAM, (uint8_t *)ui8WrBufSSRAM, BUF_LEN);
    am_util_stdio_printf("============================================================\n\n");

    am_util_stdio_printf("\nApollo4 sdio raw block read write example complete\n");

    while (1);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
