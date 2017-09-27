#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

//#include <string.h>

#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <tccore/method.h>
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
#include <ics/ics2.h>
#include <form/form.h>
#include <tccore/method.h>
#include <tccore/item_msg.h>
#include <tc/iman_arguments.h>
#include <ae/dataset.h>
#include <objio/objio.h>
#include <dispatcher/dispatcher_itk.h>
#include <sa/am_bypass.h>
#include <tccore/project.h>
#include <vm/vms.h>
#include <sa/am_bypass.h>
#include <res/res_itk.h>
#include <sa/am.h>

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

#include <iostream>
#include <iomanip>

// Include Common
#include "SBT_Common.h"

// --------------------------------------------------------------------------

std::string ccNameHelper_TrimLeadingZerosFromNumberString(std::string sString);
int SBT_generateVendorParts(char *sCCRMfgNumber, tag_t object, char sMfgItemId[ITEM_id_size_c + 1], tag_t tCommonCompItemRev);
//logical isDerivedFromClass(char *className, tag_t tObjectTag);

// --------------------------------------------------------------------------

struct CC_NAME_PATTERN_PARAMETER_ATTR_RULE_ENTRY
{
	std::string sRuleId;
	double      dRuleMin;
	double      dRuleMax;
	double      dRuleFactor;
	std::string sRulePrefix;
	std::string sRuleSuffix;
};

struct CC_NAME_PATTERN_PARAMETER_ATTR_RULE
{
	std::string                                       sRuleID;
	std::string                                       sRuleDesc;
	vector<CC_NAME_PATTERN_PARAMETER_ATTR_RULE_ENTRY> vecCcNamePatternRuleEntries;
};

struct CC_NAME_PATTERN_PARAMETER_LOV
{
	std::string sLovValue;
	std::string sLovShortValue;
};

struct CC_NAME_PATTERN_PARAMETER
{
	std::string                           sParamID;
	std::string                           sSource;
	std::string                           sValueEN;
	std::string                           sValueGER;
	std::string                           sICS_AttrID;
	std::string                           sDesc;
	std::string                           sUsedForNameGen;
	std::string                           sCadimAttrName;
	double                                dNormalizationFactor;
	double                                dCadimFactor;
	CC_NAME_PATTERN_PARAMETER_ATTR_RULE   ccNamePatternAttrRule;
	vector<CC_NAME_PATTERN_PARAMETER_LOV> vecCcNamePatternParameterLovEntries;
};

struct CC_NAME_PATTERN
{
	string                            sTcUAID;
	string                            sCadimID;
	string                            sDesc;
	vector<CC_NAME_PATTERN_PARAMETER> vecCcNamePatternParameters;
};

struct TC_ICS_VALUES
{
	string sICSAttrID;
	string sICSAttrValue;
};

vector<CC_NAME_PATTERN> vecCcNamePatterns;
vector<CC_NAME_PATTERN_PARAMETER_ATTR_RULE> vecCcNamePatternAttrRules;
char  sPathToConfigFile[256];
char *sLogDir = NULL;

// --------------------------------------------------------------------------

/**
* @brief
* Verify if the current user is a privileged member of the project defined by the parameter 'projectid'
*/
EPM_decision_t SBT_RH_isPrivilegedMemberOfProject(EPM_rule_message_t msg)
{
	int         retcode          = ITK_ok;
	const char *theFunction      = "SBT_RH_isPrivilegedMemberOfProject";
	char *      sDecision        = EPM_RESULT_False;

	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
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
		if ( strTmpParamName.compare("projectid") == 0 )
		{
			strcpy(sParamProjectId, strTmpParamValue.c_str( ) );
		}
	}

	if ( ( strcmp(sParamProjectId, "") == 0 ) )
	{
		printf(" > ERROR: The RuleHandler 'SBT_RH_isPrivilegedMemberOfProject' requires the argument 'projectid' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919719) );
		return EPM_nogo;
	}

	tag_t tProject = NULLTAG;

	// Find Project
	ITK(PROJ_find(sParamProjectId, &tProject));

	if(tProject != NULLTAG)
	{
		tag_t tCurrentGroupmember	= NULLTAG;
		tag_t tUser					= NULLTAG;
		logical	lIsPrivilegedMember	= NULLTAG;

		// Is privileged member of the project ?
		ITK(SA_ask_current_groupmember(&tCurrentGroupmember));
		ITK(SA_ask_groupmember_user(tCurrentGroupmember, &tUser));
		ITK(PROJ_is_user_a_privileged_member(tProject, tUser, &lIsPrivilegedMember));

		// Set decision result
		if (lIsPrivilegedMember == true)
		{
			sDecision = EPM_RESULT_True;
		}

	} else {
		printf(" The project %s could not be found! \n", sParamProjectId);
		ITK(EMH_store_error_s1(EMH_severity_error, 919720, sParamProjectId) );
		return EPM_nogo;
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
 * Verifies if the form BT7_CCR contains at least one Manufacturer plus if the relation
 * to the Manufacturer contains the information of the Mfg. Part Number and Part Name.
 * Verifies if the CCR or the CC is classified
 */
EPM_decision_t SBT_RH_checkCCRConformity(EPM_rule_message_t msg)
{
	int         retcode          = ITK_ok;
	int         iTargetCount     = 0;
	int         iIdx1            = 0;
	int         iIdx2            = 0;
	const char *theFunction      = "SBT_RH_checkCCRConformity";
	char *      sDecision        = EPM_RESULT_False;
	tag_t       rootTaskTag      = NULLTAG;
	tag_t       objectClassIdTag = NULLTAG;
	tag_t *     pTargetTags      = NULLTAG;

	// Validation variables
	logical lIsCommonCompClassified = false;
	logical lMfgCorrect             = false;
	logical lIsCCRClassified        = false;
	logical lDatasheetAttached      = false;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get target attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( iIdx1 = 0; ( retcode == ITK_ok ) && ( iIdx1 < iTargetCount ); iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, "BT7_CommonComp") == 0 )
		{
			// Check classification
			ITK(ICS_is_wsobject_classified(pTargetTags[iIdx1], &lIsCommonCompClassified) );
		}

		if ( strcmp(sObjectType, "BT7_CCR") == 0 )
		{
			int iManufacturerCount       = 0;
			int iExternalObjectLinkCount = 0;

			tag_t  tRelTypeCcrManufacturers   = NULLTAG;
			tag_t  tRelTypeExternalObjectLink = NULLTAG;
			tag_t *taManufacturers            = NULL;
			tag_t *taExternalObjectLinks      = NULL;

			// Check classification
			ITK(ICS_is_wsobject_classified(pTargetTags[iIdx1], &lIsCCRClassified) );

			// Check manufacturers
			ITK(GRM_find_relation_type("BT7_CCRManufacturer", &tRelTypeCcrManufacturers) );
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeCcrManufacturers, &iManufacturerCount, &taManufacturers) );

			for ( iIdx2 = 0; iIdx2 < iManufacturerCount; iIdx2++ )
			{
				tag_t tRelationInstance = NULLTAG;
				ITK(GRM_find_relation(pTargetTags[iIdx1], taManufacturers[iIdx2], tRelTypeCcrManufacturers, &tRelationInstance) );

				if ( tRelationInstance != NULLTAG )
				{
					char *sCCRMfgNumber1 = "";
					char *sCCRMfgNumber2 = "";

					ITK(AOM_ask_value_string(tRelationInstance, "bt7_CCR_MfgPrtNumber", &sCCRMfgNumber1) );
					ITK(AOM_ask_value_string(tRelationInstance, "bt7_CCR_MfgPrtNumber2", &sCCRMfgNumber2) );

					// If MfgPrtNumber1 not empty and MfgPrtNumber1 is not equal to MfgPrtNumber2: success
					if ( ( strcmp(sCCRMfgNumber1, sCCRMfgNumber2) != 0 ) && ( strcmp(sCCRMfgNumber1, "") != 0 ) )
					{
						lMfgCorrect = true;
					} else {
						lMfgCorrect = false;
					}

					SAFE_SM_FREE(sCCRMfgNumber1);
					SAFE_SM_FREE(sCCRMfgNumber2);
				}

				// After the first negative instance, exit the loop
				if ( lMfgCorrect == false )
				{
					break;
				}
			}

			// Check Datasheets
			ITK(GRM_find_relation_type("IMAN_external_object_link", &tRelTypeExternalObjectLink) );
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeExternalObjectLink, &iExternalObjectLinkCount, &taExternalObjectLinks) );
			//ITK(ITK_set_bypass(true) );
			for ( iIdx2 = 0; iIdx2 < iExternalObjectLinkCount; iIdx2++ )
			{
				tag_t tObjectClassId = NULLTAG;

				// Is derived from Dataset ?
				if ( object_is_of_class_or_desc(taExternalObjectLinks[iIdx2], "Dataset", &tObjectClassId) )
				{
					logical lIsCheckedOut = false;

					lDatasheetAttached = true;					

					// Fix for #BTQ00282353 - automatically check-in for checked-out documents in CCR::Checkin objects if checked-out
					ITK(RES_is_checked_out(taExternalObjectLinks[iIdx2], &lIsCheckedOut) );

					if ( lIsCheckedOut == true )
					{
						ITK(RES_checkin(taExternalObjectLinks[iIdx2]) );
					}// Fix for #BTQ00282353 ends

				}//ends Is derived from Dataset ?
			}//end of for loop

			// Free
			SAFE_SM_FREE(taManufacturers);
			SAFE_SM_FREE(taExternalObjectLinks);
		}
	}

	// Set validation result ( Manufacturer assigned, CCR or CC classified, Datasheet attached )
	if ( ( lMfgCorrect == true ) && ( ( lIsCCRClassified == true ) || ( lIsCommonCompClassified == true ) ) && ( lDatasheetAttached == true ) )
	{
		sDecision = EPM_RESULT_True;
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	// Free
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
}


/**
 * @brief
 * Read the Manufacturer information and generate the Commercial Part + Vendor Part
 */
int SBT_AH_generateCCRObjects(EPM_action_message_t msg)
{
	int         retcode          = ITK_ok;
	int         iTargetCount     = 0;
	int         iIdx1            = 0;
	int         iIdx2            = 0;
	const char *theFunction      = "SBT_AH_generateCCRObjects";
	tag_t       rootTaskTag      = NULLTAG;
	tag_t       objectClassIdTag = NULLTAG;
	tag_t *     pTargetTags      = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get target attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( iIdx1 = 0; ( retcode == ITK_ok ) && ( iIdx1 < iTargetCount ); iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, "BT7_CCR") == 0 )
		{
			int    iSecCount          = 0;
			tag_t *taSecObjects       = NULL;
			tag_t  tRelationType      = NULLTAG;
			tag_t  tCommonCompItem    = NULLTAG;
			tag_t  tCommonCompItemRev = NULLTAG;

			// Find attached Manufacturers
			ITK(GRM_find_relation_type("BT7_CCRManufacturer", &tRelationType) );
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelationType, &iSecCount, &taSecObjects) );


			// Find existing temporary CommercialPart and attach the CCR to it
			int    iTaskAttachmentCount = 0;
			tag_t *taTaskAttachments    = NULL;
			tag_t  tRel_Comm_CCR        = NULLTAG;
			tag_t  tRelReference        = NULLTAG;

			ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment /*EPM_reference_attachment*/, &iTaskAttachmentCount, &taTaskAttachments) );

			// Find target-CC
			for ( iIdx2 = 0; iIdx2 < iTaskAttachmentCount; iIdx2++ )
			{
				char sObjectType[WSO_name_size_c + 1] = "";

				ITK(WSOM_ask_object_type(taTaskAttachments[iIdx2], sObjectType) );

				if ( strcmp(sObjectType, "BT7_CommonComp") == 0 )
				{
					tCommonCompItem = taTaskAttachments[iIdx2];
					break;
				}
			}

			// Temp Common Comp not found: error
			if ( tCommonCompItem == NULLTAG )
			{
				retcode = 919704;
				break;
			}

			// Find Lastest Revision
			ITK(ITEM_ask_latest_rev(tCommonCompItem, &tCommonCompItemRev) );

			// Find Relation Type
			ITK(GRM_find_relation_type("IMAN_reference", &tRelReference) );

			// Relation Already Exists?
			ITK(GRM_find_relation(tCommonCompItem, pTargetTags[iIdx1], tRelReference, &tRel_Comm_CCR) );

			if ( tRel_Comm_CCR == NULLTAG )
			{
				// Relation does not exists -> create
				ITK(GRM_create_relation(tCommonCompItem, pTargetTags[iIdx1], tRelReference, NULLTAG, &tRel_Comm_CCR) );
				ITK(GRM_save_relation(tRel_Comm_CCR) );
			}
			else
			{
				// Relation alrady exists -> do not create
			}

			// Free
			SAFE_SM_FREE(taTaskAttachments);

			// Loop over Manufacturers
			for ( iIdx2 = 0; iIdx2 < iSecCount; iIdx2++ )
			{
				tag_t tRelationInstance = NULLTAG;
				char  sMfgItemId[ITEM_id_size_c + 1];

				ITK(ITEM_ask_id(taSecObjects[iIdx2], sMfgItemId) );

				ITK(GRM_find_relation(pTargetTags[iIdx1], taSecObjects[iIdx2], tRelationType, &tRelationInstance) );

				if ( tRelationInstance != NULLTAG )
				{
					char * sCCRMfgNumber1    = "";
					char * sCCRMfgNumber2    = "";

					ITK(AOM_ask_value_string(tRelationInstance, "bt7_CCR_MfgPrtNumber", &sCCRMfgNumber1) );
					ITK(AOM_ask_value_string(tRelationInstance, "bt7_CCR_MfgPrtNumber2", &sCCRMfgNumber2) );

					// If MfgPrtName and MfgPrtNumber not empty: error
					if ( strcmp(sCCRMfgNumber1, "") == 0 )
					{
						retcode = 919702;
						break;
					}
					
					if ( strcmp(sCCRMfgNumber1, sCCRMfgNumber2) == 0 )
					{
						retcode = 919702;
						break;
					}

					SBT_generateVendorParts(sCCRMfgNumber1, taSecObjects[iIdx2], sMfgItemId, tCommonCompItemRev);

					if ( strcmp(sCCRMfgNumber2, "") != 0 )
					{
						SBT_generateVendorParts(sCCRMfgNumber2, taSecObjects[iIdx2], sMfgItemId, tCommonCompItemRev);
					}

					/** Attach Commercial Part to the Workflow References
					 * tag_t *taWfRefAttachments = (tag_t *) MEM_alloc(1 * sizeof(tag_t));
					 * int *iaAttachTypes = (int*)MEM_alloc ( 1 * sizeof(int) ) ;
					 *
					 * taWfRefAttachments[0] = tCommonCompItem;
					 * iaAttachTypes[0] = EPM_reference_attachment;
					 * ITK ( EPM_add_attachments ( rootTaskTag, 1, taWfRefAttachments, iaAttachTypes ) );
					 *
					 * // Add Commercial Part to Newstuff folder
					 * ITK ( FL_user_update_newstuff_folder ( tCommonCompItem ) );
					 */

					// Free
					SAFE_SM_FREE(sCCRMfgNumber1);
					SAFE_SM_FREE(sCCRMfgNumber2);

					//SAFE_SM_FREE ( taWfRefAttachments );
					//SAFE_SM_FREE ( iaAttachTypes );
				}
			}

			SAFE_SM_FREE(taSecObjects);
		}
	}

	// Free
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_generateCCRObjects */


