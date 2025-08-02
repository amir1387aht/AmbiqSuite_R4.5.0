Name:
=====
 sd_card_bm_fatfs


Description:
============
 SD Card bare-metal FatFs example.




Purpose:
========
This example demonstrates how to use file system with SD Card device
 based on the SD Card bare-metal HAL.
 
   1) Initialize for low power
   2) Mount Filesystem
   3) Create Filesystem using FAT32
   4) Open Filesystem
   5) Perform Read/Write Test
   6) Close Filesystem
   7) Unmount Filessytem
   8) Sleep Forever
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1M Baud.
 
 If ITM is not shut down, the device will never achieve deep sleep, only
 normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


