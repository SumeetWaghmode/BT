#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

//#include <string.h>

#include <fclasses/tc_date.h>
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
#include <ps\ps.h>
#include <bom\bom.h>
#include <cfm\cfm.h>
#include <ae/ae.h>
#include <sa/tcfile.h>

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

// --------------------------------------------------------------------------

struct META_DATA_PROPERTY
{
	std::string sPropName;
	std::string sPropValue;
};

struct META_DATA_RELATION
{
	std::string sRelId;
	std::string sRelType;
	std::string sRelSecondaryObjectName;
};

struct META_DATA_BOMLINE
{
	std::string                sBomLineUID;
	std::string                sBomLineSeqNo;
	std::string                sBomLineRefDes;

	vector<META_DATA_PROPERTY> vecBomLineProperties;
};

struct META_DATA_ROOT
{
	vector<META_DATA_PROPERTY> vecParentProperties;
	vector<META_DATA_RELATION> vecParentRelations;
	vector<META_DATA_BOMLINE>  vecParentBomLines;
};

// --------------------------------------------------------------------------

int putStringPropertyToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, const char *propTcName, const char *propTcValue, const char *xmlTagName);
void getAllBOMLines(tag_t tRevTag, int *childBOMLinesCount, tag_t **childBOMLines);
int exportProperties(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr xmlNode, tag_t tObject, const char *sXmlTagName, const char *sModus);

logical compareXML(const char *sFileLoc1, const char *sFileLoc2, stringstream *txtStringStream);
META_DATA_ROOT readXMLFile(const char *sPathToXML);
string getXMLNodeValue(MSXML2::IXMLDOMNodePtr pNode, char *sAttrName);

// --------------------------------------------------------------------------

vector<string> vecValidProperties_Parent;
vector<string> vecValidProperties_BomLine;

// --------------------------------------------------------------------------

//
// DEFINES
//
#define BT7_LOV_MECH_COMP               ("pt_mech")
// --------------------------------------------------------------------------

void initPropertyVectors( )
{
	// Valid Parent Properties
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("bt7_manulocked");
	vecValidProperties_Parent.push_back("bt7_currentmanuf");
	vecValidProperties_Parent.push_back("object_desc");
	vecValidProperties_Parent.push_back("item_revision_id");
	vecValidProperties_Parent.push_back("ead_paragraph");
	// obsolete
	vecValidProperties_Parent.push_back("bt7_Treatment_1");
	vecValidProperties_Parent.push_back("bt7_docmnumber");
	// obsolete
	vecValidProperties_Parent.push_back("bt7_Treatment_2");
	vecValidProperties_Parent.push_back("bt7_SAPDIRNo");
	vecValidProperties_Parent.push_back("bt7_mailorg");
	vecValidProperties_Parent.push_back("bt7_docid");
	vecValidProperties_Parent.push_back("ics_classified");
	vecValidProperties_Parent.push_back("ip_classification");
	vecValidProperties_Parent.push_back("bt7_parttype");
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("bt7_zulce");
	vecValidProperties_Parent.push_back("bt7_MassUnit");
	vecValidProperties_Parent.push_back("bt7_rohs_compliant");
	vecValidProperties_Parent.push_back("bt7_zultuv");
	vecValidProperties_Parent.push_back("bt7_MassValue");
	vecValidProperties_Parent.push_back("bt7_remark");
	vecValidProperties_Parent.push_back("bt7_SAPMatNo");
	vecValidProperties_Parent.push_back("bt7_ApprUser_2");
	vecValidProperties_Parent.push_back("item_id");
	vecValidProperties_Parent.push_back("bt7_rtp_ApprDate_DocMapp");
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("gov_classification");
	vecValidProperties_Parent.push_back("bt7_ItemRevName_2");
	vecValidProperties_Parent.push_back("bt7_zuluse");
	vecValidProperties_Parent.push_back("bt7_stdprtind");
	vecValidProperties_Parent.push_back("bt7_VolumeValue");
	vecValidProperties_Parent.push_back("bt7_zulgrd");
	vecValidProperties_Parent.push_back("bt7_rtp_latest_rel_status");
	vecValidProperties_Parent.push_back("bt7_orgtype");
	vecValidProperties_Parent.push_back("bt7_techok");
	vecValidProperties_Parent.push_back("bt7_zuloth");

	//vecValidProperties_Parent.push_back("object_string");
	vecValidProperties_Parent.push_back("bt7_validation");

	//vecValidProperties_Parent.push_back("current_name");
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("bt7_doccanceled");
	vecValidProperties_Parent.push_back("bt7_default_quantity");
	vecValidProperties_Parent.push_back("bt7_doccancelnr");
	vecValidProperties_Parent.push_back("bt7_cadimno");
	vecValidProperties_Parent.push_back("bt7_tceno");
	vecValidProperties_Parent.push_back("bt7_wbsreq");
	vecValidProperties_Parent.push_back("bt7_MaterialRequirement_1");
	vecValidProperties_Parent.push_back("bt7_MaterialRequirement_2");
	vecValidProperties_Parent.push_back("bt7_cp_MaterialStandard");
	vecValidProperties_Parent.push_back("bt7_Supplier_1");
	vecValidProperties_Parent.push_back("bt7_smd");
	vecValidProperties_Parent.push_back("bt7_SFP_Dimension");
	vecValidProperties_Parent.push_back("bt7_Surface_1");
	vecValidProperties_Parent.push_back("bt7_Surface_2");
	vecValidProperties_Parent.push_back("bt7_rtp_resuom");
	vecValidProperties_Parent.push_back("bt7_shortinfo");
	vecValidProperties_Parent.push_back("bt7_2ndLanguageCode");
	vecValidProperties_Parent.push_back("bt7_Supplier_1_PartNo");
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("bt7_assytype");
	vecValidProperties_Parent.push_back("bt7_sapmando");
	vecValidProperties_Parent.push_back("bt7_FirstUse");
	vecValidProperties_Parent.push_back("bt7_equalpart");
	vecValidProperties_Parent.push_back("bt7_techinfo");
	vecValidProperties_Parent.push_back("object_name");
	//vecValidProperties_Parent.push_back("current_desc");
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("effectivity_text");
	// obsolete
	// obsolete
	vecValidProperties_Parent.push_back("BT7_cp_MaterialNumber");
	vecValidProperties_Parent.push_back("bt7_preferred");
	vecValidProperties_Parent.push_back("bt7_zulvde");
	vecValidProperties_Parent.push_back("bt7_Subtitle_1");
	vecValidProperties_Parent.push_back("bt7_sourceok");
	vecValidProperties_Parent.push_back("bt7_Subtitle_2");
	vecValidProperties_Parent.push_back("bt7_GeneralTolerance");
	vecValidProperties_Parent.push_back("bt7_Responsible");
	vecValidProperties_Parent.push_back("bt7_releaser");
	vecValidProperties_Parent.push_back("revision_number");
	vecValidProperties_Parent.push_back("bt7_zulappro");
	vecValidProperties_Parent.push_back("current_id");
	vecValidProperties_Parent.push_back("checked_out");
	vecValidProperties_Parent.push_back("BT7_cp_MaterialColour");

	// Valid Relation Properties (-> Secondary Object)
	//vecValidProperties_Relation.push_back("object_name");

	// Valid BomLine Properties
	vecValidProperties_BomLine.push_back("BT7_EBOM_Relevant");
	vecValidProperties_BomLine.push_back("bl_sequence_no");
	vecValidProperties_BomLine.push_back("bl_occ_notes_ref");
	vecValidProperties_BomLine.push_back("bl_occ_ref_designator");
	vecValidProperties_BomLine.push_back("bl_occ_is_precise");
	vecValidProperties_BomLine.push_back("bl_occ_IMAN_MEWorkInstruction");
	vecValidProperties_BomLine.push_back("bl_occ_IMAN_MERequiredAppr");
	vecValidProperties_BomLine.push_back("bl_occ_seq_no");
	vecValidProperties_BomLine.push_back("bl_occ_xform");
	vecValidProperties_BomLine.push_back("bl_occ_occ_type");
	vecValidProperties_BomLine.push_back("bl_occ_qty_value");
	vecValidProperties_BomLine.push_back("bl_occ_type");
	vecValidProperties_BomLine.push_back("bl_occurrence_name");
	vecValidProperties_BomLine.push_back("bl_occ_occurrence_name");
	vecValidProperties_BomLine.push_back("bl_occurrence_uid");
	vecValidProperties_BomLine.push_back("bl_occ_date_range");
	vecValidProperties_BomLine.push_back("bl_occ_cd_tags");
}


