//*****************************************************************************
//
//! @file uart_async.c
//!
//! @brief A uart example that demonstrates the async driver.
//!
//! @addtogroup peripheral_examples Peripheral Examples
//
//! @defgroup uart_async UART Asynchronous Example
//! @ingroup peripheral_examples
//! @{
//!
//! Purpose: This example demonstrates the usage of the asynchronous UART driver.<br>
//! This driver allows the application to append UART Tx data to the
//! Tx queue with interrupt code managing queue transmission.<br><br>
//!
//! Similarly, the interrupt code will move received data into the Rx queue
//! and the application periodically reads from the Rx queue.<br><br>
//!
//! The Rx timeout interrupt has been enabled in this example.<br>
//! If insufficient Rx data triggers the FIFO full interrupt,
//! the Rx timeout interrupt activates after a fixed delay.<br><br>
//!
//! The associated ISR handler am_hal_uart_interrupt_queue_service() will return
//! status in a bitfield, suitable for use as a callback or polling.<br><br>
//!
//! Default Configuration:<br>
//! By default, this example uses UART1. The I/O pins used are defined in the BSP
//! file as AM_BSP_GPIO_UART1_TX and AM_BSP_GPIO_UART1_RX<br><br>
//!
//! Configuration and Operation:<br>
//! - This example requires enabling Tx and Rx fifos and queues.<br>
//! - It operates in a non-blocking manner without using callbacks.<br>
//! - The example monitors (polls) the Rx input queue.<br>
//! - It will transmit small blocks of data every second.<br><br>
//!
//! To interact with these pins from a PC, the user should obtain a 1.8v uart/usb
//! cable (FTDI, etc.).<br>
//! Using a terminal application (CoolTerm, RealTerm, Minicomm, etc.),
//! the user will see data buffers being sent from the example
//! (a different buffer every second), and the user can send data by typing.<br>
//! The swo output will report the character count the example receives.<<br><br>
//!
//! The SWO output will send Rx/Tx status and error information.
//! SWO Printing takes place over the ITM at 1M Baud.
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
#include <string.h>

//*****************************************************************************
//
// Macro Definition.
//
//*****************************************************************************
#define UART_ID                            (1)
#define TIMER_NUM       0               ///< Timer number used in the example
#define MAX_UART_PACKET_SIZE            (2048)
#define UART_RX_TIMEOUT_MS              (5)

//
// create buffer that will cause at least two fifo refills
//
uint8_t txBuf[74] =
{
    'a', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    //'0','1','2','3','4','5','6','7','8', '9',
    'z', '\r', '\n', ' ',
};

//
// create a buffer that will not require a fifo refil
// (so no tx interrupt, just tx complete)
//
uint8_t txBuf2[14] =
{
    'a', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'y', '\r', '\n', ' ',
};

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************

//
//! enumerate uart names
//
typedef enum
{
#if defined(AM_BSP_GPIO_UART0_TX) && defined(AM_BSP_GPIO_UART0_RX)
    eUART0,
#endif
#if defined(AM_BSP_GPIO_UART1_TX) && defined(AM_BSP_GPIO_UART1_RX)
    eUART1,
#endif
#if defined(AM_BSP_GPIO_UART2_TX) && defined(AM_BSP_GPIO_UART2_RX)
    eUART2,
#endif
#if defined(AM_BSP_GPIO_UART3_TX) && defined(AM_BSP_GPIO_UART3_RX)
    eUART3,
#endif
    eMAX_UARTS,
    euidX32 = 0x70000000,  // force this to 32bit value on all compilers
}
uart_id_e;

//
//! define data used per uart instance
//
typedef struct
{
    void *pvUART;
    uint32_t ui32ISrErrorCount;
    am_hal_uart_status2_t e32Status;
    //
    // These are used when each uart has different sized buffers
    //
    uint32_t ui32TxBuffSize;
    uint32_t ui32RxBuffSize;

    //
    // in this example all uart buffers are the same size,
    // this is not needed and can be modified
    //
    uint8_t pui8UARTTXBuffer[MAX_UART_PACKET_SIZE];
    uint8_t pui8UARTRXBuffer[MAX_UART_PACKET_SIZE];

    uart_id_e uartId;
    volatile bool bRxComplete;

}
uart_async_local_vars_t;

