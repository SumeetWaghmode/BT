#include "BT7_ServerExtensions.h"

int prepareForModify(tag_t tObject)
{
	tag_t	tClassIdTag					= NULLTAG;
	int		iInstanceState				= POM_inst_is_not_loaded;
	tag_t	*taObjects					= (tag_t*) MEM_alloc(1 * sizeof(tag_t));

	taObjects[0] = tObject;

	// Get Class ID
	ITK_CALL(POM_class_id_of_class("WorkspaceObject", &tClassIdTag));

	// Ask state
	ITK_CALL(POM_ask_instance_state(tObject, &iInstanceState));
	if(iInstanceState != POM_inst_is_loaded_for_modify)
	{
		logical lIsLoaded = false;

		// Is already loaded ?
		ITK_CALL(POM_is_loaded(tObject, &lIsLoaded));

		if(lIsLoaded == true)
		{
			// Unload
			ITK_CALL(POM_unload_instances(1, taObjects));
		}

		// Load instance
		ITK_CALL(POM_load_instances(1, taObjects, tClassIdTag, POM_modify_lock));
	}

	// Free
	if(taObjects != NULL) MEM_free(taObjects);

	return ITK_ok;
}

int setLogicalPOMAttribute(tag_t tObject, logical lValue, char *sAttributeName)
{
	tag_t	tAttributeIDMigrHelper		= NULLTAG;
	tag_t	*taObjects					= (tag_t*) MEM_alloc(1 * sizeof(tag_t));

	taObjects[0] = tObject;

	// Get Attribute-ID
	ITK_CALL(POM_attr_id_of_attr(sAttributeName, "WorkspaceObject", &tAttributeIDMigrHelper));

	// Prepare for modify
	prepareForModify(tObject);

	// Set Value
	ITK_CALL(POM_set_attr_logical(1, taObjects, tAttributeIDMigrHelper, lValue));

	// Save Object
	ITK_CALL(POM_save_instances(1, taObjects, false));

	// Refresh
	ITK_CALL(POM_refresh_instances(1, taObjects, NULLTAG, POM_modify_lock));

	// Free
	if(taObjects != NULL) MEM_free(taObjects);

	return ITK_ok;
}

/*logical lCurrentUserFromRastatt()
{
	logical	lRastattUser			= false;
	tag_t	tCurrentGroupMember		= NULLTAG;
	tag_t	tCurrentUser			= NULLTAG;
	tag_t	tCurrentPerson			= NULLTAG;
	char	*sPersonCity			= NULL;

	// Get current Person
	ITK_CALL(SA_ask_current_groupmember(&tCurrentGroupMember));
	ITK_CALL(SA_ask_groupmember_user(tCurrentGroupMember, &tCurrentUser));
	ITK_CALL(SA_ask_user_person(tCurrentUser, &tCurrentPerson));
	ITK_CALL(SA_ask_person_attr(tCurrentPerson, "PA2", &sPersonCity));

	// Set return value
	lRastattUser = strcmp(sPersonCity, "Rastatt") == 0;

	// Free
	SAFE_SM_FREE(sPersonCity);

	return lRastattUser;
}*/	

logical lDisableRule( logical lNeedByPass )
{
	tag_t	tCurrentGroupMember		= NULLTAG;
	tag_t	tCurrentGroup			= NULLTAG;

	char*    sGroupName = "";

	logical lHasBypass				= false;
	logical lCurrentUserIsInfodba	= false;

	// Current user is in Group 'dba' or '100_Admin'?
	ITK_CALL( SA_ask_current_groupmember( &tCurrentGroupMember ) );
	ITK_CALL( SA_ask_groupmember_group( tCurrentGroupMember, &tCurrentGroup ) );
	ITK_CALL( SA_ask_group_name2( tCurrentGroup, &sGroupName ) );

	if( strcmp( sGroupName, "dba" ) == 0 || strcmp( sGroupName, "100_Admin" ) == 0 ) {
		lCurrentUserIsInfodba = true;
	}

	SAFE_SM_FREE( sGroupName );

	if( lNeedByPass ) {
		// Current user has bypass?
		ITK_CALL( ITK_ask_bypass( &lHasBypass ) );
	}
	else {
		lHasBypass = true;
	}
	
	return lHasBypass && lCurrentUserIsInfodba;
}