std::string from_variant(VARIANT & vt)
{
	_bstr_t bs(vt);

	return std::string(static_cast<const char *>( bs ) );
}

/**
* @brief
* Verfifies if the 3D-workflow can by skipped (if the process owner is a member of the 
* group PCBDesign + the target is a PCB-raw or PCB-A)
*/
EPM_decision_t SBT_RH_elec3d_bypass_check_for_pcbraw_possible(EPM_rule_message_t msg)
{
	int         retcode         = ITK_ok;
	int         iTargetCount    = 0;
	const char *theFunction     = "SBT_RH_elec3d_bypass_check_for_pcbraw_possible";
	char *      sDecision       = EPM_RESULT_False;
	tag_t       rootTaskTag     = NULLTAG;
	tag_t *     pTargetTags     = NULLTAG;

	std::stringstream txtStringStream;

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	// Get the root task
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	// Ask processowner
	char    sGroupName[SA_name_size_c + 1] = "";

	tag_t	tGroupMember	= NULLTAG;
	tag_t	tResourcePool	= NULLTAG;
	logical	isResourcePool	= false;


	tag_t tOwningUser = NULLTAG;
	tag_t tOwningGroup = NULLTAG;
	ITK(POM_ask_owner(rootTaskTag, &tOwningUser, &tOwningGroup));
	ITK(SA_ask_group_name(tOwningGroup, sGroupName));


	//ITK(EPM_get_participant("$PROCESSOWNER", msg.task, &tGroupMember, &tResourcePool, &isResourcePool));

	//if(isResourcePool == true)
	//{
	//	// Resourcepool
	//	tag_t	tGroup			= NULLTAG;
	//	tag_t	tRole			= NULLTAG;
	//	logical	lAllowSubGroups	= false;

	//	ITK(EPM_ask_resource_pool_group_role(tResourcePool, &tGroup, &tRole, &lAllowSubGroups) );
	//	ITK(SA_ask_group_name(tGroup, sGroupName));
	//} else {
	//	// Groupmember
	//	tag_t tGroup	= NULLTAG;

	//	ITK(SA_ask_groupmember_group(tGroupMember, &tGroup));
	//	ITK(SA_ask_group_name(tGroup, sGroupName));
	//}

	printf(" > Group of process initiator: %s \n", sGroupName);

	// Current responsible party is "110_PCBDesign_A" ?
	if(strcmp(sGroupName, "110_PCBDesign_A") == 0)
	{
		// Get the attachments
		if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
		{
			ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
		}

		// Loop attachments
		for(int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++)
		{
			char sObjectType[WSO_name_size_c+1];
			
			// Get Object Type
			ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType));

			// Current object is a Component?
			if(strcmp(sObjectType, "BT7_ComponentRevision") == 0)
			{
				char *sPartType = NULL;

				// Get Part Type
				ITK(AOM_ask_value_string(pTargetTags[iIdx1], "bt7_parttype", &sPartType));

				// Current Component is a PCB-raw ?
				// Issue 1012. 3D-Workflow - Bypass für PCB-A
				if(strcmp(sPartType, "pt_pcb") == 0 || strcmp(sPartType, "l7LGCodPEq") == 0)
				{
					sDecision = EPM_RESULT_True;

					// Free
					SAFE_SM_FREE(sPartType);

					break;
				}

				// Free
				SAFE_SM_FREE(sPartType);	
			}
		}
	}

	// Set result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );
	ITK(AOM_unlock(rootTaskTag) );

	// Free
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
} /* End SBT_RH_elec3d_bypass_check_for_pcbraw_possible */



/**
 * @brief
 * Verfifies if two meta data exports exists + if they are different
 */
