Name:
=====
 emmc_scatter_raw_read_write


Description:
============
 emmc scatter raw read and write example.



Purpose:
========
This example demonstrates demonstrates how to use ADMA based scatter IO APIs
          to read/write eMMC device..
 
   1) Initialize for low power
   2) Initialize Card Host
   3) Config Card Host
   4) Test SSRAM
   5) Test Different Memory type
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1M Baud.
 
 If ITM is not shut down, the device will never achieve deep sleep, only
 normal sleep, due to the ITM (and thus the HFRC) being enabled.
 
 The following macros can be used in this example
 
 Defined by default:
   #define TEST_SSRAM
           Test scatter io operation in ssram
   #define TEST_SCATTER_ADDRESS_SYNC_WRITE_READ
           Test scatter synchronous write and read in different memory type
   #define TEST_SCATTER_ADDRESS_ASYNC_WRITE_READ
           Test scatter asynchronous write and read in different memory type
 


******************************************************************************


