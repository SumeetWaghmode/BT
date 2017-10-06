#ifdef __cplusplus

extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <epm/signoff.h>
#include <tccore/method.h>
#include <bom/bom.h>
#include <fclasses/tc_date.h>
#include <cfm/cfm.h>
#include <ae/tool.h>
#include <itk/mem.h>
#include <property/prop.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tc/folder.h>
#include <ics/ics.h>
#include <ae/dataset.h>
#include <ae/datasettype.h>
#include <tc/envelope.h>
#include <ps/ps.h>
#include <res/res_itk.h>
#include <sa/am.h>
#include <tc/iman_arguments.h>
#include <tccore/project.h>
#include <tc/tc_util.h>
#include <form/form.h>
#include <epm\epm.h>
#include <publication\iir_itk.h>

#include <sub_mgr/tcactionhandler.h>
#include <sub_mgr/tceventmgr.h>
#include <sub_mgr/tceventtype.h>
#include <sub_mgr/standardtceventtypes.h>
#include <sub_mgr/subscription.h>
#include <property/nr.h>

#include "sy_debug.h"

#include "SBT_Workspaceobject.h"
#include "SBT_ItemRevision.h"
#include "SBT_Utilities.h"
#include "SBT_Utilities_d.h"
#include "SBT_Workflow.h"
#include "SBT_Workflow_Attributes.h"
#include "SBT_File.h"

#include "SBT_Workflow_grouphandler.h"

#ifdef __cplusplus
}
#endif

// Include Common
#include "SBT_Common.h"

#include <cctype>
#include <iostream>
#include <string>

/** @brief TC Relation */
#define TC_REL_MAT1USESMATERIAL						"Mat1UsesMaterial"
#define TC_REL_CMHASSOLUTIONITEM					"CMHasSolutionItem"

/** @brief TC Properties */
#define TC_PROP_REV_ID								"item_revision_id"
#define TC_MY_WORKFLOW_TASKS						"fnd0StartedWorkflowTasks"
#define TC_PROP_FND0INPROCESS						"fnd0InProcess"
/** @brief Constants */
#define FILE_TITLE									"Information Report"


logical checkForWipOr98(tag_t tObject, vector<tag_t> *vecTcObjectsToBeAnulled, logical lTestOnWiP, logical lTestOn90 );

void getAllBOMLines(tag_t tRevTag, int *childBOMLinesCount, tag_t **childBOMLines, char *revision_rule)
{
	tag_t tClass = NULLTAG;

	// Objects is an ItemRevision?
	if ( object_is_of_class_or_desc(tRevTag, "ItemRevision", &tClass) == true )
	{
		// Init traversal of BOM
		tag_t windowTag  = NULLTAG;
		tag_t ruleTag    = NULLTAG;
		tag_t topLineTag = NULLTAG;

		ITK(CFM_find(revision_rule, &ruleTag) );

		ITK(BOM_create_window(&windowTag) );
		ITK(BOM_set_window_config_rule(windowTag, ruleTag) );
		ITK(BOM_set_window_top_line(windowTag, NULLTAG, tRevTag, NULLTAG, &topLineTag) );
		ITK(BOM_set_window_pack_all(windowTag, true) );
		ITK(BOM_line_ask_child_lines(topLineTag, childBOMLinesCount, childBOMLines) );
	}
}


/**
 * All 1-level parents have status 98 or no status
 */
logical allParentsOn1stLevelHaveStatus98OrWip(tag_t tItemRevision, vector<tag_t> *vecTcObjectsToBeAnulled) {
	int     iBOMParents               = 0;
	int     iSupplierSpecParents      = 0;
	int     iSpecifiedParents     	  = 0;
	int     iAltMatParents		   	  = 0;// R1.05
	int     iVMRepresentsParents      = 0;
	int     iManufacturerParents      = 0;	
	int *   iaBOMParentLevels         = NULL;
	tag_t	tRelTypeSupplierSpec	  = NULLTAG;
	tag_t	tRelTypeSpecified		  = NULLTAG;
	tag_t	tRelTypeAltMat	  		  = NULLTAG;// R1.05
	tag_t	tRelTypeVMRepresents	  = NULLTAG;
	tag_t	tRelTypeManufacturer	  = NULLTAG;
	tag_t * taBOMParents              = NULL;
	tag_t * taSupplierSpecParents     = NULL;
	tag_t * taSpecifiedParents    	  = NULL;
	tag_t * taAltMatParents		   	  = NULL;// R1.05
	tag_t * taVMRepresentsParents     = NULL;
	tag_t * taManufacturerParents     = NULL;
	logical lAllParentsStatus98OrWip  = true;
	tag_t  tSelectedItem = NULLTAG;

	ITK( GRM_find_relation_type( "BT7_SUPPLIERSPEC", &tRelTypeSupplierSpec ) );
	ITK( GRM_find_relation_type( "BT7_Rel_Specified", &tRelTypeSpecified ) );
	ITK( GRM_find_relation_type( "VMRepresents", &tRelTypeVMRepresents ) );
	ITK( GRM_find_relation_type( "TC_vendor_part_rel", &tRelTypeManufacturer ) );
	ITK( GRM_find_relation_type( TC_REL_MAT1USESMATERIAL, &tRelTypeAltMat ) );// R1.05
	
	// Where used ( all Revision Rules ) ?
	ITK( PS_where_used_all( tItemRevision, 1, &iBOMParents, &iaBOMParentLevels, &taBOMParents ) );
	
	// Where referenced ( Specified and Supplier Spec )
	ITK( GRM_list_primary_objects_only(tItemRevision, tRelTypeSupplierSpec, &iSupplierSpecParents, &taSupplierSpecParents ) );
	ITK( GRM_list_primary_objects_only(tItemRevision, tRelTypeSpecified, &iSpecifiedParents, &taSpecifiedParents ) );
	
	// Where referenced ( Alternate Material )
	ITK( GRM_list_primary_objects_only(tItemRevision, tRelTypeAltMat, &iAltMatParents, &taAltMatParents ) );// R1.05
	
	// Get Item
	ITK(ITEM_ask_item_of_rev(tItemRevision, &tSelectedItem) );
	// Where referenced (VMRepresents and TC_vendor_part_rel)
	ITK( GRM_list_primary_objects_only(tSelectedItem, tRelTypeVMRepresents, &iVMRepresentsParents, &taVMRepresentsParents ) );
	ITK( GRM_list_primary_objects_only(tSelectedItem, tRelTypeManufacturer, &iManufacturerParents, &taManufacturerParents ) );

	// Traverse Parents
	for ( int iIdx1 = 0; iIdx1 < iBOMParents; iIdx1++ ) {
		if(lAllParentsStatus98OrWip)
			lAllParentsStatus98OrWip = checkForWipOr98(taBOMParents[iIdx1], vecTcObjectsToBeAnulled, true, true);// R1.05
		else
			break;
	}
	for ( int iIdx1 = 0; iIdx1 < iSupplierSpecParents; iIdx1++ ) {
		if(lAllParentsStatus98OrWip)
			lAllParentsStatus98OrWip = checkForWipOr98(taSupplierSpecParents[iIdx1], vecTcObjectsToBeAnulled, true, true);// R1.05
		else
			break;
	}
	for ( int iIdx1 = 0; iIdx1 < iSpecifiedParents; iIdx1++ ) {
		if(lAllParentsStatus98OrWip)
			lAllParentsStatus98OrWip = checkForWipOr98(taSpecifiedParents[iIdx1], vecTcObjectsToBeAnulled, true, true);// R1.05
		else
			break;
	}
	/*Merged changes of R1.05 started*/	
	for ( int iIdx1 = 0; iIdx1 < iAltMatParents; iIdx1++ ) {
		if(lAllParentsStatus98OrWip)
			lAllParentsStatus98OrWip = checkForWipOr98(taAltMatParents[iIdx1], vecTcObjectsToBeAnulled, true, true);
		else
			break;
	}
	/*Merged changes of R1.05 ends*/
	for ( int iIdx1 = 0; iIdx1 < iVMRepresentsParents; iIdx1++ ) {
		if(lAllParentsStatus98OrWip)
			lAllParentsStatus98OrWip = checkForWipOr98(taVMRepresentsParents[iIdx1], vecTcObjectsToBeAnulled, false, true);// R1.05
		else
			break;
	}
	for ( int iIdx1 = 0; iIdx1 < iManufacturerParents; iIdx1++ ) {
		if(lAllParentsStatus98OrWip)
			lAllParentsStatus98OrWip = checkForWipOr98(taManufacturerParents[iIdx1], vecTcObjectsToBeAnulled, false, true);// R1.05
		else
			break;
	}
	// Free
	SAFE_SM_FREE(taSupplierSpecParents);// R1.05
	SAFE_SM_FREE(taSpecifiedParents);// R1.05
	SAFE_SM_FREE(taAltMatParents);// R1.05
	SAFE_SM_FREE(taVMRepresentsParents);// R1.05
	SAFE_SM_FREE(taManufacturerParents);// R1.05
	SAFE_SM_FREE(iaBOMParentLevels);
	SAFE_SM_FREE(taBOMParents);

	return lAllParentsStatus98OrWip;
}

