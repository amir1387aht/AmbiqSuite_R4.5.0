Name:
=====
 pdm_to_i2s


Description:
============
 An example to show PDM to I2S(slave) operation.




Purpose:
========
This example enables the PDM and I2S interface to collect audio signals from
          an external PDM microphone, I2S module using pingpong buffer to interact with PDM,
          and start transaction when mclk is supplied(from external I2S master).
          Notice: external mclk should be supplied first at this example.
 
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
 
 NOTE: Apollo4L only have 1 I2S instance, so we only do the loopback between I2S0 TX and RX.
 

Additional Information:
=======================
 Printing takes place over the ITM at 1M Baud.


******************************************************************************