/**
 * Generate the Vendor Parts from the Relation Information on the CCR-Manufacturer
 */
int SBT_generateVendorParts(char *sCCRMfgNumber, tag_t object, char sMfgItemId[ITEM_id_size_c + 1], tag_t tCommonCompItemRev)
{
	int		ifail = ITK_ok;
	char   sVendorPartItemId[ITEM_id_size_c + 1] = "";
	int    iItemsFound       = 0;
	int	   vendorPartCount   = 1;
	tag_t *taItemsFound      = NULL;
	tag_t  tVendorItem       = NULLTAG;
	tag_t  tVendorItemRev    = NULLTAG;
	tag_t  tRelVMRepresents  = NULLTAG;
	tag_t  tRelVendorPartRel = NULLTAG;
	tag_t  tRel_Comm_VP      = NULLTAG;
	tag_t  tRel_CommRev_VP   = NULLTAG;
	tag_t  tRel_VP_Mfg       = NULLTAG;
	int		attachCount 	 = NULL;
	tag_t	*attachList		 = NULL;
	char *objectType 		 = NULL;
	// Generate VendorPart ItemID
	strcat(sVendorPartItemId, sCCRMfgNumber);
	//strcat(sVendorPartItemId, "-"); // Comment Vendor part 
    //strcat(sVendorPartItemId, sMfgItemId); // Comment Vendor part

	// Verify VendorPart ItemID
	char **attrs  = (char **) MEM_alloc(1 * sizeof( char * ) );
	char **values = (char **) MEM_alloc(1 * sizeof( char * ) );
	attrs[0]  = "item_id";
	values[0] = sVendorPartItemId;

	ITK(ITEM_find_items_by_key_attributes(1, (const char **) attrs, (const char **) values, &iItemsFound, &taItemsFound) );

	// If Vendor Part does not exist, create and relate to Vendor
	if ( iItemsFound == 0 )
	{
		// Create VendorPart		
		tag_t commPartTag = NULLTAG;
		tag_t commPartRevTag = NULLTAG;
		char* errorText =NULL;

		printf("\n\nsVendorPartItemId=[%s]\n", sVendorPartItemId);
		ifail = VMS_create_manufacturer_part(
                                               sVendorPartItemId,
                                               "",
                                               NULLTAG,
                                               "-",
                                               "BT7_VendorPart",
                                               "A",
                                               false,
                                               0,
                                               object,
                                               commPartTag,
                                               commPartRevTag,
                                               &tVendorItem,
                                               &tVendorItemRev
                                            );

        if( ifail != ITK_ok )
        {
            printf("\n Manufacturer Part creation failed");
			EMH_ask_error_text(ifail, &errorText);
			printf("\n\n2. Error message: %s\n", errorText);
        }
        else
        {
			printf("\n Manufacturer Part created successfully");
			printf("\n Manufacturer Part tag = %d",tVendorItem);

			// Issue 913. Vendor Partnummern not set. Set Vendor Part number.
			ITK(AOM_refresh(tVendorItem, true));
			ITK(AOM_refresh(tVendorItemRev, true));			
			// Set Vendor PArt Number

			ITK( GRM_list_secondary_objects_only( tVendorItem, NULLTAG, &attachCount, &attachList ) );

			for ( int indey = 0; indey < attachCount; indey++ )
			{
				ITK( AOM_ask_value_string( attachList[indey], "object_type", &objectType ) );
				if( strcmp(objectType, "BT7_VendorPartMaster") == 0 )
				{
					ITK( AOM_set_value_string( attachList[indey], "vendor_part_num", sCCRMfgNumber ) );
				}				
			}

			ITK(AOM_save(tVendorItem) );
			ITK(AOM_save(tVendorItemRev) );
			ITK(AOM_refresh(tVendorItem, false));
			ITK(AOM_refresh(tVendorItemRev, false));

        }
		MEM_free(attachList);
		MEM_free(errorText);
	}
	else
	{
		tVendorItem = taItemsFound[0];
	}

	// Relate Vendor Part to Commercial Part Revision
	int     iExistingVendorPartCount;
	tag_t * taExistingVendorParts            = NULL;
	logical lVendorPartRelationAlreadyExists = false;

	ITK(GRM_find_relation_type("VMRepresents", &tRelVMRepresents) );
	ITK(GRM_list_secondary_objects_only(tCommonCompItemRev, tRelVMRepresents, &iExistingVendorPartCount, &taExistingVendorParts) );

	for ( int iIdx3 = 0; iIdx3 < iExistingVendorPartCount; iIdx3++ )
	{
		if ( taExistingVendorParts[iIdx3] == tVendorItem )
		{
			lVendorPartRelationAlreadyExists = true;
			break;
		}
	}

	if ( lVendorPartRelationAlreadyExists == false )
	{
		ITK(GRM_create_relation(tCommonCompItemRev, tVendorItem, tRelVMRepresents, NULLTAG, &tRel_Comm_VP) );
		ITK(GRM_save_relation(tRel_Comm_VP) );
	}
	SAFE_SM_FREE(taItemsFound);
	SAFE_SM_FREE(taExistingVendorParts);

	return 0;
}
/* End SBT_generateVendorParts */




/**
 * @brief
 * Generate a temporary Commercial Part
 */
