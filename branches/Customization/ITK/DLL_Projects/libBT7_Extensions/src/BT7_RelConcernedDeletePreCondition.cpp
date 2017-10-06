#include "BT7_ServerExtensions.h"

/**
    Removes the given object from the database.

    @param tag_t object
*/
extern USER_EXT_DLL_API int BT7_RelConcernedDeletePreCondition(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;
	tag_t	tRelationTag	= va_arg(args, tag_t);

	if(DEBUG == 1) printf(" > \n");
	if(DEBUG == 1) printf(" > BT7_RelConcernedDeletePreCondition \n");
	if(DEBUG == 1) printf(" > \n");

	// Skip rule ?
	if(lDisableRule())
	{
		return ITK_ok;
	}

	// Exit if "Zug-Logic" is not applicable for the current user
	/*if(lCurrentUserFromRastatt())
	{
		return ITK_ok;
	}*/

	// Clear Error Stack
	ITK_CALL(EMH_clear_errors());

	// Default behaviour defined in the BMIDE: No Write Access required to Attach / Detach ItemRevision

	// Get Primary and secondary objects
	tag_t tPrimaryTag	= NULLTAG;
	tag_t tSecondaryTag = NULLTAG;
	ITK_CALL(GRM_ask_primary(tRelationTag, &tPrimaryTag));
	ITK_CALL(GRM_ask_secondary(tRelationTag, &tSecondaryTag));
	
	// Exit if the temporary bypass attribute != 0
	if(askRelationBypassAttribute(tPrimaryTag) != 0)
	{
		return ITK_ok;
	}

	// Only applicable for Pasted ItemRevisions
	if(isDerivedFromClass("ItemRevision", tSecondaryTag))
	{
		// Read Status
		tag_t tPrimaryLatestStatus		= NULLTAG;
		tag_t tSecondaryLatestStatus	= NULLTAG;
		int		iStatusCountPrimary		= 0;
		int		iStatusCountSecondary	= 0;
		char	*sLatestStatusPrimary	= NULL;
		char	*sLatestStatusSecondary	= NULL;

		askLatestReleaseStatus(tPrimaryTag, &sLatestStatusPrimary, &iStatusCountPrimary, &tPrimaryLatestStatus);
		askLatestReleaseStatus(tSecondaryTag, &sLatestStatusSecondary, &iStatusCountSecondary, &tSecondaryLatestStatus);

		if(
			( isStatusEmptyOr5(tPrimaryLatestStatus) && (isStatusEmptyOr5(tSecondaryLatestStatus) || isStatus10(tSecondaryLatestStatus) || isStatus60(tSecondaryLatestStatus) || isStatus90(tSecondaryLatestStatus) || isStatus98(tSecondaryLatestStatus)) )
			|| ( !isSoftware(tPrimaryTag) && isStatus10(tPrimaryLatestStatus) && isStatusEmptyOr5(tSecondaryLatestStatus) )
			|| ( isStatus60(tPrimaryLatestStatus) && isStatusEmptyOr5(tSecondaryLatestStatus) )
			|| ( isStatusEmpty(tPrimaryLatestStatus)&&( isStatusPrereleaseplus(tSecondaryLatestStatus)) )
			|| ( !isSoftware(tPrimaryTag) && isStatusPrereleaseplus(tPrimaryLatestStatus)&&(isStatusEmpty(tSecondaryLatestStatus)))
		  )
		{
			// OK
		} else {
			iStatus = 919722;
			ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
		}

		// Free
		SAFE_SM_FREE(sLatestStatusPrimary);
		SAFE_SM_FREE(sLatestStatusSecondary);
	}

	return iStatus;	
}