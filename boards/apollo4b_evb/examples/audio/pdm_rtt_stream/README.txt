Name:
=====
 pdm_rtt_stream


Description:
============
 An example to show PDM audio streaming to PC over RTT data logger.




Purpose:
========
This example enables the PDM interface to record audio signals from an
 external microphone.
 
 Build and download the program into the target device.
 Reset the target and check the SWO print for PDM settings.
 Run the helper script 'rtt_logger.py' in the project folder

 NOTE: python3 rtt_logger.py

 (In this example, RTT control block is mapped to a fixed address to facilitate
  the searching process. If the address is changed, make sure to modify
  the rtt_logger.py script to match the address.)
 
 During data streaming, press any key to stop the recording.
 The audio captured is stored into the same folder as the rtt_logger.py,
 with a file name of yyyymmdd-hh-mm-ss.pcm.
 
 To check the audio, load the *.pcm file into audio analysis tools and check.
 E.g. in Audacity, https://www.audacityteam.org/:
 - File -> Import -> Raw data...
 - Default import format is:
   - Signed 24-bit PCM
   - Little-endian
   - 2 Channels (Stereo)
   - Start offset: 0 bytes
   - Amount to import: 100%
   - Sample rate: 16000 Hz
 
 The required pin connections are:
   - PDM0 -
     GPIO 50 - PDM0 CLK
     GPIO 51 - PDM0 DATA
 
   - PDM1 -
     GPIO 52 - PDM1 CLK
     GPIO 53 - PDM1 DATA
 
   - PDM2 -
     GPIO 54 - PDM2 CLK
     GPIO 55 - PDM2 DATA
 
   - PDM3 -
     GPIO 56 - PDM3 CLK
     GPIO 57 - PDM3 DATA

 NOTE: On Apollo4l, there is only 1 PDM instance (PDM0).

Additional Information:
=======================
 Printing takes place over the ITM at 1M Baud.
 RTT logger takes place over the SWD at 4M Baud.


******************************************************************************