logical isDerivedFromClass(char *className, tag_t tObjectTag){
	tag_t	tClassIdTag				= NULLTAG;
	tag_t	tSecondaryClassIdTag	= NULLTAG;
	logical	bIsDerived				= false;

	ITK_CALL(POM_class_id_of_class(className , &tClassIdTag));
	if(tClassIdTag != NULLTAG){
		ITK_CALL(POM_class_of_instance(tObjectTag, &tSecondaryClassIdTag));
		if(tSecondaryClassIdTag != NULLTAG){
			ITK_CALL(POM_is_descendant(tClassIdTag, tSecondaryClassIdTag, &bIsDerived));
		}
	}

	return bIsDerived;
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

int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount, tag_t *tLatestStatus) {
	int		iRetValue			= ITK_ok;
	tag_t	*taStatus			= NULL;
	date_t	dateReleased;
	date_t	dateReleasedTmp;

	// Get latest release status
	ITK_CALL(WSOM_ask_release_status_list(tObject, iCompleteStatusCount, &taStatus));

	if((*iCompleteStatusCount) > 0)
	{
		// Get the last status
		ITK_CALL(AOM_ask_value_date(taStatus[0], "date_released", &dateReleasedTmp) );
		ITK_CALL(AOM_UIF_ask_value(taStatus[0], "name", sLatestStatusName) );
		*tLatestStatus = taStatus[0];

		for ( int iIdx1 = 0; iIdx1 < (*iCompleteStatusCount); iIdx1++ )
		{
			ITK_CALL(AOM_ask_value_date(taStatus[iIdx1], "date_released", &dateReleased) );

			int iDateCompareResult = CLOCK_compare_dates(&dateReleased, &dateReleasedTmp);

			if ( iDateCompareResult > 0 )
			{
				// dateReleased newer than dateReleasedTmp
				ITK_CALL(AOM_ask_value_date(taStatus[iIdx1], "date_released", &dateReleasedTmp) );

				// get status name
				if((*sLatestStatusName) != NULL)MEM_free(*sLatestStatusName);
				ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", sLatestStatusName) );
				*tLatestStatus = taStatus[iIdx1];
			} else if( iDateCompareResult == 0) {
				// Equal date -> compare status string
				char *sTmpStatusName = NULL;

				// Read tmp-status name
				ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", &sTmpStatusName) );

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
					ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", sLatestStatusName) );
					*tLatestStatus = taStatus[iIdx1];
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

logical isStatusEmptyOr5(tag_t tLatestStatus)
{
	// Is Status Empty ?
	logical lIsStatusEmptyOr5 = (tLatestStatus == NULLTAG);

	// If not empty, is Status 5 ?
	if(lIsStatusEmptyOr5 == false)
	{
		char	*sStatusName	= NULL;

		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );

		// Assign value
		lIsStatusEmptyOr5 = strcmp(sStatusName, STATUS_TYPE_5) == 0;

		// Free
		SAFE_SM_FREE(sStatusName);
	}

	return lIsStatusEmptyOr5;
}

//logical isStatus5(tag_t tLatestStatus)
//{
//	char	*sStatusName	= NULL;
//	logical	lIsStatus5		= false;
//
//	if(isStatusEmpty(tLatestStatus) == false)
//	{
//		// Check the status name and validate
//		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );
//
//		// Assign value
//		lIsStatus5 = strcmp(sStatusName, STATUS_TYPE_5) == 0;
//	}
//
//	// Free
//	SAFE_SM_FREE(sStatusName);
//
//	return lIsStatus5;
//}

logical isStatus10(tag_t tLatestStatus)
{
	char	*sStatusName	= NULL;
	logical	lIsStatus10		= false;

	if(isStatusEmptyOr5(tLatestStatus) == false)
	{
		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );

		// Assign value
		lIsStatus10 = strcmp(sStatusName, STATUS_TYPE_10) == 0;
	}

	// Free
	SAFE_SM_FREE(sStatusName);

	return lIsStatus10;
}

logical isStatus90Plus10or60(tag_t tParentRev, tag_t tChildRev)
{
	logical lIsValid			= false;
	int		iStatusCount		= 0;
	tag_t	*taStatusObjects	= NULL;
	
	//// Is Parent a Software
	//if(isSoftware(tParentRev) == false)
	//{
	//	return false;
	//}

	// Does the child contain the status 10 or 60 (not latest)
	ITK_CALL(WSOM_ask_release_status_list(tChildRev, &iStatusCount, &taStatusObjects));

	// Loop status
	for(int iIdx1 = 0; iIdx1 < iStatusCount; iIdx1++)
	{
		logical lIsStatus10 = isStatus10(taStatusObjects[iIdx1]);
		logical lIsStatus60 = isStatus60(taStatusObjects[iIdx1]);

		if(lIsStatus10 || lIsStatus60)
		{
			lIsValid = true;
			break;
		}
	}

	// Free
	SAFE_SM_FREE(taStatusObjects);

	return lIsValid;
}

logical isStatus60(tag_t tLatestStatus)
{
	char	*sStatusName	= NULL;
	logical	lIsStatus60		= false;

	if(isStatusEmptyOr5(tLatestStatus) == false)
	{
		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );

		// Assign value
		lIsStatus60 = strcmp(sStatusName, STATUS_TYPE_60) == 0;
	}

	// Free
	SAFE_SM_FREE(sStatusName);

	return lIsStatus60;
}

logical isStatusPrereleaseplus(tag_t tLatestStatus)
{

	char	*sStatusName	= NULL;
	logical	lIsStatusPrereleaseplus		= false;

	if(isStatusEmptyOr5(tLatestStatus) == false)
	{
		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );

		// Assign value
		lIsStatusPrereleaseplus = strcmp(sStatusName, STATUS_TYPE_PRERELEASEPLUS) == 0;
	}

	// Free
	SAFE_SM_FREE(sStatusName);

	return lIsStatusPrereleaseplus;

}

