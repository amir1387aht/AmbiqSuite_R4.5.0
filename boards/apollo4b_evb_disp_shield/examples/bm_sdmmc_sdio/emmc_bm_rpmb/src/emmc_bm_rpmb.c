//*****************************************************************************
//
//! @file emmc_bm_rpmb.c
//!
//! @brief emmc rpmb example.
//!
//! @addtogroup bm_sdmmc_sdio_examples SDIO Examples
//!
//! @defgroup emmc_bm_rpmb eMMC Replay Protected Memory Block Example
//! @ingroup bm_sdmmc_sdio_examples
//! @{
//!
//! Purpose: This example demonstrates how to use APIs in eMMC RPMB driver
//!          to access RPMB partition.<br>
//! <br>
//!   1) Initialize for low power<br>
//!   2) Initialize Card Host<br>
//!   3) Config Card Host<br>
//!   4) Init RPMB Device Driver<br>
//!   5) Switch Partitions to RPMB Memory block<br>
//!   6) Perform Read/Write Test<br>
//!   7) DeInit RPMB Device Driver<br>
//!   8) Sleep/Wakeup<br>
//!   9) Perform Read/Write Test<br>
//!   10) Sleep Forever
//! <br>
//! Additional Information:
//! Debug messages will be sent over ITM/SWO at 1M Baud.<br>
//! <br>
//! If ITM is not shut down, the device will never achieve deep sleep, only<br>
//! normal sleep, due to the ITM (and thus the HFRC) being enabled.<br>
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
#include "am_devices_emmc_rpmb.h"

#define ALIGN(x) __attribute__((aligned(1 << x)))

#define BLK_SIZE  (512)
#define BLK_NUM   (4)
#define BUF_LEN   (BLK_SIZE*BLK_NUM)

#define START_BLK (0)
#define TEST_BLK_CNT (2)

volatile uint8_t ui8RdBuf[BUF_LEN] ALIGN(12);
volatile uint8_t ui8WrBuf[BUF_LEN] ALIGN(12);

am_hal_card_host_t *pSdhcCardHost = NULL;
am_hal_card_t eMMCard;

void am_sdio_isr(void)
{
    uint32_t ui32IntStatus;

    am_hal_sdhc_intr_status_get(pSdhcCardHost->pHandle, &ui32IntStatus, true);
    am_hal_sdhc_intr_status_clear(pSdhcCardHost->pHandle, ui32IntStatus);
    am_hal_sdhc_interrupt_service(pSdhcCardHost->pHandle, ui32IntStatus);
}

void check_if_data_match(uint8_t *pui8RdBuf, uint8_t *pui8WrBuf, uint32_t ui32Len)
{
    uint32_t i;
    for (i = 0; i < ui32Len; i++)
    {
        if (pui8RdBuf[i] != pui8WrBuf[i])
        {
            am_util_stdio_printf("Actual[%d] = %d and Expected[%d] = %d\n", i, pui8RdBuf[i], i, pui8WrBuf[i]);
            break;
        }
    }

    if (i == ui32Len)
    {
        am_util_stdio_printf("data matched\n");
    }
}

void emmc_init(void)
{
    //
    // Configure SDIO PINs.
    //
    am_bsp_sdio_pins_enable(AM_HAL_HOST_BUS_WIDTH_8);

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

    //
    // Check if card is present
    //
    while (am_hal_card_host_find_card(pSdhcCardHost, &eMMCard) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("No card is present now\n");
        am_util_delay_ms(1000);
        am_util_stdio_printf("Checking if card is available again\n");
    }

    //
    // Card init
    //
    while (am_hal_card_init(&eMMCard, AM_HAL_CARD_TYPE_EMMC, NULL, AM_HAL_CARD_PWR_CTRL_SDHC_OFF) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_delay_ms(1000);
        am_util_stdio_printf("card init failed, try again\n");
    }

    //
    // 48MHz, 8-bit SDR mode for read and write
    //
    while (am_hal_card_cfg_set(&eMMCard, AM_HAL_CARD_TYPE_EMMC,
        AM_HAL_HOST_BUS_WIDTH_8, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
        AM_HAL_HOST_UHS_NONE) != AM_HAL_STATUS_SUCCESS)
    {
        am_util_delay_ms(1000);
        am_util_stdio_printf("setting SDR50 failed\n");
    }

    //
    // Test the CID, CSD and EXT CSD parse function
    //
    uint32_t emmc_psn;
    emmc_psn = am_hal_card_get_cid_field(&eMMCard, 16, 32);
    am_util_stdio_printf("Product Serial Number : 0x%x\n", emmc_psn);

    uint32_t emmc_csize;
    emmc_csize = am_hal_card_get_csd_field(&eMMCard,  62, 12);
    am_util_stdio_printf("Product CSD Size : 0x%x\n", emmc_csize);

    uint32_t max_enh_size_mult;
    uint32_t sec_count;
    uint32_t rpmb_size_mult;
    max_enh_size_mult = am_hal_card_get_ext_csd_field(&eMMCard, 157, 3);
    sec_count = am_hal_card_get_ext_csd_field(&eMMCard, 212, 4);
    rpmb_size_mult = am_hal_card_get_ext_csd_field(&eMMCard, 168, 1);
    am_util_stdio_printf("Product EXT CSD Max Enh Size Multi : 0x%x\n", max_enh_size_mult);
    am_util_stdio_printf("Product EXT CSD Sector Count : 0x%x\n", sec_count);
    am_util_stdio_printf("Product EXT CSD RPMB Size Multi: 0x%x\n", rpmb_size_mult);

}

