//*****************************************************************************
//
//! @file emmc_raw_block_read_write.c
//!
//! @brief emmc raw block read and write example.
//!
//! @addtogroup bm_sdmmc_sdio_examples SDIO Examples
//
//! @defgroup emmc_raw_block_read_write eMMC Raw Block Read Write Example
//! @ingroup bm_sdmmc_sdio_examples
//! @{
//!
//! Purpose: This example demonstrates how to blocking PIO and DMA read & write
//!          APIs with eMMC device.<br>
//! <br>
//!   1) Initialize for low power<br>
//!   2) Initialize Card Host<br>
//!   3) Config Card Host<br>
//!   4) Test SSRAM<br>
//!   5) Test SSRAM<br>
//!   6) Test TCM<br>
//!   7) Loop through 4, 5, 6 forever<br>
//! <br>
//! Additional Information:
//! Debug messages will be sent over ITM/SWO at 1M Baud.<br>
//! <br>
//! If ITM is not shut down, the device will never achieve deep sleep, only<br>
//! normal sleep, due to the ITM (and thus the HFRC) being enabled.<br>
//! <br>
//! The following macros can be used in this example<br>
//! <br>
//! Defined by default:<br>
//!   #define TEST_POWER_SAVING<br>
//!           Enables to power down and back up of the SDIO<br>
//!   #define TEST_SSRAM<br>
//!           Enables SSRAM testing<br>
//!   #define TEST_TCM<br>
//!           Enables TCM testing<br>
//! <br>
//! Not Defined by default:<br>
//!   #define EMMC_DDR50_TEST<br>
//!           Enables to config and test the device in DDR50 mode<br>
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


#define TEST_POWER_SAVING
//#define EMMC_DDR50_TEST

//
// Test different RAMs
//
#define TEST_SSRAM
#define TEST_TCM

#define DSP_RAM0_START_ADDR 0x10164000
#define SSRAM_START_ADDR    0x10064000

#define ALIGN(x) __attribute__((aligned(1 << x)))

#define START_BLK 3000
#define BLK_NUM 4
#define BUF_LEN 512*BLK_NUM



volatile uint8_t ui8RdBuf[BUF_LEN] AM_BIT_ALIGNED(128);
volatile uint8_t ui8WrBuf[BUF_LEN] AM_BIT_ALIGNED(128);

uint8_t *pui8RdBufSSRAM;
uint8_t *pui8WrBufSSRAM;
uint8_t *pui8RdBufDspRam0;
uint8_t *pui8WrBufDspRam0;

volatile bool bAsyncWriteIsDone = false;
volatile bool bAsyncReadIsDone  = false;