logical isProgrammingData(tag_t tItemRevision)
{
	char	sObjectType[WSO_name_size_c+1];
	logical lIsBT7_Document				= false;
	logical lIsSourcingAndManufacturing	= false;

	// Get Object Type
	ITK_CALL(WSOM_ask_object_type(tItemRevision, sObjectType));

	// Is BT7_Document
	lIsBT7_Document = strcmp(sObjectType, "BT7_DocumentRevision") == 0;

	if(lIsBT7_Document == true)
	{
		char *sEntDocType = NULL;

		// Ask TcEnt-DocType
		ITK_CALL(AOM_ask_value_string(tItemRevision, "bt7_documentClass", &sEntDocType)); 

		// Changed value after updation Is DocType = "Manufacturing"
		// Is DocType = "Sourcing and Manufacturing"
		lIsSourcingAndManufacturing = strcmp(sEntDocType, "dclass_manu") == 0;

		// Free
		SAFE_SM_FREE(sEntDocType);
	}

	return lIsBT7_Document && lIsSourcingAndManufacturing;
}

logical isCommonComponent(tag_t tItemRevision)
{    
	char	sObjectType[WSO_name_size_c+1];

	// Get Object Type
	ITK_CALL(WSOM_ask_object_type(tItemRevision, sObjectType));

	return strcmp(sObjectType, "BT7_CommonCompRevision") == 0;
}

logical isSoftware(tag_t tItemRevision)
{    
	char	sObjectType[WSO_name_size_c+1];

	// Get Object Type
	ITK_CALL(WSOM_ask_object_type(tItemRevision, sObjectType));

	return strcmp(sObjectType, "BT7_SoftwareRevision") == 0;
}

logical isStatus90(tag_t tLatestStatus)
{
	char	*sStatusName	= NULL;
	logical	lIsStatus90		= false;

	if(isStatusEmptyOr5(tLatestStatus) == false)
	{
		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );

		// Assign value
		lIsStatus90 = strcmp(sStatusName, STATUS_TYPE_90) == 0;
	}

	// Free
	SAFE_SM_FREE(sStatusName);

	return lIsStatus90;
}