int SBT_AH_generateTempCCObjects(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction        = "SBT_AH_generateTempCCObjects";
	int         retcode            = ITK_ok;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       tCommonCompItem    = NULLTAG;
	tag_t       tCommonCompItemRev = NULLTAG;

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

	// Create CommercialPart
	ITK(ITEM_create_item(NULL, sWorkflowName, "BT7_CommonComp", NULL, &tCommonCompItem, &tCommonCompItemRev) );

	// Attach Commercial Part Item & ItemRevision to the Workflow References
	tag_t *taWfRefAttachments = (tag_t *) MEM_alloc(2 * sizeof( tag_t ) );
	int *  iaAttachTypes      = (int *) MEM_alloc(2 * sizeof( int ) );

	taWfRefAttachments[0] = tCommonCompItem;
	iaAttachTypes[0]      = EPM_target_attachment;         //EPM_reference_attachment;

	taWfRefAttachments[1] = tCommonCompItemRev;
	iaAttachTypes[1]      = EPM_target_attachment;         //EPM_reference_attachment;

	ITK(EPM_add_attachments(rootTaskTag, 2, taWfRefAttachments, iaAttachTypes) );

	// Add Commercial Part to Newstuff folder
	ITK(FL_user_update_newstuff_folder(tCommonCompItem) );

	// Free
	SAFE_SM_FREE(taWfRefAttachments);
	SAFE_SM_FREE(iaAttachTypes);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_generateTempCCObjects */


std::string ccNameHelper_GetICSValue(vector<TC_ICS_VALUES> vecRequiredICSAttributes, CC_NAME_PATTERN_PARAMETER ccNameParam, logical lTransform)
{
	std::string sTmpICSValue = "";

	// Traverse Vector with ICS-Values
	for ( int iIdx2 = 0; iIdx2 < vecRequiredICSAttributes.size( ); iIdx2++ )
	{
		// ICS-Attribute found?
		if ( vecRequiredICSAttributes.at(iIdx2).sICSAttrID.compare(ccNameParam.sICS_AttrID) == 0 )
		{
			
			// Transform value
			if ( ( lTransform == true ) && ( ccNameParam.ccNamePatternAttrRule.sRuleID.compare("") != 0 ) )
			{
				// Rule exists for attribute ?
				if ( ccNameParam.ccNamePatternAttrRule.sRuleID.compare("") != 0 )
				{
					double dValue = atof(vecRequiredICSAttributes.at(iIdx2).sICSAttrValue.c_str( ) );

					// Normalize ICS-Value
					dValue = dValue * ccNameParam.dNormalizationFactor;

					// Traverse rules for ICS-Attribute
					for ( int iIdx3 = 0; iIdx3 < ccNameParam.ccNamePatternAttrRule.vecCcNamePatternRuleEntries.size( ); iIdx3++ )
					{
						CC_NAME_PATTERN_PARAMETER_ATTR_RULE_ENTRY tmpRuleEntry = ccNameParam.ccNamePatternAttrRule.vecCcNamePatternRuleEntries.at(iIdx3);

						if ( ( dValue >= tmpRuleEntry.dRuleMin ) && ( dValue < tmpRuleEntry.dRuleMax ) )
						{
							// Assign value for output
							stringstream ss_tmp;

							dValue = dValue * tmpRuleEntry.dRuleFactor;
							ss_tmp << tmpRuleEntry.sRulePrefix << dValue << tmpRuleEntry.sRuleSuffix;
							sTmpICSValue = ss_tmp.str( );
							
							// Exit traversal of rules
							break;
						}
					}

					// If no rule found
					if ( sTmpICSValue.compare("") == 0.0 )
					{
						// Assign value for output
						stringstream ss_tmp;
						ss_tmp << dValue;
						sTmpICSValue = ss_tmp.str( );
					}
				}
			}
			else
			{
				if ( ccNameParam.ccNamePatternAttrRule.sRuleID.compare("R230") == 0 )
				{
					double dValue = atof(vecRequiredICSAttributes.at(iIdx2).sICSAttrValue.c_str( ) );
					// Normalize ICS-Value
					dValue = dValue * ccNameParam.dCadimFactor;
					dValue = dValue + 0.5;
					__int64 iValue = (__int64)dValue;
					
					// Assign value for output
					stringstream ss_tmp;
					ss_tmp << iValue;

					sTmpICSValue = ss_tmp.str( );
				}
				else if ( ccNameParam.sSource.compare("ics") == 0 )
				{
					double dValue = atof(vecRequiredICSAttributes.at(iIdx2).sICSAttrValue.c_str( ) );
					dValue = dValue * ccNameParam.dCadimFactor;
					
					// Assign value for output
					stringstream ss_tmp;
					ss_tmp << std::setprecision(14) << fixed << dValue;
					sTmpICSValue = ss_tmp.str( );
				}
				else
				{
					// Assign value for output
					sTmpICSValue = vecRequiredICSAttributes.at(iIdx2).sICSAttrValue;
				}
					
			}

			// Exit traversal of ICS-Vector
			break;
		}
	}

	return sTmpICSValue;
}


int ccNameHelper_GetClassificationAttributes(tag_t tCommonComp, vector<TC_ICS_VALUES> *vecICSAttributes)
{
	int         iRetVal = ITK_ok;
	std::string strClassificationString = "";

	std::stringstream ssClassificationAttributes;
	std::stringstream ssClassificationValues;

	tag_t tClassificationObject = NULLTAG;

	// Get Classification Info
	ITK(ICS_ask_classification_object(tCommonComp, &tClassificationObject) );

	if ( tClassificationObject != NULLTAG )
	{
		int    iAttributeCount   = 0;
		char **saAttributeNames  = NULL;
		char **saAttributeValues = NULL;
		tag_t  tClassTag         = NULLTAG;

		// Get classification class id and class tag
		char *sClassID = NULL;
		char *sViewID  = NULL;
		ITK(ICS_ask_cid_sid_of_classification_obj(tClassificationObject, &sClassID, &sViewID) );
		ITK(ICS_ask_class_of_classification_obj(tClassificationObject, &tClassTag) );



		tag_t  view;
		int    nAttributes;                                 /**< (O) Number of attributes for the classification object */
		int *  unctNumbers;                                 /**< (OF) nAttributes UNCT numbers of the attributes */
		int *  unctFormats;                                 /**< (OF) nAttributes Formats of the attributes */
		char **attributeNames;                              /**< (OF) nAttributes Attribute names */
		char **attributeValues;                             /**< (OF) nAttributes Attribute values */
		char **units;
		ITK(ICS_describe_classification_object(tClassificationObject, &view, &nAttributes, &unctNumbers, &unctFormats, &attributeNames, &attributeValues, &units) );

		for ( int iIdx1 = 0; iIdx1 < nAttributes; iIdx1++ )
		{
			string strTmpAttributeValue = attributeValues[iIdx1];

			// Is KeyLOV?
			if ( unctFormats[iIdx1] < 0 )
			{
				char * sKeyLOVId = NULL;
				char * theKeyLOVName;
				int    theOptions;
				int    theCount;
				char **theKeys;
				char **theValues;
				char * theOwningSite;
				int    theSharedSiteCount;
				char **theSharedSites;

				// Get KeyLOV Description
				stringstream ssKeyLOVID;
				ssKeyLOVID << unctFormats[iIdx1];
				string strKeyLovID = ssKeyLOVID.str( );
				ITK(ICS_keylov_describe(strKeyLovID.c_str( ), &theKeyLOVName, &theOptions, &theCount, &theKeys, &theValues, &theOwningSite, &theSharedSiteCount, &theSharedSites) );

				// Get KeyLOV Value
				for ( int iIdx2 = 0; iIdx2 < theCount; iIdx2++ )
				{
					if ( strcmp(strTmpAttributeValue.c_str( ), theKeys[iIdx2]) == 0 )
					{
						//printf("Match found for LOV %s \n", strKeyLovID.c_str());
						strTmpAttributeValue.clear( );
						strTmpAttributeValue.append(theValues[iIdx2]);
						break;
					}
				}

				// Free
				ssKeyLOVID.clear( );
				strKeyLovID.clear( );

				if ( sKeyLOVId != NULL )
				{
					MEM_free(sKeyLOVId);
				}

				if ( theKeyLOVName != NULL )
				{
					MEM_free(theKeyLOVName);
				}

				if ( theKeys != NULL )
				{
					MEM_free(theKeys);
				}

				if ( theValues != NULL )
				{
					MEM_free(theValues);
				}

				if ( theOwningSite != NULL )
				{
					MEM_free(theOwningSite);
				}

				if ( theSharedSites != NULL )
				{
					MEM_free(theSharedSites);
				}
			}

			// Get string representation of the Attribute ID
			std::stringstream ssAttributeID;
			std::string       strAttributeID;
			ssAttributeID << unctNumbers[iIdx1];
			strAttributeID = ssAttributeID.str( );

			// Loop attribute vector
			logical lAttributeFoundInVector = false;

			for ( int iIdx2 = 0; iIdx2 < ( *vecICSAttributes ).size( ); iIdx2++ )
			{
				//printf(" > compare %s - %s \n", vecAttributeVector.at(iIdx2).sTcUAID.c_str(), strAttributeID.c_str());
				if ( ( *vecICSAttributes ).at(iIdx2).sICSAttrID.compare(strAttributeID) == 0 )
				{
					// Transform number strings
					//if((vecAttributeVector.at(iIdx2).sFormat.compare("number") == 0) || (vecAttributeVector.at(iIdx2).sFormat.compare("percent") == 0)){
					//	double dValue = atof(strTmpAttributeValue.c_str());
					//	double dFactor = vecAttributeVector.at(iIdx2).dFactor;
					//	dValue = dValue * dFactor;
					//
					//	stringstream ssTmpValue;
					//	ssTmpValue << std::fixed << std::setprecision(15) << dValue;

					//	strTmpAttributeValue.clear();
					//	strTmpAttributeValue.append(ssTmpValue.str());
					//	strTmpAttributeValue = trimTrailingZerosFromNumberString(strTmpAttributeValue);
					//}

					( *vecICSAttributes ).at(iIdx2).sICSAttrValue = strTmpAttributeValue;

					break;
				}
			}
			ssAttributeID.clear( );
			strAttributeID.clear( );
		}


		// Free
		SAFE_SM_FREE(saAttributeNames);
		SAFE_SM_FREE(saAttributeValues);
		SAFE_SM_FREE(sClassID);
		SAFE_SM_FREE(sViewID);
		SAFE_SM_FREE(unctNumbers);
		SAFE_SM_FREE(unctFormats);
		SAFE_SM_FREE(attributeNames);
		SAFE_SM_FREE(attributeValues);
		SAFE_SM_FREE(units);
	}

	return iRetVal;
}


string ccNameHelper_GetNodeValue(MSXML2::IXMLDOMNodePtr pNode, char *sAttrName)
{
	string strValue = "";
	MSXML2::IXMLDOMNamedNodeMapPtr nodeAttributes = pNode->Getattributes( );

	if ( ( nodeAttributes != NULL ) && ( nodeAttributes->Getlength( ) > 0 ) )
	{
		MSXML2::IXMLDOMNodePtr nodeAttribute = nodeAttributes->getNamedItem(sAttrName);

		if ( nodeAttribute != NULL )
		{
			_variant_t nodeValue = nodeAttribute->GetnodeValue( );
			_bstr_t    bs(nodeValue);
			strValue = static_cast<const char *>( bs );
		}
	}
	else
	{
		//printf(" > ERROR: No Attributes! \n");
	}

	return strValue;
}


CC_NAME_PATTERN_PARAMETER_ATTR_RULE ccNameHelper_GetAttrRule(string strAttr_PatternParam_ATTR_RULE)
{
	CC_NAME_PATTERN_PARAMETER_ATTR_RULE retVal;

	for ( int iIdx1 = 0; iIdx1 < vecCcNamePatternAttrRules.size( ); iIdx1++ )
	{
		if ( vecCcNamePatternAttrRules.at(iIdx1).sRuleID.compare(strAttr_PatternParam_ATTR_RULE) == 0 )
		{
			retVal = vecCcNamePatternAttrRules.at(iIdx1);
			break;
		}
	}

	return retVal;
}


int ccNameHelper_ReadConfigXML(char *sPathToConfig)
{
	int     iRetVal = ITK_ok;
	HRESULT hr      = CoInitialize(NULL);

	if ( SUCCEEDED(hr) )
	{
		MSXML2::IXMLDOMDocument2Ptr xmlDoc;

		//hr = xmlDoc.CreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
		xmlDoc.CreateInstance(__uuidof(DOMDocument30) );

		if ( xmlDoc->load(_T(sPathToConfig) ) != VARIANT_TRUE )
		{
			printf(" > Unable to load '%s' \n", sPathToConfig);
		}
		else
		{
			printf(" > XML successfully loaded\n");

			// Get all instances of "cc_naming_rules"
			MSXML2::IXMLDOMNodeListPtr nodeNamingRulesRoot = xmlDoc->getElementsByTagName("cc_naming_rules");

			// Naming Rule Root-Tag identified unambiguously?
			if ( nodeNamingRulesRoot->Getlength( ) == 1 )
			{
				MSXML2::IXMLDOMNodeListPtr nodeNamingRules = nodeNamingRulesRoot->Getitem(0)->GetchildNodes( );

				// Loop Naming Rules
				for ( int iIdx1 = 0; iIdx1 < nodeNamingRules->Getlength( ); iIdx1++ )
				{
					CC_NAME_PATTERN_PARAMETER_ATTR_RULE tmpAttrRule;

					MSXML2::IXMLDOMNodePtr nodeNamingRule = nodeNamingRules->Getitem(iIdx1);

					// Get Attributes
					std::string strAttr_Rule_ID   = ccNameHelper_GetNodeValue(nodeNamingRule, "id");
					std::string strAttr_Rule_DECS = ccNameHelper_GetNodeValue(nodeNamingRule, "desc");

					// Get Rules
					vector<CC_NAME_PATTERN_PARAMETER_ATTR_RULE_ENTRY> vecCcNamePatternRuleEntries;
					MSXML2::IXMLDOMNodeListPtr nodeNamingRuleEntries = nodeNamingRule->GetchildNodes( );

					for ( int iIdx2 = 0; iIdx2 < nodeNamingRuleEntries->Getlength( ); iIdx2++ )
					{
						CC_NAME_PATTERN_PARAMETER_ATTR_RULE_ENTRY tmpAttrRuleEntry;

						MSXML2::IXMLDOMNodePtr nodeNamingRuleEntry = nodeNamingRuleEntries->Getitem(iIdx2);

						// Get Attributes
						std::string strAttr_RuleEntry_ID     = ccNameHelper_GetNodeValue(nodeNamingRuleEntry, "id");
						std::string strAttr_RuleEntry_MIN    = ccNameHelper_GetNodeValue(nodeNamingRuleEntry, "min");
						std::string strAttr_RuleEntry_MAX    = ccNameHelper_GetNodeValue(nodeNamingRuleEntry, "max");
						std::string strAttr_RuleEntry_FACTOR = ccNameHelper_GetNodeValue(nodeNamingRuleEntry, "factor");
						std::string strAttr_RuleEntry_PREFIX = ccNameHelper_GetNodeValue(nodeNamingRuleEntry, "prefix");
						std::string strAttr_RuleEntry_SUFFIX = ccNameHelper_GetNodeValue(nodeNamingRuleEntry, "suffix");

						// Assign values
						tmpAttrRuleEntry.sRuleId     = strAttr_RuleEntry_ID;
						tmpAttrRuleEntry.dRuleMin    = atof(strAttr_RuleEntry_MIN.c_str( ) );
						tmpAttrRuleEntry.dRuleMax    = atof(strAttr_RuleEntry_MAX.c_str( ) );
						tmpAttrRuleEntry.dRuleFactor = atof(strAttr_RuleEntry_FACTOR.c_str( ) );
						tmpAttrRuleEntry.sRulePrefix = strAttr_RuleEntry_PREFIX;
						tmpAttrRuleEntry.sRuleSuffix = strAttr_RuleEntry_SUFFIX;

						// Add new RuleEntry to vector
						vecCcNamePatternRuleEntries.push_back(tmpAttrRuleEntry);
					}

					// Assign values
					tmpAttrRule.sRuleID   = strAttr_Rule_ID;
					tmpAttrRule.sRuleDesc = strAttr_Rule_DECS;
					tmpAttrRule.vecCcNamePatternRuleEntries = vecCcNamePatternRuleEntries;

					// Add new Rule to vector
					vecCcNamePatternAttrRules.push_back(tmpAttrRule);
				}
			}
			else
			{
				printf(" > ERROR: xml-tag 'cc_naming_rules' not identified unambiguously! \n");
			}

			////////////////////

			// Get all instances of "cc_naming_patterns"
			MSXML2::IXMLDOMNodeListPtr nodeNamingPatternsRoot = xmlDoc->getElementsByTagName("cc_naming_patterns");

			// Naming Rule Root-Tag identified unambiguously?
			if ( nodeNamingPatternsRoot->Getlength( ) == 1 )
			{
				MSXML2::IXMLDOMNodeListPtr nodeNamingPatterns = nodeNamingPatternsRoot->Getitem(0)->GetchildNodes( );

				// Loop Naming Patterns
				for ( int iIdx1 = 0; iIdx1 < nodeNamingPatterns->Getlength( ); iIdx1++ )
				{
					CC_NAME_PATTERN tmpPattern;

					MSXML2::IXMLDOMNodePtr nodeNamingPattern = nodeNamingPatterns->Getitem(iIdx1);

					// Get Attributes
					std::string strAttr_Pattern_UAID    = ccNameHelper_GetNodeValue(nodeNamingPattern, "ua_id");
					std::string strAttr_Pattern_CADIMID = ccNameHelper_GetNodeValue(nodeNamingPattern, "cadim_id");
					std::string strAttr_Pattern_DECS    = ccNameHelper_GetNodeValue(nodeNamingPattern, "cadim_id");

					// If current ID could not be read, continue to next index
					if ( strAttr_Pattern_UAID.compare("") == 0 )
					{
						continue;
					}
					else
					{
						//printf(" > Current ID: %s \n", strAttr_Pattern_UAID.c_str());
					}

					// Get Pattern Parameters
					vector<CC_NAME_PATTERN_PARAMETER> vecCcNamePatternParameters;
					MSXML2::IXMLDOMNodeListPtr        nodeNamingPatternParameters = nodeNamingPattern->GetchildNodes( );

					for ( int iIdx2 = 0; iIdx2 < nodeNamingPatternParameters->Getlength( ); iIdx2++ )
					{
						CC_NAME_PATTERN_PARAMETER tmpNamingPatternParameter;

						MSXML2::IXMLDOMNodePtr nodeNamingPatternParameter = nodeNamingPatternParameters->Getitem(iIdx2);

						// Get Attributes
						std::string strAttr_PatternParam_ID                   = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "id");
						std::string strAttr_PatternParam_SOURCE               = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "source");
						std::string strAttr_PatternParam_VAL_EN               = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "value_en");
						std::string strAttr_PatternParam_VAL_GER              = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "value_ger");
						std::string strAttr_PatternParam_DESC                 = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "desc");
						std::string strAttr_PatternParam_ATTR_ID              = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "attr_id");
						std::string strAttr_PatternParam_ATTR_RULE            = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "attr_rule");
						std::string strAttr_PatternParam_NORMALIZATION_FACTOR = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "norm_factor");
						std::string strAttr_PatternParam_CADIM_FACTOR         = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "cadim_factor");
						std::string strAttr_PatternParam_USED_FOR_NAME_GEN    = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "used_for_name_gen");
						std::string strAttr_PatternParam_CADIM_ATTR_NAME      = ccNameHelper_GetNodeValue(nodeNamingPatternParameter, "cadim_attr_name");

						// LOV required ?
						vector<CC_NAME_PATTERN_PARAMETER_LOV> vecCcNamePatternParameterLOVs;

						if ( strAttr_PatternParam_SOURCE.compare("ics_lov") == 0 )
						{
							MSXML2::IXMLDOMNodeListPtr nodeNamingPatternParameterLOVs = nodeNamingPatternParameter->GetchildNodes( );

							for ( int iIdx3 = 0; iIdx3 < nodeNamingPatternParameterLOVs->Getlength( ); iIdx3++ )
							{
								CC_NAME_PATTERN_PARAMETER_LOV tmpNamingPatternParameterLOV;
								MSXML2::IXMLDOMNodePtr        nodeNamingPatternParameterLOV = nodeNamingPatternParameterLOVs->Getitem(iIdx3);

								// Get Attributes
								std::string strAttr_PatternParam_LOV_VALUE = ccNameHelper_GetNodeValue(nodeNamingPatternParameterLOV, "value");
								std::string strAttr_PatternParam_LOV_SHORT = ccNameHelper_GetNodeValue(nodeNamingPatternParameterLOV, "short");

								// Assign values
								tmpNamingPatternParameterLOV.sLovValue      = strAttr_PatternParam_LOV_VALUE;
								tmpNamingPatternParameterLOV.sLovShortValue = strAttr_PatternParam_LOV_SHORT;

								// Add new Rule to vector
								vecCcNamePatternParameterLOVs.push_back(tmpNamingPatternParameterLOV);
							}
						}

						// Assign values
						tmpNamingPatternParameter.sParamID              = strAttr_PatternParam_ID;
						tmpNamingPatternParameter.sSource               = strAttr_PatternParam_SOURCE;
						tmpNamingPatternParameter.sValueEN              = strAttr_PatternParam_VAL_EN;
						tmpNamingPatternParameter.sValueGER             = strAttr_PatternParam_VAL_GER;
						tmpNamingPatternParameter.sICS_AttrID           = strAttr_PatternParam_ATTR_ID;
						tmpNamingPatternParameter.sDesc                 = strAttr_PatternParam_DESC;
						tmpNamingPatternParameter.ccNamePatternAttrRule = ccNameHelper_GetAttrRule(strAttr_PatternParam_ATTR_RULE);
						tmpNamingPatternParameter.vecCcNamePatternParameterLovEntries = vecCcNamePatternParameterLOVs;
						tmpNamingPatternParameter.dNormalizationFactor = atof(strAttr_PatternParam_NORMALIZATION_FACTOR.c_str( ) );
						tmpNamingPatternParameter.dCadimFactor         = atof(strAttr_PatternParam_CADIM_FACTOR.c_str( ) );
						tmpNamingPatternParameter.sUsedForNameGen      = strAttr_PatternParam_USED_FOR_NAME_GEN;
						tmpNamingPatternParameter.sCadimAttrName       = strAttr_PatternParam_CADIM_ATTR_NAME;

						// Add Paramenters to vector
						vecCcNamePatternParameters.push_back(tmpNamingPatternParameter);
					}

					// Assign values
					tmpPattern.sTcUAID  = strAttr_Pattern_UAID;
					tmpPattern.sCadimID = strAttr_Pattern_CADIMID;
					tmpPattern.sDesc    = strAttr_Pattern_DECS;
					tmpPattern.vecCcNamePatternParameters = vecCcNamePatternParameters;

					// Add Naming Pattern to vector
					vecCcNamePatterns.push_back(tmpPattern);
				}
			}
			else
			{
				printf(" > ERROR: xml-tag 'cc_naming_patterns' not identified unambiguously! \n");
			}
		}
	}

	return iRetVal;
}


