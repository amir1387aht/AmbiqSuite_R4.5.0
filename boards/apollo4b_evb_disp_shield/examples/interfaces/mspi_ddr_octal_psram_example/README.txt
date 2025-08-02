Name:
=====
 mspi_ddr_octal_psram_example


Description:
============
 Example of the MSPI operation with DDR OCTAL SPI PSRAM.



Purpose:
========
This example demonstrates MSPI DDR OCTAL operation using the PSRAM
 device.

 Initialize the test:
 1. Run a timing check to find the best timing for the MSPI clock and chipset
 2. Initialize the MSPI instance
 3. Apply the timing scan results

 Test DDR DMA R/W:
 1. Write known data to a buffer using DMA
 2. Read the data back into another buffer using DMA
 3. Compare the results from 1 and 2 immediately above

 Test DDR XIP R/W:
 1. Enable XIP
 2. Write known data to a buffer using XIP
 3. Read the data back into another buffer using XIP
 4. Compare the results from 2 and 3 immediately above

 Deinitialize the MSPI and go to sleep

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.

 The following macro will enable/disable global use to XIP in this example.
 It is defined by default
 - #define ENABLE_XIPMM


******************************************************************************