EPM_decision_t SBT_RH_elec3d_compare_meta_data(EPM_rule_message_t msg)
{
	int         retcode         = ITK_ok;
	int         iTargetCount    = 0;
	int         iReferenceCount = 0;
	const char *theFunction     = "SBT_RH_elec3d_compare_meta_data";
	char *      sDecision       = EPM_RESULT_True;
	tag_t       rootTaskTag     = NULLTAG;
	tag_t *     pTargetTags     = NULLTAG;
	tag_t *     pReferenceTags  = NULLTAG;

	std::stringstream txtStringStream;

	ENTER(theFunction);
	ITK(EMH_clear_errors( ) );

	// Get the root task
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	// Get the attachments
	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
		ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );
	}

	// Compare Text-Files in the workflow references
	if ( iReferenceCount == 2 )
	{
		tag_t  tDataset_1 = pReferenceTags[0];
		tag_t  tDataset_2 = pReferenceTags[1];
		char   sObjectType_1[WSO_name_size_c + 1];
		char   sObjectType_2[WSO_name_size_c + 1];
		char   sObjectName_1[WSO_name_size_c + 1];
		char   sObjectName_2[WSO_name_size_c + 1];
		string strObjectName_1;
		string strObjectName_2;

		// Only Text Datasets attached
		ITK(WSOM_ask_object_type(tDataset_1, sObjectType_1) );
		ITK(WSOM_ask_object_type(tDataset_2, sObjectType_2) );

		ITK(WSOM_ask_name(tDataset_1, sObjectName_1) );
		ITK(WSOM_ask_name(tDataset_2, sObjectName_2) );
		strObjectName_1 = sObjectName_1;
		strObjectName_2 = sObjectName_2;

		logical lObjectName_1_PrefixOk = strObjectName_1.substr(0, strlen("TcMetaDataDump") ).compare("TcMetaDataDump") == 0;
		logical lObjectName_2_PrefixOk = strObjectName_2.substr(0, strlen("TcMetaDataDump") ).compare("TcMetaDataDump") == 0;

		if ( ( lObjectName_1_PrefixOk == true ) && ( lObjectName_2_PrefixOk == true ) && ( strcmp(sObjectType_1, "Text") == 0 ) && ( strcmp(sObjectType_2, "Text") == 0 ) )
		{
			char *       sTempDirectory = NULL;
			stringstream ssFileLoc_old;
			stringstream ssFileLoc_new;
			date_t       modDateDataset1;
			date_t       modDateDataset2;

			// Create temp-location on the stage
			ITK(OBJIO__make_temp_staging_dir(&sTempDirectory) );
			ssFileLoc_old << sTempDirectory << "/" << "tc_meta_data_dump_old.xml";
			ssFileLoc_new << sTempDirectory << "/" << "tc_meta_data_dump_new.xml";

			// Get last mod-dates
			ITK(AOM_ask_value_date(tDataset_1, "last_mod_date", &modDateDataset1) );
			ITK(AOM_ask_value_date(tDataset_2, "last_mod_date", &modDateDataset2) );

			// Export Files
			if ( CLOCK_compare_dates(&modDateDataset2, &modDateDataset1) > 0 )
			{
				ITK(AE_export_named_ref(tDataset_1, "Text", ssFileLoc_old.str( ).c_str( ) ) );
				ITK(AE_export_named_ref(tDataset_2, "Text", ssFileLoc_new.str( ).c_str( ) ) );
			}
			else
			{
				ITK(AE_export_named_ref(tDataset_2, "Text", ssFileLoc_old.str( ).c_str( ) ) );
				ITK(AE_export_named_ref(tDataset_1, "Text", ssFileLoc_new.str( ).c_str( ) ) );
			}

			// Compare XML-Files
			logical lCompareSuccess = compareXML(ssFileLoc_old.str( ).c_str( ), ssFileLoc_new.str( ).c_str( ), &txtStringStream);

			if ( lCompareSuccess == false )
			{
				sDecision = EPM_RESULT_False;

				//txtStringStream << "ERROR - XML-Files are different" << endl;
			}

			// Delete temp-location on the stage
			ITK(OBJIO_delete_staging_dir(sTempDirectory) );

			SAFE_SM_FREE(sTempDirectory);
		}
		else
		{
			sDecision = EPM_RESULT_False;
			txtStringStream << "ERROR - Attached Datasets does not have the expected format" << endl;
		}
	}
	else
	{
		sDecision = EPM_RESULT_False;
		txtStringStream << "ERROR - Attached Datasets does not have the expected format" << endl;
	}

	// Create Error-Report File
	if ( strcmp(sDecision, EPM_RESULT_False) == 0 )
	{
		// Create a new ImanFile
		tag_t      txtFile     = NULLTAG;
		IMF_file_t txtFileDesc = NULL;

		ITK(IMF_create_file(NULL, SS_TEXT, &txtFile, &txtFileDesc) );
		ITK(IMF_open_file(txtFileDesc, SS_RDWRITE) );

		// Write the XML-Stream to the new ImanFile
		std::string txtStringLine = "";

		while ( getline(txtStringStream, txtStringLine, '\n') )
		{
			if ( txtStringLine.length( ) <= SS_MAXLLEN )
			{
				ITK(IMF_write_file_line(txtFileDesc, txtStringLine.c_str( ) ) );
			}
		}

		// Close and save the new ImanFile
		ITK(IMF_close_file(txtFileDesc) );
		ITK(AOM_save(txtFile) );
		ITK(AOM_unload(txtFile) );

		// Create the dataset and upload the dataset
		tag_t tReportDataset = createDataset(msg.task, txtFile, "Meta Data Compare Report");

		// Set status of the Dataset
		tag_t tStatus;
		char	*sRealStatusName				= NULL;
		tag_t taReleaseObjects[1];
		taReleaseObjects[0] = tReportDataset;

		// Get the real name of status before creating it.
		returnStatusRealName(STATUS_TYPE_60,&sRealStatusName);

		ITK(CR_create_release_status(sRealStatusName, &tStatus) );
		ITK(AOM_save(tStatus) );
		ITK(EPM_add_release_status(tStatus, 1, taReleaseObjects, false) );
		ITK(AOM_refresh(tReportDataset, FALSE));
	}

	// Set result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );
	ITK(AOM_unlock(rootTaskTag) );

	// Free
	SAFE_SM_FREE(pTargetTags);
	SAFE_SM_FREE(pReferenceTags);

	LEAVE(theFunction);
	return EPM_go;
} /* End SBT_RH_elec3d_compare_meta_data */


/**
 * @brief
 * Export the meta data of an electronic component to identify later changes
 */
