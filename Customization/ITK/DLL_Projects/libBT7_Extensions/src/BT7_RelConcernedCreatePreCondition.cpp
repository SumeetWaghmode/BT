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
extern USER_EXT_DLL_API int BT7_RelConcernedCreatePreCondition(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;
	tag_t	tPrimaryTag		= va_arg(args, tag_t);
    tag_t	tSecondaryTag	= va_arg(args, tag_t);
    tag_t	tRelTypeTag		= va_arg(args, tag_t);
    tag_t	tUserDataTag	= va_arg(args, tag_t);

	if(DEBUG == 1) printf(" > \n");
	if(DEBUG == 1) printf(" > BT7_RelConcernedCreatePreCondition \n");
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

	// Only applicable for Pasted ItemRevisions
	if(isDerivedFromClass("ItemRevision", tSecondaryTag))
	{
		// Default behaviour defined in the BMIDE: No Write Access required to Attach / Detach ItemRevision
		
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
			( isStatusEmptyOr5(tPrimaryLatestStatus) && (isStatusEmptyOr5(tSecondaryLatestStatus) || isStatus10(tSecondaryLatestStatus)|| isStatusPrereleaseplus(tSecondaryLatestStatus)
			|| isStatus60(tSecondaryLatestStatus)) )
			|| ( (isStatus10(tPrimaryLatestStatus)||isStatusPrereleaseplus(tPrimaryLatestStatus)) && isStatusEmptyOr5(tSecondaryLatestStatus) )
			|| ( isStatus60(tPrimaryLatestStatus) && isStatusEmptyOr5(tSecondaryLatestStatus) )
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

	// Issue 1093. CR against BT-Relation: The Concerned-Relation for user from Zug should catch the insertion of items.
	//else if ( lCurrentUserFromRastatt() == false && isDerivedFromClass("Item", tSecondaryTag))
		else if ( isDerivedFromClass("Item", tSecondaryTag))
	{
		// ERROR: Users from Zug shouldn't paste ITEMS (instead of Revisions) into the Concerned-Relation !
		iStatus = 919730;
		ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
	}

	return iStatus;	
}
