#import <MSXML6.dll>

#include <ctype.h>
#include <atlbase.h>
#include <msxml.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <time.h>
#include <sstream>
#include <itk/mem.h>
#include <unidefs.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <sa/tcfile.h>
#include <sa/user.h>
#include <string.h>
#include <fclasses/tc_string.h>
#include <fclasses/tc_date.h>
#include <tc/emh.h>
#include <tccore/aom.h>
#include <tccore/project.h>
#include <tccore/aom_prop.h>
#include <property/nr.h>
#include <qry/qry.h>
#include <tccore/grm.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tccore/uom.h>

// --------------------------------------------------------------------------

#define ITK_CALL(x){														\
    int stat;\
    char *err_string;\
    if((stat =(x))!= ITK_ok)												\
    {																		\
    EMH_get_error_string(NULLTAG, stat, &err_string);\
    printf(" > ERROR: %d ERROR MSG: %s.\n", stat, err_string);\
    printf(" >  FUNCTION: %s\nFILE: %s LINE: %d\n", #x, __FILE__, __LINE__);\
    if(err_string)MEM_free(err_string);\
    exit(EXIT_FAILURE);\
    }																		\
}

// --------------------------------------------------------------------------

int generateReport(tag_t tResult, char *file_path);
int putStringValueToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *propXMLName, const char *sValue);
int putStringPropertyToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *propXMLName, char *propTcName);
int putLogicalPropertyToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *propXMLName, char *propTcName);
int putRelationToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *sRelationTcName, char *sRelationXMLName, std::vector<tag_t> *vecUsedRevisionsForReport);
void getAllBOMLines(tag_t tRevTag, int *childRevCount, tag_t **childRevs, logical lPacked);
logical isDerivedFromClass(char *className, tag_t tObjectTag);
int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount, tag_t *tLatestStatusTag);
int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount);
int CLOCK_compare_dates(const date_t *date1, const date_t *date2);
bool sortPartNum (tag_t tRev1, tag_t tRev2);

// --------------------------------------------------------------------------

using namespace std;
using namespace MSXML2;

char		*item_id			= NULL;
char		*item_rev_id		= NULL;
char		*revision_rule		= NULL;
const char	*doc_revision_rule	= NULL;
char		*sRequestor			= NULL;
int			iMaxBomLevel		= 0;
char		*sReportType		= NULL;
char		*sOrdering			= NULL;
char		*sIncludePCB		= NULL;
char		*sIncludeSUP		= NULL;
char		*sOnlySMD			= NULL;

const char	*sLATEST_WORKING	= "Latest Working";
const char	*sLATEST_RELEASED	= "Latest Released";
logical 	isMDR				= false;
logical 	isPCBR				= false;
logical 	isEQPR				= false;
logical 	isEQPER				= false;

logical		lDebug				= false;

int ITK_user_main(int argc , char* argv[]) {
	logical	lInputError			= false;
	logical	lUseInputFile		= false;
    int		retcode				= ITK_ok;
    char	*user_id			= NULL;
    char	*user_passwd		= NULL;
    char	*user_group			= NULL;
	char	*file_path			= NULL;
	char	*max_bom_level		= NULL;
	char	*debug				= NULL;

    /* Gets the user ID */
    user_id = ITK_ask_cli_argument("-u=");

    /* Gets the user Password */
    user_passwd = ITK_ask_cli_argument("-p=");

    /* Gets the user group */
    user_group = ITK_ask_cli_argument("-g=");

    /* Gets the item_id */
    item_id = ITK_ask_cli_argument("-i=");

    /* Gets the item_rev_id */
    item_rev_id = ITK_ask_cli_argument("-r=");

    /* Gets the revision_rule */
    revision_rule = ITK_ask_cli_argument("-rr=");
	
	/* Gets the file-path */
    file_path = ITK_ask_cli_argument("-f=");

	/* Gets the max. BOM Level */
    max_bom_level = ITK_ask_cli_argument("-ml=");

	/* Gets the requestor*/
	sRequestor = ITK_ask_cli_argument("-req=");

	/* Gets the sReportType*/
	sReportType = ITK_ask_cli_argument("-rt=");

	/* Gets the sOrdering*/
	sOrdering = ITK_ask_cli_argument("-order=");

	/* Gets the sIncludePCB*/
	sIncludePCB = ITK_ask_cli_argument("-pcb=");

	/* Gets the sIncludeSUP*/
	sIncludeSUP = ITK_ask_cli_argument("-sup=");

	/* Gets the sOnlySMD*/
	sOnlySMD = ITK_ask_cli_argument("-smd=");

	/* Debug */
	debug = ITK_ask_cli_argument("-debug=");

	/* Validate parameters */
	if(user_id == NULL || strcmp(user_id, "") == 0){
		printf(" > ERROR: Parameter '-u' should not be empty! \n");
		lInputError = true;
	}
	if(user_passwd == NULL || strcmp(user_passwd, "") == 0){
		printf(" > ERROR: Parameter '-p' should not be empty! \n");
		lInputError = true;
	}
	if(user_group == NULL || strcmp(user_group, "") == 0){
		printf(" > ERROR: Parameter '-g' should not be empty! \n");
		lInputError = true;
	}
	if(item_id == NULL || strcmp(item_id, "") == 0){
		printf(" > ERROR: Parameter '-i' should not be empty! \n");
		lInputError = true;
	}
	if(item_rev_id == NULL || strcmp(item_rev_id, "") == 0){
		printf(" > ERROR: Parameter '-r' should not be empty! \n");
		lInputError = true;
	}
	if(revision_rule == NULL || strcmp(revision_rule, "") == 0){
		printf(" > ERROR: Parameter '-rr' should not be empty! \n");
		lInputError = true;
	}
	if(file_path == NULL || strcmp(file_path, "") == 0){
		printf(" > ERROR: Parameter '-f' should not be empty! \n");
		lInputError = true;
	}
	if(max_bom_level == NULL || strcmp(max_bom_level, "") == 0){
		printf(" > ERROR: Parameter '-ml' should not be empty! \n");
		lInputError = true;
	}
	if(sRequestor == NULL || strcmp(sRequestor, "") == 0){
		printf(" > ERROR: Parameter '-req' should not be empty! \n");
		lInputError = true;
	}
	if(sReportType == NULL || strcmp(sReportType, "") == 0){
		printf(" > ERROR: Parameter '-rt' should not be empty! \n");
		lInputError = true;
	} else {
		if(strcmp(sReportType, "MD Report") == 0){
			isMDR = true;
		} else if(strcmp(sReportType, "PCB BOM Report") == 0){
			isPCBR = true;
		} else if(strcmp(sReportType, "Equal Parts Report") == 0){
			isEQPR = true;
		} else if(strcmp(sReportType, "Equal Parts Equipment Report") == 0){
			isEQPER = true;
		} else {
			printf(" > ERROR: Parameter '-rt' invalid! Valid values : ['MD Report' | 'PCB BOM Report' | 'Equal Parts Report' | 'Equal Parts Equipment Report'] \n");
			lInputError = true;
		}
	}
	if(debug != NULL && strcmp(debug, "true") == 0){
		lDebug = true;
	}

	if(sOrdering == NULL || strcmp(sOrdering, "") == 0){
		printf(" > ERROR: Parameter '-order' should not be empty! \n");
		lInputError = true;
	}
	if(sIncludePCB == NULL || strcmp(sIncludePCB, "") == 0){
		printf(" > ERROR: Parameter '-pcb' should not be empty! \n");
		lInputError = true;
	}
	if(sIncludeSUP == NULL || strcmp(sIncludeSUP, "") == 0){
		printf(" > ERROR: Parameter '-sup' should not be empty! \n");
		lInputError = true;
	}
	if(sOnlySMD == NULL || strcmp(sOnlySMD, "") == 0){
		printf(" > ERROR: Parameter '-smd' should not be empty! \n");
		lInputError = true;
	}

	/* Exit if parameters are not correct */
	if(lInputError == true) {
		return 1;
	}

	/* Logging into Teamcenter */
	printf(" > Login to Teamenter (this might take a moment ...) \n");
	ITK_CALL ( ITK_init_module(user_id, user_passwd, user_group));
	ITK_CALL ( ITK_set_bypass(true));
	printf(" > Login successful \n");

	/* Prepare Doc-Revision Rule */
	if(strcmp(revision_rule, "BT7 For_Production") == 0) {
		doc_revision_rule = sLATEST_RELEASED;
	} else if(strcmp(revision_rule, "BT7 For_Prototype") == 0) {
		doc_revision_rule = sLATEST_WORKING;
	} else if(strcmp(revision_rule, "BT7 Working; For_Prototype") == 0) {
		doc_revision_rule = sLATEST_WORKING;
	} else if(strcmp(revision_rule, "Latest Working") == 0) {
		doc_revision_rule = sLATEST_WORKING;
	} else if(strcmp(revision_rule, "Latest by Alpha Rev Order") == 0) {
		doc_revision_rule = sLATEST_WORKING;
	} else if(strcmp(revision_rule, "Serienfreigabe") == 0) {
		doc_revision_rule = sLATEST_RELEASED;
	} else if(strcmp(revision_rule, "WF_Precise") == 0) {
		doc_revision_rule = sLATEST_RELEASED;
	} else if(strcmp(revision_rule, "WF_Serienfreigabe") == 0) {
		doc_revision_rule = sLATEST_RELEASED;
	} else if(strcmp(revision_rule, "Zuletzt erstellt") == 0) {
		doc_revision_rule = sLATEST_WORKING;
	} else if(strcmp(revision_rule, "In Produktion") == 0) {
		doc_revision_rule = sLATEST_RELEASED;
	} else if(strcmp(revision_rule, "zur Pruefung PR => SR") == 0) {
		doc_revision_rule = sLATEST_WORKING;
	} else if(strcmp(revision_rule, "BOM_Analysis") == 0) {
		doc_revision_rule = sLATEST_RELEASED;
	} else {
		printf(" > WARNING: Revision Rule '%s' not identified in mapping for doc-rev-rule! Using 'Latest Working'\n", revision_rule);
		doc_revision_rule = "Latest Working";
	}

	/* Prepare Max. BOM Level */
	if(strcmp(max_bom_level, "unlimited") == 0){
		iMaxBomLevel = 99;
	} else {
		iMaxBomLevel = atoi(max_bom_level);
	}

	/* Find Item Revision */
	tag_t tQryGeneral = NULLTAG;
	ITK_CALL ( QRY_find("Item Revision...", &tQryGeneral));

	if(tQryGeneral != NULLTAG){
		int		iResultCount	= 0;
		tag_t	*taResults		= NULL;
		char	**saQryEntries = (char**) MEM_alloc(2 * sizeof(char*));//[]	= {"ItemID", "Revision"};
		char	**saQryValues = (char**) MEM_alloc(2 * sizeof(char*));//[]	= {"CSE-000054","01"};

		saQryEntries[0] = (char*) MEM_alloc(strlen("ItemID") + 1);
		strcpy(saQryEntries[0], "Item ID" );
		saQryEntries[1] = (char*) MEM_alloc(strlen("Revision") + 1);
		strcpy(saQryEntries[1], "Revision" );

		saQryValues[0] = (char*) MEM_alloc(strlen(item_id) + 1);
		strcpy(saQryValues[0], item_id );
		saQryValues[1] = (char*) MEM_alloc(strlen(item_rev_id) + 1);
		strcpy(saQryValues[1], item_rev_id );

		ITK_CALL ( QRY_execute(tQryGeneral, 2, saQryEntries, saQryValues, &iResultCount, &taResults));
		printf(" > Result count in Teamcenter = %i \n", iResultCount);

		/* Generate Report */
		if(iResultCount == 1){
			generateReport(taResults[0], file_path);
		} else {
			printf(" > ERROR: RESULT COUNT MUST BE = 1 \n");
		}

		/* Free */
		if(taResults != NULL) MEM_free(taResults);
		if(saQryEntries != NULL) MEM_free(saQryEntries);
		if(saQryValues != NULL) MEM_free(saQryValues);
	} else {
		printf(" > ERROR: QUERY NOT FOUND! \n");
	}
	
    ITK_CALL ( ITK_exit_module(true));

    return ITK_ok;
}

int putStringValueToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *propXMLName, const char *sValue){
	if(lDebug)printf(" > ENTER putStringValueToXML \n");
	
	// Create XML-Field
	MSXML2::IXMLDOMElementPtr fieldEntry = pXMLDoc->createElement(_T("sbtrpt:Field"));
	fieldEntry->setAttribute(_T("Name"),_T(propXMLName));
	fieldEntry->Puttext(_T(sValue));  
	fieldEntry = pParentXMLTag->appendChild(fieldEntry);

	// Free
	fieldEntry = NULL;

	if(lDebug)printf(" > EXIT putStringValueToXML \n");

	return 0;
}

int putStringPropertyToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *propXMLName, char *propTcName){
	char	*sValue		= NULL;
	logical lFreeValue	= true;

	if(lDebug)printf(" > ENTER putStringPropertyToXML (propXMLName = %s ; propTcName = %s) \n", propXMLName, propTcName);

	// BT6 / BT7 differentiation
	std::string strPropName = propTcName;
	if(strPropName.substr(0, 3).compare("bt7") == 0){
		char	sObjectType[WSO_name_size_c+1];
		logical	lAttributeExists	= false;

		// Get Object Type
		ITK_CALL(WSOM_ask_object_type(tObject, sObjectType));

		if(strcmp(sObjectType, "BT7_DocumentRevision") == 0 || isDerivedFromClass("Part_0_Revision_alt", tObject) || isDerivedFromClass("Part", tObject)) {
			tag_t	tClass		= NULLTAG;
			char	*sClassName	= NULL;

			// Ask Class and Class Name
			ITK_CALL(POM_class_of_instance(tObject, &tClass));
			ITK_CALL(POM_name_of_class(tClass, &sClassName));

			// Attribute exists
			ITK_CALL(POM_attr_exists(propTcName, sClassName, &lAttributeExists));

			if(lAttributeExists == true)
			{
				ITK_CALL ( AOM_UIF_ask_value(tObject, propTcName, &sValue));

				if ( strcmp(sValue, "") == 0 ) {

					if(sValue != NULL ) MEM_free(sValue);

					sValue = "---";
					lFreeValue = false;
				}

			} else {
				printf(" > Attribute %s does not exist for class %s \n", propTcName, sObjectType);
			}

			// Free
			if(sClassName != NULL) MEM_free(sClassName);
		} else {
			sValue = "---";
			lFreeValue = false;
		}

		//// Is bt7_* property
		//char sObjectType[WSO_name_size_c+1];
		//ITK_CALL ( WSOM_ask_object_type ( tObject, sObjectType ) );
		//std::string strObjectType = sObjectType;
		//if(strObjectType.substr(0, 3).compare("BT7") == 0) {
		//	// Is BT7_* type
		//	ITK_CALL ( AOM_UIF_ask_value(tObject, propTcName, &sValue));
		//} else {
		//	sValue = "<No Value>";
		//	lFreeValue = false;
		//}
	} else {
		ITK_CALL ( AOM_UIF_ask_value(tObject, propTcName, &sValue));

		if ( strcmp(sValue, "") == 0 ) {
			sValue = "---";
			lFreeValue = false;
		}

	}

	// Create XML-Field
	MSXML2::IXMLDOMElementPtr fieldEntry = pXMLDoc->createElement(_T("sbtrpt:Field"));
	fieldEntry->setAttribute(_T("Name"),_T(propXMLName));
	fieldEntry->Puttext(_T(sValue));  
	fieldEntry = pParentXMLTag->appendChild(fieldEntry);

	if(sValue != NULL && lFreeValue == true) MEM_free(sValue);
	fieldEntry = NULL;

	if(lDebug)printf(" > EXIT putStringPropertyToXML \n");

	return 0;
}