std::string ccNameHelper_TrimTrailingZerosFromNumberString(std::string sString)
{
	if ( sString.length( ) > 0 )
	{
		if ( sString.substr(sString.length( ) - 1, sString.length( ) ).compare("0") == 0 )
		{
			sString = sString.substr(0, sString.length( ) - 1);
			sString = ccNameHelper_TrimTrailingZerosFromNumberString(sString);
		}
		else if ( sString.substr(sString.length( ) - 1, sString.length( ) ).compare(".") == 0 )
		{
			sString = sString.substr(0, sString.length( ) - 1);
		}
	}

	return sString;
}


std::string ccNameHelper_TrimLeadingZerosFromNumberString(std::string sString)
{
	if ( sString.length( ) > 0 )
	{
		if ( sString.substr(0, 1).compare("0") == 0 )
		{
			if ( !( ( sString.length( ) > 1 ) && ( sString.substr(1, 1).compare(".") == 0 ) ) )
			{
				sString = sString.substr(1, sString.length( ) );
				sString = ccNameHelper_TrimLeadingZerosFromNumberString(sString);
			}
		}
	}

	return sString;
}


int ccNameHelper_GetClassificationID(tag_t tTcComponent, char **sChildClassId)
{
	int   iRetVal = ITK_ok;
	tag_t tClassificationObject = NULLTAG;

	/* Get classification object */
	ITK(ICS_ask_classification_object(tTcComponent, &tClassificationObject) );

	if ( tClassificationObject != NULLTAG )
	{
		char *sChildClassName = NULL;
		tag_t tClass          = NULLTAG;

		/* Get classification ID */
		ITK(ICS_ask_class_of_classification_obj(tClassificationObject, &tClass) );
		ITK(ICS_ask_id_name(tClass, sChildClassId, &sChildClassName) );

		//printf(" > sChildClassId = %s ... sChildClassName = %s \n", sChildClassId, sChildClassName);

		/* Free */
		SAFE_SM_FREE(sChildClassName);
	}
	else
	{
		*sChildClassId = "***cc_not_classified***";
	}

	return iRetVal;
}

std::string ccNameHelper_GetVendorPartNumberFromVendorPart(tag_t tVendorPart, tag_t tManufacturer)
{
	int  iStatus = ITK_ok;
	char sVendorID[ITEM_id_size_c + 1];
	char sManufacturerID[ITEM_id_size_c + 1];

	ITK(ITEM_ask_id(tVendorPart, sVendorID) );
	ITK(ITEM_ask_id(tManufacturer, sManufacturerID) );

	std::string strVendorPartNumber = sVendorID;
	std::string strManufacturerID   = sManufacturerID;

	// Manufacturer ID found?
	if ( strManufacturerID.compare("") != 0 )
	{
		std::size_t found = strVendorPartNumber.find(strManufacturerID);

		if ( found != std::string::npos )
		{
			// Vendor Part ItemID is build by the ootb Tc Vendor Management as follows:
			// <VendorPartNr>-<ManufacturerID>
			// Means: We need to cut the "-" and the <ManufacturerID> from the VendorPart ItemID to get the Vendor Part Number
			strVendorPartNumber = strVendorPartNumber.substr(0, found - 1);
		}
	}

	return strVendorPartNumber;
}


int ccNameHelper_GetDatasetNamedReferences(tag_t dataset, char reference_name[AE_reference_size_c + 1], tag_t *reference_object)
{
	int
		ii,
		imf_count                = 0,
		reference_count          = 0;
	tag_t               prop_tag = NULLTAG;
	AE_reference_type_t reference_type;

/* Make sure no one messes with it while we're reading it */
	ITK(AOM_refresh(dataset, false) );

	ITK(AE_ask_dataset_ref_count(dataset, &reference_count) );

	if ( reference_count > 0 )
	{
		for ( ii = 0; ii < reference_count; ii++ )
		{
			tag_t prop_tag   = NULLTAG;
			char *prop_value = NULL;

			ITK(AE_find_dataset_named_ref(dataset, 0, reference_name, &reference_type, reference_object) );

			ITK(PROP_ask_property_by_name(*reference_object, "object_type", &prop_tag) );
			ITK(PROP_ask_value_string(prop_tag, &prop_value) );

			//ITK(PROP_UIF_ask_property_by_name(*reference_object, "Type", &prop_tag) );
			//ITK(PROP_UIF_ask_value(prop_tag, &prop_value) );

			if ( strcmp(prop_value, "ImanFile") == 0 )
			{
				//  strcpy((*reference_names)[imf_count], reference_name);
				//  (*reference_objects)[imf_count] = reference_object;
				imf_count++;
				break;
			}

			SAFE_SM_FREE(prop_value);
		}
	}

	//ITK(AOM_refresh(dataset, false));

	return imf_count;
}


