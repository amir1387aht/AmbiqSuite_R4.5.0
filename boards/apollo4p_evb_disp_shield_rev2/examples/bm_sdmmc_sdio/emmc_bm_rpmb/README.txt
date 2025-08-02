Name:
=====
 emmc_bm_rpmb


Description:
============
 emmc rpmb example.




Purpose:
========
This example demonstrates how to use APIs in eMMC RPMB driver
          to access RPMB partition.
 
   1) Initialize for low power
   2) Initialize Card Host
   3) Config Card Host
   4) Init RPMB Device Driver
   5) Switch Partitions to RPMB Memory block
   6) Perform Read/Write Test
   7) DeInit RPMB Device Driver
   8) Sleep/Wakeup
   9) Perform Read/Write Test
   10) Sleep Forever
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1M Baud.
 
 If ITM is not shut down, the device will never achieve deep sleep, only
 normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


