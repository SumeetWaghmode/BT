#pragma once
#include "Doc_Rev_LOV_Migration.h"
#include"Loading_LOV_Data.h"

LOV_Data_Mapping::LOV_Data_Mapping()
{

	//cout<<"Initializing Class Elements "<<endl;
	primary_UID="";
	documentClass_entdoc="";
	documentSubClass_doctype="";
	documentSNType_docdossier="";
}

 LOV_Data_Mapping::~LOV_Data_Mapping()
 {

 }

/*******************************************************************************************************************************/
/*LOV_Data_Mapping Getter&Setter Functions*/

	void LOV_Data_Mapping::  setprimary_UID(string& tempprimary_UID)
	{
		primary_UID=tempprimary_UID;
	}
	void LOV_Data_Mapping:: setdocumentClass_entdoc(string& tempdocumentClass_entdoc)
	{
		documentClass_entdoc= tempdocumentClass_entdoc;
	}
	void LOV_Data_Mapping:: setdocumentSubClass_doctype(string& tempdocumentSubClass_doctype)
	{
		documentSubClass_doctype= tempdocumentSubClass_doctype;
	}
	void LOV_Data_Mapping::setdocumentSNType_docdossier(string& tempdocumentSNType_docdossier)
	{
		documentSNType_docdossier= tempdocumentSNType_docdossier;
	}

	string& LOV_Data_Mapping:: getprimary_UID()
	{
		return primary_UID; 
	}
	string& LOV_Data_Mapping::getdocumentClass_entdoc()
	{
		return documentClass_entdoc;
	}
	string& LOV_Data_Mapping::getdocumentSubClass_doctype()
	{
		return documentSubClass_doctype;
	}
	string& LOV_Data_Mapping::getdocumentSNType_docdossier()
	{
		return documentSNType_docdossier;
	}


 
 map<int ,LOV_Data_Mapping*>LOV_Data;
 map<int ,string>UID_Data;

  //Reading from File

  int Loading_ProcessedUIDvalue_From_File()
  {

	   ifstream myfileRead;
		string tempstringline;
		//int serial_No;
		string UIDvalue;

		myfileRead.open("UIDvalue.csv");
		if (!myfileRead) {
			cout<<"Unable to open file UIDvalue for reading ......"<<endl;
				return 0;
//			exit(1); // terminate with error
		}
		cout<<" UIDvalue.csv File opened for reading Successfully......"<<endl;
		while ((!myfileRead.eof()))
		{

				getline(myfileRead, tempstringline, '\n');
				UIDvalue=tempstringline;
			

				UID_Data.insert(pair<int ,string>(keyincrementerUID(), UIDvalue));

		}
		return 0;

  }
/*Static Counters for various files Data */
  
static int keyValue=0;
static int keyValueUID=0;



int& keyincrementerUID()
 {
	 ++keyValueUID;
	 return keyValueUID;
 }

 int& keyincrementer()
 {
	 ++keyValue;
	 return keyValue;
 }




