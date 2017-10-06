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
extern USER_EXT_DLL_API int BT7_CMHasSolutionItemPreCondition(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;

	//tag_t	tPrimaryTag		= va_arg(args, tag_t);
	//tag_t	tSecondaryTag	= va_arg(args, tag_t);
	//tag_t	tRelTypeTag		= va_arg(args, tag_t);
	//tag_t	tUserDataTag	= va_arg(args, tag_t);

	//// Skip rule ?
	//if(lDisableRule())
	//{
	//	return ITK_ok;
	//}

	//// Verify Primary and Secondary Objects
	//logical bIsPrimaryDerivedFromChangeNoticeRevision = isDerivedFromClass("ChangeNoticeRevision", tPrimaryTag);
	//logical bIsSecondaryDerivedFromItemRevision = isDerivedFromClass("ItemRevision", tSecondaryTag);

	//if(bIsPrimaryDerivedFromChangeNoticeRevision && bIsSecondaryDerivedFromItemRevision){
	//	// Latest status is 10, 60 or 90 ?
	//	char	*sLatestStatusName	= NULL;
	//	int		iStatusCount		= 0;
	//	tag_t	tLatestStatusObject	= NULLTAG;

	//	askLatestReleaseStatus(tSecondaryTag, &sLatestStatusName, &iStatusCount, &tLatestStatusObject);
	//	if(iStatusCount > 0)
	//	{
	//		if(strcmp(sLatestStatusName, STATUS_TYPE_10) == 0 || strcmp(sLatestStatusName, STATUS_TYPE_60) == 0 || strcmp(sLatestStatusName, STATUS_TYPE_90) == 0)
	//		{
	//			// OK
	//		} else {
	//			// Error: Must have status 10, 60 or 90
	//			iStatus = 919721;
	//			ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
	//		}
	//	} else {
	//		// Error: Must have status 10, 60 or 90
	//		iStatus = 919721;
	//		ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
	//	}

	//	// Free
	//	SAFE_SM_FREE(sLatestStatusName);
	//} else {
	//	// Error: No ItemRevision
	//	//iStatus = 919706;
	//	//ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
	//}

	return iStatus;	
}