logical attachBomChildsForAnulling(tag_t tSelectedItemRevision, vector<tag_t> *vecTcObjectsToBeAnulled, vector<tag_t> *vecTcObjectsAlreadyTested, logical lIsTopAssembly)
{
	tag_t  tSelectedItem = NULLTAG;
	tag_t *taItemRevList = NULL;
	int    iItemRevCount = 0;
	char * ObjType		= NULL;

	// Get Item and all Item Revisions of the current object
	ITK(ITEM_ask_item_of_rev(tSelectedItemRevision, &tSelectedItem) );
	ITK(ITEM_list_all_revs(tSelectedItem, &iItemRevCount, &taItemRevList) );

	// Traverse all Revisions and check if they're ready to be anulled
	logical lAllParentsOn1stLevelHaveStatus98OrWip = true;
	logical lTempComponentIsAlreadyInList = false;
	logical lTempComponentIsAlreadyTested = false;

	for ( int iIdx1 = 0; iIdx1 < iItemRevCount; iIdx1++ )
	{
		// Jump over already in Lists Test if the Component is a top assembly
		if (!lIsTopAssembly)
		{
			// Current Component is already in the List?
			for ( int iIdx2 = 0; iIdx2 < ( *vecTcObjectsToBeAnulled ).size( ); iIdx2++ )
			{
				if ( ( *vecTcObjectsToBeAnulled ).at(iIdx2) == tSelectedItemRevision )
				{
					lTempComponentIsAlreadyInList = true;
					break;
				}
			}
			if ( ( lTempComponentIsAlreadyInList == false ) )
			{
				// Current Component already tested ?
				for ( int iIdx2 = 0; iIdx2 < ( *vecTcObjectsAlreadyTested ).size( ); iIdx2++ )
				{
					if ( ( *vecTcObjectsAlreadyTested ).at(iIdx2) == taItemRevList[iIdx1] )
					{
						lTempComponentIsAlreadyTested = true;
						break;
					}
				}
			}
		}
		if ( ( lTempComponentIsAlreadyInList == false ) /*&& ( lTempComponentIsAlreadyTested == false )*/ )
		{
			// Current Component ready to be anulled ?
			logical lTempAllParentsOn1stLevelHaveStatus98OrWip = allParentsOn1stLevelHaveStatus98OrWip(taItemRevList[iIdx1], vecTcObjectsToBeAnulled);
			( *vecTcObjectsAlreadyTested ).push_back(taItemRevList[iIdx1]);

			if ( lTempAllParentsOn1stLevelHaveStatus98OrWip == false )
			{
				lAllParentsOn1stLevelHaveStatus98OrWip = false;
				break;
			}
		}
	}

	// Return if at least one Revision is not ready to be anulled
	if ( ( lAllParentsOn1stLevelHaveStatus98OrWip == false ) || ( lTempComponentIsAlreadyInList == true ) /*|| ( lTempComponentIsAlreadyTested == true )*/ )
	{
		SAFE_SM_FREE(taItemRevList);
		return false;
	}

	// Attach Item + all ItemRevisions as targets to the Workflow
	( *vecTcObjectsToBeAnulled ).push_back(tSelectedItem);

	// Traverse Revisions for attaching to the vector
	for ( int iIdx1 = 0; iIdx1 < iItemRevCount; iIdx1++ )
	{
		ITK(WSOM_ask_object_type2(taItemRevList[iIdx1],&ObjType));
		// If status is not equal to 98 and the current ItemRevision is not the TopRevision ( -> this object is already attached to the workflow )
		//if ( ( lIsTopAssembly == true ) /*&& ( taItemRevList[iIdx1] != tSelectedItemRevision )*/ )
		if ( ( lIsTopAssembly == true ) || (tc_strcmp(ObjType,"BT7_VendorPartRevision")==0))
		{
			( *vecTcObjectsToBeAnulled ).push_back(taItemRevList[iIdx1]);
		}
	}
	/*Merged R1.05 started*/
	tag_t	tRelTypeSupplierSpec	  = NULLTAG;
	tag_t	tRelTypeSpecified		  = NULLTAG;
	tag_t	tRelTypeAltMat	  		  = NULLTAG;
	tag_t	tRelTypeVMRepresents	  = NULLTAG;
	tag_t	tRelTypeManufacturer	  = NULLTAG;
	
	ITK( GRM_find_relation_type( "BT7_SUPPLIERSPEC", &tRelTypeSupplierSpec ) );
	ITK( GRM_find_relation_type( "BT7_Rel_Specified", &tRelTypeSpecified ) );
	ITK( GRM_find_relation_type( TC_REL_MAT1USESMATERIAL, &tRelTypeAltMat ) );
	ITK( GRM_find_relation_type( "VMRepresents", &tRelTypeVMRepresents ) );
	ITK( GRM_find_relation_type( "TC_vendor_part_rel", &tRelTypeManufacturer ) );
	/*Merged R1.05 ends*/
	// Traverse Revisions to analyze their BOM-Childs and Pseudo-Folder
	for ( int iIdx1 = 0; iIdx1 < iItemRevCount; iIdx1++ )
	{
		int    iChildRevCount 				= 0;
		tag_t  tRelType	  					= NULLTAG;
		tag_t  taRev						= NULLTAG;
		tag_t  tClassTag					= NULLTAG;
		tag_t  taVPItem						= NULLTAG;
		tag_t *tBomChildrenTags 			= NULL;
		tag_t *tSupplierSpecChildrenTags 	= NULL;
		tag_t *tSpecifiedChildrenTags 		= NULL;
		tag_t *tAltMatChildrenTags	 		= NULL;//R1.05
		tag_t *tVPChildrenTags 				= NULL;
		tag_t *tManufacturerChildrenTags	= NULL;
		char * Objtype_name					= NULL;
		char * Objtype_name2				= NULL;		

		// BOM
		// Get all BOM Lines
		getAllBOMLines(taItemRevList[iIdx1], &iChildRevCount, &tBomChildrenTags, "latest working");

		// Loop over children
		for ( int iIdx = 0; iIdx < iChildRevCount; iIdx++ )
		{
			// Get the revision
			int   iBomLineItemRevAttribute = 0;
			tag_t tChildRevTag             = NULLTAG;

			ITK(BOM_line_look_up_attribute(bomAttr_lineItemRevTag, &iBomLineItemRevAttribute) );
			ITK(BOM_line_ask_attribute_tag(tBomChildrenTags[iIdx], iBomLineItemRevAttribute, &tChildRevTag) );
			
			// Recursive Call
			attachBomChildsForAnulling(tChildRevTag, vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
		}
		
		// FREE
		SAFE_SM_FREE(tBomChildrenTags);
		
		
		// SUPPLIER SPEC
		// Get the Relation Tag
		ITK( GRM_find_relation_type( "BT7_SUPPLIERSPEC", &tRelType ) );
		// Get all Childs under the Relation
		ITK( GRM_list_secondary_objects_only(taItemRevList[iIdx1], tRelType, &iChildRevCount, &tSupplierSpecChildrenTags ) );
		// Traverse all Childs
		for ( int iIdx = 0; iIdx < iChildRevCount; iIdx++ )
		{
			if ( object_is_of_class_or_desc(tSupplierSpecChildrenTags[iIdx], "ItemRevision", &tClassTag) )
			{
				// Recursive Call
				attachBomChildsForAnulling(tSupplierSpecChildrenTags[iIdx], vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
			else if ( object_is_of_class_or_desc(tSupplierSpecChildrenTags[iIdx], "Item", &tClassTag) )
			{
				ITK(ITEM_ask_latest_rev(tSupplierSpecChildrenTags[iIdx], &taRev));
				// Recursive Call
				attachBomChildsForAnulling(taRev, vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
				
			}
		}
		
		// FREE
		SAFE_SM_FREE(tSupplierSpecChildrenTags);
		
		
		// SPECIFIED
		// Get the Relation Tag
		ITK( GRM_find_relation_type( "BT7_Rel_Specified", &tRelTypeSpecified ) );//R1.05
		// Get all Childs under the Relation
		ITK( GRM_list_secondary_objects_only(taItemRevList[iIdx1], tRelTypeSpecified, &iChildRevCount, &tSpecifiedChildrenTags ) );//R1.05
		// Traverse all Childs
		for ( int iIdx = 0; iIdx < iChildRevCount; iIdx++ )
		{
			if ( object_is_of_class_or_desc(tSpecifiedChildrenTags[iIdx], "ItemRevision", &tClassTag) )
			{
				// Recursive Call
				attachBomChildsForAnulling(tSpecifiedChildrenTags[iIdx], vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
			else if ( object_is_of_class_or_desc(tSpecifiedChildrenTags[iIdx], "Item", &tClassTag) )
			{
				ITK(ITEM_ask_latest_rev(tSpecifiedChildrenTags[iIdx], &taRev));
				// Recursive Call
				attachBomChildsForAnulling(taRev, vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
				
			}
		}
		
		// FREE
		SAFE_SM_FREE(tSpecifiedChildrenTags);
		
		/*Merged R1.05 started*/
		// Alternate Material
		// Get all Childs under the Relation
		ITK( GRM_list_secondary_objects_only(taItemRevList[iIdx1], tRelTypeAltMat, &iChildRevCount, &tAltMatChildrenTags ) );
		// Traverse all Childs
		for ( int iIdx = 0; iIdx < iChildRevCount; iIdx++ )
		{
			if ( object_is_of_class_or_desc(tAltMatChildrenTags[iIdx], "ItemRevision", &tClassTag) )
			{
				// Recursive Call
				attachBomChildsForAnulling(tAltMatChildrenTags[iIdx], vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
			else if ( object_is_of_class_or_desc(tAltMatChildrenTags[iIdx], "Item", &tClassTag) )
			{
				ITK(ITEM_ask_latest_rev(tAltMatChildrenTags[iIdx], &taRev));
				// Recursive Call
				attachBomChildsForAnulling(taRev, vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
				
			}
		}
		
		// FREE
		SAFE_SM_FREE(tAltMatChildrenTags);
		/*Merged R1.05 ends*/
		
		
		// VMRepresents
		// Get the Relation Tag
		ITK( GRM_find_relation_type( "VMRepresents", &tRelTypeVMRepresents ) );//R1.05
		// Get all Childs under the Relation
		ITK( GRM_list_secondary_objects_only(taItemRevList[iIdx1], tRelTypeVMRepresents, &iChildRevCount, &tVPChildrenTags ) );//R1.05
		// Traverse all Childs
		for ( int iIdx = 0; iIdx < iChildRevCount; iIdx++ )
		{
			ITK(WSOM_ask_object_type2(tVPChildrenTags[iIdx],&Objtype_name));

			if ( object_is_of_class_or_desc(tVPChildrenTags[iIdx], "ItemRevision", &tClassTag) )
			{
				// Recursive Call
				attachBomChildsForAnulling(tVPChildrenTags[iIdx], vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
			else if ( object_is_of_class_or_desc(tVPChildrenTags[iIdx], "Item", &tClassTag) )
			{
				ITK(ITEM_ask_latest_rev(tVPChildrenTags[iIdx], &taRev));
				// Recursive Call
				attachBomChildsForAnulling(taRev, vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
		}
		
		// Free
		SAFE_SM_FREE(tVPChildrenTags);
		
		
		// TC_vendor_part_rel
		// Get the Item
		ITK( ITEM_ask_item_of_rev(taItemRevList[iIdx1], &taVPItem) );
		
		// Get the Relation Tag
		ITK( GRM_find_relation_type( "TC_vendor_part_rel", &tRelTypeManufacturer ) );//R1.05
		// Get all Childs under the Relation
		ITK( GRM_list_secondary_objects_only(taVPItem, tRelTypeManufacturer, &iChildRevCount, &tManufacturerChildrenTags ) );//R1.05
		// Traverse all Childs
		for ( int iIdx = 0; iIdx < iChildRevCount; iIdx++ )
		{
			ITK(WSOM_ask_object_type2(tManufacturerChildrenTags[iIdx],&Objtype_name2));

			if ( object_is_of_class_or_desc(tManufacturerChildrenTags[iIdx], "ItemRevision", &tClassTag) )
			{
				// Recursive Call
				attachBomChildsForAnulling(tManufacturerChildrenTags[iIdx], vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
			else if ( object_is_of_class_or_desc(tManufacturerChildrenTags[iIdx], "Item", &tClassTag) )
			{
				ITK(ITEM_ask_latest_rev(tManufacturerChildrenTags[iIdx], &taRev));
				// Recursive Call
				attachBomChildsForAnulling(taRev, vecTcObjectsToBeAnulled, vecTcObjectsAlreadyTested, false);
			}
		}
	
	// Free
	SAFE_SM_FREE(tManufacturerChildrenTags);
			
	}

	// Free
	SAFE_SM_FREE(taItemRevList);

	return true;
}


/**
 * @brief
 * Checks if the target is a PartRevision or a Document with the status None, Prerelease ( 10 ) or Rejected ( 90 ) .
 * Attach the incorrect items to a text file and exit
 */
EPM_decision_t SBT_RH_valid_for_release(EPM_rule_message_t msg)
{
	int               retcode          = ITK_ok;
	int               iTargetCount     = 0;
	const char *      theFunction      = "SBT_RH_valid_for_release";
	char *            sDecision        = EPM_RESULT_True;
	tag_t             rootTaskTag      = NULLTAG;
	tag_t             objectClassIdTag = NULLTAG;
	tag_t *           pTargetTags      = NULLTAG;
	logical           lStatusOK        = true;
	logical           lObjectTypeOK    = true;
	std::stringstream txtStringStream;
	char              sObjectTypeTmp[WSO_name_size_c + 1];


	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		// Get the Object id
		char *sObjectId;
		ITK(WSOM_ask_object_id_string(pTargetTags[iIdx1], &sObjectId) );

		// Get latest release status
		int    iStatusCount = 0;
		char * sStatusName      = NULL;
		askLatestReleaseStatus(pTargetTags[iIdx1], &sStatusName, &iStatusCount);

		if ( iStatusCount == 0 || (( strcmp(sStatusName, STATUS_TYPE_10) != 0 ) && ( strcmp(sStatusName, STATUS_TYPE_90) != 0 )))
		{
			lStatusOK = false;
			txtStringStream << "The target id <" << sObjectId << "> status is not one of the following: NONE, Prereleased ( 10 ) or Rejected ( 90 ) " << endl;
		}

		if ( sStatusName != NULL )
		{
			MEM_free(sStatusName);
		}

		// Get the object type
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		// Check allowed target type
		tag_t   tmpDelete;
		logical lTestPartRevision = object_is_of_class_or_desc(pTargetTags[iIdx1], "BT7_BTPartRevision", &tmpDelete);

		if ( ( strcmp(sObjectType, "BT7_DocumentRevision") != 0 ) &&
			 ( lTestPartRevision == false )
			 )
		{
			lObjectTypeOK = false;
			txtStringStream << "The target id <" << sObjectId << "> is not a component ( el or mech ) or a document or an assemply" << endl;
		}

		// allow only one target type
		if ( iIdx1 == 0 )
		{
			ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectTypeTmp) );
		}

		if ( strcmp(sObjectTypeTmp, sObjectType) != 0 )
		{
			lObjectTypeOK = false;
			txtStringStream << "The target id <" << sObjectId << "> type must be equal to the first target type " << sObjectTypeTmp << endl;
		}
	}

	// if the decision is false, create a file and attach it to the workflow
	if ( ( lObjectTypeOK == false ) || ( lStatusOK == false ) )
	{
		// set decision
		sDecision = EPM_RESULT_False;

		//create a new ImanFile
		tag_t      txtFile     = NULLTAG;
		IMF_file_t txtFileDesc = NULL;

		ITK(IMF_create_file(NULL, SS_TEXT, &txtFile, &txtFileDesc) );
		ITK(IMF_open_file(txtFileDesc, SS_RDWRITE) );

		//write the XML-Stream to the new ImanFile
		std::string txtStringLine = "";

		while ( getline(txtStringStream, txtStringLine, '\n') )
		{
			if ( txtStringLine.length( ) <= SS_MAXLLEN )
			{
				ITK(IMF_write_file_line(txtFileDesc, txtStringLine.c_str( ) ) );
			}
		}

		//Close and save the new ImanFile
		ITK(IMF_close_file(txtFileDesc) );
		ITK(AOM_save(txtFile) );
		ITK(AOM_unload(txtFile) );

		//create the dataset and upload the dataset
		createDataset(msg.task, txtFile, "Error Report");
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );
	LEAVE(theFunction);
	return EPM_go;
}

/**
 * @brief
 * Attach all PDFs + all DocumentRevisions which are related as "BT7_Rel_Specified"
 */
int SBT_AH_attach_pdf_and_document_revs(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_attach_pdf_and_document_revs";
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t *     pTargetTags        = NULL;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClass = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true )
		{
			vector<tag_t> vecNewWorkflowTargets;

			// Get PDF-Datasets
			vector<char*> vecRelationTypes;

			vecRelationTypes.push_back("IMAN_specification");
			vecRelationTypes.push_back("TC_Attaches");
			vecRelationTypes.push_back("BT7_Rel_Specified");
			vecRelationTypes.push_back("BT7_SUPPLIERSPEC");

			for(int iIdx2 = 0; iIdx2 < vecRelationTypes.size(); iIdx2++)
			{
				tag_t	tRelationType			= NULLTAG;
				tag_t	*taSecondaryObjects		= NULL;
				int		iSecondaryCount			= 0;

				// Get Secondary Objects
				getSecondaryObjects(pTargetTags[iIdx1], vecRelationTypes.at(iIdx2), &tRelationType, &iSecondaryCount, &taSecondaryObjects);

				// Traverse Secondary Objects
				for(int iIdx3 = 0; iIdx3 < iSecondaryCount; iIdx3++)
				{
					char sSecondaryType[WSO_name_size_c+1];
					
					// Get Object Type
					ITK(WSOM_ask_object_type(taSecondaryObjects[iIdx3], sSecondaryType));

					// If object is a PDF, BT6_Doc or BT7_Doc -> attach to the workflow
					if( /*strcmp(sSecondaryType, "PDF") == 0 ||*/ strcmp(sSecondaryType, "BT7_DocumentRevision") == 0 || strcmp(sSecondaryType, "BT7_DocumentRevision") == 0)
					{
						logical lAlreadyAttached	= false;
						int		iTmpTargetCount		= 0;
						tag_t	*pTmpTargetTags		= NULL;

						// Ask Attachments
						ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTmpTargetCount, &pTmpTargetTags) );

						/* Loop over all target attachments */
						for ( int iIdx4 = 0; iIdx4 < iTmpTargetCount; iIdx4++ )
						{
							// Object already attached to the workflow?
							if(pTmpTargetTags[iIdx4] == taSecondaryObjects[iIdx3])
							{
								lAlreadyAttached = true;
								break;
							}
						}

						// Add Attachment
						if(lAlreadyAttached == false)
						{
							tag_t	*taAttachments	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
							int		*iaAttachTypes	= (int*)MEM_alloc(1 * sizeof(int));

							char	*sLatestReleaseStatusName	= NULL;
							int		iStatusCount				= 0;

							// Get lastest release status
							askLatestReleaseStatus(taSecondaryObjects[iIdx3], &sLatestReleaseStatusName, &iStatusCount);

							if(iStatusCount > 0)
							{
								// Check if latest status is 10 / 60 / [10 / 90] / [60 / 90] ... No 90 (blanc) or 98
								if(strcmp(sLatestReleaseStatusName, STATUS_TYPE_10) == 0 || strcmp(sLatestReleaseStatusName, STATUS_TYPE_60) == 0 || 
									(strcmp(sLatestReleaseStatusName, STATUS_TYPE_90) == 0 && iStatusCount > 1))
								{
									taAttachments[0] = taSecondaryObjects[iIdx3];
									iaAttachTypes[0] = EPM_target_attachment;
									ITK(EPM_add_attachments(rootTaskTag, 1, taAttachments, iaAttachTypes));
									ITK(AOM_save(rootTaskTag));
								} else {
									// Ignore
								}
							} else {
								// Ignore
							}
						}

						// Free
						SAFE_SM_FREE(pTmpTargetTags);
					}
				}

				// Free
				SAFE_SM_FREE(taSecondaryObjects);
			}
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* END SBT_AH_attach_pdf_and_document_revs */


/**
 * @brief
 * Write UID of current Workflow to file and attach it as target (-> read by sub-process)
 */
int SBT_AH_write_uid_of_current_workflow_to_file_and_attach_it(EPM_action_message_t msg)
{
	const char *theFunction			= "SBT_AH_write_uid_of_current_workflow_to_file_and_attach_it";
	int         retcode				= ITK_ok;
	tag_t       rootTaskTag			= NULLTAG;
	int			iParentProcessCount	= 0;
	tag_t		tCurrentJob			= NULLTAG;
	tag_t		txtFile				= NULLTAG;
	IMF_file_t	txtFileDesc			= NULL;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Job */
	ITK(EPM_ask_job(rootTaskTag, &tCurrentJob));

	/* Get UID of Job */
	char *sInternalIdOfJob = getInternalIdOfTcObject(tCurrentJob);

	//create a new ImanFile
	ITK(IMF_create_file(NULL, SS_TEXT, &txtFile, &txtFileDesc) );
	ITK(IMF_open_file(txtFileDesc, SS_RDWRITE) );

	// Write UID into the file
	ITK(IMF_write_file_line(txtFileDesc, sInternalIdOfJob ) );

	//Close and save the new ImanFile
	ITK(IMF_close_file(txtFileDesc) );
	ITK(AOM_save(txtFile) );
	ITK(AOM_unload(txtFile) );

	//create the dataset and upload the dataset
	tag_t tNewTextDataset = createDataset(msg.task, txtFile, "UID of Parent Task");

	// Set Status 60 on new Text Dataset
	if(tNewTextDataset != NULLTAG)
	{
		tag_t	tNewStatus			= NULLTAG;
		tag_t	*taReleaseObjects	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
		char	*sRealStatusName				= NULL;

		// Init
		taReleaseObjects[0] = tNewTextDataset;

		// Get the real name of status before creating it.
		returnStatusRealName(STATUS_TYPE_60,&sRealStatusName);

		// Create status
		ITK(CR_create_release_status(sRealStatusName, &tNewStatus) );

		// Save status
		ITK(AOM_save(tNewStatus) );

		// Add status
		ITK(EPM_add_release_status(tNewStatus, 1, taReleaseObjects, false) );
	}

	LEAVE(theFunction);
	return retcode;
} /* SBT_AH_write_uid_of_current_workflow_to_file_and_attach_it */


/**
 * @brief
 * Generate a cancel form and attach it to the workflow reference during cancelling process
 */
int SBT_AH_generate_cancel_form_and_attach_to_workflow(EPM_action_message_t msg)
{
	const char *theFunction			= "SBT_AH_generate_cancel_form_and_attach_to_workflow";
	const char *sCancelFormName		= "Annulling Information";
	int         retcode				= ITK_ok;
	tag_t       rootTaskTag			= NULLTAG;
	tag_t		tCancelForm			= NULLTAG;
	tag_t		*taAttachments		= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
	int			*iaAttachTypes		= (int*)MEM_alloc(1 * sizeof(int));

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Create new Form BT7_CancelInfo */
	ITK(FORM_create(sCancelFormName, sCancelFormName, "BT7_CancelInfo", &tCancelForm) );
	ITK(AOM_save(tCancelForm) );

	// Add Attachment
	taAttachments[0] = tCancelForm;
	iaAttachTypes[0] = EPM_reference_attachment;

	ITK(EPM_add_attachments(rootTaskTag, 1, taAttachments, iaAttachTypes));
	ITK(AOM_save(rootTaskTag));

	SAFE_SM_FREE(taAttachments);
	SAFE_SM_FREE(iaAttachTypes);

	LEAVE(theFunction);
	return retcode;
} /* SBT_AH_generate_cancel_form_and_attach_to_workflow */


/**
 * @brief
 * Transfer information and attached datasets from cancel info form to target objects
 */
int SBT_AH_transfer_cancel_form_info_to_targets(EPM_action_message_t msg)
{
	const char *theFunction			= "SBT_AH_transfer_cancel_form_info_to_targets";
	const char *sCancelFormName		= "Cancel Information";
	const char *sCanceled			= "bt7_canceled";
	const char *sDocCancelNr		= "bt7_doccancelnr";
	const char *sAttCancelNr		= "bt7_cancelnr";
	const char *sPrefDsRelation		= "BT7_CancelInfo_Dataset_Rel";
	char	   *sDsRelation			= NULL;
	date_t 	    dCanceled			= NULLDATE;
	char	   *sCancelNr			= NULL;
	int         retcode				= ITK_ok;
	tag_t       rootTaskTag			= NULLTAG;
	tag_t		tDsAttachRelType	= NULLTAG;
	tag_t		tCancelForm			= NULLTAG;
	int         iTargetCount        = 0;
	int         iReferenceCount     = 0;
	tag_t *     pTargetTags         = NULL;
	tag_t *     pReferenceTags      = NULL;
	tag_t		tCancelInfoType		= NULLTAG;
	tag_t		tRelationType		= NULLTAG;
	tag_t		*taSecondaryObjects	= NULL;
	int			iSecondaryCount		= 0;
	
	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Reference Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );

	/* Get Form Type */
	ITK(TCTYPE_find_type("BT7_CancelInfo", "Form", &tCancelInfoType) );

	/* Avoid Server crash */
	if ( tCancelInfoType == NULLTAG )
	{
		return ITK_ok;
	}

	/* Get dataset attach relation */
	PREF_ask_char_value(sPrefDsRelation, 0, &sDsRelation);
	if ( sDsRelation == NULL || strcmp(sDsRelation, "") == 0 )
	{
		sDsRelation = "IMAN_manifestation";
	}
	ITK(GRM_find_relation_type(sDsRelation, &tDsAttachRelType));

	/* Search for cancel form */
	for ( int iIdx1 = 0; iIdx1 < iReferenceCount; iIdx1++ )
	{
		logical lIsTypeCancelInfo = false;
		ITK(AOM_is_type_of(pReferenceTags[iIdx1], tCancelInfoType, &lIsTypeCancelInfo) );
		if (lIsTypeCancelInfo)
		{
			tCancelForm = pReferenceTags[iIdx1];
			break;
		}
	}

	/* Return if no Cancel Form in reference found */
	if ( tCancelForm == NULLTAG )
	{
		return ITK_ok;
	}

	/* Retrieve cancel information from form and attached datasets to targets */
	ITK(AOM_ask_value_date(tCancelForm, sCanceled, &dCanceled));
	ITK(AOM_ask_value_string(tCancelForm, sDocCancelNr, &sCancelNr));

	ITK(GRM_find_relation_type("IMAN_external_object_link", &tRelationType));
	ITK(GRM_list_secondary_objects_only(tCancelForm, tRelationType, &iSecondaryCount, &taSecondaryObjects));

	/* Return when no cancel information found */
	if( CLOCK_compare_dates(&dCanceled, &NULLDATE) == 0 &&  strcmp(sCancelNr, "") == 0 && iSecondaryCount == 0 )
	{
		return ITK_ok;
	}

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx2 = 0; iIdx2 < iTargetCount; iIdx2++ )
	{
		tag_t tClass = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx2], "Item", &tClass) == true )
		{
			tag_t	tLastestRev				= NULLTAG;
			tag_t	tPropDesc				= NULLTAG;
			tag_t	tSpRelationType			= NULLTAG;
			tag_t	tTcEntFormType			= NULLTAG;
			tag_t	tTcEntForm				= NULLTAG;
			tag_t  *taSpSecondaryObjects	= NULL;
			int		iSpSecondaryCount		= 0;

			/* Get latest revision */
			ITK(ITEM_ask_latest_rev(pTargetTags[iIdx2], &tLastestRev));
			
			if ( tLastestRev != NULLTAG )
			{
				/* Search TcEntValue for to add cancel information */
				ITK(TCTYPE_find_type("BT7_TcEntValues", "Form", &tTcEntFormType) );

				ITK(GRM_find_relation_type("IMAN_specification", &tSpRelationType));
				ITK(GRM_list_secondary_objects_only(tLastestRev, tSpRelationType, &iSpSecondaryCount, &taSpSecondaryObjects));

				/* Search for TcEntValue form */
				for ( int i = 0; i < iSpSecondaryCount; i++ )
				{
					logical lIsTypeTcEntValue = false;
					ITK(AOM_is_type_of(taSpSecondaryObjects[i], tTcEntFormType, &lIsTypeTcEntValue) );
					if (lIsTypeTcEntValue)
					{
						tTcEntForm = taSpSecondaryObjects[i];
						break;
					}
				}

				/*Set cancel information on TcEntValue form */
				if ( tTcEntForm != NULLTAG)
				{
					ITK(AOM_refresh(tTcEntForm, true) );

					if ( sCancelNr != NULL )
					{
						ITK(AOM_set_value_string(tTcEntForm, sDocCancelNr, sCancelNr) );
					}

					ITK(AOM_save(tTcEntForm) );
					ITK(AOM_refresh(tTcEntForm, false) );
				}
				
				/* Set cancelled informaiton on item*/
				ITK(AOM_refresh(pTargetTags[iIdx2], true) );

				AOM_ask_descriptor(pTargetTags[iIdx2], sCanceled, &tPropDesc);
				if ( tPropDesc != NULLTAG && CLOCK_compare_dates(&dCanceled, &NULLDATE) != 0 )
				{
					ITK(AOM_set_value_date(pTargetTags[iIdx2], sCanceled, dCanceled) );
				}

				tPropDesc = NULLTAG;

				/* Set cancel number informaiton */
				AOM_ask_descriptor(pTargetTags[iIdx2], sAttCancelNr, &tPropDesc);
				if ( tPropDesc != NULLTAG && sCancelNr != NULL )
				{
					ITK(AOM_set_value_string(pTargetTags[iIdx2], sAttCancelNr, sCancelNr) );
				}

				/* Save revision */
				ITK(AOM_save(pTargetTags[iIdx2]) );
				ITK(AOM_refresh(pTargetTags[iIdx2], false) );

				/* Attach datasets */
				for ( int iIdx3 = 0; iIdx3 < iSecondaryCount; iIdx3++ )
				{
					logical lIsDataset = false;

					if (object_is_of_class_or_desc(taSecondaryObjects[iIdx3], "Dataset", &tClass) == true)
					{
						tag_t	tNewRelation	= NULLTAG;
						tag_t	*taAttachments	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
						int		*iaAttachTypes	= (int*)MEM_alloc(1 * sizeof(int));

						/* Attache the dataset to target */
						taAttachments[0] = taSecondaryObjects[iIdx3];
						iaAttachTypes[0] = EPM_target_attachment;
						
						ITK(AOM_refresh(rootTaskTag, true));
						ITK(EPM_add_attachments(rootTaskTag, 1, taAttachments, iaAttachTypes));
						ITK(AOM_save(rootTaskTag));
						ITK(AOM_refresh(rootTaskTag, false));

						/* Find relation */
						ITK(GRM_find_relation(tLastestRev, taSecondaryObjects[iIdx3], tDsAttachRelType, &tNewRelation));

						if(tNewRelation == NULLTAG)
						{
							/* Add relation */
							ITK(GRM_create_relation(tLastestRev, taSecondaryObjects[iIdx3], tDsAttachRelType, NULLTAG, &tNewRelation));

							/* Save relation */
							ITK(AOM_save(tNewRelation));
						}

						SAFE_SM_FREE(taAttachments);
						SAFE_SM_FREE(iaAttachTypes);
					}
				}

			}
	
		}
	}

	//SAFE_SM_FREE(dCanceled);
	SAFE_SM_FREE(sCancelNr);
	SAFE_SM_FREE(pTargetTags);
	SAFE_SM_FREE(pReferenceTags);

	LEAVE(theFunction);
	return retcode;

} /* SBT_AH_transfer_cancel_form_info_to_targets */


/**
 * @brief
 * Assign the responsible party of the parent process to the current task
 */
int SBT_AH_assign_responsible_party_of_parent_workflow(EPM_action_message_t msg)
{
	const char *theFunction			= "SBT_AH_assign_responsible_party_of_parent_workflow";
	int         retcode				= ITK_ok;
	tag_t       rootTaskTag			= NULLTAG;
	int			iParentProcessCount	= 0;
	tag_t		tCurrentJob			= NULLTAG;
	int			iReferenceAttachements	= 0;
	tag_t		*taReferenceAttachments	= NULL;
	tag_t		tParentProcess			= NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Reference Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceAttachements, &taReferenceAttachments));

	/* Loop reference Attachments*/
	for(int iIdx1 = 0; iIdx1 < iReferenceAttachements; iIdx1++)
	{
		char sReferenceType[WSO_name_size_c+1];
		char sReferenceName[WSO_name_size_c+1];

		// Is Text-Dataset with name "UID of Parent Task"?
		ITK(WSOM_ask_object_type(taReferenceAttachments[iIdx1], sReferenceType));
		ITK(WSOM_ask_name(taReferenceAttachments[iIdx1], sReferenceName));

		if(strcmp(sReferenceType, "Text") == 0 && strcmp(sReferenceName, "UID of Parent Task") == 0)
		{
			AE_reference_type_t ae_reftype = AE_BAD_REF_TYPE;
			IMF_file_t file_descriptor = NULL;
			tag_t file_tag = NULLTAG;
   
			ITK(AE_ask_dataset_named_ref(taReferenceAttachments[iIdx1], "Text", &ae_reftype, &file_tag));

			if(file_tag != NULLTAG)
			{
				ITK(AOM_refresh(file_tag, FALSE));
				ITK(IMF_ask_file_descriptor(file_tag, &file_descriptor));

				if(file_descriptor != NULL)
				{
					ITK(IMF_open_file(file_descriptor, SS_RDONLY));

					char sParentProcessUID[50] = "";
					ITK(IMF_read_file_line(file_descriptor, sParentProcessUID));

					if(strcmp(sParentProcessUID, "") != 0)
					{
						std::string strParentProcessUID = sParentProcessUID;

						if(strParentProcessUID.length() > 14)
						{
							strParentProcessUID = strParentProcessUID.substr(0, 14);
						}

						if(strParentProcessUID.length() == 14)
						{
							ITK__convert_uid_to_tag(strParentProcessUID.c_str(), &tParentProcess);
						}
					}

					ITK(IMF_close_file(file_descriptor));                   
				}
				ITK(AOM_refresh(file_tag, FALSE));
				ITK(AOM_unload(file_tag));
			}
		}
	}

	/* Get Job */
	tag_t tJob = NULLTAG;
	ITK(EPM_ask_job(rootTaskTag, &tJob));


	if(tParentProcess != NULLTAG)
	{
		tag_t	tParentRootTask			= NULLTAG;
		tag_t	tParentResponsibleParty	= NULLTAG;

		// Get Root Task
		ITK(EPM_ask_root_task(tParentProcess, &tParentRootTask));

		int		iSubTaskCount	= 0;
		tag_t	*taSubTasks		= NULL;
		ITK(EPM_ask_sub_tasks(tParentRootTask, &iSubTaskCount, &taSubTasks));
		for(int iIdx2 = 0; iIdx2 < iSubTaskCount; iIdx2++)
		{
			EPM_state_t wfState;
			ITK(EPM_ask_state(taSubTasks[iIdx2], &wfState) );
			if(wfState == EPM_started)
			{
				ITK(EPM_ask_responsible_party(taSubTasks[iIdx2], &tParentResponsibleParty));
				break;
			}
		}

		// If active parent task not found, use owner of parent workflow (-> warning: not working for CCR!)
		if(tParentResponsibleParty == NULLTAG)
		{
			tag_t tParentProcessOwningUser = NULLTAG;
			tag_t tParentProcessOwningGroup = NULLTAG;
			ITK(POM_ask_owner(tParentRootTask, &tParentProcessOwningUser, &tParentProcessOwningGroup));
			
			tParentResponsibleParty = tParentProcessOwningUser;
		}

		if(tParentResponsibleParty != NULLTAG)
		{
			// Assign Responsible Party
			ITK(EPM_assign_responsible_party(msg.task, tParentResponsibleParty));
			ITK(AOM_save(msg.task) );
			ITK(AOM_refresh(msg.task, true) );
		} else {
			printf(" > SBT_AH_assign_responsible_party_of_parent_workflow : No parent process identified \n");
		}

		// Issue 1047. Workflow crashes the server.
		// Unlock Parent root task
		char *sTaskName = NULL;
		ITK(AOM_ask_name(tParentRootTask, &sTaskName));
		if ( sTaskName != NULL && strcmp(sTaskName, "Glob - 10 Release for Prototype") != 0 
							   && strcmp(sTaskName, "BT7_AddElectronic3dRepresentation") != 0 )
		{
			ITK(AOM_unlock(tParentRootTask));
		}
		/* Free */
		SAFE_SM_FREE(sTaskName);
	}

	LEAVE(theFunction);
	return retcode;
} /* END SBT_AH_assign_responsible_party_of_parent_workflow */


/**
 * @brief
 * Attach the datasets from the WF-References to the Item Revision in the WF-Targets and set the appropriate status
 */
int SBT_AH_add_dataset_and_set_status(EPM_action_message_t msg)
{
	const char *theFunction			= "SBT_AH_add_dataset_and_set_status";
	int         retcode				= ITK_ok;
	int         iTargetCount		= 0;
	tag_t       rootTaskTag			= NULLTAG;
	tag_t *     pTargetTags			= NULL;
	tag_t		tItemRevision		= NULLTAG;

	tag_t			tRelTypeImanSpecification	= NULLTAG;
	tag_t			tRelTypeImanManifestation	= NULLTAG;
	
	// Find relation type
	ITK(GRM_find_relation_type("IMAN_specification", &tRelTypeImanSpecification));
	ITK(GRM_find_relation_type("IMAN_manifestation", &tRelTypeImanManifestation));

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClass = NULLTAG;

		// Is Item Revision
		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true )
		{
			tItemRevision = pTargetTags[iIdx1];
			break;
		}
	}

	// Target Item Revision found?
	if(tItemRevision != NULLTAG)
	{
		int		iSecondaryCountManifestation		= 0;
		tag_t	*taSecondaryObjectsManifestation	= NULL;

		// Find manifestation PDFs
		ITK(GRM_list_secondary_objects_only(tItemRevision, tRelTypeImanManifestation, &iSecondaryCountManifestation, &taSecondaryObjectsManifestation));

		/* Loop over all reference attachments */
		for ( int iIdx1 = 0; iIdx1 < iSecondaryCountManifestation; iIdx1++ )
		{
			char	sObjectType[WSO_name_size_c+1];

			// Is PDF
			ITK(WSOM_ask_object_type(taSecondaryObjectsManifestation[iIdx1], sObjectType));
			if ( strcmp(sObjectType, "PDF") == 0)
			{
				tag_t tCurrentPDF = taSecondaryObjectsManifestation[iIdx1];

				// Rename Dataset (-> not required any more)
				{
					char sObjectName[WSO_name_size_c + 1];
					std::string strObjectName;

					// Get Name
					ITK(WSOM_ask_name(tCurrentPDF, sObjectName));
					strObjectName = sObjectName;

					// Transform to Upper Case
					std::transform(strObjectName.begin( ), strObjectName.end( ), strObjectName.begin( ), ::toupper);

					// Cut suffix required ?
					if(strObjectName.length() > 4 && strObjectName.substr(strObjectName.length() - 4, 4).compare(".PDF") == 0)
					{
						logical lisCheckedOut = false;

						// Reset to real name
						strObjectName = sObjectName;

						// Cut last 4 chars
						strObjectName = strObjectName.substr(0, strObjectName.length() - 4);

						// Update Dataset Name
						ITK(WSOM_set_name(tCurrentPDF, strObjectName.c_str()));

						// Is checked-out
						ITK(RES_is_checked_out(tCurrentPDF, &lisCheckedOut));

						// Checkin
						if(lisCheckedOut == true)
						{
							ITK(RES_checkin(tCurrentPDF));
						}

						// Save
						ITK(AOM_save(tCurrentPDF));
					}
				}

				// Replace / Attach
				{
					char			sNewObjectName[WSO_name_size_c + 1];
					std::string		strObjectName;
					tag_t			*taSecondaryObjects		= NULL;
					int				iSecondaryCount			= 0;

					// get name of the new dataset
					ITK(WSOM_ask_name(tCurrentPDF, sNewObjectName));

					// get existing datasets
					ITK(GRM_list_secondary_objects_only(tItemRevision, tRelTypeImanSpecification, &iSecondaryCount, &taSecondaryObjects));

					// loop existing datasets
					for ( int iIdx2 = 0; iIdx2 < iSecondaryCount; iIdx2++ )
					{
						char sExistingObjectName[WSO_name_size_c + 1];
						char sExistingObjectType[WSO_name_size_c + 1];

						// get name of existing dataset
						ITK(WSOM_ask_name(taSecondaryObjects[iIdx2], sExistingObjectName));

						// get type of existing dataset
						ITK(WSOM_ask_object_type(taSecondaryObjects[iIdx2], sExistingObjectType));

						// cut required?
						if(strcmp(sExistingObjectName, sNewObjectName) == 0 && strcmp(sExistingObjectType, "PDF") == 0)
						{
							tag_t tExistingRelation = NULLTAG;

							// Find relation
							ITK(GRM_find_relation(tItemRevision, taSecondaryObjects[iIdx2], tRelTypeImanSpecification, &tExistingRelation));

							if(tExistingRelation != NULLTAG)
							{
								// Delete relation --> TODO: Delete existing dataset
								ITK(GRM_delete_relation(tExistingRelation));
							}
						}
					}
				}

				// Attach Dataset
				{
					tag_t	tNewRelation				= NULLTAG;

					// Find relation
					ITK(GRM_find_relation(tItemRevision, tCurrentPDF, tRelTypeImanSpecification, &tNewRelation));

					if(tNewRelation == NULLTAG)
					{
						// Add relation
						ITK(GRM_create_relation(tItemRevision, tCurrentPDF, tRelTypeImanSpecification, NULLTAG, &tNewRelation));

						// Save relation
						ITK(AOM_save(tNewRelation));
					}
				}

				// Set status
				{
					char	*sStatusName				= NULL;
					int		iStatusCount				= 0;

					askLatestReleaseStatus(tItemRevision, &sStatusName, &iStatusCount);

					if(iStatusCount > 0)
					{
						tag_t	tNewStatus			= NULLTAG;
						tag_t	*taReleaseObjects	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
						date_t	dateItemRevisionDateReleased;
						char	*sRealStatusName	= NULL;

						// Init
						taReleaseObjects[0] = tCurrentPDF;

						// Get the real name of status before creating it.
						returnStatusRealName(sStatusName,&sRealStatusName);

						// Create status
						ITK(CR_create_release_status(sRealStatusName, &tNewStatus) );

						// Copy release date
						ITK(AOM_ask_value_date(tItemRevision, "date_released", &dateItemRevisionDateReleased));
						ITK(AOM_set_value_date(tNewStatus, "date_released", dateItemRevisionDateReleased));

						// Save status
						ITK(AOM_save(tNewStatus) );

						// Add status
						ITK(EPM_add_release_status(tNewStatus, 1, taReleaseObjects, false) );
					}

					// Free
					SAFE_SM_FREE(sStatusName);
				}

				// Delete Manifestation Relation
				{
					tag_t	tCurrentManifestationRelation	= NULLTAG;

					// Find relation
					ITK(GRM_find_relation(tItemRevision, tCurrentPDF, tRelTypeImanManifestation, &tCurrentManifestationRelation));

					// Delete relation
					ITK(GRM_delete_relation(tCurrentManifestationRelation));
				}
			}
		}

		// Free
		SAFE_SM_FREE(taSecondaryObjectsManifestation);
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* END SBT_AH_add_dataset_and_set_status */

logical lCurrentUserFromRastatt()
{
	logical	lRastattUser			= false;
	tag_t	tCurrentGroupMember		= NULLTAG;
	tag_t	tCurrentUser			= NULLTAG;
	tag_t	tCurrentPerson			= NULLTAG;
	char	*sPersonCity			= NULL;

	// Get current Person
	ITK(SA_ask_current_groupmember(&tCurrentGroupMember));
	ITK(SA_ask_groupmember_user(tCurrentGroupMember, &tCurrentUser));
	ITK(SA_ask_user_person(tCurrentUser, &tCurrentPerson));
	ITK(SA_ask_person_attr(tCurrentPerson, "PA2", &sPersonCity));

	// Set return value
	lRastattUser = strcmp(sPersonCity, "Rastatt") == 0;

	// Free
	SAFE_SM_FREE(sPersonCity);

	return lRastattUser;
}

void specified_Cleanup(tag_t tCurrentTarget, tag_t tRelationType, int iRelCountSecondarySpecified, tag_t *taRelSecondarySpecified, logical lIsSoftware)
{
	vector<tag_t> vecRemoveSpecifiedRevisions;
	vector<tag_t> vecKeepSpecifiedRevisions;

	// Identify obsolete relations
	for ( int iIdx2 = 0; iIdx2 < iRelCountSecondarySpecified; iIdx2++ )
	{
		char	*sLatestSpecifiesStatus	= NULL;
		int		iSpecifiesStatusCount	= 0;

		// Ask latest status
		askLatestReleaseStatus(taRelSecondarySpecified[iIdx2], &sLatestSpecifiesStatus, &iSpecifiesStatusCount);

		if(iSpecifiesStatusCount > 0)
		{
			// Special treatment for Software + Programming Data
			if(lIsSoftware == true)
			{
				// Software
				tag_t tSpecifiesItem				= NULLTAG;
				logical	lAlreadyInKeepVector		= false;
				int		iTmpSpecRevCount			= 0;
				tag_t	*taTmpSpecRevs				= NULL;
				tag_t	tTmpLatestAttachedProgData	= NULLTAG;

				// Get Item
				ITK(ITEM_ask_item_of_rev(taRelSecondarySpecified[iIdx2], &tSpecifiesItem));

				// Get all ItemRevisions
				ITK(ITEM_list_all_revs(tSpecifiesItem, &iTmpSpecRevCount, &taTmpSpecRevs));

				// Loop Tmp Revisions backwards -> First match with related objects is the latest attached revision
				for ( int iIdx3 = iTmpSpecRevCount-1; iIdx3 >= 0; iIdx3-- )
				{
					// Is Programming Data?
					if(isProgrammingData(taTmpSpecRevs[iIdx3]))
					{
						// Match?
						for ( int iIdx4 = 0; iIdx4 < iRelCountSecondarySpecified; iIdx4++ )
						{
							if(taTmpSpecRevs[iIdx3] == taRelSecondarySpecified[iIdx4])
							{
								tTmpLatestAttachedProgData = taTmpSpecRevs[iIdx3];
								break;
							}
						}

						// Break
						if(tTmpLatestAttachedProgData != NULLTAG)
						{
							break;
						}
					}
				}

				// Keep latest revision (if exist)
				for ( int iIdx3 = 0; iIdx3 < vecKeepSpecifiedRevisions.size(); iIdx3++ )
				{
					if(vecKeepSpecifiedRevisions.at(iIdx3) == tTmpLatestAttachedProgData)
					{
						lAlreadyInKeepVector = true;
						break;
					}
				}

				if(lAlreadyInKeepVector == false)
				{
					vecKeepSpecifiedRevisions.push_back(tTmpLatestAttachedProgData);
				}

				// Remove current revision (if not in keep-vector) + if Programming Data
				if(isProgrammingData(taRelSecondarySpecified[iIdx2]))
				{
					vecRemoveSpecifiedRevisions.push_back(taRelSecondarySpecified[iIdx2]);
				}

				// Free
				SAFE_SM_FREE(taTmpSpecRevs);
			}

			//
			// Handle remaining objects (Software + ProgrammingData is handled above)
			//
			if((lIsSoftware == false || isProgrammingData(taRelSecondarySpecified[iIdx2]) == false) &&
				strcmp(sLatestSpecifiesStatus, STATUS_TYPE_90) == 0)
			{
				//
				// Latest status = 90
				//

				// Add to remove-vector, unless the revision is the latest attached revision
				{
					tag_t	tSpecifiesItem			= NULLTAG;
					int		iTmpSpecRevCount		= 0;
					tag_t	*taTmpSpecRevs			= NULL;
					tag_t	tLatestAttachedRevision	= NULLTAG;

					// Get Item
					ITK(ITEM_ask_item_of_rev(taRelSecondarySpecified[iIdx2], &tSpecifiesItem));

					// Get all ItemRevisions
					ITK(ITEM_list_all_revs(tSpecifiesItem, &iTmpSpecRevCount, &taTmpSpecRevs));

					// Loop Tmp Revisions backwards -> First match with related objects is the latest attached revision
					for ( int iIdx3 = iTmpSpecRevCount-1; iIdx3 >= 0; iIdx3-- )
					{
						// Match?
						for ( int iIdx4 = 0; iIdx4 < iRelCountSecondarySpecified; iIdx4++ )
						{
							if(taTmpSpecRevs[iIdx3] == taRelSecondarySpecified[iIdx4])
							{
								tLatestAttachedRevision = taTmpSpecRevs[iIdx3];
								break;
							}
						}

						if(tLatestAttachedRevision != NULLTAG)
						{
							break;
						}
					}

					char rev_id1[ITEM_id_size_c + 1];
					char rev_id2[ITEM_id_size_c + 1];
					ITEM_ask_rev_id(taRelSecondarySpecified[iIdx2], rev_id1);
					ITEM_ask_rev_id(tLatestAttachedRevision, rev_id2);

					// Add current status 90 revision to the remove vector, if it is not the latest one
					if(taRelSecondarySpecified[iIdx2] != tLatestAttachedRevision)
					{
						vecRemoveSpecifiedRevisions.push_back(taRelSecondarySpecified[iIdx2]);
					}

					// Free
					SAFE_SM_FREE(taTmpSpecRevs);
				}
			}
		}

		// Free
		SAFE_SM_FREE(sLatestSpecifiesStatus);
	}

	// Remove obsolete specifies relations
	for ( int iIdx2 = 0; iIdx2 < vecRemoveSpecifiedRevisions.size(); iIdx2++ )
	{
		logical lKeepItemRevision = false;

		for ( int iIdx3 = 0; iIdx3 < vecKeepSpecifiedRevisions.size(); iIdx3++ )
		{
			if(vecKeepSpecifiedRevisions.at(iIdx3) == vecRemoveSpecifiedRevisions.at(iIdx2))
			{
				lKeepItemRevision = true;
				break;
			}
		}

		if(lKeepItemRevision == false)
		{
			// Delete Relation
			deleteRelation(tCurrentTarget, vecRemoveSpecifiedRevisions.at(iIdx2), tRelationType);
		}
	}
}

void concerned_Cleanup(tag_t tCurrentTarget, tag_t tRelationType, int iRelCountSecondaryConcerned, tag_t *taRelSecondaryConcerned)
{
	vector<tag_t> vecRemoveConcernsRevisions;
	// Identify obsolete relations
	for ( int iIdx2 = 0; iIdx2 < iRelCountSecondaryConcerned; iIdx2++ )
	{
		char	*sLatestConcernsStatus	= NULL;
		int		iConcernsStatusCount	= 0;

		// Ask latest status
		askLatestReleaseStatus(taRelSecondaryConcerned[iIdx2], &sLatestConcernsStatus, &iConcernsStatusCount);

		if(iConcernsStatusCount > 0)
		{
			// No differentiation between Software and non-Software ...

			if(strcmp(sLatestConcernsStatus, STATUS_TYPE_90) == 0)
			{
				// latest status = 90


				// Add to remove-vector, unless the revision is the latest attached revision
				{
					tag_t	tConcernsItem			= NULLTAG;
					int		iTmpConsRevCount		= 0;
					tag_t	*taTmpConsRevs			= NULL;
					tag_t	tLatestAttachedRevision	= NULLTAG;

					// Get Item
					ITK(ITEM_ask_item_of_rev(taRelSecondaryConcerned[iIdx2], &tConcernsItem));

					// Get all ItemRevisions
					ITK(ITEM_list_all_revs(tConcernsItem, &iTmpConsRevCount, &taTmpConsRevs));

					// Loop Tmp Revisions backwards -> First match with related objects is the latest attached revision
					for ( int iIdx3 = iTmpConsRevCount-1; iIdx3 >= 0; iIdx3-- )
					{
						// Match?
						for ( int iIdx4 = 0; iIdx4 < iRelCountSecondaryConcerned; iIdx4++ )
						{
							if(taTmpConsRevs[iIdx3] == taRelSecondaryConcerned[iIdx4])
							{
								tLatestAttachedRevision = taTmpConsRevs[iIdx3];
								break;
							}
						}

						if(tLatestAttachedRevision != NULLTAG)
						{
							break;
						}
					}

					char rev_id1[ITEM_id_size_c + 1];
					char rev_id2[ITEM_id_size_c + 1];
					ITEM_ask_rev_id(taRelSecondaryConcerned[iIdx2], rev_id1);
					ITEM_ask_rev_id(tLatestAttachedRevision, rev_id2);

					// Add current status 90 revision to the remove vector, if it is not the latest one
					if(taRelSecondaryConcerned[iIdx2] != tLatestAttachedRevision)
					{
						vecRemoveConcernsRevisions.push_back(taRelSecondaryConcerned[iIdx2]);
					}

					// Free
					SAFE_SM_FREE(taTmpConsRevs);
				}
			}
		}

		// Free
		SAFE_SM_FREE(sLatestConcernsStatus);
	}

	// Remove obsolete concerns relations
	for ( int iIdx2 = 0; iIdx2 < vecRemoveConcernsRevisions.size(); iIdx2++ )
	{
		// Delete Relation
		deleteRelation(tCurrentTarget, vecRemoveConcernsRevisions.at(iIdx2), tRelationType);

		// TODO: Refresh of the Content of the Pseudo-Folder
	}
}

/**
 * @brief
 * Cleanup specifies / concerns relations
 */
int SBT_AH_cleanup_relations(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_attach_item_detach_revision";
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t *     pTargetTags        = NULL;
	tag_t		tRelTypeConcerned		= NULLTAG;
	tag_t		tRelTypeSpecified		= NULLTAG;
	tag_t		tRelTypeIncluded		= NULLTAG;
	tag_t		tRelTypeSupplierSpec	= NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	// Exit if "Zug-Logic" is not applicable for the current user
	if(lCurrentUserFromRastatt())
	{
		return ITK_ok;
	}

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	// General Preparation
	ITK(GRM_find_relation_type("BT7_Rel_Concerned", &tRelTypeConcerned));
	ITK(GRM_find_relation_type("BT7_Rel_Specified", &tRelTypeSpecified));
	ITK(GRM_find_relation_type("BT7_Includes", &tRelTypeIncluded));
	ITK(GRM_find_relation_type("BT7_SUPPLIERSPEC", &tRelTypeSupplierSpec));

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClass = NULLTAG;

		// Is Item Revision
		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true )
		{
			logical	lIsSoftware					= false;
			char    sObjectType[WSO_name_size_c+1];
			char	*sLatestTargetReleaseStatus	= NULL;
			int		iTargetStatusCount			= 0;

			// Is Software
			lIsSoftware = isSoftware(pTargetTags[iIdx1]);

			// Ask latest status
			askLatestReleaseStatus(pTargetTags[iIdx1], &sLatestTargetReleaseStatus, &iTargetStatusCount);

			// Only WIP->10 || WIP->60 || 5->10 || 5->60
			if(iTargetStatusCount == 0 || (iTargetStatusCount == 1 && strcmp(sLatestTargetReleaseStatus, STATUS_TYPE_5) == 0))
			{
				int		iRelCountSecondarySpecified	= 0;
				int		iRelCountSecondaryIncluded	= 0;
				int		iRelCountSecondarySupplierSpec	= 0;
				int		iRelCountSecondaryConcerned	= 0;

				tag_t	*taRelSecondarySpecified	= NULL;
				tag_t	*taRelSecondaryIncluded		= NULL;
				tag_t	*taRelSecondarySupplierSpec	= NULL;
				tag_t	*taRelSecondaryConcerned	= NULL;


				// Get Secondary Objects
				ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeSpecified, &iRelCountSecondarySpecified, &taRelSecondarySpecified));
				ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeIncluded, &iRelCountSecondaryIncluded, &taRelSecondaryIncluded));
				ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeSupplierSpec, &iRelCountSecondarySupplierSpec, &taRelSecondarySupplierSpec));
				ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeConcerned, &iRelCountSecondaryConcerned, &taRelSecondaryConcerned));

				////////////////////////////////////////////////////////////////////////

				// Specified Relations
				specified_Cleanup(pTargetTags[iIdx1], tRelTypeSpecified, iRelCountSecondarySpecified, taRelSecondarySpecified, lIsSoftware);

				// Included Relations
				specified_Cleanup(pTargetTags[iIdx1], tRelTypeIncluded, iRelCountSecondaryIncluded, taRelSecondaryIncluded, lIsSoftware);

				// Supplier Specification Relations
				specified_Cleanup(pTargetTags[iIdx1], tRelTypeSupplierSpec, iRelCountSecondarySupplierSpec, taRelSecondarySupplierSpec, lIsSoftware);

				// Concers Relation
				concerned_Cleanup(pTargetTags[iIdx1], tRelTypeConcerned, iRelCountSecondaryConcerned, taRelSecondaryConcerned);

				////////////////////////////////////////////////////////////////////////

				// Free
				SAFE_SM_FREE(taRelSecondarySpecified);
				SAFE_SM_FREE(taRelSecondaryIncluded);
				SAFE_SM_FREE(taRelSecondarySupplierSpec);
				SAFE_SM_FREE(taRelSecondaryConcerned);
			}

			// Free
			SAFE_SM_FREE(sLatestTargetReleaseStatus);
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
}


