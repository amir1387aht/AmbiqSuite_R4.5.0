//*****************************************************************************
//
//! @file sd_card_raw_block_read_write.c
//!
//! @brief sd card raw block read and write example.
//!
//! @addtogroup bm_sdmmc_sdio_examples SDIO Examples
//!
//! @defgroup sd_card_raw_block_read_write SD Card Raw Read/Write Example
//! @ingroup bm_sdmmc_sdio_examples
//! @{
//!
//! Purpose: This example demonstrates how to use blocking DMA read & write
//!          APIs with sd card device.
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

#define SD_CARD_BOARD_SUPPORT_1_8_V

//
// Test different RAMs
//
#define TEST_SSRAM
#define TEST_TCM
#define CARD_INFOR_TEST

#define TEST_ASYC_WRIT_READ
#define DELAY_MAX_COUNT   1000

#define SSRAM_START_ADDR    0x10064000

#define START_BLK 70000
#define BLK_NUM 10
#define BUF_LEN 512*BLK_NUM
#define BLK_OFFSET 10

volatile uint8_t ui8RdBuf[BUF_LEN] AM_BIT_ALIGNED(128);
volatile uint8_t ui8WrBuf[BUF_LEN] AM_BIT_ALIGNED(128);

AM_SHARED_RW uint8_t pui8RdBufSSRAM[BUF_LEN] AM_BIT_ALIGNED(128);
AM_SHARED_RW uint8_t pui8WrBufSSRAM[BUF_LEN] AM_BIT_ALIGNED(128);

volatile bool bAsyncWriteIsDone = false;
volatile bool bAsyncReadIsDone  = false;
volatile uint32_t g_ui32CardIntCnt = 0;
volatile uint32_t g_ui32CardIsertFlag = 0xFF;

