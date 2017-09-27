#include "BT7_ServerExtensions.h"

/**
    Create a new GRM relation of the given type,
    linking the specified primary and secondary objects.

    @param tag_t  primary_object
    @param tag_t  secondary_object
    @param tag_t  relation_type
    @param tag_t  user_data
    @param tag_t* new_relation
*/
extern USER_EXT_DLL_API int BT7_CMHasSolutionItemPostAction(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;
	tag_t	tPrimaryTag		= va_arg(args, tag_t);
    tag_t	tSecondaryTag	= va_arg(args, tag_t);
    tag_t	tRelTypeTag		= va_arg(args, tag_t);
    tag_t	tUserDataTag	= va_arg(args, tag_t);
    tag_t	*tNewRelation	= va_arg(args, tag_t*);

	tag_t	tECOItem        = NULLTAG;
	char	sECOID[ITEM_id_size_c + 1];
	char    sObjectType[WSO_name_size_c + 1];
	std::string strObjectType;

	if(DEBUG == 1) printf(" > \n");
	if(DEBUG == 1) printf(" > BT7_CMHasSolutionItemPostAction \n");
	if(DEBUG == 1) printf(" > \n");
	
	// Set temporary bypass to TRUE
	logical lHasBypass = false;
	ITK_CALL(ITK_ask_bypass(&lHasBypass) );
	if(DEBUG == 1) printf("Has bypass ? %i \n", lHasBypass);
	AM__set_application_bypass(true);


	// Get ECO-ID
	ITK_CALL(ITEM_ask_item_of_rev(tPrimaryTag, &tECOItem) );
	ITK_CALL(ITEM_ask_id(tECOItem, sECOID) );


	// Write ECO-ID into the Solution Item (if attribute is empty)
	ITK_CALL(WSOM_ask_object_type(tSecondaryTag, sObjectType) );
	strObjectType = sObjectType;

	//if ( isDerivedFromClass("Part_0_Revision_alt", tSecondaryTag) || strcmp("BT7_DocumentRevision", sObjectType) == 0 )
	/*Fix for 0SBT-DO-01PDM4BT|SBTDOP-67 ECO Nr. on BT7_ProdTypeKey:: 
		Solution: Modified check for object_Type as BT7_DocumentRevision to isDerived from Class, to allow BT7_ProdTypeKeyRevision too*/
	if ( isDerivedFromClass("Part_0_Revision_alt", tSecondaryTag) || isDerivedFromClass(BT7_DOCUMENTREVISION, tSecondaryTag))
	{
		char *sAttrName = "bt7_docmnumber";
		char *sValue    = NULL;

		// Write ECO-ID into the attribute if empty or has the default value "No ECO"
		ITK_CALL(AOM_ask_value_string(tSecondaryTag, sAttrName, &sValue) );


		if ( strcmp(sValue, "") == 0 || strcmp(sValue, "No ECO") == 0 )
		{
			// Check-in
			logical lisCheckedOut	= false;
			tag_t	tCurrentUser	= NULLTAG;
			tag_t	tCheckoutUser	= NULLTAG;

			// Is checked out
			ITK_CALL(RES_is_checked_out(tSecondaryTag, &lisCheckedOut));

			if(lisCheckedOut == true)
			{
				tag_t	tCurrentGroupmember	= NULLTAG;
				tag_t	tReservationObject	= NULLTAG;

				// Ask current user
				ITK_CALL(SA_ask_current_groupmember(&tCurrentGroupmember));
				ITK_CALL(SA_ask_groupmember_user(tCurrentGroupmember, &tCurrentUser));
				
				ITK_CALL(RES_ask_reservation_object(tSecondaryTag, &tReservationObject));
				ITK_CALL(RES_ask_user(tReservationObject, &tCheckoutUser));
			}

			// Set Value
			if(lisCheckedOut == false || tCheckoutUser == tCurrentUser)
			{
				// Set Value
				ITK_CALL(AOM_refresh(tSecondaryTag, true) );
				ITK_CALL(AOM_set_value_string(tSecondaryTag, sAttrName, sECOID) );
				ITK_CALL(AOM_save(tSecondaryTag) );
				ITK_CALL(AOM_refresh(tSecondaryTag, false) );
			} else {
				// TODO - how to handle this error?
				printf(" > ERROR: The ECO-ID can not be set on the Solution Item because the object is checked out by another user \n");
			}
		} else {
			// Do not overwrite existing values - TODO: CORRECT?
		}

		// Free
		SAFE_SM_FREE(sValue);
	} else {
		printf(" > %s -> something else! \n", sObjectType);
	}

	// Reset Bypass
	AM__set_application_bypass(lHasBypass);

	return iStatus;	
}