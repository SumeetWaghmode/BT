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
extern USER_EXT_DLL_API int BT7_CMHasProblemItemPreCondition(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;
	tag_t	tPrimaryTag		= va_arg(args, tag_t);
    tag_t	tSecondaryTag	= va_arg(args, tag_t);
    tag_t	tRelTypeTag		= va_arg(args, tag_t);
    tag_t	tUserDataTag	= va_arg(args, tag_t);

	// Skip rule ?
	if(lDisableRule())
	{
		return ITK_ok;
	}

	// Verify Primary and Secondary Objects
	logical bIsPrimaryDerivedFromChangeNoticeRevision = isDerivedFromClass("ChangeNoticeRevision", tPrimaryTag);
	logical bIsSecondaryDerivedFromItemRevision = isDerivedFromClass("ItemRevision", tSecondaryTag);

	if(bIsPrimaryDerivedFromChangeNoticeRevision && bIsSecondaryDerivedFromItemRevision){
		// Latest status is 10, 60 or WIP for Products?
		char	*sLatestStatusName	= NULL;
		int		iStatusCount		= 0;
		tag_t	tLatestStatusObject	= NULLTAG;
		char    sObjectType[WSO_name_size_c+1];

		// Ask object type
		ITK_CALL(WSOM_ask_object_type(tSecondaryTag, sObjectType));

		// Ask latest release status
		askLatestReleaseStatus(tSecondaryTag, &sLatestStatusName, &iStatusCount, &tLatestStatusObject);

		// Status OK?
		if (iStatusCount == 0 && (strcmp(sObjectType, "BT7_ProductRevision") == 0)) {
			// OK -> Check if this is the latest status in context of the whole Item
		} else if(iStatusCount > 0 && (strcmp(sLatestStatusName, STATUS_TYPE_10) == 0 || strcmp(sLatestStatusName, STATUS_TYPE_60) == 0)) {
			// OK -> Check if this is the latest status in context of the whole Item
		} else {
			// Error: Must have status 10 or 60 (or WIP for Products)
			iStatus = 919723;
			ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
		}

		// If the status is ok, check if it is the latest one in context of the whole Item
		if(iStatus == ITK_ok) {
			tag_t	tTmpItem					= NULLTAG;
			tag_t	*taTmpRevs					= NULL;
			int		iTmpRevCount				= 0;
			logical	lIsLatestRevOfThisStatus	= false;

			// Get Item
			ITK_CALL(ITEM_ask_item_of_rev(tSecondaryTag, &tTmpItem));

			// Get all Revisions
			ITK_CALL(ITEM_list_all_revs(tTmpItem, &iTmpRevCount, &taTmpRevs));

			// Traverse Revisions backwards
			for(int iIdx1 = iTmpRevCount - 1; iIdx1 >= 0; iIdx1--) {
				char	*sTmpLatestStatusName	= NULL;
				int		iTmpStatusCount			= 0;
				tag_t	tTmpLatestStatusObject	= NULLTAG;
				logical	lBreak					= false;

				// Get latest status
				askLatestReleaseStatus(taTmpRevs[iIdx1], &sTmpLatestStatusName, &iTmpStatusCount, &tTmpLatestStatusObject);

				// Compare Status
				if(iTmpStatusCount == 0) {
					if(iStatusCount == 0) {
						// Compare New Problem Item with the Latest Revision without a status

						// Break
						lBreak = true;

						// Compare Revision
						if(taTmpRevs[iIdx1] == tSecondaryTag) {
							// Same Revision -> OK
							lIsLatestRevOfThisStatus = true;
						}
					}					
				} else {
					if(strcmp(sTmpLatestStatusName, sLatestStatusName) == 0) {
						// Compare New Problem Item with the Latest Revision with the same status

						// Break
						lBreak = true;

						// Compare Revision
						if(taTmpRevs[iIdx1] == tSecondaryTag) {
							// Same Revision -> OK
							lIsLatestRevOfThisStatus = true;
						}
					}
				}

				// Free
				SAFE_SM_FREE(sTmpLatestStatusName);

				// Break ?
				if(lBreak == true) {
					break;
				}
			}


			// Current status is the latest one in context of the whole Item?
			if(lIsLatestRevOfThisStatus == false) {
				if(iStatusCount == 0) {
					iStatus = 919728;
				} else if(iStatusCount > 0 && strcmp(sLatestStatusName, STATUS_TYPE_10) == 0) {
					iStatus = 919726;
				} else if(iStatusCount > 0 && strcmp(sLatestStatusName, STATUS_TYPE_60) == 0) {
					iStatus = 919727;
				} else {
					iStatus = 919723;
				}
				
				// Store Error
				ITK_CALL(EMH_store_error(EMH_severity_error, iStatus));
			}
		}

		// Free
		SAFE_SM_FREE(sLatestStatusName);
	} else {
		// No ItemRevision - will be handled by GRM-Rules
	}

	return iStatus;	
}