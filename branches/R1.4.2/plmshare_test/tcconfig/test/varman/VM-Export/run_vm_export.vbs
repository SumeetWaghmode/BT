Set objFSO = CreateObject("Scripting.FileSystemObject")
root = objFSO.GetParentFolderName(Wscript.ScriptFullName)

Set objArgs = Wscript.Arguments
If objArgs.Count = 0 Then
	Set objArgs = CreateObject("Scripting.Dictionary")
	objArgs.Add 0, root & "\test\VM-Export-out.xls"
	objArgs.Add 1, root & "\test\out1.csv"
	objArgs.Add 2, root & "\test\out2.csv"
End If

objFSO.CopyFile root & "\VM-Export.xls", CStr(objArgs(0)), True

Set objExcel = CreateObject("Excel.Application")
objExcel.Application.Visible = True

Set objWorkbook = objExcel.Workbooks.Open(CStr(objArgs(0)))
objExcel.Run "LoadCSV", CStr(objArgs(1))
If objArgs.Count > 2 Then objExcel.Run "DiffCSV", CStr(objArgs(2))

objWorkbook.Save

WScript.Quit
