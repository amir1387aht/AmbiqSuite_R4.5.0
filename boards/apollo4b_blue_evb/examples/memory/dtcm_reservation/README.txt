Name:
=====
 dtcm_retention


Description:
============
 DTCM Variable Retention through reset



Purpose:
========
This example shows how to modify and reserve some TCM space to be able
 to save variables through a reset

 This example sets aside a 256 word TCM space using INFO0_SECURITY_SRAM_RESV.
 It also writes data to DTCM and uses a variable through reset to count the
 number of resets. It also checks the data section for corruption or loss.

Additional Information:
=======================
 The host example uses the ITM SWO to let the user know progress and 
 status of the demonstration.  The SWO is configured at 1M baud.


******************************************************************************


