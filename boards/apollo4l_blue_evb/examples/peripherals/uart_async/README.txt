Name:
=====
 uart_async


Description:
============
 A uart example that demonstrates the async driver.



Purpose:
========
This example demonstrates the usage of the asynchronous UART driver.<br>
 This driver allows the application to append UART Tx data to the
 Tx queue with interrupt code managing queue transmission.

 Similarly, the interrupt code will move received data into the Rx queue
 and the application periodically reads from the Rx queue.

 The Rx timeout interrupt has been enabled in this example.
 If insufficient Rx data triggers the FIFO full interrupt,
 the Rx timeout interrupt activates after a fixed delay.

 The associated ISR handler am_hal_uart_interrupt_queue_service() will return
 status in a bitfield, suitable for use as a callback or polling.

 Default Configuration:
 By default, this example uses UART1. The I/O pins used are defined in the BSP
 file as AM_BSP_GPIO_UART1_TX and AM_BSP_GPIO_UART1_RX

 Configuration and Operation:
 - This example requires enabling Tx and Rx fifos and queues.
 - It operates in a non-blocking manner without using callbacks.
 - The example monitors (polls) the Rx input queue.
 - It will transmit small blocks of data every second.

 To interact with these pins from a PC, the user should obtain a 1.8v uart/usb
 cable (FTDI, etc.).
 Using a terminal application (CoolTerm, RealTerm, Minicomm, etc.),
 the user will see data buffers being sent from the example
 (a different buffer every second), and the user can send data by typing.
 The swo output will report the character count the example receives.<

 The SWO output will send Rx/Tx status and error information.
 SWO Printing takes place over the ITM at 1M Baud.



******************************************************************************


