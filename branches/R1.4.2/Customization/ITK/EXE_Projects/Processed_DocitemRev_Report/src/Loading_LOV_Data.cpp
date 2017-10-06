#pragma once
#include "Report_generation.h"
#include"Loading_LOV_Data.h"

Final_Data::Final_Data()
{

	//cout<<"Initializing Class Elements "<<endl;
	identifier="";
	entdoc_type_old="";
	doctype_old="";
	docdossier_old="";
	documentClass_entdoc="";
	documentSubClass_doctype="";
	documentSNType_docdossier="";
	object_name="";
	bt7_Subtitle_1="";
	owning_Group="";
	owning_Site="";
	matching_status="";
	UIDvalue="";
}

 Final_Data::~Final_Data()
 {

 }

 LOV_Data_Mapping::LOV_Data_Mapping()
{

	//cout<<"Initializing Class Elements "<<endl;
	entdoc_type_old="";
	doctype_old="";
	docdossier_old="";
	documentClass_entdoc="";
	documentSubClass_doctype="";
	documentSNType_docdossier="";
}

 LOV_Data_Mapping::~LOV_Data_Mapping()
 {

 }
/*******************************************************************************************************************************/
/*LOV_Data_Mapping Getter&Setter Functions*/


 void Final_Data:: setidentifier(const char* tempidentifier)
	{
		identifier.assign(tempidentifier);
	}
	void Final_Data:: setentdoc_type_old(const char* tempentdoc_type_old)
	{
		entdoc_type_old.assign(tempentdoc_type_old);
	}

	void Final_Data:: setdoctype_old(const char* tempdoctype_old)
	{
		doctype_old.assign(tempdoctype_old);
	}
	void Final_Data:: setdocdossier_old(const char* tempdocdossier_old)
	{
		docdossier_old.assign( tempdocdossier_old);
	}
	void Final_Data:: setdocumentClass_entdoc(const char* tempdocumentClass_entdoc)
	{
		documentClass_entdoc.assign( tempdocumentClass_entdoc);
	}
	void Final_Data:: setdocumentSubClass_doctype(const char* tempdocumentSubClass_doctype)
	{
		documentSubClass_doctype.assign( tempdocumentSubClass_doctype);
	}
	void Final_Data::setdocumentSNType_docdossier(const char* tempdocumentSNType_docdossier)
	{
		documentSNType_docdossier.assign(tempdocumentSNType_docdossier);
	}

	void Final_Data::setobject_name(const char* tempobject_name)
	{
		object_name=tempobject_name;
	}
	void Final_Data::setbt7_Subtitle_1(const char* tempbt7_Subtitle_1)
	{
		bt7_Subtitle_1=tempbt7_Subtitle_1;
	}
	void Final_Data:: setowning_Group(const char* tempowning_Group)
	{
		owning_Group=tempowning_Group;
	}
	void Final_Data:: setowning_Site(const char* tempowning_Site)
	{
		owning_Site=tempowning_Site;
	}

	void Final_Data:: setrevisionlaststatus(const char* temprevisionlaststatus)
	{
		revisionlaststatus=temprevisionlaststatus;
	}

	void Final_Data:: setmatching_status(const char* tempmatching_status)
	{
		matching_status=tempmatching_status;
	}

	void Final_Data:: setUIDvalue(const char* tempUIDvalue)
	{
		UIDvalue=tempUIDvalue;
	}

	//getter functions
	string& Final_Data:: getidentifier()
	{
		return identifier;
	}

	string& Final_Data:: getentdoc_type_old()
	{
		return entdoc_type_old;
	}
	string& Final_Data::getdoctype_old()
	{
		return doctype_old;
	}
    string& Final_Data::getdocdossier_old()
	{
		return docdossier_old;
	}
	string& Final_Data::getdocumentClass_entdoc()
	{
		return documentClass_entdoc;
	}
	string& Final_Data::getdocumentSubClass_doctype()
	{
		return documentSubClass_doctype;
	}
	string& Final_Data::getdocumentSNType_docdossier()
	{
		return documentSNType_docdossier;
	}

	string& Final_Data:: getobject_name()
	{
		return object_name;
	}
	string& Final_Data:: getbt7_Subtitle_1()
	{
		return bt7_Subtitle_1;
	}
	string& Final_Data:: getowning_Group()
	{
		return owning_Group;
	}
	string& Final_Data:: getowning_Site()
	{
		return owning_Site;
	}
	string&  Final_Data:: getrevisionlaststatus()
	{
		return revisionlaststatus;
	}
	string&  Final_Data:: getmatching_status()
	{
		return  matching_status;
	}

	string&  Final_Data:: getUIDvalue()
	{
		return  UIDvalue;
	}


	/*******************************************************************************************************************************/
