Name:
=====
 tempco


Description:
============
 A brief demonstration of the Temperature Compensation feature.




Purpose:
========
This example initializes and invokes the TempCo feature.
 
  1) Initialize Temperature Compensation
  2) Periodically check for temperature change
  3) If temperature has changed, update the PWRCTRL values for lowest power
 
Additional Information:
=======================
 Debug messages will be sent over ITM/SWO at 1M Baud.
 
 Note that the device will never achieve deep sleep, only normal sleep,
 due to the ITM (and thus the HFRC) being enabled.



******************************************************************************


