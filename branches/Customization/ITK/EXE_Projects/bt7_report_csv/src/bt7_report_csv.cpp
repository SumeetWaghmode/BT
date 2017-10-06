#import <MSXML6.dll>

#include <ctype.h>
#include <atlbase.h>
#include <msxml2.h>
#include <cstdio>
#include <cfloat>
#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
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
#include <tc/folder.h>
#include <tc/envelope.h>
#include <tccore/aom.h>
#include <tccore/project.h>
#include <tccore/aom_prop.h>
#include <property/nr.h>
#include <qry/qry.h>
#include <tccore/grm.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tccore/uom.h>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <user_exits\user_exits.h>

// --------------------------------------------------------------------------

#define ITK_CALL(x){														\
    int stat;																\
    char *err_string;														\
    if((stat =(x))!= ITK_ok)												\
    {																		\
    EMH_get_error_string(NULLTAG, stat, &err_string);						\
    printf(" > ERROR: %d ERROR MSG: %s.\n", stat, err_string);				\
    printf(" >  FUNCTION: %s\nFILE: %s LINE: %d\n", #x, __FILE__, __LINE__);\
    if(err_string)MEM_free(err_string);										\
    exit(EXIT_FAILURE);														\
    }																		\
}

// --------------------------------------------------------------------------

using namespace std;
// Removed namespace - This is must for autobuild and windows 2012 server machines. windows server 2012 does not support msxml
// header files.

// --------------------------------------------------------------------------

logical isDerivedFromClass(char *className, tag_t tObjectTag);
int generateReport_EKKBHK(tag_t tCommonComp, tag_t tCommonCompRevision);
int generateReport_ZBHK(tag_t tCommonComp, tag_t tCommonCompRevision);
int generateReport_ZBHKT(tag_t tCommonComp, tag_t tCommonCompRevision);
int generateReport_DXDATABOOK(tag_t tCommonComp, tag_t tCommonCompRevision);
int findCommonCompItemRevisions(int *iResultCount, tag_t **taResults);
int findCommonCompItems(int *iResultCount, tag_t **taResults);
int executeQuery(int *iResultCount, tag_t **taResults, char *sQueryArgument);
int getManufacturer(tag_t tVendorPartItem, tag_t *tManufacturer);
int getVendorParts(tag_t tCommonCompRevision, int *iVendorPartCount, tag_t **taVendorPartObjects);
std::string getVendorPartNumberFromVendorPart(tag_t tVendorPart, tag_t tManufacturer);
std::string getItemID(tag_t tItem);
std::string getTcObjectName(tag_t tTcObject);
tag_t getSupplierSpecificationObject(tag_t tCommonCompRevision);
std::string getSupplierSpecificationItemID(tag_t tCommonCompRevision);
std::string getSupplierSpecificationItemRevName(tag_t tCommonCompRevision);
std::string getTcStringProp(tag_t tTcObject, char *sTcStringPropName);
std::string getTcStringProp_DisplayValue(tag_t tTcObject, char *sTcStringPropName);
std::string getTcLogicalProp(tag_t tTcObject, char *sTcStringPropName);
std::string getTcLogicalProp2(tag_t tTcObject, char *sTcStringPropName);
std::string getTcDateProp(tag_t tTcObject, char *sTcDatePropName, char *sDateFormat);
tag_t getTcObjectProp(tag_t tTcObject, char *sTcObjectPropName);
tag_t getLatestRevision(tag_t tTcObject);
std::string getClassificationaPath(tag_t tCommonCompRevision);
std::string getClassificationDetails(tag_t tCommonComp, int iModus);
std::string getClassificationAttributesForDXDatabook(tag_t tCommonComp);
void readECAD_ConfigFile(char *sPathToECAD_Config);
std::string prepareForCsv(std::string sText);
std::string prepareForCsv(std::string sText, int iMaxLength);
logical hasStatus(tag_t tObject, char *sStatusName);
tag_t getLatestReleasedRevision(tag_t tTcObject);
std::string adjustYesNoValues(std::string sStringValue);

// --------------------------------------------------------------------------

struct ECAD_CONFIG_ATTRIBUTE
{
	std::string sTcUAID;
	std::string sCadimName;
	std::string sFormat;
	double dFactor;
};

struct ECAD_CONFIG_CLASS
{
	std::string sClassname;
	vector<ECAD_CONFIG_ATTRIBUTE> vecEcadConfigAttributes;
};


vector<ECAD_CONFIG_CLASS> vecEcadConfigClasses;
ofstream	csvReportFile;
ifstream	xmlReportFile;

#define STATUS_TYPE_60	"Serial Release"
#define STATUS_TYPE_10	"Pre Release"

// --------------------------------------------------------------------------

