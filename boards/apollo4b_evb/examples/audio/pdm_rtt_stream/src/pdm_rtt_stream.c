//*****************************************************************************
//
//! @file pdm_rtt_stream.c
//!
//! @brief An example to show PDM audio streaming to PC over RTT data logger.
//!
//! @addtogroup audio_examples Audio Examples
//!
//! @defgroup pdm_rtt_stream PDM RTT Stream Example
//! @ingroup audio_examples
//! @{
//!
//! Purpose: This example enables the PDM interface to record audio signals from an
//! external microphone.<br>
//! <br>
//! Build and download the program into the target device.<br>
//! Reset the target and check the SWO print for PDM settings.<br>
//! Run the helper script 'rtt_logger.py' in the project folder<br>
//!
//! @note python3 rtt_logger.py
//!
//! (In this example, RTT control block is mapped to a fixed address to facilitate<br>
//!  the searching process. If the address is changed, make sure to modify<br>
//!  the rtt_logger.py script to match the address.)<br>
//! <br>
//! During data streaming, press any key to stop the recording.<br>
//! The audio captured is stored into the same folder as the rtt_logger.py,<br>
//! with a file name of yyyymmdd-hh-mm-ss.pcm.<br>
//! <br>
//! To check the audio, load the *.pcm file into audio analysis tools and check.<br>
//! E.g. in Audacity, https://www.audacityteam.org/:<br>
//! - File -> Import -> Raw data...<br>
//! - Default import format is:<br>
//!   - Signed 24-bit PCM<br>
//!   - Little-endian<br>
//!   - 2 Channels (Stereo)<br>
//!   - Start offset: 0 bytes<br>
//!   - Amount to import: 100%<br>
//!   - Sample rate: 16000 Hz<br>
//! <br>
//! The required pin connections are:<br>
//!   - PDM0 -<br>
//!     GPIO 50 - PDM0 CLK<br>
//!     GPIO 51 - PDM0 DATA<br>
//! <br>
//!   - PDM1 -<br>
//!     GPIO 52 - PDM1 CLK<br>
//!     GPIO 53 - PDM1 DATA<br>
//! <br>
//!   - PDM2 -<br>
//!     GPIO 54 - PDM2 CLK<br>
//!     GPIO 55 - PDM2 DATA<br>
//! <br>
//!   - PDM3 -<br>
//!     GPIO 56 - PDM3 CLK<br>
//!     GPIO 57 - PDM3 DATA<br>
//!
//! @note On Apollo4l, there is only 1 PDM instance (PDM0).
//!
//! Additional Information:<br>
//! Printing takes place over the ITM at 1M Baud.<br>
//! RTT logger takes place over the SWD at 4M Baud.<br>
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
#include "SEGGER_RTT.h"

//*****************************************************************************
//
// Example option parameters.
//
//*****************************************************************************
#define     PDM_MODULE          0   // PDM module to select. On Apollo4l, only PDM0 is available.

#define     USE_DMA             1   // 0 = use FIFO only, 1 = use DMA

//! PDM interrupts.
static const IRQn_Type pdm_interrupts[] =
{
    PDM0_IRQn,
#ifndef AM_PART_APOLLO4L
    PDM1_IRQn,
    PDM2_IRQn,
    PDM3_IRQn
#endif
};


//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
volatile bool g_bPDMDataReady = false;

//
// PDM buffer with or without DMA
//
#if USE_DMA
// should be padded to 16Bytes
#define BUFFER_SIZE_BYTES       1024                // 4 bytes per mono sample,
                                                    // 8 bytes per stereo sample
#define DMA_BYTES               BUFFER_SIZE_BYTES   // Same as buffer size

#else

#define BUFFER_SIZE_BYTES       128                 // 4 bytes per mono sample,
                                                    // 8 bytes per stereo sample
#endif

#define FIFO_THRESHOLD_CNT      16
AM_SHARED_RW uint32_t g_ui32PDMDataBuffer[BUFFER_SIZE_BYTES / 4 + BUFFER_SIZE_BYTES / 4 + 3];
//
// RTT buffer
//
#define RTT_BUFFER_LENGTH       (256*1024)
uint8_t g_rttRecorderBuffer[RTT_BUFFER_LENGTH];

// AXI Scratch buffer
// On Apollo4B, need to allocate 20 Words even though we only need 16, to ensure we have 16 Byte alignment
#ifdef AM_PART_APOLLO4B
AM_SHARED_RW uint32_t axiScratchBuf[20];
#endif

//*****************************************************************************
//
// PDM configuration information.
//
//*****************************************************************************
void *PDMHandle;

