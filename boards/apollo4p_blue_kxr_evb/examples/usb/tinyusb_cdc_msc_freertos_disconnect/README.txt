Name:
=====
 tinyusb_cdc_msc_freertos_disconnect


Description:
============
 tinyusb cdc-acm and mass storage FreeRTOS USB disconnect example.



Purpose:
========
This example demonstrates a composite USB device including one USB CDC-ACM
 and one mass storage device. the COM port will echo back the
 the input from the terminal and the mass storage device will be disk when
 connecting to other OS like Windows and Linux. The CDC-ACM and Mass storage
 functions will be handled by two separate FreeRTOS tasks.

 This example demonstrates how to use disconnect and reconnect
 (USB V bus voltage lost and restore) to shutdown / restart the usb module
 Since this is an example, designed to run on eval boards without USB V bus sensing,
 the two evb buttons have been drafted to simulate USB V bus voltage
 lost and recovered. The file usb_disconnect.c contains the
 button init and button scan code and also the method used to inform the
 tinyusb stack that it needs to service power up/power down calls.


******************************************************************************