int ITK_user_main(int argc , char* argv[]) {
	logical	lInputError			= false;
	logical	lUseInputFile		= false;
    int		retcode				= ITK_ok;
    char	*user_id			= NULL;
    char	*user_passwd		= NULL;
    char	*user_group			= NULL;
	char	*output_file		= NULL;
	char	*report_type		= NULL;
	char	*ecad_config		= NULL;

	/* Gets the user ID */
    user_id = ITK_ask_cli_argument("-u=");

    /* Gets the user Password */
    user_passwd = ITK_ask_cli_argument("-p=");

    /* Gets the user group */
    user_group = ITK_ask_cli_argument("-g=");

	/* Gets the report type */
    report_type = ITK_ask_cli_argument("-report_type=");

	/* Gets the ecad config */
    ecad_config = ITK_ask_cli_argument("-ecad_config=");

	/* Gets the file path */
    output_file = ITK_ask_cli_argument("-output=");


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
	if(report_type == NULL || strcmp(report_type, "") == 0){
		printf(" > ERROR: Parameter '-report_type' should not be empty! \n");
		lInputError = true;
	}
	if(ecad_config == NULL || strcmp(ecad_config, "") == 0){
		printf(" > ERROR: Parameter '-ecad_config' should not be empty! \n");
		lInputError = true;
	}
	if(output_file == NULL || strcmp(output_file, "") == 0){
		printf(" > ERROR: Parameter '-output' should not be empty! \n");
		lInputError = true;
	}

	/* Check files: Output File */
	csvReportFile.open(output_file);
	if(csvReportFile.good() == false){
		printf(" > ERROR: Output file is not accessible! \n");
		lInputError = true;
	}

	/* Check files: XML File */
	xmlReportFile.open(ecad_config);
	if(xmlReportFile.good() == false){
		printf(" > ERROR: ECAD_Config.xml is not accessible! \n");
		lInputError = true;
	}
	xmlReportFile.close();

	/* Exit if parameters are not correct */
	if(lInputError == true) {
		return 9999;
	}

	/* Logging into Teamcenter */
	printf(" > Login to Teamenter (this might take a moment ...) \n");
	ITK_CALL ( ITK_init_module(user_id, user_passwd, user_group));
	ITK_CALL ( ITK_set_bypass(true));
	printf(" > Login successful \n");

	/* Find Common Components and print Header*/
	int		iResultCount	= 0;
	tag_t	*taResults		= NULL;
	if(strcmp(report_type, "ekk_bhk") == 0) {
		// Print Header to file
		csvReportFile << "\"Part-Nr\"\t"
					  << "\"Nomenclature\"\t"
					  << "\"Supplier Specification\"\t"
					  << "\"Technical Info\"\t"
					  << "\"ID\"\t"
					  << "\"Manufacturer\"\t"
					  << "\"Type at Manufact.\"\t"
					  << "\"Remark on Rel\"\t"
					  << "\"Origin Last Update\"\t"
					  << "\"RoHS compliant?\"\t"
					  << endl;
		retcode = findCommonCompItems(&iResultCount, &taResults);
	} else if(strcmp(report_type, "zbhkt") == 0) {
		// Issue 1045. ZBHK & ZBHKT - Column Header
		// Print Header to file
		csvReportFile << "\"Part-Nr\"\t"
					  << "\"Manufacturer ID\"\t"
					  << "\"Type at Manufact.\"\t"
					  << "\"Manufacturer\"\t"
					  << "\"Relation Index\"\t"
					  << "\"Remark on Rel\"\t"
					  << "\"Origin Last Update\"\t"
					  << endl;
		retcode = findCommonCompItems(&iResultCount, &taResults);
	} else if(strcmp(report_type, "dx_databook") == 0) {
		// Print Header to file
		csvReportFile << "%START SBT ECAD XML REPORT EXPORT CSV%" << endl;

		// Read ECAD DATA
		readECAD_ConfigFile(ecad_config);

		// Loop ECAD Config Classes
		//for(int iIdx1 = 0; iIdx1 < vecEcadConfigClasses.size(); iIdx1++){
		//	printf(" > %i. class name: %s \n", iIdx1 + 1, vecEcadConfigClasses.at(iIdx1).sClassname.c_str());

		//	// Loop ECAD Config Attributes
		//	for(int iIdx2 = 0; iIdx2 < vecEcadConfigClasses.at(iIdx1).vecEcadConfigAttributes.size(); iIdx2++){
		//		printf("   > %i. TcUAID :   %s \n", iIdx2 + 1, vecEcadConfigClasses.at(iIdx1).vecEcadConfigAttributes.at(iIdx2).sTcUAID.c_str());
		//		printf("   > %i. CadimName: %s \n", iIdx2 + 1, vecEcadConfigClasses.at(iIdx1).vecEcadConfigAttributes.at(iIdx2).sCadimName.c_str());
		//		printf("   > %i. Format   : %s \n", iIdx2 + 1, vecEcadConfigClasses.at(iIdx1).vecEcadConfigAttributes.at(iIdx2).sFormat.c_str());				
		//		printf("   > %i. Factor   : %s \n", iIdx2 + 1, vecEcadConfigClasses.at(iIdx1).vecEcadConfigAttributes.at(iIdx2).sFactor.c_str());				
		//	}
		//}

		retcode = findCommonCompItems(&iResultCount, &taResults);
	} else if(strcmp(report_type, "zbhk") == 0) {
		// Issue 1045. ZBHK & ZBHKT - Column Header
		// Print Header to file
		csvReportFile << "\"Part-Nr\"\t"
					  << "\"Nomenclature\"\t"
					  << "\"Classification path\"\t"
					  << "\"Technical Info\"\t"
					  << "\"Document Name\"\t"
					  << "\"Originator\"\t"
					  << "\"Technically OK?\"\t"
					  << "\"Sourcing OK?\"\t"
					  << "\"Preferred?\"\t"
					  << "\"Last Update\"\t"
					  << "\"User Remark\"\t"
					  << endl;
		retcode = findCommonCompItems(&iResultCount, &taResults);
	}

	/* Start traversing of the results*/ //iResultCount
	for(int iIdx1 = 0; iIdx1 < iResultCount; iIdx1++) {
		tag_t tCommonCompRevision	= NULLTAG;
		tag_t tCommonComp			= NULLTAG;

		
		
		if ( ((iIdx1 + 1) % 50 == 0) || ((iIdx1 + 1) == iResultCount) ) {
			printf(" > %i objects handled ... \n", iIdx1 + 1);
		}

		// Ignore non-A5Q IDs for dx_databook report
		if( strcmp(report_type, "dx_databook") == 0 )
		{
			char sCommonCompItem[ITEM_id_size_c + 1];
			ITK_CALL(ITEM_ask_id(taResults[iIdx1], sCommonCompItem));
			std::string strCommonCompItem = sCommonCompItem;

			if(strCommonCompItem.length() > 3)
			{
				if(strCommonCompItem.substr(0, 3).compare("A5Q") != 0)
				{
					continue;
				}
			}
		}

		// Init
		if(strcmp(report_type, "ekk_bhk") == 0 ||strcmp(report_type, "zbhkt") == 0 || strcmp(report_type, "dx_databook") == 0 || strcmp(report_type, "zbhk") == 0){
			tCommonComp = taResults[iIdx1];
			//ITK_CALL ( ITEM_ask_latest_rev ( tCommonComp, &tCommonCompRevision) );		
			tCommonCompRevision = getLatestReleasedRevision(tCommonComp);
		} else {
			printf(" > ERROR: Report Type '%s' not identified ! \n", report_type);
			retcode = 9999;
		}
		
		// Generate Report
		if(strcmp(report_type, "ekk_bhk") == 0) {
			retcode = generateReport_EKKBHK(tCommonComp, tCommonCompRevision);
		} else if(strcmp(report_type, "zbhk") == 0) {
			retcode = generateReport_ZBHK(tCommonComp, tCommonCompRevision);
		} else if(strcmp(report_type, "zbhkt") == 0) {
			retcode = generateReport_ZBHKT(tCommonComp, tCommonCompRevision);
		} else if(strcmp(report_type, "dx_databook") == 0) {
			retcode = generateReport_DXDATABOOK(tCommonComp, tCommonCompRevision);
		} else {
			printf(" > ERROR: Report Type '%s' not identified ! \n", report_type);
			retcode = 9999;
		}
	}

	/* Free */
	if(taResults != NULL) MEM_free(taResults);

	// Close File
	if (csvReportFile.is_open()) {
		csvReportFile.close();
	}

	printf(" > Logout \n");
	ITK_CALL ( ITK_exit_module(true));

    return retcode;
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

logical verifyLatestReleaseStatus(tag_t tItemRevision, char *sSearchForStatusName)
{
	logical verifyLatestReleaseStatus = false;

	// Get the status list
	int    iStatusCount = 0;
	tag_t *taStatusList = NULLTAG;
	ITK_CALL(WSOM_ask_release_status_list(tItemRevision, &iStatusCount, &taStatusList) );

	// Check the target statuses
	if ( iStatusCount > 0 )
	{
		int    iLastStatusIndex = 0;
		char * sStatusName      = NULL;
		date_t dateReleased;
		date_t dateReleasedTmp;

		// Get the last status
		ITK_CALL(AOM_ask_value_date(taStatusList[0], "date_released", &dateReleasedTmp) );

		for ( int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++ )
		{
			ITK_CALL(AOM_ask_value_date(taStatusList[iIdx2], "date_released", &dateReleased) );

			if ( CLOCK_compare_dates(&dateReleased, &dateReleasedTmp) > 0 )
			{
				ITK_CALL(AOM_ask_value_date(taStatusList[iIdx2], "date_released", &dateReleasedTmp) );
				iLastStatusIndex = iIdx2;
			}
		}

		// Check the status name and validate
		ITK_CALL(AOM_UIF_ask_value(taStatusList[iLastStatusIndex], "name", &sStatusName) );

		if(strcmp(sStatusName, sSearchForStatusName) == 0)
		{
			verifyLatestReleaseStatus = true;
		}

		// Free
		if(sStatusName != NULL) MEM_free(sStatusName);
	}

	return verifyLatestReleaseStatus;
}


int findCommonCompItems(int *iResultCount, tag_t **taResults){
	return executeQuery(iResultCount, taResults, "BT7_CommonComp");
}

int findCommonCompItemRevisions(int *iResultCount, tag_t **taResults){
	return executeQuery(iResultCount, taResults, "BT7_CommonCompRevision");
}

int executeQuery(int *iResultCount, tag_t **taResults, char *sQueryArgument){
	int iRetcode = ITK_ok;

	/* Find Common Components */
	tag_t tQryGeneral = NULLTAG;
	ITK_CALL ( QRY_find("General...", &tQryGeneral));

	if(tQryGeneral != NULLTAG){
		char	**saQryEntries = (char**) MEM_alloc(1 * sizeof(char*));//[]	= {"Type"};
		char	**saQryValues = (char**) MEM_alloc(1 * sizeof(char*));//[]	= {"BT7_CommonComp"};

		saQryEntries[0] = (char*) MEM_alloc(strlen("Type") + 1);
		strcpy(saQryEntries[0], "Type" );

		saQryValues[0] = (char*) MEM_alloc(strlen(sQueryArgument) + 1);
		strcpy(saQryValues[0], sQueryArgument );

		ITK_CALL ( QRY_execute(tQryGeneral, 1, saQryEntries, saQryValues, iResultCount, taResults));
		printf(" > Result count in Teamcenter = %i \n", *iResultCount);

		// Free
		if(saQryEntries != NULL) MEM_free(saQryEntries);
		if(saQryValues != NULL) MEM_free(saQryValues);
	} else {
		printf(" > ERROR: Query 'General...' not found! \n");
		iRetcode = 9999;
	}

	return iRetcode;
}

int generateReport_EKKBHK(tag_t tCommonComp, tag_t tCommonCompRevision){
	int		iRetcode				= ITK_ok;
	int		iVendorPartCount		= 0;
	char	*sDelimiter				= "\t";	
	tag_t	*taVendorParts			= NULL;
	
	// Get Vendor Parts
	iRetcode = getVendorParts(tCommonCompRevision, &iVendorPartCount, &taVendorParts);

	// Traverse Vendor Management Objects - Vendor Parts  - iVendorPartCount
	for(int iIdx1 = 0; iIdx1 < iVendorPartCount; iIdx1++) {
		tag_t tManufacturer = NULLTAG;

		//logical lIsActiveUser = false;
		logical lHasSupplierSpec = false;
		logical lManufacturerFound = false;
		logical lIsManufacturerLocked = false;
		logical lLatestStatusIs60 = false;
		logical lLatestStatusIs10 = false;		
		// Get Manufacturer
		getManufacturer(taVendorParts[iIdx1], &tManufacturer);

		// Is active user ?
		if(tManufacturer != NULLTAG)
		{
			tag_t tManufacturerRevision = NULLTAG;
			ITK_CALL(ITEM_ask_latest_rev(tManufacturer, &tManufacturerRevision));
			//ITK_CALL(AOM_ask_value_logical(tManufacturerRevision, "bt7_activeuser", &lIsActiveUser));
			lManufacturerFound = true;
		} else {
			lManufacturerFound = false;
		}

		// Commented out and flag set to true due to Issue 1044. EKK_BHK & ZBHKT - Supplier Document
		// Has Supplier Specification ?
		//lHasSupplierSpec = getSupplierSpecificationObject(tCommonCompRevision) != NULLTAG;
		lHasSupplierSpec = true;

		// Vendor Part Locked ?
		tag_t tVendorPartRevision = NULLTAG;
		ITK_CALL(ITEM_ask_latest_rev(taVendorParts[iIdx1], &tVendorPartRevision));
		ITK_CALL(AOM_ask_value_logical(tVendorPartRevision, "bt7_manulocked", &lIsManufacturerLocked));

		// Check Latest Status
		lLatestStatusIs60 = verifyLatestReleaseStatus(tCommonCompRevision, STATUS_TYPE_60);
		lLatestStatusIs10 = verifyLatestReleaseStatus(tCommonCompRevision, STATUS_TYPE_10);
		
		

		if(lHasSupplierSpec && lIsManufacturerLocked == false && (lLatestStatusIs10 || lLatestStatusIs60))
		{
			// Print line to CSV
			csvReportFile << "\"" << prepareForCsv(getItemID(tCommonComp), 25) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getTcObjectName(tCommonComp), 60) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getSupplierSpecificationItemID(tCommonCompRevision), 50) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getTcStringProp(tCommonCompRevision, "bt7_techinfo"), 160) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getItemID(tManufacturer), 30) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getTcObjectName(tManufacturer), 60) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getVendorPartNumberFromVendorPart(taVendorParts[iIdx1], tManufacturer), 60) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getTcStringProp(getLatestRevision(taVendorParts[iIdx1]), "bt7_remark"), 50) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(getTcDateProp(getLatestRevision(taVendorParts[iIdx1]), "last_mod_date", "%Y/%m/%d"), 10) << "\"" << sDelimiter
						  << "\"" << prepareForCsv(adjustYesNoValues(getTcStringProp(tCommonCompRevision, "bt7_rohs_compliant"))) << "\"" << sDelimiter
						  << endl;
		} else {
			//printf(" > INFO : Common Comp '%s' not added to EKK_BHK. \n", getItemID(tCommonComp).c_str());
			//printf("        > Status 60     : %i (should be = 1) \n", lLatestStatusIs60);
			//printf("        > Suppl. Spec.  : %i (should be = 1) \n", lHasSupplierSpec);
			//printf("        > Active user   : %i (should be = 1) \n", lIsActiveUser);
			//printf("        > Manuf. found  : %i (should be = 1) \n", lManufacturerFound);
			//printf("        > Manuf. locked : %i (should be = 0) \n", lIsManufacturerLocked);
		}
	}

	// Free
	if(taVendorParts != NULL) MEM_free(taVendorParts);

	return iRetcode;
}