int SBT_AH_elec3d_export_meta_data(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_elec3d_export_meta_data";
	int         retcode     = ITK_ok;
	tag_t       rootTaskTag = NULLTAG;

	int    iTargetCount = 0;
	tag_t *pTargetTags  = NULL;

	tag_t tSelectedItemRevision = NULLTAG;


	// MSXML
	CoInitialize(NULL);

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	initPropertyVectors( );

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
			tSelectedItemRevision = pTargetTags[iIdx1];
			break;
		}
	}

	/* Target Object identified? */
	if ( tSelectedItemRevision != NULLTAG )
	{
		tag_t *taChildLines    = NULL;
		int    iChildLineCount = 0;

		// Create XML-File
		// Header
		MSXML2::IXMLDOMDocument2Ptr pXMLDoc;
		HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60) );
		pXMLDoc->loadXML(_T("<tc_meta_data_dump></tc_meta_data_dump>") );

		//Get the root element
		MSXML2::IXMLDOMElementPtr               pXMLRootElem       = pXMLDoc->GetdocumentElement( );
		MSXML2::IXMLDOMProcessingInstructionPtr pXMLProcessingNode = pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='ISO-8859-1'");

		_variant_t vtObject;
		vtObject.vt       = VT_DISPATCH;
		vtObject.pdispVal = pXMLRootElem;
		vtObject.pdispVal->AddRef( );

		pXMLDoc->insertBefore(pXMLProcessingNode, vtObject);

		// Define XML Entries
		MSXML2::IXMLDOMElementPtr xmlNodeTcProps = pXMLDoc->createElement(_T("tc_parent_object_properties") );
		xmlNodeTcProps = pXMLRootElem->appendChild(xmlNodeTcProps);

		MSXML2::IXMLDOMElementPtr xmlNodeTcRelations = pXMLDoc->createElement(_T("tc_parent_object_relations") );
		xmlNodeTcRelations = pXMLRootElem->appendChild(xmlNodeTcRelations);

		MSXML2::IXMLDOMElementPtr xmlNodeTcBOM = pXMLDoc->createElement(_T("tc_bom_lines") );
		xmlNodeTcBOM = pXMLRootElem->appendChild(xmlNodeTcBOM);

		// Export Properties
		exportProperties(pXMLDoc, xmlNodeTcProps, tSelectedItemRevision, "tc_property", "Parent");

		// Export Relations
		vector<string> vecRelationTypes;
		vecRelationTypes.push_back("IMAN_specification");
		vecRelationTypes.push_back("IMAN_reference");
		vecRelationTypes.push_back("BT7_Rel_Specified");
		vecRelationTypes.push_back("BT7_Rel_Concerned");
		vecRelationTypes.push_back("BT7_SUPPLIERSPEC");

		// Traverse Relation Types
		for ( int iIdx2 = 0; iIdx2 < vecRelationTypes.size( ); iIdx2++ )
		{
			tag_t  tRelType           = NULLTAG;
			int    iRelCount          = 0;
			tag_t *taSecondaryObjects = NULL;

			// Find Relation Type
			ITK(GRM_find_relation_type(vecRelationTypes.at(iIdx2).c_str( ), &tRelType) );
			ITK(GRM_list_secondary_objects_only(tSelectedItemRevision, tRelType, &iRelCount, &taSecondaryObjects) );

			// Tranverse Secondary Objects
			for ( int iIdx3 = 0; iIdx3 < iRelCount; iIdx3++ )
			{
				stringstream ssIdentifier;
				char         secondaryName[WSO_name_size_c + 1];

				// Builde Identifier String
				ssIdentifier << vecRelationTypes.at(iIdx2) << "_" << getInternalIdOfTcObject(taSecondaryObjects[iIdx3]);

				// Get Name
				ITK(WSOM_ask_name(taSecondaryObjects[iIdx3], secondaryName) );

				// Create XML-Tag for the current BOM Line
				MSXML2::IXMLDOMElementPtr xmlNodeTcRelation = pXMLDoc->createElement(_T("tc_parent_object_relation") );
				xmlNodeTcRelation->setAttribute(_T("uid"), _T(ssIdentifier.str( ).c_str( ) ) );
				xmlNodeTcRelation->setAttribute(_T("RelationType"), _T(vecRelationTypes.at(iIdx2).c_str( ) ) );
				xmlNodeTcRelation->setAttribute(_T("SecondaryObjectName"), _T(secondaryName) );
				xmlNodeTcRelation = xmlNodeTcRelations->appendChild(xmlNodeTcRelation);
			}

			// Free
			SAFE_SM_FREE(taSecondaryObjects);
		}

		// Export BOM
		getAllBOMLines(tSelectedItemRevision, &iChildLineCount, &taChildLines);

		for ( int iIdx2 = 0; iIdx2 < iChildLineCount; iIdx2++ )
		{
			char *       sChildFindNo      = NULL;
			char *       sChildItemId      = NULL;
			char *       sChildItemRevId   = NULL;
			char *       sChildInternalUID = NULL;
			char *       sChildRefDes      = NULL;
			stringstream ssIdentifier;

			// Build Identifier String
			ITK(AOM_ask_value_string(taChildLines[iIdx2], "bl_occurrence_uid", &sChildInternalUID) );
			ITK(AOM_ask_value_string(taChildLines[iIdx2], "bl_rev_item_id", &sChildItemId) );
			ITK(AOM_ask_value_string(taChildLines[iIdx2], "bl_rev_item_revision_id", &sChildItemRevId) );

			ITK(AOM_ask_value_string(taChildLines[iIdx2], "bl_sequence_no", &sChildFindNo) );
			ITK(AOM_ask_value_string(taChildLines[iIdx2], "bl_occ_ref_designator", &sChildRefDes) );

			ssIdentifier << sChildItemId << "_" << sChildItemRevId << "_" << sChildInternalUID;

			// Create XML-Tag for the current BOM Line
			MSXML2::IXMLDOMElementPtr xmlNodeTcBOMLine = pXMLDoc->createElement(_T("tc_bom_line") );
			xmlNodeTcBOMLine->setAttribute(_T("uid"), _T(ssIdentifier.str( ).c_str( ) ) );
			xmlNodeTcBOMLine->setAttribute(_T("seq_no"), _T(sChildFindNo) );
			xmlNodeTcBOMLine->setAttribute(_T("ref_des"), _T(sChildRefDes) );
			xmlNodeTcBOMLine = xmlNodeTcBOM->appendChild(xmlNodeTcBOMLine);

			// Free
			SAFE_SM_FREE(sChildFindNo);
			SAFE_SM_FREE(sChildItemId);
			SAFE_SM_FREE(sChildItemRevId);
			SAFE_SM_FREE(sChildInternalUID);

			// Export BOM Line Properties
			exportProperties(pXMLDoc, xmlNodeTcBOMLine, taChildLines[iIdx2], "tc_bom_line_property", "BomLine");
		}

		// Free
		SAFE_SM_FREE(taChildLines);

		// Create temporary location on the staging location
		char *       directory = NULL;
		stringstream ssFilepath;
		ITK(OBJIO__make_temp_staging_dir(&directory) );

		ssFilepath << directory << "/" << "tc_meta_data_dump.xml";

		// Save XML
		pXMLDoc->save(ssFilepath.str( ).c_str( ) );

		// Generate Dataset Name
		time_t     theTime = time(NULL);
		struct tm *aTime   = localtime(&theTime);
		int        day     = aTime->tm_mday;
		int        month   = aTime->tm_mon + 1;                                                                                                          // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
		int        year    = aTime->tm_year + 1900;                                                                                                      // Year is # years since 1900
		int        hour    = aTime->tm_hour;
		int        min     = aTime->tm_min;
		int        sec     = aTime->tm_sec;

		std::stringstream ssDatasetName;
		ssDatasetName << "TcMetaDataDump (" << year << "-" << ( ( month < 10 ) ? "0" : "" ) << month << "-" << ( ( day < 10 ) ? "0" : "" ) << day << " " << ( ( hour < 10 ) ? "0" : "" ) << hour << ":" << ( ( min < 10 ) ? "0" : "" ) << min << ":" << ( ( sec < 10 ) ? "0" : "" ) << sec << ")";

		// Create ImanFile
		tag_t      tFileTag;
		IMF_file_t fileDescriptor;
		ITK(IMF_import_file(ssFilepath.str( ).c_str( ), NULL, SS_TEXT, &tFileTag, &fileDescriptor) );
		ITK(AOM_save(tFileTag) );
		ITK(AOM_refresh(tFileTag, FALSE) );

		// Attach file to a new Dataset and attach the Dataset to the current workflow-process as
		tag_t tNewDataset = createDataset(msg.task, tFileTag, ssDatasetName.str( ).c_str( ) );

		// Set status of the Dataset
		tag_t tStatus;
		char	*sRealStatusName				= NULL;
		tag_t taReleaseObjects[1];
		taReleaseObjects[0] = tNewDataset;

		// Get the real name of status before creating it.
		returnStatusRealName(STATUS_TYPE_60,&sRealStatusName);


		ITK(CR_create_release_status(sRealStatusName, &tStatus) );
		ITK(AOM_save(tStatus) );
		ITK(EPM_add_release_status(tStatus, 1, taReleaseObjects, false) );

		// Delete temprary staging location
		ITK(OBJIO_delete_staging_dir(directory) );

		SAFE_SM_FREE(directory);
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	// MSXML
	CoUninitialize( );

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_elec3d_export_meta_data */


