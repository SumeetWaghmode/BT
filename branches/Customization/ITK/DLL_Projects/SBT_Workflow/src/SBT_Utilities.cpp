/**
 * This file is to hold the implementation of the general utility methods.
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include <tccore/grm.h>
#include <ae/ae.h>
#include <tc/emh.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <fclasses/tc_date.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <time.h>
#include <fclasses/tc_date.h>
#include <sa/am_bypass.h>
#include <tccore/item.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <bom/bom.h>

#include "sy_debug.h"

#include "SBT_Utilities.h"

#ifdef __cplusplus
}
#endif

// Include Common
#include "SBT_Common.h"

/**
 * @brief A convenience function to store errors.
 *        Insert text stringt, comma seperated, terminate with a null.
 */
int SBT_EMH_store_error
  (
    const char *text, 
    ...
  )
{
    int len = 0;

    va_list lst;
    const char *ptr = NULL;

    char *complete_text = NULL;

    TRACE_ON(0);

    /* initialize the f...... global variable */

    retcode = ITK_ok;

    /* get the length og the complete error string */

    ptr = text;
    va_start(lst, text);

    while (ptr) 
    {
        len += (int) strlen(ptr);
        ptr = va_arg(lst, char *);
    }

    va_end(lst);

    len++; /* tailing zero */

    /* allocate mem for the error string */

    complete_text = (char *)MEM_alloc(len * sizeof(char));

    if (complete_text == NULL) 
    {
        retcode = SS_NOMEM;
    }

    if (retcode == ITK_ok) 
    {
        *complete_text = '\0';

        ptr = text;
        va_start(lst, text);

        while (ptr) 
        {
            strcat(complete_text, ptr);
            ptr = va_arg(lst, char *);
        }

        va_end(lst);
    }

    /* write the string to the error stack */

    if (retcode == ITK_ok) 
    {
        ITK(EMH_store_error_s1(EMH_severity_information,
                                        SBT_ERROR_DETAILS,
                                        complete_text));
    }

    /* free memory */
    if (complete_text)
        MEM_free(complete_text);

    return SBT_ERROR_BASE;

} /* End SBT_EMH_store_error */


/*
 * Reads one entry of the iman env entry specified.
 * @param iman_env_entry_name The iman_env entry to read.
 * @param index               The index of the value to read (starts at 0).
 * @param value               The value read.
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_UTIL_read_iman_env_entry
  (
    const char *iman_env_entry_name,                            /* I */
    int index,                                                  /* I */
    char **value                                                /* OF */
  )
{

    ITK(PREF_initialize());

    if (retcode == ITK_ok) 
    {
        ITK(PREF_set_search_scope(TC_preference_site));
    }

    if (retcode == ITK_ok) 
    {
        ITK(PREF_ask_char_value
                      (
                        iman_env_entry_name,
                        index,
                        value
                      )
                    );
    }

    return retcode;

} /* End SBT_UTIL_read_iman_env_entry */


/*
 * Reads the entries of the iman env entry specified.
 * \param iman_env_entry_name the iman_env entry to read
 * \param count the number of entries found
 * \param values the array of entries found
 */
int SBT_UTIL_read_iman_env_entries
  (
    const char *iman_env_entry_name,                            /* I */
    int *count,                                                 /* O */
    char ***values                                              /* OF */
  )
{

    const char* theFunction = "SBT_Utilities/SBT_UTIL_read_iman_env_entries";
    ENTER(theFunction);    

    ITK(PREF_initialize());

    if (retcode == ITK_ok) 
    {
        ITK(PREF_set_search_scope(TC_preference_site));//replaced IMAN_preference_site with TC_preference_site
    }

    if (retcode == ITK_ok) 
    {
        ITK(PREF_ask_char_values
                      (
                        iman_env_entry_name,
                        count,
                        values
                      )
                    );
    }

    LEAVE(theFunction);    
    return retcode;

} /* End ECCO_KMM_read_iman_env_entries */


/*
 * @brief This function creates a unique file name in the directory given
 *        by the .iman_env variable SBT_tmp_export_dir.
 * @param extension An extension (if desired, otherwise NULL).
 * @param file_path_name the path and name of the unique file created.
 */
int SBT_UTIL_create_unique_filename
  (
    const char *extension,
    char **file_path_name
  )
{
    const char* theFunction = "SBT_Utilities/SBT_UTIL_create_unique_filename";
    TRACE_ON(0);
    ENTER(theFunction);

    ITK(SBT_UTIL_create_unique_filename_by_config(SBT_tmp_export_dir,
                                                           extension,
                                                           file_path_name));

    LEAVE(theFunction);
    return retcode;

} /* End SBT_UTIL_create_unique_filename */


