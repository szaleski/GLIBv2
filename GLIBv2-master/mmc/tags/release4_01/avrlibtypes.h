//*****************************************************************************
//
// File Name	: 'avrlibtypes.h'
// Title		: AVRlib global types and typedefines include file
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
//	Description : Type-defines required and used by AVRlib.  Most types are also
//						generally useful.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef AVRLIBTYPES_H
#define AVRLIBTYPES_H

#ifndef WIN32
	#define FALSE	0
	#define TRUE	-1
#endif

// data type definitions macros
typedef unsigned char      u08;
typedef   signed char      s08;
typedef unsigned short     u16;
//typedef   signed short     s16;   //MJ: not used
typedef unsigned long      u32;
//typedef   signed long      s32;   //MJ: not used
//typedef unsigned long long u64;   //MJ: not used
//typedef   signed long long s64;   //MJ: not used

// maximum value that can be held
// by unsigned data types (8,16,32bits)
//#define MAX_U08	255               //MJ: not used
//#define MAX_U16	65535             //MJ: not used
//#define MAX_U32	4294967295        //MJ: not used

// maximum values that can be held
// by signed data types (8,16,32bits)
//#define MIN_S08	-128               //MJ: not used
//#define MAX_S08	127                //MJ: not used
//#define MIN_S16	-32768             //MJ: not used
//#define MAX_S16	32767              //MJ: not used
//#define MIN_S32	-2147483648        //MJ: not used
//#define MAX_S32	2147483647         //MJ: not used

#ifndef WIN32
	// more type redefinitions
//	typedef unsigned char   BOOL;   //MJ: not used
	typedef unsigned char	BYTE;
//	typedef unsigned int	WORD;   //MJ: not used
//	typedef unsigned long	DWORD;  //MJ: not used

//	typedef unsigned char	UCHAR;  //MJ: not used
	typedef unsigned int	UINT;
//	typedef unsigned short  USHORT; //MJ: not used
//	typedef unsigned long	ULONG;  //MJ: not used

//	typedef char			CHAR;   //MJ: not used
//	typedef int				INT;    //MJ: not used
//	typedef long			LONG;   //MJ: not used
#endif

#endif
