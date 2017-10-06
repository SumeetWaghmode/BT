#include "BT7_ServerExtensions.h"

void specifiedLogic_SetReleaseStatus(tag_t tItemRevision, int iRelCountPrimarySpecified, tag_t *taRelPrimarySpecified, tag_t tRelTypeSpecified)
{
	// Loop Primary Specified Objects
	for(int iIdx1 = 0; iIdx1 < iRelCountPrimarySpecified; iIdx1++)
	{
		tag_t	tPrimaryLatestStatus	= NULLTAG;
		int		iPrimaryStatusCount		= 0;
		char	*sPrimaryLatestStatus	= NULL;

		// Get latest primary status
		askLatestReleaseStatus(taRelPrimarySpecified[iIdx1], &sPrimaryLatestStatus, &iPrimaryStatusCount, &tPrimaryLatestStatus);

		// Only delete relations to parents with status WIP, 5, 10 or 60
		if(isStatusEmptyOr5(tPrimaryLatestStatus) || isStatus10(tPrimaryLatestStatus) || isStatus60(tPrimaryLatestStatus)||isStatusPrereleaseplus(tPrimaryLatestStatus))
		{
			// Set temporary bypass attribute on the Item Revision
			setRelationBypassAttribute(taRelPrimarySpecified[iIdx1], 1);

			// Delete Relation
			deleteRelation(taRelPrimarySpecified[iIdx1], tItemRevision, tRelTypeSpecified);

			// Reset temporary bypass attribute on the Item Revision
			setRelationBypassAttribute(taRelPrimarySpecified[iIdx1], 0);
		}
	}
}

void concernedLogic_SetReleaseStatus(tag_t tItemRevision, int iRelCountPrimaryConcerned, tag_t *taRelPrimaryConcerned, tag_t tRelTypeConcerned)
{
	// Loop Primary Concerned Objects
	for(int iIdx1 = 0; iIdx1 < iRelCountPrimaryConcerned; iIdx1++)
	{
		tag_t	tPrimaryLatestStatus	= NULLTAG;
		int		iPrimaryStatusCount		= 0;
		char	*sPrimaryLatestStatus	= NULL;

		// Get latest primary status
		askLatestReleaseStatus(taRelPrimaryConcerned[iIdx1], &sPrimaryLatestStatus, &iPrimaryStatusCount, &tPrimaryLatestStatus);

		// Only delete relations to parents with status WIP, 5, 10 or 60
		if(isStatusEmptyOr5(tPrimaryLatestStatus) || isStatus10(tPrimaryLatestStatus) || isStatus60(tPrimaryLatestStatus)||isStatusPrereleaseplus(tPrimaryLatestStatus))
		{
			// Set temporary bypass attribute on the Item Revision
			setRelationBypassAttribute(taRelPrimaryConcerned[iIdx1], 1);

			// Delete Relation
			deleteRelation(taRelPrimaryConcerned[iIdx1], tItemRevision, tRelTypeConcerned);

			// Reset temporary bypass attribute on the Item Revision
			setRelationBypassAttribute(taRelPrimaryConcerned[iIdx1], 0);
		}
	}
}

