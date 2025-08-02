//*****************************************************************************
//
//! @file tinyusb_config.h
//!
//! @brief contains prototypes for functions added to usb_descriptors_cdc_hid.h
//!
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
#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include "tusb.h"

typedef enum
{
    e_PROFILE_CDC,
    e_PROFILE_COMPOSITE_CDC,
    e_PROFILE_COMPOSITE_CDC_HID,
    e_PROFILE_MAX,
    e_PROFILE_x32 = 0x40000000,  // force size / sizeof to be 4 bytes
}
usb_profile_e;

//*****************************************************************************
//
//! @brief print the profile name that is in use
//!
//! @return profile enum
//
//*****************************************************************************
usb_profile_e usb_desc_print_profile_name(void);
//*****************************************************************************
//
//! @brief set profile to supplied enum
//!
//! @param eProfile
//
//*****************************************************************************
void usb_desc_set_profile(usb_profile_e eProfile);
//*****************************************************************************
//
//! @brief set profile to the next in line (in a circular fashion)
//!
//! @return enum for new profile
//
//*****************************************************************************
usb_profile_e usb_desc_set_next_profile(void);


#endif //USB_DESCRIPTORS_H