int generateReport_ZBHK(tag_t tCommonComp, tag_t tCommonCompRevision){
	int		iRetcode				= ITK_ok;
	char	*sDelimiter				= "\t";	
	int		iVendorPartCount		= 0;
	tag_t	*taVendorParts			= NULL;

	//logical lIsLatestCommonCompRev = false;
	logical lLatestCommonCompRevHasStatusIs60 = false;
	logical lLatestCommonCompRevHasStatusIs10 = false;

	// Get Vendor Parts
	iRetcode = getVendorParts(tCommonCompRevision, &iVendorPartCount, &taVendorParts);

	//// is latest common comp rev ?
	//tag_t tTmpLatestCommonCompRev = NULLTAG;
	//ITK_CALL(ITEM_ask_latest_rev(tCommonComp, &tTmpLatestCommonCompRev));
	//if(tTmpLatestCommonCompRev == tCommonCompRevision)
	//{
	//	lIsLatestCommonCompRev = true;
	//}

	// Latest status = 60 ?
	lLatestCommonCompRevHasStatusIs60 = verifyLatestReleaseStatus(tCommonCompRevision, STATUS_TYPE_60);
	lLatestCommonCompRevHasStatusIs10 = verifyLatestReleaseStatus(tCommonCompRevision, STATUS_TYPE_10);

	if(/*lIsLatestCommonCompRev && */iVendorPartCount > 0 && (lLatestCommonCompRevHasStatusIs60 || lLatestCommonCompRevHasStatusIs10))
	{
		csvReportFile << prepareForCsv(getItemID(tCommonComp), 18) << sDelimiter
					  << prepareForCsv(getTcObjectName(tCommonCompRevision), 40) << sDelimiter
					  << prepareForCsv(getClassificationaPath(tCommonComp), 60) << sDelimiter
					  << prepareForCsv(getTcStringProp(tCommonCompRevision, "bt7_techinfo"), 60) << sDelimiter
					  << prepareForCsv(getSupplierSpecificationItemID(tCommonCompRevision), 18) << sDelimiter
					  << prepareForCsv(getTcStringProp(getTcObjectProp(getTcObjectProp(tCommonComp, "owning_user"), "person"), "user_name"), 10) << sDelimiter
					  << prepareForCsv(getTcLogicalProp(tCommonCompRevision, "bt7_techok"), 1) << sDelimiter
					  << prepareForCsv(getTcLogicalProp(tCommonCompRevision, "bt7_sourceok"), 1) << sDelimiter
					  << prepareForCsv(getTcLogicalProp(tCommonCompRevision, "bt7_preferred"), 1) << sDelimiter
					  << prepareForCsv(getTcDateProp(tCommonCompRevision, "last_mod_date", "%Y%m%d"), 8) << sDelimiter
					  << prepareForCsv(getTcStringProp(tCommonCompRevision, "bt7_remark"), 160) << sDelimiter
					  << endl;
	}

	// Free
	if(taVendorParts != NULL) MEM_free(taVendorParts);

	return iRetcode;
}

