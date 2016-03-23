//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: AVR project global include 
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H

#include "avrlibdefs.h"   // global AVRLIB defines
#include "avrlibtypes.h"  // global AVRLIB types definitions


// project/system dependent constants
// CPU clock speed
#define f_cpu         4000000                	    // 4MHz processor
#define CYCLES_PER_US ((f_cpu + 500000) / 1000000) 	// CPU cycles per microsecond

//MJ: The formula looks wrong:
//   4000000
// +  500000
// = 4500000
// / 1000000
// = 4.5 (instead of 4)
// Why do we add 500000?

#endif