/**
 * @brief
 * Attach the Item(s) of the Revision(s) + Detach the Revision(s)
 */
int SBT_AH_attach_item_detach_revision(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_attach_item_detach_revision";
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t *     pTargetTags        = NULL;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClass = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true )
		{
			tag_t	tItem			= NULLTAG;
			tag_t	*taAttachments	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
			int		*iaAttachTypes	= (int*)MEM_alloc(1 * sizeof(int));

			// Get Item of Revision
			ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tItem));

			// Add Attachment
			taAttachments[0] = tItem;
			iaAttachTypes[0] = EPM_target_attachment;
			ITK(EPM_add_attachments(rootTaskTag, 1, taAttachments, iaAttachTypes));
			ITK(AOM_save(rootTaskTag));

			// Remove Attachment
			taAttachments[0] = pTargetTags[iIdx1];
			ITK(EPM_remove_attachments(rootTaskTag, 1, taAttachments));
			ITK(AOM_save(rootTaskTag));
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* END SBT_AH_attach_item_detach_revision*/


/**
 * @brief
 * Send notification including target details and release Statuses with release dates.
 */
int SBT_AH_send_notification(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_send_notification";
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	int         iSignoffCount      = 0;
	tag_t *     taSignoffs         = NULL;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       tCommonCompItem    = NULLTAG;
	tag_t       tCommonCompItemRev = NULLTAG;
	tag_t *     pTargetTags        = NULL;
	int			iCurrentSiteId	   = 0;
	tag_t		tCurrentSite	   = NULLTAG; 
	char		sSitename[SA_site_size_c + 1];

	// Message Attributes
	char              sParamSubject[128] = "";
	char              sParamMessage[240] = "";

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );
	
	// Get site information
	ITK(POM_site_id(&iCurrentSiteId));
	ITK(SA_find_site_by_id(iCurrentSiteId, &tCurrentSite));
	ITK(SA_ask_site_info(tCurrentSite, sSitename, &iCurrentSiteId));
	std::string sLowerSiteName = sSitename;
	// Transform Site Name to lower case
	std::transform(sLowerSiteName.begin( ), sLowerSiteName.end( ), sLowerSiteName.begin( ), ::tolower);

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Read Handler Arguments */
	int iArgCount = IMAN_number_of_arguments(msg.arguments);

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Transform Param Name to lower case
		std::transform(strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower);
		
		// Get Param Value
		if ( strTmpParamName.compare("subject") == 0 )
		{
			if (tc_strstr (sLowerSiteName.c_str( ), "test") != NULL)
			{	
				strcpy(sParamSubject, "Teamcenter: Test - " );
			}
			else if(tc_strstr (sLowerSiteName.c_str( ), "dev") != NULL)
			{
				strcpy(sParamSubject, "Teamcenter: Dev - " );
			}
			else
			{
				strcpy(sParamSubject, "Teamcenter: Prod - " );
			}

			strcat(sParamSubject, strTmpParamValue.c_str( ) );
		}
		else if ( strTmpParamName.compare("message") == 0 )
		{
			strcpy(sParamMessage, strTmpParamValue.c_str( ) );
		}
	}

	// Parameters correct?
	if ( ( strcmp(sParamSubject, "") == 0 ) || ( strcmp(sParamMessage, "") == 0 ) )
	{
		printf(" > ERROR: The ActionHandler 'SBT_AH_send_notification' requires the arguments 'subject' and 'message' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919713) );
		return 919713;
	}

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		std::stringstream ssMessage;
		tag_t tClass = NULLTAG;

		// Assign Message-Prefix
		ssMessage << sParamMessage << endl;

		// Only send out messages for Item Revisions
		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true )
		{
			tag_t             tTmpTargetItem  = NULLTAG;
			tag_t *           taTmpStatus     = NULL;
			int               iTmpStatusCount = 0;
			char              sObjectType[WSO_name_size_c + 1];
			char              sObjectName[WSO_name_size_c + 1];
			char              sItemID[ITEM_id_size_c + 1];
			char              sRevID[ITEM_id_size_c + 1];
			std::stringstream ssStatus;
			
			// Get Information from target
			ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tTmpTargetItem) );
			ITK(ITEM_ask_id(tTmpTargetItem, sItemID) );
			ITK(ITEM_ask_rev_id(pTargetTags[iIdx1], sRevID) );
			ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );
			ITK(WSOM_ask_name(pTargetTags[iIdx1], sObjectName) );

			// Get Status-Names
			ITK(WSOM_ask_release_status_list(pTargetTags[iIdx1], &iTmpStatusCount, &taTmpStatus) );

			for ( int iIdx2 = iTmpStatusCount -1; iIdx2 >= 0; iIdx2-- )
			{
				char *sTmpStatusName = NULL;
				char *sTmpStatusDate = NULL;

				ITK(AOM_UIF_ask_value(taTmpStatus[iIdx2], "name", &sTmpStatusName) );
				ITK(AOM_UIF_ask_value(taTmpStatus[iIdx2], "date_released", &sTmpStatusDate) );

				if ( ssStatus.str( ).size( ) == 0 )
				{
					if(strcmp(sTmpStatusName,STATUS_TYPE_10) == 0)
					{
						ssStatus << "PR on " <<sTmpStatusDate;
					}
					else if(strcmp(sTmpStatusName,STATUS_TYPE_60) == 0)
					{
						ssStatus << "SR on " <<sTmpStatusDate;
					}
					else if(strcmp(sTmpStatusName,STATUS_TYPE_90) == 0)
					{
						ssStatus << "OS on " <<sTmpStatusDate;
					}
					else if(strcmp(sTmpStatusName,STATUS_TYPE_98) == 0)
					{
						ssStatus << "Annulled on " <<sTmpStatusDate;
					}
					else 
					{
						ssStatus << sTmpStatusName << " on " <<sTmpStatusDate;
					}
				}

				SAFE_SM_FREE(sTmpStatusName);
				SAFE_SM_FREE(sTmpStatusDate);
			}

			if ( ssStatus.str( ).size( ) == 0 )
			{
				ssStatus << "-";
			}

			// Assign Message
			ssMessage << "> " << sItemID << "/" << sRevID << " - " << sObjectName << " ( status : " << ssStatus.str( ) << " )" << endl;
			ssMessage << endl;

			// Append the link of the target items in the email
			char*   url = NULL;

			// Change 15.10.2015: Pref of Mailserver is not set in some Sites.
			int iPrefCount = 0;
			ITK(PREF_ask_value_count("WEB_default_site_server", &iPrefCount));
			if ( iPrefCount > 0 )
			{
				ITK(TC_tag_to_url(pTargetTags[iIdx1], AWC, &url));
			}
			
			if ( url != NULL )
			{
				ssMessage << "Target Object: " << url << endl;
			}
			else
			{
				TRACE1("url is NULL for tag %i", pTargetTags[iIdx1]);
			}

			MEM_free(url);


			// Fill ignore infomation
			ssMessage 
			<< endl << "**********"
			<< endl << "Please ignore Rich Client Link"
			<< endl << "**********"
			<< endl << endl;

			/* Assign Message 2 (?) */
			std::string strTmpMessage = ssMessage.str( );

			if ( strlen(strTmpMessage.c_str( ) ) > 239 )
			{
				strTmpMessage = strTmpMessage.substr(0, 239);
			}

			// Get Distribution List and send mail
			{
				tag_t	tParticipantType	= NULLTAG;
				tag_t	*taParticipants		= NULL;
				int		iParticipantCount	= 0;
				tag_t	tMail				= NULLTAG;

				// Get Paricipant-Type
				ITK(EPM_get_participanttype("BT7_DistributionList", &tParticipantType));

				// Get Participant
				int iParticipantsOk = ITEM_rev_ask_participants(pTargetTags[iIdx1], tParticipantType, &iParticipantCount, &taParticipants); 

				// Remove all warnings
				ITK(EMH_clear_errors());
				
				// Participants ok ?
				if(iParticipantsOk == ITK_ok)
				{
					// Create Mail-Object
					createMail(sParamSubject, strTmpMessage.c_str(), &tMail );

					// Traverse participants
					for ( int iIdx2 = 0; iIdx2 < iParticipantCount; iIdx2++ )
					{
						tag_t tAssignee = NULLTAG;

						ITK(AOM_ask_value_tag(taParticipants[iIdx2], "assignee", &tAssignee));
						addReceiverToMail(tMail, tAssignee);
					}

					// Send Mail
					sendEmailNotification(tMail);
				} else {
					char *sErrorText = NULL;

					// Get Error-Text
					ITK(EMH_ask_error_text(iParticipantsOk, &sErrorText));

					// Print Error-Text
					printf(" > ERROR: ITEM_rev_ask_participants return with status %i \n", iParticipantsOk);
					printf(" > ERROR-Text: %s", sErrorText);

					// Free
					SAFE_SM_FREE(sErrorText);
				}

				// Free
				SAFE_SM_FREE(taParticipants);
			}			
		}

		// Clear message
		ssMessage.clear();
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);
	SAFE_SM_FREE(taSignoffs);

	// Clear Error Stack
	ITK(EMH_clear_errors());

	LEAVE(theFunction);
	return retcode;
} /* SBT_AH_send_notification */