int generateReport_ZBHKT(tag_t tCommonComp, tag_t tCommonCompRevision){
	int		iRetcode				= ITK_ok;
	int		iVendorPartCount		= 0;
	char	*sDelimiter				= "\t";	
	tag_t	*taVendorParts			= NULL;
	int		iValidVendortPartCounter= 0;

	// Get Vendor Parts
	iRetcode = getVendorParts(tCommonCompRevision, &iVendorPartCount, &taVendorParts);

	// Traverse Vendor Management Objects - Vendor Parts
	for(int iIdx1 = 0; iIdx1 < iVendorPartCount; iIdx1++) {
		//logical lIsActiveUser = false;
		logical lHasSupplierSpec = false;
		logical lManufacturerFound = false;
		logical lIsManufacturerLocked = false;
		logical lLatestStatusIs60 = false;
		logical lLatestStatusIs10 = false;
		tag_t tManufacturer = NULLTAG;

		// Get Manufacturer
		getManufacturer(taVendorParts[iIdx1], &tManufacturer);

		// Is active user ?
		if(tManufacturer != NULLTAG)
		{
			tag_t tManufacturerRevision = NULLTAG;
			ITK_CALL(ITEM_ask_latest_rev(tManufacturer, &tManufacturerRevision));
			//ITK_CALL(AOM_ask_value_logical(tManufacturerRevision, "bt7_activeuser", &lIsActiveUser));
			lManufacturerFound = true;
		} else {
			lManufacturerFound = false;
		}

		// Commented out and flag set to true due to Issue 1044. EKK_BHK & ZBHKT - Supplier Document
		// Has Supplier Specification ?
		//lHasSupplierSpec = getSupplierSpecificationObject(tCommonCompRevision) != NULLTAG;
		lHasSupplierSpec = true;

		// Vendor Part Locked ?
		tag_t tVendorPartRevision = NULLTAG;
		ITK_CALL(ITEM_ask_latest_rev(taVendorParts[iIdx1], &tVendorPartRevision));
		ITK_CALL(AOM_ask_value_logical(tVendorPartRevision, "bt7_manulocked", &lIsManufacturerLocked));

		// Latest status = 60 ?
		lLatestStatusIs60 = verifyLatestReleaseStatus(tCommonCompRevision, STATUS_TYPE_60);
		lLatestStatusIs10 = verifyLatestReleaseStatus(tCommonCompRevision, STATUS_TYPE_10);

		if(lHasSupplierSpec && lIsManufacturerLocked == false && (lLatestStatusIs60 || lLatestStatusIs10))
		{
			csvReportFile << prepareForCsv(getItemID(tCommonComp), 18) << sDelimiter
						  << prepareForCsv(getItemID(tManufacturer), 8) << sDelimiter
						  << prepareForCsv(getVendorPartNumberFromVendorPart(taVendorParts[iIdx1], tManufacturer), 60) << sDelimiter
						  << prepareForCsv(getTcStringProp(tManufacturer, "object_name"), 35) << sDelimiter /* << getTcObjectName(tManufacturer) << sDelimiter */
/* TODO: SAP range : 0-9*/	<< iValidVendortPartCounter + 1 << sDelimiter // Index of the VendorParts (old: l0RelIndex)
						  << prepareForCsv(getTcStringProp(getLatestRevision(taVendorParts[iIdx1]), "bt7_remark"), 60) << sDelimiter
						  << prepareForCsv(getTcDateProp(getLatestRevision(taVendorParts[iIdx1]), "last_mod_date", "%Y%m%d"), 8) << sDelimiter
						  << endl;

			iValidVendortPartCounter++;
		} else {
			//printf(" > INFO : Common Comp '%s' not added to ZBHKT. \n", getItemID(tCommonComp).c_str());
			//printf("        > Status 60     : %i (should be = 1) \n", lLatestStatusIs60);
			//printf("        > Suppl. Spec.  : %i (should be = 1) \n", lHasSupplierSpec);
			//printf("        > Active user   : %i (should be = 1) \n", lIsActiveUser);
			//printf("        > Manuf. found  : %i (should be = 1) \n", lManufacturerFound);
			//printf("        > Manuf. locked : %i (should be = 0) \n", lIsManufacturerLocked);
		}
	}

	// Free
	if(taVendorParts != NULL) MEM_free(taVendorParts);

	return iRetcode;
}

int generateReport_DXDATABOOK(tag_t tCommonComp, tag_t tCommonCompRevision){
	int iRetcode = ITK_ok;
	tag_t tClassificationObject = NULLTAG;

	// Is Classified ?
	ITK_CALL ( ICS_ask_classification_object(tCommonComp, &tClassificationObject));

	// Only print the result if the current common component is classified
	if(tClassificationObject != NULLTAG)
	{
		csvReportFile << "{" << endl
					  << getItemID(tCommonComp) << endl
					  << getClassificationDetails(tCommonComp, 1) << endl
					  << getClassificationDetails(tCommonComp, 2) << endl
					  << getClassificationAttributesForDXDatabook(tCommonComp)
					  << "Status=" << getTcStringProp_DisplayValue(tCommonCompRevision, "bt7_validation") << endl
					  // Issue 1109. DxDatabook: In DX Databook, "Part Name" should be filled with the attribute
					  //             "Generic Type" of the "Common Component Revision".
					  << "Part Name=" << getTcStringProp_DisplayValue(tCommonCompRevision, "bt7_GenericType") << endl
					  << "}" << endl;
	}

	return iRetcode;
}

tag_t getLatestReleasedRevision(tag_t tTcObject)
{
	tag_t	tItem						= NULLTAG;
	tag_t	tLatestReleasedRevision		= NULLTAG;
	int		iRevCount					= 0;
	tag_t	*taRevs						= NULL;

	// Get Item
	if(isDerivedFromClass("Item", tTcObject)){
		tItem = tTcObject;
	} else if (isDerivedFromClass("ItemRevision", tTcObject)){
		ITK_CALL(ITEM_ask_item_of_rev(tTcObject, &tItem));
	} else {
		printf(" > ERROR - getLatestReleasedRevision -> Object Type not Item or ItemRevision \n");
		return NULLTAG;
	}

	// Get all Revisions
	ITK_CALL(ITEM_list_all_revs(tItem, &iRevCount, &taRevs));

	// Loop Revisions backwards
	for(int iIdx1 = (iRevCount - 1); iIdx1 >= 0; iIdx1--)
	{
		// Has Status 10 or 60 ?
		logical lHasStatus10or60 = hasStatus(taRevs[iIdx1], STATUS_TYPE_10) || hasStatus(taRevs[iIdx1], STATUS_TYPE_60);

		if(lHasStatus10or60 == true)
		{
			tLatestReleasedRevision = taRevs[iIdx1];
			break;
		}
	}

	// Latest Relased Revision found ?
	if(tLatestReleasedRevision == NULLTAG)
	{
		// Get latest revision (independet of the status)
		ITK_CALL(ITEM_ask_latest_rev(tItem, &tLatestReleasedRevision));
	}

	// Free
	if(taRevs != NULL)MEM_free(taRevs);

	return tLatestReleasedRevision;
}

tag_t getLatestRevision(tag_t tTcObject)
{
	tag_t tLatestRevision = NULLTAG;

	// Get Item
	if(isDerivedFromClass("Item", tTcObject)){
		ITK_CALL(ITEM_ask_latest_rev(tTcObject, &tLatestRevision));
	} else if (isDerivedFromClass("ItemRevision", tTcObject)){
		tag_t tTmpItem = NULLTAG;
		ITK_CALL(ITEM_ask_item_of_rev(tTcObject, &tTmpItem));
		ITK_CALL(ITEM_ask_latest_rev(tTmpItem, &tLatestRevision));
	} else {
		char sObjectType[WSO_name_size_c+1];
		ITK_CALL(WSOM_ask_object_type(tTcObject, sObjectType));
		printf(" > ERROR: Identification of Latest Revision not possible for the type '%s'  \n", sObjectType);
	}

	return tLatestRevision;
}