am_hal_pdm_config_t g_sPdmConfig =
{
    //
    // Example setting:
    //  1.5MHz PDM CLK OUT:
    //      AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ, AM_HAL_PDM_MCLKDIV_1, AM_HAL_PDM_PDMA_CLKO_DIV7
    //  16.00KHz 24bit Sampling:
    //      DecimationRate = 48
    //
    .ePDMClkSpeed = AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ,
    .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
    .ePDMAClkOutDivder = AM_HAL_PDM_PDMA_CLKO_DIV7,
    .ui32DecimationRate = 48,

    .eLeftGain = AM_HAL_PDM_GAIN_P210DB,
    .eRightGain = AM_HAL_PDM_GAIN_P210DB,
    .eStepSize = AM_HAL_PDM_GAIN_STEP_0_13DB,

    .bHighPassEnable = AM_HAL_PDM_HIGH_PASS_ENABLE,
    .ui32HighPassCutoff = 0x3,
    .bDataPacking = 1,
    .ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO,

    .bPDMSampleDelay = AM_HAL_PDM_CLKOUT_PHSDLY_NONE,
    .ui32GainChangeDelay = AM_HAL_PDM_CLKOUT_DELAY_NONE,

    .bSoftMute = 0,
    .bLRSwap = 0,
};

am_hal_pdm_transfer_t sTransfer =
{
    .ui32TargetAddr        = 0x0,
    .ui32TargetAddrReverse = 0x0,
    .ui32TotalCount        = BUFFER_SIZE_BYTES,
};

//*****************************************************************************
//
// PDM initialization.
//
//*****************************************************************************
void
pdm_init(void)
{
    //
    // Initialize, power-up, and configure the PDM.
    //
    am_hal_pdm_initialize(PDM_MODULE, &PDMHandle);
    am_hal_pdm_power_control(PDMHandle, AM_HAL_PDM_POWER_ON, false);

    // use external XTHS, not reference clock
    am_hal_mcuctrl_control_arg_t ctrlArgs = g_amHalMcuctrlArgDefault;
    ctrlArgs.ui32_arg_hfxtal_user_mask  = 1 << (AM_HAL_HCXTAL_PDM_BASE_EN + PDM_MODULE);

    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START, (void *) &ctrlArgs);

    // enable HFRC2
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFRC2_START, false);
    am_util_delay_us(200);      // wait for FLL to lock

    // set HF2ADJ for 24.576MHz output
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE, false);
    am_util_delay_us(500);      // wait for adj to apply

    am_hal_pdm_configure(PDMHandle, &g_sPdmConfig);

    //
    // Configure the necessary pins.
    //

    uint32_t ui32PDMModule = PDM_MODULE;

    am_bsp_pdm_pins_enable(ui32PDMModule);

    am_hal_pdm_fifo_threshold_setup(PDMHandle, FIFO_THRESHOLD_CNT);

    //
    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    //
#if USE_DMA
    am_hal_pdm_interrupt_enable(PDMHandle, (AM_HAL_PDM_INT_DERR
                                            | AM_HAL_PDM_INT_DCMP
                                            | AM_HAL_PDM_INT_UNDFL
                                            | AM_HAL_PDM_INT_OVF));
#else
    am_hal_pdm_interrupt_enable(PDMHandle, (AM_HAL_PDM_INT_DERR
                                            | AM_HAL_PDM_INT_UNDFL
                                            | AM_HAL_PDM_INT_OVF
                                            | AM_HAL_PDM_INT_THR));
