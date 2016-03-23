   ################################################################################################
   ###################### This is the SVN repository of the GBT-FPGA project ######################
   ################################################################################################

   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!                                                                                            !!
   !! * The different parameters of the GBT Bank are set through:                                !!  
   !!   (Note!! These parameters are vendor specific)                                            !!                    
   !!                                                                                            !!
   !!   - The MGT control ports of the GBT Bank module (these ports are listed in the records    !!
   !!     of the file "<vendor>_<device>_gbt_bank_package.vhd").                                 !! 
   !!     (e.g. xlx_v6_gbt_bank_package.vhd)                                                     !!
   !!                                                                                            !!  
   !!   - By modifying the content of the file "<vendor>_<device>_gbt_bank_user_setup.vhd".      !!
   !!     (e.g. xlx_v6_gbt_banks_user_setup.vhd)                                                 !! 
   !!                                                                                            !! 
   !! * The "<vendor>_<device>_gbt_banks_user_setup.vhd" is the only file of the GBT Bank that   !!
   !!   may be modified by the user. The rest of the files MUST be used as is.                   !!
   !!                                                                                            !!  
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   --==========--
   -- GBT Bank --
   --==========--

   * The top file of the GBT Bank can be found in:

     "..\gbt_bank\core_sources\"

   --=================--
   -- Example designs --
   --=================--

   * The top level file of the example design for the selected hardware platform can be found in:

     "..\example_designs\<fpga_serie>\<hardware_platform>\" 
     
     (e.g. "..\example_designs\xilinx_v6\glib\")

   * The project file of the example design for the selected hardware platform can be found in:

     "..\example_designs\<fpga_serie>\<hardware_platform>\<development_tool>_project\"
     
     (e.g. "..\example_designs\xilinx_v6\glib\ise_project\")

   * The pre-generated bitstream file of the example designs for the selected hardware platform can be found in: 

     "..\example_designs\<fpga_serie>\<hardware_platform>\bitstreams\"
     
     (e.g. "..\example_designs\xilinx_v6\glib\bitstreams\")

   --===========================--
   -- GBT Banks User Setup File --
   --===========================--

   * There is one "<hardware_platform>_gbt_banks_user_setup.vhd" file per FPGA serie.

   * The file "<hardware_platform>_gbt_banks_user_setup.vhd" can be found in:

     "..\gbt_bank\<fpga_serie>\"
     
     (e.g. "..\gbt_bank\xilinx_v6\")
 
   --=========--
   -- Contact --
   --=========--

   - Sophie Baron        (Sophie.Baron@cern.ch)
   - Manoel Barros Marin (manoel.barros.marin@cern.ch)

   ################################################################################################
   ################################################################################################