std::string getSupplierSpecificationItemRevName(tag_t tCommonCompRevision){
	std::string	strSupplierSpecItemRevName = "---";

	// Get Supplier Spec Item Object
	tag_t tSupplierSpec = getSupplierSpecificationObject(tCommonCompRevision);
	
	if(tSupplierSpec != NULLTAG){
		// Get Item Revision
		if(isDerivedFromClass("Item", tSupplierSpec)){
			tag_t tTmpSupplierSpecItemRevision = NULLTAG;
			ITK_CALL(ITEM_ask_latest_rev(tSupplierSpec, &tTmpSupplierSpecItemRevision));
			strSupplierSpecItemRevName = getTcObjectName(tTmpSupplierSpecItemRevision);
		} else if (isDerivedFromClass("ItemRevision", tSupplierSpec)){
			strSupplierSpecItemRevName = getTcObjectName(tSupplierSpec);
		}
	}

	return strSupplierSpecItemRevName;
}

std::string getSupplierSpecificationItemID(tag_t tCommonCompRevision){
	std::string	strSupplierSpecID	= "---";

	// Get Supplier Spec Item Object
	tag_t tSupplierSpec = getSupplierSpecificationObject(tCommonCompRevision);

	if(tSupplierSpec != NULLTAG){
		// Get Item
		if(isDerivedFromClass("Item", tSupplierSpec)){
			strSupplierSpecID = getItemID(tSupplierSpec);
		} else if (isDerivedFromClass("ItemRevision", tSupplierSpec)){
			tag_t tTmpSupplierSpecItem = NULLTAG;
			ITK_CALL(ITEM_ask_item_of_rev(tSupplierSpec, &tTmpSupplierSpecItem));
			strSupplierSpecID = getItemID(tTmpSupplierSpecItem);
		}
	}

	return strSupplierSpecID;
}

tag_t getSupplierSpecificationObject(tag_t tCommonCompRevision){
	int			iSupplierspecCount	= 0;
	tag_t		tRelSupplierSpec	= NULLTAG;
	tag_t		*taSupplierSpecs	= NULL;
	tag_t		tSupplierSpecObject	= NULLTAG;

	// Get Relation Types
	ITK_CALL ( GRM_find_relation_type ( "BT7_SUPPLIERSPEC", &tRelSupplierSpec ) );

	// Get Vendor Parts
	ITK_CALL ( GRM_list_secondary_objects_only ( tCommonCompRevision, tRelSupplierSpec, &iSupplierspecCount, &taSupplierSpecs) );

	// There should be exactly one Supplier Spec related to the Commercial Part Revision
	if(iSupplierspecCount == 1){
		char sObjectType[WSO_name_size_c+1];
		tag_t tTmpRevision = NULLTAG;

		// Get BT7_DocumentRevision
		ITK_CALL(WSOM_ask_object_type(taSupplierSpecs[0], sObjectType));
		if(strcmp(sObjectType, "BT7_Document") == 0)
		{
			ITK_CALL(ITEM_ask_latest_rev(taSupplierSpecs[0], &tTmpRevision));
		} else if(strcmp(sObjectType, "BT7_DocumentRevision") == 0) {
			tTmpRevision = taSupplierSpecs[0];
		} else {
			tTmpRevision = NULLTAG;
		}
		//changed the attribute name from 'bt7_entdoc_type' from 'bt7_documentClass' 
		if(tTmpRevision != NULLTAG)
		{
			char *sEntDocType = NULL;
			ITK_CALL(AOM_ask_value_string(tTmpRevision, "bt7_documentClass", &sEntDocType));
			if(strcmp(sEntDocType, "dclass_manu") == 0)
			{
				tSupplierSpecObject = taSupplierSpecs[0];
			}
		}
	} else {
		tag_t tCommonComp = NULLTAG;
		ITK_CALL(ITEM_ask_item_of_rev(tCommonCompRevision, &tCommonComp));
		//printf(" > ERROR: SupplierSpec count = '%i' for Common Component '%s' \n", iSupplierspecCount, getItemID(tCommonComp).c_str());
	}

	// Free
	if(taSupplierSpecs != NULL) MEM_free(taSupplierSpecs);

	return tSupplierSpecObject;
}

std::string getItemID(tag_t tItem){
	char	sItemID[ITEM_id_size_c + 1];

	if(tItem != NULLTAG)
	{
		ITK_CALL ( ITEM_ask_id(tItem, sItemID) );
	} else {
		printf(" > ERROR: Invalid ItemTag \n");
	}

	return std::string(sItemID);
}

std::string getTcObjectName(tag_t tTcObject){
    char	sTcObjectName[WSO_name_size_c + 1];

	if(tTcObject != NULLTAG)
	{
		ITK_CALL ( WSOM_ask_name(tTcObject, sTcObjectName) );
	} else {
		printf(" > ERROR: Invalid tTcObject \n");
	}

	return std::string(sTcObjectName);
}

std::string getTcLogicalProp(tag_t tTcObject, char *sTcStringPropName){
	logical lTcLogicalPropValue = true;

	ITK_CALL(AOM_ask_value_logical(tTcObject, sTcStringPropName, &lTcLogicalPropValue));

	return lTcLogicalPropValue ? std::string("Y") : std::string("N");
}

std::string getTcLogicalProp2(tag_t tTcObject, char *sTcStringPropName){
	logical lTcLogicalPropValue = true;

	ITK_CALL(AOM_ask_value_logical(tTcObject, sTcStringPropName, &lTcLogicalPropValue));

	return lTcLogicalPropValue ? std::string("YES") : std::string("NO");
}

tag_t getTcObjectProp(tag_t tTcObject, char *sTcObjectPropName){
	tag_t tTcObjectPropValue = NULLTAG;
	ITK_CALL(AOM_ask_value_tag(tTcObject, sTcObjectPropName, &tTcObjectPropValue));
	return tTcObjectPropValue;
}

std::string getTcStringProp_DisplayValue(tag_t tTcObject, char *sTcStringPropName){
	char		*sTcStringPropValue		= NULL;
	std::string	strTcStringPropValue	= "";

	// Get Property
	//printf("Read Property '%s' \n", sTcStringPropName);
	ITK_CALL (AOM_UIF_ask_value(tTcObject, sTcStringPropName, &sTcStringPropValue));
	strTcStringPropValue = sTcStringPropValue;

	// Free
	if(sTcStringPropValue != NULL) MEM_free(sTcStringPropValue);

	return strTcStringPropValue;
}

std::string adjustYesNoValues(std::string sStringValue){
	char *sReturnValue = "";

	if(sStringValue.compare("Yes") == 0) {
		sReturnValue = "YES";
	} else {
		sReturnValue = "NO";
	}

	return sReturnValue;
}

std::string getTcStringProp(tag_t tTcObject, char *sTcStringPropName){
	char		*sTcStringPropValue		= NULL;
	std::string	strTcStringPropValue	= "";

	// Get Property
	//printf("Read Property '%s' \n", sTcStringPropName);
	ITK_CALL (AOM_ask_value_string(tTcObject, sTcStringPropName, &sTcStringPropValue) );
	strTcStringPropValue = sTcStringPropValue;

	// Issue 1046. ZBHK Report remove carriage return.
	std::size_t start_pos = 0;
	while((start_pos = strTcStringPropValue.find("\r\n", start_pos)) != std::string::npos)
	{
		strTcStringPropValue.replace(start_pos, 2, " ");
		//cout<<"\\r\\n found: " << strTcStringPropValue << endl;
	}
	start_pos = 0;
	while((start_pos = strTcStringPropValue.find("\r", start_pos)) != std::string::npos)
	{
		strTcStringPropValue.replace(start_pos, 1, " ");
		//cout<<"\\r found: " << strTcStringPropValue << endl;
	}
	start_pos = 0;
	while((start_pos = strTcStringPropValue.find("\n", start_pos)) != std::string::npos)
	{
		strTcStringPropValue.replace(start_pos, 1, " ");
		//cout<<"\\n found: " << strTcStringPropValue << endl;
	}

	// Free
	if(sTcStringPropValue != NULL) MEM_free(sTcStringPropValue);

	return strTcStringPropValue;
}

