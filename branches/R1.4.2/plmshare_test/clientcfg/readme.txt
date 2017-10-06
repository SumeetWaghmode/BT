For each possible client configuration ther is a switch script:

PLMCare Integration: 					switch_4tier_server_plmcint.bat
BT Integration: 						switch_4tier_server_int.bat
BT Production:							switch_4tier_server_prod.bat
BT Production with RAS store/foreward	switch_4tier_server_prod_ras.bat
BT Production USA:						switch_4tier_server_prod_bfg.bat	
BT Production Peking:					switch_4tier_server_prod_pek.bat
BT Production Korea:					switch_4tier_server_prod_kpo.bat		


The scripts are called by the following script with the follwoing syntax
switch_4tier_clientcfg.bat [SITE] [Environment] [DEBUGFLAG]
[SITE]			mandatory - (ZUG, RAS, PEK, KPO, BFG, MRT)
[Environment]	mandatory - (prod, int, PLMCint)
[DEBUGFLAG]		optional - TEST (shows but not executes) ON (SHOWS DEBUG Informations) or OFF

in the startup scripts add (example below)
%SPLM_SHR_DIR%\clientcfg\switch_4tier_clientcfg.bat ZUG prod