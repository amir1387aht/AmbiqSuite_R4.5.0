Name:
=====
 pwr_32mhz


Description:
============
 This demonstrates using the Apollo4 32Mhz XTHS and HFRC2




Purpose:
========
This example shows how to:
  1. Enable 32Mhz XTHS.
  2. Enable 32Mhz clockout.
  2. Enable HFRC2 clockout.
  4. Enable HFRC2:
     a. Enable normal HFRC2 with FLL (HFRC2-adjust)
     b. Use HFRC2 tune where the FLL is disabled, and HFRC2 adjust is run periodically.
     c. Disable the XTHS once the FLL is disabled

 Default HFRC2 will output a default frequency based on a calibrated tuned value.
 HFRC2 adjust will use the 32Mhz XTHS and an FLL to generate the
 user requested Frequency. However the FLL can add some frequency jitter.

 HFRC2 tune, will use HFRC2 adjust to periodically compute a new tune value
 This process takes about 1msec. Once the tune value is computed,HFRC2 adjust
 (the FLL) is disabled and the tune value is used to generate the HFRC2 freq.
 Using HFRC2-tune enables the HFRC2 to run with reduced jitter and reduced power use.

 When the HFRC2 FLL is off, the XTHS may optionally be disabled (if it isn't used elsewhere)
 Disabling the 32Mhz XTHS will use less power. A delay is necessary and built into the
 HAL to allow time for the 32Mhz XTHS to start before starting HFRC2 FLL.

 A logic analyzer or oscilloscope can be attached to the CLOCKOUT pins


******************************************************************************


