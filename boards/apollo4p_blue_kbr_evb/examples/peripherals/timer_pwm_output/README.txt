Name:
=====
 timer_pwm_output


Description:
============
 TIMER PWM example.



Purpose:
========
This example shows one way to vary the brightness of an LED using an additional
 timer in PWM mode.
 The timer can be clocked from either the HFRC (default) or the LFRC.
 The led brightness varies over time, make the LEDs appear to breath.

 Two timers are used:
 AM_BSP_PWM_LED_TIMER is the used for the high frequency PWM. This controls
 pins attached to the LEDs.
 LED_CYCLE_TIMER is used for the lower frequency pwm duty cycle updates


 NOTE: This example shows a low power operation using reduced DTCM size
 The 8K size is very near the limit with most BSPs.
 The standard stack size had to be slightly  reduced for this example's
 RAM use to fit in 8K.(with IAR compiler)

 Printing takes place over the ITM at 1M Baud.



******************************************************************************