std::string getTcDateProp(tag_t tTcObject, char *sTcDatePropName, char *sDateFormat){
	date_t		dDateValue	= NULLDATE;
	char			*sDate	= NULL;
	std::string		strDate	= "";

	// Get formatted date
	ITK_CALL(AOM_ask_value_date(tTcObject, sTcDatePropName, &dDateValue));
	ITK_CALL(DATE_date_to_string(dDateValue, sDateFormat, &sDate));
	strDate = sDate;

	// Free
	if(sDate != NULL) MEM_free(sDate);

	return strDate;
}

std::string getVendorPartNumberFromVendorPart(tag_t tVendorPart, tag_t tManufacturer) {
	int	iStatus	= ITK_ok;
	std::string strVendorPartNumber = "";

	if(tManufacturer != NULLTAG && tVendorPart != NULLTAG)
	{
		std::string	strManufacturerID	= getItemID(tManufacturer);
		strVendorPartNumber = getItemID(tVendorPart);

		// Manufacturer ID found?
		if(strManufacturerID.compare("") != 0){
			std::size_t found = strVendorPartNumber.find(strManufacturerID);
			if(found != std::string::npos){
				// Vendor Part ItemID is build by the ootb Tc Vendor Management as follows:
				//  <VendorPartNr>-<ManufacturerID>
				// Means: We need to cut the "-" and the <ManufacturerID> from the VendorPart ItemID to get the Vendor Part Number
				strVendorPartNumber = strVendorPartNumber.substr(0, found - 1);
			}
		}
	} else {
		printf(" > ERROR: Invalid VendorPart or ManufacturerPart! \n");
	}

	return strVendorPartNumber;
}

int getVendorParts(tag_t tCommonCompRevision, int *iVendorPartCount, tag_t **taVendorPartObjects){
	int		iRetcode			= 0;
	tag_t	tRelVMRepresents	= NULLTAG;

	// Get Relation Types
	ITK_CALL ( GRM_find_relation_type ( "VMRepresents", &tRelVMRepresents ) );

	// Get Vendor Parts
	ITK_CALL ( GRM_list_secondary_objects_only ( tCommonCompRevision, tRelVMRepresents, iVendorPartCount, taVendorPartObjects ) );
	
	return iRetcode;
}

// Teamcenter Vendor Management prevents the user from attaching more than one Manufacturer to a Vendor Part
int getManufacturer(tag_t tVendorPartItem, tag_t *tManufacturer){
	int		iRetcode			= 0;
	int		iManufacturerCount	= 0;
	tag_t	tRelVendorPartRel	= NULLTAG;
	tag_t	*taManufacturers	= NULL;

	// Get Relation Types
	ITK_CALL ( GRM_find_relation_type ( "TC_vendor_part_rel", &tRelVendorPartRel ) );

	// Get Manufacturers
	ITK_CALL ( GRM_list_secondary_objects_only ( tVendorPartItem, tRelVendorPartRel, &iManufacturerCount, &taManufacturers) );
	
	if(iManufacturerCount == 1){
		*tManufacturer = taManufacturers[0];
	} else {
		//printf(" > ERROR: Manufacturer not found for Vendor Part '%s' \n", getItemID(tVendorPartItem).c_str());
	}

	// Free
	if(taManufacturers != NULL) MEM_free(taManufacturers);

	return iRetcode;
}

std::string from_variant(VARIANT& vt) {
	_bstr_t bs(vt);
	return std::string(static_cast<const char*>(bs));
}

void readECAD_ConfigFile(char *sPathToECAD_Config){
    HRESULT hr = CoInitialize(NULL); 
	if (SUCCEEDED(hr)){
		MSXML2::IXMLDOMDocument2Ptr xmlDoc;
		//hr = xmlDoc.CreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
		xmlDoc.CreateInstance(__uuidof(DOMDocument30));

		if (xmlDoc->load(_T(sPathToECAD_Config)) != VARIANT_TRUE) {
			printf(" > Unable to load '%s' \n", sPathToECAD_Config);
		} else {
			printf(" > XML successfully loaded\n");

			// Get all instances of "ClassAttribConfig"
			MSXML2::IXMLDOMNodeListPtr nodeClassAttribConfigs = xmlDoc->getElementsByTagName("ClassAttribConfig");
			for(int iIdx1 = 0; iIdx1 < nodeClassAttribConfigs->Getlength(); iIdx1++) {
				// create class-object
				ECAD_CONFIG_CLASS ecad_config_class;
				vector<ECAD_CONFIG_ATTRIBUTE> vecTmpAttributes;

				// Get classname
				MSXML2::IXMLDOMNodePtr pNodeClassAttrConf = nodeClassAttribConfigs->Getitem(iIdx1);		
				MSXML2::IXMLDOMNamedNodeMapPtr pNodeClassAttributes = pNodeClassAttrConf->Getattributes();
				MSXML2::IXMLDOMNodePtr pNodeClassAtt = pNodeClassAttributes->getNamedItem("classname");
				if(pNodeClassAtt != NULL) {
					_variant_t nodeValue = pNodeClassAtt->GetnodeValue();
					std::string sValue = from_variant(nodeValue);

					// Assign classname to struct
					ecad_config_class.sClassname = sValue;
				}

				// Traverse child nodes of "ClassAttribConfig"
				MSXML2::IXMLDOMNodeListPtr nodeAttributes = pNodeClassAttrConf->GetchildNodes();
				for(int iIdx2 = 0; iIdx2 < nodeAttributes->Getlength(); iIdx2++) {
					// create attribute-object
					ECAD_CONFIG_ATTRIBUTE ecad_config_attribute;

					MSXML2::IXMLDOMNodePtr nodeAttribute = nodeAttributes->Getitem(iIdx2);		
					MSXML2::IXMLDOMNamedNodeMapPtr nodeAttributeAttributes = nodeAttribute->Getattributes();
					
					// Get value for "TCUAName"
					MSXML2::IXMLDOMNodePtr pNodeAttrTCUAName = nodeAttributeAttributes->getNamedItem("TcUAID");
					if(pNodeAttrTCUAName != NULL) {
						_variant_t nodeValue = pNodeAttrTCUAName->GetnodeValue();
						std::string sValue = from_variant(nodeValue);

						//Assign
						ecad_config_attribute.sTcUAID = sValue;
					}

					// Get value for "CadimName"
					MSXML2::IXMLDOMNodePtr pNodeAttrCadimName = nodeAttributeAttributes->getNamedItem("CadimName");
					if(pNodeAttrCadimName != NULL) {
						_variant_t nodeValue = pNodeAttrCadimName->GetnodeValue();
						std::string sValue = from_variant(nodeValue);

						//Assign
						ecad_config_attribute.sCadimName = sValue;
					}

					// Get value for "Format"
					MSXML2::IXMLDOMNodePtr pNodeAttrFormat = nodeAttributeAttributes->getNamedItem("Format");
					if(pNodeAttrFormat != NULL) {
						_variant_t nodeValue = pNodeAttrFormat->GetnodeValue();
						std::string sValue = from_variant(nodeValue);

						//Assign
						ecad_config_attribute.sFormat = sValue;
					}

					// Get value for "Format"
					MSXML2::IXMLDOMNodePtr pNodeAttrFactor = nodeAttributeAttributes->getNamedItem("Factor");
					if(pNodeAttrFactor != NULL) {
						_variant_t nodeValue = pNodeAttrFactor->GetnodeValue();
						std::string sValue = from_variant(nodeValue);

						//Assign
						ecad_config_attribute.dFactor = atof(sValue.c_str());
					} else {
						//Assign
						ecad_config_attribute.dFactor = 1.0;
					}

					// Add to Vector
					vecTmpAttributes.push_back(ecad_config_attribute);
				}

				// Assign tmp Vector to struct
				ecad_config_class.vecEcadConfigAttributes = vecTmpAttributes;

				// Add class object to vector
				vecEcadConfigClasses.push_back(ecad_config_class);
			}
		}
	}
}

