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


extern  map<int ,LOV_Data_Mapping*>LOV_Data;
map<int ,LOV_Data_Mapping*>::iterator locator;

extern map<int ,string>UID_Data;
map<int ,string>::iterator Current_UID;

static int totalitemtobeprocesed=0;
static int processedcounter=0;
static int incremental_counter=0;

/* Main Body 
Calling of function to Create Table in Database*/
int ITK_user_main(int argc, char *argv[])
{
	char* table_name=NULL;
	logical	lInputError			= false;
	table_name = ITK_ask_cli_argument("-t=");
	Loading_ProcessedUIDvalue_From_File();
	/* Validate parameters */
	if(table_name == NULL || strcmp(table_name, "") == 0){
		printf(" > ERROR: Parameter '-t--(Tablename)' should not be empty! \n");
		lInputError = true;
	}
	if(lInputError==true)
	{
		return 0;
	}

	int iFail=ITK_ok;
	int bt7documentrevisioncount=0;
	char *bt7documentrevision=NULL;

	/* Logging into Teamcenter */
	printf(" > Login to Teamcenter(this might take a moment ...)\n");
	ITK_CALL(ITK_init_module("infodba","UGSup5050","dba") );
	AM__set_application_bypass( true );
	printf(" > Login successful \n");

	ITK_CALL(ReadingTableFromDatabase(&bt7documentrevisioncount,&bt7documentrevision,table_name));

	/* Logout */
	printf("\n");
	printf(" > Logout \n");
	AM__set_application_bypass( false );
	return 0;
}
/*Processing of Lodaed Elements in Storage*/
int ProcessingloadedElements()
{
	int ifail=ITK_ok;
	char* docidentifier=NULL;
	char* UID_Doc_Rev=NULL;
	string status="Failure";
	string UIDstatus="Failure";
	static string strUID;
	static int callingprocessedcounter=0;
	static long int totaldocrevisioncount=0;
	tag_t sitevalue=NULLTAG;
	tag_t docclassattribute=NULLTAG;
	tag_t docsubclassattribute=NULLTAG;
	tag_t docSNtypeattribute=NULLTAG;
	tag_t Documentrevisionclassid=NULLTAG;

	logical verdict=false;
	char* unloadedrevisionUID=NULL;
	logical reserved=true;
	tag_t docrevisiontag=NULLTAG;
	string primary_uid="";
	int processedindatabase=0;
	string Processed_UID_Status="Failure";
	string correctedvalue="";

	for(locator= LOV_Data.begin();locator!=LOV_Data.end();locator++)
	{ 
		UIDstatus="Failure";
		++processedindatabase;
		++incremental_counter;
		cout<<"!!PROCESSING ITEM NUMBER-------->>>  !!"<<incremental_counter<<endl;
		primary_uid=locator->second->getprimary_UID();
		TC_write_syslog("\n PROCESSING CURRENT UID -------->>!!!  %s\n",primary_uid);
		ITK__convert_uid_to_tag(primary_uid.c_str(),&docrevisiontag);
		if(docrevisiontag!=NULL)
		{
			ITK_CALL(RES_is_checked_out(docrevisiontag,&reserved));

			if(reserved==true)
			{
				ITK_CALL(POM_set_env_info( POM_bypass_attr_update, false, 0, 0, NULLTAG, NULL ));
				ITK_CALL(RES_checkin(docrevisiontag));
			}
			UIDstatus=CheckProcessedUIDValue(Processed_UID_Status,docrevisiontag);
			AOM_ask_value_tag(docrevisiontag,"owning_site",&sitevalue);
			if(sitevalue!=NULL)
              TC_write_syslog("\n SKIPPING MULTISITE UID -------->>!!!  %s\n",primary_uid);

			ITK_CALL(AOM_ask_value_string(docrevisiontag,"object_string",&docidentifier));

			if(strcmp(UIDstatus.c_str(),"Failure")==0 && sitevalue==NULL)
			{
				ITK__convert_tag_to_uid(docrevisiontag,&UID_Doc_Rev);
				strUID.assign(UID_Doc_Rev);
				UID_Data.insert(pair<int ,string>(keyincrementerUID(), strUID));
				if(processedindatabase==5000){
					Writing_UID_In_File("UIDvalue.csv",callingprocessedcounter);
					callingprocessedcounter++;
					processedindatabase=0;
				}
				ITK_CALL(POM_set_env_info( POM_bypass_attr_update, false, 0, 0, NULLTAG, NULL ));
				ITK_CALL(POM_is_loaded(docrevisiontag,&verdict));
				if(verdict==false)
				{
					TC_write_syslog(" > ERROR: TRYING TO LOAD REVISION ------------->> %s.\n", *(UID_Doc_Rev) );
					ITK_CALL(POM_class_of_instance(docrevisiontag,&Documentrevisionclassid));
					ITK_CALL(POM_load_instances(1,&docrevisiontag,Documentrevisionclassid, POM_modify_lock));
					ITK_CALL(POM_is_loaded(docrevisiontag,&verdict));
				}

				if(verdict==true)
				{
					ITK_CALL( AOM_refresh(docrevisiontag,true));

					ITK_CALL(POM_attr_id_of_attr ("bt7_documentClass","BT7_DocumentRevision",&docclassattribute));	
					correctedvalue=ReplaceCommaInattribute(locator->second->getdocumentClass_entdoc());
					ITK_CALL( POM_set_attr_string(1,&docrevisiontag,docclassattribute,correctedvalue.c_str()));
					correctedvalue="";

					ITK_CALL(POM_attr_id_of_attr ("bt7_documentSubClass","BT7_DocumentRevision",&docsubclassattribute));	
					correctedvalue=ReplaceCommaInattribute(locator->second->getdocumentSubClass_doctype());
					ITK_CALL( POM_set_attr_string(1,&docrevisiontag,docsubclassattribute,correctedvalue.c_str()));
					correctedvalue="";

					ITK_CALL(POM_attr_id_of_attr ("bt7_documentSNType","BT7_DocumentRevision",&docSNtypeattribute));
					correctedvalue=ReplaceCommaInattribute(locator->second->getdocumentSNType_docdossier());
					ITK_CALL( POM_set_attr_string(1,&docrevisiontag,docSNtypeattribute,correctedvalue.c_str()));
					correctedvalue="";

					ITK_CALL(AOM_save(docrevisiontag));

					ITK_CALL( AOM_refresh(docrevisiontag,false));

					POM_unload_instances(1,&docrevisiontag);
				}

			}
		}
		else
		{
			TC_write_syslog("\n ERROR-----UID NOT PRESENT IN DATABASE  %s\n",primary_uid);
		}
	}
	/*Function to free allocated memory to class objects*/
	FreeingLOVDatamemory();
	return ifail;
}