/*
 * @brief This function creates a unique file name in the directory given
 *        by the .iman_env variable specified.
 * @param config    The .iman_env variable where to red the directory from
 *                  where we should create the unique file.
 * @param extension An extension (if desired, otherwise NULL).
 * @param file_path_name the path and name of the unique file created.
 */
int SBT_UTIL_create_unique_filename_by_config
  (
    const char *config,
    const char *extension,
    char **file_path_name
  )
{
    char *file_path = NULL;
    char *ptr =  NULL;
    size_t len;

    char *local_file_path_name = NULL;

    const char* theFunction = "SBT_Utilities/SBT_UTIL_create_unique_filename_by_config";
    TRACE_ON(0);
    ENTER(theFunction);

    /* intialize result parameter */
    retcode = ITK_ok;
    *file_path_name = NULL;


    if (retcode == ITK_ok) {

        ITK(SBT_UTIL_read_iman_env_entry(config,
                                                 0,
                                                 &file_path));
    }

    if (retcode == ITK_ok) {

        if (file_path == NULL) {

            retcode = SBT_IMAN_ENV_ENTRIES_MISSING;

        } else if (file_path[0] == '\0') {

            retcode = SBT_IMAN_ENV_ENTRIES_MISSING;
        }
    }

    if (retcode == SBT_IMAN_ENV_ENTRIES_MISSING) {

        EMH_clear_errors();

        SBT_EMH_store_error("Die .iman_env-Variable ", config, " ist nicht definiert.", NULL);
        SBT_EMH_store_error("Bitte wenden sie sich an einen Systemadministrator.", NULL);
    }

    /* create a unique file name */

    if (retcode == ITK_ok) {

        ptr = (char *)tempnam(file_path, NULL);
    }

    if (retcode == ITK_ok && ptr == NULL) {

        retcode = SS_NOMEM;
    }

    if (retcode == ITK_ok) {

        len = strlen(ptr) + 1;

        if (extension != NULL) {

            len += strlen(extension);
        }

        local_file_path_name = (char *)MEM_alloc((int)len * sizeof(char));
    }

    if (retcode == ITK_ok && local_file_path_name == 0) {

        retcode = SS_NOMEM;
    }

    if (retcode == ITK_ok) {

        if (extension != NULL) {

                sprintf(local_file_path_name, "%s%s", ptr, extension);

        } else {

                sprintf(local_file_path_name, "%s", ptr);
        }
    }

    if (retcode == ITK_ok) {

        *file_path_name = local_file_path_name;
    }

    if (ptr)
        free(ptr);

    if (file_path)
        MEM_free(file_path);

    LEAVE(theFunction);
    return retcode;

} /* End SBT_UTIL_create_unique_filename_by_config */


logical object_is_of_class_or_desc (
	const tag_t		objectTag ,				/* (I) */
	const char		* pcClassName ,			/* (I) */
	tag_t			* pObjectClassIdTag )	/* (O) */
{
	tag_t	classIdTag = NULLTAG		;
	tag_t	objectClassIdTag = NULLTAG	;

	logical		vIsDescendant = FALSE	;


    const char	* theFunction = "object_is_of_class_or_desc"	;
    ENTER(theFunction);


	// Initialize output
	* pObjectClassIdTag = objectClassIdTag ;
	vIsDescendant = FALSE ;


	// Check input integrity
	if ( objectTag == NULLTAG ) goto LEAVE_RETURN ;
	if ( ( pcClassName == NULL ) || ( strlen ( pcClassName ) == 0 ) ) goto LEAVE_RETURN ;


	ITK ( POM_class_id_of_class ( pcClassName , &classIdTag ) ) ;
	if ( ( retcode == ITK_ok ) && ( classIdTag != NULLTAG ) )
	{
		ITK ( POM_class_of_instance ( objectTag , &objectClassIdTag ) ) ;
		if ( ( retcode == ITK_ok ) && ( objectClassIdTag != NULLTAG ) ) 
		{
			ITK ( POM_is_descendant ( classIdTag , objectClassIdTag , &vIsDescendant ) ) ;
		}
	}


	// Set output
	* pObjectClassIdTag = objectClassIdTag ;


	LEAVE_RETURN : ;
    LEAVE(theFunction);
    return vIsDescendant;
}

