#include "BT7_ServerExtensions.h"

/**
    This is for reading the Site Preference Rule set for DeepCopy operation and
    based on the copy rules set for each of the Item revision attachments,
    the copy operation will be carried out as

    @param tag_t                    new_rev           The Tag of the newly created Item Revision
    @param char*                    operation         Whether the operation is Save As/Revise
    @param tag_t                    parent_rev        The Tag of the Item Revision over which Save As/Revise operation will be performed
    @param int*                     copyCount         Number of deep copy rules set
    @param ITEM_deepcopy_info_t*    copyInfo          Deep Copy Rule Information
    @param int*                     count             Number of Datasets attached
    @param tag_t**                  copied_objects    The tags of the deep copied attachments
*/
extern USER_EXT_DLL_API int BT7_ItemRevisionDeepCopyPostAction(METHOD_message_t* msg, va_list args) {
	int						status				= ITK_ok;

	tag_t					new_rev				= va_arg(args, tag_t);
    char					*operation			= va_arg(args, char*);
    tag_t                   parent_rev			= va_arg(args, tag_t);
    int						*copyCount			= va_arg(args, int*);
    ITEM_deepcopy_info_t	*copyInfo			= va_arg(args, ITEM_deepcopy_info_t*);
    int						*count				= va_arg(args, int*);
    tag_t					**copied_objects	= va_arg(args, tag_t**);

	char					sObjectType[WSO_name_size_c+1];
	logical					lDebug				= false;

	// Clear Error Stack
	ITK_CALL(EMH_clear_errors());

	// Get Object Type
	ITK_CALL ( WSOM_ask_object_type(new_rev, sObjectType ) ); 
	std::string strObjectType = sObjectType;

	if ( strObjectType.compare(0, 3, "BT7") == 0 )
	{
		// Clear SAP-Properties during Revise / Save-As
		{
			char *saProperties[4];
			saProperties[0] = "bt7_SentMMToSAP";
			saProperties[1] = "bt7_SentDIRToSAP";
			saProperties[2] = "bt7_SAPMatNo";
			saProperties[3] = "bt7_SAPDIRNo";

			for (int iIdx1 = 0; iIdx1 < 4; iIdx1++ ) {
				logical	lPropExists		= false;
				logical	lPropModifiable	= false;
				logical	lIsNullEmpty	= false;		
				logical lIsPartRev		= isDerivedFromClass("Part_0_Revision_alt", new_rev);

				// Attr. Exists?
				ITK_CALL ( POM_attr_exists(saProperties[iIdx1], sObjectType, &lPropExists ) );
				if(lDebug) printf(" > Property %s exists ? %i \n", saProperties[iIdx1], lPropExists);

				if(lIsPartRev == true || lPropExists == true) {
					// Attr. Modifiable?
					ITK_CALL ( AOM_is_modifiable ( new_rev, saProperties[iIdx1], &lPropModifiable ) );
					if(lDebug) printf("   > lPropModifiable = %i \n", lPropModifiable);

					ITK_CALL ( AOM_is_null_empty ( new_rev, saProperties[iIdx1], true, &lIsNullEmpty ) );
					if(lDebug) printf("   > lIsNullEmpty = %i \n", lIsNullEmpty);

					if(lPropModifiable == true) {
						if(lIsNullEmpty == false) {
							// Ask Attr. Type
							PROP_value_type_t	valtype;
							char*				valtype_n = NULL;

							// Get Value Type
							ITK_CALL ( AOM_ask_value_type ( new_rev, saProperties[iIdx1], &valtype, &valtype_n ) );

							// Refresh and lock component
							ITK_CALL ( AOM_refresh ( new_rev, TRUE ) );

							// Differentiate on attribute level
							if(strcmp(saProperties[iIdx1], "bt7_SAPMatNo") == 0){
								// Don't delete the attribute during Revise
								if(strcmp(operation, "SaveAs") == 0) {
									if(valtype == PROP_string){
										ITK_CALL ( AOM_set_value_string(new_rev, saProperties[iIdx1], "" ) );
										ITK_CALL ( AOM_save ( new_rev ) );
									} else if(valtype == PROP_date) {
										ITK_CALL ( AOM_set_value_date (new_rev, saProperties[iIdx1], NULLDATE ) );
										ITK_CALL ( AOM_save ( new_rev ) );
									} else {
										// Not applicable
									}
								}
							} else {
								// All other properties have identical behavior for Revise & SaveAs
								if(valtype == PROP_string){								
									ITK_CALL ( AOM_set_value_string(new_rev, saProperties[iIdx1], "" ) );
									ITK_CALL ( AOM_save ( new_rev ) );
								} else if(valtype == PROP_date) {
									ITK_CALL ( AOM_set_value_date (new_rev, saProperties[iIdx1], NULLDATE ) );
									ITK_CALL ( AOM_save ( new_rev ) );
								} else {
									// Not applicable
								}
							}

							// Refresh and lock component
							ITK_CALL ( AOM_refresh ( new_rev, TRUE ) );

							// Free
							SAFE_SM_FREE (valtype_n);
						} else {
							// Prop. is null or empty
						}
					} else {
						printf(" ERROR: Prop %s not modifiable! \n", saProperties[iIdx1]);
					}
				} else {
						tag_t	tTmpItem = NULLTAG;
						char	sItemId[ITEM_id_size_c + 1];
						char    sItemRevId[ITEM_id_size_c + 1];

						ITK_CALL(ITEM_ask_item_of_rev(new_rev, &tTmpItem));
						ITK_CALL(ITEM_ask_id(tTmpItem, sItemId));
						ITK_CALL(ITEM_ask_rev_id(new_rev, sItemRevId))

							printf(" ERROR: Prop %s does not exist for object %s/%s! \n", saProperties[iIdx1], sItemId, sItemRevId);
				}
			}
		}
	}

	// Clear ECO-ID (revise & save-as)
	{
		// Derived from PartRevision or is Document
		/*Fix for 0SBT-DO-01PDM4BT|SBTDOP-67 ECO Nr. on BT7_ProdTypeKey:: 
		Solution: Modified check for object_Type as BT7_DocumentRevision to isDerived from Class, to allow BT7_ProdTypeKeyRevision too*/
		if(isDerivedFromClass("Part_0_Revision_alt", new_rev) ||  isDerivedFromClass(BT7_DOCUMENTREVISION, new_rev)){
			char *sAttrName = "bt7_docmnumber";

			ITK_CALL ( AOM_refresh ( new_rev, true ) );
			ITK_CALL ( AOM_set_value_string ( new_rev, sAttrName, "" ) );
			ITK_CALL ( AOM_save ( new_rev ) );
			ITK_CALL ( AOM_refresh ( new_rev, false ) );
		} else{
			printf(" > %s -> something else! \n", sObjectType);
		}
	}

	// Revise or Save-As ?
	if(strcmp(operation, "Revise") == 0)
	{
		tag_t	tRelTypeConcerned			= NULLTAG;
		tag_t	tRelTypeSpecified			= NULLTAG;
		tag_t	tRelTypeIncluded			= NULLTAG;
		tag_t	tRelTypeSupplierSpec		= NULLTAG;

		// General Preparation
		ITK_CALL(GRM_find_relation_type("BT7_Rel_Concerned", &tRelTypeConcerned));
		ITK_CALL(GRM_find_relation_type("BT7_Rel_Specified", &tRelTypeSpecified));
		ITK_CALL(GRM_find_relation_type("BT7_Includes", &tRelTypeIncluded));
		ITK_CALL(GRM_find_relation_type("BT7_SUPPLIERSPEC", &tRelTypeSupplierSpec));

		// USE CASE: Revise Part
		revisePart(new_rev, parent_rev, tRelTypeConcerned, tRelTypeSpecified, tRelTypeIncluded, tRelTypeSupplierSpec);

		// USE CASE: REVISE DOCUMENT
		reviseDocument(new_rev, parent_rev, tRelTypeConcerned, tRelTypeSpecified, tRelTypeIncluded, tRelTypeSupplierSpec);

		// CHECK REVISE OF IMPACTED ITEMS
		{
			int		iPrimaryImpactedCount		= 0;
			char	*sPrefValue					= NULL;
			tag_t	tRelTypeImpacted			= NULLTAG;
			tag_t	*taPrimaryImpactedObjects	= NULL;

			// Find relation type
			ITK_CALL(GRM_find_relation_type("CMHasImpactedItem", &tRelTypeImpacted));

			// Count primary objects
			ITK_CALL(GRM_list_primary_objects_only(parent_rev, tRelTypeImpacted,
					&iPrimaryImpactedCount, &taPrimaryImpactedObjects));

		  	// Issue 945. 131 "ECO?" items are not in doc-merge revised.
			// Find preference "BT7_HideMessageByRevise"
			ITK_CALL(PREF_set_search_scope( TC_preference_site ));	
			ITK_CALL(PREF_ask_char_value("BT7_HideMessageByRevise", 0, &sPrefValue));

			if (iPrimaryImpactedCount > 0 )
			{
				if ( sPrefValue == NULL	|| sPrefValue != NULL && strcmp (sPrefValue, "true") != 0 )
				{
					ITK_CALL(EMH_clear_errors());
					status = 919725;
				}
			}

			// Free
			SAFE_SM_FREE(sPrefValue);
			SAFE_SM_FREE(taPrimaryImpactedObjects);
		}
	} else {
		// Save-As
	}

	// Clean TcEnt-Migration-Marker
	setLogicalPOMAttribute(new_rev, false, "bt7_migrated_from_ent");

	return status;
}
