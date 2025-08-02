Name:
=====
 pdm_fft


Description:
============
 An example to show basic PDM operation.




Purpose:
========
This example enables the PDM interface to record audio signals from an
 external microphone.
 
 The required pin connections are:
   - PDM0 -
     GPIO 50 - PDM0 CLK
     GPIO 51 - PDM0 DATA
 
   - PDM1 -
     GPIO 52 - PDM1 CLK
     GPIO 53 - PDM1 DATA
 
   - PDM2 -
     GPIO 54 - PDM2 CLK
     GPIO 55 - PDM2 DATA
 
   - PDM3 -
     GPIO 56 - PDM3 CLK
     GPIO 57 - PDM3 DATA

 NOTE: On Apollo4l, there is only 1 PDM instance (PDM0).

Additional Information:
=======================
 Printing takes place over the ITM at 1M Baud.
 RTT logger takes place over the SWD at 4M Baud.


******************************************************************************