char* getInternalIdOfTcObject(tag_t tTcObject){
	char *sInternalUID = NULL;

	ITK__convert_tag_to_uid(tTcObject, &sInternalUID);

	char *sReturnValue = (char*) MEM_alloc(strlen(sInternalUID) + 1);  
	strcpy(sReturnValue, sInternalUID);
	MEM_free(sInternalUID);

	return sReturnValue;
}


int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount) {
	int		iRetValue			= ITK_ok;
	tag_t	*taStatus			= NULL;
	date_t	dateReleased;
	date_t	dateReleasedTmp;

	// Get latest release status
	ITK(WSOM_ask_release_status_list(tObject, iCompleteStatusCount, &taStatus));

	if((*iCompleteStatusCount) > 0)
	{
		// Get the last status
		ITK(AOM_ask_value_date(taStatus[0], "date_released", &dateReleasedTmp) );
		ITK(AOM_UIF_ask_value(taStatus[0], "name", sLatestStatusName) );

		for ( int iIdx1 = 0; iIdx1 < (*iCompleteStatusCount); iIdx1++ )
		{
			ITK(AOM_ask_value_date(taStatus[iIdx1], "date_released", &dateReleased) );

			int iDateCompareResult = CLOCK_compare_dates(&dateReleased, &dateReleasedTmp);

			if ( iDateCompareResult > 0 )
			{
				// dateReleased newer than dateReleasedTmp
				ITK(AOM_ask_value_date(taStatus[iIdx1], "date_released", &dateReleasedTmp) );

				// get status name
				if((*sLatestStatusName) != NULL)MEM_free(*sLatestStatusName);
				ITK(AOM_UIF_ask_value(taStatus[iIdx1], "name", sLatestStatusName) );
			} else if( iDateCompareResult == 0) {
				// Equal date -> compare status string
				char *sTmpStatusName = NULL;

				// Read tmp-status name
				ITK(AOM_UIF_ask_value(taStatus[iIdx1], "name", &sTmpStatusName) );

				// Normalize Status names
				std::string strLastestStatusName = *sLatestStatusName;
				std::string strTmpStatusName = sTmpStatusName;

				if(strLastestStatusName.length() == 1){
					char sTmp[2];
					strcpy(sTmp, strLastestStatusName.c_str());
					strLastestStatusName.clear();
					strLastestStatusName.append("0");
					strLastestStatusName.append(sTmp);
				}
				if(strTmpStatusName.length() == 1){
					char sTmp[2];
					strcpy(sTmp, strTmpStatusName.c_str());
					strTmpStatusName.clear();
					strTmpStatusName.append("0");
					strTmpStatusName.append(sTmp);
				}

				// Compare Status names
				if(strcmp(strTmpStatusName.c_str(), strLastestStatusName.c_str()) > 0)
				{
					// get status name
					if((*sLatestStatusName) != NULL)MEM_free(*sLatestStatusName);
					ITK(AOM_UIF_ask_value(taStatus[iIdx1], "name", sLatestStatusName) );					
				}

				// Free
				if(sTmpStatusName != NULL)MEM_free(sTmpStatusName);
			}
		}
	}

	// Free	
	if(taStatus != NULL)MEM_free(taStatus);

	return iRetValue;
}

int CLOCK_compare_dates(const date_t *date1, const date_t *date2)
{
	if ( date1->year != date2->year )
	{
		return date1->year > date2->year ? 1 : -1;
	}

	if ( date1->month != date2->month )
	{
		return date1->month > date2->month ? 1 : -1;
	}

	if ( date1->day != date2->day )
	{
		return date1->day > date2->day ? 1 : -1;
	}

	if ( date1->hour != date2->hour )
	{
		return date1->hour > date2->hour ? 1 : -1;
	}

	if ( date1->minute != date2->minute )
	{
		return date1->minute > date2->minute ? 1 : -1;
	}

	if ( date1->second != date2->second )
	{
		return date1->second > date2->second ? 1 : -1;
	}

	return 0;
}

int getSecondaryObjects(tag_t tItemRevision, char *sRelationType, tag_t *tRelationType, int *iSecondaryCount, tag_t **taSecondaryObjects)
{
	int iRetVal = ITK_ok;

	ITK(GRM_find_relation_type(sRelationType, tRelationType));
	ITK(GRM_list_secondary_objects_only(tItemRevision, *tRelationType, iSecondaryCount, taSecondaryObjects));

	return iRetVal;
}