int ccNameHelper_UpdateCCRForm(tag_t tRootCCRForm, tag_t tCurrentObject, tag_t tCurrentObject_Revision, int iModus, char *sAttr_CC_PARTNUMBER, char *sAttr_CC_LGUSRBEMERKUNG, CC_NAME_PATTERN requiredPattern, vector<TC_ICS_VALUES> vecRequiredICSAttributes, char *sParamCadimStatus)
{
	int    iRetVal = ITK_ok;
	int    iCountRelatedCadimDbEntries           = 0;
	tag_t *taRelSecondaryImanExternalObjectLinks = NULL;
	tag_t  tCurrentCadimDbEntry       = NULLTAG;
	tag_t  tRelImanExternalObjectLink = NULLTAG;
	char   sCurrentObjectID[ITEM_id_size_c + 1];

	char *  sEmailValue = NULL;
	char *  sAttr_CLASS = "";
	char *  sAttr_CC_RESPONSABLE_COMP_ENG = NULL;
	char *	genericType	= NULL;
	string  strAttr_CC_RELATIVE_PATH    = "";
	string  strAttr_SUPPLIERPARTNUMBER  = "";
	string  strAttr_L0MANUFNORIS        = "";
	string  strAttr_L5_A_BAUF           = "";
	string  strAttr_L0PARTTYPEGEN       = "";
	string  strAttr_CC_KEY   = "";
	logical lInitialCreation = false;

	int    iCadimDbEntryPropCount  = 0;
	char **saCadimDbEntryPropNames = NULL;

	// Read ID of Current Object
	if ( ( iModus == 1 ) || ( iModus == 2 ) )
	{
		// Common Component or Vendor Part
		ITK(ITEM_ask_id(tCurrentObject, sCurrentObjectID) );
	}
	else if ( iModus == 3 )
	{
		// Document
		strcpy(sCurrentObjectID, "CADIM_Document");
	}
	else if ( iModus == 4 )
	{
		// File
		char  reference_name[AE_reference_size_c + 1];
		tag_t reference_object = NULLTAG;
		int   iReferenceCount  = ccNameHelper_GetDatasetNamedReferences(tCurrentObject, reference_name, &reference_object);

		if ( iReferenceCount > 0 )
		{
			char   sTmpFileName[IMF_filename_size_c + 1];
			string strTmpFileName;

			ITK(IMF_ask_original_file_name(reference_object, sTmpFileName) );
			strTmpFileName = sTmpFileName;

			// Define Relative Path
			char *sTmpRelativeFileName = NULL;
			ITK(AOM_ask_value_string(reference_object, "file_name", &sTmpRelativeFileName) );

			strAttr_CC_RELATIVE_PATH.append("PROD/");
			strAttr_CC_RELATIVE_PATH.append(string(sTmpRelativeFileName) );

			SAFE_SM_FREE(sTmpRelativeFileName);

			// Trim Filename (if required)
			if ( strTmpFileName.length( ) > 128 )
			{
				// Trim leading chars to keep file-extension
				strTmpFileName.substr(strTmpFileName.length( ) - 128, 128);
			}

			strcpy(sCurrentObjectID, strTmpFileName.c_str( ) );
		}
		else
		{
			printf(" ERROR : No Named References! \n");
			return -1;
		}
	}

	// Read related CADIM-DB-Entries
	ITK(GRM_find_relation_type("IMAN_external_object_link", &tRelImanExternalObjectLink) );
	ITK(GRM_list_secondary_objects_only(tRootCCRForm, tRelImanExternalObjectLink, &iCountRelatedCadimDbEntries, &taRelSecondaryImanExternalObjectLinks) );

	// Read Responsable Person
	ITK(AOM_ask_value_string(tRootCCRForm, "bt7_RespCompEng", &sAttr_CC_RESPONSABLE_COMP_ENG) );

	// Find correct CADIM DB Entry
	for ( int iIdx1 = 0; iIdx1 < iCountRelatedCadimDbEntries; iIdx1++ )
	{
		char sCCRFormName[WSO_name_size_c + 1];
		char sObjectType[WSO_name_size_c + 1];

		ITK(WSOM_ask_name(taRelSecondaryImanExternalObjectLinks[iIdx1], sCCRFormName) );
		ITK(WSOM_ask_object_type(taRelSecondaryImanExternalObjectLinks[iIdx1], sObjectType) );

		if ( ( strcmp(sObjectType, "BT7_CCR_CADIM_Entry") == 0 ) && ( strcmp(sCCRFormName, sCurrentObjectID) == 0 ) )
		{
			tCurrentCadimDbEntry = taRelSecondaryImanExternalObjectLinks[iIdx1];
			break;
		}
	}

	// If CADIM DB Entry does not exist yet, create a new one, relate it to the root-form and release it (as this handler is executed in "Bypass-Mode", there should be not issue with the access rights
	if ( tCurrentCadimDbEntry == NULLTAG )
	{
		tag_t tNewRelation = NULLTAG;

		lInitialCreation = true;

		// Create new CCR_CADIM_Entry
		ITK(FORM_create(sCurrentObjectID, sCurrentObjectID, "BT7_CCR_CADIM_Entry", &tCurrentCadimDbEntry) );
		ITK(AOM_save(tCurrentCadimDbEntry) );

		// Attach CCR_CADIM_Entry to Root-CCR-Form
		ITK(GRM_create_relation(tRootCCRForm, tCurrentCadimDbEntry, tRelImanExternalObjectLink, NULLTAG, &tNewRelation) );
		ITK(AOM_save(tNewRelation) );

		// Release the CCR_CADIM_Entry
		tag_t tStatus;
		tag_t taReleaseObjects[1];
		taReleaseObjects[0] = tCurrentCadimDbEntry;

		ITK(CR_create_release_status("BT7_Implemented", &tStatus) );
		ITK(AOM_save(tStatus) );
		ITK(EPM_add_release_status(tStatus, 1, taReleaseObjects, false) );
	}

	// Read property names of the BT7_CCR_CADIM_Entry-Form
	ITK(AOM_ask_prop_names(tCurrentCadimDbEntry, &iCadimDbEntryPropCount, &saCadimDbEntryPropNames) );

	// Modus differentiation
	if ( iModus == 1 )
	{
		// Root Component
		sAttr_CLASS    = "l5CadPrt";
		strAttr_CC_KEY = "KAT";

		logical lSMD = false;
		ITK(AOM_ask_value_logical(tCurrentObject_Revision, "bt7_smd", &lSMD) );

		if ( lSMD == true )
		{
			strAttr_L5_A_BAUF = "SMD";
		}
		else
		{
			strAttr_L5_A_BAUF = "THD";
		}
		
		ITK(AOM_ask_value_string(tCurrentObject_Revision, "bt7_GenericType", &genericType) );
		strAttr_L0PARTTYPEGEN = genericType;
	}
	else if ( iModus == 2 )
	{
		// Read Vendors
		int    iCountRelVendorPartRel    = 0;
		tag_t  tRelVendorPartRel         = NULLTAG;
		tag_t *taRelVendorPartRelObjects = NULL;

		// Init Attributes
		sAttr_CLASS    = "l5CadMnf";
		strAttr_CC_KEY = "HER";

		// Find Vendors
		ITK(GRM_find_relation_type("TC_vendor_part_rel", &tRelVendorPartRel) );
		ITK(GRM_list_secondary_objects_only(tCurrentObject, tRelVendorPartRel, &iCountRelVendorPartRel, &taRelVendorPartRelObjects) );

		if ( iCountRelVendorPartRel == 1 )
		{
			string sVendorPartNr = ccNameHelper_GetVendorPartNumberFromVendorPart(tCurrentObject, taRelVendorPartRelObjects[0]);
			strAttr_SUPPLIERPARTNUMBER = sVendorPartNr.c_str( );

			char sVendorId[ITEM_id_size_c + 1];
			ITK(ITEM_ask_id(taRelVendorPartRelObjects[0], sVendorId) );
			strAttr_L0MANUFNORIS = sVendorId;
			strAttr_L0MANUFNORIS = strAttr_L0MANUFNORIS.substr(0, 3);
		}

		// Free
		SAFE_SM_FREE(taRelVendorPartRelObjects);
	}
	else if ( iModus == 3 )
	{
		// Document (Supplier Spec)

		sAttr_CLASS    = "l5CadDoc";
		strAttr_CC_KEY = "DOK";
	}
	else if ( iModus == 4 )
	{
		// File (Datasheet)

		sAttr_CLASS    = "l5CadFil";
		strAttr_CC_KEY = "FIL";
	}
	else
	{
		printf(" > ERROR: Invalid Modus %i \n", iModus);
		return -1;
	}


	// Set properties of the form
	{
		ITK(AOM_refresh(tCurrentCadimDbEntry, true));
		// Set property 'bt7_CDM_CLASS'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_CLASS", sAttr_CLASS) );

		// Set property 'bt7_CDM_CREATIONDATE'
		if ( lInitialCreation == true )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_CREATIONDATE", "" /*ccNameHelper_GetCurrentDateTime()*/) );
		}

		// Set property 'bt7_CDM_CURDBNAME'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_CURDBNAME", "tcua") );

		// Set property 'bt7_CDM_DOCUMENTNAME'
		if ( ( strcmp(sAttr_CLASS, "l5CadDoc") == 0 ) || ( strcmp(sAttr_CLASS, "l5CadFil") == 0 ) )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_DOCUMENTNAME", sAttr_CC_PARTNUMBER) );
		}

		// Set property 'bt7_CDM_LGUSRBEMERKUNG'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_LGUSRBEMERKUNG", sAttr_CC_LGUSRBEMERKUNG) );

		// Set property 'bt7_CDM_LASTUPDATE'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_LASTUPDATE", "" /*ccNameHelper_GetCurrentDateTime()*/) );

		// Set property 'bt7_CDM_OBID'

		/*char *sUIDValue = NULL;
		 * ITK(AOM_ask_value_string(tCurrentObject, "UID", &sUIDValue));
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_OBID", sUIDValue));
		 * SAFE_SM_FREE(sUIDValue);*/
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_OBID", getInternalIdOfTcObject(tCurrentObject) ) );

		// Set property 'bt7_CDM_PARTNUMBER'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_PARTNUMBER", sAttr_CC_PARTNUMBER) );

		// Set property 'bt7_CDM_RELATIVEPATH'
		if ( strcmp(sAttr_CLASS, "l5CadFil") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_RELATIVEPATH", strAttr_CC_RELATIVE_PATH.c_str( ) ) );
		}

		// Set property 'bt7_CDM_RESPONSABLEPERSON'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			char respPerson[30];
			strncpy(respPerson, sAttr_CC_RESPONSABLE_COMP_ENG, 29);
			respPerson[29] = '\0';
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_RESPONSABLEPERSON", respPerson) );
		}

		// Set property 'bt7_CDM_SUPPLIERPARTNUMBER'
		if ( strcmp(sAttr_CLASS, "l5CadMnf") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_SUPPLIERPARTNUMBER", strAttr_SUPPLIERPARTNUMBER.c_str( ) ) );
		}

		// Set property 'bt7_CDM_WORKINGRELATIVEPATH'
		if ( strcmp(sAttr_CLASS, "l5CadFil") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_WORKINGRELATIVEPATH", sCurrentObjectID) );
		}

		// Set property 'bt7_CDM_ZBLOB'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_ZBLOB", "-") );

		// Set property 'bt7_CDM_A0TOPMLIST'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_A0TOPMLIST", "") );

		// Set property 'bt7_CDM_L0INSTANCENAME'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L0INSTANCENAME", "PROD") );

		// Set property 'bt7_CDM_L0MANUFNORIS'
		if ( strcmp(sAttr_CLASS, "l5CadMnf") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L0MANUFNORIS", strAttr_L0MANUFNORIS.c_str( ) ) );
		}

		// Set property 'bt7_CDM_L0PARTTYPEGEN'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L0PARTTYPEGEN", "") );

		// Set property 'bt7_CDM_L4XFERFORLCMANDRULE'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L4XFERFORLCMANDRULE", "-") );

		// Set property 'bt7_CDM_L5_A_AS'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_A_AS", "01") );

		// Set property 'bt7_CDM_L5_A_BAUF'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_A_BAUF", strAttr_L5_A_BAUF.c_str( ) ) );
		
		// Set property 'bt7_CDM_L0PARTTYPEGEN'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L0PARTTYPEGEN", strAttr_L0PARTTYPEGEN.c_str( ) ) );

		// Set property 'bt7_CDM_L5_A_BEN_ENG'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_A_BEN_ENG", "") );

		// Set property 'bt7_CDM_L5_A_BEN_GER'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_A_BEN_GER", "") );

		// Set property 'bt7_CDM_L5_A_CLASS'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_A_CLASS", requiredPattern.sCadimID.c_str( ) ) );
		}

		// Set property 'bt7_CDM_L5_A_ORG_EINH'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_A_ORG_EINH", "410000") );
		}

		// Set property 'bt7_CDM_L5_CADIM_ID'
		ITK(AOM_set_value_int(tCurrentCadimDbEntry, "bt7_CDM_L5_CADIM_ID", 0) );

		// Set property 'bt7_CDM_L5_D_BEN_GER'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_D_BEN_GER", "") );

		// Set property 'bt7_CDM_L5_D_DART'
		if ( strcmp(sAttr_CLASS, "l5CadDoc") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_D_DART", "K5") );
		}

		// Set property 'bt7_CDM_L5_D_DS'
		if ( strcmp(sAttr_CLASS, "l5CadDoc") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_D_DS", "01") );
		}

		// Set property 'bt7_CDM_L5_D_FK'
		if ( strcmp(sAttr_CLASS, "l5CadDoc") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_D_FK", "110") );
		}

		// Set property 'bt7_CDM_L5_FK_GLOBAL'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_FK_GLOBAL", "130") );
		}

		// Set property 'bt7_CDM_L5_FK_WERK'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_FK_WERK", "130") );
		}

		// Set property 'bt7_CDM_L5_KEY'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_KEY", strAttr_CC_KEY.c_str( ) ) );

		// Set property 'bt7_CDM_L5_ORG_WERK'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_ORG_WERK", "600000") );
		}

		// Set property 'bt7_CDM_L5_Q_STATUS'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_Q_STATUS", "4") );
		}

		// Set property 'bt7_CDM_L5_TRANS_ERR'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_TRANS_ERR", "") );

		// Set property 'bt7_CDM_L5_TRANS_ID'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_TRANS_ID", "") );

		// Set property 'bt7_CDM_L5_TRANS_STAT' --> Only if sParamCadimStatus is not empty
		if ( strcmp(sParamCadimStatus, "") != 0 )
		{
			if ( strcmp(sAttr_CLASS, "l5CadFil") != 0 )
			{
				// No File
				ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_TRANS_STAT", sParamCadimStatus) );
			}
			else
			{
				// File

				// Lock the status of the entry, will be unlocked by the Dispatcher Service once the file is exporrted
				ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_TRANS_STAT", "LOCKED") );

				// Trigger Dispatcher Request
				tag_t request = NULLTAG;

				const char **saDispatcherArguments = (const char **) MEM_alloc(2 * sizeof( char * ) );

				char *sFileUID    = getInternalIdOfTcObject(tCurrentObject);
				char *sDbEntryUID = getInternalIdOfTcObject(tCurrentCadimDbEntry);

				string strFileUID_Arg = "FileUID=";
				strFileUID_Arg.append(sFileUID);
				string strCadimEntryUID_Arg = "CadimEntryUID=";
				strCadimEntryUID_Arg.append(sDbEntryUID);

				saDispatcherArguments[0] = (char *) MEM_alloc( ( strlen(strFileUID_Arg.c_str( ) ) + 1 ) * sizeof( char ) );
				saDispatcherArguments[1] = (char *) MEM_alloc( ( strlen(strCadimEntryUID_Arg.c_str( ) ) + 1 ) * sizeof( char ) );

				saDispatcherArguments[0] = strFileUID_Arg.c_str( );
				saDispatcherArguments[1] = strCadimEntryUID_Arg.c_str( );

				ITK(ETS_create_request2(1, &tCurrentObject, &tCurrentCadimDbEntry, 2, "Accenture", "cadim_file_export", "ONDEMAND", 2, saDispatcherArguments, &request) );
			}
		}

		// Set property 'bt7_CDM_L5_VZT'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_VZT", "") );

		// Set property 'bt7_CDM_L9ORIGCLASS'

		if(strcmp(sAttr_CLASS, "l5CadFil") != 0)
		{
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L9ORIGCLASS", "l0SbtCmp") );
		} else {
			ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L9ORIGCLASS", "") );
		}

		// Set property 'bt7_CDM_L9ORIGDB'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L9ORIGDB", "") );

		// Set property 'bt7_CDM_L9ORIGOBID'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L9ORIGOBID", "") );

		// Set property 'bt7_CDM_L5_S_1' -> 'bt7_CDM_L5_S_20'
		if ( strcmp(sAttr_CLASS, "l5CadPrt") == 0 )
		{
			for ( int iIdx1 = 0; iIdx1 < requiredPattern.vecCcNamePatternParameters.size( ); iIdx1++ )
			{
				CC_NAME_PATTERN_PARAMETER ccNameParam = requiredPattern.vecCcNamePatternParameters.at(iIdx1);

				// CADIM Mapping exists?
				if ( ( ccNameParam.sCadimAttrName.compare("") != 0 ) && ( ( ccNameParam.sSource.compare("ics") == 0 ) || ( ccNameParam.sSource.compare("ics_lov") == 0 ) || ( ccNameParam.sSource.compare("ics_string") == 0 ) || ( ccNameParam.sSource.compare("ics_string_only_exported") == 0 ) || ( ccNameParam.sSource.compare("macro") == 0 ) || ( ccNameParam.sSource.compare("ics_only_exported") == 0 ) ) )
				{
					logical lCadimAttrExistsInTcUA = false;
					string  sTmpICSValue           = "";

					// Read value
					if ( ( ccNameParam.sSource.compare("ics") == 0 ) || ( ccNameParam.sSource.compare("ics_string") == 0 ) || ( ccNameParam.sSource.compare("ics_string_only_exported") == 0 ) || ( ccNameParam.sSource.compare("ics_only_exported") == 0 ) )
					{
						// ICS
						sTmpICSValue = ccNameHelper_GetICSValue(vecRequiredICSAttributes, ccNameParam, true);
						if(sTmpICSValue.substr(sTmpICSValue.length() - 1, sTmpICSValue.length()).compare("%") != 0)
						{
							sTmpICSValue = ccNameHelper_GetICSValue(vecRequiredICSAttributes, ccNameParam, false);
							if( ( ccNameParam.sSource.compare("ics") == 0 ) || ( ccNameParam.sSource.compare("ics_only_exported") == 0 ) )
							{
								sTmpICSValue = ccNameHelper_TrimLeadingZerosFromNumberString(sTmpICSValue);
								sTmpICSValue = ccNameHelper_TrimTrailingZerosFromNumberString(sTmpICSValue);
							}
						}
					}
					else if ( ccNameParam.sSource.compare("macro") == 0 )
					{
					}
					else
					{
						// ICS-LOV
						sTmpICSValue = ccNameHelper_GetICSValue(vecRequiredICSAttributes, ccNameParam, false);
					}

					// Property exists ?
					for ( int iIdx2 = 0; iIdx2 < iCadimDbEntryPropCount; iIdx2++ )
					{
						if ( ccNameParam.sCadimAttrName.compare(saCadimDbEntryPropNames[iIdx2]) )
						{
							lCadimAttrExistsInTcUA = true;
							break;
						}
					}

					// Write value to the CADIM DB-Entry (if prop exists)
					if ( lCadimAttrExistsInTcUA == true )
					{
						ITK(AOM_set_value_string(tCurrentCadimDbEntry, ccNameParam.sCadimAttrName.c_str( ), sTmpICSValue.c_str( ) ) );
					}
					else
					{
						printf(" > ERROR: CADIM-Property '%' does not exists on the BT7_CCR_CADIM_Entry-Form ! \n", ccNameParam.sCadimAttrName.c_str( ) );
					}
				}
			}
		}

		/*
		 * // Set property 'bt7_CDM_L5_S_1'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_1", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_2'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_2", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_3'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_3", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_4'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_4", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_5'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_5", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_6'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_6", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_7'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_7", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_8'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_8", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_9'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_9", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_10'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_10", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_11'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_11", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_12'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_12", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_13'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_13", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_14'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_14", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_15'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_15", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_16'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_16", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_17'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_17", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_18'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_18", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_19'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_19", ""));
		 *
		 * // Set property 'bt7_CDM_L5_S_20'
		 * ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_L5_S_20", ""));
		 */

		// Set property 'bt7_CDM_SEINDEXLANGUAGELIST'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_SEINDEXLANGUAGELIST", "") );

		// Set property 'bt7_CDM_WBSID'
		char sRootCCRObjectName[WSO_name_size_c + 1];
		ITK(WSOM_ask_name(tRootCCRForm, sRootCCRObjectName) );
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_WBSID", sRootCCRObjectName) );

		// Set property 'bt7_CDM_RECYCLED'
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_RECYCLED", "") );

		// Set property 'bt7_CDM_EMAILADDR'
		ITK(AOM_ask_value_string(tRootCCRForm, "bt7_Email", &sEmailValue) );
		ITK(AOM_set_value_string(tCurrentCadimDbEntry, "bt7_CDM_EMAILADDR", sEmailValue) );

		// Save CADIM-Entry
		ITK(AOM_save(tCurrentCadimDbEntry) );
		ITK(AOM_refresh(tCurrentCadimDbEntry, false));
	}

	// Free
	SAFE_SM_FREE(taRelSecondaryImanExternalObjectLinks);
	SAFE_SM_FREE(sAttr_CC_RESPONSABLE_COMP_ENG);
	SAFE_SM_FREE(sEmailValue);
	SAFE_SM_FREE(saCadimDbEntryPropNames);

	return iRetVal;
}


