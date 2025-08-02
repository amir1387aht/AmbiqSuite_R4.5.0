Name:
=====
 audadc_power_cycling


Description:
============
 This example tests AUADC power cycling sequence.




Purpose:
========
This example sets up tthe AUDADC to sample audio and printf
  that data to ITM Interface.
 
 The device will:
   1) Configure the board for low power.
   2) Configure the AUDADC DMA data buffers with 16B alignment.
   3) Power up the PrePGA
   4) Turn on mic bias
   5) Configure the AUDADC
   6) Configure the Gain settings
   7) Configure the AUDADC slot
   8) Spin for 0 - 500ms
   9) Power down the AUDADC
   10) Repeat steps 2 - 9

Additional Information:
=======================
 Printing takes place over the ITM at 1M Baud.


******************************************************************************


