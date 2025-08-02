Name:
=====
 rng_test


Description:
============
 A simple example to demonstrate use of the mbedtls RNG.



Purpose:
========
This example demonstrates the use of the RSA CryptoCell Library<br>
 It initializes the mbedTLS crypto library.
 It then invokes crypto and uses it to produce a stream of random numbers.
 It then disables crypto and goes to sleep

Additional Information:
=======================
 To enable debug printing, add the following project-level macro definitions.
 When defined, debug messages will be sent over ITM/SWO at 1M Baud.
 - #define AM_DEBUG_PRINTF

 Note that when this macro is defined, the device will never achieve deep
 sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.


******************************************************************************