//
//! list uarts used 0-3 in this example
//! each entry must be unique, there is no error checking for that
//
#define NUM_UARTS_USED 1

//
//! define each UARTs used here
//! @note Ensure each uart used is properly defined in the table: gs_UartPinDefs
//
static const uart_id_e uartsUsed[NUM_UARTS_USED] =
{
    eUART1,
    //eUART3,
    //eUART2

};

//
// define global ram used in this example
//
typedef struct
{
    uart_async_local_vars_t tUartVars[NUM_UARTS_USED];

    uart_async_local_vars_t *uartIsrMap[eMAX_UARTS];

    bool bTimerIntOccured;
}
uart_example_ramGlobals_t;

//
// allocate global ram
//
uart_example_ramGlobals_t g_localv;

//
// define const table below
//

//
//! gpio pin map for each uart
//
typedef struct
{
    uint32_t pinNUmber;
    am_hal_gpio_pincfg_t *pinCfg;
}
uart_pin_descr_t;

typedef struct
{
    am_hal_gpio_pincfg_t *pinCfg;
    uart_pin_descr_t rxPin;
    uart_pin_descr_t txPin;
    const  am_hal_uart_config_t *pUartConfigs;
    uart_id_e uartId;
    uint8_t   uartHwIndex;

}
uart_defs_t;

//
//! Uart configs used.
//! Modify and add additional configs as needed.
//! Use this in gs_UartPinDefs below.
//
static const am_hal_uart_config_t sUartConfig =
{
    //
    // Standard UART settings: 115200-8-N-1
    //
    .ui32BaudRate    = 115200,
    //
    // Set TX and RX FIFOs to interrupt at three-quarters full.
    //
    .eDataBits    = AM_HAL_UART_DATA_BITS_8,
    .eParity      = AM_HAL_UART_PARITY_NONE,
    .eStopBits    = AM_HAL_UART_ONE_STOP_BIT,
    .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
    .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_4,
    .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_24,
};

//
//! Uart pin definitions
//! This defaults with standard BSP pin definitions.
//
static const uart_defs_t gs_UartPinDefs[eMAX_UARTS] =
{
#if defined(AM_BSP_GPIO_UART0_TX) && defined(AM_BSP_GPIO_UART0_RX)
[eUART0] = {
        .uartId = eUART0,
        .rxPin = {AM_BSP_GPIO_COM_UART_RX, &g_AM_BSP_GPIO_COM_UART_RX},
        .txPin = {AM_BSP_GPIO_COM_UART_TX, &g_AM_BSP_GPIO_COM_UART_TX},
        .pUartConfigs = &sUartConfig,
        .uartHwIndex = 0,

    },
#endif
#if defined(AM_BSP_GPIO_UART1_TX) && defined(AM_BSP_GPIO_UART1_RX)
    [eUART1] = {
        .uartId = eUART1,
        .rxPin = {AM_BSP_GPIO_UART1_RX, &g_AM_BSP_GPIO_UART1_RX},
        .txPin = {AM_BSP_GPIO_UART1_TX, &g_AM_BSP_GPIO_UART1_TX},
        .pUartConfigs = &sUartConfig,
        .uartHwIndex = 1,
    },
#endif
#if defined(AM_BSP_GPIO_UART2_TX) && defined(AM_BSP_GPIO_UART2_RX)
    [eUART2] = {
        .uartId = eUART2,
        .rxPin = {AM_BSP_GPIO_UART2_RX, &g_AM_BSP_GPIO_UART2_RX},
        .txPin = {AM_BSP_GPIO_UART2_TX, &g_AM_BSP_GPIO_UART2_TX},
        .pUartConfigs = &sUartConfig,
        .uartHwIndex = 2,
    },
#endif
#if defined(AM_BSP_GPIO_UART3_TX) && defined(AM_BSP_GPIO_UART3_RX)
    [eUART3] = {
        .uartId = eUART3,
        .rxPin = {AM_BSP_GPIO_UART3_RX, &g_AM_BSP_GPIO_UART3_RX},
        .txPin = {AM_BSP_GPIO_UART3_TX, &g_AM_BSP_GPIO_UART3_TX},
        .pUartConfigs = &sUartConfig,
        .uartHwIndex = 3,
    },
#endif
};