int getCurrentDateTime(char sDateTime[20])
{
	// Get Date / Time
	time_t     theTime = time(NULL);
	struct tm *aTime   = localtime(&theTime);
	int        day     = aTime->tm_mday;
	int        month   = aTime->tm_mon + 1;             // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
	int        year    = aTime->tm_year + 1900;         // Year is # years since 1900
	int        hour    = aTime->tm_hour;
	int        min     = aTime->tm_min;
	int        sec     = aTime->tm_sec;

	std::stringstream ssDateTime;

	ssDateTime << year << "/" << ( ( month < 10 ) ? "0" : "" ) << month << "/" << ( ( day < 10 ) ? "0" : "" ) << day << "-" << ( ( hour < 10 ) ? "0" : "" ) << hour << ":" << ( ( min < 10 ) ? "0" : "" ) << min << ":" << ( ( sec < 10 ) ? "0" : "" ) << sec;

	strcpy(sDateTime, ssDateTime.str().c_str());

	return ITK_ok;
}

int deleteRelation(tag_t tPrimary, tag_t tSecondary, tag_t tRelationType)
{
	tag_t tRelation = NULLTAG;

	// Set temporary bypass to TRUE
	logical lHasBypass = false;
	ITK(ITK_ask_bypass(&lHasBypass) );
	AM__set_application_bypass(true);

	// Delete Relation
	ITK(GRM_find_relation(tPrimary, tSecondary, tRelationType, &tRelation));
	if(tRelation != NULLTAG)
	{
		ITK(GRM_delete_relation(tRelation));
	}

	// Reset Bypass
	AM__set_application_bypass(lHasBypass);

	return ITK_ok;
}

int createRelation(tag_t tPrimaryObject, tag_t tSecondaryObject, tag_t tRelationType)
{
	tag_t tNewRelation = NULLTAG;

	// Relation already exists ?
	ITK(GRM_find_relation(tPrimaryObject, tSecondaryObject, tRelationType, &tNewRelation));

	if(tNewRelation == NULLTAG)
	{
		// Set temporary bypass to TRUE
		logical lHasBypass = false;
		ITK(ITK_ask_bypass(&lHasBypass) );
		AM__set_application_bypass(true);

		// Create Relation
		ITK(GRM_create_relation(tPrimaryObject, tSecondaryObject, tRelationType, NULLTAG, &tNewRelation));
		ITK(AOM_save(tNewRelation));

		// Reset Bypass
		AM__set_application_bypass(lHasBypass);
	}

	return ITK_ok;
}

int returnStatusRealName(char *sStatusName, char **sRealStatusName)
{
	
	if ( (strcmp(sStatusName, "Locked") == 0) || (strcmp(sStatusName, "5") == 0))
	{
			*sRealStatusName="5";
	}
	else if ( (strcmp(sStatusName, "Pre Release") == 0) || (strcmp(sStatusName, "10") == 0))
	{
			*sRealStatusName="10";
	}
	else if ( (strcmp(sStatusName, "Serial Release") == 0) || (strcmp(sStatusName, "60") == 0))
	{
			*sRealStatusName="60";
	}
	else if ( (strcmp(sStatusName, "Obsolete") == 0) || (strcmp(sStatusName, "90") == 0))
	{
			*sRealStatusName="90";
	}
	else if ( (strcmp(sStatusName, "Pre Release+") == 0) || (strcmp(sStatusName, "BT7_ECO") == 0))
	{
			*sRealStatusName="BT7_ECO";
	}	
	else if ( (strcmp(sStatusName, "Annulled") == 0) || (strcmp(sStatusName, "98") == 0))
	{
			*sRealStatusName="98";
	}
	else if ( (strcmp(sStatusName, "Implemented") == 0) || (strcmp(sStatusName, "BT7_Implemented") == 0))
	{
			*sRealStatusName="BT7_Implemented";
	}
	else if ( (strcmp(sStatusName, "Abgelehnt") == 0) || (strcmp(sStatusName, "Abgelehnt") == 0))
	{
			*sRealStatusName="Abgelehnt";
	}
	else if( (strcmp(sStatusName, "Freigegeben") == 0) || (strcmp(sStatusName, "Freigegeben") == 0))
	{
			*sRealStatusName="Freigegeben";
	}
	else if( (strcmp(sStatusName, "TCM Released") == 0))
	{
			*sRealStatusName="TCM Released";
	}
	else if( (strcmp(sStatusName, "Rejected") == 0))
	{
			*sRealStatusName="Rejected";
	}

	return ITK_ok;
}