char *getInternalIdOfTcObject(tag_t tTcObject){
	char *sInternalUID = NULL;

	if(lDebug)printf(" > ENTER getInternalIdOfTcObject \n");

	ITK__convert_tag_to_uid(tTcObject, &sInternalUID);

	char *sReturnValue = (char*) MEM_alloc(strlen(sInternalUID) + 1);  
	strcpy(sReturnValue, sInternalUID);
	MEM_free(sInternalUID);

	if(lDebug)printf(" > EXIT getInternalIdOfTcObject \n");

	return sReturnValue;
}

int putLogicalPropertyToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *propXMLName, char *propTcName){
	char	*sValue		= "";
	logical	lValue		= false;
	logical lFreeValue	= true;

	if(lDebug)printf(" > ENTER putLogicalPropertyToXML \n");

	// BT6 / BT7 differentiation
	std::string strPropName = propTcName;
	if(strPropName.substr(0, 3).compare("bt7") == 0){

		if(isDerivedFromClass("Part_0_Revision_alt", tObject) || isDerivedFromClass("Part", tObject)) {
			logical lAttributeExists = false;
			tag_t	tClass		= NULLTAG;
			char	*sClassName	= NULL;

			// Ask Class and Class Name
			ITK_CALL(POM_class_of_instance(tObject, &tClass));
			ITK_CALL(POM_name_of_class(tClass, &sClassName));
			
			// Attribute exists
			ITK_CALL(POM_attr_exists(propTcName, sClassName, &lAttributeExists));

			if(lAttributeExists == true)
			{
				ITK_CALL ( AOM_ask_value_logical(tObject, propTcName, &lValue));
				sValue = (lValue == true) ? "True" : "False";
			} else {
				printf(" > Attribute '%s' does not exist for class '%s' \n", propTcName, sClassName);
			}
			
			// Free
			if(sClassName != NULL) MEM_free(sClassName);
		} else {
			sValue = "---";
			lFreeValue = false;
		}

		//// Is bt7_* property
		//char sObjectType[WSO_name_size_c+1];
		//ITK_CALL ( WSOM_ask_object_type ( tObject, sObjectType ) );
		//std::string strObjectType = sObjectType;
		//if(strObjectType.substr(0, 3).compare("BT7") == 0) {
		//	// Is BT7_* type
		//	ITK_CALL ( AOM_ask_value_logical(tObject, propTcName, &lValue));
		//	sValue = (lValue == true) ? "True" : "False";
		//} else {
		//	sValue = "<No Value>";
		//	lFreeValue = false;
		//}
	} else {
		ITK_CALL ( AOM_ask_value_logical(tObject, propTcName, &lValue));
		sValue = (lValue == true) ? "True" : "False";
	}

	// Create XML-Field
	MSXML2::IXMLDOMElementPtr fieldEntry = pXMLDoc->createElement(_T("sbtrpt:Field"));
	fieldEntry->setAttribute(_T("Name"),_T(propXMLName));
	fieldEntry->Puttext(_T(sValue));  
	fieldEntry = pParentXMLTag->appendChild(fieldEntry);

	fieldEntry = NULL;

	if(lDebug)printf(" > EXIT putLogicalPropertyToXML \n");

	return 0;
}