int ccNameHelper_DownloadConfigXML(const char *sItemID)
{
	int         iRetVal    = ITK_ok;
	const char *sAttrNames = "item_id";
	int         iResults   = 0;
	tag_t *     taItems    = NULL;

	// Find Config Item
	ITK(ITEM_find_items_by_key_attributes(1, &sAttrNames, &sItemID, &iResults, &taItems) );

	if ( iResults == 1 )
	{
		tag_t  tConfigItemRev         = NULLTAG;
		tag_t  tRelType_Specification = NULLTAG;
		tag_t *taSpecifications       = NULL;
		int    iCountSpecification    = 0;

		// Get latest Revision
		ITK(ITEM_ask_latest_rev(taItems[0], &tConfigItemRev) );

		// Get related Datasets
		ITK(GRM_find_relation_type("IMAN_specification", &tRelType_Specification) );
		ITK(GRM_list_secondary_objects_only(tConfigItemRev, tRelType_Specification, &iCountSpecification, &taSpecifications) );

		// Only one specification allowed
		if ( iCountSpecification == 1 )
		{
			ITK(OBJIO__make_temp_staging_dir(&sLogDir) );
			sprintf_s(sPathToConfigFile, 256, "%s\\cc_naming_pattern.xml", sLogDir);

			ITK(AE_export_named_ref(taSpecifications[0], "Text", sPathToConfigFile) );
		}
		else
		{
			printf(" > ERROR: Not exactly one IMAN_specification found! \n");
			iRetVal = 919709;
		}

		// Free
		SAFE_SM_FREE(taSpecifications);
	}
	else
	{
		printf(" > ERROR: Config Item '%s' could not be found! \n", sItemID);
		iRetVal = 919708;
	}

	// Free
	SAFE_SM_FREE(taItems);

	return iRetVal;
}


/**
 * @brief
 * Generate CADIM Name
 */