logical hasStatus(tag_t tTcObject, const char *sStatus)
{
	tag_t * taStatusList = NULL;
	int     iStatusCount = 0;
	logical lHasStatus = false;

	// Is Status 98 ?
	ITK(WSOM_ask_release_status_list(tTcObject, &iStatusCount, &taStatusList) );

	for ( int iIdx1 = 0; iIdx1 < iStatusCount; iIdx1++ )
	{
		char *sStatusName = NULL;

		ITK(AOM_UIF_ask_value(taStatusList[iIdx1], "name", &sStatusName) );

		if ( strcmp(sStatusName, sStatus) == 0 )
		{
			lHasStatus = true;

			// Free
			SAFE_SM_FREE(sStatusName);

			break;
		}

		// Free
		SAFE_SM_FREE(sStatusName);
	}

	// Free
	SAFE_SM_FREE(taStatusList);

	return lHasStatus;
}

logical isProgrammingData(tag_t tItemRevision)
{
	char	sObjectType[WSO_name_size_c+1];
	logical lIsBT7_Document				= false;
	logical lIsSourcingAndManufacturing	= false;

	// Get Object Type
	ITK(WSOM_ask_object_type(tItemRevision, sObjectType));

	// Is BT7_Document
	lIsBT7_Document = strcmp(sObjectType, "BT7_DocumentRevision") == 0;

	if(lIsBT7_Document == true)
	{
		char *sEntDocType = NULL;

		// Ask TcEnt-DocType
		ITK(AOM_ask_value_string(tItemRevision, "bt7_documentClass", &sEntDocType)); // will be in R1.4.2

		// Is DocType = "Sourcing and Manufacturing"
		lIsSourcingAndManufacturing = strcmp(sEntDocType, "dclass_manu") == 0; // will be in R1.4.2

		// Free
		SAFE_SM_FREE(sEntDocType);
	}

	return lIsBT7_Document && lIsSourcingAndManufacturing;
}

logical isSoftware(tag_t tItemRevision)
{    
	char	sObjectType[WSO_name_size_c+1];

	// Get Object Type
	ITK(WSOM_ask_object_type(tItemRevision, sObjectType));

	return strcmp(sObjectType, "BT7_SoftwareRevision") == 0;
}

/* create dataset not using default tool */
tag_t createDataset(tag_t task, tag_t txtFile, const char *name)
{
	int    retcode       = ITK_ok;
	tag_t  ds_type       = NULLTAG;
	tag_t  tool          = NULLTAG;
	tag_t  relation_type = NULLTAG;
	tag_t  relation      = NULLTAG;
	tag_t  newDataset    = NULLTAG;
	tag_t  rootTask      = NULLTAG;
	char   dsName[100];
	char   txtName[104];
	std::string strRep 	 = "";

	tag_t *attachmentObjects = (tag_t *) MEM_alloc(sizeof( tag_t ) );
	int *  attachmentTypes   = (int *) MEM_alloc(sizeof( int ) );

	if ( task != NULLTAG )
	{
		ITK(AE_find_datasettype("Text", &ds_type) );
		ITK(AE_find_tool("TextEditor", &tool) );

		if ( tool == NULLTAG )
		{
			return NULLTAG;
		}

		// define dataset name
		sprintf(dsName, "%s", name);

		strRep = name;
		std::replace(strRep.begin(), strRep.end(), '\\', '_');
		std::replace(strRep.begin(), strRep.end(), '/', '_');
		std::replace(strRep.begin(), strRep.end(), ':', '_');
		std::replace(strRep.begin(), strRep.end(), '*', '_');
		std::replace(strRep.begin(), strRep.end(), '?', '_');
		std::replace(strRep.begin(), strRep.end(), '\"', '_');
		std::replace(strRep.begin(), strRep.end(), '"', '_');
		std::replace(strRep.begin(), strRep.end(), '<', '_');
		std::replace(strRep.begin(), strRep.end(), '>', '_');
		std::replace(strRep.begin(), strRep.end(), '|', '_');

		sprintf(txtName, "%s.txt", strRep.c_str());

		//dsName = name;

		//create empty dataset
		ITK(AE_create_dataset_with_id(ds_type, dsName, dsName, "ds_id", "A", &newDataset) );
		ITK(AE_set_dataset_tool(newDataset, tool) );
		ITK(AE_set_dataset_format(newDataset, "txt") );
		ITK(AOM_save(newDataset) );

		// Attach the Text Dataset to the root task
		attachmentObjects[0] = newDataset;
		attachmentTypes[0]   = EPM_reference_attachment;

		ITK(EPM_ask_root_task(task, &rootTask) );

		//ITK(AOM_refresh(rootTask, true) );
		ITK(EPM_add_attachments(rootTask, 1, attachmentObjects, attachmentTypes) );
		ITK(AOM_lock(rootTask));
		ITK(AOM_save(rootTask) );

		//ITK(AOM_refresh(rootTask, false) );

		MEM_free(attachmentObjects);
		MEM_free(attachmentTypes);

		//import file
		if ( ( txtFile != NULLTAG ) )
		{
			/* add ImanFile to dataset */
			ITK(AOM_refresh(newDataset, TRUE) );
			ITK(AOM_lock(newDataset) );

			//ITK ( AOM_save ( xmlFile ) );
			ITK(AOM_refresh(txtFile, TRUE) );
			ITK(IMF_set_original_file_name(txtFile, txtName) );
			ITK(AOM_save(txtFile) );
			ITK(AOM_refresh(txtFile, FALSE) );

			ITK(AE_add_dataset_named_ref(newDataset, "Text", AE_PART_OF, txtFile) );
			ITK(AE_save_myself(newDataset) );
			ITK(AOM_unlock(newDataset) );
			ITK(AOM_unload(txtFile) );
			ITK(AOM_unload(newDataset) );
		}
	}

	return newDataset;
}

