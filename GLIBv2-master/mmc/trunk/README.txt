//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
//*****************************************************************************


The developement tools were used to create MMC project:

1.AVR Studio 4 -  an Integrated Development Environment (IDE) for writing and debugging AVR applications 
  in Windows 9x/ME/NT/2000/XP/VISTA environments.  AVR Studio provides a project management tool, source file editor,
  simulator,assembler and front-end for C/C++, programming, emulation and on-chip debugging.
  
2.WinAVR - a collection of executable software development tools for the Atmel AVR processor hosted on Windows.

These software development tools include:

    

     Compilers
    *

      Assembler
    *

      Linker
    *

      Librarian
    *

      File converter
    *

      Other file utilities
    *

      C Library
    *

      Programmer software
    *

      Debugger
    *

      In-Circuit Emulator software
    *

      Editor / IDE
    *

      Many support utilities


   CREATE AND COMPILE PROJECT:
   
   1.Create new GCC project in AVR Studio (do not create initial file)
   2.Select programmer and microcontroller(ATmegaXXX) to be used
   3.Add all files to the project.
  

//*************************************************************
// Copyright (C) 2012 CERN
//
//
//
// Author       : Markus Joos (markus.joos@cern.ch)
//
//***************************************************************


The code in this project is based on code developed originally by DESY (see above) and modified by CPPM. 
It has been modified and re-structured once more at CERN.

In its latest form the code has been split such that the general infrastructure is located in the files in this directory. 
The user code (i.e. the code that is specific to each AMC) has been pushed into sub-directories (at the time of writing: amc_alb_rtm and amc_test_rtm). 
A user who wants to implement his own AMC should:

- Create a new sub-directory
- Copy the files user_code.h and user_code.c from amc_alb_rtm to the new directory. These files are working templates and should be modified according to the needs of the new AMC
- Edit the file user_code_select.h such that it includes the user code for the new AMC.

Alternatively the user can copy his user files directly to the user_code folder.

NOTE: There may still be some minor user code sections in the general infrastructure. This will be fixed as we spot it. Please send feedback to markus.joos@cern.ch if you need support

Once the user files have been created use Version 6 of the AVR studio, open the project file "cern_mmc" and build the solution. 
Finally the binary can be downloaded into the NV-RAM of the MMC module
 