int putRelationToXML(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, char *sRelationTcName, char *sRelationXMLName, std::vector<tag_t> *vecUsedRevisionsForReport){
	tag_t	tPartDocRelType	= NULLTAG;
	int		iCountSeconcary	= 0;
	tag_t	*taSecondaries	= NULL;
	date_t	dateCreated;
	date_t	dateCreatedTmp;
	std::vector<tag_t> vecReportRevs;
	if(lDebug)printf(" > ENTER putRelationToXML \n");

	ITK_CALL ( GRM_find_relation_type(sRelationTcName, &tPartDocRelType));
	ITK_CALL ( GRM_list_secondary_objects_only(tObject, tPartDocRelType, &iCountSeconcary, &taSecondaries));

	for(int iIdx1 = 0; iIdx1 < iCountSeconcary; iIdx1++) {
		tag_t	tLatestRev							= NULLTAG;
		char	sLatestRevId[ITEM_id_size_c + 1]	= "";
		char	*sLatestReleaseStatus				= "";
		logical isItem								= isDerivedFromClass ( "Item", taSecondaries[iIdx1] );
		logical isItemRevision						= isDerivedFromClass ( "ItemRevision", taSecondaries[iIdx1] );

		// Get correct Document Revision, if more than Revision one of the same Item are attached --- TODO: VERIFY LOGIC WITH SBT!!
		if(isItem == true) {
			ITK_CALL ( ITEM_ask_latest_rev ( taSecondaries[iIdx1], &tLatestRev ) );
		} else if(isItemRevision == true) {
			tag_t	tItem				= NULLTAG;
			tag_t	*taItemRevisions	= NULL;
			int		iRevCount			= 0;

			ITK_CALL ( ITEM_ask_item_of_rev ( taSecondaries[iIdx1], &tItem ) );
			ITK_CALL ( ITEM_list_all_revs ( tItem, &iRevCount, &taItemRevisions ) );
			for(int iIdx2 = 0; iIdx2 < iRevCount; iIdx2++) {
				char	sRevId[ITEM_id_size_c + 1];
				int		iStatusCount	= 0;
				tag_t	*taStatusList	= NULL;
				logical	lHasStatus10	= false;
				logical	lHasStatus60	= false;
				logical	lHasStatus90	= false;
				// Revision attached to parent Part?
				logical lDocRevRelatedToPart = false;
				for(int iIdx3 = 0; iIdx3 < iCountSeconcary; iIdx3++) {
					if(taItemRevisions[iIdx2] == taSecondaries[iIdx3]){
						lDocRevRelatedToPart = true;
						break;
					}
				}
				if(lDocRevRelatedToPart == false){
					continue;
				}

				// Get Status
				ITK_CALL ( WSOM_ask_release_status_list ( taItemRevisions[iIdx2], &iStatusCount, &taStatusList ) );
				for(int iIdx3 = 0; iIdx3 < iStatusCount; iIdx3++) {
					char *sStatusName = NULL;
					
					ITK_CALL ( AOM_UIF_ask_value(taStatusList[iIdx3], "object_name", &sStatusName));
					if(strcmp(sStatusName, "10") == 0) {
						lHasStatus10 = true;
					} else if(strcmp(sStatusName, "60") == 0) {
						lHasStatus60 = true;
					} else if(strcmp(sStatusName, "90") == 0) {
						lHasStatus90 = true;
					}

					if(sStatusName != NULL) MEM_free(sStatusName);
				}

				// Get Rev-Id
				//ITK_CALL ( ITEM_ask_rev_id ( taItemRevisions[iIdx2], sRevId ) );
                                // Bug fix for issue 836
				// Get creation date 
				ITK_CALL( AOM_ask_value_date(taItemRevisions[iIdx2], "creation_date", &dateCreated) );
				if(tLatestRev == NULLTAG) {
					// First checked revision
					tLatestRev = taItemRevisions[iIdx2];
					//strcpy (sLatestRevId, sRevId);
					dateCreatedTmp = dateCreated;
				} else {
					// Further revisions
					if(strcmp(doc_revision_rule, "Latest Released") == 0) {
			  			// Latest Released
						// Compare creation dates instead of revisions here
						int iDateCompareResult = CLOCK_compare_dates(&dateCreated, &dateCreatedTmp);
						if ( iDateCompareResult > 0 )
						//if(strcmp(sLatestRevId, sRevId) < 0) 
						{
							if(lHasStatus60 == true) {
								// Use latest status 60
								tLatestRev = taItemRevisions[iIdx2];
								dateCreatedTmp = dateCreated;
								//strcpy (sLatestRevId, sRevId);
								sLatestReleaseStatus = "60";
							} else if( (lHasStatus10 == true) && (strcmp(sLatestReleaseStatus, "60") != 0) ) {
								// If no status 60 has been found, use latest status 10
								tLatestRev = taItemRevisions[iIdx2];
								dateCreatedTmp = dateCreated;
								//strcpy (sLatestRevId, sRevId);
								sLatestReleaseStatus = "10";
							} 
						}
					} else {
						int iDateCompareResult = CLOCK_compare_dates(&dateCreated, &dateCreatedTmp);
						// Latest Working with date compare logic
						if ( iDateCompareResult > 0 )
						{
							printf("\n Latest rev is : %s",sRevId);
							tLatestRev = taItemRevisions[iIdx2];
							dateCreatedTmp = dateCreated;
						}
						// Latest Working
					/*	if(strcmp(sLatestRevId, sRevId) < 0){
							tLatestRev = taItemRevisions[iIdx2];
							strcpy (sLatestRevId, sRevId);
						}*/
					}
				}

				if(taStatusList != NULL) MEM_free(taStatusList);
			}

			if(taItemRevisions != NULL) MEM_free(taItemRevisions);
		// END: Get correct Document Revision, if more than Revision one of the same Item are attached
		} else {
			char object_type[WSO_name_size_c+1];
			ITK_CALL ( WSOM_ask_object_type ( taSecondaries[iIdx1], object_type ) );
			printf(" >>> ERROR: Object type is neither Item nor Item Revision: %s \n", object_type);
			continue;
		}

		// Verify that current Rev is not already attached to the report
		logical lElementAlreadyExists = false;
		for(int iIdx2 = 0; iIdx2 < (*vecUsedRevisionsForReport).size(); iIdx2++){
			if((*vecUsedRevisionsForReport).at(iIdx2) == tLatestRev){
				lElementAlreadyExists = true;
				break;
			}
		}

		if(lElementAlreadyExists == true) {
			continue;
		} else {
			logical	lHasStatus98	= false;
			logical	lIs_DOC			= false;

			// Prevent Status 98 _DOCs
			{
				tag_t	*taStatusList	= NULL;
				int		iStatusCount	= 0;
				tag_t	tRelatedItem	= NULLTAG;
				char	sRelatedItemId[ITEM_id_size_c + 1];
				string strRelatedItemId = "";
				
				// Has Status 98
				ITK_CALL(WSOM_ask_release_status_list(tLatestRev, &iStatusCount, &taStatusList));
				for(int iIdx3 = 0; iIdx3 < iStatusCount; iIdx3++)
				{
					char *sTmpStatusName = NULL;

					// Read tmp-status name
					ITK_CALL(AOM_UIF_ask_value(taStatusList[iIdx3], "object_name", &sTmpStatusName));

					// Is Status 98
					if(strcmp(sTmpStatusName, "98") == 0)
					{
						lHasStatus98 = true;
					}

					// Free
					if(sTmpStatusName != NULL)MEM_free(sTmpStatusName);
				}

				// Is _DOC
				ITK_CALL(ITEM_ask_item_of_rev(tLatestRev, &tRelatedItem));
				ITK_CALL(ITEM_ask_id(tRelatedItem, sRelatedItemId));
				strRelatedItemId = sRelatedItemId;

				if(strRelatedItemId.size() > 4)
				{
					// Has Status 98 and is an _DOC-Object ?
					if(strRelatedItemId.substr(strRelatedItemId.length() - 4, 4).compare("_DOC") == 0)
					{
						lIs_DOC = true;
					}
				}

				// Free
				if(taStatusList != NULL)MEM_free(taStatusList);
			}

			// Skip current Revision ?
			if(lIs_DOC == true && lHasStatus98 == true)
			{
				continue;
			}

			// Add to vector
			(*vecUsedRevisionsForReport).push_back(tLatestRev);
			vecReportRevs.push_back(tLatestRev);
		}
	}

	// Issue 1050. BOM Reports - MD Report - Sort the attached doc.
	std::sort(vecReportRevs.begin(), vecReportRevs.end(), sortPartNum);

	for (std::vector<tag_t>::iterator it = vecReportRevs.begin(); it != vecReportRevs.end(); it++)
	{

		tag_t tLatestRev = *it;

		if ( tLatestRev == NULL_TAG)
		{
			continue;
		}

		// Preparation
		char		*sEntDocType	= NULL;
		logical		lIsBT7Doc		= false;
		int			iStatusCount	= 0;
		std::string	sStatusName		= "<No Status>";
		char        sDocObjType[WSO_name_size_c+1];

		//changed the attribute name from 'bt7_entdoc_type' from 'bt7_documentClass' 
		ITK_CALL ( WSOM_ask_object_type ( tLatestRev, sDocObjType ) );
		if ( strcmp (sDocObjType, "BT7_DocumentRevision") == 0 ) {
			ITK_CALL ( AOM_ask_value_string(tLatestRev, "bt7_documentClass", &sEntDocType)); // technical name required!
			lIsBT7Doc = true;
		} else if ( strcmp (sDocObjType, "BT7_ComponentRevision") == 0 ) { 
			sEntDocType = "LGSouMan";// BT6_ProductPart
		} else {
			sEntDocType = "---";
		}

		// Ask latest release status
		char *sTmpStatusName = NULL;
		askLatestReleaseStatus(tLatestRev, &sTmpStatusName, &iStatusCount);

		if(iStatusCount > 0){
			sStatusName.clear();
			sStatusName.append(sTmpStatusName);
		} else {
			sStatusName.clear();
			sStatusName.append("---");
		}
		if(sTmpStatusName != NULL) MEM_free(sTmpStatusName);
		
		// Define new Relation Header
		MSXML2::IXMLDOMElementPtr newRelationXMLEntry = pXMLDoc->createElement(_T("sbtrpt:Relation"));
		newRelationXMLEntry->setAttribute(_T("Class"),_T(sRelationXMLName));
		if(isPCBR || isEQPR) newRelationXMLEntry->setAttribute(_T("OBID"),_T(getInternalIdOfTcObject(tLatestRev))); // TODO: Check
		newRelationXMLEntry = pParentXMLTag->appendChild(newRelationXMLEntry);


		// TODO: Additional field for PCB BOM Report
		if(isPCBR || isEQPR) {
			if(strcmp(sRelationXMLName, "l0DocPrt") == 0 ||
				strcmp(sRelationXMLName, "PartDoc") == 0)
			{
				//PartDoc & l0DocPrt
				MSXML2::IXMLDOMElementPtr fieldEntry = pXMLDoc->createElement(_T("sbtrpt:Field"));
				fieldEntry->setAttribute(_T("Name"),_T("l0DocInBom"));
				fieldEntry->Puttext(_T("True"));
				fieldEntry = newRelationXMLEntry->appendChild(fieldEntry);
			} else {
				//l0PrDcLv
				MSXML2::IXMLDOMElementPtr fieldEntry = pXMLDoc->createElement(_T("sbtrpt:Field"));
				fieldEntry->setAttribute(_T("Name"),_T("l0DocInBom"));
				fieldEntry->Puttext(_T("False"));
				fieldEntry = newRelationXMLEntry->appendChild(fieldEntry);
			}
		}


		// Define new Relation Details
		MSXML2::IXMLDOMElementPtr newRelationDetailsXMLEntry = pXMLDoc->createElement(_T("sbtrpt:Node"));
		newRelationDetailsXMLEntry->setAttribute(_T("Class"),_T(sEntDocType));
		if(isPCBR || isEQPR) newRelationDetailsXMLEntry->setAttribute(_T("OBID"),_T(getInternalIdOfTcObject(tLatestRev)));
		newRelationDetailsXMLEntry = newRelationXMLEntry->appendChild(newRelationDetailsXMLEntry);


		// Field 1 - DisplayedType
		//if(isMDR) putStringValueToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DisplayedType", "$ToDo$");
		if(isMDR) putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DisplayedType", "bt7_doctype");
	

		// Field 2 - DocumentName
		tag_t tDocItem;
		ITK_CALL ( ITEM_ask_item_of_rev(tLatestRev, &tDocItem));
		if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tDocItem, "DocumentName", "item_id");

		// Issue 1029. Report Mechanical Design -> Fields Designation en/de are not filled correctly
		// Field 3 - DocumentTitle
		if(isMDR )
		{
			// For BT7_DocumentRevision, german name is stored on attribute bt7_doctitle_de
			if ( strcmp (sDocObjType, "BT7_DocumentRevision") == 0 ) {

				putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DocumentTitle", "bt7_doctitle_de");

			// For all children of Part Revision (BT7 parts), german name is stored on attribute bt7_ItemRevName_2
			} else if (strstr (sDocObjType, "BT7_") == sDocObjType )  {
				
				putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DocumentTitle", "bt7_ItemRevName_2");

			// For all direct children of ItemRevision (BT6 objects), german name is stored 
			// on attribute BT6_ItemRevName_2 of the ItemRevision Master Form
			} else if (strstr (sDocObjType, "BT7_") == sDocObjType )  {

				tag_t  tMasterFormRelType = NULLTAG;
				tag_t *taMasterForms      = NULL;
				int    iMasterFormCount   = 0;

				ITK_CALL(GRM_find_relation_type("IMAN_master_form", &tMasterFormRelType) );
				ITK_CALL(GRM_list_secondary_objects_only(tLatestRev, tMasterFormRelType, &iMasterFormCount, &taMasterForms) );

				if ( iMasterFormCount == 1 )
				{
					putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, taMasterForms[0], "DocumentTitle", "bt7_ItemRevName_2");
				} else {
					putStringValueToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DocumentTitle", "---");
				}

			// No german name is found
			} else {

				putStringValueToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DocumentTitle", "---");

			}

		// Issue 1140. Euq. Part / PCB Report Document Nomenclature not correct.
		} else if (isPCBR || isEQPR) {

			putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "DocumentTitle", "object_name");

		}

		// Field 4 - LifeCycleState
		if(isMDR || isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "LifeCycleState", sStatusName.c_str());
		
		// Field 5 - Revision
		if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "Revision", "item_revision_id");

		// Field 6 - l0DocumentTitleEng
		if(isMDR) putStringPropertyToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "l0DocumentTitleEng", "object_name");


		/////////////////// TODO: Attributes for PCB Report
		if(false) putStringValueToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "ReleaseLevel", "");
		if(isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newRelationDetailsXMLEntry, tLatestRev, "Superseded", "False");
		///////////////////


		// Cleanup
		newRelationXMLEntry = NULL;
		if(sEntDocType != NULL && lIsBT7Doc == true) MEM_free(sEntDocType);
	}

	if(lDebug)printf(" > EXIT putRelationToXML \n");

	// Free
	if(taSecondaries != NULL) MEM_free(taSecondaries);
}

