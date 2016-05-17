rm *.o
echo 0
cp /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/zm/generated_files/*.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/
echo 1
/work/SVN/Log4KMW/log4kmw/def2cpp/x86_linux64bit/logspec2cpp /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/base.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/apu_uc_log_specification.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/apu_cu_log_specification.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/apu_ecu_log_specification.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/apu_fans.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/log_fgr.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/zm_logspec.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/log_specification.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/log_vehicle.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/rfh_ge.def /work/SVN/Leopard_2/01_Trunk/GE_A7/FGR/Appl/fgr_ppc/fgr_ppc_nc/logspec/defs/nav.def -o./generated_files
echo 2
make reader 