/**
 * @brief
 * Assign the workflow targets to the project defined in the parameter
 */
int SBT_AH_assign_to_project(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_assign_to_project";
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       tCommonCompItem    = NULLTAG;
	tag_t       tCommonCompItemRev = NULLTAG;
	tag_t *     pTargetTags        = NULL;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Read Handler Arguments */
	int iArgCount = IMAN_number_of_arguments(msg.arguments);

	char sParamProjectId[WSO_name_size_c + 1] = "";

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Transform Param Name to lower case
		std::transform(strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower);

		// Get Param Value
		if ( strTmpParamName.compare("project_id") == 0 )
		{
			strcpy(sParamProjectId, strTmpParamValue.c_str( ) );
		}
	}

	if ( strcmp(sParamProjectId, "") == 0 )
	{
		printf(" > ERROR: The ActionHandler 'SBT_AH_assign_to_project' requires the argument 'project_id' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919712) );
		return 919712;
	}

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClass = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "Item", &tClass) )
		{
			tag_t tProject = NULLTAG;

			// Find project
			ITK(PROJ_find(sParamProjectId, &tProject) );

			// Prepare assignment of the current workflow-target to the project
			tag_t *taProjects      = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );
			tag_t *taAssignObjects = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );

			taProjects[0]      = tProject;
			taAssignObjects[0] = pTargetTags[iIdx1];

			// Assign to project
			ITK(PROJ_assign_objects(1, taProjects, 1, taAssignObjects) );
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_assign_to_project*/


/**
 * @brief
 * Set a release status only to specific object-types within the WF-targets and -references
 */
int SBT_AH_set_status_for_object_type(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_set_status_for_object_type";
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       tCommonCompItem    = NULLTAG;
	tag_t       tCommonCompItemRev = NULLTAG;
	tag_t *     pTargetTags        = NULL;
	tag_t 		tRevClassTag 	   = NULLTAG;
	char		RealNameStatus	   = NULL;
	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}
	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
	char sParamObjectType[WSO_name_size_c + 1]   = "";
	char sParamReferenceType[WSO_name_size_c + 1]   = "";
	char sParamReleaseStatus[ITEM_id_size_c + 1] = "";

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Transform Param Name to lower case
		std::transform(strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower);

		// Transform Param Value to lower case
		// MRT- commenting below line to avoid converting status name to lower case
		//std::transform(strTmpParamValue.begin( ), strTmpParamValue.end( ), strTmpParamValue.begin( ), ::tolower);

		// Get Param Value
		if ( strTmpParamName.compare("object_type") == 0 )
		{
			strcpy(sParamObjectType, strTmpParamValue.c_str( ) );
		}
		else if ( strTmpParamName.compare("status_name") == 0 )
		{
			strcpy(sParamReleaseStatus, strTmpParamValue.c_str( ) );
		}
		else if ( strTmpParamName.compare("reference_type") == 0 )
		{
			strcpy(sParamReferenceType, strTmpParamValue.c_str( ) );
		}
	}

	if ( ( strcmp(sParamObjectType, "") == 0 ) || ( strcmp(sParamReleaseStatus, "") == 0 ) )
	{
		printf(" > ERROR: The ActionHandler 'SBT_AH_set_status_for_object_type' requires the arguments 'object_type' and 'status_name' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919710) );
		return 919710;
	}

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sTargetObjectType[WSO_name_size_c + 1] = "";

		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sTargetObjectType) );

		// Transform Target Object Type to lower case
		std::string strTargetObjectType = sTargetObjectType;
		//std::transform(strTargetObjectType.begin( ), strTargetObjectType.end( ), strTargetObjectType.begin( ), ::tolower);

		// workflow well captial "A" form handler,  need to do this as we are converting handler values to lower case because of 
		// new status display names
		if ( ( strcmp("All", sParamObjectType) == 0 ) || ( strcmp("all", sParamObjectType) == 0 )  || ( strcmp(strTargetObjectType.c_str( ), sParamObjectType) == 0 ) )
		{
			int     iStatusCount         = 0;
			tag_t * taStatusList         = NULL;
			logical lAlreadyHasNewStatus = false;

			// Get all status
			ITK(WSOM_ask_release_status_list(pTargetTags[iIdx1], &iStatusCount, &taStatusList) );

			// Traverse status
			for ( int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++ )
			{
				char *sStatusName = NULL;

				// Get Status Name
				ITK(AOM_UIF_ask_value(taStatusList[iIdx2], "name", &sStatusName) );

				if ( strcmp(sStatusName, sParamReleaseStatus) == 0 )
				{
					lAlreadyHasNewStatus = true;
				}

				// Free
				SAFE_SM_FREE(sStatusName);

				// Break
				if ( lAlreadyHasNewStatus == true )
				{
					break;
				}
			}

			if ( lAlreadyHasNewStatus == false )
			{
				char	*sRealStatusName				= NULL;
				tag_t tNewStatus = NULLTAG;
				tag_t taReleaseObjects[1];
				taReleaseObjects[0] = pTargetTags[iIdx1];

				// Get the real name of status before creating it.
				returnStatusRealName(sParamReleaseStatus,&sRealStatusName);

				ITK(CR_create_release_status(sRealStatusName, &tNewStatus) );
				ITK(AOM_save(tNewStatus) );

				// Retain release date if status is 90
				if(strcmp(sParamReleaseStatus, STATUS_TYPE_90) == 0)
				{
					ITK(EPM_add_release_status(tNewStatus, 1, taReleaseObjects, true) );
				} else {
					// For all other status, update release date on the target object
					ITK(EPM_add_release_status(tNewStatus, 1, taReleaseObjects, false) );
				}
			}

			// Free
			SAFE_SM_FREE(taStatusList);
		}
	}

	// Attache status to references
	if (strcmp(sParamReferenceType, "") != 0)
	{

		int         iReferenceCount       = 0;
		tag_t *     pReferenceTags        = NULL;

		/* Get Reference Attachments */
		ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );

		/* Loop over all target attachments */
		for ( int iIdx1 = 0; iIdx1 < iReferenceCount; iIdx1++ )
		{
			tag_t tClass = 0;

			if ( object_is_of_class_or_desc( pReferenceTags[iIdx1], sParamReferenceType, &tClass  ) )
			{
				int     iStatusCount         = 0;
				tag_t * taStatusList         = NULL;
				logical lAlreadyHasNewStatus = false;

				// Get all status
				ITK(WSOM_ask_release_status_list(pReferenceTags[iIdx1], &iStatusCount, &taStatusList) );

				// Traverse status
				for ( int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++ )
				{
					char *sStatusName = NULL;

					// Get Status Name
					ITK(AOM_UIF_ask_value(taStatusList[iIdx2], "name", &sStatusName) );

					if ( strcmp(sStatusName, sParamReleaseStatus) == 0 )
					{
						lAlreadyHasNewStatus = true;
					}

					// Free
					SAFE_SM_FREE(sStatusName);

					// Break
					if ( lAlreadyHasNewStatus == true )
					{
						break;
					}
				}

				if ( lAlreadyHasNewStatus == false )
				{
					char	*sRealStatusName				= NULL;
					tag_t tNewStatus = NULLTAG;
					tag_t taReleaseObjects[1];
					taReleaseObjects[0] = pReferenceTags[iIdx1];

					// Get the real name of status before creating it.
					returnStatusRealName(sParamReleaseStatus,&sRealStatusName);

					ITK(CR_create_release_status(sRealStatusName, &tNewStatus) );
					ITK(AOM_save(tNewStatus) );

					// Retain release date if status is 90
					if(strcmp(sParamReleaseStatus, STATUS_TYPE_90) == 0)
					{
						ITK(EPM_add_release_status(tNewStatus, 1, taReleaseObjects, true) );
					} else {
						// For all other status, update release date on the target object
						ITK(EPM_add_release_status(tNewStatus, 1, taReleaseObjects, false) );
					}
				}

				// Free
				SAFE_SM_FREE(taStatusList);
			}
		}

		/* Free */
		SAFE_SM_FREE(pReferenceTags);
	}


	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* END SBT_AH_set_status_for_object_type */


/**
 * @brief
 *Set status PreRelease to PDF
 */
int SBT_AH_set_status_PR_PDF(EPM_action_message_t msg)
{
	const char *theFunction			= "SBT_AH_set_status_PR_PDF";
	int         retcode				= ITK_ok;
	int         iTargetCount		= 0;
	tag_t       rootTaskTag			= NULLTAG;
	tag_t *     pTargetTags			= NULL;	
	tag_t		tRelTypeImanSpecification	= NULLTAG;
	
	
	// Find relation type
	ITK(GRM_find_relation_type("IMAN_specification", &tRelTypeImanSpecification));
	

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t		tItemRevision		= NULLTAG;
		tag_t tClass = NULLTAG;

		// Is Item Revision
		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true )
		{
			tItemRevision = pTargetTags[iIdx1];			
		}
	
		// Target Item Revision found?
		if(tItemRevision != NULLTAG)
		{
			int		iSecondaryCount		= 0;
			tag_t	*taSecondaryObjects	= NULL;

		
			// get existing datasets
			ITK(GRM_list_secondary_objects_only(tItemRevision, tRelTypeImanSpecification, &iSecondaryCount, &taSecondaryObjects));					

			/* Loop over all target attachments */
			for ( int iIdx1 = 0; iIdx1 < iSecondaryCount; iIdx1++ )
			{
				char	sObjectType[WSO_name_size_c+1];

			// Is PDF ?

			ITK(WSOM_ask_object_type(taSecondaryObjects[iIdx1], sObjectType));

				if ( strcmp(sObjectType, "PDF") == 0)
				{		
					tag_t	tCurrentPDF = taSecondaryObjects[iIdx1];
					tag_t	*taReleaseObjects	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
					tag_t	tStatus;
					// Init
					taReleaseObjects[0] = tCurrentPDF;
					ITK(CR_create_release_status("10", &tStatus) );
					ITK(AOM_save(tStatus) );
					// Add status
					ITK(EPM_add_release_status(tStatus, 1, taReleaseObjects, false));
				}
			}	
			SAFE_SM_FREE(taSecondaryObjects);
		}
	}
	// Free
	SAFE_SM_FREE(pTargetTags);
	
	LEAVE(theFunction);
	return retcode;
}/* End SBT_AH_set_status_PR_PDF */

/**
 * @brief
 * If the target is a Part, checks if an Item or BOM is attached and has status Prerelease ( 10 ) or Released ( 60 ) .
 */
EPM_decision_t SBT_RH_part_has_released_bom_attached(EPM_rule_message_t msg)
{
	int         retcode         = ITK_ok;
	int         iTargetCount    = 0;
	int         iReferenceCount = 0;
	int         iChildRevCount  = 0;
	const char *theFunction     = "SBT_RH_part_has_released_bom_attached";
	char *      sDecision       = EPM_RESULT_True;
	tag_t       rootTaskTag     = NULLTAG;
	tag_t *     pTargetTags     = NULLTAG;
	tag_t *     pReferenceTags  = NULLTAG;

	char sLatestStatusName[256] = "NULL";

	// Init traversal of BOM
	tag_t             tWindowTag    = NULLTAG;
	tag_t             tRuleTag      = NULLTAG;
	tag_t             tTopLineTag   = NULLTAG;
	tag_t *           tChildrenTags = NULL;
	tag_t **          taChildRevs   = NULLTAG;
	std::stringstream txtStringStream;

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );


	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
	char sParamStatusAllowed[WSO_name_size_c + 1] = "";
	vector<std::string> vecStatusAllowed;

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Transform Param Name to lower case
		std::transform(strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower);

		// Get Param Value
		if ( strTmpParamName.compare("status_allowed") == 0 )
		{
			strcpy(sParamStatusAllowed, strTmpParamValue.c_str( ) );
		}
	}

	if ( ( strcmp(sParamStatusAllowed, "") == 0 ) )
	{
		printf(" > ERROR: The ActionHandler 'SBT_RH_part_has_released_bom_attached' requires the arguments 'status_allowed' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919715) );
		return EPM_nogo;
	}

	// Fill Status-Allowed Vector
	string strParamStatusAllowed = sParamStatusAllowed;
	logical lContinueLoop = true;

	do
	{
		// Get Delimiter-Pos
		// MRTPLM: Changed declaration of iDelimPos from unsigned to size_t as it was taking garbage values when the string had no ','.
		std::size_t iDelimPos = strParamStatusAllowed.find(",");

		if(iDelimPos != std::string::npos)
		{
			std::string strTmpStatus  = strParamStatusAllowed.substr(0, iDelimPos);
			strParamStatusAllowed = strParamStatusAllowed.substr(iDelimPos + 1);
			vecStatusAllowed.push_back(strTmpStatus);
		}
		else {
			vecStatusAllowed.push_back(strParamStatusAllowed);
			lContinueLoop = false;
		}

		// Get Param Name
	} while(lContinueLoop == true);


	// Get the root task
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	// Get the attachments
	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
		ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );
	}


	/* Loop over all targets*/
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		// 1. Check: All Targets have the same status?
		//{
		//	// Get the status
		//	char * sTmpLatestStatusName = NULL;
		//	int    iStatusCount         = 0;

		//	// Read latest status
		//	askLatestReleaseStatus(pTargetTags[iIdx1], &sTmpLatestStatusName, &iStatusCount);

		//	// Init latest status
		//	if ( iStatusCount > 0 && strcmp(sLatestStatusName, "NULL") == 0 )
		//	{
		//		strcpy(sLatestStatusName, sTmpLatestStatusName);
		//	}

		//	// Identify errors
		//	if ( ( sTmpLatestStatusName == NULL ) && ( strcmp(sLatestStatusName, "NULL") != 0 ) )
		//	{
		//		txtStringStream << "All targets must have the same release status!" << endl;
		//		sDecision = EPM_RESULT_False;
		//	} else if ( ( sTmpLatestStatusName != NULL ) && ( strcmp(sLatestStatusName, sTmpLatestStatusName) != 0 ) ) {
		//		txtStringStream << "All targets must have the same release status!" << endl;
		//		sDecision = EPM_RESULT_False;
		//	} else {
		//		// success
		//	}

		//	/* Free */
		//	SAFE_SM_FREE(sTmpLatestStatusName);
		//}

		// 2. Check: BOM Lines valid ?
		// Issue 1036. WF Task crashes TC Server. Only check for children of Item revision
		tag_t tRevClassTag = NULLTAG;
		if(object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tRevClassTag))
		{
			// BOM check starts here 
			/*
			getAllBOMLines(pTargetTags[iIdx1], &iChildRevCount, &tChildrenTags, "BOM_Analysis" );//"Serienfreigabe" -"BT6 For_Prototype"
			// Loop over children
			for ( int iIdx2 = 0; iIdx2 < iChildRevCount; iIdx2++ )
			{
				logical lStatusOK					= false;
				int     iBomLineItemRevAttribute	= 0;
				tag_t   tChildRevTag1				= NULLTAG;
				int		iStatusCount				= 0;
				char *	sStatusName					= NULL;
				char    sChildType[WSO_name_size_c+1];

				// Get the revision
				ITK(BOM_line_look_up_attribute(bomAttr_lineItemRevTag, &iBomLineItemRevAttribute) );
				ITK(BOM_line_ask_attribute_tag(tChildrenTags[iIdx2], iBomLineItemRevAttribute, &tChildRevTag1) );

				// Get Object Type
				ITK(WSOM_ask_object_type(tChildRevTag1, sChildType));

				// Exit check of the current child if Type = BT7_SoftwareRevision
				if(strcmp(sChildType, "BT7_SoftwareRevision") == 0)
				{
					continue;
				}

				// Get the latest release status
				askLatestReleaseStatus(tChildRevTag1, &sStatusName, &iStatusCount);

				// Loop Allowed-Status Vector
				if(vecStatusAllowed.size() > 0)
				{
					lStatusOK = false;
					if(iStatusCount > 0)
					{
						for ( int iIdx2 = 0; iIdx2 < vecStatusAllowed.size(); iIdx2++ )
						{
							if ( strcmp(sStatusName, vecStatusAllowed.at(iIdx2).c_str()) == 0 )
							{
								lStatusOK = true;
								break;
							}
						}
					}
				} else {
					lStatusOK = true;
				}

				// Get the Object id
				char *sObjectId;
				ITK(WSOM_ask_object_id_string(tChildRevTag1, &sObjectId) );

				if ( lStatusOK == false )
				{
					txtStringStream << "The BOM line id <" << sObjectId << "> does not have the correct status '" << sParamStatusAllowed << "'" << endl;
					sDecision = EPM_RESULT_False;
				}

				// Free
				SAFE_SM_FREE(sStatusName);
			}

			// Free
			SAFE_SM_FREE(tChildrenTags); */

			// BOM check ends here
		}

		// 3. Check: Related BT7_Rel_Specified Objects
		{
			char	sObjectType[WSO_name_size_c+1];
			
			// Init relation types
			vector<char*> vecRelationTypes;
			vecRelationTypes.push_back("BT7_Rel_Specified");
			vecRelationTypes.push_back("BT7_Includes");
			vecRelationTypes.push_back("BT7_SUPPLIERSPEC");

			// Traverse Relations
			for(int iIdx2 = 0; iIdx2 < vecRelationTypes.size(); iIdx2++)
			{
				tag_t	tRelationType		= NULLTAG;
				tag_t	*taSecondaryObjects	= NULL;
				int		iSecondaryCount		= 0;

				// Get Secondary Objects
				ITK(GRM_find_relation_type(vecRelationTypes.at(iIdx2), &tRelationType));
				ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelationType, &iSecondaryCount, &taSecondaryObjects));

				// Traverse Secondary Objects
				for(int iIdx3 = 0; iIdx3 < iSecondaryCount; iIdx3++)
				{
					tag_t tClassTag = NULLTAG;

					// Secondary Objects is derived from Item Revision
					if(object_is_of_class_or_desc(taSecondaryObjects[iIdx3], "ItemRevision", &tClassTag))
					{
						logical lSecondaryStatusOK		= false;
						char	*sSecLatestStatusName	= NULL; // currently not used

						// Verify Status
						if(hasStatus(taSecondaryObjects[iIdx3], STATUS_TYPE_98))
						{
							// Error, status 98 not allowed ("lSecondaryStatusOK" is already set to "false")
						} else {
							// Identify Revision(s) which must be validated ...
							//  > Regular Case: If more than one revision is attached, at least one must be valid
							//  > Firmware Case: Latest attached revision must be valid

							// Variables
							tag_t			tItem			= NULLTAG;
							tag_t			*taTmpRevs		= NULL;
							tag_t			tLatestProgData	= NULLTAG;
							int				iTmpRevCount	= 0;
							vector<tag_t>	vecRelatedRevisionsOfCurrentObject;

							// Get Item
							ITK(ITEM_ask_item_of_rev(taSecondaryObjects[iIdx3], &tItem));

							// Get all Revisions
							ITK(ITEM_list_all_revs(tItem, &iTmpRevCount, &taTmpRevs));

							// Loop backwards and find attached Revisions
							for(int iIdx4 = (iTmpRevCount - 1); iIdx4 >= 0; iIdx4--)
							{
								tag_t tFoundRelation = NULLTAG;

								// Find relations
								ITK(GRM_find_relation(pTargetTags[iIdx1], taTmpRevs[iIdx4], tRelationType, &tFoundRelation));

								// Relation exist
								if(tFoundRelation != NULLTAG)
								{
									// Put to valid vector
									vecRelatedRevisionsOfCurrentObject.push_back(taTmpRevs[iIdx4]);

									// Identify latest attached Programming Data
									if(tLatestProgData == NULLTAG && isProgrammingData(taTmpRevs[iIdx4]))
									{
										tLatestProgData = taTmpRevs[iIdx4];
									}
								}
							}

							// Firmware- or Regular-Case?
							if(isSoftware(pTargetTags[iIdx1]) && isProgrammingData(taSecondaryObjects[iIdx3]) && tLatestProgData != NULLTAG)
							{
								// Firmware-Case: Latest attached revision must be valid
								
								// Loop valid status
								for ( int iIdx4 = 0; iIdx4 < vecStatusAllowed.size(); iIdx4++ )
								{
									if(hasStatus(tLatestProgData, vecStatusAllowed.at(iIdx4).c_str()))
									{
										lSecondaryStatusOK = true;
										break;
									}
								}
							} else {
								// Regular-Case: At least one of the attached revisions must be valid

								// Loop related revisions
								for(int iIdx4 = 0; iIdx4 < vecRelatedRevisionsOfCurrentObject.size(); iIdx4++)
								{
									// Loop valid status
									for ( int iIdx5 = 0; iIdx5 < vecStatusAllowed.size(); iIdx5++ )
									{
										if(hasStatus(vecRelatedRevisionsOfCurrentObject.at(iIdx4), vecStatusAllowed.at(iIdx5).c_str()))
										{
											lSecondaryStatusOK = true;
											break;
										}
									}

									// Exit loop if one of the objects is ok
									if(lSecondaryStatusOK == true)
									{
										break;
									}
								}
							}
						}

						// In case of an error, add a line to the error-stream
						if ( lSecondaryStatusOK == false )
						{
							char *sObjectId;

							ITK(WSOM_ask_object_id_string(taSecondaryObjects[iIdx3], &sObjectId) );
							txtStringStream << "The object <" << sObjectId << "> related with the relation <" << vecRelationTypes.at(iIdx2) << "> does not have the correct status '" << sParamStatusAllowed << "'" << endl;
							
							sDecision = EPM_RESULT_False;
						}

						// Free
						SAFE_SM_FREE(sSecLatestStatusName);
					} else {
						// Related objects is not an ItemRevision -> ignore
					}
				}

				// Free
				SAFE_SM_FREE(taSecondaryObjects);
			}
		}
	}

	if ( strcmp(sDecision, EPM_RESULT_False) == 0 )
	{
		//create a new ImanFile
		tag_t      txtFile     = NULLTAG;
		IMF_file_t txtFileDesc = NULL;

		ITK(IMF_create_file(NULL, SS_TEXT, &txtFile, &txtFileDesc) );
		ITK(IMF_open_file(txtFileDesc, SS_RDWRITE) );

		//write the XML-Stream to the new ImanFile
		std::string txtStringLine = "";

		while ( getline(txtStringStream, txtStringLine, '\n') )
		{
			if ( txtStringLine.length( ) <= SS_MAXLLEN )
			{
				ITK(IMF_write_file_line(txtFileDesc, txtStringLine.c_str( ) ) );
			}
		}

		//Close and save the new ImanFile
		ITK(IMF_close_file(txtFileDesc) );
		ITK(AOM_save(txtFile) );
		ITK(AOM_unload(txtFile) );

		//create the dataset and upload the dataset
		std::stringstream ssDatasetName;
		char sDateTime[20] = "";
		getCurrentDateTime(sDateTime);
		ssDatasetName << "Error Report" << " ( " << sDateTime << " )";
		printf(" >>> NEW DATASET NAME: %s \n", ssDatasetName.str().c_str());

		createDataset(msg.task, txtFile, ssDatasetName.str().c_str());
	}

	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );
	LEAVE(theFunction);
	return EPM_go;
}