bool sortPartNum (tag_t tRev1, tag_t tRev2)
{
	int iresult = 0;

	tag_t tItem1 = NULLTAG;
	tag_t tItem2 = NULLTAG;

	char sItemId1[ITEM_id_size_c+1] = "";
	char sItemId2[ITEM_id_size_c+1] = "";

	iresult = ITEM_ask_item_of_rev(tRev1, &tItem1);
	iresult = ITEM_ask_item_of_rev(tRev2, &tItem2);

	if ( tItem1 != NULLTAG )
	{
		iresult = ITEM_ask_id (tItem1, sItemId1);
	}

	if ( tItem2 != NULLTAG )
	{
		iresult = ITEM_ask_id (tItem2, sItemId2);
	}

	return ( strcmp(sItemId1, sItemId2) < 0);

}


void getAllBOMLines(tag_t tRevTag, int *childBOMLinesCount, tag_t **childBOMLines, logical lPacked) {
	// Init traversal of BOM
    tag_t windowTag			= NULLTAG;
    tag_t ruleTag			= NULLTAG;
    tag_t topLineTag		= NULLTAG;

	if(lDebug)printf(" > ENTER getAllBOMLines \n");

	ITK_CALL ( CFM_find ( revision_rule , &ruleTag ) );
	
	ITK_CALL ( BOM_create_window(&windowTag));
	ITK_CALL ( BOM_set_window_config_rule(windowTag , ruleTag));
	ITK_CALL ( BOM_set_window_top_line(windowTag , NULLTAG , tRevTag , NULLTAG , &topLineTag));
	ITK_CALL ( BOM_set_window_pack_all(windowTag , lPacked));
	ITK_CALL ( BOM_line_ask_child_lines(topLineTag , childBOMLinesCount , childBOMLines));

	if(lDebug)printf(" > EXIT getAllBOMLines \n");
}

