Name:
=====
 mspi_octal_example_fifo_full


Description:
============
 Example of the MSPI operation with Octal SPI Flash.



Purpose:
========
This example demonstrates how to detect the MSPI FIFO Full
 condition in an application

 Initialize the test:
 1. Initialize the MSPI instance
 2. Erase Sector

 Test SDR DMA R/W:
 1. Write known data to a buffer using DMA
 2. Deinit and Reinit the MSPI Instance
 3. Read the data back into another buffer using DMA
 4. Check for AM_HAL_MSPI_FIFO_FULL_CONDITION
 5. Compare the results from 1 and 3 immediately above
 6. Check Again for FIFO Full Condition

 Deinitialize the MSPI and go to sleep

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


