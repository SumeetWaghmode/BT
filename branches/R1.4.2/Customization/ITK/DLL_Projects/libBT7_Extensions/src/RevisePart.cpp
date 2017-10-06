#include "BT7_ServerExtensions.h"

void specifiedLogic_RevisePart(tag_t new_rev, tag_t parent_rev, int iRelCountSecondarySpecified, tag_t *taRelSecondarySpecified, tag_t tRelTypeSpecified)
{
	// Loop Secondary Specified Objects - identifiy latest attached revision of each part
	vector<tag_t> vecNewerRevs;
	for(int iIdx1 = 0; iIdx1 < iRelCountSecondarySpecified; iIdx1++)
	{
		if(isDerivedFromClass("ItemRevision", taRelSecondarySpecified[iIdx1]))
		{
			vector<tag_t> vecTmpNewerRevs;
			logical lIgnoreCurrentSetOfNewerRevisions = false;

			// Get Newer Revisions
			getNewerRevisions(taRelSecondarySpecified[iIdx1], &vecTmpNewerRevs);

			// Is one of the newer revisions already attached to the parent?
			for(int iIdx2 = 0; iIdx2 < vecTmpNewerRevs.size(); iIdx2++)
			{
				for(int iIdx3 = 0; iIdx3 < iRelCountSecondarySpecified; iIdx3++)
				{
					if(vecTmpNewerRevs.at(iIdx2) == taRelSecondarySpecified[iIdx3])
					{
						lIgnoreCurrentSetOfNewerRevisions = true;
						break;
					}
				}

				if(lIgnoreCurrentSetOfNewerRevisions == true)
				{
					break;
				}
			}

			// Attach required ?
			if(lIgnoreCurrentSetOfNewerRevisions == false)
			{
				for(int iIdx2 = 0; iIdx2 < vecTmpNewerRevs.size(); iIdx2++)
				{
					tag_t	tLatestStatus	= NULLTAG;
					int		iStatusCount	= 0;
					char	*sLatestStatus	= NULL;

					// Get Latest Status of the Document Revision
					askLatestReleaseStatus(vecTmpNewerRevs.at(iIdx2), &sLatestStatus, &iStatusCount, &tLatestStatus);

					// Attach the Document Revision to the Part Revision if the Document has no status or 10 or 60
					if(isStatusEmptyOr5(tLatestStatus) || isStatus10(tLatestStatus) || isStatus60(tLatestStatus) || isStatus90Plus10or60(parent_rev, vecTmpNewerRevs.at(iIdx2)))
					{

						// Set temporary bypass attribute on the Item Revision
						setRelationBypassAttribute(new_rev, 1);

						// Create Relation (if it does not already exist)
						createRelation(new_rev, vecTmpNewerRevs.at(iIdx2), tRelTypeSpecified);
					}

					// Free
					SAFE_SM_FREE(sLatestStatus);
				}
			}
		}
	}

	// Loop Secondary Specified Objects backwards -> Delete Status 98
	for(int iIdx1 = (iRelCountSecondarySpecified - 1); iIdx1 >= 0; iIdx1--)
	{
		tag_t	tLatestStatus	= NULLTAG;
		int		iStatusCount	= 0;
		char	*sLatestStatus	= NULL;

		// Get Latest Status of the Document Revision
		askLatestReleaseStatus(taRelSecondarySpecified[iIdx1], &sLatestStatus, &iStatusCount, &tLatestStatus);

		// Delete Status 98 relations
		if(isStatus98(tLatestStatus))
		{
			// Delete Relation
			deleteRelation(new_rev, taRelSecondarySpecified[iIdx1], tRelTypeSpecified);
		}
		
		// Free
		SAFE_SM_FREE(sLatestStatus);
	}
}