int handleObject(MSXML2::IXMLDOMDocument2Ptr pXMLDoc, MSXML2::IXMLDOMElementPtr pParentXMLTag, tag_t tObject, int iLevel, char *sRefDesValue) {
    char	sObjectType[WSO_name_size_c+1];
	char	*sCustomClassName = "";

	if(lDebug)printf(" > ENTER handleObject \n");

	ITK_CALL ( WSOM_ask_object_type(tObject, sObjectType));

	// Get class of current object
	if((strcmp(sObjectType, "BT7_ProductRevision") == 0) || (strcmp(sObjectType, "BT7_ProductRevision") == 0)){ 
		sCustomClassName = "LGProdkt";
	} else /*if(strcmp(sObjectType, "BT7_ComponentRevision") == 0 ||
			  strcmp(sObjectType, "BT7_ToolRevision") == 0 ||
			  strcmp(sObjectType, "BT7_SoftwareRevision") == 0 ||
			  strcmp(sObjectType, "BT7_ELCADRevision") == 0 ||
			  strcmp(sObjectType, "BT7_CompProgRevision") == 0 ||
			  strcmp(sObjectType, "BT7_CommonCompRevision") == 0)*/ {
		int		childBOMLinesCount	= 0;
		tag_t	*childBOMLines		= NULL;

		// Has BOM Lines ?
		getAllBOMLines(tObject, &childBOMLinesCount, &childBOMLines, true);
		if(childBOMLinesCount == 0) {

			// Check for l0SbtCmp or Cmponent
			if((strcmp(sObjectType, "BT7_CommonCompRevision") == 0))
			{
				sCustomClassName = "l0SbtCmp";
			}
			else
			{
				sCustomClassName = "Cmponent";
			}
		
		// Issue 990. Equal Parts Equipment Report / Presentation. Extra logic for BT7_CompProgRevision
		// When BT7_CompProgRevision is found, hide it and then show the children.
		} else if ( strcmp(sObjectType, "BT7_CompProgRevision") == 0 ){
			sCustomClassName = "l0AsmPrg";
		} else {
			sCustomClassName = "Assembly";
		}

		// Free
		if(childBOMLines != NULL) MEM_free(childBOMLines);
	}

	// Class found ?
	if(strcmp(sCustomClassName, "") == 0) {
		printf(" > ERROR: Class %s not handled by the code! \n", sObjectType);
		return -1;
	}

	// Define new XML Entry
	MSXML2::IXMLDOMElementPtr newXMLEntry = pXMLDoc->createElement(_T("sbtrpt:Node"));
	newXMLEntry->setAttribute(_T("Class"),_T(sCustomClassName));
	newXMLEntry->setAttribute(_T("OBID"),_T(getInternalIdOfTcObject(tObject)));
	newXMLEntry = pParentXMLTag->appendChild(newXMLEntry);

	// Define Fields
	// Issue 962. BOM Reports - "verantwortliche Stelle" is not filled.
	if(strcmp(sCustomClassName, "LGProdkt") == 0 || iLevel == 0) {
		tag_t	tOwningUser		= NULLTAG;
		tag_t	tOwningPerson	= NULLTAG;
		char	sPersonName[SA_person_name_size_c + 1] = "";
		char	*sPersonPhone	= NULL;
		char	*sPersonOrg	= NULL;

		ITK_CALL ( AOM_ask_value_tag ( tObject, "owning_user", &tOwningUser ) );
		ITK_CALL ( SA_ask_user_person ( tOwningUser, &tOwningPerson ) );
		ITK_CALL ( SA_ask_person_name ( tOwningPerson, sPersonName ) );
		ITK_CALL ( SA_ask_person_attr ( tOwningPerson, "PA10", &sPersonPhone ) );
		ITK_CALL ( SA_ask_person_attr ( tOwningPerson, "PA6", &sPersonOrg ) );

		// Used for PCB BOM report
		if(isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "ResponsablePerson", sPersonName);
		if(isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "LGRespPhone", sPersonPhone);
		if(isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "LGRespUnit", sPersonOrg);
		if(false) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "RespParty", "");	// TODO: Clarify! Group-Name?

		// Free
		if(sPersonPhone != NULL) MEM_free(sPersonPhone);
		if(sPersonOrg != NULL) MEM_free(sPersonOrg);
	}

	if(strcmp(sCustomClassName, "LGProdkt") == 0 || strcmp(sCustomClassName, "Assembly") == 0 
		|| strcmp(sCustomClassName, "l0AsmPrg") == 0 || strcmp(sCustomClassName, "l0SbtCmp") == 0
		|| strcmp(sCustomClassName, "Cmponent") == 0) {

		tag_t			tItem			= NULLTAG;
		ostringstream	sLevel;
		int				iStatusCount	= 0;
		tag_t			*taStatusList	= NULL;
		date_t			dateReleased;
		char			*sFormattedDate	= "<no rel. date>";

		// Preparation
		ITK_CALL ( ITEM_ask_item_of_rev(tObject, &tItem));

		sLevel << iLevel;

		// TODO: Find latest release status!!!


		// Ask latest release status
		char	*sTmpStatusName = NULL;
		tag_t	tLatestStatus	= NULLTAG;
		askLatestReleaseStatus(tObject, &sTmpStatusName, &iStatusCount, &tLatestStatus);
		std::string	sStatusName	= "";

		if(iStatusCount > 0){
			sStatusName.clear();
			sStatusName.append(sTmpStatusName);

			ITK_CALL ( AOM_ask_value_date(tLatestStatus, "date_released", &dateReleased));
			ITK_CALL ( DATE_date_to_string(dateReleased, "%Y.%m.%d", &sFormattedDate));
		} else {
			sStatusName.clear();
			sStatusName.append("---");
		}
		if(sTmpStatusName != NULL) MEM_free(sTmpStatusName);

		//ITK_CALL ( WSOM_ask_release_status_list(tObject, &iStatusCount, &taStatusList));
		//for(int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++) {
		//	char *sTmpStatusName = NULL;
		//	ITK_CALL ( AOM_UIF_ask_value(taStatusList[iIdx2], "name", &sTmpStatusName));
		//	if(iIdx2 == 0){
		//		sStatusName.clear();
		//		sStatusName.append(sTmpStatusName);
		//	} else {
		//		sStatusName.append(",");
		//		sStatusName.append(sTmpStatusName);
		//	}
		//	if(sTmpStatusName != NULL) MEM_free(sTmpStatusName);
		//}


		// Field 0 - Class
		putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "Class", sCustomClassName);


		// Field 1: Nomenclature
		if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newXMLEntry, tObject, "Nomenclature", "object_name");

		// Issue 1029. Report Mechanical Design -> Fields Designation en/de are not filled correctly
		// Field 1.1: Nomenclature2
		if(isMDR || isPCBR || isEQPR)
		{
			// For BT7_DocumentRevision, german name is stored on attribute bt7_doctitle_de
			if ( strcmp (sObjectType, "BT7_DocumentRevision") == 0 ) {

				putStringPropertyToXML(pXMLDoc, newXMLEntry, tObject, "Nomenclature2", "bt7_doctitle_de");

			// For all children of Part Revision (BT7 parts), german name is stored on attribute bt7_ItemRevName_2
			} else if (strstr (sObjectType, "BT7_") == sObjectType )  {
				
				putStringPropertyToXML(pXMLDoc, newXMLEntry, tObject, "Nomenclature2", "bt7_ItemRevName_2");

			// For all direct children of ItemRevision (BT6 objects), german name is stored 
			// on attribute BT6_ItemRevName_2 of the ItemRevision Master Form
			} else if (strstr (sObjectType, "BT7_") == sObjectType )  {

				tag_t  tMasterFormRelType = NULLTAG;
				tag_t *taMasterForms      = NULL;
				int    iMasterFormCount   = 0;

				ITK_CALL(GRM_find_relation_type("IMAN_master_form", &tMasterFormRelType) );
				ITK_CALL(GRM_list_secondary_objects_only(tObject, tMasterFormRelType, &iMasterFormCount, &taMasterForms) );

				if ( iMasterFormCount == 1 ) {

					putStringPropertyToXML(pXMLDoc, newXMLEntry, taMasterForms[0], "Nomenclature2", "bt7_ItemRevName_2");
				}
				else{
					putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "Nomenclature2", "---");
				}

			// No german name is found
			} else {

				putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "Nomenclature2", "---");

			}


		}

		// Field 2: LifeCycleState
		if(isMDR || isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "LifeCycleState", sStatusName.c_str());

		// Field 3: PartNumber
		putStringPropertyToXML(pXMLDoc, newXMLEntry, tItem, "PartNumber", "item_id");

		// Field 4: Revision
		if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newXMLEntry, tObject, "Revision", "item_revision_id");

		// Field 5: l0ReleaseDate
		if(isMDR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "l0ReleaseDate", sFormattedDate);

		// Field 6: l0TechInfo
		if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newXMLEntry, tObject, "l0TechInfo", "bt7_techinfo");

		// Field 7: PartType
		if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newXMLEntry, tObject, "PartType", "bt7_parttype");

		// Field 8: L0DocCode
		//TODO: Differentiate between Parts and Docs!!!
		putStringPropertyToXML(pXMLDoc, newXMLEntry, tItem, "L0DocCode", "bt7_doceqpartno");

		// Field 9: Level
		putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "Level", sLevel.str().c_str());

		
		// TODO: Attributes for PCB Report
		if(false) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "MakeBuyIndicator", "Make"); // TODO: ObjectType = BT7_CommonComp || Property on Part?
		if(isPCBR || isEQPR || isEQPER) putLogicalPropertyToXML(pXMLDoc, newXMLEntry, tObject, "l0SMDevice", "bt7_smd");
		if(isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "StdPartIndicator", "False"); // TODO: ObjectType = BT7_CommonComp || bt7_parttype == "Electronic Component" ?
		if(isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "Superseded", "False"); // TODO: ???
		//
		
		// TODO: Attributes for Equal Part Equipment Report
		if(isPCBR || isEQPR || isEQPER) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "l0RefDesigList", sRefDesValue);
		if(false) putStringValueToXML(pXMLDoc, newXMLEntry, tObject, "l9RelFN", sRefDesValue);
		//

		// Clean
		sLevel.clear();
		if(taStatusList != NULL) MEM_free(taStatusList);
		if(iStatusCount > 0) MEM_free(sFormattedDate);
	}

	// Define Document Relations
	if(strcmp(sCustomClassName, "LGProdkt") == 0 || strcmp(sCustomClassName, "Assembly") == 0 
		|| strcmp(sCustomClassName, "l0AsmPrg") == 0 || strcmp(sCustomClassName, "l0SbtCmp") == 0
		|| strcmp(sCustomClassName, "Cmponent") == 0) {
		// TODO: According to the Stylesheet we need to respect the following relations: J0DocStr,l0DocPrt,PartDoc,l0PrDcLv

		std::vector<tag_t> vecUsedRevisionsForReport;

		putRelationToXML(pXMLDoc, newXMLEntry, tObject, "BT7_Rel_Specified", "PartDoc", &vecUsedRevisionsForReport);
		putRelationToXML(pXMLDoc, newXMLEntry, tObject, "BT7_Rel_Concerned", "l0DocPrt", &vecUsedRevisionsForReport); 

		// Issue 813. Extend the Relationen für die BOM Reports
		if(strcmp(sIncludeSUP, "true") == 0){
			putRelationToXML(pXMLDoc, newXMLEntry, tObject, "BT7_SUPPLIERSPEC", "l0PrDcLv", &vecUsedRevisionsForReport);
		}
	}

	// Stop of the traversal required?
	logical		lKeepTraversingBOM = true;
	{
		char sObjectType[WSO_name_size_c+1];

		// Get ObjectType
		ITK_CALL(WSOM_ask_object_type(tObject, sObjectType));

		// If the current part is a BT7_ComponentRevision and the part type is equal to 'l7LGCodPEq' and the option 'IncludePCB' is 'false' -> stop traversal of the BOM
		if(strcmp(sObjectType, "BT7_ComponentRevision") == 0) {
			char *sPartType = NULL;

			// Get Part Type
			ITK_CALL(AOM_ask_value_string(tObject, "bt7_parttype", &sPartType));

			// Stop required?
			if(strcmp(sReportType, "MD Report") == 0 && strcmp(sIncludePCB, "false") == 0 && strcmp(sPartType, "l7LGCodPEq") == 0){
				lKeepTraversingBOM = false;
			}

			// Free
			if(sPartType != NULL) MEM_free(sPartType);
		}
	}
	//l7LGCodPEq


	// Define BOM Structures
	if(lKeepTraversingBOM == true && ((strcmp(sCustomClassName, "LGProdkt") == 0 || strcmp(sCustomClassName, "Assembly") == 0 
		|| strcmp(sCustomClassName, "l0AsmPrg") == 0 || strcmp(sCustomClassName, "l0SbtCmp") == 0
		|| strcmp(sCustomClassName, "Cmponent") == 0) && (iLevel < iMaxBomLevel))) {
		int		childBOMLinesCount	= 0;
		tag_t	*childBOMLines		= NULL;

		// Get Child lines (unpacked if EqualPartReport or EqualPartEquipmentReport)
		if(isEQPER || isEQPR || isPCBR) 
		{
			getAllBOMLines(tObject, &childBOMLinesCount, &childBOMLines, false);
		} else {
			getAllBOMLines(tObject, &childBOMLinesCount, &childBOMLines, true);
		}

		// Traverse child lines
		for(int iIdx1 = 0; iIdx1 < childBOMLinesCount; iIdx1++){
			// Prepare Recursive Call
			int		iBomLineItemRevAttribute	= 0;
			int		iBomLineEbomRelevant		= 0;
			tag_t	tChildRevTag				= NULLTAG;
			char	*sEbomRelevant				= NULL;

			// Check if the Revision Rule could be resolved (-> attribute 'bl_revision' is null or empty)
			ITK_CALL ( BOM_line_look_up_attribute(/*bomAttr_lineItemRevTag*/ bomAttr_lineObjectTag, &iBomLineItemRevAttribute));
			ITK_CALL ( BOM_line_ask_attribute_tag(childBOMLines[iIdx1], iBomLineItemRevAttribute , &tChildRevTag));

			// Check if the BOMLine is "EBOM Relevant" or not
			ITK_CALL ( BOM_line_look_up_attribute("BT7_EBOM_Relevant", &iBomLineEbomRelevant));
			ITK_CALL ( BOM_line_ask_attribute_string (childBOMLines[iIdx1], iBomLineEbomRelevant , &sEbomRelevant));

			if ( tChildRevTag != NULLTAG && stricmp(sEbomRelevant, "NO") != 0) {

				// Define new BOM Lines Header
				MSXML2::IXMLDOMElementPtr newBOMLineXMLEntry = pXMLDoc->createElement(_T("sbtrpt:Relation"));
				newBOMLineXMLEntry->setAttribute(_T("Class"),_T("AsRevRev"));
				newBOMLineXMLEntry = newXMLEntry->appendChild(newBOMLineXMLEntry);

				// Field 0 - Class
				putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "Class", "AsRevRev");

				// Field 1 - DisplayQuantity
				char *sBlQuantity = NULL;
				char *sRealQuantity = NULL;
				std::string strBlQuantity = "";

				ITK_CALL ( AOM_UIF_ask_value(childBOMLines[iIdx1], "fnd0bl_real_quantity", &sBlQuantity));
				ITK_CALL ( AOM_UIF_ask_value(childBOMLines[iIdx1], "bl_real_quantity", &sRealQuantity));

				if ( strcmp(sRealQuantity, "0.000000") == 0 || strcmp(sRealQuantity, "1.000000") == 0 )
				{
					if(strcmp(sBlQuantity, "") == 0 )
					{
						strBlQuantity = "1";
					}
					else
					{
						strBlQuantity = sBlQuantity;
					}
				}
				else
				{
					strBlQuantity = sRealQuantity;
				}

				if(strBlQuantity.compare("") != 0){
					while(strBlQuantity.substr(strBlQuantity.length()-1,strBlQuantity.length()).compare("0") == 0){
						strBlQuantity = strBlQuantity.substr(0, strBlQuantity.length()-1);
					}
					if(strBlQuantity.compare("") != 0){
						if(strBlQuantity.substr(strBlQuantity.length()-1,strBlQuantity.length()).compare(".") == 0){
							strBlQuantity = strBlQuantity.substr(0, strBlQuantity.length()-1);
						}
					}
				}
				if(isMDR) putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "DisplayQuantity", strBlQuantity.c_str());
				putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "Quantity", strBlQuantity.c_str());// TODO: Added for PCB BOM XML

				if(sBlQuantity != NULL) MEM_free(sBlQuantity);

				// Field 2 - FindNumber
				if(isMDR || isPCBR || isEQPR) putStringPropertyToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "FindNumber", "bl_sequence_no");

				// Field 3 - ReferenceDesignator (special treatment to enable passing of the parameter to the recursive call. This had to be implemented because the Eq Part Equipment Report requires this attribute
				char *sTmpRefDesValue = NULL;
				char sNewRefDesValue[30];
				ITK_CALL ( AOM_UIF_ask_value ( childBOMLines[iIdx1], "bl_occ_ref_designator", &sTmpRefDesValue));
				strcpy(sNewRefDesValue, sTmpRefDesValue);
				if(isMDR) putStringPropertyToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "ReferenceDesignator", "bl_occ_ref_designator");
				if(isPCBR || isEQPR || isEQPER) putStringPropertyToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "l0RefDesigListCl", "bl_occ_ref_designator");// TODO: Added for PCB BOM XML
				if(sTmpRefDesValue != NULL) MEM_free(sTmpRefDesValue);

				// Field 4 - UnitOfMeasure