int SBT_AH_generateCadimName(EPM_action_message_t msg)
{
	const char *theFunction        = "SBT_AH_generateCadimName";
	char*		genericType;
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


	/* Read Handler Arguments */
	int  iArgCount = IMAN_number_of_arguments(msg.arguments);
	char sParamCadimStatus[WSO_name_size_c + 1] = "";
	char sParamConfigItemID[ITEM_id_size_c + 1] = "";
	char sParamConfigFileExport[10] = "";

	for ( int iIdx1 = 0; iIdx1 < iArgCount; iIdx1++ )
	{
		// Get Param
		char *      sTmpParam   = IMAN_next_argument(msg.arguments);
		std::string strTmpParam = sTmpParam;

		// Get Delimiter-Pos
		unsigned iDelimPos = strTmpParam.find("=");

		if(iDelimPos != string::npos)
		{
			// Get Param Name and Value
			std::string strTmpParamName  = strTmpParam.substr(0, iDelimPos);
			std::string strTmpParamValue = strTmpParam.substr(iDelimPos + 1);

			// Transform Param Name to lower case
			std::transform(strTmpParamName.begin( ), strTmpParamName.end( ), strTmpParamName.begin( ), ::tolower);

			// Get Param Value
			if ( strTmpParamName.compare("cadimstatus") == 0 )
			{
				strcpy(sParamCadimStatus, strTmpParamValue.c_str( ) );
			}
			else if ( strTmpParamName.compare("itemid") == 0 )
			{
				strcpy(sParamConfigItemID, strTmpParamValue.c_str( ) );
			}
			else if ( strTmpParamName.compare("fileexport") == 0 )
			{
				strcpy(sParamConfigFileExport, strTmpParamValue.c_str( ) );
			}
		}
	}

	if ( /*( strcmp(sParamCadimStatus, "") == 0 ) ||*/ ( strcmp(sParamConfigItemID, "") == 0 ) )
	{
		//printf(" > ERROR: The ActionHandler 'SBT_AH_generateCadimName' requires the arguments 'CadimStatus' and 'ItemId' ! \n");
		printf(" > ERROR: The ActionHandler 'SBT_AH_generateCadimName' requires the argument 'ItemId' ! \n");
		ITK(EMH_store_error(EMH_severity_error, 919711) );
		return 919711;
	}

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1] = "";

		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, "BT7_CommonCompRevision") == 0 )
		{
			tCommonCompItemRev = pTargetTags[iIdx1];
			ITK(ITEM_ask_item_of_rev(tCommonCompItemRev, &tCommonCompItem) );
		}

		if ( strcmp(sObjectType, "BT7_CommonComp") == 0 )
		{
			tCommonCompItem = pTargetTags[iIdx1];
			ITK(ITEM_ask_latest_rev(tCommonCompItem, &tCommonCompItemRev) );
		}

		if ( tCommonCompItem != NULLTAG )
		{
			break;
		}
	}


	/* Generate name of Common Component */
	if ( tCommonCompItem != NULLTAG )
	{
		char *                sChildClassId = NULL;
		vector<TC_ICS_VALUES> vecRequiredICSAttributes;
		CC_NAME_PATTERN       requiredPattern;

		/* Init */
		vecCcNamePatternAttrRules.clear( );
		vecCcNamePatterns.clear( );
		vecRequiredICSAttributes.clear( );
		requiredPattern.sTcUAID = "";

		/* Download Config-XML */
		int iDownloadConfigXML = ITK_ok;

		if ( ( iDownloadConfigXML = ccNameHelper_DownloadConfigXML(sParamConfigItemID) ) != ITK_ok )
		{
			SAFE_SM_FREE(pTargetTags);
			return iDownloadConfigXML;
		}

		/* 1.) Get Classification Class-ID */
		ccNameHelper_GetClassificationID(tCommonCompItem, &sChildClassId);

		//RP
		int			counter = 0;
		int			mAttrIdCount= 0;
		int*		maAttrIds = 0;
		int*		maAttrValueCount = 0;
		char***		mAttrValues = 0;
		char*		tmp_name = NULL;
		int			m_idCount	= 0;
		int			c_idCount	= 0;
		char**		mclassID	= NULL;
		char**		cclassID	= NULL;
		tag_t		mClassificationObject = NULL;
		tag_t		tCommomCompRev = NULL;
		tag_t		tParent = NULLTAG;
		char*		idParent = NULL;
		char*		idChild = sChildClassId;
		char*		tmp = NULL;
		char*		tmpString1 = NULL;
		char*		tmpString2 = NULL;
		string		objName;
		boolean mflag = false;
		boolean cflag = false;

		ITK(PREF_ask_char_values("ICS_Mechanical_component_classID",&m_idCount,&mclassID));
		ITK(PREF_ask_char_values("ICS_Common_component_classID",&c_idCount,&cclassID));
		do
		{
			ITK(ICS_class_ask_parent(idChild,&tParent,&idParent));
			if(strcmp(idParent,mclassID[0])==0)
			{
				mflag = true;
				break;
			}
			else if(strcmp(idParent,cclassID[0])==0)
			{
				cflag = true;
				break;
			}
			else
			{
				mflag = false;
				cflag = false;
				idChild = idParent;
			}
		}while((mflag == false)||(cflag == false));
				
		if(mflag == true)
		{
				TC_write_syslog("\nBT7 Electronic & Electronic-Mechanic Comp classified under SBT Mechanical Components group");
				ITK(ICS_ask_classification_object(tCommonCompItem, &mClassificationObject));
				ITK(ICS_ico_ask_attributes(mClassificationObject, &mAttrIdCount, &maAttrIds, &maAttrValueCount, &mAttrValues));
				for(int m=0;m<mAttrIdCount;m++)
				{
					for(int n= 0;n<maAttrValueCount[m];n++)
					{

 						tmp = mAttrValues[m][n];
						if(counter==0)
							objName.assign(tmp);
						else
							objName.append(tmp);
				
						objName.append("_");
						counter = 1;
					}
				}
				objName.pop_back();
				if(objName.length()>40)
				{
					objName = objName.substr(0, 40);
				}
				//Modify BT7 Electronic & Electronic-Mechanic Comp object name to AttrVal1_AttrVal2_AttrVal3
				ITK(ITEM_find_revision(tCommonCompItem,"A",&tCommomCompRev));
				ITK(AOM_refresh(tCommonCompItem,true));
				ITK(AOM_set_value_string(tCommonCompItem,"object_name",objName.c_str()));
				ITK(AOM_save(tCommonCompItem));
				ITK(AOM_refresh(tCommonCompItem,false));

				ITK(AOM_refresh(tCommomCompRev,true));
				ITK(AOM_set_value_string(tCommomCompRev,"object_name",objName.c_str()));
				ITK(AOM_save(tCommomCompRev));
				ITK(AOM_refresh(tCommomCompRev,false));
			}
		
			else if(cflag == true)
			{
				/* 2.) Read Config-XML */
				ccNameHelper_ReadConfigXML(sPathToConfigFile);

				// Identify required ICS-Attributes
				for ( int iIdx1 = 0; iIdx1 < vecCcNamePatterns.size( ); iIdx1++ )
				{
					if ( vecCcNamePatterns.at(iIdx1).sTcUAID.compare(sChildClassId) == 0 )
					{
						requiredPattern = vecCcNamePatterns.at(iIdx1);

						for ( int iIdx2 = 0; iIdx2 < requiredPattern.vecCcNamePatternParameters.size( ); iIdx2++ )
						{
							if ( requiredPattern.vecCcNamePatternParameters.at(iIdx2).sICS_AttrID.compare("") != 0 )
							{
								TC_ICS_VALUES tmpIcsValue;
								tmpIcsValue.sICSAttrID = requiredPattern.vecCcNamePatternParameters.at(iIdx2).sICS_AttrID;
								vecRequiredICSAttributes.push_back(tmpIcsValue);
							}
						}

						break;
					}
				}

				/* 3.) Get req. ICS Attribute Values */
				ccNameHelper_GetClassificationAttributes(tCommonCompItem, &vecRequiredICSAttributes);

				if(requiredPattern.sTcUAID.compare("") == 0)
				{
					/* Free */
					SAFE_SM_FREE(pTargetTags);

					printf(" > ERROR: Common Component Item is not classified or the naming-rule can not be found ! \n");
					ITK(EMH_store_error(EMH_severity_error, 919716) );
					return 919716;
				}																					

				/* 4.) Generate Name */
				stringstream ss;

				for ( int iIdx1 = 0; iIdx1 < requiredPattern.vecCcNamePatternParameters.size( ); iIdx1++ )
				{
					CC_NAME_PATTERN_PARAMETER ccNameParam = requiredPattern.vecCcNamePatternParameters.at(iIdx1);

					if ( ccNameParam.sSource.compare("string") == 0 )
					{
						ss << ccNameParam.sValueEN.c_str( );
					} else if ( ccNameParam.sSource.compare("ics") == 0 ) {
						string sTmpICSValue = "";

						sTmpICSValue = ccNameHelper_GetICSValue(vecRequiredICSAttributes, ccNameParam, true);

						// ICS-Value found?
						if ( sTmpICSValue.compare("") != 0 )
						{
							// Trim leading & trailing zeros from number-string
							sTmpICSValue = ccNameHelper_TrimLeadingZerosFromNumberString(sTmpICSValue);
							sTmpICSValue = ccNameHelper_TrimTrailingZerosFromNumberString(sTmpICSValue);

							ss << sTmpICSValue;
						} else {
							printf(" > ERROR: ICS-Attribute '%s' not found! \n", ccNameParam.sICS_AttrID.c_str( ) );
						}
					} else if ( ccNameParam.sSource.compare("ics_string") == 0 ) {
						string sTmpICSValue = "";

					sTmpICSValue = ccNameHelper_GetICSValue(vecRequiredICSAttributes, ccNameParam, true);

					// ICS-Value found?
					if ( sTmpICSValue.compare("") != 0 )
					{
					
						// Kai: CaseSize lost his leading Zeros 09/02/2015
						// Trim leading zeros from number-string
						// sTmpICSValue = ccNameHelper_TrimLeadingZerosFromNumberString(sTmpICSValue);
					
						ss << sTmpICSValue;
					} else {
						printf(" > ERROR: ICS_STRING-Attribute '%s' not found! \n", ccNameParam.sICS_AttrID.c_str( ) );
					}
				} else if ( ccNameParam.sSource.compare("ics_string_only_exported") == 0 ) {
				} else if ( ccNameParam.sSource.compare("ics_only_exported") == 0 ) {
				} else if ( ccNameParam.sSource.compare("ics_lov") == 0 )
				{
					string sTmpICSValue = "";

					// Traverse ICS-Vector
					for ( int iIdx2 = 0; iIdx2 < vecRequiredICSAttributes.size( ); iIdx2++ )
					{
						TC_ICS_VALUES tmp_ICS_Value = vecRequiredICSAttributes.at(iIdx2);
						std::transform(tmp_ICS_Value.sICSAttrValue.begin( ), tmp_ICS_Value.sICSAttrValue.end( ), tmp_ICS_Value.sICSAttrValue.begin( ), ::tolower);

						// Correct Attribute found ?
						if ( tmp_ICS_Value.sICSAttrID.compare(ccNameParam.sICS_AttrID) == 0 )
						{
							// Traverse possible LOV-Values of the parameter
							for ( int iIdx3 = 0; iIdx3 < ccNameParam.vecCcNamePatternParameterLovEntries.size( ); iIdx3++ )
							{
								CC_NAME_PATTERN_PARAMETER_LOV tmpLOV_Entry        = ccNameParam.vecCcNamePatternParameterLovEntries.at(iIdx3);
								CC_NAME_PATTERN_PARAMETER_LOV tmpLOV_EntryToLower = ccNameParam.vecCcNamePatternParameterLovEntries.at(iIdx3);
								std::transform(tmpLOV_EntryToLower.sLovValue.begin( ), tmpLOV_EntryToLower.sLovValue.end( ), tmpLOV_EntryToLower.sLovValue.begin( ), ::tolower);

								// Check lower-case values
								if ( tmpLOV_EntryToLower.sLovValue.compare(tmp_ICS_Value.sICSAttrValue) == 0 )
								{
									sTmpICSValue = tmpLOV_Entry.sLovShortValue;
									break;
								}
							}

							if ( sTmpICSValue.compare("") != 0 )
							{
								break;
							}
						}
					}

					// ICS-ShortValue found?
					if ( sTmpICSValue.compare("") != 0 )
					{
						ss << sTmpICSValue;
					}
					else
					{
						printf(" > ERROR: ICS_LoV_Attribute '%s' not found! \n", ccNameParam.sICS_AttrID.c_str( ) );
					}
				} 
				else if ( ccNameParam.sSource.compare("macro") == 0 ) 
				{
					if ( ccNameParam.sValueEN.compare("$MOUNTING$") == 0 )
					{
						logical lSMD = false;
						ITK(AOM_ask_value_logical(tCommonCompItemRev, "bt7_smd", &lSMD) );

						if ( lSMD == true )
						{
							ss << "SMD";
						}
						else
						{
							ss << "THD";            // TODO: Implement logic of the macro --> SPEC
						}					
					}
					// NEU
					else if ( ccNameParam.sValueEN.compare("$NORM$") == 0 )
					{
						ITK(AOM_ask_value_string(tCommonCompItemRev, "bt7_GenericType", &genericType) );
						if ( strcmp(genericType, "") != 0 )
						{
							ss << "_";
							ss << genericType;
						}
					}
					// NEU ENDE
					else
					{
						printf(" > ERROR: Macro '%s' not defined! \n", ccNameParam.sValueEN.c_str( ) );
					}
				} 
				else
				{
					printf(" > ERROR: XML-Param source '%s' not found! \n", ccNameParam.sSource.c_str( ) );
				}
			}// End generate CC Object Name


			// Set temporary bypass to TRUE
			logical lHasBypass = false;
			ITK(ITK_ask_bypass(&lHasBypass) );
			printf("Has bypass ? %i \n", lHasBypass);
			AM__set_application_bypass(true);

				// Shorten Object Name to 40 chars (if required)
				string strNewObjName = ss.str( );

				if ( strNewObjName.length( ) > 40 )
				{
					strNewObjName = strNewObjName.substr(0, 40);
				}

				printf(" > Generated Object Name based on migrated CADIM-rules: %s \n", strNewObjName.c_str( ) );

				// Set Name for Common Comp Item
				ITK(WSOM_set_name(tCommonCompItem, strNewObjName.c_str( ) ) );
				ITK(AOM_save(tCommonCompItem) );
				ITK(AOM_refresh(tCommonCompItem, false) );

				// Set Name for Common Comp Item Revisions
				int		iTmpRevCount	= 0;
				tag_t	*taTmpRevList	= NULL;
				ITK(ITEM_list_all_revs(tCommonCompItem, &iTmpRevCount, &taTmpRevList));

				for(int iIdx2 = 0; iIdx2 < iTmpRevCount; iIdx2++)
				{
					ITK(WSOM_set_name(taTmpRevList[iIdx2], strNewObjName.c_str( ) ) );
					ITK(AOM_save(taTmpRevList[iIdx2]) );
					ITK(AOM_refresh(taTmpRevList[iIdx2], false) );
				}

				// Free
				SAFE_SM_FREE(taTmpRevList);


			/* 5.) Update exchange table (-> stored on the CCR-form) */
			if ( strcmp(sParamCadimStatus, "") != 0 )
			{
				// Separate run for every Vendor Part + every Supplier Spec Document
				// 1st run: Common Componment
				// 2nd run: 1st Vendor Part
				// 3rd run: Supplier Spec of the Vendor Part
				// 4th run: 2nd Vendor Part
				// ...

				// Identify root-CCR-Form
				int    iCountRelReference        = 0;
				int    iCountRelVMRepresents     = 0;
				tag_t *taRelSpecificationObjects = NULL;
				tag_t *taRelVMRepresentsObjects  = NULL;
				tag_t  tRootCCRForm     = NULLTAG;
				tag_t  tRelReference    = NULLTAG;
				tag_t  tRelVMRepresents = NULLTAG;
				tag_t  tRelSpecification= NULLTAG;

				char  sAttr_CC_PARTNUMBER[ITEM_id_size_c + 1];
				char *sAttr_CC_LGUSRBEMERKUNG = NULL;

				// Init global attributes for the CCR-Forms
				ITK(ITEM_ask_id(tCommonCompItem, sAttr_CC_PARTNUMBER) );
				ITK(AOM_ask_value_string(tCommonCompItem, "bt7_remark", &sAttr_CC_LGUSRBEMERKUNG) );

				// Init Relationships
				ITK(GRM_find_relation_type("IMAN_specification", &tRelSpecification) );
				ITK(GRM_find_relation_type("IMAN_reference", &tRelReference) );
				ITK(GRM_find_relation_type("VMRepresents", &tRelVMRepresents) );

				// Find Root-CCR-Form below Item
				ITK(GRM_list_secondary_objects_only(tCommonCompItem, tRelReference, &iCountRelReference, &taRelSpecificationObjects) );
				for ( int iIdx1 = 0; iIdx1 < iCountRelReference; iIdx1++ )
				{
					char sObjectType[WSO_name_size_c + 1];

					ITK(WSOM_ask_object_type(taRelSpecificationObjects[iIdx1], sObjectType) );

					if ( strcmp(sObjectType, "BT7_CCR") == 0 )
					{
						tRootCCRForm = taRelSpecificationObjects[iIdx1];
						break;
					}
				}

				// Find Root-CCR-Form below ItemRevision if not found below Item
				if ( tRootCCRForm == NULLTAG )
				{
					SAFE_SM_FREE(taRelSpecificationObjects);
					iCountRelReference = 0;
					ITK(GRM_list_secondary_objects_only(tCommonCompItemRev, tRelSpecification, &iCountRelReference, &taRelSpecificationObjects) );
					for ( int iIdx1 = 0; iIdx1 < iCountRelReference; iIdx1++ )
					{
						char sObjectType[WSO_name_size_c + 1];

						ITK(WSOM_ask_object_type(taRelSpecificationObjects[iIdx1], sObjectType) );

						if ( strcmp(sObjectType, "BT7_CCR") == 0 )
						{
							tRootCCRForm = taRelSpecificationObjects[iIdx1];
							break;
						}
					}
				}

				// CCR-Form found?
				if ( tRootCCRForm != NULLTAG )
				{
					// Update Root-CCR-Form
					ccNameHelper_UpdateCCRForm(tRootCCRForm, tCommonCompItem, tCommonCompItemRev, 1 /* Modus=Root */, sAttr_CC_PARTNUMBER, sAttr_CC_LGUSRBEMERKUNG, requiredPattern, vecRequiredICSAttributes, sParamCadimStatus);

					// Read Vendor Part Items
					ITK(GRM_list_secondary_objects_only(tCommonCompItemRev, tRelVMRepresents, &iCountRelVMRepresents, &taRelVMRepresentsObjects) );

					// Traverse Vendor Part Items
					for ( int iIdx1 = 0; iIdx1 < iCountRelVMRepresents; iIdx1++ )
					{
						// Update VendorPart-CCR-Form
						ccNameHelper_UpdateCCRForm(tRootCCRForm, taRelVMRepresentsObjects[iIdx1], NULLTAG, 2 /* Modus=VendorPart */, sAttr_CC_PARTNUMBER, sAttr_CC_LGUSRBEMERKUNG, requiredPattern, vecRequiredICSAttributes, sParamCadimStatus);
					}

					// Read Datasets below CCR-Form
					int    iExternalObjectLinkCount   = 0;
					tag_t  tRelTypeExternalObjectLink = NULLTAG;
					tag_t *taExternalObjectLinks      = NULL;

					ITK(GRM_find_relation_type("IMAN_external_object_link", &tRelTypeExternalObjectLink) );
					ITK(GRM_list_secondary_objects_only(tRootCCRForm, tRelTypeExternalObjectLink, &iExternalObjectLinkCount, &taExternalObjectLinks) );

					// Document-Entry (-> simulated, there is not BT7_Document in TcUA!)
					if ( iExternalObjectLinkCount > 0 )
					{
						ccNameHelper_UpdateCCRForm(tRootCCRForm, NULLTAG, NULLTAG, 3 /* Modus=Document */, sAttr_CC_PARTNUMBER, sAttr_CC_LGUSRBEMERKUNG, requiredPattern, vecRequiredICSAttributes, sParamCadimStatus);
					}

					// Traverse Datasets (--> Datasheets)
					for ( int iIdx1 = 0; iIdx1 < iExternalObjectLinkCount; iIdx1++ )
					{
						tag_t tObjClass = NULLTAG;

						// Is derived from Dataset
						if ( object_is_of_class_or_desc(taExternalObjectLinks[iIdx1], "Dataset", &tObjClass) )
						{
							// File shall be exported
							if ( strcmp(sParamConfigFileExport, "true") == 0 )
							{
								// Update VendorPart-CCR-Form
								ccNameHelper_UpdateCCRForm(tRootCCRForm, taExternalObjectLinks[iIdx1], NULLTAG, 4 /* Modus=Files */, sAttr_CC_PARTNUMBER, sAttr_CC_LGUSRBEMERKUNG, requiredPattern, vecRequiredICSAttributes, sParamCadimStatus);
							}
						}
					}

					// Free
					SAFE_SM_FREE(taRelVMRepresentsObjects);
					SAFE_SM_FREE(taExternalObjectLinks);
				}
				else
				{
					printf(" > ERROR: Root-CCR-Form could not be identified! \n");
				}

				// Free
				SAFE_SM_FREE(taRelSpecificationObjects);
				SAFE_SM_FREE(sAttr_CC_LGUSRBEMERKUNG);
			}

			// Reset Bypass
			AM__set_application_bypass(lHasBypass);

			// Delete temporary staging dir
			if ( sLogDir != NULL )
			{
				ITK(OBJIO_delete_staging_dir(sLogDir) );
				SAFE_SM_FREE(sLogDir);
			}

			/* Free */
			SAFE_SM_FREE(sChildClassId);

			/* Clear vectors */
			vecCcNamePatternAttrRules.clear( );
			vecCcNamePatterns.clear( );
			vecRequiredICSAttributes.clear( );
			}
			
		}
		else
		{
			// SF: Commented out because also BT7_Components are allowed
			//printf(" > ERROR: The Handler 'SBT_AH_generateCadimName' could not identify the Common Component \n");
			//SAFE_SM_FREE(pTargetTags);
			//return -1;
		}
		/* Free */
			SAFE_SM_FREE(pTargetTags);

			LEAVE(theFunction);
			return retcode;
} /* End SBT_AH_generateCadimName */

