/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2014 - 2023 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* - Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File      : Cortex_M_Startup.s
Purpose   : Generic startup and exception handlers for Cortex-M devices.

Additional information:
  Preprocessor Definitions
    __NO_SYSTEM_INIT
      If defined, 
        SystemInit is not called.
      If not defined,
        SystemInit is called.
        SystemInit is usually supplied by the CMSIS files.
        This file declares a weak implementation as fallback.
        
    __SUPPORT_RESET_HALT_AFTER_BTL
      If != 0
        Support J-Link's reset strategy Reset and Halt After Bootloader.
        https://wiki.segger.com/Reset_and_Halt_After_Bootloader
      If == 0 (default),
        Disable support for Reset and Halt After Bootloader.

    __SOFTFP__
      Defined by the build system.
      If not defined, the FPU is enabled for floating point operations.
*/

        .syntax unified  

        
#ifndef   __SUPPORT_RESET_HALT_AFTER_BTL
  #define __SUPPORT_RESET_HALT_AFTER_BTL  0
#endif
        
/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
//
// Just place a vector (word) in the table
//
.macro VECTOR Name=
        .section .vectors, "a"
        .word \Name
.endm
//
// Declare an interrupt handler
//
.macro ISR_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "a"
        .word \Name
        //
        // Insert dummy handler in init section
        //
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
        END_FUNC \Name
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED
        .section .vectors, "a"
        .word 0
.endm

//
// Mark the end of a function and calculate its size
//
.macro END_FUNC name
        .size \name,.-\name
.endm

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
/*********************************************************************
*
*  Setup of the vector table and weak definition of interrupt handlers
*
*/
        .section .vectors, "a"
        .code 16
        .balign 4
        .global _vectors