std::string trimTrailingZerosFromNumberString(std::string sString){
	if(sString.length() > 0){
		if(sString.substr(sString.length() - 1, sString.length()).compare("0") == 0){
			sString = sString.substr(0, sString.length() - 1);
			sString = trimTrailingZerosFromNumberString(sString);
		} else if(sString.substr(sString.length() - 1, sString.length()).compare(".") == 0){
			sString = sString.substr(0, sString.length() - 1);
		}
	}

	return sString;
}

std::string trimAndFormatString(std::string sString){
	if(sString.length() > 0){

		std::size_t iNoZeroPos = sString.find_first_not_of('0');
		std::size_t iDotPos = sString.find_first_of('.');

		// All zeros
		if ( iNoZeroPos == std::string::npos )
		{
			return "0";
		}
		// erase leading zeros
		else
		{
			sString.erase(0, iNoZeroPos);
		}

		// Add "0" for ".xxxx" to get "0.xxxxx"
		if ( iDotPos != std::string::npos && iDotPos == iNoZeroPos )
		{
			sString = "0" + sString;
		}

		// erase trailing zeros
		std::size_t iLastNoZeroPos = sString.find_last_not_of('0');
		iDotPos = sString.find_first_of('.');

		if (iLastNoZeroPos != std::string::npos && iLastNoZeroPos != (sString.size()-1) && iDotPos != std::string::npos)
		{
			if ( iDotPos == iLastNoZeroPos )
			{
				sString.erase(iLastNoZeroPos);
			}
			else if ( iDotPos < iLastNoZeroPos )
			{
				sString.erase(iLastNoZeroPos+1);
			}
		}

	}

	return sString;
}

std::string getClassificationAttributesForDXDatabook(tag_t tCommonComp){
	std::string strClassificationString = "";
	
	std::stringstream ssClassificationAttributes;
	std::stringstream ssClassificationValues;

	tag_t		tClassificationObject	= NULLTAG;
	
	// Get Classification Info
	ITK_CALL ( ICS_ask_classification_object(tCommonComp, &tClassificationObject));
	if(tClassificationObject != NULLTAG){
		int		iAttributeCount			= 0;
		char	**saAttributeNames		= NULL;
		char	**saAttributeValues		= NULL;
		tag_t	tClassTag				= NULLTAG;
		
		// Get classification class id and class tag
		char *sClassID = NULL;
		char *sViewID = NULL;
		ITK_CALL ( ICS_ask_cid_sid_of_classification_obj(tClassificationObject, &sClassID, &sViewID));
		ITK_CALL ( ICS_ask_class_of_classification_obj(tClassificationObject, &tClassTag));

		// Get classification object attributes
		//ITK_CALL(ICS_ask_attributes_of_classification_obj(tClassificationObject, &iAttributeCount, &saAttributeNames, &saAttributeValues));

		// Get Attribute Vector for current Classification Class
		vector<ECAD_CONFIG_ATTRIBUTE> vecAttributeVector;
		for(int iIdx1 = 0; iIdx1 < vecEcadConfigClasses.size(); iIdx1++){
			if(vecEcadConfigClasses.at(iIdx1).sClassname.compare(sClassID) == 0){
				vecAttributeVector = vecEcadConfigClasses.at(iIdx1).vecEcadConfigAttributes;
				//printf(" > Vector found for %s \n", sClassID);
				break;
			}
		}



		tag_t view;
		int           nAttributes;             /**< (O) Number of attributes for the classification object */
		int*          unctNumbers;             /**< (OF) nAttributes UNCT numbers of the attributes */
		int*          unctFormats;             /**< (OF) nAttributes Formats of the attributes */
		char**        attributeNames;          /**< (OF) nAttributes Attribute names */
		char**        attributeValues;         /**< (OF) nAttributes Attribute values */
		char**        units;
		ITK_CALL(ICS_describe_classification_object(tClassificationObject, &view,&nAttributes,&unctNumbers,&unctFormats,&attributeNames,&attributeValues,&units));
		for(int iIdx1 = 0; iIdx1 < nAttributes; iIdx1++){
			string strTmpAttributeValue = attributeValues[iIdx1];

			//printf("\n");
			//printf("  > unctNumbers     : %i \n", unctNumbers[iIdx1]);
			//printf("  > unctFormats     : %i \n", unctFormats[iIdx1]);
			//printf("  > attributeNames  : %s \n", attributeNames[iIdx1]);
			//printf("  > attributeValues : %s \n", attributeValues[iIdx1]);
			//printf("  > units           : %s \n", units[iIdx1]);

			// Is KeyLOV?
			if(unctFormats[iIdx1] < 0){
				char	*sKeyLOVId = NULL;
				char	*theKeyLOVName;
				int		theOptions;
				int		theCount;
				char	**theKeys;
				char	**theValues;
				char	*theOwningSite;
				int		theSharedSiteCount;
				char	**theSharedSites;

				// Get KeyLOV Description
				stringstream ssKeyLOVID;
				ssKeyLOVID << unctFormats[iIdx1];
				string strKeyLovID = ssKeyLOVID.str();
				ITK_CALL(ICS_keylov_describe(strKeyLovID.c_str(), &theKeyLOVName, &theOptions, &theCount, &theKeys, &theValues, &theOwningSite, &theSharedSiteCount, &theSharedSites));

				// Get KeyLOV Value
				for(int iIdx2 = 0; iIdx2 < theCount; iIdx2++){
					if(strcmp(strTmpAttributeValue.c_str(), theKeys[iIdx2]) == 0){
						//printf("Match found for LOV %s \n", strKeyLovID.c_str());
						strTmpAttributeValue.clear();
						strTmpAttributeValue.append(theValues[iIdx2]);
						break;
					}
				}	

				// Free
				ssKeyLOVID.clear();
				strKeyLovID.clear();
				if(sKeyLOVId != NULL) MEM_free(sKeyLOVId);
				if(theKeyLOVName != NULL) MEM_free(theKeyLOVName);
				if(theKeys != NULL) MEM_free(theKeys);
				if(theValues != NULL) MEM_free(theValues);
				if(theOwningSite != NULL) MEM_free(theOwningSite);
				if(theSharedSites != NULL) MEM_free(theSharedSites);
			}

			// Get string representation of the Attribute ID
			std::stringstream ssAttributeID;
			std::string strAttributeID;
			ssAttributeID << unctNumbers[iIdx1];
			strAttributeID = ssAttributeID.str();

			// Loop attribute vector
			logical lAttributeFoundInVector = false;
			for(int iIdx2 = 0; iIdx2 < vecAttributeVector.size(); iIdx2++){
				//printf("  > compare %s - %s \n", vecAttributeVector.at(iIdx2).sTcUAID.c_str(), strAttributeID.c_str());
				if(vecAttributeVector.at(iIdx2).sTcUAID.compare(strAttributeID) == 0){
					// Transform number strings
					if((vecAttributeVector.at(iIdx2).sFormat.compare("number") == 0) || (vecAttributeVector.at(iIdx2).sFormat.compare("percent") == 0)){
						double dValue = atof(strTmpAttributeValue.c_str());
						double dFactor = vecAttributeVector.at(iIdx2).dFactor;
						dValue = dValue * dFactor;
						
						stringstream ssTmpValue;
						ssTmpValue << std::fixed << std::setprecision(15) << dValue;

						strTmpAttributeValue.clear();
						strTmpAttributeValue.append(ssTmpValue.str());
						strTmpAttributeValue = trimTrailingZerosFromNumberString(strTmpAttributeValue);
					}

					// Issue 1111. dxdatabook(2). Remove leading and trailing zeros.
					if (strcmp(vecAttributeVector.at(iIdx2).sCadimName.c_str(), "Breaking Capacity") == 0
					 || strcmp(vecAttributeVector.at(iIdx2).sCadimName.c_str(), "Size") == 0
					 || strcmp(vecAttributeVector.at(iIdx2).sCadimName.c_str(), "No of Res") == 0 )
					{
						strTmpAttributeValue = trimAndFormatString(strTmpAttributeValue);
					}

					ssClassificationAttributes << vecAttributeVector.at(iIdx2).sCadimName << "." << vecAttributeVector.at(iIdx2).sFormat << ";";
					ssClassificationValues << vecAttributeVector.at(iIdx2).sCadimName << "=" << strTmpAttributeValue << endl;

					break;
				}
			}
			ssAttributeID.clear();
			strAttributeID.clear();
		}

		MEM_free(unctNumbers);
		MEM_free(unctFormats);
		MEM_free(attributeNames);
		MEM_free(attributeValues);
		MEM_free(units);



		// Free
		if(saAttributeNames != NULL) MEM_free(saAttributeNames);
		if(saAttributeValues != NULL) MEM_free(saAttributeValues);
		if(sClassID != NULL) MEM_free(sClassID);
		if(sViewID != NULL) MEM_free(sViewID);
	}

	// Appending default attributes
	ssClassificationAttributes << "Status.text;Part Name.text;";

	// Preapare return string
	strClassificationString.append(ssClassificationAttributes.str());
	strClassificationString.append("\n");
	strClassificationString.append(ssClassificationValues.str());

	return strClassificationString;
}