/**
 * @brief
 * If WF Target has no status -> no action on WF Target
 * If WF Target has status 10 -> no action on WF Target
 * If WF Target has status 60 -> create new Minor Revision (e.g.: B -> B1, A2->A3), set Status 10 on new created Minor Revision, replace WF Target by new created Minor Revision as new WF Target
 */
int SBT_AH_elec3d_newMinorRevisionIfRequired(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_elec3d_newMinorRevisionIfRequired";
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

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tSelectedPartRevision = NULLTAG;
		tag_t tClassTag             = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "Part_0_Revision_alt", &tClassTag) )
		{
			tSelectedPartRevision = pTargetTags[iIdx1];
		}

		/* Target Object identified? */
		if ( tSelectedPartRevision != NULLTAG )
		{
			// Get Part Type
			char *sPartType = NULL;
			ITK(AOM_ask_value_string(tSelectedPartRevision, "bt7_parttype", &sPartType) );

			// Only continue creation of minor revision if bt7_parttype = "pt_ekk" || "I7LGCodPEq" || "pt_pcb"
			if ( ( strcmp(sPartType, "pt_ekk") == 0 ) || ( strcmp(sPartType, "l7LGCodPEq") == 0 ) || ( strcmp(sPartType, "pt_pcb") == 0 ) 
				// Fix for #0SBT-PR-01PDM4BT| SBTPRPDMBT-95 BTQ00285075 3D Workflow mech. Components:: now allows for Mechanical Component check
				||( strcmp(sPartType, BT7_LOV_MECH_COMP) == 0 ) 
					)
			{
				int     iStatusCount      = 0;
				char *  sLatestStatusName = NULL;
				char	*sRealStatusName				= NULL;
				// Get latest release stastus
				askLatestReleaseStatus(tSelectedPartRevision, &sLatestStatusName, &iStatusCount);

				if ( iStatusCount == 0 )
				{
					// No Action Required
				} else if ( strcmp(sLatestStatusName, STATUS_TYPE_10) == 0 ) {
					// No Action Required
				} else if ( strcmp(sLatestStatusName, STATUS_TYPE_60) == 0 ) {
					// New Minor Revision
					char  rev_id[ITEM_id_size_c + 1];
					tag_t tNewRevision  = NULLTAG;
					tag_t tNewStatus_5 = NULLTAG;
					tag_t taReleaseObjects[1];

					ITK(ITEM_ask_rev_id(tSelectedPartRevision, rev_id) );

					// Issue 1087. 3D-Workflow will create a minro-revision automatically.
					// Handle major Rev "AA" and minor Rev "01".
					if ( strlen(rev_id) == 1 || (strlen(rev_id) == 2 && isalpha(rev_id[0]) && isalpha(rev_id[1])) )
					{
						tag_t tItem		= NULLTAG;
						tag_t tMinorRev = NULLTAG;

						// Current Revision is a major revision -> attach a '1' to the Rev-ID
						string strNewRevId = rev_id;
						strNewRevId.append("01");

						// Issue 1133. 3D-Workflow Issue. If A01 already exists, error occurred.
						ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tItem) );
						ITK(ITEM_find_revision(tItem, strNewRevId.c_str(), &tMinorRev) );

						if ( tMinorRev == NULLTAG )
						{
							ITK(ITEM_copy_rev(tSelectedPartRevision, strNewRevId.c_str( ), &tNewRevision) );
						}
						else
						{
							// First minor revision is found -> let Teamcenter increase the ID
							ITK(ITEM_copy_rev(tSelectedPartRevision, NULL, &tNewRevision) );
						}
					}
					else
					{
						// Current Revision is a minor revision -> let Teamcenter increase the ID
						ITK(ITEM_copy_rev(tSelectedPartRevision, NULL, &tNewRevision) );
					}

					// Set Status 10 for new Revision
					taReleaseObjects[0] = tNewRevision;

					// Get the real name of status before creating it.
					returnStatusRealName(STATUS_TYPE_5,&sRealStatusName);


					ITK(CR_create_release_status(sRealStatusName, &tNewStatus_5) );
					ITK(AOM_save(tNewStatus_5) );
					ITK(EPM_add_release_status(tNewStatus_5, 1, taReleaseObjects, false) );

					// Change Workflow Target of the Sub-Workflow
					int *  iaNewAttachmentTypes = (int *) MEM_alloc(1 * sizeof( int ) );
					tag_t *taNewTargets         = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );
					tag_t *taOldTargets         = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );

					iaNewAttachmentTypes[0] = EPM_target_attachment;
					taNewTargets[0]         = tNewRevision;
					taOldTargets[0]         = tSelectedPartRevision;

					ITK(EPM_remove_attachments(rootTaskTag, 1, taOldTargets) );
					ITK(EPM_add_attachments(rootTaskTag, 1, taNewTargets, iaNewAttachmentTypes) );
					ITK(AOM_lock(rootTaskTag));//Fix for Issue SBTPRPDMBT-95:to Fix error msg "The instance is not locked".
					ITK(AOM_save_without_extensions(rootTaskTag) );

					// Free
					SAFE_SM_FREE(iaNewAttachmentTypes);
					SAFE_SM_FREE(taNewTargets);
				}
				else
				{
					printf(" > WARNING: Status '%s' not handled by the system - no action performed by the handler 'SBT_AH_elec3d_newMinorRevisionIfRequired' \n", sLatestStatusName);
				}

				//Free
				SAFE_SM_FREE(sLatestStatusName);
			}

			SAFE_SM_FREE(sPartType);
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_elec3d_newMinorRevisionIfRequired */


/**
 * @brief
 * Assigns the responsible MCAD Designer based on the following rule:
 * - If process owner is a member of the CCR-Designers -> Use the Ressource Pool "3D_EL-CAD_Design"
 * - Otherwise assign the task to the process owner (who will be a MCAD Designer)
 */
