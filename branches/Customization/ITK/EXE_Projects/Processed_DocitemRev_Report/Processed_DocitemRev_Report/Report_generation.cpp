#pragma once
//#include "Doc_Rev_LOV_Migration.h"
#include"Loading_LOV_Data.h"


// --------------------------------------------------------------------------

#define ITK_CALL(x)																	  \
	{																				  \
	int   stat;																	  \
	char *err_string;															  \
	if ( ( stat = ( x ) ) != ITK_ok )											  \
		{																			  \
		EMH_get_error_string(NULLTAG, stat, &err_string);						  \
		TC_write_syslog(" > ERROR: %d ERROR MSG: %s.\n", stat, err_string);				  \
		TC_write_syslog(" >  FUNCTION: %s\nFILE: %s LINE: %d\n", # x, __FILE__, __LINE__); \
		if ( err_string ) { MEM_free(err_string); }								  \
		/*exit(EXIT_FAILURE);														  */\
		}																			  \
	}																				  \


// --------------------------------------------------------------------------

/* Methods */
int FreeingLOVDatamemory();
int FreeingProcessedCombinations();
int FreeingUnprocessedCombinations();
string& CheckProcessedUIDValue(string&,tag_t&);
int Writing_Combinations_In_File_1(const char*,int&);
int Dumping_processedCombinations_In_File(ofstream& );
int Dumping_UnprocessedCombinations_In_File(ofstream &);
string UpdatingDatabase(Final_Data*,string&);
string Checkifexists(char*,string&);
int ReplaceCommaInattribute(char*,string&);


string& Checkifexistinprocessed(char*,string&);
string& CheckifexistinUnprocessed(char*,string&);

extern  map<int ,Final_Data*>Output_Data;
map<int ,Final_Data*>::iterator VerifiedData;

extern map<int ,Final_Data*>Output_Data_Unprocessed;
map<int ,Final_Data*>::iterator VerifiedDataUnprocessed;

extern map<int ,string>UID_Data;
map<int ,string>::iterator Current_UID;

extern map<int ,LOV_Data_Mapping*>LOV_Data;
map<int ,LOV_Data_Mapping*>::iterator locator;


/* Main Body */
int ITK_user_main(int argc, char *argv[])
{
	int PreviousProcessedCounter=0;
    int PreviousUnprocessedCounter=0;
	static long int totalprocessed=0;
	int newprocessed=0;
	int newunprocessed=0;

	Loading_ProcessedUIDvalue_From_File();
	Loading_DataStorage_From_File();
	Loading_ProcessedCombinations_From_File();
	Loading_UnprocessedCombinations_From_File();

	if(Output_Data.size()!=NULL){
	totalprocessed+=Output_Data.size();
	}
	if(Output_Data_Unprocessed.size()!=NULL){
	totalprocessed+=Output_Data_Unprocessed.size();
	}

	int iFail=ITK_ok;
	int  DoucmentRevcount=NULL;
	int num=1;
	int returnCount=0;
	int  remained=0;
	int counter=1;
	int site_id=0;
	static int callingcounter=0;
	static int callingcounterunprocessed=0;
	static long int totalitemstobeprocessed=0;



	char** entdocvalue=NULL;
	char** doctypevalue=NULL;
	char** docdossiervalue=NULL;
	char** docclass=NULL;
	char** docsubclass=NULL;
	char** docSntype=NULL;
	char* docidentifier=NULL;
	char** objectname=NULL;
	char** subtitle=NULL;

	char *group_full_name = NULL ;
	char *site_full_name = NULL ;
	char* unloadedrevisionUID=NULL;
	char* revlaststatus=NULL;
	char* owningroupvalue=NULL;
	char* UIDinprocessing=NULL;
	//char* sitename=NULL;

	tag_t * tDoucmentRevision=NULL;
	tag_t docrevision=NULLTAG;
	tag_t login_group_tag = NULLTAG ;
	tag_t owning_site_tag = NULLTAG ;
	tag_t sitevalue=NULLTAG;
	tag_t owninggroup=NULLTAG;

	string sOwninggroup;
	string sSitevalue;
	string UIDstatus="Failure";
	string modifiedValueIdentifier="";
	string modifiedValueObjectName="";
	string modifiedValueSubtitle="";
	string Processed_UID_Status="Failure";
	static string tempstatus="Failure";
	static string identifierstatus="Failure";
	string UID;

	logical verdict=false;
	logical reserved=true;

	/* Logging into Teamcenter */
	cout<<(" > Login to Teamcenter(this might take a moment ...)\n");
	ITK_CALL(ITK_init_module("infodba","UGSup5050","dba") );
	AM__set_application_bypass( true );
	cout<<(" > Login successful \n");

	Current_UID=UID_Data.begin();
	totalitemstobeprocessed=UID_Data.size();
	// for(Current_UID=std::next(Current_UID,1);Current_UID!=UID_Data.end();Current_UID++,counter++)
	for(Current_UID=UID_Data.begin();Current_UID!=UID_Data.end();Current_UID++,counter++)
	{
		cout<<"Processing UID number "<<counter<<endl;

		ITK__convert_uid_to_tag(Current_UID->second.c_str(),&docrevision);

		ITK__convert_tag_to_uid(docrevision,&UIDinprocessing);

		ITK_CALL(AOM_ask_value_string(docrevision,"object_string",&docidentifier));

		//Function call to check whether the current Doc Rev has been processed or not!
		Checkifexists(UIDinprocessing,identifierstatus);

		if(strcmp(identifierstatus.c_str(),"Failure")==0)
		{
			ITK_CALL(AOM_ask_displayable_values(docrevision,"bt7_entdoc_type",&num,&entdocvalue));
			ITK_CALL(AOM_ask_displayable_values(docrevision,"bt7_doctype",&num,&doctypevalue));
			ITK_CALL(AOM_ask_displayable_values(docrevision,"bt7_docdossier",&num,&docdossiervalue));
			ITK_CALL(AOM_ask_displayable_values(docrevision,"object_name",&num,&objectname));
			ITK_CALL(AOM_ask_displayable_values(docrevision,"bt7_Subtitle_1",&num,&subtitle));
			ITK_CALL(AOM_ask_value_string(docrevision,"bt7_RevLastStatus",&revlaststatus));
			ITK_CALL(AOM_ask_value_tag(docrevision,"owning_group",&owninggroup));
			

			if(owninggroup!=login_group_tag)
			{
				ITK_CALL( SA_ask_group_display_name( owninggroup, &group_full_name ), false ) ;
				// cout<<group_full_name<<endl;
			}
			//ITK_CALL(AOM_ask_value_string(owninggroup,"owning_group",&owningroupvalue));
			ITK_CALL(AOM_ask_value_tag(docrevision,"owning_site",&sitevalue));
			if(sitevalue!=owning_site_tag)
			{

				ITK_CALL( SA_ask_site_info2( sitevalue,&site_full_name, &site_id ), false ) ;
				// cout<<group_full_name<<endl;
			}
			else
			{
				site_full_name="NULL";
			}

			Final_Data* processedIRdata=new Final_Data;

			ReplaceCommaInattribute(docidentifier,modifiedValueIdentifier);
			processedIRdata->setidentifier(modifiedValueIdentifier.c_str());
			modifiedValueIdentifier="";

			processedIRdata->setentdoc_type_old(entdocvalue[0]);
			processedIRdata->setdoctype_old(doctypevalue[0]);
			processedIRdata->setdocdossier_old(docdossiervalue[0]);

			ReplaceCommaInattribute(objectname[0],modifiedValueObjectName);
			processedIRdata->setobject_name(modifiedValueObjectName.c_str());
			modifiedValueObjectName="";


			ReplaceCommaInattribute(subtitle[0],modifiedValueSubtitle);
			processedIRdata->setbt7_Subtitle_1(modifiedValueSubtitle.c_str());
			modifiedValueSubtitle="";

			processedIRdata->setrevisionlaststatus(revlaststatus);
			processedIRdata->setowning_Group(group_full_name);
			processedIRdata->setowning_Site(site_full_name);
			processedIRdata->setmatching_status(tempstatus.c_str());
			processedIRdata->setUIDvalue(UIDinprocessing);

			//Function call to update value of new LOV attribute
			UpdatingDatabase(processedIRdata,tempstatus);

			if(strcmp(processedIRdata->getmatching_status().c_str(),"Success")==0)
			{
				Output_Data.insert(pair<int ,Final_Data*>(ProcessedElementsincrementer(),processedIRdata));
				    ++newprocessed;
					totalprocessed++;
					if(newprocessed==1000){

						++callingcounter;
						TC_write_syslog("\nThe size of totalprocessed is  %d\n",totalprocessed);
						Writing_Combinations_In_File_1("ProcessedCombinations.csv",callingcounter);
						newprocessed=0;
					}
					else if ((totalitemstobeprocessed-totalprocessed)<1000)
					{
						remained=totalitemstobeprocessed-totalprocessed;
						TC_write_syslog("\nInside new condition (Remaining items in processed)  %d\n",remained);
						++callingcounter;
						Writing_Combinations_In_File_1("ProcessedCombinations.csv",callingcounter);
					}
			}
			else
			{
				Output_Data_Unprocessed.insert(pair<int ,Final_Data*>(UnprocessedElementsincrementer(),processedIRdata));
				    newunprocessed++;
					totalprocessed++;
					if(newunprocessed==1000){

						++callingcounterunprocessed;
						TC_write_syslog("\nThe size of totalunprocessed is  %d\n",totalprocessed);
						Writing_Combinations_In_File_1("UnprocessedCombinations.csv",callingcounterunprocessed);
						newunprocessed=0;
					}
					else if ((totalitemstobeprocessed-totalprocessed)<1000)
					{
						remained=totalitemstobeprocessed-totalprocessed;
						TC_write_syslog("\nInside new condition (Remaining items in unprocessed)  %d\n",remained);
						++callingcounterunprocessed;
						Writing_Combinations_In_File_1("UnprocessedCombinations.csv",callingcounterunprocessed);
					}
			}

		}

	}
	FreeingLOVDatamemory();
	SAFE_SM_FREE(revlaststatus);
	SAFE_SM_FREE(docidentifier);
	SAFE_SM_FREE(entdocvalue);
	SAFE_SM_FREE(doctypevalue);
	SAFE_SM_FREE(docdossiervalue);
	SAFE_SM_FREE(subtitle);
	SAFE_SM_FREE(objectname);
	/* Logout */
	cout<<("\n");
	cout<<(" > Logout \n");
	AM__set_application_bypass( false );
	return 0;
}

/*****************************************************************************************************************************************/
int ReplaceCommaInattribute(char* tempvalue,string& modifiedValue)
{
	string sTobemodified;
	//string sModified;
	sTobemodified.assign(tempvalue);

	for (string::iterator it = sTobemodified.begin(); it != sTobemodified.end(); ++it)	
	{
		char singlechar =*(it);
		stringstream tempstring;
		string storingvalue;
		tempstring << singlechar;
		tempstring >> storingvalue;
		if(storingvalue==","){
			storingvalue.clear();
			storingvalue.append("()");
		}
		modifiedValue.append(storingvalue);
	}
	/*tempvalue=new char(strlen(sModified.c_str()) + 1);
	strcpy(tempvalue, sModified.c_str());*/
	return 0;
}
/*****************************************************************************************************************************************/
string UpdatingDatabase(Final_Data* inputdata,string& tempstatus)
{
	tempstatus="Failure";

	string entdocvalue;
	string doctypevalue;
	string docdossiervalue;

	entdocvalue=inputdata->getentdoc_type_old();
	doctypevalue=inputdata->getdoctype_old();
	docdossiervalue=inputdata->getdocdossier_old();


	for(locator= LOV_Data.begin();locator!=LOV_Data.end();locator++)
	{
		if((strcmp(entdocvalue.c_str(),(locator->second->getentdoc_type_old()).c_str())==0) && (strcmp(doctypevalue.c_str(),(locator->second->getdoctype_old()).c_str())==0) && (strcmp(docdossiervalue.c_str(),(locator->second->getdocdossier_old()).c_str())==0))
		{
			inputdata->setdocumentClass_entdoc(locator->second->getdocumentClass_entdoc().c_str());
			inputdata->setdocumentSubClass_doctype(locator->second->getdocumentSubClass_doctype().c_str());
			inputdata->setdocumentSNType_docdossier(locator->second->getdocumentSNType_docdossier().c_str());

			tempstatus="Success";
			inputdata->setmatching_status(tempstatus.c_str());
			TC_write_syslog("\nThe status returned from match  is  %s\n",tempstatus);
			return tempstatus;
		}
	}

	inputdata->setdocumentClass_entdoc("---");
	inputdata->setdocumentSubClass_doctype("---");
	inputdata->setdocumentSNType_docdossier("---");
	tempstatus="Failure";
	inputdata->setmatching_status(tempstatus.c_str());

	//}
	TC_write_syslog("\nThe status returned from match  is  %s\n",tempstatus);
	return tempstatus;

}
/*****************************************************************************************************************************************/
/* Function to check if identifier exist in anyone of both files*/
string Checkifexists(char* tempdocidentifier,string& tempidentifierstatus)
{
	tempidentifierstatus=Checkifexistinprocessed(tempdocidentifier,tempidentifierstatus);

	if(strcmp(tempidentifierstatus.c_str(),"Failure")==0)
	{
		tempidentifierstatus=CheckifexistinUnprocessed(tempdocidentifier,tempidentifierstatus);
		if(strcmp(tempidentifierstatus.c_str(),"Success")==0)
			return tempidentifierstatus;
	}

	return tempidentifierstatus;

}
/*****************************************************************************************************************************************/
/*Wrapper  function to check if functions exist in ProcessedCombinations*/
string& Checkifexistinprocessed(char* tempdocidentifier,string& tempidentifierstatus)
{
	for (VerifiedData = Output_Data.begin(); VerifiedData!= Output_Data.end(); VerifiedData++) 
	{
		//	cout<<VerifiedData->second->getidentifier()<<endl;
		if(strcmp(tempdocidentifier,(VerifiedData->second->getUIDvalue()).c_str())==0)
		{
			tempidentifierstatus="Success";
			return tempidentifierstatus;
		}

	}
	tempidentifierstatus="Failure";
	return tempidentifierstatus;
}
/*****************************************************************************************************************************************/
/*Wrapper  function to check if functions exist in UnprocessedCombinations*/
string& CheckifexistinUnprocessed(char* tempdocidentifier,string& tempidentifierstatus)
{
	/*string s;
	s.assign(tempdocidentifier);*/
	for (VerifiedDataUnprocessed = Output_Data_Unprocessed.begin(); VerifiedDataUnprocessed!= Output_Data_Unprocessed.end(); VerifiedDataUnprocessed++) 
	{
		if(strcmp(tempdocidentifier,(VerifiedDataUnprocessed->second->getUIDvalue()).c_str())==0)
		{
			tempidentifierstatus="Success";
			return tempidentifierstatus;
		}
	}
	tempidentifierstatus="Failure";
	return tempidentifierstatus;
}

/*****************************************************************************************************************************************/
/*Function to write processed combinations in csv file*/
int Writing_Combinations_In_File_1(const char* Filename,int& callingcounter)
{

	ofstream myfileWrite;
	if ( callingcounter==1)
	{
		myfileWrite.open(Filename, ios::out);
		myfileWrite.clear();
		cout << Filename<< " file opened successfully for writing...." << endl;
		myfileWrite << "Identifier " << " , " << " bt7_entdoc_type "<<" , "<<" bt7_doctype "<<" , "<<" bt7_docdossier "<<" , "<<" bt7_documentClass "<<" , "<<" bt7_documentSubClass "<<" , "<<" bt7_documentSNType "<<" , "<<" Object_Name "<<" , "<<" Subtitle "<<" , "<<" Owning_Group "<<" , "<<" Owning_Site "<<" , "<<" RevisionLastStatus "<<" , "<<" UID_value "<<endl;

		if(strcmp(Filename,"ProcessedCombinations.csv")==0){
			Dumping_processedCombinations_In_File(myfileWrite);
		}
		else
		{
			Dumping_UnprocessedCombinations_In_File(myfileWrite);
		}
	}
	/*else 
	{
	cout << Filename<< " file could not be opened for writing...." << endl;
	}*/
	if(callingcounter>1)
	{
		//myfileWrite.is_open(),ofstream::app;
		myfileWrite.open(Filename,ios::app);
		if(strcmp(Filename,"ProcessedCombinations.csv")==0){
			Dumping_processedCombinations_In_File(myfileWrite);
		}
		else
		{
			Dumping_UnprocessedCombinations_In_File(myfileWrite);
		}
	}
	return 0;
}

/*****************************************************************************************************************************************/
/*Wrapper function to write processed_Combinations in file*/
int Dumping_processedCombinations_In_File(ofstream & myfileWrite)
{
	int i_size = 0;


	for (VerifiedData = Output_Data.begin(); VerifiedData!= Output_Data.end(); VerifiedData++) 
	{
		myfileWrite << (VerifiedData->second)->getidentifier()<<","<< (VerifiedData->second)->getentdoc_type_old()<<","<<(VerifiedData->second)->getdoctype_old()<<","<<(VerifiedData->second)->getdocdossier_old()<<","<<(VerifiedData->second)->getdocumentClass_entdoc()<<","<<(VerifiedData->second)->getdocumentSubClass_doctype()<<","<<(VerifiedData->second)->getdocumentSNType_docdossier()<<","<<(VerifiedData->second)->getobject_name()<<","<<(VerifiedData->second)->getbt7_Subtitle_1()<<","<<(VerifiedData->second)->getowning_Group()<<","<<(VerifiedData->second)->getowning_Site()<<","<<(VerifiedData->second)->getrevisionlaststatus()<<","<<(VerifiedData->second)->getUIDvalue()<<endl;
		//cout << (VerifiedData->second)->getidentifier()<< " , " << (VerifiedData->second)->getentdoc_type_old()<<" , "<<(VerifiedData->second)->getdoctype_old()<<" , "<<(VerifiedData->second)->getdocdossier_old()<<" , "<<(VerifiedData->second)->getdocumentClass_entdoc()<<" , "<<(VerifiedData->second)->getdocumentSubClass_doctype()<<" , "<<(VerifiedData->second)->getdocumentSNType_docdossier()<<" , "<<(VerifiedData->second)->getobject_name()<<" , "<<(VerifiedData->second)->getbt7_Subtitle_1()<<" , "<<(VerifiedData->second)->getowning_Group()<<" , "<<(VerifiedData->second)->getowning_Site()<<" , "<<(VerifiedData->second)->getrevisionlaststatus()<<endl;


	}
	myfileWrite.close();
	FreeingProcessedCombinations();
	return 0;
}

/*****************************************************************************************************************************************/
/*Wrapper function to write Unprocessed Combinations in file*/
int Dumping_UnprocessedCombinations_In_File(ofstream & myfileWrite)
{
	int i_size = 0;


	for (VerifiedDataUnprocessed = Output_Data_Unprocessed.begin(); VerifiedDataUnprocessed!= Output_Data_Unprocessed.end(); VerifiedDataUnprocessed++) 
	{
		myfileWrite << (VerifiedDataUnprocessed->second)->getidentifier()<< ","<< (VerifiedDataUnprocessed->second)->getentdoc_type_old()<<","<<(VerifiedDataUnprocessed->second)->getdoctype_old()<<","<<(VerifiedDataUnprocessed->second)->getdocdossier_old()<<","<<(VerifiedDataUnprocessed->second)->getdocumentClass_entdoc()<<","<<(VerifiedDataUnprocessed->second)->getdocumentSubClass_doctype()<<","<<(VerifiedDataUnprocessed->second)->getdocumentSNType_docdossier()<<","<<(VerifiedDataUnprocessed->second)->getobject_name()<<","<<(VerifiedDataUnprocessed->second)->getbt7_Subtitle_1()<<","<<(VerifiedDataUnprocessed->second)->getowning_Group()<<","<<(VerifiedDataUnprocessed->second)->getowning_Site()<<","<<(VerifiedDataUnprocessed->second)->getrevisionlaststatus()<<","<<(VerifiedDataUnprocessed->second)->getUIDvalue()<<endl;
		//cout << (VerifiedDataUnprocessed->second)->getidentifier()<< " , " << (VerifiedDataUnprocessed->second)->getentdoc_type_old()<<" , "<<(VerifiedDataUnprocessed->second)->getdoctype_old()<<" , "<<(VerifiedDataUnprocessed->second)->getdocdossier_old()<<" , "<<(VerifiedDataUnprocessed->second)->getdocumentClass_entdoc()<<" , "<<(VerifiedDataUnprocessed->second)->getdocumentSubClass_doctype()<<" , "<<(VerifiedDataUnprocessed->second)->getdocumentSNType_docdossier()<<" , "<<(VerifiedDataUnprocessed->second)->getobject_name()<<" , "<<(VerifiedDataUnprocessed->second)->getbt7_Subtitle_1()<<" , "<<(VerifiedDataUnprocessed->second)->getowning_Group()<<" , "<<(VerifiedDataUnprocessed->second)->getowning_Site()<<" , "<<(VerifiedDataUnprocessed->second)->getrevisionlaststatus()<<endl;


	}
	myfileWrite.close();
	FreeingUnprocessedCombinations();
	return 0;
}
/*****************************************************************************************************************************************/
/*Freeing up the allocated memory for map Output_Data */
int FreeingLOVDatamemory()
{
	for ( locator = LOV_Data.begin(); locator != LOV_Data.end(); ++locator)
	{
		// found it - delete it
		LOV_Data_Mapping* point = locator->second;
		// delete it->second;
		delete(point);
	}
	LOV_Data.clear();

	cout<<"The size of map LOV_Data after clear is "<<LOV_Data.size()<<endl;
	TC_write_syslog("\nThe size of map LOV_Data after clear is  %d\n",LOV_Data.size());


	return 0;
}
/*****************************************************************************************************************************************/
/*Freeing up the allocated memory for map Output_Data */
int FreeingProcessedCombinations()
{
	for ( VerifiedData = Output_Data.begin(); VerifiedData != Output_Data.end(); ++VerifiedData)
	{
		// found it - delete it
		Final_Data* point = VerifiedData->second;
		// delete it->second;
		delete(point);
	}
	Output_Data.clear();

	cout<<"The size of map LOV_Data after clear is "<<Output_Data.size()<<endl;
	TC_write_syslog("\nThe size of map LOV_Data after clear is  %d\n",Output_Data.size());


	return 0;
}
/*****************************************************************************************************************************************//*Freeing up the allocated memory for map Output_Data */
int FreeingUnprocessedCombinations()
{
	for ( VerifiedDataUnprocessed = Output_Data_Unprocessed.begin(); VerifiedDataUnprocessed != Output_Data_Unprocessed.end(); ++VerifiedDataUnprocessed)
	{
		// found it - delete it
		Final_Data* point = VerifiedDataUnprocessed->second;
		// delete it->second;
		delete(point);
	}
	Output_Data_Unprocessed.clear();

	cout<<"The size of map LOV_Data after clear is "<<Output_Data_Unprocessed.size()<<endl;
	TC_write_syslog("\nThe size of map LOV_Data after clear is  %d\n",Output_Data_Unprocessed.size());


	return 0;
}
/*****************************************************************************************************************************************/