void copyDatasets( tag_t tItemRevWithSuffix, tag_t tItemRevWithoutSuffix ) {
	std::vector<char*> vecRelationTypes_CopyDatasets;
	vecRelationTypes_CopyDatasets.push_back("IMAN_specification");
	vecRelationTypes_CopyDatasets.push_back("IMAN_manifestation");
	vecRelationTypes_CopyDatasets.push_back("IMAN_reference");
	vecRelationTypes_CopyDatasets.push_back("BT7_Rel_Notice");
	vecRelationTypes_CopyDatasets.push_back("TC_Attaches");

	// Traverse Relation Types
	for(int iIdx1 = 0; iIdx1 < vecRelationTypes_CopyDatasets.size(); iIdx1++) {
		int		iSecondaryCount		= 0;
		tag_t	*taSecondaryObjects	= NULL;
		tag_t	tRelationType		= NULLTAG;

		int		iSecondaryCount_without_suffix		= 0;
		tag_t	*taSecondaryObjects_without_suffix	= NULL;
		tag_t	tRelationType_without_suffix		= NULLTAG;
		std::vector<tag_t> vecStatusAssignmentObjects;
		
		ITK( GRM_find_relation_type( vecRelationTypes_CopyDatasets.at( iIdx1 ), &tRelationType ) );
		
		// Get Secondary Objects for IR with Suffix
		ITK( GRM_list_secondary_objects_only( tItemRevWithSuffix, tRelationType, &iSecondaryCount, &taSecondaryObjects ) );

		// Get Secondary Objects for IR without Suffix
		ITK( GRM_list_secondary_objects_only( tItemRevWithoutSuffix, tRelationType, &iSecondaryCount_without_suffix, &taSecondaryObjects_without_suffix ) );

		// Traverse Secondary Objects of IR with Suffix
		for( int iIdx2 = 0; iIdx2 < iSecondaryCount; iIdx2++ ) {
			tag_t	tDatasetWithoutSuffix		= NULLTAG;
			char*	sDatasetObjectType;
			char*	sDatasetObjectName;

			// Get Dataset Information
			ITK( WSOM_ask_object_type2( taSecondaryObjects[ iIdx2 ], &sDatasetObjectType ) );
			ITK( WSOM_ask_name2( taSecondaryObjects[ iIdx2 ], &sDatasetObjectName ) );

			tag_t	tRelationWithoutSuffix	= NULLTAG;

			if( isDerivedFromClass( "Dataset", taSecondaryObjects[ iIdx2 ] ) == true ) {
						
				// Copy Dataset
				ITK( AE_copy_dataset_with_id( taSecondaryObjects[ iIdx2 ], sDatasetObjectName, "", "", &tDatasetWithoutSuffix ) );
				ITK( AE_save_myself( tDatasetWithoutSuffix ) );

				// Copy Creation Date
				date_t creationDateOfDatasetWithSuffix;
				ITK( AOM_ask_value_date( taSecondaryObjects[ iIdx2 ], "creation_date", &creationDateOfDatasetWithSuffix ) );
				prepareForModify( tDatasetWithoutSuffix );
				ITK( POM_set_creation_date( tDatasetWithoutSuffix, creationDateOfDatasetWithSuffix ) );
				ITK( AOM_save( tDatasetWithoutSuffix ) );

				// Create Relation
				ITK( GRM_create_relation( tItemRevWithoutSuffix, tDatasetWithoutSuffix, tRelationType, NULLTAG, &tRelationWithoutSuffix ) );
				ITK( GRM_save_relation( tRelationWithoutSuffix ) );

				/* Add Status List to new target Dataset */
				tag_t * datasetStatusList = NULL;
				int datasetStatusCount;

				ITK( WSOM_ask_release_status_list( taSecondaryObjects[iIdx2], &datasetStatusCount, &datasetStatusList ) );
				for( int iIdx3 = 0; iIdx3 < datasetStatusCount; iIdx3++ ) {
					tag_t * releaseObjects = ( tag_t * ) MEM_alloc( 1 * sizeof( tag_t ) );
					releaseObjects[ 0 ] = tDatasetWithoutSuffix;
					ITK( RELSTAT_add_release_status( datasetStatusList[ iIdx3 ], 1, releaseObjects, false ) );
					SAFE_SM_FREE( releaseObjects );
				}
				SAFE_SM_FREE( datasetStatusList );

			} else if(isDerivedFromClass("ItemRevision", taSecondaryObjects[iIdx2]) == true) {
				// Create Relation
				ITK( GRM_create_relation( tItemRevWithoutSuffix, taSecondaryObjects[iIdx2], tRelationType, NULLTAG, &tRelationWithoutSuffix ) );
				ITK( GRM_save_relation( tRelationWithoutSuffix ) );
			}

			SAFE_SM_FREE( sDatasetObjectName );
			SAFE_SM_FREE( sDatasetObjectType );

			// Sync Named References if suffix and non-suffix dataset exists
			if(tDatasetWithoutSuffix != NULLTAG && isDerivedFromClass("Dataset", tDatasetWithoutSuffix) == true && isDerivedFromClass("Dataset", taSecondaryObjects[iIdx2]) == true) {
				int		iNRsWithSuffixCount = 0;
				tag_t	*taNRsWithSuffix	= NULL;

				// Get Named Ref Count of Suffix-Object
				ITK(AE_ask_dataset_ref_count(taSecondaryObjects[iIdx2], &iNRsWithSuffixCount));

				// Traverse Suffix-NRs
				for(int iIdx3 = 0; iIdx3 < iNRsWithSuffixCount; iIdx3++) {
					char					reference_name[AE_reference_size_c + 1];
					AE_reference_type_t		reference_type1;
					AE_reference_type_t		reference_type2;
					tag_t					tReferenceObjectWithSuffix		= NULLTAG;
					tag_t					tReferenceObjectWithoutSuffix	= NULLTAG;

					// Find Named Reference
					ITK(AE_find_dataset_named_ref(taSecondaryObjects[iIdx2], iIdx3, reference_name, &reference_type1, &tReferenceObjectWithSuffix));

					// Reference exists in Non-Suffix object
					ITK(AE_ask_dataset_named_ref(tDatasetWithoutSuffix, reference_name, &reference_type2, &tReferenceObjectWithoutSuffix));

					if(tReferenceObjectWithoutSuffix == NULLTAG) {
						ITK(AOM_load(tReferenceObjectWithSuffix));
						ITK(AOM_load(tDatasetWithoutSuffix));
						ITK(AOM_refresh(tDatasetWithoutSuffix, true));
						ITK(AE_add_dataset_named_ref(tDatasetWithoutSuffix, reference_name, reference_type1, tReferenceObjectWithSuffix));
						ITK(AOM_save(tDatasetWithoutSuffix));
						ITK(AOM_refresh(tDatasetWithoutSuffix, false));
						ITK(AOM_unload(tReferenceObjectWithSuffix)); 
						ITK(AOM_unload(tDatasetWithoutSuffix));
					}
				}
				
				int iCompleteStatusCount = 0;
				tag_t* taStatus = NULL;
				logical hasStatus98 = false;
				char* sStatusName = "";

				ITK( WSOM_ask_release_status_list( taSecondaryObjects[iIdx2], &iCompleteStatusCount, &taStatus ) );
				for( int statusIdx = 0; statusIdx < iCompleteStatusCount; statusIdx++ ) {
					ITK( AOM_UIF_ask_value( taStatus[statusIdx], "name", &sStatusName ) );
					if( strcmp( sStatusName, STATUS_TYPE_98 ) == 0 ) {
						hasStatus98 = true;
						break;
					}
				}
				if( ! hasStatus98 ) {
					vecStatusAssignmentObjects.push_back( taSecondaryObjects[iIdx2] );
				}

				// Free
				SAFE_SM_FREE( taNRsWithSuffix );
				SAFE_SM_FREE( taStatus );
				SAFE_SM_FREE( sStatusName );
			}
		}

		tag_t* taReleaseObjects = (tag_t*) MEM_alloc( ( vecStatusAssignmentObjects.size( ) ) * sizeof( tag_t ) );

		for( int iIdx2 = 0; iIdx2 < vecStatusAssignmentObjects.size(); iIdx2++ ) {
			taReleaseObjects[ iIdx2 ] = vecStatusAssignmentObjects.at( iIdx2 );
		}
		
		/* Add Annulling Status to Source Datasets */
		tag_t annulledStatus = NULLTAG;
		ITK( RELSTAT_create_release_status( STATUS_TYPE_98, &annulledStatus ) );
		ITK( RELSTAT_add_release_status( annulledStatus, vecStatusAssignmentObjects.size( ), taReleaseObjects, true ) );
		
		// Free
		SAFE_SM_FREE( taSecondaryObjects );
		SAFE_SM_FREE( taSecondaryObjects_without_suffix );
	}
}

