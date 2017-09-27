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
extern USER_EXT_DLL_API int BT7_RelSpecifiedCreatePreCondition(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;
	tag_t	tPrimaryTag		= va_arg(args, tag_t);
    tag_t	tSecondaryTag	= va_arg(args, tag_t);
    tag_t	tRelTypeTag		= va_arg(args, tag_t);
    tag_t	tUserDataTag	= va_arg(args, tag_t);

	if(DEBUG == 1) printf(" > \n");
	if(DEBUG == 1) printf(" > BT7_RelSpecifiedCreatePreCondition \n");
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

	// Exit if the temporary bypass attribute != 0
	if(askRelationBypassAttribute(tPrimaryTag) != 0)
	{
		return ITK_ok;
	}

	// Clear Error Stack
	ITK_CALL(EMH_clear_errors());

	// Only applicable for Pasted ItemRevisions
	if(isDerivedFromClass("ItemRevision", tSecondaryTag))
	{
		// Default behaviour defined in the BMIDE: Write Access required to Attach / Detach ItemRevision
		
		// Read Status
		tag_t	tPrimaryLatestStatus	= NULLTAG;
		tag_t	tSecondaryLatestStatus	= NULLTAG;
		int		iStatusCountPrimary		= 0;
		int		iStatusCountSecondary	= 0;
		char	*sLatestStatusCountPrimary		= NULL;
		char	*sLatestStatusCountSecondary	= NULL;

		askLatestReleaseStatus(tPrimaryTag, &sLatestStatusCountPrimary, &iStatusCountPrimary, &tPrimaryLatestStatus);
		askLatestReleaseStatus(tSecondaryTag, &sLatestStatusCountSecondary, &iStatusCountSecondary, &tSecondaryLatestStatus);

		 	if(
			(isStatusEmptyOr5(tPrimaryLatestStatus) || (isStatus10(tPrimaryLatestStatus) && isCommonComponent(tPrimaryTag)))
			 &&
			(isStatusEmptyOr5(tSecondaryLatestStatus) || isStatus10(tSecondaryLatestStatus) || isStatusPrereleaseplus(tSecondaryLatestStatus)|| isStatus60(tSecondaryLatestStatus) || (isStatus90(tSecondaryLatestStatus) && isSoftware(tPrimaryTag)))
			
		  )
		{
			// OK
		} else {
			iStatus = 919722;
			ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
		}

		// Free
		SAFE_SM_FREE(sLatestStatusCountPrimary);
		SAFE_SM_FREE(sLatestStatusCountSecondary);
	}

	return iStatus;	
}