void am_hal_card_event_test_cb(am_hal_host_evt_t *pEvt)
{
    am_hal_card_host_t *pHost = (am_hal_card_host_t *)pEvt->pCtx;

    if (AM_HAL_EVT_XFER_COMPLETE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_READ)
    {
        bAsyncReadIsDone = true;
        am_util_debug_printf("Last Read Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_XFER_COMPLETE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_WRITE)
    {
        bAsyncWriteIsDone = true;
        am_util_debug_printf("Last Write Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_SDMA_DONE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_READ)
    {
        am_util_debug_printf("SDMA Read Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_SDMA_DONE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_WRITE)
    {
        am_util_debug_printf("SDMA Write Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_DAT_ERR == pEvt->eType)
    {
        am_util_debug_printf("Data error type %d\n", pHost->AsyncCmdData.eDataError);
    }

    if (AM_HAL_EVT_CARD_PRESENT == pEvt->eType)
    {
        g_ui32CardIsertFlag = true;
        g_ui32CardIntCnt ++;
        am_util_stdio_printf("SD card is inserted\n");
    }

    if (AM_HAL_EVT_CARD_NOT_PRESENT == pEvt->eType)
    {
        g_ui32CardIsertFlag = false;
        am_util_stdio_printf("SD Card removal\n");
    }
}

//
// Register SD Card power cycle function for card power on/off/reset,
// Usually used for switching to 3.3V signaling in sd card initialization
//
uint32_t sd_card_power_config(am_hal_card_pwr_e eCardPwr)
{
    am_hal_gpio_pinconfig(AM_BSP_GPIO_SD_POWER_CTRL, am_hal_gpio_pincfg_output);

    if ( eCardPwr == AM_HAL_CARD_PWR_CYCLE )
    {
        //
        // SD Card power cycle or power on
        //
        am_hal_gpio_output_clear(AM_BSP_GPIO_SD_POWER_CTRL);
        am_util_delay_ms(20);
        am_hal_gpio_output_set(AM_BSP_GPIO_SD_POWER_CTRL);

        //
        // wait until the power supply is stable
        //
        am_util_delay_ms(20);

#ifdef SD_CARD_BOARD_SUPPORT_1_8_V
        am_hal_gpio_output_clear(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL);
#endif
    }
    else if ( eCardPwr == AM_HAL_CARD_PWR_OFF )
    {
        am_hal_gpio_output_clear(AM_BSP_GPIO_SD_POWER_CTRL);

#ifdef SD_CARD_BOARD_SUPPORT_1_8_V
        am_hal_gpio_output_clear(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL);
#endif
    }
    else if ( eCardPwr == AM_HAL_CARD_PWR_SWITCH )
    {
        //
        // set level shifter to 1.8V
        //
        am_hal_gpio_output_set(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL);
        am_util_delay_ms(20);
    }

    return true;
}

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

am_hal_card_host_t *pSdhcCardHost = NULL;

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
    uint32_t ui32Status = 0;
    uint32_t ui32CardInitCnt = 0;

    am_hal_card_t SDcard;

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Configure SDIO PINs.
    //
    am_bsp_sdio_pins_enable(AM_HAL_HOST_BUS_WIDTH_4);

    //
    // SD card voltage level translator control
    //
#ifdef SD_CARD_BOARD_SUPPORT_1_8_V
    am_hal_gpio_pinconfig(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_clear(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL);
#else
    am_hal_gpio_pinconfig(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_set(AM_BSP_GPIO_SD_LEVEL_SHIFT_SEL);
#endif

    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    //
    // Enable printing to the console.
    //
    am_bsp_itm_printf_enable();

    //
    // initialize the test read and write buffers
    //
    for (int i = 0; i < BUF_LEN; i++)
    {
        pui8WrBufSSRAM[i] = 0xFF & (0xFF-i);
        ui8WrBuf[i] = i % 256;
        ui8RdBuf[i] = 0x0;
    }

    pSdhcCardHost = am_hal_get_card_host(AM_HAL_SDHC_CARD_HOST, true);

    if ( pSdhcCardHost == NULL )
    {
        am_util_stdio_printf("No such card host and stop\n");
        while(1);
    }
    am_util_stdio_printf("card host is found\n");

    //
    // check if card is present
    //
    while (am_hal_card_host_find_card(pSdhcCardHost, &SDcard) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("No card is present now\n");
        am_util_delay_ms(1000);
        am_util_stdio_printf("Checking if card is available again\n");
    }

    //
    // Must config a gpio as sd card WP pin in bsp before sd card write protect detection function
    //
#ifndef SD_CARD_BOARD_SUPPORT_1_8_V
    am_bsp_sd_wp_pin_enable(true);
#endif

    //
    // Must config a gpio as sd card CD pin in bsp before enable card detection function
    //
    am_bsp_sd_cd_pin_enable(true);

    ui32Status = am_hal_sd_card_enable_card_detect(&SDcard, am_hal_card_event_test_cb);
    if ( ui32Status !=  AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("card detection enable failed\n");
    }

    for ( uint32_t ui32LoopCnt = 0; ui32LoopCnt < 0x1400; ui32LoopCnt++ )
    {
        if ( g_ui32CardIsertFlag == 0xff)
        {
            am_util_stdio_printf("No card connected, need card insert\n");
        }
        else if ( g_ui32CardIsertFlag == 1 )
        {
            //
            // SD Card need be initialized
            //
            if ( ui32CardInitCnt != g_ui32CardIntCnt )
            {
                //
                // Get the uderlying SDHC card host instance
                //
                pSdhcCardHost = am_hal_get_card_host(AM_HAL_SDHC_CARD_HOST, true);

                if ( pSdhcCardHost == NULL )
                {
                    am_util_stdio_printf("No such card host and stop\n");
                    continue;
                }
                am_util_stdio_printf("card host is found\n");

                //
                // check if card is present
                //
                if ( am_hal_card_host_find_card(pSdhcCardHost, &SDcard) != AM_HAL_STATUS_SUCCESS )
                {
                    am_util_stdio_printf("No card is present now\n");
                    am_util_delay_ms(1000);
                    am_util_stdio_printf("Checking if card is available again\n");
                    continue;
                }

#ifndef SD_CARD_BOARD_SUPPORT_1_8_V
                am_hal_sd_card_write_protect_detect(&SDcard);
                if ( ui32Status !=  AM_HAL_STATUS_SUCCESS)
                {
                    am_util_stdio_printf("card write protect detect failed\n");
                }

#endif

                ui32Status = am_hal_sd_card_enable_card_detect(&SDcard, am_hal_card_event_test_cb);
                if ( ui32Status !=  AM_HAL_STATUS_SUCCESS)
                {
                    am_util_stdio_printf("card detection enable failed\n");
                }

                if ( am_hal_card_init(&SDcard, AM_HAL_CARD_TYPE_SDHC, sd_card_power_config, AM_HAL_CARD_PWR_CTRL_SDHC_OFF) != AM_HAL_STATUS_SUCCESS )
                {
                    am_util_delay_ms(1000);
                    am_util_stdio_printf("card init failed, try again\n");
                    continue;
                }

                //
                // Set bus width and speed
                //
                ui32Status = am_hal_card_cfg_set(&SDcard, AM_HAL_CARD_TYPE_SDHC,
                    AM_HAL_HOST_BUS_WIDTH_4, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
                    AM_HAL_HOST_UHS_NONE);
                if ( ui32Status != AM_HAL_STATUS_SUCCESS )
                {
                    am_util_delay_ms(1000);
                    am_util_stdio_printf("setting bus 4 bit failed\n");
                    continue;
                }

                ui32CardInitCnt = g_ui32CardIntCnt;
            }

            am_hal_card_host_set_xfer_mode(pSdhcCardHost, AM_HAL_HOST_XFER_ADMA);

#ifdef CARD_INFOR_TEST
            //
            // Get sd card CSD information
            //
            am_util_stdio_printf("\n======== Get SD Card CSD information ========\n");

            uint32_t ui32CSDVersion;
            uint32_t ui32MaxBlks;
            uint32_t ui32BlkSize;
            uint32_t ui32CSize;

            ui32CSDVersion = am_hal_card_get_csd_field(&SDcard, 126, 2);
            if ( ui32CSDVersion == 0 )
            {
                uint32_t ui32CSizeMult = am_hal_card_get_csd_field(&SDcard, 47, 3);
                ui32CSize = am_hal_card_get_csd_field(&SDcard, 62, 12);
                uint32_t ui32RdBlockLen = am_hal_card_get_csd_field(&SDcard, 80, 4);

                uint32_t ui32Mult = 1 << ( ui32CSizeMult + 2 );
                uint32_t ui32BlockLen = 1 << ui32RdBlockLen;

                //
                // CSD 1.0: Memory capacity = ( C_SIZE + 1 ) * ui32Mult * ui32BlockLen.
                //
                ui32MaxBlks = ( ui32CSize + 1 ) * ui32Mult * ui32BlockLen / 512;
                ui32BlkSize = ui32BlockLen;
            }
            else if ( ui32CSDVersion == 1 )
            {
                ui32CSize = am_hal_card_get_csd_field(&SDcard, 48, 22);
                uint32_t ui32RdBlockLen = am_hal_card_get_csd_field(&SDcard, 80, 4);
                ui32BlkSize = 1 << ui32RdBlockLen;

                //
                // CSD 2.0: Memory capacity = ( C_SIZE + 1 ) * 512K.
                //
                ui32MaxBlks = (ui32CSize + 1) * 1024;
            }

            am_util_stdio_printf("Product CSD Version : 0x%x\n", ui32CSDVersion + 1);
            am_util_stdio_printf("Product CSD C_Size :0x%x\n", ui32CSize);
            am_util_stdio_printf("Product Block Size :%d\n", ui32BlkSize);
            am_util_stdio_printf("Product Max Block Count : 0x%x\n", ui32MaxBlks);

            am_util_stdio_printf("============================================================\n\n");
#endif

#ifdef TEST_SSRAM
            //
            // write 1block to sd card
            //
            uint8_t ui32BlockCnt = 1;
            am_util_stdio_printf("\n======== Test single block sync write and read in SSRAM ========\n");
            ui32Status = am_hal_sd_card_block_write_sync(&SDcard, START_BLK, ui32BlockCnt, (uint8_t *)pui8WrBufSSRAM);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block write error:%x\n", ui32Status);
                continue;
            }

            memset((void *)pui8RdBufSSRAM, 0x0, ui32BlockCnt*512);

            ui32Status = am_hal_sd_card_block_read_sync(&SDcard, START_BLK, ui32BlockCnt, (uint8_t *)pui8RdBufSSRAM);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block read error:%x\n", ui32Status);
                continue;
            }

            //
            // check if block data match or not
            //
            check_if_data_match((uint8_t *)pui8RdBufSSRAM, (uint8_t *)pui8WrBufSSRAM, 512*ui32BlockCnt);
            am_util_stdio_printf("\n======== Single block sync write and read in SSRAM Done ========\n");

            ui32BlockCnt = BLK_NUM;
            am_util_stdio_printf("\n======== Test multiple block:%d, at %d sync write and read in SSRAM ========\n", BLK_NUM, START_BLK + 10);
            ui32Status = am_hal_sd_card_block_write_sync(&SDcard, START_BLK + BLK_OFFSET, ui32BlockCnt, (uint8_t *)pui8WrBufSSRAM);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block write error:%x\n", ui32Status);
                continue;
            }

            memset((void *)pui8RdBufSSRAM, 0x0, ui32BlockCnt*512);

            ui32Status = am_hal_sd_card_block_read_sync(&SDcard, START_BLK + BLK_OFFSET, ui32BlockCnt, (uint8_t *)pui8RdBufSSRAM);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block read error:%x\n", ui32Status);
                continue;
            }

            //
            // check if block data match or not
            //
            check_if_data_match((uint8_t *)pui8RdBufSSRAM, (uint8_t *)pui8WrBufSSRAM, 512*ui32BlockCnt);
            am_util_stdio_printf("============================================================\n\n");
#endif

#ifdef TEST_TCM
            ui32BlockCnt = BLK_NUM;
            am_util_stdio_printf("\n======== Test multiple block:%d, at %d sync write and read in TCM ========\n", ui32BlockCnt, START_BLK);
            ui32Status = am_hal_sd_card_block_write_sync(&SDcard, START_BLK, ui32BlockCnt, (uint8_t *)ui8WrBuf);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block write error:%x\n", ui32Status);
                continue;
            }

            memset((void *)ui8RdBuf, 0x0, ui32BlockCnt*512);

            ui32Status = am_hal_sd_card_block_read_sync(&SDcard, START_BLK, ui32BlockCnt, (uint8_t *)ui8RdBuf);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block read error:%x\n", ui32Status);
                continue;
            }

            //
            // check if block data match or not
            //
            check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, 512*ui32BlockCnt);
            am_util_stdio_printf("============================================================\n\n");
#endif

#ifdef TEST_ASYC_WRIT_READ
            am_util_stdio_printf("\n======== multiple block async write and read ========\n");
            //
            // write 5K bytes to sd card
            //
            bAsyncWriteIsDone = false;
            ui32Status = am_hal_sd_card_block_write_async(&SDcard, START_BLK, ui32BlockCnt, (uint8_t *)ui8WrBuf);

            //
            // wait until the async write is done
            //
            int i = 0;
            while (!bAsyncWriteIsDone)
            {
                am_util_delay_ms(1);
                i++;
                if ( i == DELAY_MAX_COUNT )
                {
                    am_util_stdio_printf("Async Write Timeout");
                    break;
                }
            }

            if ( i < DELAY_MAX_COUNT)
            {
                am_util_stdio_printf("asynchronous block write is done\n");
            }

            //
            // read back the data from sd card
            //
            memset((void *)ui8RdBuf, 0x0, ui32BlockCnt*512);
            bAsyncReadIsDone = false;
            ui32Status = am_hal_sd_card_block_read_async(&SDcard, START_BLK, ui32BlockCnt, (uint8_t *)ui8RdBuf);

            //
            // wait until the async read is done
            //
            i = 0;
            while (!bAsyncReadIsDone)
            {
                am_util_delay_ms(1);
                i++;
                if ( i == DELAY_MAX_COUNT )
                {
                    am_util_stdio_printf("Async Read Timeout");
                    break;
                }
            }

            if ( i < DELAY_MAX_COUNT )
            {
                am_util_stdio_printf("asynchronous block read is done\n");
            }

            //
            // check if block data match or not
            //
            check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, 512*ui32BlockCnt);
            am_util_stdio_printf("============================================================\n\n");
#endif
            //
            // erase the test block and check erase data
            //
            am_util_stdio_printf("\n======== Test block erase start at %d, end at %d ========\n", START_BLK + 10, START_BLK + 10 + ui32BlockCnt);
            am_hal_sd_card_block_erase(&SDcard, START_BLK + BLK_OFFSET, ui32BlockCnt, 10000);

            ui32Status = am_hal_sd_card_block_read_sync(&SDcard, START_BLK + BLK_OFFSET, ui32BlockCnt, (uint8_t *)pui8RdBufSSRAM);
            if ( ( ui32Status & 0xFFFF ) != AM_HAL_STATUS_SUCCESS )
            {
                am_util_stdio_printf("card block read error:%x\n", ui32Status);
                continue;
            }

            for (int i = 0; i < 512*ui32BlockCnt; i++)
            {
                if ((pui8RdBufSSRAM[i] != 0) && (pui8RdBufSSRAM[i] != 0xFF))
                {
                    am_util_stdio_printf("card erase failed pui8RdBuf[%d] = %d\n", i, pui8RdBufSSRAM[i]);
                }
            }

            am_util_stdio_printf("============================================================\n\n");
        }
        else if ( !g_ui32CardIsertFlag )
        {
            am_util_stdio_printf("Detected SD Card pulled out , deinit sd card\n");
            am_hal_card_deinit(&SDcard);
        }
        am_util_stdio_printf("Detecting SD Card \n");
    }

    am_util_stdio_printf("SD Card raw block read and write test done\n");
    while(1);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
