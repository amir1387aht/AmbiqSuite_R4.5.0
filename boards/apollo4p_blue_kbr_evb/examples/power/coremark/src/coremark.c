//*****************************************************************************
//
//! @file coremark.c
//!
//! @brief EEMBC COREMARK test.
//!
//! @addtogroup power_examples Power Examples
//!
//! @defgroup ble_freertos_fit_lp BLE FreeRTOS Fit Lowpower Example
//! @ingroup power_examples
//! @{
//!
//! Purpose: This example runs the official EEMBC COREMARK test.
//!
//! EEMBC’s CoreMark® is a benchmark that measures the performance of
//! microcontrollers (MCUs) and central processing units (CPUs) used in
//! embedded systems. Replacing the antiquated Dhrystone benchmark, Coremark
//! contains implementations of the following algorithms: list processing
//! (find and sort), matrix manipulation (common matrix operations), state
//! machine (determine if an input stream contains valid numbers), and CRC
//! (cyclic redundancy check). It is designed to run on devices from 8-bit
//! microcontrollers to 64-bit microprocessors.
//!
//! The CRC algorithm serves a dual function; it provides a workload commonly
//! seen in embedded applications and ensures correct operation of the CoreMark
//! benchmark, essentially providing a self-checking mechanism. Specifically,
//! to verify correct operation, a 16-bit CRC is performed on the data
//! contained in elements of the linked-list.
//!
//! To ensure compilers cannot pre-compute the results at compile time, every
//! operation in the benchmark derives a value that is not available at compile
//! time. Furthermore, all code used within the timed portion of the benchmark
//! is part of the benchmark itself (no library calls).
//!
//! More info may be found at the [EEMBC CoreMark website](https://www.eembc.org/coremark/).
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
// End Doxygen group.
//! @}
//
//*****************************************************************************