/*****************************************************************************************************************************************/
string& ReplaceCommaInattribute(string& sTobemodified)
{
	string modifiedvalue;
	modifiedvalue.assign(sTobemodified);
	sTobemodified="";
	for (string::iterator it = modifiedvalue.begin(); it != modifiedvalue.end(); ++it)	
	{
		char singlechar =*(it);
		stringstream tempstring;
		string storingvalue;
		tempstring << singlechar;
		tempstring >> storingvalue;
		if(storingvalue=="@"){
			storingvalue.clear();
			storingvalue.append(",");
		}
		sTobemodified.append(storingvalue);
	}
	return sTobemodified;
}
/*****************************************************************************************************************************************/
/*Function to write processed UID in csv file*/
int Writing_UID_In_File(const char* Filename,int& tempcallingprocessedcounter)
{

	ofstream myfileWrite;

	cout << " UIDvalue File opened successfully for writing...." << endl;
	if(tempcallingprocessedcounter==0)
	{
		myfileWrite.open(Filename, ios::out);
		myfileWrite.clear();

		myfileWrite  << " Processed UID Values "<<endl;

		for (Current_UID = UID_Data.begin(); Current_UID!= UID_Data.end(); Current_UID++) 
		{
			Current_UID->first;
			myfileWrite << Current_UID->second <<endl;

		}
	}
	else if(tempcallingprocessedcounter>=1)
	{
		myfileWrite.open(Filename,ios::app);
		for (Current_UID = UID_Data.begin(); Current_UID!= UID_Data.end(); Current_UID++) 
		{
			Current_UID->first;
			myfileWrite << Current_UID->second <<endl;

		}

	}
	UID_Data.clear();
	cout<<"The size of UID_Data map after clear is "<<UID_Data.size()<<endl;
	myfileWrite.close();

	return 0;
}
/*****************************************************************************************************************************************/
/*Function to check whether the item has been processed or not*/
string& CheckProcessedUIDValue(string& tempstatus,tag_t& docrevisiontag)
{
	char* UIDvalue;
	string strUIDvalue;
	ITK__convert_tag_to_uid(docrevisiontag,&UIDvalue);
	strUIDvalue.assign(UIDvalue);

	if(UID_Data.size()!=0){
		for(Current_UID= UID_Data.begin();Current_UID!=UID_Data.end();Current_UID++)
		{
			string storedUIDvalue;
			storedUIDvalue=Current_UID->second;
			if(strcmp(strUIDvalue.c_str(),storedUIDvalue.c_str())==0)
			{
				tempstatus="Success";
				return tempstatus;
			}
		}
	}
	SAFE_SM_FREE(UIDvalue);
	tempstatus="Failure";
	return tempstatus;
}
/*****************************************************************************************************************************************/
/*Freeing up the allocated memory for map LOV_Data */
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
/*Function to create table in database as per tbale name provided*/
int ReadingTableFromDatabase(int *tablecount,char **bt7documentrevision,char* table_name_input)
{
	try
	{
		int iFail = 0;
		int iStatus = ITK_ok;
		int objIdx	= 0;
		int n_rows	= 0;
		int n_cols	= 0;
		int attrCnt = 3;
		string columnvalue="";
		tag_t classTag=NULLTAG;
		bool valuetobeset=true;
		string UIDunderprocessing="";


		const char* enqID="FetchingData";
		const char* select_attrs[]={"PRIMARY_UID"};

		void ***report = NULL;
		tag_t eventtypeName=NULLTAG;
		tag_t Enqidtag=NULLTAG;

		char **table_name=&table_name_input;
		int n_columns=4;
		char **column_names= NULL;
		const char	    *select_attr_list[] =  {"PRIMARY_UID","BT7_DOCUMENTCLASS","BT7_DOCUMENTSUBCLASS","BT7_DOCUMENTSNTYPE"};
		int iRows, iCols =0;
		logical isRegistered = false;
		//    void    ***report	 = NULL;
		int column_types[4]={POM_string,POM_string,POM_string,POM_string};
		int column_sizes[4]={16,50,50,50};

		column_names = (char **) MEM_alloc(4*sizeof(char*));

		column_names[0]=(char *) MEM_alloc(sizeof(char)* ((int)tc_strlen("PRIMARY_UID") +1));
		tc_strcpy(column_names[0],"PRIMARY_UID");
		column_names[1]=(char *) MEM_alloc(sizeof(char)* ((int)tc_strlen("BT7_DOCUMENTCLASS") +1));
		tc_strcpy(column_names[1],"BT7_DOCUMENTCLASS");
		column_names[2]=(char *) MEM_alloc(sizeof(char)* ((int)tc_strlen("BT7_DOCUMENTSUBCLASS")+ 1));
		tc_strcpy(column_names[2],"BT7_DOCUMENTSUBCLASS");
		column_names[3]=(char *) MEM_alloc(sizeof(char)* ((int)tc_strlen("BT7_DOCUMENTSNTYPE")+ 1));
		tc_strcpy(column_names[3],"BT7_DOCUMENTSNTYPE");


		// Create SQL as select RPRIMARY_OBJECTU from BT7_SUPPLIERSPEC_temp
		ITK_CALL(POM_enquiry_create(enqID));

		ITK_CALL(POM_enquiry_register_table(*(table_name),n_columns,(const char **)column_names,column_types,column_sizes));
		ITK_CALL(POM_enquiry_is_table_registered(*(table_name),&isRegistered));

		ITK_CALL(POM_enquiry_create_class_alias(enqID,*(table_name),false,"custom_alias"));

		ITK_CALL(POM_enquiry_add_select_attrs ( enqID,  "custom_alias",  4, select_attr_list));

		ITK_CALL(POM_enquiry_execute(enqID, &n_rows, &n_cols, &report));	
		cout<<"\nIn number bt7_DocumentRevision of rows returned by enquiry\n"<<n_rows;
		if(n_rows>0)
		{
			totalitemtobeprocesed=n_rows;
			*tablecount = n_rows;	

			*bt7documentrevision = (char *) MEM_alloc ( n_rows * 15);

			for(objIdx=0 ; objIdx<n_rows; objIdx++)
			{	
				LOV_Data_Mapping* Datatobeprocessed = new LOV_Data_Mapping();
				void** col = report[objIdx];
				valuetobeset=true;

				for(int j=0 ; j<n_cols; j++)
				{
					/*Does not allow null as well '---' values to be entered in storage */
					if((char*)col[j]!=NULL && valuetobeset==true)
					{
						columnvalue = (char*)col[j];
						switch(j)
						{
						case 0:
							UIDunderprocessing=columnvalue;
							Datatobeprocessed->setprimary_UID(columnvalue);
							columnvalue="";
							break;
						case 1:
							if(strcmp(columnvalue.c_str(),"---")!=0){
							Datatobeprocessed->setdocumentClass_entdoc(columnvalue);
							columnvalue="";
							break;
							}
							else
							{
								valuetobeset=false;
							    break;
							}
						case 2:
							if(strcmp(columnvalue.c_str(),"---")!=0){
							Datatobeprocessed->setdocumentSubClass_doctype(columnvalue);
							columnvalue="";
							break;
							}
							else
							{
								valuetobeset=false;
							    break;
							}
						case 3:
							if(strcmp(columnvalue.c_str(),"---")!=0){
							Datatobeprocessed->setdocumentSNType_docdossier(columnvalue);
							columnvalue="";
							break;
							}
					    	else
							{
								valuetobeset=false;
							    break;
							}
						}	
					}
				}
				/*Checks value to be set status if false then does not insert entry in storage*/
				if(valuetobeset==true)
				 LOV_Data.insert(pair<int ,LOV_Data_Mapping*>(keyincrementer(), Datatobeprocessed));
				else
                  TC_write_syslog("\n FOUND VALUES NOTALLOWED IN TABLE -------->>!!!  %s\n",UIDunderprocessing);

				if(LOV_Data.size()==5000)
				{
					processedcounter=+LOV_Data.size();
					ProcessingloadedElements();

				}
				if(totalitemtobeprocesed-processedcounter<5000)
				{
					ProcessingloadedElements();
				}
				(*bt7documentrevision)[objIdx] = *((char*)report[objIdx][0]);

			}
		}
		ITK_CALL(POM_enquiry_delete(enqID));
		SAFE_SM_FREE(report);
	}
	catch(const std::exception &exc)
	{
		string error;
		std::cerr << exc.what();
		TC_write_syslog(" > ERROR: EXCEPTION CAUGHT!! ------------->>.\n");
	}

	return 0;

}