#endif

    NVIC_SetPriority(pdm_interrupts[PDM_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(pdm_interrupts[PDM_MODULE]);

}

//*****************************************************************************
//
// Print PDM configuration data.
//
//*****************************************************************************
void
pdm_config_print(void)
{
    uint32_t ui32PDMClk;
    uint32_t ui32MClkDiv;
    uint32_t ui32DivClkQ;
    uint32_t ui32SampleFreq;

    //
    // Read the config structure to figure out what our internal clock is set
    // to.
    //
    switch (g_sPdmConfig.eClkDivider)
    {
        case AM_HAL_PDM_MCLKDIV_3: ui32MClkDiv = 3; break;
        case AM_HAL_PDM_MCLKDIV_2: ui32MClkDiv = 2; break;
        case AM_HAL_PDM_MCLKDIV_1: ui32MClkDiv = 1; break;

        default:
            ui32MClkDiv = 1;
    }

    switch (g_sPdmConfig.ePDMAClkOutDivder)
    {
        case AM_HAL_PDM_PDMA_CLKO_DIV15: ui32DivClkQ = 15; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV14: ui32DivClkQ = 14; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV13: ui32DivClkQ = 13; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV12: ui32DivClkQ = 12; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV11: ui32DivClkQ = 11; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV10: ui32DivClkQ = 10; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV9: ui32DivClkQ = 9; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV8: ui32DivClkQ = 8; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV7: ui32DivClkQ = 7; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV6: ui32DivClkQ = 6; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV5: ui32DivClkQ = 5; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV4: ui32DivClkQ = 4; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV3: ui32DivClkQ = 3; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV2: ui32DivClkQ = 2; break;
        case AM_HAL_PDM_PDMA_CLKO_DIV1: ui32DivClkQ = 1; break;
        default:
            ui32DivClkQ = 1;
    }

    switch (g_sPdmConfig.ePDMClkSpeed)
    {
        case AM_HAL_PDM_CLK_HFRC2ADJ_24_576MHZ:     ui32PDMClk = 24576000; break;
        case AM_HAL_PDM_CLK_HFXTAL:                 ui32PDMClk = 32000000; break;
        case AM_HAL_PDM_CLK_HFRC_24MHZ:             ui32PDMClk = 24000000; break;

        default:
            ui32PDMClk = 24000000;
    }

    ui32SampleFreq = (ui32PDMClk /
                      ((ui32MClkDiv + 1) * (ui32DivClkQ + 1) * 2 * g_sPdmConfig.ui32DecimationRate));

    am_util_stdio_printf("PDM Settings:\n");
    am_util_stdio_printf("PDM Clock (Hz):         %12d\n", ui32PDMClk);
    am_util_stdio_printf("Decimation Rate:        %12d\n", g_sPdmConfig.ui32DecimationRate);
    am_util_stdio_printf("Effective Sample Freq.: %12d\n", ui32SampleFreq);
}

//*****************************************************************************
//
// pcm_rtt_record
//
//*****************************************************************************
void pcm_rtt_record(void* pBuffer, uint32_t NumBytes)
{
    SEGGER_RTT_Write(1, (uint8_t*)pBuffer, NumBytes);
}

//
// Take over the interrupt handler for whichever PDM we're using.
//
#define example_pdm_isr     am_pdm_isr1(PDM_MODULE)
#define am_pdm_isr1(n)      am_pdm_isr(n)
#define am_pdm_isr(n)       am_pdm ## n ## _isr

//*****************************************************************************
//
// PDM interrupt handler.
//
//*****************************************************************************
void
example_pdm_isr(void)
{
    uint32_t ui32Status;

    //
    // Read the interrupt status.
    //
    am_hal_pdm_interrupt_status_get(PDMHandle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(PDMHandle, ui32Status);

#if USE_DMA
    //
    // DMA transaction
    //
    am_hal_pdm_interrupt_service(PDMHandle, ui32Status, &sTransfer);
    if (ui32Status & AM_HAL_PDM_INT_DCMP)
    {
        g_bPDMDataReady = true;
    }
#else
    if (ui32Status & AM_HAL_PDM_INT_THR)
    {
        g_bPDMDataReady = true;
    }

    if (ui32Status & AM_HAL_PDM_INT_OVF)
    {

        am_hal_pdm_fifo_flush(PDMHandle);
    }
#endif // #if USE_DMA
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("==============================================\n");
    am_util_stdio_printf("PDM RTT streaming example.\n\n");

    //
    // Initialize RTT
    //
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(1, "DataLogger", g_rttRecorderBuffer, RTT_BUFFER_LENGTH, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#if defined(AM_PART_APOLLO4B)
    //
    // Set up scratch AXI buf (needs 64B - aligned to 16 Bytes)
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_AXIMEM, (uint8_t *)((uint32_t)(axiScratchBuf + 3) & ~0xF));
#else
    //
    // Since low power init is not called in this example, we need the call below to init DAXI
    //
    am_hal_daxi_control(AM_HAL_DAXI_CONTROL_ENABLE, NULL);
#endif

    //
    // PDM DMA data config:
    //   DMA buffers padded at 16B alignment.
    //
    uint32_t ui32PDMDataPtr = (uint32_t)((uint32_t)(g_ui32PDMDataBuffer + 3) & ~0xF);
    sTransfer.ui32TargetAddr = ui32PDMDataPtr;
    sTransfer.ui32TargetAddrReverse = sTransfer.ui32TargetAddr + sTransfer.ui32TotalCount;

    //
    // Initialize PDM-to-PCM module
    //
    pdm_init();

    // enable HP model for faster processing
    am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);

    //
    // Print compiler version and rtt control block address
    //
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
    am_util_stdio_printf("RTT Control Block Address:  0x%08X\n", (uint32_t)&_SEGGER_RTT);

    //
    // Print the PDM configuration
    //
    pdm_config_print();

    //
    // Enable interrupts
    //
    am_hal_pdm_enable(PDMHandle);

    am_util_stdio_printf("Disable SWO print and starting RTT streaming...\n");
    am_util_stdio_printf("==============================================\n");

    am_util_delay_ms(20);

    am_bsp_itm_printf_disable();
    am_hal_interrupt_master_enable();

    //
    // Start data conversion
    //
    am_hal_pdm_dma_start(PDMHandle, &sTransfer);

    //
    // Loop forever.
    //
    while (1)
    {
        if (g_bPDMDataReady)
        {
            g_bPDMDataReady = false;
            //
            // Re-arrange data
            //
            uint32_t* ui32PDMDatabuffer = (uint32_t*)am_hal_pdm_dma_get_buffer(PDMHandle);
            uint8_t* temp1 = (uint8_t*)ui32PDMDatabuffer;

            for ( uint32_t i = 0; i < (DMA_BYTES / 4); i++ )
            {
                temp1[3*i]     = ui32PDMDatabuffer[i] & 0xFF;
                temp1[3*i + 1] = (ui32PDMDatabuffer[i] & 0xFF00) >> 8U;
                temp1[3*i + 2] = (ui32PDMDatabuffer[i] & 0xFF0000) >> 16U;
            }

            //
            // Record the DMA buffer.
            //
            pcm_rtt_record(ui32PDMDatabuffer, DMA_BYTES * 3 / 4);
        }

    }
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

