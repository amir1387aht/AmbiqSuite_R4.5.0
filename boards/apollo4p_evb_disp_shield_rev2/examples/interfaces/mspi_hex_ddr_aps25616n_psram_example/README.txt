Name:
=====
 mspi_hex_ddr_aps25616n_psram_example


Description:
============
 Example of the MSPI operation with DDR HEX SPI PSRAM APS25616N.



Purpose:
========
This example demonstrates MSPI DDR HEX operation using the APS25616N
 PSRAM device.

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

 Test Function Call after XIP writes function to PSRAM:
 1. Write test function to External PSRAM
 2. Call function located in PSRAM

 Deinitialize the MSPI and go to sleep

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