//				tag_t	tUOM	= NULLTAG;
//				tag_t	tItem	= NULLTAG;
				char 	*sTmpUom = NULL;

//				ITK_CALL ( ITEM_ask_item_of_rev(tChildRevTag, &tItem));
//				ITK_CALL ( AOM_ask_value_tag(tItem, "uom_tag", &tUOM));
				ITK_CALL ( AOM_UIF_ask_value ( childBOMLines[iIdx1], "bl_uom", &sTmpUom));

				if ( sTmpUom != NULL && strcmp(sTmpUom, "") != 0 && strcmp(sTmpUom, "each") != 0 )
				{
					if(isMDR || isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "UnitOfMeasure", sTmpUom);
				}
				else
				{
					if(isMDR || isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "UnitOfMeasure", "ea");// Default value if not UOM has been set
				}

//				if(tUOM != NULLTAG){
//					char	*sUOMSymbol	= NULL;
//					ITK_CALL ( UOM_ask_symbol(tUOM, &sUOMSymbol));
//					if(isMDR || isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "UnitOfMeasure", sUOMSymbol);
//					if(sUOMSymbol != NULL) MEM_free(sUOMSymbol);
//				} else {
//					if(isMDR || isPCBR || isEQPR) putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "UnitOfMeasure", "ea");// Default value if not UOM has been set
//				}

				// Field 5 - Mat In BOM --- TODO: Added for PCB BOM XML
				if(false) putStringValueToXML(pXMLDoc, newBOMLineXMLEntry, childBOMLines[iIdx1], "l0MatInBom", "True"); 

				// Execute Recursive Call
				handleObject(pXMLDoc, newBOMLineXMLEntry, tChildRevTag, iLevel + 1, sNewRefDesValue);
			} else {
				// Revision Rule could not be resolved
				// TODO: Add "empty" entry to the report
			}

			// Free
			if(sEbomRelevant != NULL) MEM_free(sEbomRelevant);
		}

		// Free
		if(childBOMLines != NULL) MEM_free(childBOMLines);
	}

	// Cleanup
	newXMLEntry = NULL;

	if(lDebug)printf(" > EXIT handleObject \n");

	return 0;
}

