#ifndef NEMA_RASTER_STROKED_ARC_AA_EX
#define NEMA_RASTER_STROKED_ARC_AA_EX

#include "nema_core.h"
#include "nema_raster.h"
#include "nema_provisional.h"
#include "nema_programHW.h"

//*****************************************************************************
//
//! @brief nema_raster_stroked_arc_aa_ex
//!
//! sPosition[i].p0_x = x0 + r * nema_cos(end_angle) - w/2*nema_cos(end_angle);
//! this get the PO cornor position
//! param x0 start x position
//! param y0 start y position
//! param r  arc radius
//! param w  arc width
//! param start_angle  start arc angle degree
//! param start_round_ending_enable flag to if enable the start round ending
//! param end_angle  end arc angle degree
//! param end_round_ending_enable flag to if enable the end round ending
//! param color drawing color
//!
//! @return none
//
//*****************************************************************************
void nema_raster_stroked_arc_aa_ex(float x0, float y0, float r, float w, 
                                   float start_angle, 
                                   bool start_round_ending_enable, 
                                   float end_angle, 
                                   bool end_round_ending_enable, 
                                   uint32_t color);

#endif