void concernedLogic_RevisePart(tag_t new_rev, tag_t parent_rev, int iRelCountSecondaryConcerned, tag_t *taRelSecondaryConcerned, tag_t tRelTypeConcerned)
{
	// Loop Secondary Concerned Objects
	vector<tag_t> vecValidItemRevisions;
	// Collect valid revisions
	for(int iIdx1 = 0; iIdx1 < iRelCountSecondaryConcerned; iIdx1++)
	{
		// If derived from Item, check if current revision is latest one.
		// If yes, attach all Revisions to the list of valid relations for copy
		if(isDerivedFromClass("ItemRevision", taRelSecondaryConcerned[iIdx1]))
		{
			tag_t	tSecondaryConcernedItem					= NULLTAG;
			tag_t	tLatestSecondaryConcernedItemRevision	= NULLTAG;

			// Get Item
			ITK_CALL(ITEM_ask_item_of_rev(taRelSecondaryConcerned[iIdx1], &tSecondaryConcernedItem));

			// Get Latest Revision
			ITK_CALL(ITEM_ask_latest_rev(tSecondaryConcernedItem, &tLatestSecondaryConcernedItemRevision));

			// Is current Rev the latest Rev ?
			if(taRelSecondaryConcerned[iIdx1] == tLatestSecondaryConcernedItemRevision)
			{
				int		iTmpRevCount	= 0;
				tag_t	*taTmpRevList	= NULL;

				// Get all Revs
				ITK_CALL(ITEM_list_all_revs(tSecondaryConcernedItem, &iTmpRevCount, &taTmpRevList));

				// Loop Revisions
				for(int iIdx2 = 0; iIdx2 < iTmpRevCount; iIdx2++)
				{
					logical lAlreadyInTheVector = false;

					// Object already in the vector ?
					for(int iIdx3 = 0; iIdx3 < vecValidItemRevisions.size(); iIdx3++)
					{
						if(taTmpRevList[iIdx2] == vecValidItemRevisions.at(iIdx3))
						{
							lAlreadyInTheVector = true;
							break;
						}
					}

					// Add to Vector
					if(lAlreadyInTheVector == false)
					{
						vecValidItemRevisions.push_back(taTmpRevList[iIdx2]);
					}
				}

				// Free
				SAFE_SM_FREE(taTmpRevList);
			}
		}
	}

	// Cleanup concerned revisions
	for(int iIdx1 = 0; iIdx1 < iRelCountSecondaryConcerned; iIdx1++)
	{
		logical lValidObjectForNewRevisions = false;

		for(int iIdx2 = 0; iIdx2 < vecValidItemRevisions.size(); iIdx2++)
		{
			// Valid Revision ?
			if(taRelSecondaryConcerned[iIdx1] == vecValidItemRevisions.at(iIdx2))
			{
				lValidObjectForNewRevisions = true;
				break;
			}
		}

		// Remove Revision if not valid
		if(lValidObjectForNewRevisions == false)
		{
			deleteRelation(new_rev, taRelSecondaryConcerned[iIdx1], tRelTypeConcerned);
		}


		//tag_t	tLatestStatus	= NULLTAG;
		//int		iStatusCount	= 0;
		//char	*sLatestStatus	= NULL;

		//// Get latest primary status
		//askLatestReleaseStatus(taRelSecondaryConcerned[iIdx1], &sLatestStatus, &iStatusCount, &tLatestStatus);

		//// If secondary status = 90 -> Detach the Document Revision from the primary object 
		//if(isStatus90(tLatestStatus) || isStatus98(tLatestStatus))
		//{
		//	deleteRelation(new_rev, taRelSecondaryConcerned[iIdx1], tRelTypeConcerned);
		//}

		//// Attach all revisions of the secondary document to the primary part which have no status or 10 or 60
		//if(isDerivedFromClass("ItemRevision", taRelSecondaryConcerned[iIdx1]))
		//{
		//	tag_t	tDocumentItem			= NULLTAG;
		//	tag_t	*taDocumentRevisions	= NULL;
		//	int		iDocumentRevisionCount	= 0;

		//	// Get Document Item
		//	ITK_CALL(ITEM_ask_item_of_rev(taRelSecondaryConcerned[iIdx1], &tDocumentItem));

		//	// Get all revisions
		//	ITK_CALL(ITEM_list_all_revs(tDocumentItem, &iDocumentRevisionCount, &taDocumentRevisions));

		//	// Loop Revisions
		//	for(int iIdx2 = 0; iIdx2 < iDocumentRevisionCount; iIdx2++)
		//	{
		//		tag_t	tLatestStatus	= NULLTAG;
		//		int		iStatusCount	= 0;
		//		char	*sLatestStatus	= NULL;

		//		// Get Latest Status of the Document Revision
		//		askLatestReleaseStatus(taDocumentRevisions[iIdx2], &sLatestStatus, &iStatusCount, &tLatestStatus);

		//		// Attach the Document Revision to the Part Revision if the Document has no status or 10 or 60
		//		if(isStatusEmptyOr5(tLatestStatus) || isStatus10(tLatestStatus) || isStatus60(tLatestStatus))
		//		{
		//			// Create Relation (if it does not already exist)
		//			createRelation(new_rev, taDocumentRevisions[iIdx2], tRelTypeConcerned);
		//		}

		//		// Free
		//		SAFE_SM_FREE(sLatestStatus);
		//	}

		//	// Free
		//	SAFE_SM_FREE(taDocumentRevisions);
		//}

		//// Free
		//SAFE_SM_FREE(sLatestStatus);
	}
}