/**
* @brief
* Only allow Common Component Engineers.
*/
EPM_decision_t  SBT_RH_current_user_is_common_comp_eng(EPM_rule_message_t msg)
{
	int				iTargetCount	= 0;
	EPM_decision_t	decision		= EPM_go;
	char *			sDecision		= EPM_RESULT_True;
	const char		*theFunction	= "SBT_RH_current_user_is_common_comp_eng";

	tag_t		tCurrentGroupMember	= NULLTAG;
	tag_t		tGroup				= NULLTAG;
	tag_t		tRole				= NULLTAG;
    char        sGroupName[SA_name_size_c + 1];
    char        sRoleName[SA_name_size_c + 1];

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	// Get current group member
	ITK(SA_ask_current_groupmember(&tCurrentGroupMember));

	// Get Group
	ITK(SA_ask_groupmember_group(tCurrentGroupMember, &tGroup));

	// Get Role
	ITK(SA_ask_groupmember_role(tCurrentGroupMember, &tRole));

	// Ask Group Name
	ITK(SA_ask_group_name(tGroup, sGroupName));

	// Ask Role Name
	ITK(SA_ask_role_name(tRole, sRoleName));

	// Current group is "Common Component Engineering" + role is "Designer"
	if(strcmp(sGroupName, "Common Component Engineering") != 0 || strcmp(sRoleName, "Designer") != 0)
	{
		sDecision = EPM_RESULT_False;
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );

	LEAVE(theFunction);

	return decision;
}


/**
* @brief
* Prevents minor revisions as targets.
*/
EPM_decision_t SBT_RH_prevent_minor_revision(EPM_rule_message_t msg)
{
	int				iTargetCount	= 0;
	EPM_decision_t	decision		= EPM_go;
	char *			sDecision		= EPM_RESULT_True;
	const char		*theFunction	= "SBT_RH_prevent_minor_revision";
	tag_t			rootTaskTag		= NULLTAG;
	tag_t *			pTargetTags		= NULLTAG;
	tag_t			tClassTag		= NULLTAG;

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Return if msg.tag is one of the attachment types */
    if ( msg.proposed_action == EPM_add_attachment_action )
    {
        if ( ( msg.tag == EPM_target_attachment ) ||
             ( msg.tag == EPM_reference_attachment ) ||
             ( msg.tag == EPM_signoff_attachment ) ||
             ( msg.tag == EPM_release_status_attachment ) ||
             ( msg.tag == EPM_comment_attachment ) ||
             ( msg.tag == EPM_instruction_attachment ) ||
             ( msg.tag == EPM_interprocess_task_attachment ) ||
             ( msg.tag == EPM_project_task_attachment ) ||
             ( msg.tag == EPM_user_attachment ) 
           )
		{
			return EPM_go;
		}
	}

	// If target is an ItemRevision
	if ( object_is_of_class_or_desc(msg.tag, "ItemRevision", &tClassTag) )
	{
		logical lFirstIsAlpha	= false;
		logical lFirstIsDigit	= false;
		logical lSecondIsAlpha	= false;
		logical lSecondIsDigit	= false;
		logical lThirdIsDigit	= false;
		logical lFourthIsDigit	= false;

		logical lIsPcbRaw		= false;

		char sRevID[ITEM_id_size_c + 1];
		char sObjectType[WSO_name_size_c+1];

		// Get object type
		ITK(WSOM_ask_object_type(msg.tag, sObjectType));

		// Is PCB-Raw?
		if(strcmp(sObjectType, "BT7_ComponentRevision") == 0)
		{
			char *sPartType = NULL;

			// Ask Part Type
			ITK(AOM_ask_value_string(msg.tag, "bt7_parttype", &sPartType));
			if(strcmp(sPartType, "pt_pcb") == 0)
			{
				lIsPcbRaw = true;
			}

			SAFE_SM_FREE(sPartType);
		}

		// Skip minor-rev check for PCB-raw
		if(lIsPcbRaw == false)
		{
			// Get ItemRev ID
			ITK(ITEM_ask_rev_id(msg.tag, sRevID));

			// Traverse Characters of the Revision ID
			for ( int iIdx2 = 0; iIdx2 < strlen(sRevID); iIdx2++)
			{
				int iIsAlpha = isalpha(sRevID[iIdx2]);
				int iIsDigit = isdigit(sRevID[iIdx2]);

				switch (iIdx2)
				{
					case 0:
						lFirstIsAlpha = (iIsAlpha != 0);
						lFirstIsDigit = (iIsDigit != 0);
						break;
					case 1:
						lSecondIsAlpha = (iIsAlpha != 0);
						lSecondIsDigit = (iIsDigit != 0);
						break;
					case 2:
						lThirdIsDigit = (iIsDigit != 0);
						break;
					case 3:
						lFourthIsDigit = (iIsDigit != 0);
						break;
					default:
						break;
				}
			}

			// Is minor revision ?
			if( (strlen(sRevID) == 2 && lFirstIsDigit && lSecondIsDigit) ||
				(strlen(sRevID) == 2 && lFirstIsAlpha && lSecondIsDigit) ||
				(strlen(sRevID) == 3 && lFirstIsAlpha && lSecondIsDigit && lThirdIsDigit) ||
				(strlen(sRevID) == 3 && lFirstIsAlpha && lSecondIsAlpha && lThirdIsDigit) ||
				(strlen(sRevID) == 4 && lFirstIsAlpha && lSecondIsAlpha && lThirdIsDigit && lFourthIsDigit)
			  )
			{
				decision = EPM_nogo;
				ITK(EMH_store_error(EMH_severity_error, 919717) );
			}
		}
	}

	LEAVE(theFunction);

	return decision;
}

/**
 * @brief
 * Checks if the target is a PartRevision or a Document with the status None, Prerelease ( 10 ) or Rejected ( 90 ) .
 * Attach the incorrect items to a text file and exit
 */
EPM_decision_t SBT_RH_check_target_type(EPM_rule_message_t msg)
{
	int         iTargetCount = 0;
	const char *theFunction  = "SBT_RH_check_target_type";
	char *      sDecision    = EPM_RESULT_True;
	tag_t       rootTaskTag  = NULLTAG;
	tag_t *     pTargetTags  = NULLTAG;

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}


	// read arguments
	TC_argument_list_t *myArguments = msg.arguments;
	std::string         sParamValue = "BT7_DocumentRevision";

	if ( myArguments != NULL )
	{
		for ( int i = 0; i < ( *myArguments ).number_of_arguments; i++ )
		{
			stringstream ss( ( *myArguments ).arguments[i].val_union.str_value);
			std::string  s1 = "";
			s1.clear( );

			logical     lParam1     = false;
			std::string sParamValue = "";

			while ( getline(ss, s1, '=') )
			{
				if ( s1.compare("type") == 0 )
				{
					lParam1 = true;
				}
				else
				{
					if ( lParam1 == true )
					{
						lParam1     = false;
						sParamValue = s1;
					}
					break;
				}
			}
		}
	}
	else
	{
		printf("ActionHandler doesn't contain any parameters, falling back to BT7_DocumentRevision! \n");
	}


	/* Loop over all targets*/
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		// Get the object type
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		// Check target type
		if ( strcmp(sObjectType, sParamValue.c_str( ) ) != 0 )
		{
			sDecision = EPM_RESULT_False;
		}
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );
	LEAVE(theFunction);
	return EPM_go;
}


/**
* @brief
* Verifies if the target object is a document revision (BT6 or BT7)
*/
EPM_decision_t  SBT_RH_is_document(EPM_rule_message_t msg)
{
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	const char *theFunction        = "SBT_RH_is_document";
	char *      sDecision          = EPM_RESULT_False;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t *     pTargetTags        = NULLTAG;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c+1];

		// Get Object Type
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType));

		// Is a BT6 or BT7 Document ?
		if(strcmp(sObjectType, "BT7_DocumentRevision") == 0)
		{
			// Is Doc
			sDecision = EPM_RESULT_True;
		} else {
			// No Doc
			sDecision = EPM_RESULT_False;

			// Break
			break;
		}
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
} /* END SBT_RH_is_document */


/**
* @brief
* Verifies if a UGMASTER is related to the target item revision
*/
EPM_decision_t  SBT_RH_add_drawing_pdf_required(EPM_rule_message_t msg)
{
	int         retcode      = ITK_ok;
	int         iTargetCount = 0;
	const char *theFunction  = "SBT_RH_add_drawing_pdf_required";
	char *      sDecision    = EPM_RESULT_False;
	tag_t       rootTaskTag  = NULLTAG;
	tag_t *     pTargetTags  = NULLTAG;
	logical     lStatusOK    = true;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get the root task */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get workflow targets */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClass = NULLTAG;

		// Is Item Revision
		if(object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClass) == true)
		{
			int		iSecondaryCount			= 0;
			tag_t	*taSecondaryObjects		= NULL;
			tag_t	tRelTypeSpecification	= NULLTAG;

			// Get Relation Type
			ITK(GRM_find_relation_type("IMAN_specification", &tRelTypeSpecification));
			
			// Get Secondary Objects
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeSpecification, &iSecondaryCount, &taSecondaryObjects));

			/* Loop over all secondary objects */
			for ( int iIdx2 = 0; iIdx2 < iSecondaryCount; iIdx2++ )
			{
				char sObjectType[WSO_name_size_c+1];

				// get object type
				ITK(WSOM_ask_object_type(taSecondaryObjects[iIdx2], sObjectType));

				// is UGPART ?
				if(strcmp(sObjectType, "UGPART") == 0)
				{
					// Set decision
					sDecision = EPM_RESULT_True;

					break;
				}
			}

			// Free
			SAFE_SM_FREE(taSecondaryObjects);
		}
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
}

/**
 * @brief
 * Verifies if the target is the status released.
 */
EPM_decision_t SBT_RH_check_target_released(EPM_rule_message_t msg)
{
	int         retcode      = ITK_ok;
	int         iTargetCount = 0;
	const char *theFunction  = "SBT_RH_check_target_released";
	char *      sDecision    = EPM_RESULT_False;
	tag_t       rootTaskTag  = NULLTAG;
	tag_t *     pTargetTags  = NULLTAG;
	logical     lStatusOK    = true;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );


	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
	char sParamStatusAllowed[WSO_name_size_c + 1] = "";
	vector<std::string> vecStatusAllowed;

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Transform Param Name to lower case
		std::transform(strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower);

		// Get Param Value
		if ( strTmpParamName.compare("status") == 0 )
		{
			strcpy(sParamStatusAllowed, strTmpParamValue.c_str( ) );
		}
	}

	if ( ( strcmp(sParamStatusAllowed, "") == 0 ) )
	{
		printf(" > ERROR: The ActionHandler 'SBT_RH_check_target_released' requires the arguments 'status' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919714) );
		return EPM_nogo;
	}

	// Fill Status-Allowed Vector
	string strParamStatusAllowed = sParamStatusAllowed;
	logical lContinueLoop = true;

	do
	{
		// Get Delimiter-Pos
		// MRTPLM: Changed declaration of iDelimPos from unsigned to size_t as it was taking garbage values when the string had no ','.
		size_t iDelimPos = strParamStatusAllowed.find(",");

		if(iDelimPos != std::string::npos)
		{
			std::string strTmpStatus  = strParamStatusAllowed.substr(0, iDelimPos);
			strParamStatusAllowed = strParamStatusAllowed.substr(iDelimPos + 1);
			vecStatusAllowed.push_back(strTmpStatus);
		}
		else {
			vecStatusAllowed.push_back(strParamStatusAllowed);
			lContinueLoop = false;
		}

		// Get Param Name
	} while(lContinueLoop == true);

	/* Get the target objects*/
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		// Get the status
		int    iStatusCount = 0;
		char * sStatusName      = NULL;

		// Get latest status
		askLatestReleaseStatus(pTargetTags[iIdx1], &sStatusName, &iStatusCount);

		// Loop Allowed-Status Vector
		if(vecStatusAllowed.size() > 0)
		{
			lStatusOK = false;
			if(iStatusCount > 0)
			{
				for ( int iIdx2 = 0; iIdx2 < vecStatusAllowed.size(); iIdx2++ )
				{
					if ( strcmp(sStatusName, vecStatusAllowed.at(iIdx2).c_str()) == 0 )
					{
						lStatusOK = true;
						break;
					}
				}
			}
		} else {
			lStatusOK = true;
		}

		if ( lStatusOK == true )
		{
			sDecision = EPM_RESULT_True;
		}

		// Free
		SAFE_SM_FREE(sStatusName);
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
}


void checkInObject(tag_t tObject)
{
	logical lIsCheckedOut = false;

	// Checkin objects if checked-out
	ITK(RES_is_checked_out(tObject, &lIsCheckedOut) );

	if ( lIsCheckedOut == true )
	{
		logical lCheckinAllowed = false;

		ITK(AM_check_privilege(tObject, "CHECKIN", &lCheckinAllowed) );
		printf("Checkin Allowed ? %i \n", lCheckinAllowed);
		ITK(RES_checkin(tObject) );
	}
}


/**
 * @brief
 * Assign the current task to the approver-role of the same group like the requestor is logged in to
 */
int SBT_AH_assign_to_approver_of_same_group(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_assign_to_approver_of_same_group";
	int         retcode     = ITK_ok;
	tag_t       rootTaskTag = NULLTAG;

	tag_t tApproverResourcePool = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get RootTask-Name */
	ITK(EPM_ask_name(rootTaskTag, sWorkflowName) );

	/* Get Owning Group */
	tag_t  tOwningGroup = NULLTAG;
	int    iRoleCount   = 0;
	tag_t *taRoles      = NULL;

	ITK(AOM_ask_value_tag(rootTaskTag, "owning_group", &tOwningGroup) );
	ITK(SA_ask_roles_from_group(tOwningGroup, &iRoleCount, &taRoles) );

	/* Loop over all roles */
	for ( int iIdx1 = 0; iIdx1 < iRoleCount; iIdx1++ )
	{
		char sRolename[SA_name_size_c + 1];

		// Ask Role Name
		ITK(SA_ask_role_name(taRoles[iIdx1], sRolename) );

		if ( strcmp(sRolename, "Approver") == 0 )
		{
			ITK(EPM_get_resource_pool(tOwningGroup, taRoles[iIdx1], false, &tApproverResourcePool) );

			break;
		}
	}

	// Reviewer Resource-Pool empty?
	if ( tApproverResourcePool == NULLTAG )
	{
		tag_t tApproverRole = NULLTAG;
		ITK(SA_find_role("Approver", &tApproverRole) );
		ITK(EPM_get_resource_pool(NULLTAG, tApproverRole, false, &tApproverResourcePool) );
	}

	// Set Proposed Responsible Party
	if ( tApproverResourcePool != NULLTAG )
	{
		ITK(EPM_assign_responsible_party(msg.task, tApproverResourcePool) );
		ITK(AOM_save(msg.task) );
		ITK(AOM_refresh(msg.task, false) );
	}

	// Free
	SAFE_SM_FREE(taRoles);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_assign_to_approver_of_same_group */

/**
 * @brief
 * Remove the current revision from the targets and add all based-on revisions to the targets
 * Background: If a revision is set to status 10 or 60, all based-on revisions must be set to 90
 */
int SBT_AH_remove_current_revision_add_based_on_revisions(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_remove_current_revision_add_based_on_revisions";
	int         retcode     = ITK_ok;
	tag_t       rootTaskTag = NULLTAG;

	int    iTargetCount = 0;
	tag_t *pTargetTags  = NULL;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get RootTask-Name */
	ITK(EPM_ask_name(rootTaskTag, sWorkflowName) );

	/* Get target attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Remove existing revisions from targets */
	ITK(EPM_remove_attachments(rootTaskTag, iTargetCount, pTargetTags) );
	ITK(AOM_lock(rootTaskTag));
	ITK(AOM_save(rootTaskTag) );
	ITK(AOM_unlock(rootTaskTag));
	
	/* Attach based-on revisions to targets */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClassTag = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClassTag) )
		{
			int		iRevCount	= 0;
			tag_t	*taRevs		= NULL;
			tag_t	tItem		= NULLTAG;
			date_t	creationDate;
			char	*sLatestReleaseStatus_Parent	= NULL;
			int		iParentStatusCount				= 0;

			// Get creation date of current revision
			ITK(AOM_ask_value_date(pTargetTags[iIdx1], "creation_date", &creationDate) );

			// Get latest release status
			askLatestReleaseStatus(pTargetTags[iIdx1], &sLatestReleaseStatus_Parent, &iParentStatusCount);

			// Only applicable if status to be set is '10' or '60'
			if(iParentStatusCount > 0 && (strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_10) == 0 || strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_60) == 0 || strcmp(sLatestReleaseStatus_Parent, "ECO") == 0) || strcmp(sLatestReleaseStatus_Parent, "BT7_ECO") == 0)
			{
				// Find Item
				ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tItem));

				// Find all revisions
				ITK(ITEM_list_all_revs(tItem, &iRevCount, &taRevs));

				// Loop revisions
				for ( int iIdx2 = 0; iIdx2 < iRevCount; iIdx2++ )
				{
					date_t tmpCreationDate;

					// Get creation date of current revision
					ITK(AOM_ask_value_date(taRevs[iIdx2], "creation_date", &tmpCreationDate) );

					// Compare Creation Date with current revision
					if ( CLOCK_compare_dates(&creationDate, &tmpCreationDate) > 0 )
					{
						int		iTmpTargetCount		= 0;
						logical	lAlreadyAttached	= false;
						char	*sLatestReleaseStatus_OldRevision	= NULL;
						int		iStatusCount_OldRevision			= 0;
						logical	lDoSetStatus90		= false;

						// Get latest release status of the superseeded revision
						askLatestReleaseStatus(taRevs[iIdx2], &sLatestReleaseStatus_OldRevision, &iStatusCount_OldRevision);

						// Status 90 applicable ?
						if(iStatusCount_OldRevision == 0)
						{
							lDoSetStatus90 = true;
						} else if(iStatusCount_OldRevision == 0 && (strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_10) == 0 || strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_60) == 0)) {
							lDoSetStatus90 = true;
						} else if(strcmp(sLatestReleaseStatus_OldRevision, STATUS_TYPE_10) == 0 && strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_10) == 0) {
							lDoSetStatus90 = true;
						} else if(strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_60) == 0) {
							lDoSetStatus90 = true;
						} else if(strcmp(sLatestReleaseStatus_OldRevision, "BT7_ECO") == 0 || strcmp(sLatestReleaseStatus_Parent, STATUS_TYPE_10) == 0) {
							lDoSetStatus90 = true;
						}

						if(lDoSetStatus90 == true)
						{
							tag_t	*pTmpTargetTags		= NULL;

							/* Get target attachments */
							ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTmpTargetCount, &pTmpTargetTags) );

							/* Loop current targets */
							for ( int iIdx3 = 0; iIdx3 < iTmpTargetCount; iIdx3++ )
							{
								if ( pTmpTargetTags[iIdx3] == taRevs[iIdx2] )
								{
									lAlreadyAttached = true;
									break;
								}
							}

							// Attach as target to the workflow if not already attached
							if ( lAlreadyAttached == false )
							{
								// Attach object to the workflow targets
								tag_t *taObjectsToBeSetToStatus90 = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );
								int *  iaAttachmentType           = (int *) MEM_alloc(1 * sizeof( int ) );

								taObjectsToBeSetToStatus90[0] = taRevs[iIdx2];
								iaAttachmentType[0]           = EPM_target_attachment;

								ITK(EPM_add_attachments(rootTaskTag, 1, taObjectsToBeSetToStatus90, iaAttachmentType) );
								ITK(AOM_lock(rootTaskTag));
								ITK(AOM_save(rootTaskTag));
							}


							// Free
							SAFE_SM_FREE(pTmpTargetTags);
						}
					}
				}
			}

			// Free
			SAFE_SM_FREE(taRevs);
			SAFE_SM_FREE(sLatestReleaseStatus_Parent);
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_remove_current_revision_add_based_on_revisions */


/**
 * @brief
 * Attach Item and all ItemRevisions for reactivation
 */