std::string getClassificationDetails(tag_t tCommonComp, int iModus){
	std::string strClassificationString = "";
	tag_t		tClassificationObject	= NULLTAG;
	
	// Get Classification Info
	ITK_CALL ( ICS_ask_classification_object(tCommonComp, &tClassificationObject));
	if(tClassificationObject != NULLTAG){
		char	*sClassId		= NULL;
		char	*sClassName		= NULL;
		tag_t	tClass			= NULLTAG;

		ITK_CALL ( ICS_ask_class_of_classification_obj(tClassificationObject, &tClass) );
		ITK_CALL ( ICS_ask_id_name(tClass, &sClassId, &sClassName) );

		if(iModus == 1){
			strClassificationString = sClassId;
		} else if(iModus ==2 ) {
			strClassificationString = sClassName;
		} else {
			printf(" > ERROR: getClassificationDetails -> invalid mode '%i' \n", iModus);
		}

		// Free
		if(sClassId != NULL) MEM_free(sClassId);
		if(sClassName != NULL) MEM_free(sClassName);
	}

	return strClassificationString;
}

std::string getClassificationaPath(tag_t tCommonComp){
	std::string strClassificationPath	= "---";
	tag_t	tClassificationObject		= NULLTAG;
	tag_t	tClass						= NULLTAG;

	// Get Classification Info
	ITK_CALL ( ICS_ask_classification_object(tCommonComp, &tClassificationObject));
	if(tClassificationObject != NULLTAG){
		char	*sChildClassId		= NULL;
		char	*sChildClassName	= NULL;
		tag_t	tParentClass		= NULLTAG;

		ITK_CALL ( ICS_ask_class_of_classification_obj(tClassificationObject, &tClass) );
		ITK_CALL ( ICS_ask_id_name(tClass, &sChildClassId, &sChildClassName) );
		//printf("\n sChildClassName = %s \n", sChildClassName);
		strClassificationPath = sChildClassName;

		ITK_CALL ( ICS_ask_parent(tClass, &tParentClass) );

		// Loop parents
		while(tParentClass != NULLTAG){
			tag_t	tCurrentParent			= tParentClass;
			char	*sCurrentParentClassId	= NULL;
			char	*sCurrentParentClassName= NULL;
			std::string	strTmpStorage		= strClassificationPath;

			tParentClass = NULLTAG;

			ITK_CALL ( ICS_ask_id_name(tCurrentParent, &sCurrentParentClassId, &sCurrentParentClassName) );
			//printf(" sCurrentParentClassName = %s \n", sCurrentParentClassName);
			
			// Stop at class "SBT Common Components"
			if(strcmp(sCurrentParentClassId, "ekSBT") != 0){
				// Get Partent and attach current name to string
				ITK_CALL ( ICS_ask_parent(tCurrentParent, &tParentClass) );
				strClassificationPath.clear();
				strClassificationPath.append(sCurrentParentClassName);
				strClassificationPath.append(",");
				strClassificationPath.append(strTmpStorage);
			}

			// Free
			if(sCurrentParentClassId != NULL) MEM_free(sCurrentParentClassId);
			if(sCurrentParentClassName != NULL) MEM_free(sCurrentParentClassName);
		}

		// Free
		if(sChildClassId != NULL) MEM_free(sChildClassId);
		if(sChildClassName != NULL) MEM_free(sChildClassName);
	}

	return strClassificationPath;
}

logical isDerivedFromClass(char *className, tag_t tObjectTag){
	tag_t	tClassIdTag				= NULLTAG;
	tag_t	tSecondaryClassIdTag	= NULLTAG;
	logical	bIsDerived				= false;

	ITK_CALL(POM_class_id_of_class(className, &tClassIdTag));
	if(tClassIdTag != NULLTAG){
		ITK_CALL(POM_class_of_instance(tObjectTag, &tSecondaryClassIdTag));
		if(tSecondaryClassIdTag != NULLTAG){
			ITK_CALL(POM_is_descendant(tClassIdTag, tSecondaryClassIdTag, &bIsDerived));
		}
	}

	return bIsDerived;
}

std::string prepareForCsv(std::string sText)
{
	// Remove Delimiter
	for (int i = 0; i < sText.length(); ++i)
	{
		if (sText[i] == '\t')
		{
			sText[i] = ' ';
		}
	}

	// Issue 1046. ZBHK Report remove carriage return.
	std::size_t start_pos = 0;
	while((start_pos = sText.find("\r\n", start_pos)) != std::string::npos)
	{
		sText.replace(start_pos, 2, " ");
		//cout<<"\\r\\n found: " << strTcStringPropValue << endl;
	}
	start_pos = 0;
	while((start_pos = sText.find("\r", start_pos)) != std::string::npos)
	{
		sText.replace(start_pos, 1, " ");
		//cout<<"\\r found: " << strTcStringPropValue << endl;
	}
	start_pos = 0;
	while((start_pos = sText.find("\n", start_pos)) != std::string::npos)
	{
		sText.replace(start_pos, 1, " ");
		//cout<<"\\n found: " << strTcStringPropValue << endl;
	}

	return sText;	
}

std::string prepareForCsv(std::string sText, int iMaxLength)
{
	// Trim String
	if(sText.length() > iMaxLength)
	{
		sText = sText.substr(0, iMaxLength);
	}

	// Call general function
	return prepareForCsv(sText);
}

logical hasStatus(tag_t tObject, char *sStatusName)
{
	logical lHasStatus		= false;
	int		iStatusCount	= 0;
	tag_t	*taStatusList	= NULL;

	// Get Status List
	ITK_CALL(WSOM_ask_release_status_list(tObject, &iStatusCount, &taStatusList) );

	// Loop Status List
	for(int iIdx2 = 0; iIdx2 < iStatusCount; iIdx2++)
	{
		char *sStatusName_WithoutSuffix = NULL;

		// Get status name
		ITK_CALL(AOM_UIF_ask_value(taStatusList[iIdx2], "name", &sStatusName_WithoutSuffix) );

		// Has Status 10 ?
		if(strcmp(sStatusName_WithoutSuffix, sStatusName) == 0)
		{
			// Set return value
			lHasStatus = true;

			// Free
			if(sStatusName_WithoutSuffix != NULL)MEM_free(sStatusName_WithoutSuffix);

			// Break
			break;
		}

		// Free
		if(sStatusName_WithoutSuffix != NULL)MEM_free(sStatusName_WithoutSuffix);
	}

	// Free
	if(taStatusList != NULL)MEM_free(taStatusList);

	return lHasStatus;
}