int revisePart(tag_t new_rev, tag_t parent_rev, tag_t tRelTypeConcerned, tag_t tRelTypeSpecified, tag_t tRelTypeIncluded, tag_t tRelTypeSupplierSpec)
{
	int iRetVal = ITK_ok;

	tag_t	*taRelSecondaryConcerned		= NULL;
	tag_t	*taRelSecondarySpecified		= NULL;
	tag_t	*taRelSecondaryIncluded			= NULL;
	tag_t	*taRelSecondarySupplierSpec		= NULL;

	int		iRelCountSecondaryConcerned		= 0;
	int		iRelCountSecondarySpecified		= 0;
	int		iRelCountSecondaryIncluded		= 0;
	int		iRelCountSecondarySupplierSpec	= 0;
	
	// Exit if "Zug-Logic" is not applicable for the current user
/*	if(lCurrentUserFromRastatt())
	{
		return ITK_ok;
	}*/

	// List Secondary Objects
	ITK_CALL(GRM_list_secondary_objects_only(new_rev, tRelTypeConcerned, &iRelCountSecondaryConcerned, &taRelSecondaryConcerned));
	ITK_CALL(GRM_list_secondary_objects_only(new_rev, tRelTypeSpecified, &iRelCountSecondarySpecified, &taRelSecondarySpecified));
	ITK_CALL(GRM_list_secondary_objects_only(new_rev, tRelTypeIncluded, &iRelCountSecondaryIncluded, &taRelSecondaryIncluded));
	ITK_CALL(GRM_list_secondary_objects_only(new_rev, tRelTypeSupplierSpec, &iRelCountSecondarySupplierSpec, &taRelSecondarySupplierSpec));

	////////////////////////////////////////////////////////////////////////

	// Concerned-Relation
	concernedLogic_RevisePart(new_rev, parent_rev, iRelCountSecondaryConcerned, taRelSecondaryConcerned, tRelTypeConcerned);

	// Specified-Relation
	specifiedLogic_RevisePart(new_rev, parent_rev, iRelCountSecondarySpecified, taRelSecondarySpecified, tRelTypeSpecified);

	// Included-Relation
	specifiedLogic_RevisePart(new_rev, parent_rev, iRelCountSecondaryIncluded, taRelSecondaryIncluded, tRelTypeIncluded);

	// SupplierSpec-Relation
	specifiedLogic_RevisePart(new_rev, parent_rev, iRelCountSecondarySupplierSpec, taRelSecondarySupplierSpec, tRelTypeSupplierSpec);

	////////////////////////////////////////////////////////////////////////

	// Free
	SAFE_SM_FREE(taRelSecondaryConcerned);
	SAFE_SM_FREE(taRelSecondarySpecified);
	SAFE_SM_FREE(taRelSecondaryIncluded);
	SAFE_SM_FREE(taRelSecondarySupplierSpec);

	return iRetVal;
}
