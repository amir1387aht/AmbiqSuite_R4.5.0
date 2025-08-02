Name:
=====
 rsa_sign_verify


Description:
============
 A simple example to demonstrate using runtime crypto APIs.



Purpose:
========
This example demonstrates the use of the RSA Crypto Library.<br>
 It initializes the runtime crypto lib at the beginning.
 It then invokes crypto and use it to do SHA and RSA.
 If ENABLE_CRYPTO_ON_OFF is defined, it also controls Crypto power and keeps
   it active only while in use.

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