//***************************** local prototypes *******************************

static uint32_t init_timer(void);

static void serial_interface_init(void);

void am_timer00_isr(void);

void am_uart_isr(void);

void am_uart1_isr(void);

void am_uart2_isr(void);

void am_uart3_isr(void);

void am_isr_common(uart_async_local_vars_t *pUartDescriptor);

//***************************** code below *************************************

//*****************************************************************************
//
//! @brief Interrupt handler for the CTIMER
//! This provides a clock to gate the uart tx activities
//
//*****************************************************************************
void
am_timer00_isr(void)
{
    //
    // Clear Timer Interrupt.
    //
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));
    am_hal_timer_clear(TIMER_NUM);

    g_localv.bTimerIntOccured = true;
}

//*****************************************************************************
//
//! @brief Uart isr handlers
//
//*****************************************************************************
void am_uart_isr(void)
{
#if defined(AM_BSP_GPIO_UART0_TX) && defined(AM_BSP_GPIO_UART0_RX)
    am_isr_common(g_localv.uartIsrMap[eUART0]);
#endif
}

void am_uart1_isr(void)
{
#if defined(AM_BSP_GPIO_UART1_TX) && defined(AM_BSP_GPIO_UART1_RX)
    am_isr_common(g_localv.uartIsrMap[eUART1]);
#endif
}

void am_uart2_isr(void)
{
#if defined(AM_BSP_GPIO_UART2_TX) && defined(AM_BSP_GPIO_UART2_RX)
    am_isr_common(g_localv.uartIsrMap[eUART2]);
#endif
}

void am_uart3_isr(void)
{
#if defined(AM_BSP_GPIO_UART3_TX) && defined(AM_BSP_GPIO_UART3_RX)
    am_isr_common(g_localv.uartIsrMap[eUART3]);
#endif
}

//*****************************************************************************
//
//! @brief Common uart isr handler
//!
//! @param pUartDescriptor
//
//*****************************************************************************
void am_isr_common(uart_async_local_vars_t *pUartDescriptor)
{
    pUartDescriptor->e32Status |= am_hal_uart_interrupt_queue_service(pUartDescriptor->pvUART);
    if (pUartDescriptor->e32Status != AM_HAL_UART_STATUS2_SUCCESS)
    {
        pUartDescriptor->ui32ISrErrorCount++;
    }
}

//*****************************************************************************
//
//! @brief Start UART test timer
//!
//! @return standard hal status
//
//*****************************************************************************
static uint32_t
init_timer(void)
{
    am_hal_timer_config_t TimerConfig;

    //
    // Set up the desired TIMER.
    // The default config parameters include:
    //  eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16
    //  eFunction = AM_HAL_TIMER_FN_EDGE
    //  Compare0 and Compare1 maxed at 0xFFFFFFFF
    //
    am_hal_timer_default_config_set(&TimerConfig);
    TimerConfig.eInputClock = AM_HAL_TIMER_CLOCK_LFRC;     // ~900Hz
    //
    // Modify the default parameters.
    // Configure the timer to a 1s period via ui32Compare1.
    //
    TimerConfig.eFunction = AM_HAL_TIMER_FN_UPCOUNT;
    TimerConfig.ui32Compare1 = 900 * 4;

    //
    // Configure the timer
    //
    uint32_t status = am_hal_timer_config(TIMER_NUM, &TimerConfig);
    if (status != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("Failed to configure TIMER%d.\n", TIMER_NUM);
        return status;
    }

    //
    // Clear the timer and its interrupt
    //
    am_hal_timer_clear(TIMER_NUM);
    am_hal_timer_interrupt_clear(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));
    //
    // Clear the timer Interrupt
    //
    NVIC_SetPriority(TIMER0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(TIMER0_IRQn);

    //
    // Enable the timer Interrupt.
    //
    am_hal_timer_interrupt_enable(AM_HAL_TIMER_MASK(TIMER_NUM, AM_HAL_TIMER_COMPARE1));

    return AM_HAL_STATUS_SUCCESS;
} // init_timer()

