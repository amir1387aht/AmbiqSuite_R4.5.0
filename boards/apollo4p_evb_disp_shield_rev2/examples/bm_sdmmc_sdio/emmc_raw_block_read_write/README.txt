Name:
=====
 emmc_raw_block_read_write


Description:
============
 emmc raw block read and write example.



Purpose:
========
This example demonstrates how to blocking PIO and DMA read & write
          APIs with eMMC device.
 
   1) Initialize for low power
   2) Initialize Card Host
   3) Config Card Host
   4) Test SSRAM
   5) Test SSRAM
   6) Test TCM
   7) Loop through 4, 5, 6 forever
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1M Baud.
 
 If ITM is not shut down, the device will never achieve deep sleep, only
 normal sleep, due to the ITM (and thus the HFRC) being enabled.
 
 The following macros can be used in this example
 
 Defined by default:
   #define TEST_POWER_SAVING
           Enables to power down and back up of the SDIO
   #define TEST_SSRAM
           Enables SSRAM testing
   #define TEST_TCM
           Enables TCM testing
 
 Not Defined by default:
   #define EMMC_DDR50_TEST
           Enables to config and test the device in DDR50 mode


******************************************************************************


