Name:
=====
 tinyusb_cdc_hid_on_off


Description:
============
 TinyUSB HID On/Off Example.



Purpose:
========
This example demonstrates how to use the USB dcd_powerdown()
 and dcd_powerup() to shutdown the USB subsystem,
 change the profile, then power back up with that changed profile.

 This example uses CDC, CDC-composite, and CDC-HID descriptors
 one of these USB profile is active when the USB enumerates.

 Some notes: a different USB PID is used for each profile.
 On an evb, SW2 is used to disconnect,
 the SW1 is used to advance the usb configuration and re-connect.

 A PC program like USBDview can be used to watch the configured devices change
 as the user scrolls through the USB profiles.

 The actual usb disconnect and reconnect code is in usb_disconnect.c
 This uses tinyusb callbacks and tinyusb deferred function calls


******************************************************************************