int SBT_AH_attach_item_and_all_revs(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_attach_item_and_all_revs";
	int         retcode     = ITK_ok;
	tag_t       rootTaskTag = NULLTAG;

	int    iTargetCount = 0;
	tag_t *pTargetTags  = NULL;

	tag_t tSelectedItemRevision = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get RootTask-Name */
	ITK(EPM_ask_name(rootTaskTag, sWorkflowName) );

	/* Get target attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClassTag = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClassTag) )
		{
			int           iRevCount = 0;
			tag_t *       taRevs    = NULL;
			tag_t         tItem     = NULLTAG;
			vector<tag_t> vecNewWorkflowAttachments;

			tSelectedItemRevision = pTargetTags[iIdx1];

			// Get Item
			ITK(ITEM_ask_item_of_rev(tSelectedItemRevision, &tItem) );
			vecNewWorkflowAttachments.push_back(tItem);

			// Get all Revisions
			ITK(ITEM_list_all_revs(tItem, &iRevCount, &taRevs) );

			// Loop All Revisions
			for ( int iIdx2 = 0; iIdx2 < iRevCount; iIdx2++ )
			{
				// Loop Workflow Targets
				for ( int iIdx3 = 0; iIdx3 < iTargetCount; iIdx3++ )
				{
					// If Revision is not in the Workflow Targets -> Add to Vector
					if ( taRevs[iIdx2] != pTargetTags[iIdx3] )
					{
						vecNewWorkflowAttachments.push_back(taRevs[iIdx2]);
					}
				}
			}

			// Attach object to the workflow targets
			tag_t *taObjectsToBeReactivated = (tag_t *) MEM_alloc(vecNewWorkflowAttachments.size( ) * sizeof( tag_t ) );
			int *  iaAttachmentType         = (int *) MEM_alloc(vecNewWorkflowAttachments.size( ) * sizeof( int ) );

			for ( int iIdx2 = 0; iIdx2 < vecNewWorkflowAttachments.size( ); iIdx2++ )
			{
				taObjectsToBeReactivated[iIdx2] = vecNewWorkflowAttachments.at(iIdx2);
				iaAttachmentType[iIdx2]         = EPM_target_attachment;
			}

			ITK(EPM_add_attachments(rootTaskTag, vecNewWorkflowAttachments.size( ), taObjectsToBeReactivated, iaAttachmentType) );
			ITK(AOM_save(rootTaskTag) );

			// Free
			SAFE_SM_FREE(taRevs);

			break;
		}

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "Item", &tClassTag) )
		{
			int           iRevCount = 0;
			tag_t *       taRevs    = NULL;
			tag_t         tItem     = NULLTAG;
			vector<tag_t> vecNewWorkflowAttachments;

			// Get all Revisions
			ITK(ITEM_list_all_revs(pTargetTags[iIdx1], &iRevCount, &taRevs) );

			// Loop All Revisions
			for ( int iIdx2 = 0; iIdx2 < iRevCount; iIdx2++ )
			{
				// Loop Workflow Targets
				for ( int iIdx3 = 0; iIdx3 < iTargetCount; iIdx3++ )
				{
					// If Revision is not in the Workflow Targets -> Add to Vector
					if ( taRevs[iIdx2] != pTargetTags[iIdx3] )
					{
						vecNewWorkflowAttachments.push_back(taRevs[iIdx2]);
					}
				}
			}

			// Attach object to the workflow targets
			tag_t *taObjectsToBeReactivated = (tag_t *) MEM_alloc(vecNewWorkflowAttachments.size( ) * sizeof( tag_t ) );
			int *  iaAttachmentType         = (int *) MEM_alloc(vecNewWorkflowAttachments.size( ) * sizeof( int ) );

			for ( int iIdx2 = 0; iIdx2 < vecNewWorkflowAttachments.size( ); iIdx2++ )
			{
				taObjectsToBeReactivated[iIdx2] = vecNewWorkflowAttachments.at(iIdx2);
				iaAttachmentType[iIdx2]         = EPM_target_attachment;
			}

			ITK(EPM_add_attachments(rootTaskTag, vecNewWorkflowAttachments.size( ), taObjectsToBeReactivated, iaAttachmentType) );
			ITK(AOM_save(rootTaskTag) );

			// Free
			SAFE_SM_FREE(taRevs);

			break;
		}
	}

	/* Target Object identified? */
	if ( tSelectedItemRevision != NULLTAG )
	{
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_attach_item_and_all_revs */


/**
 * @brief
 * Attach BOM childs for annulling
 */
int SBT_AH_attach_bom_childs_for_anulling(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_attach_bom_childs_for_anulling";
	int         retcode     = ITK_ok;
	tag_t       rootTaskTag = NULLTAG;

	int    iTargetCount = 0;
	tag_t *pTargetTags  = NULL;
	
	vector<tag_t> vecTcObjectsToBeAnulled;
	vector<tag_t> vecTcObjectsAlreadyTested;
	vector<tag_t> vecTcWorkflowTargets;


	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get RootTask-Name */
	ITK(EPM_ask_name(rootTaskTag, sWorkflowName) );

	/* Get target attachments */
	// ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClassTag = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClassTag) )
		{
			vecTcObjectsToBeAnulled.push_back(pTargetTags[iIdx1]);
			vecTcWorkflowTargets.push_back(pTargetTags[iIdx1]);
		}
	}

	/* Target Object identified? */
	if ( vecTcObjectsToBeAnulled.size( ) > 0 )
	{
		// Traverse BOM with Revision Rule: Latest Working
		// Identify BOM children without any usage at non-annulled assembly ( -> Revision Rule: Latest Released / All ( ? ) )
		// Attach the item + all revisions of those children as targets to the current workflow

		// Identify objects to be annulled
		for ( int iIdx1 = 0; iIdx1 < vecTcWorkflowTargets.size( ); iIdx1++ )
		{
			attachBomChildsForAnulling(vecTcWorkflowTargets.at(iIdx1), &vecTcObjectsToBeAnulled, &vecTcObjectsAlreadyTested, true);
		}

		// Check-in objects if required
		for ( int iIdx1 = 0; iIdx1 < vecTcObjectsToBeAnulled.size( ); iIdx1++ )
		{
			int    iSecondaryCount    = 0;
			tag_t *taSecondaryObjects = NULL;
			tag_t  tClassTag          = NULLTAG;

			if ( object_is_of_class_or_desc(vecTcObjectsToBeAnulled.at(iIdx1), "ItemRevision", &tClassTag) )
			{
				int    iBVRCount = 0;
				tag_t *taBVRs    = NULL;
								
				// Get all BVRs of the current Item Revision
				ITK(ITEM_rev_list_bom_view_revs(vecTcObjectsToBeAnulled.at(iIdx1), &iBVRCount, &taBVRs) );
				// Check-in
				for ( int iIdx2 = 0; iIdx2 < iBVRCount; iIdx2++ )
				{
					checkInObject(taBVRs[iIdx2]);
				}

				// Free
				SAFE_SM_FREE(taBVRs);
			}

			// Check-in secondary objects if required
			ITK(GRM_list_secondary_objects_only(vecTcObjectsToBeAnulled.at(iIdx1), NULLTAG, &iSecondaryCount, &taSecondaryObjects) );

			for ( int iIdx2 = 0; iIdx2 < iSecondaryCount; iIdx2++ )
			{
				// Checkin objects if checked-out
				checkInObject(taSecondaryObjects[iIdx2]);
			}

			// Checkin the current objects itself if checked-out
			checkInObject(vecTcObjectsToBeAnulled.at(iIdx1) );

			// Free
			SAFE_SM_FREE(taSecondaryObjects);
		}

		// Remove top-assembly-revision from vector ( -> is already a target of the workflow )
		for ( int iIdx1 = vecTcObjectsToBeAnulled.size( ) - 1; iIdx1 >= 0 ; iIdx1-- )
		{
			for ( int iIdx2 = 0; iIdx2 < vecTcWorkflowTargets.size( ); iIdx2++ )
			{
				if ( vecTcObjectsToBeAnulled.at(iIdx1) == vecTcWorkflowTargets.at(iIdx2) )
				{
					vecTcObjectsToBeAnulled.erase(vecTcObjectsToBeAnulled.begin( ) + iIdx1);
					break;
				}
			}

		}

		if ( vecTcObjectsToBeAnulled.size( ) > 0 )
		{
			// Attach object to the workflow targets
			tag_t *taObjectsToBeAnulled = (tag_t *) MEM_alloc(vecTcObjectsToBeAnulled.size( ) * sizeof( tag_t ) );
			int *  iAttachmentType     = (int *) MEM_alloc(vecTcObjectsToBeAnulled.size( ) * sizeof( int ) );
			
			tag_t tClassTag;
			int iTargetCounter = 0;

			for ( int iIdx1 = 0; iIdx1 < vecTcObjectsToBeAnulled.size( ); iIdx1++ )
			{
				logical lHasStatus98 = hasStatus(vecTcObjectsToBeAnulled.at(iIdx1), STATUS_TYPE_98);
				vecTcObjectsToBeAnulled.erase(unique(vecTcObjectsToBeAnulled.begin(),vecTcObjectsToBeAnulled.end()),vecTcObjectsToBeAnulled.end());
				
				if ( lHasStatus98 == false )
				{
					if ( object_is_of_class_or_desc(vecTcObjectsToBeAnulled.at(iIdx1), "WorkspaceObject", &tClassTag) )
					{
						taObjectsToBeAnulled[iTargetCounter] = vecTcObjectsToBeAnulled.at(iIdx1);
						iAttachmentType[iTargetCounter]     = EPM_target_attachment;
						iTargetCounter++;
					}
				}
			}				
			ITK(EPM_add_attachments(rootTaskTag, iTargetCounter, taObjectsToBeAnulled, iAttachmentType) );
			ITK(AOM_save(rootTaskTag) );
			
			/* Free */
			SAFE_SM_FREE(taObjectsToBeAnulled);
			SAFE_SM_FREE(iAttachmentType);
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_attach_bom_childs_for_annulling */

/**
* @brief
* Check whether selected item can be annulled
*/
EPM_decision_t SBT_RH_check_item_annullable(EPM_action_message_t msg)
{
	int         	retcode         		= ITK_ok;
	int         	iTargetCount    		= 0;
	const char *	theFunction     		= "SBT_RH_check_item_annullable";
	tag_t       	rootTaskTag     		= NULLTAG;
	tag_t *     	pTargetTags     		= NULLTAG;
	EPM_decision_t	dResult					= EPM_nogo;
	vector<tag_t> 	vecTcObjectsToBeRemovedFromWF;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Get target attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tClassTag = NULLTAG;

		/* Target Object identified? */
		if ( (pTargetTags[iIdx1] != NULLTAG) && (object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClassTag)) )
		{
			vector<tag_t> vecTcObjectsToBeAnulled;
			vector<tag_t> vecTcObjectsAlreadyTested;

			// Identify whether object can be annulled
			logical lItemCanBeAnnulled = allParentsOn1stLevelHaveStatus98OrWip(pTargetTags[iIdx1], &vecTcObjectsToBeAnulled);

			if (lItemCanBeAnnulled)
			{
				dResult = EPM_go;
			}
			else
			{
				vecTcObjectsToBeRemovedFromWF.push_back(pTargetTags[iIdx1]);
			}
		}
	}

	// Remove Revision that can not be annulled
	if(vecTcObjectsToBeRemovedFromWF.size( ) > 0)
	{
		tag_t *taObjectsToBeRemoved = (tag_t *) MEM_alloc(vecTcObjectsToBeRemovedFromWF.size( ) * sizeof( tag_t ) );
		
		for ( int iIdx1 = 0; iIdx1 < vecTcObjectsToBeRemovedFromWF.size( ); iIdx1++ )
		{
				taObjectsToBeRemoved[iIdx1] = vecTcObjectsToBeRemovedFromWF.at(iIdx1);
		}
		
		ITK(EPM_remove_attachments( rootTaskTag, vecTcObjectsToBeRemovedFromWF.size( ), taObjectsToBeRemoved) );
		ITK(AOM_save(rootTaskTag) );
		
		/* Free */
		SAFE_SM_FREE(taObjectsToBeRemoved);
	}
	
	// Free
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return dResult;

} /* End SBT_RH_check_item_annullable */

/**
* @brief
* Unlock the root task
*/
int SBT_AH_unlock_task(EPM_action_message_t msg)
{
	int         	retcode         		= ITK_ok;
	const char *	theFunction     		= "SBT_AH_unlock_task";
	tag_t       	rootTaskTag     		= NULLTAG;
	int				iInstanceState			= POM_inst_is_not_loaded;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	// Ask state
	ITK(POM_ask_instance_state(rootTaskTag, &iInstanceState));

	if(iInstanceState == POM_inst_is_loaded_for_modify)
	{
		// Unloak the root task
		ITK(AOM_refresh(rootTaskTag, false));
	}

	LEAVE(theFunction);
	return retcode;

} /* End SBT_AH_unlock_task */


/**
 * is Revision WiP or 98?
 */
// R1.05
logical checkForWipOr98(tag_t tObject, vector<tag_t> *vecTcObjectsToBeAnulled, logical lTestOnWiP, logical lTestOn90 ) {//logical checkForWipOr98(tag_t tObject, vector<tag_t> *vecTcObjectsToBeAnulled, logical lTestOnWiP ) {

	int     iStatusCount                  = 0;
	tag_t * taStatusList                  = NULL;
	logical lParentIsStatus98OrWip        = false;
	logical lParentInProcessToGetStatus98 = false;
	tag_t  tClassTag					  = NULLTAG;

	// If current parent is in the list of to-be anulled objects, skip status-check
	for ( int iIdx1 = 0; iIdx1 < ( *vecTcObjectsToBeAnulled ).size( ); iIdx1++ ) {
		if ( tObject == ( *vecTcObjectsToBeAnulled ).at(iIdx1) ) {
			lParentInProcessToGetStatus98 = true;
			break;
		}
	}

	if ( lParentInProcessToGetStatus98 == true ) {
		return lParentInProcessToGetStatus98;
	}
	
	if ( object_is_of_class_or_desc(tObject, "ItemRevision", &tClassTag) )
	{
		// Has Parent Status 98 or no status?
		ITK(WSOM_ask_release_status_list(tObject, &iStatusCount, &taStatusList) );

		if ( iStatusCount == 0 ) {
			lParentIsStatus98OrWip = lTestOnWiP;
		}
		else {
			for ( int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++ ) {
				char *sStatusName = NULL;

				ITK(AOM_UIF_ask_value(taStatusList[iIdx2], "name", &sStatusName) );

				if ( strcmp(sStatusName, STATUS_TYPE_98) == 0 ) {
					lParentIsStatus98OrWip = true;
					break;
				}
				if ( lTestOn90 && ( strcmp(sStatusName, STATUS_TYPE_90) == 0 ) ) {
					lParentIsStatus98OrWip = true;//return true;
					break;
				}//R1.05

				// Free
				SAFE_SM_FREE(sStatusName);
			}
		}
	}
	else if ( object_is_of_class_or_desc(tObject, "Item", &tClassTag) )
	{
		int           iRevCount   = 0;
		tag_t *       taRevs      = NULL;
		tag_t         tItem       = NULLTAG;

		// Get all Revisions	
		ITK(ITEM_list_all_revs(tObject, &iRevCount, &taRevs) );
		for(int iIdx1 = 0; iIdx1 < iRevCount; iIdx1++)
		{
			// Has Parent Status 98 or no status?
			ITK( WSOM_ask_release_status_list(taRevs[iIdx1], &iStatusCount, &taStatusList) );
			
			if ( iStatusCount == 0 ) {
				lParentIsStatus98OrWip = lTestOnWiP;
				break;
			}
			else {
				for ( int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++ ) {
					char *sStatusName = NULL;

					ITK(AOM_UIF_ask_value(taStatusList[iIdx2], "name", &sStatusName) );

					if ( strcmp(sStatusName, STATUS_TYPE_98) == 0 ) {
						lParentIsStatus98OrWip = true;
						break;
					}
					if ( lTestOn90 && ( strcmp(sStatusName, "90") == 0 ) ) {
						lParentIsStatus98OrWip = true;//return true;	
						break;
					}//R1.05

					// Free
					SAFE_SM_FREE(sStatusName);
				}
				if(lParentIsStatus98OrWip)
				{
					break;
				}
			}
		}
	}

	// Free
	SAFE_SM_FREE(taStatusList);
	
	return lParentIsStatus98OrWip;
}

/**
 * @brief
 * Sets the latest release Status to a specific property of a specific item type
 */
int SBT_AH_set_latest_release_status(EPM_action_message_t msg)
{
	const char 	*theFunction      = "SBT_AH_set_latest_release_status";
	tag_t        rootTaskTag      = NULLTAG;
	int          iTargetCount     = 0;
	tag_t *      pTargetTags      = NULLTAG;
	tag_t 		 tRevClassTag 	  = NULLTAG;
	char * 		 sStatusName      = NULL;
	
	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}
	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );
	
	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
	char sParamObjectType[WSO_name_size_c + 1]   = "";
	char sParamProperty[WSO_name_size_c + 1]   = "";
	
	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Get Param Value
		if ( strTmpParamName.compare("object_type") == 0 )
		{
			strcpy(sParamObjectType, strTmpParamValue.c_str( ) );
		}
		// Get Param Value
		if ( strTmpParamName.compare("props") == 0 )
		{
			strcpy(sParamProperty, strTmpParamValue.c_str( ) );
		}
	}
	
	if ( ( strcmp(sParamObjectType, "") == 0 ) || ( strcmp(sParamProperty, "") == 0 ) )
	{
		printf(" > ERROR: The ActionHandler 'SBT_AH_set_latest_release_status' requires the argument 'object_type' and 'props' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919710) );
		return 919710;
	}

	/* Get Target Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		if(object_is_of_class_or_desc(pTargetTags[iIdx1], sParamObjectType, &tRevClassTag))
		{
			int     iStatusCount         = 0;
			tag_t * taStatusList         = NULL;

			// Get all status
			ITK(WSOM_ask_release_status_list(pTargetTags[iIdx1], &iStatusCount, &taStatusList) );
			
			// Get Latest Status Name
			ITK(AOM_UIF_ask_value(taStatusList[iStatusCount-1], "object_name", &sStatusName) );
	
			ITK( AOM_refresh ( pTargetTags[iIdx1], true ) );
			ITK( AOM_set_value_string( pTargetTags[iIdx1], sParamProperty, sStatusName ) );
			ITK( AOM_save ( pTargetTags[iIdx1] ) );
			ITK( AOM_refresh ( pTargetTags[iIdx1], false ) );
			
			// Free
			SAFE_SM_FREE(sStatusName);
		}
	}
	
	LEAVE(theFunction);
	return retcode;
} /* END SBT_AH_set_latest_release_status */


/* Fills the Name of the Resp Party into a specific Attribute of a specific Object Class. */
int SBT_AH_fill_In_Resp_Party_Name_In_Attr(EPM_action_message_t msg)
{
	const char *theFunction     	= "SBT_AH_fill_In_Resp_Party_Name_In_Attr";
	int         retcode         	= ITK_ok;

	int         iTargetCount    	= 0;

	tag_t       rootTaskTag     	= NULLTAG;
	tag_t		respPartyTag		= NULLTAG;	
	tag_t		classIdTag			= NULLTAG;
	tag_t		tUserTag			= NULLTAG;
	tag_t		tObjectType			= NULLTAG;
	
	tag_t *     pTargetTags     	= NULL;
	
	char 		personname[SA_person_name_size_c+1];
	char 		respPerson[30];

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	
	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
	char sParamObjectType[WSO_name_size_c + 1]   = "";
	char sParamProperty[WSO_name_size_c + 1]   = "";

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
		std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

		// Get Param Value Object Type
		if ( strTmpParamName.compare("object_type") == 0 )
		{
			strcpy(sParamObjectType, strTmpParamValue.c_str( ) );
		}
		// Get Param Value Property Name
		if ( strTmpParamName.compare("props") == 0 )
		{
			strcpy(sParamProperty, strTmpParamValue.c_str( ) );
		}
	}
	
	if ( ( strcmp(sParamObjectType, "") == 0 ) || ( strcmp(sParamProperty, "") == 0 ) )
	{
		printf(" > ERROR: The ActionHandler 'SBT_AH_fill_In_Resp_Party_Name_In_Attr' requires the argument 'object_type' and 'props' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919710) );
		return 919710;
	}
	
	/* Get Reference Attachments */
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	
	/* Get Object Type */
	ITK(TCTYPE_find_type(sParamObjectType, NULL, &tObjectType) );

	/* Avoid Server crash */
	if ( tObjectType == NULLTAG )
	{
		return ITK_ok;
	}
	
	ITK(EPM_ask_responsible_party(msg.task, &respPartyTag));
	ITK(SA_ask_user_person_name(respPartyTag, personname) );

	strncpy(respPerson, personname, 29);
	respPerson[29] = '\0';

	/* Search for Object Type */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		logical lIsGivenType = false;
		ITK(AOM_is_type_of(pTargetTags[iIdx1], tObjectType, &lIsGivenType) );

		/* Save the Person Name into the Attribute */
		if (lIsGivenType)
		{
			ITK(AOM_refresh(pTargetTags[iIdx1], true));
			ITK(AOM_set_value_string(pTargetTags[iIdx1], sParamProperty, respPerson) );
			ITK(AOM_save(pTargetTags[iIdx1]) );
			ITK(AOM_refresh(pTargetTags[iIdx1], false));
		}
	}
	
	/* Free */
	SAFE_SM_FREE(pTargetTags);	
	
	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_fill_In_Resp_Party_Name_In_Attr */

/* 
	Proposed solution to ensure that no information is lost during the type change: 
		1. Change Item ID of the BT7 Product (from SSN to the next free TNS) 
		2. Create a BT6 Product with the SSN previously used by the BT7 Product
		3. Delete the BT7 Product 
*/


int SBT_AH_change_product_type(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_change_product_type";

	/* avoid infinite loops */
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	logical hasByPass = FALSE;
	AM__ask_application_bypass(&hasByPass);
	AM__set_application_bypass(TRUE);

	try 
	{
		tag_t       rootTaskTag		= NULLTAG;
		int         retcode			= ITK_ok;
		int         iTargetCount	= 0;
		tag_t       *taTargetTag		= NULL;

		/* Get RootTask */
		ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

		/* Get the target BT7_Product Item */
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &taTargetTag));

		for(int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++){

			tag_t	tBT7_Product	= taTargetTag[iIdx1];

			/* Ask attributes on BT7_Product */
			char
				*sBT7_ItemID		= NULL,
				*sBT7_ObjectName	= NULL,
				*sBT7_ObjectDesc	= NULL;

			ITK(AOM_ask_value_string(tBT7_Product, "item_id",	  &sBT7_ItemID));
			ITK(AOM_ask_value_string(tBT7_Product, "object_name", &sBT7_ObjectName));
			ITK(AOM_ask_value_string(tBT7_Product, "object_desc", &sBT7_ObjectDesc));

			/* Change the BT7 item_id*/
			char sBT7_newItemID[ITEM_id_size_c + 1];
			strcpy(sBT7_newItemID, sBT7_ItemID);
			strcat(sBT7_newItemID, "_old");
			
			ITK(AOM_refresh(tBT7_Product, TRUE));
			ITK(AOM_set_value_string(tBT7_Product, "item_id", sBT7_newItemID));
			ITK(AOM_save(tBT7_Product));

			/* Create the BT6_Product */
			tag_t	tBT6_ProductType	= NULLTAG;
			tag_t	tBT6_ProductInput	= NULLTAG;
			tag_t	tBT6_Product		= NULLTAG;

			/* Create input */
			ITK(TCTYPE_find_type("BT7_Product", NULL, &tBT6_ProductType));
			ITK(TCTYPE_construct_create_input(tBT6_ProductType, &tBT6_ProductInput));

			/* Set attributes on th BT6_Product*/
			ITK(AOM_set_value_string(tBT6_ProductInput, "item_id", sBT7_ItemID));
			ITK(AOM_set_value_string(tBT6_ProductInput, "object_name", sBT7_ObjectName));
			ITK(AOM_set_value_string(tBT6_ProductInput, "object_desc", sBT7_ObjectDesc));

			SAFE_SM_FREE(sBT7_ItemID);
			SAFE_SM_FREE(sBT7_ObjectName);
			SAFE_SM_FREE(sBT7_ObjectDesc);
			
			int		iRevisionCount		= 0;
			tag_t	*taItemRevision     	= NULL;

			tag_t
				tBT7_ProductRevision		= NULLTAG,
				tBT6_ProductRevisionType	= NULLTAG,
				tBT6_ProductRevisionInput	= NULLTAG,
				tBT6_ProductRevision		= NULLTAG;
		
			char
				*sBT7_RevisionID	= NULL,
				*sBT7_RevisionName	= NULL,
				*sBT7_RevisionDesc	= NULL;
			

			ITK(TCTYPE_find_type("BT7_ProductRevision", NULL, &tBT6_ProductRevisionType));

			/* For each existing revision, Create and update */
			ITK(ITEM_list_all_revs(tBT7_Product, &iRevisionCount, &taItemRevision));
			for (int iIdx2 = 0; iIdx2 < iRevisionCount; iIdx2++){
				tBT7_ProductRevision	= taItemRevision[iIdx2];

				/* Ask attributes on BT7_ProductRevision */
				ITK(AOM_ask_value_string(tBT7_ProductRevision, "item_revision_id", &sBT7_RevisionID));
				ITK(AOM_ask_value_string(tBT7_ProductRevision, "object_name", &sBT7_RevisionName));
				ITK(AOM_ask_value_string(tBT7_ProductRevision, "object_desc", &sBT7_RevisionDesc));

				/* Create the ProductRevision */
				ITK(TCTYPE_construct_create_input(tBT6_ProductRevisionType, &tBT6_ProductRevisionInput));

				/* Set values on the BT6_ProductRevision */
				ITK(AOM_set_value_string(tBT6_ProductRevisionInput, "item_revision_id", sBT7_RevisionID));
				ITK(AOM_set_value_string(tBT6_ProductRevisionInput, "object_name", sBT7_RevisionName));
				ITK(AOM_set_value_string(tBT6_ProductRevisionInput, "object_desc", sBT7_RevisionDesc));

				if(iIdx2 == 0){
					/* Persist the BT6_Product*/
					ITK(AOM_set_value_tag(tBT6_ProductInput, "revision", tBT6_ProductRevisionInput));
					ITK(TCTYPE_create_object(tBT6_ProductInput, &tBT6_Product));
					ITK(ITEM_save_item(tBT6_Product));

				}else{
					
					ITK(AOM_set_value_tag(tBT6_ProductRevisionInput, "items_tag", tBT6_Product));
					ITK(TCTYPE_create_object(tBT6_ProductRevisionInput, &tBT6_ProductRevision));
					ITK(ITEM_save_item(tBT6_ProductRevision));

					ITK(AOM_refresh(tBT6_Product, TRUE));
					ITK(AOM_set_value_tag(tBT6_Product, "revision", tBT6_ProductRevision));
					ITK(ITEM_save_item(tBT6_Product));
					
				}
			}

			SAFE_SM_FREE(taItemRevision);
			SAFE_SM_FREE(sBT7_RevisionID);
			SAFE_SM_FREE(sBT7_RevisionName);
			SAFE_SM_FREE(sBT7_RevisionDesc);


			/* Add the BT6_Product to the reference attachement */
			tag_t	*taAttachments	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
			int		*iaAttachTypes	= (int*)MEM_alloc(1 * sizeof(int));
			taAttachments[0] = tBT6_Product;
			iaAttachTypes[0] = EPM_reference_attachment;

			ITK(EPM_add_attachments(rootTaskTag, 1, taAttachments, iaAttachTypes));
			ITK(AOM_save(rootTaskTag));

			/* Get the Home folder */
			tag_t 
				tHome = NULLTAG, 
				tUser = NULLTAG;
			char 
				*sUserName= NULL;

			ITK(POM_get_user(&sUserName, &tUser));
			ITK(SA_ask_user_home_folder(tUser, &tHome));
			SAFE_SM_FREE(sUserName);

			/* Paste the BT6_Product to the top of the home folder */
			ITK(AOM_refresh(tHome, TRUE));
			ITK(FL_insert(tHome, tBT6_Product, -1));
			ITK(AOM_save(tHome));
			ITK(AOM_unload(tHome));



		} /* End of target iteration */

		SAFE_SM_FREE(taTargetTag);
		AM__set_application_bypass(hasByPass);

		LEAVE(theFunction);
		return retcode;
	} catch( ... )
	{
		LEAVE(theFunction);
	}

} /* SBT_AH_change_product_type */



