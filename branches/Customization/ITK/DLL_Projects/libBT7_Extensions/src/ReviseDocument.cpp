#include "BT7_ServerExtensions.h"

void specifiedLogic_ReviseDocument(tag_t new_rev, tag_t parent_rev, int iRelCountPrimarySpecified, tag_t *taRelPrimarySpecified, tag_t tRelTypeSpecified)
{
	// Loop Primary Specified Objects
	for(int iIdx1 = 0; iIdx1 < iRelCountPrimarySpecified; iIdx1++)
	{
		tag_t	tPrimaryLatestStatus	= NULLTAG;
		int		iPrimaryStatusCount		= 0;
		char	*sPrimaryLatestStatus	= NULL;

		// Get latest primary status
		askLatestReleaseStatus(taRelPrimarySpecified[iIdx1], &sPrimaryLatestStatus, &iPrimaryStatusCount, &tPrimaryLatestStatus);

		// Differentiate between "regular" and "firmware" specifies logic
		if(isSoftware(taRelPrimarySpecified[iIdx1]) && isProgrammingData(new_rev))
		{
			// "Firmware-Case"

			vector<tag_t> vecNewerParentRevisions;

			// If status WIP or 5 -> Attach the new Document Revision to the object 
			if(isStatusEmptyOr5(tPrimaryLatestStatus))
			{
				// Set temporary bypass attribute on the Item Revision
				setRelationBypassAttribute(taRelPrimarySpecified[iIdx1], 1);

				// Create Relation
				createRelation(taRelPrimarySpecified[iIdx1], new_rev, tRelTypeSpecified);
			}

			// Get newer parent revisions
			getNewerRevisions(taRelPrimarySpecified[iIdx1], &vecNewerParentRevisions);

			// Traverse newer parent revisions
			for(int iIdx2 = 0; iIdx2 < vecNewerParentRevisions.size(); iIdx2++)
			{
				tag_t	tTmpPrimaryLatestStatus	= NULLTAG;
				int		iTmpPrimaryStatusCount		= 0;
				char	*sTmpPrimaryLatestStatus	= NULL;

				// Get latest status of newer parent revisions
				askLatestReleaseStatus(vecNewerParentRevisions.at(iIdx2), &sTmpPrimaryLatestStatus, &iTmpPrimaryStatusCount, &tTmpPrimaryLatestStatus);

				// If status WIP, 5 -> Attach the new Document Revision to the object 
				if(isStatusEmptyOr5(tTmpPrimaryLatestStatus))
				{
					// Set temporary bypass attribute on the Item Revision
					setRelationBypassAttribute(vecNewerParentRevisions.at(iIdx2), 1);

					// Create Relation
					createRelation(vecNewerParentRevisions.at(iIdx2), new_rev, tRelTypeSpecified);
				}

				// Free
				SAFE_SM_FREE(sTmpPrimaryLatestStatus);
			}
		} else {
			// "Regular-Case"

			// If status WIP, 5, 10 , PR+ or 60 -> Attach the new Document Revision to the object 
			if(isStatusEmptyOr5(tPrimaryLatestStatus) || isStatus10(tPrimaryLatestStatus) || isStatus60(tPrimaryLatestStatus) ||isStatusPrereleaseplus(tPrimaryLatestStatus))
			{
				// Set temporary bypass attribute on the Item Revision
				setRelationBypassAttribute(taRelPrimarySpecified[iIdx1], 1);

				// Create Relation
				createRelation(taRelPrimarySpecified[iIdx1], new_rev, tRelTypeSpecified);
			}
		}

		// Free
		SAFE_SM_FREE(sPrimaryLatestStatus);
	}
}

void concernedLogic_ReviseDocument(tag_t new_rev, tag_t parent_rev, int iRelCountPrimaryConcerned, tag_t *taRelPrimaryConcerned, tag_t tRelTypeConcerned)
{
	// Loop Primary Concerned Objects
	for(int iIdx1 = 0; iIdx1 < iRelCountPrimaryConcerned; iIdx1++)
	{
		// Attach all revisions to the primary part which have no status or 10 or 60
		if(isDerivedFromClass("ItemRevision", taRelPrimaryConcerned[iIdx1]))
		{
			tag_t	tLatestStatus	= NULLTAG;
			int		iStatusCount	= 0;
			char	*sLatestStatus	= NULL;

			// Get Latest Status of the Document Revision
			askLatestReleaseStatus(taRelPrimaryConcerned[iIdx1], &sLatestStatus, &iStatusCount, &tLatestStatus);

			if(isStatusEmptyOr5(tLatestStatus) || isStatus10(tLatestStatus) || isStatus60(tLatestStatus) ||isStatusPrereleaseplus(tLatestStatus))
			{
				// No temporary Bypass required, as the rules of the Concerns Pre-Condition still match

				// Create Relation (if it does not already exist)
				createRelation(taRelPrimaryConcerned[iIdx1], new_rev, tRelTypeConcerned);
			}

			// Free
			SAFE_SM_FREE(sLatestStatus);
		}
	}
}