logical isDerivedFromClass(char *className, tag_t tObjectTag) {
	tag_t	tClassIdTag				= NULLTAG;
	tag_t	tSecondaryClassIdTag	= NULLTAG;
	logical	bIsDerived				= false;

	ITK(POM_class_id_of_class(className, &tClassIdTag));
	if(tClassIdTag != NULLTAG){
		ITK(POM_class_of_instance(tObjectTag, &tSecondaryClassIdTag));
		if(tSecondaryClassIdTag != NULLTAG){
			ITK(POM_is_descendant(tClassIdTag, tSecondaryClassIdTag, &bIsDerived));
		}
	}

	return bIsDerived;
}

int prepareForModify(tag_t tObject)
{
	tag_t	tClassIdTag					= NULLTAG;
	int		iInstanceState				= POM_inst_is_not_loaded;
	tag_t	*taObjects					= (tag_t*) MEM_alloc(1 * sizeof(tag_t));

	taObjects[0] = tObject;

	// Get Class ID
	ITK(POM_class_id_of_class("WorkspaceObject", &tClassIdTag));

	// Ask state
	ITK(POM_ask_instance_state(tObject, &iInstanceState));
	if(iInstanceState != POM_inst_is_loaded_for_modify)
	{
		logical lIsLoaded = false;

		// Is already loaded ?
		ITK(POM_is_loaded(tObject, &lIsLoaded));

		if(lIsLoaded == true)
		{
			// Unload
			ITK(POM_unload_instances(1, taObjects));
		}

		// Load instance
		ITK(POM_load_instances(1, taObjects, tClassIdTag, POM_modify_lock));
	}

	// Free
	if(taObjects != NULL) MEM_free(taObjects);

	return ITK_ok;
}

