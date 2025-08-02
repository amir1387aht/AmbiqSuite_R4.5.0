Name:
=====
 tinyusb_cdc_disconnect


Description:
============
 tinyusb disconnect/reconnect operation example



Purpose:
========
This example demonstrates how to use disconnect and reconnect
 (USB V bus voltage lost and restore) to shutdown / restart the usb module
 Since this is an example, designed to run on eval boards without USB V bus sensing,
 the two evb buttons have been drafted to simulate USB V bus voltage
 lost and recovered. The file usb_disconnect.c contains the
 button init and button scan code and also the method used to inform the
 tinyusb stack that it needs to service power up/power down calls.


******************************************************************************


