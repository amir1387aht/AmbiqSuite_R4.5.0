Name:
=====
 mpu_mspi_ddr_octal_psram_example


Description:
============
 Example of using MPU protection as workaround for DSP RAM issue.




Purpose:
========
This example demonstrates how to use MPU as workaround for DSP RAM
 issue.

 You need to configure the DSP RAM regions in MPU and to enable this, you must
 configure the MPU globally as well.

*** CODE SECTION ***
     am_hal_mpu_region_configure(&sDSPRAM_A, true);
     am_hal_mpu_region_configure(&sDSPRAM_B, true);

     am_hal_mpu_global_configure(true, true, false);
*** END CODE SECTION ***

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.



******************************************************************************


