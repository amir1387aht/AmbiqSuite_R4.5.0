Name:
=====
 adc_lpmode0_dma


Description:
============
 This example takes samples with the ADC at high-speed using DMA.



Purpose:
========
This example shows the ADC internal timer setup on the HFRC,
 triggering repeated samples of an external input at 1.2Msps in LPMODE0.

 The example uses the ADC internal timer clocked by HFRC to trigger
 ADC sampling.

 Each data point is 128 sample average and is transferred
 from the ADC FIFO into an SRAM buffer using DMA.

Additional Information:
=======================
 Printing takes place over the SWO/ITM at 1M Baud.



******************************************************************************


