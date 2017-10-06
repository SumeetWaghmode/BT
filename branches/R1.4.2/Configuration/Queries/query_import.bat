@echo off

for %%f in (*.xml) do call %TC_ROOT%\bin\plmxml_import -u=infodba -p=UGSup5050 -g=dba -xml_file=%%~nxf -import_mode=overwrite
rem for %%f in (*.xml) do call %TC_ROOT%\bin\plmxml_import -u=infodba -p=UGSup5050 -g=dba -xml_file=BT7_SBT_CommonComponentsByVendorPart.xml -import_mode=overwrite