int reviseDocument(tag_t new_rev, tag_t parent_rev, tag_t tRelTypeConcerned, tag_t tRelTypeSpecified, tag_t tRelTypeIncluded, tag_t tRelTypeSupplierSpec)
{
	int iRetVal = ITK_ok;

	tag_t	*taRelPrimaryConcerned			= NULL;
	tag_t	*taRelPrimarySpecified			= NULL;
	tag_t	*taRelPrimaryIncluded			= NULL;
	tag_t	*taRelPrimarySupplierSpec		= NULL;

	int		iRelCountPrimaryConcerned		= 0;
	int		iRelCountPrimarySpecified		= 0;
	int		iRelCountPrimaryIncluded		= 0;
	int		iRelCountPrimarySupplierSpec	= 0;

	tag_t	tLatestRevisionBeforeRevise	= parent_rev;

	// Exit if "Zug-Logic" is not applicable for the current user
	/*if(lCurrentUserFromRastatt())
	{
		return ITK_ok;
	}*/


	// Identify latest revision before revise (-> required for specified-case)
	{
		int		iTmpRevCount	= 0;
		tag_t	tTmpItem		= NULLTAG;
		tag_t	*taTmpRevs		= NULL;

		// Get Item
		ITK_CALL(ITEM_ask_item_of_rev(new_rev, &tTmpItem));

		// List Revisions
		ITK_CALL(ITEM_list_all_revs(tTmpItem, &iTmpRevCount, &taTmpRevs));

		// Traverse Revisions backwards
		for(int iIdx1 = (iTmpRevCount - 1); iIdx1 >= 0; iIdx1--)
		{
			// Is latest revision which is unequal to the new revision ?
			if(taTmpRevs[iIdx1] != new_rev)
			{
				tLatestRevisionBeforeRevise = taTmpRevs[iIdx1];
				break;
			}
		}

		// Free
		SAFE_SM_FREE(taTmpRevs);
	}


	// List Primary Objects ("latest revision before revise" for specified, "parent based-on revision" for concerned)
	ITK_CALL(GRM_list_primary_objects_only(tLatestRevisionBeforeRevise, tRelTypeConcerned, &iRelCountPrimaryConcerned, &taRelPrimaryConcerned));
	ITK_CALL(GRM_list_primary_objects_only(tLatestRevisionBeforeRevise, tRelTypeSpecified, &iRelCountPrimarySpecified, &taRelPrimarySpecified));
	ITK_CALL(GRM_list_primary_objects_only(tLatestRevisionBeforeRevise, tRelTypeIncluded, &iRelCountPrimaryIncluded, &taRelPrimaryIncluded));
	ITK_CALL(GRM_list_primary_objects_only(tLatestRevisionBeforeRevise, tRelTypeSupplierSpec, &iRelCountPrimarySupplierSpec, &taRelPrimarySupplierSpec));
	
	////////////////////////////////////////////////////////////////////////

	// Concerned-Relation
	concernedLogic_ReviseDocument(new_rev, parent_rev, iRelCountPrimaryConcerned, taRelPrimaryConcerned, tRelTypeConcerned);

	// Specified-Relation
	specifiedLogic_ReviseDocument(new_rev, parent_rev, iRelCountPrimarySpecified, taRelPrimarySpecified, tRelTypeSpecified);

	// Included-Relation
	specifiedLogic_ReviseDocument(new_rev, parent_rev, iRelCountPrimaryIncluded, taRelPrimaryIncluded, tRelTypeIncluded);

	// SupplierSpec-Relation
	specifiedLogic_ReviseDocument(new_rev, parent_rev, iRelCountPrimarySupplierSpec, taRelPrimarySupplierSpec, tRelTypeSupplierSpec);

	////////////////////////////////////////////////////////////////////////

	// Free
	SAFE_SM_FREE(taRelPrimaryConcerned);
	SAFE_SM_FREE(taRelPrimarySpecified);
	SAFE_SM_FREE(taRelPrimaryIncluded);
	SAFE_SM_FREE(taRelPrimarySupplierSpec);

	return iRetVal;
}
