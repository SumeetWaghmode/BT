rem Role "NXChecker" is missing in Group. Should be in Group "110_MDesign_A", "110_MDesign_B", "110_MDesign_A".
cd  /D %TC_ROOT%

cd bin 

make_user -u=infodba -p=UGSup5050 -g=dba  -group=110_MDesign_A -role="NXChecker"
make_user -u=infodba -p=UGSup5050 -g=dba  -group=110_MDesign_B -role="NXChecker"
make_user -u=infodba -p=UGSup5050 -g=dba  -group=110_MDesign_D -role="NXChecker"