int SBT_AH_elec3d_assignMcadDesigner(EPM_action_message_t msg)
{
	char        sWorkflowName[WSO_name_size_c + 1];
	const char *theFunction = "SBT_AH_elec3d_assignMcadDesigner";
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

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		tag_t tSelectedItemRevision = NULLTAG;
		tag_t tClassTag             = NULLTAG;

		if ( object_is_of_class_or_desc(pTargetTags[iIdx1], "ItemRevision", &tClassTag) )
		{
			tSelectedItemRevision = pTargetTags[iIdx1];
		}

		/* Target Object identified? */
		if ( tSelectedItemRevision != NULLTAG )
		{
			tag_t  t3dGroupTag       = NULLTAG;
			tag_t  tParticipantType  = NULLTAG;
			tag_t  tResponsibleParty = NULLTAG;
			tag_t *taParticipantList = NULL;
			tag_t *taRoles           = NULL;
			tag_t  t3dRoleTag        = NULLTAG;
			tag_t  t3dRessourcepool  = NULLTAG;
			int    iParticipantCount = 0;
			int    iRoleCount        = 0;
			tag_t  tCurrentGroupTag  = NULLTAG;
			char * sCurrentGroupName = NULL;

			// Ask current group
			ITK(POM_ask_group(&sCurrentGroupName, &tCurrentGroupTag) );

			// If current group is not equal to "Common Component Engineering"
			if ( strcmp(sCurrentGroupName, "Common Component Engineering") == 0 || strcmp(sCurrentGroupName, "110_PCBDesign_A") == 0 )
			{
				ITK(POM_ask_group_tag("3D_EL-CAD_Design", &t3dGroupTag) );
				ITK(SA_ask_roles_from_group(t3dGroupTag, &iRoleCount, &taRoles) );

				for ( int iIdx2 = 0; iIdx2 < iRoleCount; iIdx2++ )
				{
					char sRoleName[SA_name_size_c + 1];
					ITK(SA_ask_role_name(taRoles[iIdx2], sRoleName) );

					if ( strcmp(sRoleName, "Designer") == 0 )
					{
						t3dRoleTag = taRoles[iIdx2];
						break;
					}
				}

				if ( t3dRoleTag == NULLTAG )
				{
					ITK(SA_ask_group_default_role(t3dGroupTag, &t3dRoleTag) );
				}

				ITK(EPM_get_resource_pool(t3dGroupTag, t3dRoleTag, false, &t3dRessourcepool) );

				SAFE_SM_FREE(taRoles);
			}
			else
			{
				// Current Group != "Common Component Engineering"
				ITK(SA_ask_current_groupmember(&t3dRessourcepool) );
			}

			// Ask current Responsbile Party
			ITK(EPM_get_participanttype("ProposedResponsibleParty",
										&tParticipantType
										) );
			ITK(ITEM_rev_ask_participants(tSelectedItemRevision,
										  tParticipantType,
										  &iParticipantCount,
										  &taParticipantList
										  ) );

			// Remove current Responsible Party (if required)
			if ( iParticipantCount > 0 )
			{
				// Assumption is only one change specialist always
				ITK(AOM_refresh(tSelectedItemRevision, TRUE) );
				ITK(ITEM_rev_remove_participant(tSelectedItemRevision, taParticipantList[0]) );
				ITK(AOM_save(tSelectedItemRevision) );
				ITK(AOM_refresh(tSelectedItemRevision, FALSE) );
			}

			// Set Proposed Responsible Party
			ITK(EPM_create_participant(t3dRessourcepool, tParticipantType, &tResponsibleParty) );
			ITK(AOM_refresh(tSelectedItemRevision, TRUE) );
			ITK(ITEM_rev_add_participant(tSelectedItemRevision, tResponsibleParty) );
			ITK(AOM_save(tSelectedItemRevision) );
			ITK(AOM_refresh(tSelectedItemRevision, FALSE) );

			// Free
			SAFE_SM_FREE(taParticipantList);
			SAFE_SM_FREE(sCurrentGroupName);
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_elec3d_assignMcadDesigner */


logical compareXML(const char *sFileLoc_old, const char *sFileLoc_new, stringstream *txtStringStream)
{
	logical lCompareOk = true;

	// Read XML-Files
	META_DATA_ROOT metaDataFile_old = readXMLFile(sFileLoc_old);
	META_DATA_ROOT metaDataFile_new = readXMLFile(sFileLoc_new);

	// Validate XML-Structure (XML must contain Properties, BOM and Relations are optional)
	if ( ( metaDataFile_old.vecParentProperties.size( ) == 0 ) || ( metaDataFile_new.vecParentProperties.size( ) == 0 ) )
	{
		( *txtStringStream ) << "ERROR - Attached Datasets does not have the expected format" << endl;
		lCompareOk = false;
		return lCompareOk;
	}

	// Traverse File_1 Properties
	for ( int iIdx1 = 0; iIdx1 < metaDataFile_old.vecParentProperties.size( ); iIdx1++ )
	{
		META_DATA_PROPERTY prop_1     = metaDataFile_old.vecParentProperties.at(iIdx1);
		logical            lPropFound = false;

		// Traverse File_2 Properties
		for ( int iIdx2 = 0; iIdx2 < metaDataFile_new.vecParentProperties.size( ); iIdx2++ )
		{
			META_DATA_PROPERTY prop_2 = metaDataFile_new.vecParentProperties.at(iIdx2);

			if ( prop_1.sPropName.compare(prop_2.sPropName) == 0 )
			{
				lPropFound = true;

				// Identical Property found
				if ( prop_1.sPropValue.compare(prop_2.sPropValue) != 0 )
				{
					lCompareOk = false;
					( *txtStringStream ) << " > Property '" << prop_1.sPropName << "' has been changed ('" << prop_1.sPropValue << "' -> '" << prop_2.sPropValue << "')" << endl;
				}

				break;
			}
		}

		// Property not found
		if ( lPropFound == false )
		{
			lCompareOk = false;
			( *txtStringStream ) << " > Property '" << prop_1.sPropName << "' not found in the 2nd file" << endl;
		}
	}

	// Traverse File_1 Relations
	for ( int iIdx1 = 0; iIdx1 < metaDataFile_old.vecParentRelations.size( ); iIdx1++ )
	{
		META_DATA_RELATION rel_1     = metaDataFile_old.vecParentRelations.at(iIdx1);
		logical            lRelFound = false;

		// Traverse File_2 Relations
		for ( int iIdx2 = 0; iIdx2 < metaDataFile_new.vecParentRelations.size( ); iIdx2++ )
		{
			META_DATA_RELATION rel_2 = metaDataFile_new.vecParentRelations.at(iIdx2);

			if ( rel_1.sRelId.compare(rel_2.sRelId) == 0 )
			{
				lRelFound = true;
			}
		}

		// Relation not found
		if ( lRelFound == false )
		{
			lCompareOk = false;
			( *txtStringStream ) << " > Relation '" << rel_1.sRelType << "' to object '" << rel_1.sRelSecondaryObjectName << "' has been removed" << endl;
		}
	}

	// Traverse File_2 Relations
	for ( int iIdx2 = 0; iIdx2 < metaDataFile_new.vecParentRelations.size( ); iIdx2++ )
	{
		META_DATA_RELATION rel_2     = metaDataFile_new.vecParentRelations.at(iIdx2);
		logical            lRelFound = false;

		// Traverse File_2 Relations
		for ( int iIdx1 = 0; iIdx1 < metaDataFile_old.vecParentRelations.size( ); iIdx1++ )
		{
			META_DATA_RELATION rel_1 = metaDataFile_old.vecParentRelations.at(iIdx1);

			if ( rel_2.sRelId.compare(rel_1.sRelId) == 0 )
			{
				lRelFound = true;
			}
		}

		// Relation not found
		if ( lRelFound == false )
		{
			lCompareOk = false;
			( *txtStringStream ) << " > Relation '" << rel_2.sRelType << "' to object '" << rel_2.sRelSecondaryObjectName << "' has been added" << endl;
		}
	}

	// Traverse File_1 BOM-Lines
	for ( int iIdx1 = 0; iIdx1 < metaDataFile_old.vecParentBomLines.size( ); iIdx1++ )
	{
		META_DATA_BOMLINE bomline_1     = metaDataFile_old.vecParentBomLines.at(iIdx1);
		logical           lBomLineFound = false;

		// Traverse File_2 BOM-Lines
		for ( int iIdx2 = 0; iIdx2 < metaDataFile_new.vecParentBomLines.size( ); iIdx2++ )
		{
			META_DATA_BOMLINE bomline_2 = metaDataFile_new.vecParentBomLines.at(iIdx2);

			if ( bomline_1.sBomLineUID.compare(bomline_2.sBomLineUID) == 0 )
			{
				lBomLineFound = true;

				// Identical BOM-Line found - start traversing properties

				// Traverse BOM_1 Properties
				for ( int iIdx3 = 0; iIdx3 < bomline_1.vecBomLineProperties.size( ); iIdx3++ )
				{
					META_DATA_PROPERTY bomprop_1     = bomline_1.vecBomLineProperties.at(iIdx3);
					logical            lBomPropFound = false;

					// Traverse BOM_2 Properties
					for ( int iIdx4 = 0; iIdx4 < bomline_2.vecBomLineProperties.size( ); iIdx4++ )
					{
						META_DATA_PROPERTY bomprop_2 = bomline_2.vecBomLineProperties.at(iIdx4);

						if ( bomprop_1.sPropName.compare(bomprop_2.sPropName) == 0 )
						{
							lBomPropFound = true;

							// Identical Property found
							if ( bomprop_1.sPropValue.compare(bomprop_2.sPropValue) != 0 )
							{
								lCompareOk = false;
								( *txtStringStream ) << " > BOMLine-Property '" << bomprop_1.sPropName << "' has been changed ('" << bomprop_1.sPropValue << "' -> '" << bomprop_2.sPropValue << "')" << endl;
							}

							break;
						}
					}

					// Property not found
					if ( lBomPropFound == false )
					{
						lCompareOk = false;
						( *txtStringStream ) << " > BOMLine-Property " << bomprop_1.sPropName << " not found in 2nd file" << endl;
					}
				}

				break;
			}
		}

		// BOM Line not found
		if ( lBomLineFound == false )
		{
			lCompareOk = false;
			( *txtStringStream ) << " > BOMLine '" << bomline_1.sBomLineUID << "' has been removed (SeqNo.: '" << bomline_1.sBomLineSeqNo << "' RefDes.: '" << bomline_1.sBomLineRefDes << "')" << endl;
		}
	}

	// Traverse File_2 BOM-Lines
	for ( int iIdx2 = 0; iIdx2 < metaDataFile_new.vecParentBomLines.size( ); iIdx2++ )
	{
		META_DATA_BOMLINE bomline_2     = metaDataFile_new.vecParentBomLines.at(iIdx2);
		logical           lBomLineFound = false;

		// Traverse File_1 BOM-Lines
		for ( int iIdx1 = 0; iIdx1 < metaDataFile_old.vecParentBomLines.size( ); iIdx1++ )
		{
			META_DATA_BOMLINE bomline_1 = metaDataFile_old.vecParentBomLines.at(iIdx1);

			if ( bomline_2.sBomLineUID.compare(bomline_1.sBomLineUID) == 0 )
			{
				lBomLineFound = true;

				break;
			}
		}

		// BOM Line not found
		if ( lBomLineFound == false )
		{
			lCompareOk = false;
			( *txtStringStream ) << " > BOMLine '" << bomline_2.sBomLineUID << "' has been added (SeqNo.: '" << bomline_2.sBomLineSeqNo << "' RefDes.: '" << bomline_2.sBomLineRefDes << "')" << endl;
		}
	}

	return lCompareOk;
}


META_DATA_ROOT readXMLFile(const char *sPathToXML)
{
	META_DATA_ROOT meta_data_root;
	HRESULT        hr = CoInitialize(NULL);

	if ( SUCCEEDED(hr) )
	{
		MSXML2::IXMLDOMDocument2Ptr xmlDoc;

		//hr = xmlDoc.CreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
		xmlDoc.CreateInstance(__uuidof(DOMDocument30) );

		if ( xmlDoc->load(_T(sPathToXML) ) != VARIANT_TRUE )
		{
			printf(" > Unable to load '%s' \n", sPathToXML);
		}
		else
		{
			printf(" > XML successfully loaded\n");

			// Get all instances of "ClassAttribConfig"
			MSXML2::IXMLDOMNodeListPtr pNodeRoots = xmlDoc->getElementsByTagName("tc_meta_data_dump");

			if ( pNodeRoots->Getlength( ) == 1 )
			{
				vector<META_DATA_PROPERTY> vecTmpParentProperties;
				vector<META_DATA_RELATION> vecTmpParentRelations;
				vector<META_DATA_BOMLINE>  vecTmpParentBomLines;

				MSXML2::IXMLDOMNodePtr pNodeRoot = pNodeRoots->Getitem(0);

				// Traverse child nodes of "ClassAttribConfig"
				MSXML2::IXMLDOMNodeListPtr pNode1stLevelChilds = pNodeRoot->GetchildNodes( );

				for ( int iIdx1 = 0; iIdx1 < pNode1stLevelChilds->Getlength( ); iIdx1++ )
				{
					MSXML2::IXMLDOMNodePtr pNode1stLevelChild = pNode1stLevelChilds->Getitem(iIdx1);

					_variant_t  nodeName  = pNode1stLevelChild->GetnodeName( );
					std::string sNodeName = from_variant(nodeName);

					if ( sNodeName.compare("tc_parent_object_properties") == 0 )
					{
						// Read and traverse 2nd level childs
						MSXML2::IXMLDOMNodeListPtr pNode2ndLevelChilds = pNode1stLevelChild->GetchildNodes( );

						for ( int iIdx2 = 0; iIdx2 < pNode2ndLevelChilds->Getlength( ); iIdx2++ )
						{
							META_DATA_PROPERTY tmpProperty;

							// Read values from XML
							tmpProperty.sPropName  = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "name");
							tmpProperty.sPropValue = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "value");

							// Append to Vector
							vecTmpParentProperties.push_back(tmpProperty);
						}
					}
					else if ( sNodeName.compare("tc_parent_object_relations") == 0 )
					{
						// Read and traverse 2nd level childs
						MSXML2::IXMLDOMNodeListPtr pNode2ndLevelChilds = pNode1stLevelChild->GetchildNodes( );

						for ( int iIdx2 = 0; iIdx2 < pNode2ndLevelChilds->Getlength( ); iIdx2++ )
						{
							META_DATA_RELATION         tmpRelation;
							vector<META_DATA_PROPERTY> vecTmp3rdLevelProperties;

							// Read values from XML
							tmpRelation.sRelId   = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "uid");
							tmpRelation.sRelType = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "RelationType");
							tmpRelation.sRelSecondaryObjectName = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "SecondaryObjectName");

							// Append to Vector
							vecTmpParentRelations.push_back(tmpRelation);
						}
					}
					else if ( sNodeName.compare("tc_bom_lines") == 0 )
					{
						// Read and traverse 2nd level childs
						MSXML2::IXMLDOMNodeListPtr pNode2ndLevelChilds = pNode1stLevelChild->GetchildNodes( );

						for ( int iIdx2 = 0; iIdx2 < pNode2ndLevelChilds->Getlength( ); iIdx2++ )
						{
							META_DATA_BOMLINE          tmpBomLine;
							vector<META_DATA_PROPERTY> vecTmp3rdLevelProperties;

							// Read values from XML
							tmpBomLine.sBomLineUID    = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "uid");
							tmpBomLine.sBomLineSeqNo  = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "seq_no");
							tmpBomLine.sBomLineRefDes = getXMLNodeValue(pNode2ndLevelChilds->Getitem(iIdx2), "ref_des");

							// Read and traverse 3rd level childs
							MSXML2::IXMLDOMNodeListPtr pNode3rdLevelChilds = pNode2ndLevelChilds->Getitem(iIdx2)->GetchildNodes( );

							for ( int iIdx3 = 0; iIdx3 < pNode3rdLevelChilds->Getlength( ); iIdx3++ )
							{
								META_DATA_PROPERTY tmpProperty;

								// Read values from XML
								tmpProperty.sPropName  = getXMLNodeValue(pNode3rdLevelChilds->Getitem(iIdx3), "name");
								tmpProperty.sPropValue = getXMLNodeValue(pNode3rdLevelChilds->Getitem(iIdx3), "value");

								// Append to Vector
								vecTmp3rdLevelProperties.push_back(tmpProperty);
							}

							tmpBomLine.vecBomLineProperties = vecTmp3rdLevelProperties;

							// Append to Vector
							vecTmpParentBomLines.push_back(tmpBomLine);
						}
					}
					else
					{
						printf("ERROR: Invalid XML-Entry found: '%s' \n", sNodeName.c_str( ) );
					}
				}

				meta_data_root.vecParentProperties = vecTmpParentProperties;
				meta_data_root.vecParentRelations  = vecTmpParentRelations;
				meta_data_root.vecParentBomLines   = vecTmpParentBomLines;
			}
		}
	}

	return meta_data_root;
}