int SBT_AH_remove_target_attachments(EPM_action_message_t msg)
{

	const char *theFunction        = "SBT_AH_remove_target_attachments";

	/* avoid infinite loops */
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	logical hasByPass = FALSE;
	AM__ask_application_bypass(&hasByPass);
	AM__set_application_bypass(TRUE);

	try 
	{
		tag_t       rootTaskTag		= NULLTAG;
		int         retcode			= ITK_ok;
		int         iTargetCount	= 0;
		tag_t       *taTargetTag	= NULL;

		/* Get RootTask */
		ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

		/* Get the target BT7_Product Item */
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &taTargetTag));

		for(int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++){
			tag_t	tItem	= taTargetTag[iIdx1];

			/* Remove the parent relations */
			/*int iRelationCount = 0;
			GRM_relation_t * taRelations = NULL;
			ITK(GRM_list_primary_objects(tItem, NULLTAG, &iRelationCount, &taRelations));
			for (int iIdx3 = 0; iIdx3 < iRelationCount; iIdx3++){
				ITK(GRM_delete_relation(taRelations[iIdx3].the_relation));
			}*/

			//SAFE_SM_FREE(taRelations);

			// remove from parent folder
			int		iReferencers			= 0,
					*iaReferenceLevels		= NULL;

			tag_t	*taReferencers			= NULL;
			char	**saRelationTypeName	= NULL;

			ITK(WSOM_where_referenced(tItem, 1, &iReferencers, &iaReferenceLevels, &taReferencers, &saRelationTypeName));

			for(int iIdx1 = 0; iIdx1 < iReferencers; iIdx1++)
			{
				tag_t	tReference	= taReferencers[iIdx1];
				char *sRelationTypeName = saRelationTypeName[iIdx1];

				char  type_name[WSO_name_size_c + 1] = "";
				ITK(WSOM_ask_object_type(tReference, type_name));

			    tag_t parent_class = NULLTAG;
				ITK(POM_class_id_of_class("Folder", &parent_class));
			        
				tag_t class_tag = NULLTAG;
				ITK(POM_class_of_instance(tReference, &class_tag));
			    
				logical isFolder = FALSE;
				ITK(POM_is_descendant(parent_class, class_tag, &isFolder));

				if(isFolder == true)
				{
					ITK(FL_remove(tReference, tItem));
					ITK(AOM_save(tReference));
					ITK(AOM_save(tItem));
					ITK(AOM_refresh(tReference, FALSE));
				}
				else if ( (sRelationTypeName != NULL) && (strlen(sRelationTypeName) > 0 ) )
				{
					tag_t relation_type_tag = NULLTAG;
					ITK(GRM_find_relation_type(sRelationTypeName, &relation_type_tag));
				    
					// GRM_delete_relation requires the relation object which we don't have
					tag_t relation_tag = NULLTAG;

					// First try relation using the reference as a primary object
					ITK(GRM_find_relation(tReference, tItem, relation_type_tag, &relation_tag));

										// If no relation is found try the reference as a secondary object
					if (relation_tag == NULLTAG)
					{
						ITK(GRM_find_relation(tItem, tReference, relation_type_tag, &relation_tag));
					}

					if (relation_tag != NULLTAG)
					{
						// Delete relation
						int iErrCode = GRM_delete_relation(relation_tag );
						printf(" > GRM_delete_relation returned error-code = %i \n", iErrCode);
						ITK(EMH_clear_errors());
					}
				}
				else if(strcmp(type_name, "EPMTask") == 0 )
				{
					EPM_state_t state;
					ITK(EPM_ask_state(tReference, &state));

					char state_string[WSO_name_size_c + 1] = "";
					ITK(EPM_ask_state_string(state, state_string));

					if(strcmp(state_string, "Completed") == 0)
					{
						EMH_store_error(EMH_severity_error, 919731);
						return 919731;
					}else{

						tag_t	*taAttachments	= (tag_t*)MEM_alloc(1 * sizeof(tag_t));
						taAttachments[0] = tItem;
						ITK(EPM_remove_attachments(tReference, 1, taAttachments));
					}

					
				}
				SAFE_SM_FREE(sRelationTypeName);
				
			}

			SAFE_SM_FREE(iaReferenceLevels);
			SAFE_SM_FREE(taReferencers);
			SAFE_SM_FREE(saRelationTypeName);
		}


		for(int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++){
			
			tag_t	tItem	= taTargetTag[iIdx1];

			/* Remove the BT7_Product */
			if (isDerivedFromClass("Item", tItem) == true) {

				ITK(AOM_refresh(tItem, TRUE));
				ITK(AOM_delete(tItem));
			}

		} /* End of target iteration */

		SAFE_SM_FREE(taTargetTag);
		AM__set_application_bypass(hasByPass);

		LEAVE(theFunction);
		return retcode;
	} catch( ... )
	{
		LEAVE(theFunction);
	}

} /* SBT_AH_remove_target_attachments */


/*  Checks if the part revision is a part of the ECO solution folder */
EPM_decision_t SBT_RH_part_rev_is_solution_item_of_ECO(EPM_action_message_t msg)
{
	int			ifail				=	ITK_ok;
	int			targetCount			=	0;
	int			number				=	0;
	tag_t		rootTaskTag			=	NULLTAG;
	tag_t		relationTypeTag		=	NULLTAG;
	tag_t		relationTag			=	NULLTAG;
	tag_t		*ecoRevTag			=	NULLTAG;
	tag_t		ecoTag				=	NULLTAG;
	tag_t*		attachmentTag		=	NULL;
	char*		errorText			=	NULL;
	char*		ecoValue			=	NULL;
	tag_t		tCurrentGroupmember	= NULLTAG;
	tag_t		tCurrentGroup		= NULLTAG;
    char		sGroupname[SA_name_size_c + 1];
	EPM_decision_t	decision		= EPM_undecided;
	char		sObjectType[WSO_name_size_c + 1];
	const char	*theFunction		= "SBT_RH_part_rev_is_solution_item_of_ECO";

	ENTER(theFunction);
	decision = EPM_go ;


	
	ITK(EMH_clear_errors( ) );

	ITK(EPM_ask_root_task(msg.task, &rootTaskTag));// get the root task tag
	
	ITK(EPM_ask_attachments(rootTaskTag,EPM_target_attachment,&targetCount,&attachmentTag));// get the all target attachments tag list
	

	// Get Groupname
	ITK(SA_ask_current_groupmember(&tCurrentGroupmember));//get current group member tag
	ITK(SA_ask_groupmember_group(tCurrentGroupmember, &tCurrentGroup));//get current group tag
	ITK(SA_ask_group_name(tCurrentGroup, sGroupname));	//get current group name
	
	//Fix for #0SBT-DO-01PDM4BT|SBTDOP-86 "Serial Release" ECO:: Now iterating all target attachments
	/* Loop over all target attachments */
	for(int inx=0;inx<targetCount;inx++)	
	{
		//ITK(AOM_refresh(attachmentTag[inx],false));

		// Get the object type
		ITK(WSOM_ask_object_type(attachmentTag[inx], sObjectType) );

		if ( ( strcmp ( sGroupname, "110_MDesign_C" ) == 0 ) || ( strcmp ( sGroupname, "110_MDesign_D" ) == 0 ) || ( strcmp ( sGroupname, "Common Component Engineering" ) == 0 ) || ( strcmp ( sGroupname, "100_Library_A" ) == 0 ) || ( strcmp ( sObjectType, "BT7_ToolRevision" ) == 0 ) || ( strcmp ( sObjectType, "BT7_ToolPartRevision" ) == 0 )  )	
		{
		  TC_write_syslog("\n This Group / Item revision type -->%s has exception .... Set status 60 without check ECO relation for \n",sGroupname);
		}
		
		else
		{
			ITK(AOM_ask_value_string(attachmentTag[inx],BT_PROP_BT7_DOCMNUMBER,&ecoValue));//get target revision attribute bt7_docmnumber value

			//if block when Value of bt7_docmnumber is Empty for target revision
			if(strlen(ecoValue) == 0)
			{
				//printf(" > ERROR: The part revision is not present in ECO- solution. Unable to start workflow \n");
				TC_write_syslog(" > ERROR: The part revision is not present in ECO- solution. Unable to start workflow \n");
				decision = EPM_nogo;
				ITK(EMH_store_error(EMH_severity_error, 919732) );
			}	
			/*else block:process WF when Value of bt7_docmnumber is Non-Empty for target revision*/
			else
			{
				ITK(ITEM_find_item(ecoValue,&ecoTag));

				if(ecoTag == NULL)//checks if valid ECO item not found
				{
					//printf(" > ERROR: ECO not found. Unable to start workflow \n");
					TC_write_syslog(" > ERROR: ECO not found. Unable to start workflow \n"); // Tested
					decision = EPM_nogo;
					ITK(EMH_store_error(EMH_severity_error, 919352) );
				}
				/*Fix for SBTDOP-82::else block when valid ECO item found*/
				else					
				{
					//ITK(ITEM_find_revision(ecoTag,"A",&ecoRevTag));
					int		ecoRevCount	=	0;

					ITK(ITEM_list_all_revs(ecoTag,&ecoRevCount,&ecoRevTag));

					//Added logic to check if ECo is in WIP status is yes then continue else throw error
					int				numWFs								=	0;			
					//char**			allWfValues							=	NULL;
					logical			wipECOFlag							=	false;
					logical			value								=	false;
					tag_t*			propValuesTags						=	NULL;
			
					//ITK(AOM_UIF_ask_values(ecoRevTag,TC_MY_WORKFLOW_TASKS,&numWFs,&allWfValues));

					//Get logical value of property fnd0InProcess
					ITK(AOM_ask_value_logical(ecoRevTag[0],TC_PROP_FND0INPROCESS,&value));

					//Get number and tag of values in property fnd0StartedWorkflowTasks
					ITK (AOM_ask_value_tags(ecoRevTag[0],TC_MY_WORKFLOW_TASKS,&numWFs,&propValuesTags));

					// Check associated ECO is in workflow process or not
					if((value)&&(numWFs>0))
					{
						wipECOFlag = true;
					}	
			
					//check if ECO is not in Workflow Process
					if(!wipECOFlag)
					{
						//printf(" >ERROR : ECO must not be opened or finished.\n");
						TC_write_syslog(" > ERROR : ECO must not be opened or finished. \n");
						decision = EPM_nogo;
						ITK(EMH_store_error(EMH_severity_error, 919732) );
					}
					/*Fix for SBTDOP-82::else block to process WF if ECO is in Workflow Process*/
					else
					{
						ITK(GRM_find_relation_type(TC_REL_CMHASSOLUTIONITEM,&relationTypeTag));

						ITK(GRM_find_relation(ecoRevTag[0],attachmentTag[inx],relationTypeTag,&relationTag));

						if(relationTag == NULL && !wipECOFlag)
						{
							//printf(" > ERROR: Item Revision has to be a Solution Item in started ECO. \n");
							TC_write_syslog(" > ERROR: Item Revision has to be a Solution Item in started ECO. \n");
							decision = EPM_nogo;
							ITK(EMH_store_error(EMH_severity_error, 919732) );
						}
					}//end of else block to process data if ECO is in Workflow Process

					/* Memory Handling*/
					MEM_free(propValuesTags);
				}// end of else block when valid ECO item found
			}// end of else block, Value of bt7_docmnumber is Non-Empty for target revision
			MEM_free(ecoValue);
			MEM_free(ecoRevTag);
		}// end of else block, for valid Group

	}


	//MEM_free(ecoRevTag);
	MEM_free(attachmentTag);
	

	LEAVE(theFunction);
	return decision;
}/* SBT_RH_part_rev_is_solution_item_of_ECO */

/**
 * @brief
 * Attach List of secondary object's details with Materials relation to a text file and exit
 * 
 */
EPM_decision_t SBT_RH_Materials_for_release(EPM_rule_message_t msg)
{
	int               retcode          = ITK_ok;
	int               iTargetCount     = 0;
	const char *      theFunction      = "SBT_RH_Materials_for_release";
	char *            sDecision        = EPM_RESULT_True;
	tag_t             rootTaskTag      = NULLTAG;
	tag_t             objectClassIdTag = NULLTAG;
	tag_t *           pTargetTags      = NULLTAG;//(OF)
	logical           lStatusOK        = true;
	logical           lObjectTypeOK    = true;
	std::stringstream txtStringStream;
	char              sObjectTypeTmp[WSO_name_size_c + 1];


	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		// Check allowed target type
		tag_t   tmpDelete;
		logical lTestItemRevision = object_is_of_class_or_desc(pTargetTags[iIdx1], TC_ITEMREVISION, &tmpDelete);

		if (  lTestItemRevision== true )	
		{
			int		iMaterials				 	  = 0;
			tag_t	tRelTypeMaterials	  		  = NULLTAG;
			tag_t*	taMaterialRevisions		   	  = NULL;// (OF)

			ITK( GRM_find_relation_type( TC_REL_MAT1USESMATERIAL, &tRelTypeMaterials ) );

			// List Secondary Objects of source object by relation Materials
			ITK(GRM_list_secondary_objects_only	(pTargetTags[iIdx1], tRelTypeMaterials, &iMaterials,&taMaterialRevisions));	
			
			/*Merged changes of R1.05 started*/	
			for ( int iIdx2 = 0; iIdx2 < iMaterials; iIdx2++ ) {

				char *matObjectId		= NULL;//(OF)

				lObjectTypeOK = false;
				ITK(WSOM_ask_object_id_string(taMaterialRevisions[iIdx2], &matObjectId) );
				//The object <ID-Revision-Name> related with the relation <Mat1UsesMaterial> will not be annulled.
				txtStringStream << "The object <" << matObjectId<< "> related with the relation <"<<"Mat1UsesMaterial"<<"> will not be annulled." << endl;				

				/*Memory free*/
				MEM_free(matObjectId);
			}	
			MEM_free(taMaterialRevisions);
		}		
	}

	// if the decision is false, create a file and attach it to the workflow
	tag_t tNewTextDataset =  NULLTAG;
	if ( ( lObjectTypeOK == false ) || ( lStatusOK == true ) )
	{
		//create a new ImanFile
		tag_t      txtFile     = NULLTAG;
		IMF_file_t txtFileDesc = NULL;

		ITK(IMF_create_file(NULL, SS_TEXT, &txtFile, &txtFileDesc) );
		ITK(IMF_open_file(txtFileDesc, SS_RDWRITE) );

		//write the XML-Stream to the new ImanFile
		std::string txtStringLine = "";

		while ( getline(txtStringStream, txtStringLine, '\n') )
		{
			if ( txtStringLine.length( ) <= SS_MAXLLEN )
			{
				ITK(IMF_write_file_line(txtFileDesc, txtStringLine.c_str( ) ) );
			}
		}

		//Close and save the new ImanFile
		ITK(IMF_close_file(txtFileDesc) );
		ITK(AOM_save(txtFile) );
		ITK(AOM_unload(txtFile) );

		//create the dataset and upload the dataset
		tNewTextDataset = createDataset(msg.task, txtFile, FILE_TITLE);
	}
	
	MEM_free(pTargetTags);
	LEAVE(theFunction);
	return EPM_go;
}

 /**
 * @brief
 * Validate if disptacher should be created for rendering, only if dataset is present in the target
 * 
 */
EPM_decision_t SBT_RH_check_target_attachments_dataset(EPM_rule_message_t msg)
{
	int               retcode          = ITK_ok;
	int               iTargetCount     = 0;
	const char *      theFunction      = "SBT_RH_check_target_attachments_dataset";
	char *            sDecision        = EPM_RESULT_False;
	tag_t             rootTaskTag      = NULLTAG;
	tag_t             objectClassIdTag = NULLTAG;
	tag_t *           pTargetTags      = NULLTAG;//(OF)
	logical           lStatusOK        = true;
	logical           lObjectTypeOK    = true;
	std::stringstream txtStringStream;
	char              sObjectTypeTmp[WSO_name_size_c + 1];
	int ifail							= 0;
	int l_nPreferenceCount				= 0;
	char** l_pcharPreferences			= NULL;
	logical validdatasetfound			= false;

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}


	ITK(PREF_ask_char_values("SBT_TCPB_Dispatcher_Valid_Dataset",&l_nPreferenceCount,&l_pcharPreferences));

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		// Check allowed target type
		tag_t   tmpDelete;
		tag_t  	datasettype ;
		char*  	datasettypeName ;
		
		logical lTestDataset = object_is_of_class_or_desc(pTargetTags[iIdx1], TC_DATASET, &tmpDelete);
		
		if (  lTestDataset== true )	
		{
		 ITK(AE_ask_dataset_datasettype(pTargetTags[iIdx1],&datasettype));
		 ITK(AE_ask_datasettype_name2(datasettype,&datasettypeName));

		 
		 for(int i=0;i<l_nPreferenceCount;i++)
		 {
			 if(tc_strcasecmp(datasettypeName,l_pcharPreferences[i])==0)
			 {				 
				  sDecision = EPM_RESULT_True;
				  validdatasetfound= true;
				  break;
			 }
		 }

		}		
		else
		{
			sDecision = EPM_RESULT_False;
			validdatasetfound = false;
		}

		if(validdatasetfound==true)
		{
			break;
		}
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	LEAVE(theFunction);
	return EPM_go;
}