//logical isDerivedFromClass(char *className, tag_t tObjectTag)
//{
//	tag_t   tClassIdTag          = NULLTAG;
//	tag_t   tSecondaryClassIdTag = NULLTAG;
//	logical bIsDerived           = false;
//
//	ITK(POM_class_id_of_class(className, &tClassIdTag) );
//
//	if ( tClassIdTag != NULLTAG )
//	{
//		ITK(POM_class_of_instance(tObjectTag, &tSecondaryClassIdTag) );
//
//		if ( tSecondaryClassIdTag != NULLTAG )
//		{
//			ITK(POM_is_descendant(tClassIdTag, tSecondaryClassIdTag, &bIsDerived) );
//		}
//	}
//	return bIsDerived;
//}

/**
 * @brief
 * Replace the CCR-classification with the CC
 */
int SBT_AH_replaceCcrClassification(EPM_action_message_t msg)
{
	const char *theFunction     = "SBT_AH_replaceCcrClassification";
	int         retcode         = ITK_ok;
	int         iTargetCount    = 0;
	tag_t       rootTaskTag     = NULLTAG;
	tag_t *     pTargetTags     = NULL;
	tag_t       tCommonCompItem = NULLTAG;
	tag_t       tCommonCompReq  = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1] = "";

		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, "BT7_CommonComp") == 0 )
		{
			tCommonCompItem = pTargetTags[iIdx1];
		}
		else if ( strcmp(sObjectType, "BT7_CCR") == 0 )
		{
			tCommonCompReq = pTargetTags[iIdx1];
		}

		if ( ( tCommonCompItem != NULLTAG ) && ( tCommonCompReq != NULLTAG ) )
		{
			break;
		}
	}

	if ( ( tCommonCompItem != NULLTAG ) && ( tCommonCompReq != NULLTAG ) )
	{
		tag_t tCC_ClassificationObject  = NULLTAG;
		tag_t tCCR_ClassificationObject = NULLTAG;
		tag_t tRelationType             = NULLTAG;
		tag_t tNewRelation = NULLTAG;

		//tag_t	tLatestCCRev				= NULLTAG;
		tag_t   tViewTag         = NULLTAG;
		int     iAttrIdCount     = 0;
		int *   iaAttrValueCount = NULL;
		int *   iaAttrIds        = NULL;
		char ***saAttrValues     = NULL;
		logical lIsCommonCompItemClassified = false;
		logical lIsCommonCompReqClassified  = false;

		// Are the objects classified ?
		ITK(ICS_is_wsobject_classified(tCommonCompItem, &lIsCommonCompItemClassified) );
		ITK(ICS_is_wsobject_classified(tCommonCompReq, &lIsCommonCompReqClassified) );

		if(lIsCommonCompReqClassified == true)
		{
			// get the classification-values of the ccr
			ITK(ICS_ask_classification_object(tCommonCompReq, &tCCR_ClassificationObject) );

			//ITK ( ICS_ask_attributes_of_classification_obj ( tCCR_ClassificationObject, &iaAttrValueCount, &saAttrNames, &saAttrValues ) );
			ITK(ICS_ico_ask_attributes(tCCR_ClassificationObject, &iAttrIdCount, &iaAttrIds, &iaAttrValueCount, &saAttrValues) );

			ITK(ICS_ask_view_of_classification_obj(tCCR_ClassificationObject, &tViewTag) );

			// delete cc classification (if exists)
			if ( lIsCommonCompItemClassified )
			{
				ITK(ICS_ask_classification_object(tCommonCompItem, &tCC_ClassificationObject) );
				ITK(ICS_delete_reference_to_ico(tCC_ClassificationObject) );
				ITK(ICS_delete_classification_object(tCC_ClassificationObject) );

				tCC_ClassificationObject = NULLTAG;
			}

			// classify cc with values read from ccr
			ITK(ICS_create_classification_object(tCommonCompItem, "-", tViewTag, &tCC_ClassificationObject) );
			ITK(ICS_classify_wsobject(tCommonCompItem, tCC_ClassificationObject) );
			
			char *sClassId = NULL;

			int    theCount;                                           /**< (O) The number of attributes of this class */
			int *  theIds;                                             /**< (OF) theCount The attribute id */
			char **theNames;                                           /**< (OF) theCount The display name */
			char **theShortNames;                                      /**< (OF) theCount The short version of the display name */
			char **theAnnotations;                                     /**< (OF) theCount The annotation */
			int *  theArraySize;                                       /**< (OF) theCount 1 for non vla attributes, > 1 for vla attribute */
			int *  theFormat;                                          /**< (OF) theCount The internal format definition.
																		*                                                              Use ICS_describe_format() to get details. */
			char **theUnit;                                            /**< (OF) theCount */
			char **theMinValues;                                       /**< (OF) theCount Minimum allowed value ( valid for integer and float attributes) */
			char **theMaxValues;                                       /**< (OF) theCount Maximum allowed value ( valid for integer and float attributes) */
			char **theDefaultValues;                                   /**< (OF) theCount Default value for new Instances */
			char **theDescriptions;                                    /**< (OF) theCount The attribute description from the dictionary */
			int *  theOptions;                                         /**< (OF) theCount Bit encoded values from #ICS_class_attr_flags_e */			
			
			
			// Read class id
			ITK(ICS_ico_ask_class(tCC_ClassificationObject, &sClassId) );
				
			// Get attribute definitions for the current class id
			ITK(ICS_class_describe_attributes(sClassId, &theCount, &theIds, &theNames, &theShortNames, &theAnnotations, &theArraySize, &theFormat, &theUnit, &theMinValues, &theMaxValues, &theDefaultValues, &theDescriptions, &theOptions) );
			
			// Traverse attribute definitions
			for ( int iIdx1 = 0; iIdx1 < theCount; iIdx1++ )
			
			// Traverse attribute definitions
			for ( int iIdx1 = 0; iIdx1 < theCount; iIdx1++ )
			{
				// if format < 0 : KeyLOV
				if ( theFormat[iIdx1] < 0 )
				{
					// identify corresponding attribute in the current value-set
					for ( int iIdx2 = 0; iIdx2 < iAttrIdCount; iIdx2++ )
					{
						if ( iaAttrIds[iIdx2] == theIds[iIdx1] )
						{
							// Matching property found
							int    iPopUpCount        = 0;
							char **sPopUpOptionIDs    = NULL;
							char **sPopUpOptionValues = NULL;
							
							// get KeyLOV definition
							ITK(ICS_get_popup_definition(theFormat[iIdx1], &iPopUpCount, &sPopUpOptionIDs, &sPopUpOptionValues) );
							
							for ( int iIdx3 = 0; iIdx3 < iPopUpCount; iIdx3++ )
							{
								// Match found ? - CAUTION: If Hide Key LoVs is not checked:
								// sPopUpOptionValues[iIdx3] = sPopUpOptionIDs[iIdx3] + " " + saAttrValues[iIdx2][0]
								if ( strcmp(sPopUpOptionValues[iIdx3], saAttrValues[iIdx2][0]) == 0 )
								{
									saAttrValues[iIdx2][0] = sPopUpOptionIDs[iIdx3];
									break;
								}
							}
							// free
							SAFE_SM_FREE(sPopUpOptionIDs);
							SAFE_SM_FREE(sPopUpOptionValues);

							break;
						}
					}
				}
			}

			SAFE_SM_FREE(theIds);
			SAFE_SM_FREE(theNames);
			SAFE_SM_FREE(theShortNames);
			SAFE_SM_FREE(theAnnotations);
			SAFE_SM_FREE(theArraySize);
			SAFE_SM_FREE(theFormat);
			SAFE_SM_FREE(theUnit);
			SAFE_SM_FREE(theMinValues);
			SAFE_SM_FREE(theMaxValues);
			SAFE_SM_FREE(theDefaultValues);
			SAFE_SM_FREE(theDescriptions);
			SAFE_SM_FREE(theOptions);
			
			// This function is causing problems with KeyLOVs -> not with the working test implemented above! -> recheck!
			ITK(ICS_ico_set_attributes(tCC_ClassificationObject, iAttrIdCount, iaAttrIds, iaAttrValueCount, (const char ***) saAttrValues) );

			// delete ccr classification
			ITK(ICS_delete_reference_to_ico(tCCR_ClassificationObject) );
			ITK(ICS_delete_classification_object(tCCR_ClassificationObject) );

			// Free
			if ( saAttrValues != NULL )
			{
				SAFE_SM_FREE(saAttrValues);
			}

			if ( iaAttrIds != NULL )
			{
				SAFE_SM_FREE(iaAttrIds);
			}

			if ( iaAttrValueCount != NULL )
			{
				SAFE_SM_FREE(iaAttrValueCount);
			}

			// Clear error stack to avoid message "Option not found: MRMPropagationAlternateAttributes"
			ITK(EMH_clear_errors( ) );
		}
	}
	else
	{
		retcode = 919705;
	}

	// Free
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_replaceCcrClassification */