extern USER_EXT_DLL_API int BT7_SetReleaseStatusListPostAction(METHOD_message_t* msg, va_list args) {
	int		iStatus				= ITK_ok;
	tag_t	tProperty			= va_arg(args, tag_t);
	int		iValue				= va_arg(args, int);
	tag_t	*taValue			= va_arg(args, tag_t*);

	char	*sLatestStatusName	= NULL;
	tag_t	tItemRevision		= NULLTAG;

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_SetReleaseStatusListPostAction \n");
	if(DEBUG == 1) printf("################################################################ \n");

	// Ask Item Revision
	ITK_CALL(PROP_ask_owning_object(tProperty, &tItemRevision));

	// Read Status Name
	if(iValue > 0)
	{
		ITK_CALL(AOM_UIF_ask_value(taValue[iValue - 1], "name", &sLatestStatusName) );
		//printf(" > sLatestStatusName = %s \n", sLatestStatusName);

		// Cleanup Specifies / Concerns Relations if the Revision has WIP -> 90 or 5 -> 90
		if(strcmp(sLatestStatusName, STATUS_TYPE_90) == 0)
		{
			// New Status  = 90
			tag_t	*taStatus				= NULL;
			int		iCompleteStatusCount	= 0;
			logical	lOnlyStatus90or5		= true;

			// Get release status list
			ITK_CALL(WSOM_ask_release_status_list(tItemRevision, &iCompleteStatusCount, &taStatus));

			// Traverse Status list
			for(int iIdx1 = 0; iIdx1 < iCompleteStatusCount; iIdx1++)
			{
				char	*sTmpStatusName	= NULL;

				// Get status name
				ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", &sTmpStatusName) );

				// Status 5 or 90 ?
				if(strcmp(sTmpStatusName, STATUS_TYPE_5) != 0 && strcmp(sTmpStatusName, STATUS_TYPE_90) != 0)
				{
					lOnlyStatus90or5 = false;
				}

				// Free
				SAFE_SM_FREE(sTmpStatusName);

				// Break
				if(lOnlyStatus90or5 == false)
				{
					break;
				}
			}
			
			// Child-Status: WIP -> 90 or 5 -> 90 ?
			if(lOnlyStatus90or5 == true)
			{
				// WIP -> 90 or 5 -> 90
				int		iRelCountPrimaryConcerned		= 0;
				int		iRelCountPrimarySpecified		= 0;
				int		iRelCountPrimaryIncluded		= 0;
				int		iRelCountPrimarySupplierSpec	= 0;

				tag_t	*taRelPrimaryConcerned			= NULL;
				tag_t	*taRelPrimarySpecified			= NULL;
				tag_t	*taRelPrimaryIncluded			= NULL;
				tag_t	*taRelPrimarySupplierSpec		= NULL;
				
				tag_t	tRelTypeConcerned			= NULLTAG;
				tag_t	tRelTypeSpecified			= NULLTAG;
				tag_t	tRelTypeIncluded			= NULLTAG;
				tag_t	tRelTypeSupplierSpec		= NULLTAG;

				// Find relation types
				ITK_CALL(GRM_find_relation_type("BT7_Rel_Concerned", &tRelTypeConcerned));
				ITK_CALL(GRM_find_relation_type("BT7_Rel_Specified", &tRelTypeSpecified));
				ITK_CALL(GRM_find_relation_type("BT7_Includes", &tRelTypeIncluded));
				ITK_CALL(GRM_find_relation_type("BT7_SUPPLIERSPEC", &tRelTypeSupplierSpec));

				// List primary objects
				ITK_CALL(GRM_list_primary_objects_only(tItemRevision, tRelTypeConcerned, &iRelCountPrimaryConcerned, &taRelPrimaryConcerned));
				ITK_CALL(GRM_list_primary_objects_only(tItemRevision, tRelTypeSpecified, &iRelCountPrimarySpecified, &taRelPrimarySpecified));
				ITK_CALL(GRM_list_primary_objects_only(tItemRevision, tRelTypeIncluded, &iRelCountPrimaryIncluded, &taRelPrimaryIncluded));
				ITK_CALL(GRM_list_primary_objects_only(tItemRevision, tRelTypeSupplierSpec, &iRelCountPrimarySupplierSpec, &taRelPrimarySupplierSpec));

				////////////////////////////////////////////////////////////////////////

				// Concerned-Relation
				concernedLogic_SetReleaseStatus(tItemRevision, iRelCountPrimaryConcerned, taRelPrimaryConcerned, tRelTypeConcerned);

				// Specified-Relation
				specifiedLogic_SetReleaseStatus(tItemRevision, iRelCountPrimarySpecified, taRelPrimarySpecified, tRelTypeSpecified);

				// Included-Relation
				specifiedLogic_SetReleaseStatus(tItemRevision, iRelCountPrimaryIncluded, taRelPrimaryIncluded, tRelTypeIncluded);
			
				// SupplierSpec-Relation
				specifiedLogic_SetReleaseStatus(tItemRevision, iRelCountPrimarySupplierSpec, taRelPrimarySupplierSpec, tRelTypeSupplierSpec);

				////////////////////////////////////////////////////////////////////////

				// Free
				SAFE_SM_FREE(taRelPrimaryConcerned);
				SAFE_SM_FREE(taRelPrimarySpecified);
				SAFE_SM_FREE(taRelPrimaryIncluded);
				SAFE_SM_FREE(taRelPrimarySupplierSpec);
			}

			// Free
			SAFE_SM_FREE(taStatus);
		}

		// Free
		SAFE_SM_FREE(sLatestStatusName);
		
	}

	return iStatus;	
}