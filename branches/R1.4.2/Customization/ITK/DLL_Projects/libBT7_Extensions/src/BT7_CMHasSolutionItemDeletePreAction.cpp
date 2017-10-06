#include "BT7_ServerExtensions.h"

/**
    Delete a new GRM relation 

    @param tag_t object
*/
extern USER_EXT_DLL_API int BT7_CMHasSolutionItemDeletePreAction(METHOD_message_t* msg, va_list args) {
	tag_t	tRelationTag		= va_arg(args, tag_t);

	int		iStatus				= ITK_ok;
	tag_t	tECOItem			= NULLTAG;
	tag_t	tECOItemRev			= NULLTAG;
	tag_t	tSolutionItemRev	= NULLTAG;
	char	sECOID[ITEM_id_size_c + 1];
	char    sObjectType[WSO_name_size_c + 1];
	std::string strObjectType;
	char	*sSolutionItemECOID	= NULL;
	tag_t	tSolutionItemECO_Container = NULLTAG;
	tag_t	*taMasterForms      = NULL;
	char	sAttrName[30];

	if(DEBUG == 1) printf(" > \n");
	if(DEBUG == 1) printf(" > BT7_CMHasSolutionItemDeletePreAction \n");
	if(DEBUG == 1) printf(" > \n");

	// Get Primary and Secondary Object
	ITK_CALL(GRM_ask_primary(tRelationTag, &tECOItemRev));
	ITK_CALL(GRM_ask_secondary(tRelationTag, &tSolutionItemRev));

	// Get ECO-ID
	ITK_CALL(ITEM_ask_item_of_rev(tECOItemRev, &tECOItem) );
	ITK_CALL(ITEM_ask_id(tECOItem, sECOID) );

	// Ask ECO-ID in Solution Item
	ITK_CALL(WSOM_ask_object_type(tSolutionItemRev, sObjectType) );
	strObjectType = sObjectType;

	/*Fix for 0SBT-DO-01PDM4BT|SBTDOP-67 ECO Nr. on BT7_ProdTypeKey:: 
	Solution: Modified check for object_Type as BT7_DocumentRevision to isDerived from Class, to allow BT7_ProdTypeKeyRevision too*/
	if ( isDerivedFromClass("Part_0_Revision_alt", tSolutionItemRev) ||  isDerivedFromClass(BT7_DOCUMENTREVISION, tSolutionItemRev) )
	{
		char *sTmpAttrName = "bt7_docmnumber";
		strcpy(sAttrName, sTmpAttrName);

		// Write ECO-ID into the attribute if empty or has the default value "No ECO"
		ITK_CALL(AOM_ask_value_string(tSolutionItemRev, sAttrName, &sSolutionItemECOID) );
		tSolutionItemECO_Container = tSolutionItemRev;
	}

	// Reset ECO-ID if required
	if(sSolutionItemECOID != NULL && strcmp(sSolutionItemECOID, sECOID) == 0)
	{
		// Set temporary bypass to TRUE
		logical lHasBypass = false;
		ITK_CALL(ITK_ask_bypass(&lHasBypass) );
		if(DEBUG == 1) printf("Has bypass ? %i \n", lHasBypass);
		AM__set_application_bypass(true);

		// Reset ECO-ID
		{
			logical lisCheckedOut	= false;
			tag_t	tCurrentUser	= NULLTAG;
			tag_t	tCheckoutUser	= NULLTAG;

			// Is checked out
			ITK_CALL(RES_is_checked_out(tSolutionItemECO_Container, &lisCheckedOut));

			if(lisCheckedOut == true)
			{
				tag_t	tCurrentGroupmember	= NULLTAG;
				tag_t	tReservationObject	= NULLTAG;

				// Ask current user
				ITK_CALL(SA_ask_current_groupmember(&tCurrentGroupmember));
				ITK_CALL(SA_ask_groupmember_user(tCurrentGroupmember, &tCurrentUser));
				
				ITK_CALL(RES_ask_reservation_object(tSolutionItemECO_Container, &tReservationObject));
				ITK_CALL(RES_ask_user(tReservationObject, &tCheckoutUser));
			}

			// Set Value
			if(lisCheckedOut == false || tCheckoutUser == tCurrentUser)
			{
				// Set Value
				ITK_CALL(AOM_refresh(tSolutionItemECO_Container, true) );
				ITK_CALL(AOM_set_value_string(tSolutionItemECO_Container, sAttrName, "") );
				ITK_CALL(AOM_save(tSolutionItemECO_Container) );
				ITK_CALL(AOM_refresh(tSolutionItemECO_Container, false) );
			} else {
				// TODO - how to handle this error?
				printf(" > ERROR: The ECO-ID can not be reset on the Solution Item because the object is checked out by another user \n");
			}
		}


		// Reset Bypass
		AM__set_application_bypass(lHasBypass);
	}

	// Free
	SAFE_SM_FREE(taMasterForms);
	SAFE_SM_FREE(sSolutionItemECOID);

	return iStatus;	
}