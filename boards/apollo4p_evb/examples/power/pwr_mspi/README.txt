Name:
=====
 pwr_mspi


Description:
============
 Example for MSPI DMA power measurements.




Purpose:
========
Example for MSPI DMA power measurements.

 @details Simple example that uses HAL to queue multiple MSPI DMA transfers.

 The instance 1 and 2 may not support all of the Quad, Octal and Hex mode,
 check the corresponding manuals for details

   1. Enter deepsleep during DMA transfer.
   2. DMA write-loop or read-loop every 1s.
   3. Have configurable MSPI buffer size.
   4. GPIO indicator for CPU wake state.

 Debug:
     GPIO[0]  GPIO_DBG1_RW Set LOW for start read, set HIGH for start write
     GPIO[3]  DEBUG_PIN_2 will keep HIGH during all setting counts transfer
     GPIO[2]  DEBUG_PIN_SLEEP will remain LOW during deep sleep mode


******************************************************************************


