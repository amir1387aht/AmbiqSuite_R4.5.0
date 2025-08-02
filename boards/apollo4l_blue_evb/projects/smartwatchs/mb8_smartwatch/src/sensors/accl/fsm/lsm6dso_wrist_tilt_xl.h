/*
 ******************************************************************************
 * @file    lsm6dso_wrist_tilt_xl.h
 * @author  Sensors Software Solution Team
 * @brief   This file contains the configuration for lsm6dso_wrist_tilt_xl.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 */

 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LSM6DSO_WRIST_TILT_XL_H
#define LSM6DSO_WRIST_TILT_XL_H

#ifdef __cplusplus
extern "C" {
#endif

	/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#ifndef MEMS_UCF_SHARED_TYPES
#define MEMS_UCF_SHARED_TYPES

/** Common data block definition **/
	typedef struct {
		uint8_t address;
		uint8_t data;
	} ucf_line_t;

#endif /* MEMS_UCF_SHARED_TYPES */

	/** Configuration array generated from Unico Tool **/
	const ucf_line_t lsm6dso_wrist_tilt_xl[] = {
        // Disable sensors first
        {.address = 0x10, .data = 0x00,},  // CTRL1_XL off
        {.address = 0x11, .data = 0x00,},  // CTRL2_G off

        // Enable embedded functions
        {.address = 0x01, .data = 0x80,},  // EMB_FUNC_EN_A

        // FSM ODR
        {.address = 0x04, .data = 0x00,},
        {.address = 0x05, .data = 0x00,},

        // Enable FSM1
        {.address = 0x5F, .data = 0x01,},  // Only FSM1 enabled

        // FSM start address
        {.address = 0x46, .data = 0x01,},
        {.address = 0x47, .data = 0x00,},

        // Clear other FSM settings
        {.address = 0x0A, .data = 0x00,},
        {.address = 0x0B, .data = 0x01,},
        {.address = 0x0C, .data = 0x00,},
        {.address = 0x0E, .data = 0x00,},
        {.address = 0x0F, .data = 0x00,},
        {.address = 0x10, .data = 0x00,},
        {.address = 0x17, .data = 0x40,},

        // Program FSM
        {.address = 0x09, .data = 0x00,},
        {.address = 0x02, .data = 0x11,},
        {.address = 0x08, .data = 0x7A,},

        // Simple FSM: Trigger when Z > 0.3g (for inverted sensor)
        {.address = 0x09, .data = 0x00,},  // Reset
        {.address = 0x09, .data = 0x00,},
        {.address = 0x09, .data = 0x01,},  // CONFIG_A
        {.address = 0x09, .data = 0x01,},  // CONFIG_B
        {.address = 0x09, .data = 0x00,},
        {.address = 0x09, .data = 0x04,},  // Size

        {.address = 0x02, .data = 0x41,},
        {.address = 0x08, .data = 0x00,},

        // FSM program
        {.address = 0x09, .data = 0x53,},  // SINMUX
        {.address = 0x09, .data = 0x04,},  // Z-axis only
        {.address = 0x09, .data = 0x80,},  // COND (Any)
        {.address = 0x09, .data = 0x0F,},  // Low threshold for testing
        {.address = 0x09, .data = 0x06,},  // OUTC
        {.address = 0x09, .data = 0x22,},  // STOP

        // End program
        {.address = 0x04, .data = 0x00,},
        {.address = 0x05, .data = 0x01,},
        {.address = 0x17, .data = 0x00,},
        {.address = 0x01, .data = 0x00,},

        // Configure sensor
        {.address = 0x10, .data = 0x20,},  // 26Hz, 2g
        {.address = 0x11, .data = 0x00,},  // Gyro off
        {.address = 0x12, .data = 0x04,},  // BDU
        {.address = 0x13, .data = 0x00,},
        {.address = 0x14, .data = 0x00,},
        {.address = 0x15, .data = 0x00,},
        {.address = 0x58, .data = 0x00,},  // TAP_CFG2
        {.address = 0x0D, .data = 0x02,},  // INT1_CTRL - EMB_FUNC
        {.address = 0x5E, .data = 0x02,},  // MD1_CFG - FSM1
    };

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSO_WRIST_TILT_XL_H */