static uint32_t init_uart(uart_id_e uartId, uart_async_local_vars_t *pUartLocalVar);

//*****************************************************************************
//
//! @brief Initialize a UART instance
//!
//! @param uartId  the uart number (0<->(N-1))
//! @param pUartLocalVar ram allocated for this uart
//!
//! @return standard hal status
//
//*****************************************************************************

static uint32_t init_uart(uart_id_e uartId, uart_async_local_vars_t *pUartLocalVar)
{
    //
    //! get and check the uart number
    //

    if (uartId >= eMAX_UARTS)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    pUartLocalVar->uartId = uartId;
    //
    // associate this instance with this uart ID
    // (so don't loop on IDs, loop through the instance structures)
    //
    g_localv.uartIsrMap[uartId] = pUartLocalVar;

    //
    //! setup rx and tx pins
    //
    const uart_defs_t *pUartDefs = &gs_UartPinDefs[uartId];
    am_hal_gpio_pinconfig(pUartDefs->txPin.pinNUmber, *(pUartDefs->txPin.pinCfg));
    am_hal_gpio_pinconfig(pUartDefs->rxPin.pinNUmber, *(pUartDefs->rxPin.pinCfg));

    //
    //! set up ram buffers and uart module
    //
    pUartLocalVar->ui32RxBuffSize = sizeof(pUartLocalVar->pui8UARTRXBuffer);
    pUartLocalVar->ui32TxBuffSize = sizeof(pUartLocalVar->pui8UARTTXBuffer);

    am_hal_uart_initialize(UART_ID, &pUartLocalVar->pvUART);
    am_hal_uart_power_control(pUartLocalVar->pvUART, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(pUartLocalVar->pvUART, pUartDefs->pUartConfigs);

    am_hal_uart_buffer_configure(pUartLocalVar->pvUART,
                                 pUartLocalVar->pui8UARTTXBuffer, pUartLocalVar->ui32TxBuffSize,
                                 pUartLocalVar->pui8UARTRXBuffer, pUartLocalVar->ui32RxBuffSize);

    //
    // Make sure to enable the interrupts for RX, since the HAL doesn't already
    // know we intend to use them., this is done later
    //
    uint32_t uartInstance = pUartDefs->uartHwIndex;
    NVIC_SetPriority((IRQn_Type) (UART0_IRQn + uartInstance), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type) (UART0_IRQn + uartInstance));

    return AM_HAL_STATUS_SUCCESS;
}
//*****************************************************************************
//
//! @brief Initialize all the serial modules and associated tx and tx pins
//
//*****************************************************************************

