Name:
=====
 deepsleep


Description:
============
 Example demonstrating how to enter deepsleep.




Purpose:
========
This example configures the device to go into a deep sleep mode.
 Once in sleep mode the device has no ability to wake up. This example is
 merely to provide the opportunity to measure deepsleep current without
 interrupts interfering with the measurement.
 
   1) Initialize memory for performance and low power
   2) Place device into deepsleep
   3) Measure current draw

 This example uses a button press upon power up to allow the user to retain
 the minimum amount of DTCM. If not pressed, no DTCM will be retained in
 deepsleep.
 
 The example begins by printing out a banner announcement message through
 the UART, which is then completely disabled for the remainder of execution.
 
 Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
 Please note that text end-of-line is a newline (LF) character only.
 Therefore, the UART terminal must be set to simulate a CR/LF.


******************************************************************************