_vectors:
        VECTOR __stack_end__
        VECTOR Reset_Handler
        ISR_HANDLER NMI_Handler
        VECTOR HardFault_Handler
        ISR_HANDLER MemManage_Handler 
        ISR_HANDLER BusFault_Handler
        ISR_HANDLER UsageFault_Handler
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_HANDLER SVC_Handler
        ISR_HANDLER DebugMon_Handler
        ISR_RESERVED
        ISR_HANDLER PendSV_Handler
        ISR_HANDLER SysTick_Handler
        //
        // Add external interrupt vectors here.
        // Example:
        //   ISR_HANDLER ExternalISR0
        //   ISR_HANDLER ExternalISR1
        //   ISR_HANDLER ExternalISR2
        //   ISR_HANDLER ExternalISR3
        //
        ISR_HANDLER     am_brownout_isr             //  0: Brownout (rstgen)
        ISR_HANDLER     am_watchdog_isr             // 1: Watchdog (WDT)
        ISR_HANDLER     am_rtc_isr                  // 2: RTC
        ISR_HANDLER     am_vcomp_isr                // 3: Voltage Comparator
        ISR_HANDLER     am_ioslave_ios_isr          // 4: I/O Slave general
        ISR_HANDLER     am_ioslave_acc_isr          // 5: I/O Slave access
        ISR_HANDLER     am_iomaster0_isr            // 6: I/O Master 0
        ISR_HANDLER     am_iomaster1_isr            // 7: I/O Master 1
        ISR_HANDLER     am_iomaster2_isr            // 8: I/O Master 2
        ISR_HANDLER     am_iomaster3_isr            // 9: I/O Master 3
        ISR_HANDLER     am_iomaster4_isr            //10: I/O Master 4
        ISR_HANDLER     am_iomaster5_isr            //11: I/O Master 5
        ISR_HANDLER     am_iomaster6_isr            //12: I/O Master 6 (I3C/I2C/SPI)
        ISR_HANDLER     am_iomaster7_isr            //13: I/O Master 7 (I3C/I2C/SPI)
        ISR_HANDLER     am_ctimer_isr               //14: OR of all timerX interrupts
        ISR_HANDLER     am_uart_isr                 //15: UART0
        ISR_HANDLER     am_uart1_isr                //16: UART1
        ISR_HANDLER     am_uart2_isr                //17: UART2
        ISR_HANDLER     am_uart3_isr                //18: UART3
        ISR_HANDLER     am_adc_isr                  //19: ADC
        ISR_HANDLER     am_mspi0_isr                //20: MSPI0
        ISR_HANDLER     am_mspi1_isr                //21: MSPI1
        ISR_HANDLER     am_mspi2_isr                //22: MSPI2
        ISR_HANDLER     am_clkgen_isr               //23: ClkGen
        ISR_HANDLER     am_cryptosec_isr            //24: Crypto Secure
        ISR_HANDLER     am_default_isr              //25: Reserved
        ISR_HANDLER     am_sdio_isr                 //26: SDIO
        ISR_HANDLER     am_usb_isr                  //27: USB
        ISR_HANDLER     am_gpu_isr                  //28: GPU
        ISR_HANDLER     am_disp_isr                 //29: DISP
        ISR_HANDLER     am_dsi_isr                  //30: DSI
        ISR_RESERVED
        ISR_HANDLER     am_stimer_cmpr0_isr         //32: System Timer Compare0
        ISR_HANDLER     am_stimer_cmpr1_isr         //33: System Timer Compare1
        ISR_HANDLER     am_stimer_cmpr2_isr         //34: System Timer Compare2
        ISR_HANDLER     am_stimer_cmpr3_isr         //35: System Timer Compare3
        ISR_HANDLER     am_stimer_cmpr4_isr         //36: System Timer Compare4
        ISR_HANDLER     am_stimer_cmpr5_isr         //37: System Timer Compare5
        ISR_HANDLER     am_stimer_cmpr6_isr         //38: System Timer Compare6
        ISR_HANDLER     am_stimer_cmpr7_isr         //39: System Timer Compare7
        ISR_HANDLER     am_stimerof_isr             //40: System Timer Cap Overflow
        ISR_RESERVED
        ISR_HANDLER     am_audadc0_isr              //42: Audio ADC
        ISR_RESERVED
        ISR_HANDLER     am_dspi2s0_isr              //44: I2S0
        ISR_HANDLER     am_dspi2s1_isr              //45: I2S1
        ISR_HANDLER     am_dspi2s2_isr              //46: I2S2
        ISR_HANDLER     am_dspi2s3_isr              //47: I2S3
        ISR_HANDLER     am_pdm0_isr                 //48: PDM0
        ISR_HANDLER     am_pdm1_isr                 //49: PDM1
        ISR_HANDLER     am_pdm2_isr                 //50: PDM2
        ISR_HANDLER     am_pdm3_isr                 //51: PDM3
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_HANDLER     am_gpio0_001f_isr           //56: GPIO N0 pins  0-31
        ISR_HANDLER     am_gpio0_203f_isr           //57: GPIO N0 pins 32-63
        ISR_HANDLER     am_gpio0_405f_isr           //58: GPIO N0 pins 64-95
        ISR_HANDLER     am_gpio0_607f_isr           //59: GPIO N0 pins 96-104, virtual 105-127
        ISR_HANDLER     am_gpio1_001f_isr           //60: GPIO N1 pins  0-31
        ISR_HANDLER     am_gpio1_203f_isr           //61: GPIO N1 pins 32-63
        ISR_HANDLER     am_gpio1_405f_isr           //62: GPIO N1 pins 64-95
        ISR_HANDLER     am_gpio1_607f_isr           //63: GPIO N1 pins 96-104, virtual 105-127
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_HANDLER     am_timer00_isr              //67: timer0
        ISR_HANDLER     am_timer01_isr              //68: timer1
        ISR_HANDLER     am_timer02_isr              //69: timer2
        ISR_HANDLER     am_timer03_isr              //70: timer3
        ISR_HANDLER     am_timer04_isr              //71: timer4
        ISR_HANDLER     am_timer05_isr              //72: timer5
        ISR_HANDLER     am_timer06_isr              //73: timer6
        ISR_HANDLER     am_timer07_isr              //74: timer7
        ISR_HANDLER     am_timer08_isr              //75: timer8
        ISR_HANDLER     am_timer09_isr              //76: timer9
        ISR_HANDLER     am_timer10_isr              //77: timer10
        ISR_HANDLER     am_timer11_isr              //78: timer11
        ISR_HANDLER     am_timer12_isr              //79: timer12
        ISR_HANDLER     am_timer13_isr              //80: timer13
        ISR_HANDLER     am_timer14_isr              //81: timer14
        ISR_HANDLER     am_timer15_isr              //82: timer15
        ISR_HANDLER     am_cachecpu_isr             //83: CPU cache


        .section .vectors, "a"
        .size _vectors, .-_vectors
_vectors_end:

/*********************************************************************
*
* The Patch table.
*
* The patch table should pad the vector table size to a total of 128 entrie
* such that the code begins at 0x200.
* In other words, the final peripheral IRQ is always IRQ 111 (0-based).
*
**********************************************************************/
        .section .patch, "a"
        .code 16
        .balign 4
        .global __Patchable
__Patchable:
        .word                 0, 0, 0, 0, 0, 0    //  84-89
        .word     0, 0, 0, 0, 0, 0, 0, 0, 0, 0    //  90-99
        .word     0, 0, 0, 0, 0, 0, 0, 0, 0, 0    // 100-109
        .word     0, 0                            // 110-111
        .section .patch, "a"
        .size __Patchable, .-__Patchable
