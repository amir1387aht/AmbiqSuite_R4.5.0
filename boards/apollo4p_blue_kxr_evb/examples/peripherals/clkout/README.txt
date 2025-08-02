Name:
=====
 clkout


Description:
============
 This example demonstrates the Apollo4 Family CLKOUT feature.



Purpose:
========
This example enables CLKOUT, configures a pin to output the CLKOUT
 signal and sets up a GPIO interrupt to count the number of low-to-high
 transitions of CLKOUT.
 The transitions are counted in the ISR in order to toggle an LED about
 once per second.

 A logic analyzer can be attached to the pin specified by CLKOUT_PIN to
 observe the CLKOUT signal.

Additional Information:
=======================
 Printing takes place over the SWO/ITM at 1M Baud.



******************************************************************************