static void serial_interface_init(void)
{
    //
    // Start the UARTs that are enabled here
    //
    uart_async_local_vars_t *pUartDesc = g_localv.tUartVars;
    for (uint32_t i = 0; i < NUM_UARTS_USED; i++)
    {
        init_uart(uartsUsed[i], pUartDesc);
        pUartDesc++;

    }
} //serial_interface_init

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM output
    //
    if (am_bsp_debug_printf_enable())
    {
        // Cannot print - so no point proceeding
        while (1);
    }

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Uart Async Example!\n\n");

    serial_interface_init();

    //
    // Enable interrupt service routines.
    //
    am_hal_interrupt_master_enable();

    uint8_t rxBuf[16];

    uint32_t tx_stat = 0;
    uint32_t rx_stat = 0;

    // do a non-blocking write, set interrupts here
    //tx_stat = serial_data_write(txBuf, txLen);
    // do a non block read, set rx interrupt here
    //rx_stat = serial_data_read(rxBuf, &txLen );

    init_timer();

    am_util_stdio_printf("serial test started, tx_start_stat: %d, rx_start_stat %d\n", tx_stat, rx_stat);

    // enable tx and tx interrupts
    for (uint32_t i = 0; i < NUM_UARTS_USED; i++)
    {
        am_hal_uart_interrupt_enable(g_localv.tUartVars[i].pvUART,
                (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_TX | AM_HAL_UART_INT_RX_TMOUT));
    }

    uint32_t txCount = 0;

    while (1)
    {
        // loop through all the uart descriptors
        uart_async_local_vars_t *pUartDesc = g_localv.tUartVars;
        for (uint32_t i = 0; i < NUM_UARTS_USED; i++)
        {
            // check through the uart status
            // read data
            // look at input status
            // could mask interrupts or disable ISR interrupts
            uart_id_e eUartId = pUartDesc->uartId;

            am_hal_uart_status2_t isrStatus;
            AM_CRITICAL_BEGIN
                //
                // clear off the accumulated status for this module
                //
                isrStatus = pUartDesc->e32Status;
                pUartDesc->e32Status &= ~(AM_HAL_UART_STATUS2_INTRNL_MSK | AM_HAL_UART_STATUS2_RX_DATA_AVAIL |
                                          AM_HAL_UART_STATUS2_TX_COMPLETE);
            AM_CRITICAL_END

            //
            // print isr activity/status messages
            //
            if (isrStatus & AM_HAL_UART_STATUS2_TX_COMPLETE)
            {
                am_util_stdio_printf("txComplete on uart %d\r\n", eUartId);
            }
            if (isrStatus & AM_HAL_UART_STATUS2_INTRNL_MSK)
            {
                am_util_stdio_printf("uart error on uart %d : 0x%x\r\n", eUartId,
                                     isrStatus & AM_HAL_UART_STATUS2_INTRNL_MSK);

            }
            if (isrStatus & AM_HAL_UART_STATUS2_RX_DATA_AVAIL)
            {
                //
                // read uart after interrupt
                // here the uart will accumulate data until the fifo is full or a rx timeout occurs
                // Note: the rx-timeout interrupt must be enabled for the rx timeout to occur
                //
                int32_t i32numRx = am_hal_uart_get_rx_data(pUartDesc->pvUART, rxBuf, sizeof(rxBuf));

                if (i32numRx < 0)
                {
                    am_util_stdio_printf("rxError on uart %d : %d\r\n", eUartId, -i32numRx);
                }
                else if (i32numRx > 0)
                {
                    am_util_stdio_printf("num bytes rxed on uart %d : %d\r\n", eUartId, i32numRx);
                }
            }
#ifdef POLL_UART
            else
            {
                // this iwll poll the uart and will dig data out of the fifo, depending on polling and data reate,
                // possibly before it causes an rx interrupt. This is typically used if the RX-TIMEOUT interrups is
                // NOT enabled.

                int32_t i32numRx = am_hal_uart_get_rx_data(pUartDesc->pvUART, rxBuf, sizeof(rxBuf));

                if (i32numRx < 0)
                {
                    am_util_stdio_printf("noRxIsr flag: rxError2 on uart %d : %d\r\n", eUartId, -i32numRx);
                }
                else if (i32numRx > 0)
                {
                    am_util_stdio_printf("noRxIsr flag: num bytes rxed on uart %d : %d %c\r\n", eUartId, i32numRx, rxBuf[0] );
                }

            }
#endif
            //
            // process the next uart
            //
            pUartDesc++;
        }

        if (g_localv.bTimerIntOccured)
        {
            //
            // timer interrupt, this is used to send data periodically
            //
            g_localv.bTimerIntOccured = false;

            txCount++;
            uint8_t *p;
            uint32_t txSize;
            if (txCount & 0x01)
            {
                p = txBuf;
                txSize = 73;
            }
            else
            {
                p = txBuf2;
                txSize = 13;

            }

            uint32_t ax = (txCount % 26) + 'a';
            pUartDesc = g_localv.tUartVars;
            for (uint32_t i = 0; i < NUM_UARTS_USED; i++)
            {
                p[0] = ax;
                uint32_t ui32TxStat = am_hal_uart_append_tx(pUartDesc->pvUART, (uint8_t *) p, txSize);
                ui32TxStat = am_hal_uart_append_tx(pUartDesc->pvUART, (uint8_t *) p, txSize);
                if (ui32TxStat)
                {
                    am_util_stdio_printf("serial tx, tx_start_stat: %d\n", ui32TxStat);
                }

                pUartDesc++;
            }

        } // g_localv.bTimerIntOccured
    } // while(1)
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

