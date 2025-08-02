Name:
=====
 mspi_ds35x1ga_quad_example


Description:
============
 Example of the MSPI operation with SDR Quad SPI NAND Flash.



Purpose:
========
This example configures an MSPI connected NAND flash device in Quad
 mode and performs various operations

 Initialize the example:
 1. Run a timing check to find the best timing for the MSPI clock and chipset
 2. Initialize the MSPI instance
 3. Apply the timing scan results

 Test SDR DMA R/W:
 1. Write known data to a buffer using DMA
 2. Read the data back into another buffer using DMA
 3. Compare the results from 1 and 2 immediately above

 Deinitialize the MSPI and go to sleep

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.

 The example can be configured and tested in either Serial or Quad using the
 following macro:
 - #define NAND_TEST_FLASH_CFG_MODE 1 // 0:serial - 1:quad


******************************************************************************