int replace_bom_child(tag_t tParentRevision, tag_t revRuleTag, tag_t oldChildRev, tag_t newChildRev, tag_t bomView) {
	tag_t windowTag = NULLTAG;
	tag_t topLineTag = NULLTAG;
	int childRevCount = 0;
	tag_t *childrenTags = NULL;

	ITK(BOM_create_window(&windowTag));
	ITK(BOM_set_window_config_rule(windowTag , revRuleTag));
	ITK(BOM_set_window_top_line(windowTag , NULLTAG , tParentRevision , NULLTAG , &topLineTag));
	ITK(BOM_set_window_pack_all(windowTag , true));
	ITK(BOM_line_ask_child_lines(topLineTag, &childRevCount, &childrenTags));

	// Traverse BOM of Parent
	for(int iIdx1 = 0 ; iIdx1 < childRevCount; iIdx1++)
	{
		int iBomLineItemRevAttribute = 0;
        tag_t tChildRevTag1 = NULLTAG;
        char *object_type;

        // Get the corresponding ItemRevision
        ITK( BOM_line_look_up_attribute(bomAttr_lineItemRevTag , &iBomLineItemRevAttribute ) );
        ITK( BOM_line_ask_attribute_tag( childrenTags[ iIdx1 ], iBomLineItemRevAttribute , &tChildRevTag1 ) );

            if( tChildRevTag1 != NULLTAG && tChildRevTag1 == oldChildRev ) {
                // Replace BOM-Line
                ITK(BOM_line_replace(childrenTags[ iIdx1 ], NULLTAG, newChildRev, bomView));
            }
      }

      // Save
      ITK( BOM_save_window( windowTag ) );
      ITK( BOM_close_window( windowTag ) );

	  return ITK_ok;
}

/* End file SBT_Utilities.c */