logical isStatus98(tag_t tLatestStatus)
{
	char	*sStatusName	= NULL;
	logical	lIsStatus98		= false;

	if(isStatusEmptyOr5(tLatestStatus) == false)
	{
		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(tLatestStatus, "name", &sStatusName) );

		// Assign value
		lIsStatus98 = strcmp(sStatusName, STATUS_TYPE_98) == 0;
	}

	// Free
	SAFE_SM_FREE(sStatusName);

	return lIsStatus98;
}

int deleteRelation(tag_t tPrimary, tag_t tSecondary, tag_t tRelationType)
{
	tag_t tRelation = NULLTAG;

	// Set temporary bypass to TRUE
	logical lHasBypass = false;
	ITK_CALL(ITK_ask_bypass(&lHasBypass) );
	AM__set_application_bypass(true);

	// Delete Relation
	ITK_CALL(GRM_find_relation(tPrimary, tSecondary, tRelationType, &tRelation));
	if(tRelation != NULLTAG)
	{
		ITK_CALL(GRM_delete_relation(tRelation));
	}

	// Reset Bypass
	AM__set_application_bypass(lHasBypass);

	return ITK_ok;
}

int getNewerRevisions(tag_t tItemRevision, vector<tag_t> *vecNewerRevisions)
{
	int		iRetVal				= ITK_ok;

	tag_t	tTmpItem			= NULLTAG;
	tag_t	*taRevisions		= NULL;
	int		iRevCount			= 0;
	logical	lPassedRevReached	= false;

	// Ask Item
	ITK_CALL(ITEM_ask_item_of_rev(tItemRevision, &tTmpItem));

	// Ask Item Revisions
	ITK_CALL(ITEM_list_all_revs(tTmpItem, &iRevCount, &taRevisions));

	// Loop Revisions
	for(int iIdx1 = 0; iIdx1 < iRevCount; iIdx1++)
	{
		if(lPassedRevReached == true)
		{
			(*vecNewerRevisions).push_back(taRevisions[iIdx1]);
		}

		if(tItemRevision == taRevisions[iIdx1])
		{
			lPassedRevReached = true;
		}
	}

	// Free
	SAFE_SM_FREE(taRevisions);

	return iRetVal;
}

int createRelation(tag_t tPrimaryObject, tag_t tSecondaryObject, tag_t tRelationType)
{
	tag_t tNewRelation = NULLTAG;

	// Relation already exists ?
	ITK_CALL(GRM_find_relation(tPrimaryObject, tSecondaryObject, tRelationType, &tNewRelation));

	if(tNewRelation == NULLTAG)
	{
		// Set temporary bypass to TRUE
		logical lHasBypass = false;
		ITK_CALL(ITK_ask_bypass(&lHasBypass) );
		AM__set_application_bypass(true);

		// Create Relation
		ITK_CALL(GRM_create_relation(tPrimaryObject, tSecondaryObject, tRelationType, NULLTAG, &tNewRelation));
		ITK_CALL(AOM_save(tNewRelation));

		// Reset Bypass
		AM__set_application_bypass(lHasBypass);
	} else {
		// Reset Bypass Attribute on Parent
		setRelationBypassAttribute(tPrimaryObject, 0);
	}

	return ITK_ok;
}

int setRelationBypassAttribute(tag_t tItemRevision, int iValue)
{
	// Set attribute
	if(isDerivedFromClass("ItemRevision", tItemRevision))
	{
		// Set temporary bypass to TRUE
		logical lHasBypass = false;
		ITK_CALL(ITK_ask_bypass(&lHasBypass) );
		AM__set_application_bypass(true);

		// Prepare for Modify
		prepareForModify(tItemRevision);

		// Set Value
		ITK_CALL(AOM_set_value_int(tItemRevision, "bt7_relation_logic_bypass", iValue));

		// Save Object
		ITK_CALL(AOM_save(tItemRevision));

		// Reset Bypass
		AM__set_application_bypass(lHasBypass);
	}

	return ITK_ok;;
}

int askRelationBypassAttribute(tag_t tItemRevision)
{
	int iValue = 0;

	ITK_CALL(AOM_ask_value_int(tItemRevision, "bt7_relation_logic_bypass", &iValue));

	return iValue;
}
logical isStatusEmpty(tag_t tLatestStatus)
{
	// Is Status Empty ?
	logical lIsStatusEmpty = (tLatestStatus == NULLTAG);	

	return lIsStatusEmpty;
}