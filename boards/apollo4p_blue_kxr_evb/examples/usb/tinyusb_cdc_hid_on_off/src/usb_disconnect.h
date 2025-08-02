//*****************************************************************************
//
//! @file usb_disconnect.h
//!
//! @brief tinyusb cdc- button scan with USB disconnect/connect.
//!
//! This function programs the buttons to  disconnect and reconnect
//! (USB V bus voltage lost and restore) to shutdown / restart the usb module
//! This specifically contains code for helper functions that
//! will poll the buttons on an evb. These are used to simulate programmable actions
//! where the firmware wants to disable on USB profile and start another.
//! the example here is switching between three profiles
//!   CDC
//!   composite CDC
//!   composite CDC - HID
//!
//! when the back button (closet to edge) is pressed
//! usb will be disconnected and powered down
//! when the forward button is pressed
//! the next profile is selected and the usb is re-started causing a USB enumeration
//!
//*****************************************************************************
//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef USB_DISCONNECT_EXAMP_H
#define USB_DISCONNECT_EXAMP_H

#include "am_mcu_apollo.h"

//*****************************************************************************
//
//! @brief This is used to configure the button press.
//
//*****************************************************************************
extern void usb_disconnect_init( void );

//*****************************************************************************
//
//! @brief periodic button scan mgr, this is used to simulate a usb disconnect
//!
//! @details this is called periodically to scan the buttons and
//! initiate action when pressed
//!
//! @note, Used to simulate internally USB requested power down, and internally
//! requested USB power up. On simulated USB power up the USB configuration
//! is advanced (e.g. from CDC to CDC-HID)
//!
//! @param calledFromIsrOrThread set to true when in multithreaded environment
//!                              or when calling from ISR
//
//*****************************************************************************
extern void usb_disconnect_periodic(bool calledFromIsrOrThread);

#endif //USB_DISCONNECT_EXAMP_H