int generateReport(tag_t tResult, char *file_path) {
	CoInitialize(NULL);

	if(lDebug)printf(" > ENTER generateReport \n");

	{
		// Get Date / Time
		time_t theTime = time(NULL);
		struct tm *aTime = localtime(&theTime);
		int day = aTime->tm_mday;
		int month = aTime->tm_mon + 1;    // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
		int year = aTime->tm_year + 1900; // Year is # years since 1900
		int hour = aTime->tm_hour;
		int min = aTime->tm_min;

		// Get User Name
		tag_t	tUser		= NULLTAG;
		tag_t	tPerson	= NULLTAG;
		char	sPersonName[SA_person_name_size_c + 1];

		ITK_CALL ( SA_find_user ( sRequestor, &tUser ) );
		ITK_CALL ( SA_ask_user_person ( tUser, &tPerson ) );
		ITK_CALL ( SA_ask_person_name ( tPerson, sPersonName ) );

		// Prepare Header
		std::stringstream ssReportHeader;
		ssReportHeader << "<sbtrpt:ResultSet Title=\"md_slpartlist\" Creator=\"" << sPersonName << "\" Date=\"" << year << "-" << ((month < 10) ? "0" : "") << month << "-" << ((day < 10) ? "0" : "") << day << "\" Time=\"" << ((hour < 10) ? "0" : "") << hour << ":" << ((min < 10) ? "0" : "") << min << "\" xmlns:sbtrpt=\"http://mrtplm.com/pdm/xmlrep\" xmlns:rptsup=\"http://mrtplm.com/pdm/xmlrep-support\"></sbtrpt:ResultSet>";

		// Header
		MSXML2::IXMLDOMDocument2Ptr pXMLDoc;    
		HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
		pXMLDoc->loadXML(_T(ssReportHeader.str().c_str()));
	    
		//Get the root element
		MSXML2::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
		MSXML2::IXMLDOMProcessingInstructionPtr pXMLProcessingNode = pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='ISO-8859-1'");

		_variant_t vtObject;
		vtObject.vt = VT_DISPATCH;
		vtObject.pdispVal = pXMLRootElem;
		vtObject.pdispVal->AddRef();

		pXMLDoc->insertBefore(pXMLProcessingNode, vtObject);

		// Create the child elements
		int iStatus = handleObject(pXMLDoc, pXMLRootElem, tResult, 0, "");

		// Save XML
		try
		{
			if(lDebug)printf(" > BEFORE pXMLDoc->save(file_path) -> file_path = %s \n", file_path);

			if(pXMLDoc->save(file_path) == VARIANT_FALSE)
			{
				printf("ERROR save file %s: %s\n", file_path, (LPCSTR)pXMLDoc->parseError->Getreason());
			}
			if(lDebug)printf(" > AFTER pXMLDoc->save(file_path) \n");
		}
		catch(_com_error errorObject)
		{
			printf("Exception thrown, HRESULT: 0x%08x", errorObject.Error());
		}
		catch(...)
		{
			printf("Unknown error");
		}   
	}
	
	CoUninitialize();

	if(lDebug)printf(" > EXIT generateReport \n");

	return ITK_ok;
}

logical isDerivedFromClass(char *className, tag_t tObjectTag){
	tag_t	tClassIdTag				= NULLTAG;
	tag_t	tSecondaryClassIdTag	= NULLTAG;
	logical	bIsDerived				= false;

	if(lDebug)printf(" > ENTER isDerivedFromClass \n");

	ITK_CALL(POM_class_id_of_class(className , &tClassIdTag));
	if(tClassIdTag != NULLTAG){
		ITK_CALL(POM_class_of_instance(tObjectTag, &tSecondaryClassIdTag));
		if(tSecondaryClassIdTag != NULLTAG){
			ITK_CALL(POM_is_descendant(tClassIdTag, tSecondaryClassIdTag, &bIsDerived));
		}
	}

	if(lDebug)printf(" > EXIT isDerivedFromClass \n");

	return bIsDerived;
}

int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount, tag_t *tLatestStatusTag) {
	int		iRetValue			= ITK_ok;
	tag_t	*taStatus			= NULL;
	date_t	dateReleased;
	date_t	dateReleasedTmp;

	// Get latest release status
	ITK_CALL(WSOM_ask_release_status_list(tObject, iCompleteStatusCount, &taStatus));

	if((*iCompleteStatusCount) > 0)
	{
		// Get the last status
		ITK_CALL(AOM_ask_value_date(taStatus[0], "date_released", &dateReleasedTmp) );
		ITK_CALL(AOM_UIF_ask_value(taStatus[0], "name", sLatestStatusName) );
		*tLatestStatusTag = taStatus[0];

		for ( int iIdx1 = 0; iIdx1 < (*iCompleteStatusCount); iIdx1++ )
		{
			ITK_CALL(AOM_ask_value_date(taStatus[iIdx1], "date_released", &dateReleased) );

			int iDateCompareResult = CLOCK_compare_dates(&dateReleased, &dateReleasedTmp);

			if ( iDateCompareResult > 0 )
			{
				// dateReleased newer than dateReleasedTmp
				ITK_CALL(AOM_ask_value_date(taStatus[iIdx1], "date_released", &dateReleasedTmp) );

				// get status name
				if((*sLatestStatusName) != NULL)MEM_free(*sLatestStatusName);
				ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", sLatestStatusName) );
				*tLatestStatusTag = taStatus[0];
			} else if( iDateCompareResult == 0) {
				// Equal date -> compare status string
				char *sTmpStatusName = NULL;

				// Read tmp-status name
				ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", &sTmpStatusName) );

				// Normalize Status names
				std::string strLatestStatusName = std::string(*sLatestStatusName);
				std::string strTmpStatusName = std::string(sTmpStatusName);

				if(strLatestStatusName.length() == 1){
					char sTmp[2];
					strcpy(sTmp, strLatestStatusName.c_str());
					strLatestStatusName.clear();
					strLatestStatusName.append("0");
					strLatestStatusName.append(sTmp);
				}
				if(strTmpStatusName.length() == 1){
					char sTmp[2];
					strcpy(sTmp, strTmpStatusName.c_str());
					strTmpStatusName.clear();
					strTmpStatusName.append("0");
					strTmpStatusName.append(sTmp);
				}

				// Compare Status names
				if(/*strcmp(sTmpStatusName, *sLatestStatusName) > 0*/ strcmp(strTmpStatusName.c_str(), strLatestStatusName.c_str()) > 0)
				{
					// get status name
					if((*sLatestStatusName) != NULL)MEM_free(*sLatestStatusName);
					ITK_CALL(AOM_UIF_ask_value(taStatus[iIdx1], "name", sLatestStatusName) );					
					*tLatestStatusTag = taStatus[0];
				}

				// Free
				if(sTmpStatusName != NULL)MEM_free(sTmpStatusName);
			}
		}
	}

	// Free	
	if(taStatus != NULL)MEM_free(taStatus);

	return iRetValue;
}

int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount) {
	int		iRetValue		= ITK_ok;
	tag_t	tLatestStatus	= NULLTAG;

	// Ask latest status
	askLatestReleaseStatus(tObject, sLatestStatusName, iCompleteStatusCount, &tLatestStatus);

	return iRetValue;
}

int CLOCK_compare_dates(const date_t *date1, const date_t *date2)
{
	if ( date1->year != date2->year )
	{
		return date1->year > date2->year ? 1 : -1;
	}

	if ( date1->month != date2->month )
	{
		return date1->month > date2->month ? 1 : -1;
	}

	if ( date1->day != date2->day )
	{
		return date1->day > date2->day ? 1 : -1;
	}

	if ( date1->hour != date2->hour )   
	{
		return date1->hour > date2->hour ? 1 : -1;
	}

	if ( date1->minute != date2->minute )
	{
		return date1->minute > date2->minute ? 1 : -1;
	}

	if ( date1->second != date2->second )
	{
		return date1->second > date2->second ? 1 : -1;
	}

	return 0;
}
