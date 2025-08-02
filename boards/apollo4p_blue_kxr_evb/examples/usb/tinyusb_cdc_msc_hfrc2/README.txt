Name:
=====
 tinyusb_cdc_msc_hfrc2


Description:
============
 tinyusb cdc-acm / msc example.




Purpose:
========
This example demonstrates a composite USB device including
 one USB CDC-ACM and one mass storage device.
 This defaults to USB-HIGH speed operation.

 This example show how to enable HFRC2 -tune. In this mode, the HFRC2 FLL is
 usually off to minimize frequency jitter and save power. Periodically
 HFRC2 tune will be called under application control
 to adjust the output frequency. This will compensate for frequency drift
 due to temperature changes.

 The COM port will echo back the
 the input from the terminal and the mass storage device will mount as a disk
 when connecting to other OS like Windows and Linux.



******************************************************************************