__Patchable_end:

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       Reset_Handler
*
*  Function description
*    Exception handler for reset.
*    Generic bringup of a Cortex-M system.
*
*  Additional information
*    The stack pointer is expected to be initialized by hardware,
*    i.e. read from vectortable[0].
*    For manual initialization add
*      ldr R0, =__stack_end__
*      mov SP, R0
*/
        .global reset_handler
        .global Reset_Handler
        .equ reset_handler, Reset_Handler
        .section .init.Reset_Handler, "ax"
        .balign 2
        .thumb_func
Reset_Handler:
#if __SUPPORT_RESET_HALT_AFTER_BTL != 0
        //
        // Perform a dummy read access from address 0x00000008 followed by two nop's
        // This is needed to support J-Links reset strategy: Reset and Halt After Bootloader.
        // https://wiki.segger.com/Reset_and_Halt_After_Bootloader
        //
        movs    R0, #8
        ldr     R0, [R0]
        nop
        nop
#endif
#ifdef __SEGGER_STOP
        .extern __SEGGER_STOP_Limit_MSP
        //
        // Initialize main stack limit to 0 to disable stack checks before runtime init
        //
        movs    R0, #0
        ldr     R1, =__SEGGER_STOP_Limit_MSP
        str     R0, [R1]
#endif
#ifndef __NO_SYSTEM_INIT
        //
        // Set the VTOR to what it should have been (0x18000)
        // To Do - FIXME - This is a workaround to a problem with SES overwriting the SBL VTOR.
        //
        movw   R0, 0xED08
        movt   R0, 0xE000
        ldr    R1, =_vectors
        str    R1, [R0]
        //
        // Call SystemInit
        //
        bl      SystemInit
#endif
#if !defined(__SOFTFP__)
        //
        // Enable CP11 and CP10 with CPACR |= (0xf<<20)
        //
        movw    R0, 0xED88
        movt    R0, 0xE000
        ldr     R1, [R0]
        orrs    R1, R1, #(0xf << 20)
        str     R1, [R0]
#endif
        //
        // Call runtime initialization, which calls main().
        //
        bl      _start
END_FUNC Reset_Handler

        //
        // Weak only declaration of SystemInit enables Linker to replace bl SystemInit with a NOP,
        // when there is no strong definition of SystemInit.
        //
        .weak SystemInit

/*********************************************************************
*
*       HardFault_Handler
*
*  Function description
*    Simple exception handler for HardFault.
*    In case of a HardFault caused by BKPT instruction without 
*    debugger attached, return execution, otherwise stay in loop.
*
*  Additional information
*    The stack pointer is expected to be initialized by hardware,
*    i.e. read from vectortable[0].
*    For manual initialization add
*      ldr R0, =__stack_end__
*      mov SP, R0
*/

#undef L
#define L(label) .LHardFault_Handler_##label

        .weak HardFault_Handler
        .section .init.HardFault_Handler, "ax"
        .balign 2
        .thumb_func
HardFault_Handler:
        //
        // Check if HardFault is caused by BKPT instruction
        //
        ldr     R1, =0xE000ED2C         // Load NVIC_HFSR
        ldr     R2, [R1]
        cmp     R2, #0                  // Check NVIC_HFSR[31]

L(hfLoop):
        bmi     L(hfLoop)               // Not set? Stay in HardFault Handler.
        //
        // Continue execution after BKPT instruction
        //
#if defined(__thumb__) && !defined(__thumb2__)
        movs    R0, #4
        mov     R1, LR
        tst     R0, R1                  // Check EXC_RETURN in Link register bit 2.
        bne     L(Uses_PSP)
        mrs     R0, MSP                 // Stacking was using MSP.
        b       L(Pass_StackPtr)
L(Uses_PSP):
        mrs     R0, PSP                 // Stacking was using PSP.
L(Pass_StackPtr):
#else
        tst     LR, #4                  // Check EXC_RETURN[2] in link register to get the return stack
        ite     eq
        mrseq   R0, MSP                 // Frame stored on MSP
        mrsne   R0, PSP                 // Frame stored on PSP
#endif
        //
        // Reset HardFault Status
        //
#if defined(__thumb__) && !defined(__thumb2__)
        movs    R3, #1
        lsls    R3, R3, #31
        orrs    R2, R3
        str     R2, [R1]
#else
        orr R2, R2, #0x80000000
        str R2, [R1]
#endif
        //
        // Adjust return address
        //
        ldr     R1, [R0, #24]           // Get stored PC from stack
        adds    R1, #2                  // Adjust PC by 2 to skip current BKPT
        str     R1, [R0, #24]           // Write back adjusted PC to stack
        //
        bx      LR                      // Return
END_FUNC HardFault_Handler

/*************************** End of file ****************************/