string getXMLNodeValue(MSXML2::IXMLDOMNodePtr pNode, char *sAttrName)
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


int exportProperties(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr xmlParentNode, tag_t tObject, const char *sXmlTagName, const char *sModus)
{
	int    iPropCount  = 0;
	char **saPropNames = NULL;

	// Get Property Names
	ITK(AOM_ask_prop_names(tObject, &iPropCount, &saPropNames) );

	// Traverse Property Names
	for ( int iIdx1 = 0; iIdx1 < iPropCount; iIdx1++ )
	{
		PROP_type_t  proptype;
		char *       proptype_n = NULL;
		stringstream ssPropertyValue;
		logical      lIncludeIntoXML = false;

		// Property applicable ?
		{
			vector<string> vecProperties;
			logical        lValidProperty = false;

			if ( strcmp(sModus, "Parent") == 0 )
			{
				vecProperties = vecValidProperties_Parent;
			}
			else if ( strcmp(sModus, "BomLine") == 0 )
			{
				vecProperties = vecValidProperties_BomLine;
			}
			else
			{
				continue;
			}

			for ( int iIdx2 = 0; iIdx2 < vecProperties.size( ); iIdx2++ )
			{
				if ( vecProperties.at(iIdx2).compare(saPropNames[iIdx1]) == 0 )
				{
					lValidProperty = true;
					break;
				}
			}

			// Go / No Go
			if ( lValidProperty == false )
			{
				continue;
			}
		}

		// Get Property Type
		ITK(AOM_ask_property_type(tObject, saPropNames[iIdx1], &proptype, &proptype_n) );

		if ( ( proptype == PROP_attribute ) || ( proptype == PROP_compound ) || ( proptype == PROP_runtime ) )
		{
			PROP_value_type_t valtype;
			char *            valtype_n = NULL;

			// Get value type
			ITK(AOM_ask_value_type(tObject, saPropNames[iIdx1], &valtype, &valtype_n) );

			// Get value based on type
			if ( valtype == PROP_date )
			{
				date_t tmpDate;
				char * sDate = NULL;

				// Get Date Value and transform it to a string
				ITK(AOM_ask_value_date(tObject, saPropNames[iIdx1], &tmpDate) );
				ITK(DATE_date_to_string(tmpDate, "%Y-%b-%d %H:%M:%S", &sDate) );
				ssPropertyValue << sDate;

				// Free
				SAFE_SM_FREE(sDate);

				lIncludeIntoXML = true;
			}
			else if ( ( valtype == PROP_double ) || ( valtype == PROP_float ) )
			{
				double dValue = 0.0;

				ITK(AOM_ask_value_double(tObject, saPropNames[iIdx1], &dValue) );
				ssPropertyValue << dValue;

				lIncludeIntoXML = true;
			}
			else if ( valtype == PROP_int )
			{
				int iValue = 0;

				ITK(AOM_ask_value_int(tObject, saPropNames[iIdx1], &iValue) );
				ssPropertyValue << iValue;

				lIncludeIntoXML = true;
			}
			else if ( valtype == PROP_logical )
			{
				logical lValue = false;

				ITK(AOM_ask_value_logical(tObject, saPropNames[iIdx1], &lValue) );
				ssPropertyValue << lValue;

				lIncludeIntoXML = true;
			}
			else if ( valtype == PROP_string )
			{
				char *sValue = "";

				ITK(AOM_ask_value_string(tObject, saPropNames[iIdx1], &sValue) );
				ssPropertyValue << sValue;

				lIncludeIntoXML = true;
			}

			// Free
			SAFE_SM_FREE(valtype_n);
		}                                                                                                                 // end "if(proptype == PROP_attribute)"


		// Write value into XML-File
		if ( lIncludeIntoXML == true )
		{
			putStringPropertyToXML(pXMLDoc, xmlParentNode, saPropNames[iIdx1], ssPropertyValue.str( ).c_str( ), sXmlTagName);
		}

		// Free
		SAFE_SM_FREE(proptype_n);
	}

	// Free
	SAFE_SM_FREE(saPropNames);

	return 0;
}


int putStringPropertyToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, const char *propTcName, const char *propTcValue, const char *xmlTagName)
{
	// Create XML-Field
	MSXML2::IXMLDOMElementPtr fieldEntry = pXMLDoc->createElement(_T(xmlTagName) );

	fieldEntry->setAttribute(_T("name"), _T(propTcName) );
	fieldEntry->setAttribute(_T("value"), _T(propTcValue) );
	fieldEntry = pParentXMLTag->appendChild(fieldEntry);

	fieldEntry = NULL;

	return 0;
}


void getAllBOMLines(tag_t tRevTag, int *childBOMLinesCount, tag_t **childBOMLines)
{
	// Init traversal of BOM
	tag_t windowTag  = NULLTAG;
	tag_t ruleTag    = NULLTAG;
	tag_t topLineTag = NULLTAG;

	ITK(CFM_find("latest working", &ruleTag) );

	ITK(BOM_create_window(&windowTag) );
	ITK(BOM_set_window_config_rule(windowTag, ruleTag) );
	ITK(BOM_set_window_top_line(windowTag, NULLTAG, tRevTag, NULLTAG, &topLineTag) );
	ITK(BOM_set_window_pack_all(windowTag, true) );
	ITK(BOM_line_ask_child_lines(topLineTag, childBOMLinesCount, childBOMLines) );
}