char *dummy_key_hash =
{
    "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHH"
};

uint8_t keyhash[AM_DEVICES_EMMC_RPMB_MAC_SIZE];

bool setkey(void)
{
    uint32_t ui32Stat;
    am_util_id_t sIdDevice;

    //
    // Print the device info.
    //
    am_util_id_device(&sIdDevice);
    am_util_stdio_printf("Vendor Name:  %s\n", sIdDevice.pui8VendorName);
    am_util_stdio_printf("Device type:  %s\n", sIdDevice.pui8DeviceName);
    am_util_stdio_printf("Device Info:\n"
                         "\tPart number:  0x%08X\n"
                         "\tChip ID0:     0x%08X\n"
                         "\tChip ID1:     0x%08X\n"
                         "\tRevision:     0x%08X (Rev%c%c)\n",
                         sIdDevice.sMcuCtrlDevice.ui32ChipPN,
                         sIdDevice.sMcuCtrlDevice.ui32ChipID0,
                         sIdDevice.sMcuCtrlDevice.ui32ChipID1,
                         sIdDevice.sMcuCtrlDevice.ui32ChipRev,
                         sIdDevice.ui8ChipRevMaj, sIdDevice.ui8ChipRevMin );

    //
    // Example convertion of id into 256bit key
    //
    memcpy(keyhash, &sIdDevice, AM_DEVICES_EMMC_RPMB_MAC_SIZE);

    //
    // Switch to rpmb partition
    //
    if ( am_hal_card_get_ext_csd_field(&eMMCard, MMC_EXT_REGS_PARTITON_CONFIG, 1) != AM_DEVICES_EMMC_RPMB_ACCESS )
    {
        if ( am_devices_emmc_rpmb_partition_switch(&eMMCard, AM_DEVICES_EMMC_RPMB_ACCESS) != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
        {
            am_util_stdio_printf("eMMC switch partition failed\n");
            return -1;
        }
    }

    //
    // Set 256bit key to emmc
    //
#if defined(SET_REALY_KEY)
    ui32Stat = am_devices_emmc_rpmb_set_key(&eMMCard, keyhash);
#else
    ui32Stat = am_devices_emmc_rpmb_set_key(&eMMCard, (uint8_t *)dummy_key_hash);
#endif
    if (ui32Stat != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS)
    {
        am_util_stdio_printf("eMMC set key failed, Stat: %d\n", ui32Stat);
        return -1;
    }
    return true;
}

bool checkkey(uint32_t *ui32Stat)
{
    uint32_t ui32Writecnt;

    //
    // Switch to rpmb partition
    //
    if ( am_hal_card_get_ext_csd_field(&eMMCard, MMC_EXT_REGS_PARTITON_CONFIG, 1) != AM_DEVICES_EMMC_RPMB_ACCESS )
    {
        if ( am_devices_emmc_rpmb_partition_switch(&eMMCard, AM_DEVICES_EMMC_RPMB_ACCESS) != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
        {
            am_util_stdio_printf("eMMC switch partition failed\n");
            return -1;
        }
    }

    //
    // Get counter
    //
    *ui32Stat = am_devices_emmc_rpmb_get_counter(&eMMCard, &ui32Writecnt, (uint8_t *)dummy_key_hash);
    if ( *ui32Stat != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
    {
        am_util_stdio_printf("eMMC failed to get write counter, Stat:%d\n", *ui32Stat);
    }
    else
    {
        am_util_stdio_printf("eMMC writer counter: %d\n", ui32Writecnt);
    }

    return true;
}

int
main(void)
{
    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Enable printing to the console.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Apollo4 emmc rpmb example.\n\n");

    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    am_devices_emmc_mebedtls_init();

    emmc_init();

    //
    // Switch to rpmb partition
    //
    if ( am_hal_card_get_ext_csd_field(&eMMCard, MMC_EXT_REGS_PARTITON_CONFIG, 1) != AM_DEVICES_EMMC_RPMB_ACCESS )
    {
        if ( am_devices_emmc_rpmb_partition_switch(&eMMCard, AM_DEVICES_EMMC_RPMB_ACCESS) != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
        {
            am_util_stdio_printf("eMMC switch partition failed\n");
            return -1;
        }
    }

    //
    // Initialize write buffers
    //
    for (int i = 0; i < BUF_LEN; i++)
    {
        ui8WrBuf[i] = i % 0xFF;
    }

    uint32_t ui32Stat = AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS;
    uint32_t ui32BlockCount = RPMB_BLKCNT_MAX(eMMCard.ui32RpmbSizeMult);

    //
    // Check whether key is programmed
    //
    if ( checkkey(&ui32Stat) )
    {
        if ( ui32Stat == AM_DEVICES_EMMC_RPMB_STATUS_KEY_NOT_PROGRAMMED_ERROR )
        {
            if ( setkey() )
            {
                am_util_stdio_printf("Key set successful\n");
            }
            else
            {
                am_util_stdio_printf("Key set failed\n");
                goto RPMB_END;
            }
            checkkey(&ui32Stat);
        }

        //
        // Start write & read test
        //
        for ( uint32_t ui32StartBlk = 0; ui32StartBlk < ui32BlockCount; ui32StartBlk += ui32BlockCount / 2 - 1 )
        {
            am_util_stdio_printf("Testing RPMB block#:%d\n", ui32StartBlk);
            if ( am_devices_emmc_rpmb_write(&eMMCard, (uint8_t *)ui8WrBuf, ui32StartBlk, TEST_BLK_CNT, (uint8_t *)dummy_key_hash) != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
            {
                am_util_stdio_printf("eMMC rpmb write failed\n");
            }

            memset((void *)ui8RdBuf, 0x0, BUF_LEN);

            if ( am_devices_emmc_rpmb_read(&eMMCard, (uint8_t *)ui8RdBuf, ui32StartBlk, TEST_BLK_CNT, (uint8_t *)dummy_key_hash) != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
            {
                am_util_stdio_printf("eMMC rpmb read failed\n");
            }

            check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, AM_DEVICES_EMMC_RPMB_DATA_SIZE * TEST_BLK_CNT);
        }
    }

RPMB_END:
    am_devices_emmc_mebedtls_deinit();

    //
    // Switch to normal partition
    //
    if ( am_hal_card_get_ext_csd_field(&eMMCard, MMC_EXT_REGS_PARTITON_CONFIG, 1) != AM_DEVICES_EMMC_NO_BOOT_ACCESS )
    {
        if ( am_devices_emmc_rpmb_partition_switch(&eMMCard, AM_DEVICES_EMMC_NO_BOOT_ACCESS) != AM_DEVICES_EMMC_RPMB_STATUS_SUCCESS )
        {
            am_util_stdio_printf("eMMC switch partition failed\n");
            return -1;
        }
    }
    am_util_stdio_printf("\nTesting normal write & read\n");

    //
    // Write 512 bytes to emmc flash
    //
    ui32Stat = am_hal_card_block_write_sync(&eMMCard, START_BLK, BLK_NUM, (uint8_t *)ui8WrBuf);
    am_util_stdio_printf("Synchronous Writing %d blocks is done, Xfer Status %d\n", ui32Stat >> 16, ui32Stat & 0xffff);
    if ( (ui32Stat & 0xffff) != 0 )
    {
        am_util_stdio_printf("Failed to write card.\n");
        return -1;
    }

    //
    // Test the power saving feature
    //
#ifdef TEST_POWER_SAVING
    //
    // Power down SDIO peripheral
    //
    am_util_stdio_printf("\nCard power saving\n");
    ui32Stat = am_hal_card_pwrctrl_sleep(&eMMCard);
    if ( ui32Stat != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Failed to power down card\n");
        return -1;
    }

    am_util_delay_ms(1000);

    //
    // Power up SDIO peripheral
    //
    am_util_stdio_printf("\nCard power wakeup\n");
    ui32Stat = am_hal_card_pwrctrl_wakeup(&eMMCard);
    if ( ui32Stat != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Failed to power up card.\n");
        return -1;
    }
#endif

    memset((void *)ui8RdBuf, 0x0, BUF_LEN);

    //
    // Read 512 bytes from emmc
    //
    ui32Stat = am_hal_card_block_read_sync(&eMMCard, START_BLK, BLK_NUM, (uint8_t *)ui8RdBuf);
    am_util_stdio_printf("Synchronous Reading %d blocks is done, Xfer Status %d\n", ui32Stat >> 16, ui32Stat & 0xffff);
    if ( (ui32Stat & 0xffff) != 0 )
    {
        am_util_stdio_printf("Failed to read card.\n");
        return -1;
    }

    //
    // Check if block data match or not
    //
    check_if_data_match((uint8_t *)ui8RdBuf, (uint8_t *)ui8WrBuf, BLK_NUM * BLK_SIZE);

    am_util_stdio_printf("Apollo4 emmc rpmb example complete.\n");
    am_util_stdio_printf("============================================================\n\n");

    ui32Stat = am_hal_card_pwrctrl_sleep(&eMMCard);
    if ( ui32Stat != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Failed to power down card\n");
        return -1;
    }
    //
    // Sleep forever
    //
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