void am_hal_card_event_test_cb(am_hal_host_evt_t *pEvt)
{
    am_hal_card_host_t *pHost = (am_hal_card_host_t *)pEvt->pCtx;

    if (AM_HAL_EVT_XFER_COMPLETE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_READ)
    {
        bAsyncReadIsDone = true;
        am_util_stdio_printf("Last Read Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_XFER_COMPLETE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_WRITE)
    {
        bAsyncWriteIsDone = true;
        am_util_stdio_printf("Last Write Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_SDMA_DONE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_READ)
    {
        am_util_stdio_printf("SDMA Read Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_SDMA_DONE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_WRITE)
    {
        am_util_stdio_printf("SDMA Write Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_DAT_ERR == pEvt->eType)
    {
        am_util_stdio_printf("Data error type %d\n", pHost->AsyncCmdData.eDataError);
    }

    if (AM_HAL_EVT_CARD_PRESENT == pEvt->eType)
    {
        am_util_stdio_printf("A card is inserted\n");
    }
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

//
// customer provides their load function
//
bool custom_load_ddr50_calib(uint8_t ui8TxRxDelays[2])
{
    //
    // Load ui8TxRxDelays from the somewhere - for example non-volatile memory ...
    // here simply hardcoding these values.
    //
    ui8TxRxDelays[0] = 9;
    ui8TxRxDelays[1] = 6;

    //
    // Return true not to force the calibration
    //
    return -1;
}

int
main(void)
{
    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Configure SDIO PINs.
    //
    am_bsp_sdio_pins_enable(AM_HAL_HOST_BUS_WIDTH_8);


    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    //
    // Enable printing to the console.
    //
    am_bsp_itm_printf_enable();

    pui8RdBufDspRam0 = (uint8_t *)DSP_RAM0_START_ADDR + 4*512;
    pui8WrBufDspRam0 = (uint8_t *)DSP_RAM0_START_ADDR;
    pui8RdBufSSRAM = (uint8_t *)SSRAM_START_ADDR;
    pui8WrBufSSRAM = (uint8_t *)SSRAM_START_ADDR + 4*512;

    //
    // initialize the test read and write buffers
    //
    for (int i = 0; i < BUF_LEN; i++)
    {
        pui8WrBufDspRam0[i] = i % 256;
        pui8WrBufSSRAM[i] = i % 256;
        ui8WrBuf[i] = i % 256;
        ui8RdBuf[i] = 0x0;
    }

    //
    // Get the underlying SDHC card host instance
    //
    pSdhcCardHost = am_hal_get_card_host(AM_HAL_SDHC_CARD_HOST, true);

    if (pSdhcCardHost == NULL)
    {
        am_util_stdio_printf("No such card host and stop\n");
        while(1);
    }
    am_util_stdio_printf("card host is found\n");

#ifdef EMMC_DDR50_TEST
    uint8_t ui8TxRxDelays[2];
    bool bValid = custom_load_ddr50_calib(ui8TxRxDelays);
    if (bValid)
    {
        am_hal_card_host_set_txrx_delay(pSdhcCardHost, ui8TxRxDelays);
    }
    else
    {
        am_hal_card_emmc_calibrate(AM_HAL_HOST_UHS_DDR50, 48000000, AM_HAL_HOST_BUS_WIDTH_4,
            (uint8_t *)ui8WrBuf, START_BLK, 2, ui8TxRxDelays);

        am_util_stdio_printf("SDIO TX delay - %d, RX Delay - %d\n", ui8TxRxDelays[0], ui8TxRxDelays[1]);
    }
#endif

    am_hal_card_t eMMCard;

    //
    // check if card is present
    //
    while (am_hal_card_host_find_card(pSdhcCardHost, &eMMCard) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("No card is present now\n");
        am_util_delay_ms(1000);
        am_util_stdio_printf("Checking if card is available again\n");
    }

    while (am_hal_card_init(&eMMCard, AM_HAL_CARD_TYPE_EMMC, NULL, AM_HAL_CARD_PWR_CTRL_SDHC_OFF) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_delay_ms(1000);
        am_util_stdio_printf("card init failed, try again\n");
    }

#ifdef EMMC_DDR50_TEST
    //
    // 48MHz, 4-bit DDR mode for read and write
    //
    while (am_hal_card_cfg_set(&eMMCard, AM_HAL_CARD_TYPE_EMMC,
        AM_HAL_HOST_BUS_WIDTH_4, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
        AM_HAL_HOST_UHS_DDR50) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_delay_ms(1000);
        am_util_stdio_printf("setting DDR50 failed\n");
    }
#else
    //
    // 48MHz, 4-bit SDR mode for read and write
    //
    while (am_hal_card_cfg_set(&eMMCard, AM_HAL_CARD_TYPE_EMMC,
        AM_HAL_HOST_BUS_WIDTH_4, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
        AM_HAL_HOST_UHS_NONE) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_delay_ms(1000);
        am_util_stdio_printf("setting SDR50 failed\n");
    }
#endif

    for ( ; ; )
    {

#if 0
        //
        // Test the CID, CSD and EXT CSD parse function
        //
        am_util_stdio_printf("\n======== Test the CID, CSD and EXT CSD parse function ========\n");
        uint32_t emmc_psn;
        emmc_psn = am_hal_card_get_cid_field(&eMMCard, 16, 32);
        am_util_stdio_printf("Product Serial Number : 0x%x\n", emmc_psn);

        uint32_t emmc_csize;
        emmc_csize = am_hal_card_get_csd_field(&eMMCard,  62, 12);
        am_util_stdio_printf("Product CSD Size : 0x%x\n", emmc_csize);

        uint32_t max_enh_size_mult;
        uint32_t sec_count;
        max_enh_size_mult = am_hal_card_get_ext_csd_field(&eMMCard, 157, 3);
        sec_count = am_hal_card_get_ext_csd_field(&eMMCard, 212, 4);
        am_util_stdio_printf("Product EXT CSD Max Enh Size Multi : 0x%x\n", max_enh_size_mult);
        am_util_stdio_printf("Product EXT CSD Sector Count : 0x%x\n", sec_count);

        am_util_stdio_printf("============================================================\n\n");

#endif


#ifdef TEST_SSRAM
        //
        // write 4*512 bytes to emmc flash
        //
        am_util_stdio_printf("\n======== Test multiple block sync write and read in SSRAM ========\n");
        am_hal_card_block_write_sync(&eMMCard, START_BLK, 4, (uint8_t *)pui8WrBufSSRAM);

        memset((void *)pui8RdBufSSRAM, 0x0, 4*512);

        am_hal_card_block_read_sync(&eMMCard, START_BLK, 4, (uint8_t *)pui8RdBufSSRAM);

        //
        // check if block data match or not
        //
        check_if_data_match((uint8_t *)pui8RdBufSSRAM, (uint8_t *)pui8WrBufSSRAM, 512*4);
        am_util_stdio_printf("============================================================\n\n");

#endif


#ifdef TEST_SSRAM
        //
        // write 4*512 bytes to emmc flash
        //
        am_util_stdio_printf("\n======== Test multiple block sync write and read in SSRAM ========\n");
        am_hal_card_block_write_sync(&eMMCard, START_BLK, 4, (uint8_t *)pui8WrBufSSRAM);

        memset((void *)pui8RdBufSSRAM, 0x0, 4*512);

        am_hal_card_block_read_sync(&eMMCard, START_BLK, 4, (uint8_t *)pui8RdBufSSRAM);
        //
        // check if block data match or not
        //
        check_if_data_match((uint8_t *)pui8RdBufSSRAM, (uint8_t *)pui8WrBufSSRAM, 512*4);
        am_util_stdio_printf("============================================================\n\n");

#endif

#ifdef TEST_TCM

        //
        // write 512 bytes to emmc flash
        //
        am_util_stdio_printf("\n======== Test single block sync write and read in TCM ========\n");

        am_hal_card_block_write_sync(&eMMCard, START_BLK, 1, (uint8_t *)ui8WrBuf);

#ifdef TEST_POWER_SAVING

        //
        // Test the power saving feature
        //
        am_util_stdio_printf("\nCard power saving\n");
        am_hal_card_pwrctrl_sleep(&eMMCard);
        am_util_delay_ms(1000);
        am_util_stdio_printf("\nCard power wakeup\n");
        am_hal_card_pwrctrl_wakeup(&eMMCard);
#endif

        am_util_stdio_printf("\nRead 512 bytes block\n");

        memset((void *)ui8RdBuf, 0x0, BUF_LEN);

        am_hal_card_block_read_sync(&eMMCard, START_BLK, 1, (uint8_t *)ui8RdBuf);

        //
        // check if block data match or not
        //
        check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, 512);

        am_util_stdio_printf("============================================================\n\n");

#endif

        //
        // TCM DDR50 testing
        //
#if 0
        am_hal_card_cfg_set(&eMMCard, AM_HAL_CARD_TYPE_EMMC,
            AM_HAL_HOST_BUS_WIDTH_8, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
            AM_HAL_HOST_UHS_DDR50);

        am_util_debug_printf("\n======== Test multiple block sync write and read ========\n");

        uint32_t ui32Status;
        ui32Status = am_hal_card_block_write_sync(&eMMCard, START_BLK, BLK_NUM, (uint8_t *)ui8WrBuf);
        am_util_stdio_printf("Synchronous Writing %d blocks is done, Xfer Status %d\n", ui32Status >> 16, ui32Status & 0xffff);

#ifdef TEST_POWER_SAVING

        //
        // Test the power saving feature
        //
        am_util_stdio_printf("\nCard power saving\n");
        am_hal_card_pwrctrl_sleep(&eMMCard);
        am_util_delay_us(10);
        am_util_stdio_printf("\nCard power wakeup\n");
        am_hal_card_pwrctrl_wakeup(&eMMCard);
#endif

        memset((void *)ui8RdBuf, 0x0, BUF_LEN);

        ui32Status = am_hal_card_block_read_sync(&eMMCard, START_BLK, BLK_NUM, (uint8_t *)ui8RdBuf);
        am_util_stdio_printf("Synchronous Reading %d blocks is done, Xfer Status %d\n", ui32Status >> 16, ui32Status & 0xffff);

        //
        // check if block data match or not
        //
        check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, BUF_LEN);

        am_util_stdio_printf("============================================================\n\n");

#endif

#if 0

        am_hal_card_cfg_set(&eMMCard, AM_HAL_CARD_TYPE_EMMC,
            AM_HAL_HOST_BUS_WIDTH_4, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
            AM_HAL_HOST_UHS_NONE);

        //
        // async read & write, card insert & remove needs a callback function
        //
        am_util_debug_printf("========  Test multiple block async write and read  ========\n");
        am_hal_card_register_evt_callback(&eMMCard, am_hal_card_event_test_cb);

        //
        // Write 'BLK_NUM' blocks to the eMMC flash
        //
        bAsyncWriteIsDone = false;
        am_hal_card_block_write_async(&eMMCard, START_BLK, BLK_NUM, (uint8_t *)ui8WrBuf);

        //
        // wait until the async write is done
        //
        while (!bAsyncWriteIsDone)
        {
            am_util_delay_ms(1000);
            am_util_stdio_printf("waiting the asynchronous block write to complete\n");
        }

        am_util_stdio_printf("asynchronous block write is done\n");

#ifdef TEST_POWER_SAVING
        //
        // Test the power saving feature
        //

        am_util_stdio_printf("\nCard power saving\n");
        am_hal_card_pwrctrl_sleep(&eMMCard);
        am_util_delay_ms(10);
        am_util_stdio_printf("\nCard power wakeup\n");
        am_hal_card_pwrctrl_wakeup(&eMMCard);

#endif
        //
        // Read 'BLK_NUM' blocks to the eMMC flash
        //
        bAsyncReadIsDone = false;
        memset((void *)ui8RdBuf, 0x0, BUF_LEN);
        am_hal_card_block_read_async(&eMMCard, START_BLK, BLK_NUM, (uint8_t *)ui8RdBuf);

        //
        // wait until the async read is done
        //
        while (!bAsyncReadIsDone)
        {
            am_util_delay_ms(1000);
            am_util_stdio_printf("waiting the asynchronous block read to complete\n");
        }

        check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, BUF_LEN);

        am_util_stdio_printf("============================================================\n\n");

#endif

    } // End of for loop
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