int SBT_AH_change_object_type( EPM_action_message_t msg ) {
	
	const char *theFunction			= "SBT_AH_change_object_type";
	int         retcode				= ITK_ok;
	int         iTargetCount		= 0;
	tag_t       rootTaskTag			= NULLTAG;
	tag_t *     pTargetTags			= NULL;
	tag_t		sourceItemTag		= NULLTAG;
	tag_t		sourceRevTag		= NULLTAG;
	tag_t		targetItemTag		= NULLTAG;
	tag_t		targetRevTag		= NULLTAG;
	char*		sourceItemType		= "";
	char*		sourceRevType		= "";
	char		targetItemType[WSO_name_size_c + 1] = "";
	char		sourceItemSuffix[WSO_name_size_c + 1] = "";

	// avoid infinite loops
	if ( msg.action > EPM_perform_action ) {
		return ITK_ok;
	}

	ENTER( theFunction);
	ITK( EMH_clear_errors( ) );

	/* Get RootTask */
	ITK( EPM_ask_root_task( msg.task, &rootTaskTag ) );

	/* Get Target Attachments */
	ITK( EPM_ask_attachments( rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags ) );

	/* Read Handler Arguments */
	int iArgCount = IMAN_number_of_arguments( msg.arguments );

	char sParamTargetObjectType[ WSO_object_type_size_c + 1 ] = "";

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ ) {
		// Get Param
		char *      sTmpParam   = IMAN_next_argument( msg.arguments );
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find( "=" );

		// Get Param Name
		std::string strTmpParamName  = strTmpParam.substr( 0, iDelimPos );
		std::string strTmpParamValue = strTmpParam.substr( iDelimPos + 1 );

		// Transform Param Name to lower case
		std::transform( strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower );

		// Get Param Value
		if ( strTmpParamName.compare( "object_type" ) == 0 ) {
			strcpy( targetItemType, strTmpParamValue.c_str( ) );
		}

		// Get Param Value
		if ( strTmpParamName.compare( "suffix" ) == 0 ) {
			strcpy( sourceItemSuffix, strTmpParamValue.c_str( ) );
		}
	}

	if( strcmp( targetItemType, "" ) == 0 )
	{
		printf( " > ERROR: The ActionHandler 'SBT_AH_change_object_type' requires the argument 'object_type' ! \n" );
		ITK( EMH_store_error( EMH_severity_error, 919712 ) );
		return 919712;
	}

	if( strcmp( targetItemType, "" ) == 0 )
	{
		printf( " > ERROR: The ActionHandler 'SBT_AH_change_object_type' requires the argument 'object_type' ! \n" );
		ITK( EMH_store_error( EMH_severity_error, 919712 ) );
		return 919712;
	}

	tag_t objectClassIdTag = NULLTAG;
	if( iTargetCount == 0 ) {
		printf( " > ERROR: The ActionHandler 'SBT_AH_change_object_type' requires to run on at least one target ! \n" );
		ITK( EMH_store_error( EMH_severity_error, 919712 ) );
		return 919712;
	}
	else if( object_is_of_class_or_desc( pTargetTags[0], "Item", &objectClassIdTag ) ) {
		sourceItemTag = pTargetTags[ 0 ];
	}
	else {
		printf( " > ERROR: The ActionHandler 'SBT_AH_change_object_type' requires an item as attachment ! \n" );
		ITK( EMH_store_error( EMH_severity_error, 919712 ) );
		return 919712;
	}

	tag_t	tCurrentGroupMember		= NULLTAG;
	tag_t	tCurrentGroup			= NULLTAG;
	char*   sGroupName				= "";
	logical	lHasBypass				= false;

	// Current user is in Group 'dba' or '100_Admin'?
	ITK( SA_ask_current_groupmember( &tCurrentGroupMember ) );
	ITK( SA_ask_groupmember_group( tCurrentGroupMember, &tCurrentGroup ) );
	ITK( SA_ask_group_name2( tCurrentGroup, &sGroupName ) );

	if( strcmp( sGroupName, "dba" ) != 0 && strcmp( sGroupName, "100_Admin" ) != 0 ) {
		SAFE_SM_FREE( sGroupName );
		printf( " > ERROR: The ActionHandler 'SBT_AH_change_object_type' requires that the User in the Group 'dba' or '100_Admin' ! \n" );
		return 919712;
	}
	SAFE_SM_FREE( sGroupName );
	
	ITK( WSOM_ask_object_type2( sourceItemTag, &sourceItemType ) );
	if( tc_strcmp( sourceItemType, targetItemType ) == 0 ) {
		printf( " > ERROR: The ActionHandler 'SBT_AH_change_object_type' requires an argument 'object_type' that differs from the Item Type of the attached Object ! \n" );
		ITK( EMH_store_error( EMH_severity_error, 919712 ) );
		return 919712;
	}
	
	// Set Bypass
	logical hasByPass = FALSE;
	AM__ask_application_bypass(&hasByPass);
	AM__set_application_bypass(TRUE);
	
	ITK_set_bypass(TRUE);

	/* Create BOM-Window */
	char *sRevisionRule		= "Latest Working";
	tag_t ruleTag			= NULLTAG;
	ITK( CFM_find( sRevisionRule, &ruleTag ) );

	/* Get Rev Properties */
	int sourceRevCount = 0;
	tag_t* sourceRevList = NULL;
	int targetRevCount = 0;
	tag_t* targetRevList = NULL;

	char* targetItemItemID		= "";

	char* targetStringAttr			= "";
	date_t targetDateAttr			= NULLDATE;
	tag_t targetTagAttr				= NULLTAG;

	/* Define Attributes copied from Item */
	vector<char*> vecItemStringAttributes;
	vector<char*> vecItemDateAttributes;
	vector<char*> vecItemTagAttributes;
	vecItemStringAttributes.push_back( "object_name" );
	vecItemStringAttributes.push_back( "object_desc" );
	vecItemTagAttributes.push_back( "owning_user" );
	vecItemTagAttributes.push_back( "owning_group" );

	/* Define Attributes copied from ItemRevision */
	vector<char*> vecItemRevisionStringAttributes;
	vector<char*> vecItemRevisionDateAttributes;
	vector<char*> vecItemRevisionTagAttributes;
	vecItemRevisionStringAttributes.push_back( "item_revision_id" );
	vecItemRevisionStringAttributes.push_back( "object_name" );
	vecItemRevisionStringAttributes.push_back( "object_desc" );
	vecItemRevisionStringAttributes.push_back( "bt7_docmnumber" );
	vecItemRevisionStringAttributes.push_back( "bt7_docmnumber_40" );
	vecItemRevisionStringAttributes.push_back( "bt7_docmnumber_60" );
	vecItemRevisionStringAttributes.push_back( "bt7_docmnumber_98" );
	vecItemRevisionStringAttributes.push_back( "bt7_docchange" );
	//vecItemRevisionStringAttributes.push_back( "bt7_RevLastStatus" );
	//vecItemRevisionStringAttributes.push_back( "bt7_rohs_compliant" );
	vecItemRevisionTagAttributes.push_back( "owning_user" );
	vecItemRevisionTagAttributes.push_back( "owning_group" );

	ITK( ITEM_list_all_revs( sourceItemTag, &sourceRevCount, &sourceRevList ) );

	/* Handle old and new Item_ID */
	ITK( ITEM_ask_id2( sourceItemTag, &targetItemItemID ) );

	tag_t targetRevCreateInputTag = NULLTAG;
	tag_t targetItemCreateInputTag = NULLTAG;
	tag_t targetRevTypeTag = NULLTAG;
	tag_t targetItemTypeTag = NULLTAG;
	char* targetRevType = ( char * ) MEM_alloc( ( strlen( targetItemType ) + strlen( "Revision" ) + 1) * sizeof( char ) );
	char* newSourceItemID = ( char * ) MEM_alloc( ( strlen( targetItemItemID ) + strlen( sourceItemSuffix ) + 1) * sizeof( char ) );

	strcpy(targetRevType, targetItemType);
    strcat(targetRevType, "Revision");
	strcpy(newSourceItemID, targetItemItemID);
    strcat(newSourceItemID, sourceItemSuffix);

	int * attachTypes = ( int * ) MEM_alloc( sourceRevCount * sizeof( int ) );
	for( int iIdx2 = 0; iIdx2 < sourceRevCount; iIdx2++ ) {
		attachTypes[ iIdx2 ] = EPM_target_attachment;
	}
						
	ITK( AOM_refresh( rootTaskTag, true ) );
	ITK( EPM_add_attachments( rootTaskTag, sourceRevCount, sourceRevList, attachTypes ) );
	ITK( AOM_save_without_extensions( rootTaskTag ) );
	ITK( AOM_refresh( rootTaskTag, false ) );

	/* Prepare for modification */
	prepareForModify( sourceItemTag );
	ITK( AOM_refresh( sourceItemTag, true ) );
	for( int iIdx2 = 0; iIdx2 < sourceRevCount; iIdx2++ ) {
		prepareForModify( sourceRevList[ iIdx2 ] );
		ITK( AOM_refresh( sourceRevList[ iIdx2 ], true ) );
	}

	char *sPrefValue[] = {"On"};
	ITK( PREF_set_search_scope( TC_preference_user ) );
	ITK( PREF_set_char_values( "NR_BYPASS", 1, sPrefValue ) );

	/* Set new Item ID */
	ITK( ITEM_set_id( sourceItemTag, newSourceItemID ) );
	ITK( ITEM_save_item( sourceItemTag ) );
	ITK( AOM_refresh( sourceItemTag, false ) );

	/* Delete ODS Registration */
	int isRegistered;
	ITK( IIR_is_item_id_registered( targetItemItemID, &isRegistered, 1400794284 ) );
	if( isRegistered != 0 ) {
		ITK( IIR_unregister_item_id( targetItemItemID, 1400794284 ) );
	}

    /* Construct a CreateInput object for Item Revision */
    ITK( TCTYPE_find_type( targetRevType, NULL, &targetRevTypeTag ) );
    ITK( TCTYPE_construct_create_input( targetRevTypeTag, &targetRevCreateInputTag ) );
    for ( int attrIdx = 0; attrIdx < vecItemRevisionStringAttributes.size( ); attrIdx++ ) {
		ITK( AOM_ask_value_string( sourceRevList[ 0 ], vecItemRevisionStringAttributes.at( attrIdx ), &targetStringAttr ) );
		if( strcmp( targetStringAttr, "" ) != 0 )
			ITK( AOM_set_value_string( targetRevCreateInputTag, vecItemRevisionStringAttributes.at( attrIdx ), targetStringAttr ) );
	}
	if ( targetRevType) MEM_free( targetRevType);

    /* Construct a CreateInput object for Item */
    ITK( TCTYPE_find_type( targetItemType, NULL, &targetItemTypeTag ) );
    ITK( TCTYPE_construct_create_input( targetItemTypeTag, &targetItemCreateInputTag ) );
    ITK( AOM_set_value_string( targetItemCreateInputTag, "item_id", targetItemItemID ) );
	for ( int attrIdx = 0; attrIdx < vecItemStringAttributes.size( ); attrIdx++ ) {
		ITK( AOM_ask_value_string( sourceItemTag, vecItemStringAttributes.at( attrIdx ), &targetStringAttr ) );
		if( strcmp( targetStringAttr, "" ) != 0 )
			ITK( AOM_set_value_string( targetItemCreateInputTag, vecItemStringAttributes.at( attrIdx ), targetStringAttr ) );
	}
    ITK( AOM_set_value_tag( targetItemCreateInputTag, "revision", targetRevCreateInputTag ) );
    ITK( TCTYPE_create_object( targetItemCreateInputTag, &targetItemTag ) );
    ITK( ITEM_save_item( targetItemTag ) );

	/* Create additional Revisions */
	for( int i = 1; i < sourceRevCount; i++ ) {
		targetRevCreateInputTag = NULLTAG;
		ITK( TCTYPE_construct_create_input( targetRevTypeTag, &targetRevCreateInputTag ) );
		for ( int attrIdx = 0; attrIdx < vecItemRevisionStringAttributes.size( ); attrIdx++ ) {
			ITK( AOM_ask_value_string( sourceRevList[ i ], vecItemRevisionStringAttributes.at( attrIdx ), &targetStringAttr ) );
			if( strcmp( targetStringAttr, "" ) != 0 )
				ITK( AOM_set_value_string( targetRevCreateInputTag, vecItemRevisionStringAttributes.at( attrIdx ), targetStringAttr ) );
		}
		ITK( AOM_set_value_tag( targetRevCreateInputTag, "items_tag", targetItemTag ) );
		ITK( TCTYPE_create_object( targetRevCreateInputTag, &targetRevTag ) );
		ITK( ITEM_save_rev( targetRevTag ) );
	}

	char *sPrefValueOff[] = {"Off"};
	ITK( PREF_set_search_scope( TC_preference_user ) );
	ITK( PREF_set_char_values("NR_BYPASS", 1, sPrefValueOff) );

	// Set creation Date on Item
	date_t creationDateForIRwithSuffix;
	ITK( AOM_ask_value_date( sourceItemTag, "creation_date", &creationDateForIRwithSuffix ) );
	prepareForModify( targetItemTag );
	ITK( POM_set_creation_date( targetItemTag, creationDateForIRwithSuffix ) );
	ITK( AOM_save( targetItemTag ) );

	/* Atach old ItemRevision to new ItemRevision and duplicate Relations */
	tag_t	relationTypeTag				= NULLTAG;
	tag_t	newRel						= NULLTAG;
	tag_t	bomAttr						= NULLTAG;
	
	int     sourceRelCount;
	int		sourceBomCount;
	int     sourceReleaseStatusCount;
	int     targetSpecifiedCount;
	
	logical specifiedRelAlreadyExists	= false;

	char* sourceRevID;
	char* targetRevID;

	/* Relations for secondaryObjects */
	vector<char*> vecSecondaryRelationTypes;
	vecSecondaryRelationTypes.push_back("BT7_Rel_Concerned");
	vecSecondaryRelationTypes.push_back("BT7_Includes");
	vecSecondaryRelationTypes.push_back("BT7_Rel_Specified");

	ITK( ITEM_list_all_revs( targetItemTag, &targetRevCount, &targetRevList ) );

	/* Loop over Source Revisions */
	for( int iIdx = 0; iIdx < sourceRevCount; iIdx++ ) {
		ITK( ITEM_ask_rev_id2( sourceRevList[ iIdx ], &sourceRevID ) );
		/* Loop over Target Revisions */
		for( int iIdx1 = 0; iIdx1 < targetRevCount; iIdx1++ ) {
			ITK( ITEM_ask_rev_id2( targetRevList[ iIdx1 ], &targetRevID ) );

			/* Is this the correct new Revision */
			if( strcmp( sourceRevID, targetRevID ) == 0 ) {
				
				// Set creation Date on Item Revision
				date_t creationDateForIRwithSuffix;
				ITK( AOM_ask_value_date( sourceRevList[ iIdx ], "creation_date", &creationDateForIRwithSuffix ) );
				prepareForModify( targetRevList[ iIdx1 ] );
				ITK( POM_set_creation_date( targetRevList[ iIdx1 ], creationDateForIRwithSuffix ) );
				ITK( AOM_save( targetRevList[ iIdx1 ] ) );

				/* Set Attributes */
				ITK( AOM_refresh( targetRevList[ iIdx1 ], true ) );
				for ( int attrIdx = 0; attrIdx < vecItemRevisionDateAttributes.size( ); attrIdx++ ) {
					ITK( AOM_ask_value_date( sourceRevList[ iIdx ], vecItemRevisionDateAttributes.at( attrIdx ), &targetDateAttr ) );
					if( CLOCK_compare_dates( &targetDateAttr, &NULLDATE ) != 0 )
						ITK( AOM_set_value_date( targetRevList[ iIdx1 ], vecItemRevisionDateAttributes.at( attrIdx ), targetDateAttr ) );
				}
				for ( int attrIdx = 0; attrIdx < vecItemRevisionTagAttributes.size( ); attrIdx++ ) {
					ITK( AOM_ask_value_tag( sourceRevList[ iIdx ], vecItemRevisionTagAttributes.at( attrIdx ), &targetTagAttr ) );
					if( targetTagAttr != NULLTAG ) {
						if( tc_strcmp( vecItemRevisionTagAttributes.at( attrIdx ), "owning_user" ) == 0 ) {
							ITK( POM_set_owning_user( targetRevList[ iIdx1 ], targetTagAttr ) );
						}
						else if( tc_strcmp( vecItemRevisionTagAttributes.at( attrIdx ), "owning_group" ) == 0 ) {
							ITK( POM_set_owning_group( targetRevList[ iIdx1 ], targetTagAttr ) );
						}
						else {
							ITK( AOM_set_value_tag( targetRevList[ iIdx1 ], vecItemRevisionTagAttributes.at( attrIdx ), targetTagAttr ) );
						}
					}
				}
				ITK( AOM_save_without_extensions( targetRevList[ iIdx1 ] ) );
				ITK( AOM_refresh( targetRevList[ iIdx1 ], false ) );
				
				/* Duplicate Relations */
				copyDatasets( sourceRevList[ iIdx ], targetRevList[ iIdx1 ] );
				for ( int relIdx = 0; relIdx < vecSecondaryRelationTypes.size( ); relIdx++ ) {
					tag_t * sourceRels = NULL;
					ITK( GRM_find_relation_type( vecSecondaryRelationTypes.at( relIdx ), &relationTypeTag ) );
					ITK( GRM_list_secondary_objects_only( sourceRevList[ iIdx ], relationTypeTag, &sourceRelCount, &sourceRels ) );
					for( int iIdx2 = 0; iIdx2 < sourceRelCount; iIdx2++ ) { 
						ITK( GRM_create_relation( targetRevList[ iIdx1 ], sourceRels[ iIdx2 ], relationTypeTag, NULLTAG, &newRel ) );
						ITK( GRM_save_relation( newRel ) );
						newRel = NULLTAG;
					}
					SAFE_SM_FREE( sourceRels );
				}
				
				/* Replace in Where Used */
				int parentCount;
				int *bomParentLevels;
				tag_t *bomParents;

				/* Where Used */
				ITK( PS_where_used_configured( sourceRevList[ iIdx ], ruleTag, 1, &parentCount, &bomParentLevels, &bomParents));

				for( int parentIdx = 0; parentIdx < parentCount; parentIdx++ ) {
					
					replace_bom_child( bomParents[ parentIdx ], ruleTag, sourceRevList[ iIdx ], targetRevList[ iIdx1 ], NULLTAG );
				}
				SAFE_SM_FREE( bomParents );

				/* Move BOMViewRevision to new Revision */
				tag_t * bomViewObjects;
				tag_t * sourceObjects = ( tag_t * ) MEM_alloc( 1 * sizeof( tag_t ) );
				tag_t * targetObjects = ( tag_t * ) MEM_alloc( 1 * sizeof( tag_t ) );
				sourceObjects[ 0 ] = sourceRevList[ iIdx ];
				targetObjects[ 0 ] = targetRevList[ iIdx1 ];
				// Get BOMview from ItemRevision
				ITK( AOM_ask_value_tags( sourceRevList[ iIdx ], "structure_revisions", &sourceBomCount, &bomViewObjects  ) );
				if( sourceBomCount > 0 ) {
					ITK( AOM_refresh( targetRevList[ iIdx1 ], true ) );
					// Get attributeID
					ITK( POM_attr_id_of_attr( "structure_revisions", "ItemRevision", &bomAttr ) );
					// Add BOMview to new object
					ITK( POM_append_attr_tags( 1, targetObjects, bomAttr, sourceBomCount, bomViewObjects) );
					// Remove BOMview from old object
					ITK( POM_clear_attr( 1, sourceObjects, bomAttr ) );
					// Save
					ITK( AOM_save_without_extensions( sourceRevList[ iIdx ] ) );
					ITK( AOM_save_without_extensions( targetRevList[ iIdx1 ] ) );
					ITK( AOM_refresh( targetRevList[ iIdx1 ], false ) );
				}
				SAFE_SM_FREE( sourceObjects );
				SAFE_SM_FREE( targetObjects );
				SAFE_SM_FREE( bomViewObjects );

				/* Attach old ItemRevision to new ItemRevision */
				ITK( GRM_find_relation_type( "BT7_Rel_Specified", &relationTypeTag ) );
				tag_t * targetSpecifiedRels = NULL;
				ITK( GRM_list_secondary_objects_only( targetRevList[ iIdx1 ], relationTypeTag, &sourceRelCount, &targetSpecifiedRels ) );
				for( int iIdx2 = 0; iIdx2 < sourceRelCount; iIdx2++ ) {
					if ( targetSpecifiedRels[iIdx2] == sourceRevList[ iIdx ] ) {
						specifiedRelAlreadyExists = true;
						break;
					}
				}
				if( specifiedRelAlreadyExists == false ) {
					ITK( GRM_create_relation( targetRevList[ iIdx1 ], sourceRevList[ iIdx ], relationTypeTag, NULLTAG, &newRel ) );
					ITK( GRM_save_relation( newRel ) );
					newRel = NULLTAG;
				}
				SAFE_SM_FREE( targetSpecifiedRels );

				/* Add Status List to new target Revisions */
				tag_t * sourceReleaseStatusList = NULL;
				ITK( WSOM_ask_release_status_list( sourceRevList[ iIdx ], &sourceReleaseStatusCount, &sourceReleaseStatusList ) );
				for( int iIdx2 = 0; iIdx2 < sourceReleaseStatusCount; iIdx2++ ) {
					tag_t * releaseObjects = ( tag_t * ) MEM_alloc( 1 * sizeof( tag_t ) );
					releaseObjects[ 0 ] = targetRevList[ iIdx1 ];
					ITK( RELSTAT_add_release_status( sourceReleaseStatusList[ iIdx2 ], 1, releaseObjects, false ) );
					SAFE_SM_FREE( releaseObjects );
				}
				break;
			}
		}
	}

	/* Set Attributes of new Item */
	for ( int attrIdx = 0; attrIdx < vecItemDateAttributes.size( ); attrIdx++ ) {
		ITK( AOM_ask_value_date( sourceItemTag, vecItemDateAttributes.at( attrIdx ), &targetDateAttr ) );
		if( CLOCK_compare_dates( &targetDateAttr, &NULLDATE ) != 0 )
			ITK( AOM_set_value_date( targetItemTag, vecItemDateAttributes.at( attrIdx ), targetDateAttr ) );
	}
    for ( int attrIdx = 0; attrIdx < vecItemTagAttributes.size( ); attrIdx++ ) {
		ITK( AOM_ask_value_tag( sourceItemTag, vecItemTagAttributes.at( attrIdx ), &targetTagAttr ) );
		if( targetTagAttr != NULLTAG ) {
			if( tc_strcmp( vecItemTagAttributes.at( attrIdx ), "owning_user" ) == 0 ) {
				ITK( POM_set_owning_user( targetItemTag, targetTagAttr ) );
			}
			else if( tc_strcmp( vecItemTagAttributes.at( attrIdx ), "owning_group" ) == 0 ) {
				ITK( POM_set_owning_group( targetItemTag, targetTagAttr ) );
			}
			else {
				ITK( AOM_set_value_tag( targetItemTag, vecItemTagAttributes.at( attrIdx ), targetTagAttr ) );
			}
		}
	}

	/* Add Status List to new target Item */
	tag_t * sourceReleaseStatusList = NULL;
	ITK( WSOM_ask_release_status_list( sourceItemTag, &sourceReleaseStatusCount, &sourceReleaseStatusList ) );
	for( int iIdx = 0; iIdx < sourceReleaseStatusCount; iIdx++ ) {
		tag_t * releaseObjects = ( tag_t * ) MEM_alloc( 1 * sizeof( tag_t ) );
		releaseObjects[ 0 ] = targetItemTag;
		ITK( RELSTAT_add_release_status( sourceReleaseStatusList[ iIdx ], 1, releaseObjects, false ) );
		SAFE_SM_FREE( releaseObjects );
	}

	/* Add Annulling Status to Source Revisions */
	tag_t annulledStatus = NULLTAG;
	ITK( RELSTAT_create_release_status( STATUS_TYPE_98, &annulledStatus ) );
	ITK( RELSTAT_add_release_status( annulledStatus, sourceRevCount, sourceRevList, true ) );

	/* Add Annulling Status to Source Item */
	tag_t * sourceItemList = ( tag_t * ) MEM_alloc( 1 * sizeof( tag_t ) );
	sourceItemList[ 0 ] = sourceItemTag;
	ITK( RELSTAT_add_release_status( annulledStatus, 1, sourceItemList, true ) );

	AOM_save_without_extensions( targetItemTag );

	/* Object is now created. Lets switch the old to the new in Relations and Occurrences */
	tag_t relationToSwitch = NULLTAG;

	/* Relations for primaryObjects */
	vector<char*> vecPrimaryRelationTypes;
	vecPrimaryRelationTypes.push_back("BT7_Rel_Concerned");
	vecPrimaryRelationTypes.push_back("BT7_Includes");
	vecPrimaryRelationTypes.push_back("IMAN_specification");
	vecPrimaryRelationTypes.push_back("TC_Attaches");
	vecPrimaryRelationTypes.push_back("BT7_Rel_Specified");

	/* First lets switch Item References */
	/* Loop over Item Relations */
	for ( int relIdx = 0; relIdx < vecPrimaryRelationTypes.size( ); relIdx++ ) {
		tag_t * sourceRels = NULL;
		ITK( GRM_find_relation_type( vecPrimaryRelationTypes.at( relIdx ), &relationTypeTag ) );
		ITK( GRM_list_primary_objects_only( sourceItemTag, relationTypeTag, &sourceRelCount, &sourceRels ) );
					
		if( sourceRelCount != 0 ) {
			/* Attach Primary Object to the Workflow References, to gain Access */
			tag_t * wfRefAttachments = ( tag_t * ) MEM_alloc( sourceRelCount * sizeof( tag_t ) );
			int * attachTypes = ( int * ) MEM_alloc( sourceRelCount * sizeof( int ) );
			for( int iIdx2 = 0; iIdx2 < sourceRelCount; iIdx2++ ) {
				wfRefAttachments[ iIdx2 ] = sourceRels[ iIdx2 ];
				attachTypes[ iIdx2 ] = EPM_target_attachment;
			}
			ITK( AOM_refresh( rootTaskTag, true ) );
			ITK( EPM_add_attachments( rootTaskTag, sourceRelCount, wfRefAttachments, attachTypes ) );
			ITK( AOM_save_without_extensions( rootTaskTag ) );
			ITK( AOM_refresh( rootTaskTag, false ) );
			/* Create new Relation to new Object and remove old Relation */
			for( int iIdx2 = 0; iIdx2 < sourceRelCount; iIdx2++ ) {
				prepareForModify( sourceRels[ iIdx2 ] );
				ITK( AOM_refresh( sourceRels[ iIdx2 ], true ) );
				ITK( GRM_create_relation( sourceRels[ iIdx2 ], targetItemTag, relationTypeTag, NULLTAG, &newRel ) );
				ITK( GRM_save_relation( newRel ) );
				newRel = NULLTAG;
				ITK( GRM_find_relation( sourceRels[ iIdx2 ], sourceItemTag, relationTypeTag, &relationToSwitch ) );
				if( relationToSwitch != NULLTAG ) {
					ITK( GRM_delete_relation( relationToSwitch ) );
				}
				ITK( AOM_save_without_extensions( sourceRels[ iIdx2 ] ) );
				ITK( AOM_refresh( sourceRels[ iIdx2 ], false ) );
			}
			/* Remove Primary Objects from Workflow Reference */
			ITK( EPM_remove_attachments( rootTaskTag, sourceRelCount, wfRefAttachments ) );
			ITK( AOM_save_without_extensions( rootTaskTag ) );
			SAFE_SM_FREE( wfRefAttachments );
			SAFE_SM_FREE( attachTypes );
		}
		SAFE_SM_FREE( sourceRels );
	}

	/* And now the Item Revision References */
	/* Loop over Source Revisions */
	for( int iIdx = 0; iIdx < sourceRevCount; iIdx++ ) {
		ITK( ITEM_ask_rev_id2( sourceRevList[ iIdx ], &sourceRevID ) );
		/* Loop over Target Revisions */
		for( int iIdx1 = 0; iIdx1 < targetRevCount; iIdx1++ ) {
			ITK( ITEM_ask_rev_id2( targetRevList[ iIdx1 ], &targetRevID ) );
			/* Is this the correct new Revision */
			if( strcmp( sourceRevID, targetRevID ) == 0 ) {
				/* Loop over Relations */
				for ( int relIdx = 0; relIdx < vecSecondaryRelationTypes.size( ); relIdx++ ) {
					tag_t * sourceRels = NULL;
					ITK( GRM_find_relation_type( vecSecondaryRelationTypes.at( relIdx ), &relationTypeTag ) );
					ITK( GRM_list_primary_objects_only( sourceRevList[ iIdx ], relationTypeTag, &sourceRelCount, &sourceRels ) );
					if( sourceRelCount != 0 ) {
						/* Attach Primary Object to the Workflow References, to gain Access */
						tag_t * wfRefAttachments = ( tag_t * ) MEM_alloc( sourceRelCount * sizeof( tag_t ) );
						int * attachTypes = ( int * ) MEM_alloc( sourceRelCount * sizeof( int ) );
						for( int iIdx2 = 0; iIdx2 < sourceRelCount; iIdx2++ ) {
							wfRefAttachments[ iIdx2 ] = sourceRels[ iIdx2 ];
							attachTypes[ iIdx2 ] = EPM_target_attachment;
						}
						ITK( AOM_refresh( rootTaskTag, true ) );
						ITK( EPM_add_attachments( rootTaskTag, sourceRelCount, wfRefAttachments, attachTypes ) );
						ITK( AOM_save_without_extensions( rootTaskTag ) );
						ITK( AOM_refresh( rootTaskTag, false ) );
						/* Create new Relation to new Object and remove old Relation */
						for( int iIdx2 = 0; iIdx2 < sourceRelCount; iIdx2++ ) {
							/* Jump over old in new Revision Relation */
							if( sourceRels[ iIdx2 ] != targetRevList[ iIdx1 ] ) {
								prepareForModify( sourceRels[ iIdx2 ] );
								ITK( AOM_refresh( sourceRels[ iIdx2 ], true ) );
								ITK( GRM_create_relation( sourceRels[ iIdx2 ], targetRevList[ iIdx1 ], relationTypeTag, NULLTAG, &newRel ) );
								ITK( GRM_save_relation( newRel ) );
								newRel = NULLTAG;
								ITK( AOM_save_without_extensions( sourceRels[ iIdx2 ] ) );
								ITK( GRM_find_relation( sourceRels[ iIdx2 ], sourceRevList[ iIdx ], relationTypeTag, &relationToSwitch ) );
								if( relationToSwitch != NULLTAG ) {
									ITK( GRM_delete_relation( relationToSwitch ) );
								}
								ITK( AOM_save_without_extensions( sourceRels[ iIdx2 ] ) );
								ITK( AOM_refresh( sourceRels[ iIdx2 ], false ) );
							}
						}
						/* Remove Primary Objects from Workflow Reference */
						ITK( EPM_remove_attachments( rootTaskTag, sourceRelCount, wfRefAttachments ) );
						SAFE_SM_FREE( wfRefAttachments );
						SAFE_SM_FREE( attachTypes );
					}
					SAFE_SM_FREE( sourceRels );
				}
			}
		}
	}

	for( int iIdx2 = 0; iIdx2 < sourceRevCount; iIdx2++ ) {
		ITK( AOM_refresh( sourceRevList[ iIdx2 ], false ) );
	}
	SAFE_SM_FREE( sourceRevList );
	SAFE_SM_FREE( targetRevList );

	AM__set_application_bypass( hasByPass );

	LEAVE( theFunction );
	return retcode;
}