/*LOV_Data_Mapping Getter&Setter Functions*/

	void LOV_Data_Mapping:: setentdoc_type_old(string& tempentdoc_type_old)
	{
		entdoc_type_old=tempentdoc_type_old;
	}

	void LOV_Data_Mapping:: setdoctype_old(string& tempdoctype_old)
	{
		doctype_old= tempdoctype_old;
	}
	void LOV_Data_Mapping:: setdocdossier_old(string& tempdocdossier_old)
	{
		docdossier_old= tempdocdossier_old;
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

	string& LOV_Data_Mapping:: getentdoc_type_old()
	{
		return entdoc_type_old;
	}
	string& LOV_Data_Mapping::getdoctype_old()
	{
		return doctype_old;
	}
    string& LOV_Data_Mapping::getdocdossier_old()
	{
		return docdossier_old;
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



 map<int ,string>UID_Data;
 map<int ,Final_Data*>Output_Data;
 map<int ,Final_Data*>Output_Data_Unprocessed;
 map<int ,LOV_Data_Mapping*>LOV_Data;

 //extern int PreviousProcessedCounter;
//extern int PreviousUnprocessedCounter;


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
		cout<<"File opened for reading Successfully......"<<endl;
		while ((!myfileRead.eof()))
		{

				/*getline(myfileRead, tempstringline, ',');
				serial_No=(atoi(tempstringline.c_str()));*/

				getline(myfileRead, tempstringline, '\n');
				UIDvalue=tempstringline;
			

				UID_Data.insert(pair<int ,string>(UIDincrementer(), UIDvalue));
		}
		return 0;
  }

  //Reading from File
  int Loading_DataStorage_From_File()
	{
	 ifstream myfileRead;
		string tempstringline;

		myfileRead.open("Doc_Rev_LOV_Migration_Data.csv");
		if (!myfileRead) {
			cout<<"Unable to open file Doc_Rev_LOV_Migration_Data for reading ......"<<endl;
			exit(1); // terminate with error
		}
		cout<<"File opened for reading Successfully......"<<endl;
		while ((!myfileRead.eof()))
		{
			LOV_Data_Mapping* Data = new LOV_Data_Mapping();
				//getline(myfileRead, tempstringline, ',');
				

				getline(myfileRead, tempstringline, ',');
				Data->setentdoc_type_old(tempstringline);

				getline(myfileRead, tempstringline, ',');
				Data->setdoctype_old(tempstringline);

				getline(myfileRead, tempstringline, ',');
				Data->setdocdossier_old(tempstringline);
				
				getline(myfileRead, tempstringline, ',');
				Data->setdocumentClass_entdoc(tempstringline);
				
				getline(myfileRead, tempstringline, ',');
				Data->setdocumentSubClass_doctype(tempstringline);

				getline(myfileRead, tempstringline, '\n');
				Data->setdocumentSNType_docdossier(tempstringline);

				LOV_Data.insert(pair<int ,LOV_Data_Mapping*>(keyincrementer(), Data));

		}
		return 0;
	}

 int Loading_ProcessedCombinations_From_File()
 {
	 Loading_Combinations_From_File("ProcessedCombinations.csv");
	 return 0;
 }
int Loading_UnprocessedCombinations_From_File()
{
	Loading_Combinations_From_File("UnprocessedCombinations.csv");
	return 0;
}

/*Wrapper function to print combinations in file*/
  int Loading_Combinations_From_File(const char* Filename)
  {

	    ifstream myfileRead;
		string tempstringline;
//		int serial_No;

		myfileRead.open(Filename);
		if (!myfileRead) {
			cout<<Filename<<" Unable to open file UnprocessedCombinations for reading ......"<<endl;
				return 0;
//			exit(1); // terminate with error
		}
		cout<<Filename<<" File opened for reading Successfully......"<<endl;
		while ((!myfileRead.eof()))
		{
			Final_Data *CombinationsData=new Final_Data;

			   getline(myfileRead, tempstringline, ',');
				CombinationsData->setidentifier((tempstringline.c_str()));
			
				getline(myfileRead, tempstringline, ',');
				CombinationsData->setentdoc_type_old((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setdoctype_old((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setdocdossier_old((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setdocumentClass_entdoc((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setdocumentSubClass_doctype((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setdocumentSNType_docdossier((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setobject_name((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setbt7_Subtitle_1((tempstringline.c_str()));
				
				getline(myfileRead, tempstringline, ',');
				CombinationsData->setowning_Group((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setowning_Site((tempstringline.c_str()));

				getline(myfileRead, tempstringline, ',');
				CombinationsData->setrevisionlaststatus((tempstringline.c_str()));

				getline(myfileRead, tempstringline, '\n');
				CombinationsData->setUIDvalue((tempstringline.c_str()));

				if(strcmp(Filename,"ProcessedCombinations.csv")==0){
				Output_Data.insert(pair<int ,Final_Data*>(ProcessedElementsincrementer(), CombinationsData));
				//PreviousProcessedCounterfunction();
				}
				else{
				Output_Data_Unprocessed.insert(pair<int ,Final_Data*>(UnprocessedElementsincrementer(), CombinationsData));
				//PreviousUnprocessedCounterfunction();
				}
		}
		return 0;
  }

/*Static Counters for various files Data */
  
static int UIDkeyValue=0;
static int keyValue=0;
static int UnprocessedElementsvalue=0;
static int ProcessedElementsvalue=0;


  int& UIDincrementer()
 {
	 ++UIDkeyValue;
	 return UIDkeyValue;
 }
  
 int& keyincrementer()
 {
	 ++keyValue;
	 return keyValue;
 }

   int& UnprocessedElementsincrementer()
  {
	  ++UnprocessedElementsvalue;
	  return UnprocessedElementsvalue;
  }

   int& ProcessedElementsincrementer()
  {
	  ++ProcessedElementsvalue;
	  return ProcessedElementsvalue;
  }