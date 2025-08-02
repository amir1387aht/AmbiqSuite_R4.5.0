Name:
=====
 binary_counter


Description:
============
 Example that displays the timer count on the LEDs.



Purpose:
========
This example increments a variable on every timer interrupt and
 uses a global variable to set the state of the LEDs. The example will
 sleep otherwise.

 The example switches between HIGH/LOW PERFORMANCE MODE once the global
 reaches the MAX_COUNT of LEDs on board and will reset back to 0x0 after.

Additional Information:
=======================
 Printing takes place over the SWO/ITM at 1M Baud.



******************************************************************************


