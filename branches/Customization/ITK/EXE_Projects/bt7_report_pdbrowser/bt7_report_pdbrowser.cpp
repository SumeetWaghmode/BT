#import <MSXML6.dll>

#include <ctype.h>
#include <atlbase.h>
#include <msxml.h>
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
//using namespace MSXML2;

// --------------------------------------------------------------------------

void readFile(char *sFileToRead, int iModus);
std::string from_variant(VARIANT& vt);
void exportFilesFromTeamcenter(char *sRootExportDir);
void updateExportHistory(char *sExportHistoryFile);
int findItem(const char *sItemID, tag_t *tResult);
logical exportRequired(const char *sItemID, const char *sItemRevID);
int getPDFDatasets(tag_t tItemRevision, int *iPDFDatasetCount, tag_t **taPDFDatasets);
logical isDerivedFromClass(char *className, tag_t tObjectTag);
bool fileExists(const char *sFileName);

// --------------------------------------------------------------------------

struct PD_BROWSER_DOCUMENT_LIST
{
	std::string identnumber;
	std::string timestamp;
};

struct PD_BROWSER_EXPORT_HISTORY
{
	std::string identnumber;
	std::string revision;
	std::string timestamp;
};

struct PD_BROWSER_EXPORT_HISTORY_NEW_ENTRIES
{
	std::string identnumber;
	std::string revision;
	std::string timestamp;
};

vector<PD_BROWSER_DOCUMENT_LIST> vecPdBrowserDocumentList;
vector<PD_BROWSER_EXPORT_HISTORY> vecPdBrowserExportHistory;
vector<PD_BROWSER_EXPORT_HISTORY_NEW_ENTRIES> vecPdBrowserExportHistory_NewEntries;

ifstream	exportHistoryFile;
ifstream	documentListFile;
char		*mode = NULL;

// --------------------------------------------------------------------------

int ITK_user_main(int argc , char* argv[]) {
	logical	lInputError			= false;
	logical	lUseInputFile		= false;
    int		retcode				= ITK_ok;
    char	*user_id			= NULL;
    char	*user_passwd		= NULL;
    char	*user_group			= NULL;
	char	*export_history		= NULL;
	char	*document_list		= NULL;
	char	*root_export_dir	= NULL;

	/* Gets the user ID */
    user_id = ITK_ask_cli_argument("-u=");

    /* Gets the user Password */
    user_passwd = ITK_ask_cli_argument("-p=");

    /* Gets the user group */
    user_group = ITK_ask_cli_argument("-g=");

	/* Gets the export history file */
    export_history = ITK_ask_cli_argument("-export_history=");

	/* Gets the document list file */
    document_list = ITK_ask_cli_argument("-document_list=");

	/* Gets the root export dir*/
	root_export_dir = ITK_ask_cli_argument("-root_export_dir=");

	/* Gets the mode */
	mode = ITK_ask_cli_argument("-mode=");

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
	
	if(export_history == NULL || strcmp(export_history, "") == 0){
		printf(" > ERROR: Parameter '-export_history' should not be empty! \n");
		lInputError = true;
	} else {
		/* Check files: export_history */
		exportHistoryFile.open(export_history);
		if(exportHistoryFile.good() == false){
			printf(" > ERROR: export_history is not accessible! \n");
			lInputError = true;
		} else {
			exportHistoryFile.close();
		}
	}
	
	if(document_list == NULL || strcmp(document_list, "") == 0){
		printf(" > ERROR: Parameter '-document_list' should not be empty! \n");
		lInputError = true;
	} else {
		/* Check files: document_list */
		documentListFile.open(document_list);
		if(documentListFile.good() == false){
			printf(" > ERROR: document_list is not accessible! \n");
			lInputError = true;
		} else {
			documentListFile.close();
		}
	}

	if(root_export_dir == NULL || strcmp(root_export_dir, "") == 0){
		printf(" > ERROR: Parameter '-root_export_dir' should not be empty! \n");
		lInputError = true;
	}

	if(mode == NULL || strcmp(mode, "") == 0 || (strcmp(mode, "full") != 0 && strcmp(mode, "increment") != 0)){
		printf(" > ERROR: Parameter '-mode' must have the value 'full' or 'increment'! \n");
		lInputError = true;
	}

	/* Exit if parameters are not correct */
	if(lInputError == true) {
		return 9999;
	}

	/* Read Input files */
	readFile(export_history, 1);
	readFile(document_list, 2);

	/* Logging into Teamcenter */
	printf(" > Login to Teamenter (this might take a moment ...) \n");
	ITK_CALL ( ITK_init_module(user_id, user_passwd, user_group));
	ITK_CALL ( ITK_set_bypass(true));
	printf(" > Login successful \n");

	/* Export files from Teamcenter */
	exportFilesFromTeamcenter(root_export_dir);

	/* Update export-history */
	updateExportHistory(export_history);

	printf(" > Logout \n");
	//ITK_CALL ( ITK_exit_module(true));

    return retcode;
}

void readFile(char *sFileToRead, int iModus){
    HRESULT hr = CoInitialize(NULL); 
	if (SUCCEEDED(hr)){
		MSXML2::IXMLDOMDocument2Ptr xmlDoc;
		//hr = xmlDoc.CreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
		xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));

		if (xmlDoc->load(_T(sFileToRead)) != VARIANT_TRUE) {
			printf(" > Unable to load '%s' \n", sFileToRead);
		} else {
			printf(" > XML '%s' successfully loaded\n", sFileToRead);

			// Get all instances of "ClassAttribConfig"
			MSXML2::IXMLDOMNodeListPtr documentNodes = xmlDoc->getElementsByTagName("document");
			for(int iIdx1 = 0; iIdx1 < documentNodes->Getlength(); iIdx1++) {
				MSXML2::IXMLDOMNodePtr documentNode = documentNodes->Getitem(iIdx1);

				// Create temporary struct elements
				PD_BROWSER_EXPORT_HISTORY tmpExportHistory;
				PD_BROWSER_DOCUMENT_LIST tmpDocumentList;

				// Traverse child nodes of "document"
				MSXML2::IXMLDOMNodeListPtr documentChildNodes = documentNode->GetchildNodes();
				for(int iIdx2 = 0; iIdx2 < documentChildNodes->Getlength(); iIdx2++) {
					MSXML2::IXMLDOMNodePtr documentChildNode = documentChildNodes->Getitem(iIdx2);		

					// Get Name
					_variant_t sName = documentChildNode->GetnodeName();
					std::string strName = from_variant(sName);

					// Get Text
					_variant_t sText = documentChildNode->Gettext();
					std::string strText = from_variant(sText);

					// Attach properties to struct
					if(iModus == 1){
						// Export History
						if(strName.compare("identnumber") == 0){
							tmpExportHistory.identnumber = strText;
						} else if(strName.compare("revision") == 0){
							tmpExportHistory.revision = strText;
						} else if(strName.compare("timestamp") == 0){
							tmpExportHistory.timestamp = strText;
						} else {
							// Not applicable
						}
					} else if (iModus == 2){
						// Document List
						if(strName.compare("identnumber") == 0){
							tmpDocumentList.identnumber = strText;
						} else if(strName.compare("timestamp") == 0){
							tmpDocumentList.timestamp = strText;
						} else {
							// Not applicable
						}
					} else {
						// Not applicable
					}
				}

				// Add struct element to vector
				if(iModus == 1){
					// Export History
					vecPdBrowserExportHistory.push_back(tmpExportHistory);
				} else if (iModus == 2){
					// Document List
					vecPdBrowserDocumentList.push_back(tmpDocumentList);
				} else {
					// Not applicable
				}
			}
		}
	}
}

std::string from_variant(VARIANT& vt) {
	_bstr_t bs(vt);
	return std::string(static_cast<const char*>(bs));
}


bool fileExists(const char *sFileName) {
    ifstream f(sFileName);
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }   
}

void exportFilesFromTeamcenter(char *sRootExportDir){
	// Traverse Document List
	for(int iIdx1 = 0; iIdx1 < vecPdBrowserDocumentList.size(); iIdx1++){
		tag_t tItem = NULLTAG;
		PD_BROWSER_DOCUMENT_LIST tmpDocumentListEntry = vecPdBrowserDocumentList.at(iIdx1);

		// Find Item
		findItem(tmpDocumentListEntry.identnumber.c_str(), &tItem);
		if(tItem != NULLTAG){
			int		iRevCount	= 0;
			tag_t	*taItemRevs = NULL;

			// Get Revisions
			ITK_CALL(ITEM_list_all_revs(tItem, &iRevCount, &taItemRevs));

			// Traverse Revisions
			for(int iIdx2 = 0; iIdx2 < iRevCount; iIdx2++){
				char sRevId[ITEM_id_size_c + 1];

				// Get Revision Id
				ITK_CALL(ITEM_ask_rev_id(taItemRevs[iIdx2], sRevId));

				// Export required ?
				if(exportRequired(tmpDocumentListEntry.identnumber.c_str(), sRevId)){
					tag_t	*taPDFDatasets		= NULL;
					int		iPDFDatasetCount	= 0;

					//printf(" > Export required for %s/%s \n", tmpDocumentListEntry.identnumber.c_str(), sRevId);
					
					// Get PDF Datasets of the Item Revision
					getPDFDatasets(taItemRevs[iIdx2], &iPDFDatasetCount, &taPDFDatasets);
					if(iPDFDatasetCount > 0){
						// Traverse PDFs
						for(int iIdx3 = 0; iIdx3 <  iPDFDatasetCount; iIdx3++){
							int		iPDFReferenceCount	= 0;
							tag_t	*taPDFReferences	= NULL;

							ITK_CALL(AE_ask_all_dataset_named_refs(taPDFDatasets[iIdx3], "PDF_Reference", &iPDFReferenceCount, &taPDFReferences));

							// PDF found?
							if(iPDFReferenceCount > 0){
								// Build export path
								std::string sExportFileName = sRootExportDir;
								sExportFileName.append("\\");
								sExportFileName.append(tmpDocumentListEntry.identnumber);

								// Create Directory
								if (CreateDirectory(sExportFileName.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
									sExportFileName.append("\\");
									sExportFileName.append(sRevId);
									if (CreateDirectory(sExportFileName.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
										char sPDFDatasetName[WSO_name_size_c + 1];

										// Get PDF datset name
										ITK_CALL(WSOM_ask_name(taPDFDatasets[iIdx3], sPDFDatasetName));

										sExportFileName.append("\\");
										sExportFileName.append(sPDFDatasetName);

										// Extension required ?
										std::string strPDFDatasetName = sPDFDatasetName;
										logical lExtensionRequired = true;
										if(strPDFDatasetName.length() >= 4)
										{
											if(strPDFDatasetName.substr(strPDFDatasetName.length() - 4, strPDFDatasetName.length()).compare(".pdf") == 0)
											{
												lExtensionRequired = false;
											}
										}

										if(lExtensionRequired == true)
										{
											sExportFileName.append(".pdf");
										}

										// Remove file if exists
										logical lPerformExport = true;
										if(fileExists(sExportFileName.c_str())){
											if( remove( sExportFileName.c_str() ) != 0 ) {
												printf(" > Error removing file '%s' \n", sExportFileName.c_str());
												lPerformExport = false;
											}
										}

										if(lPerformExport == true) {
											// Export Datset
											int iExportStatus = AE_export_named_ref(taPDFDatasets[iIdx3], "PDF_Reference", sExportFileName.c_str());

											if(iExportStatus == ITK_ok){
												// Extend the vector for the export history
												PD_BROWSER_EXPORT_HISTORY_NEW_ENTRIES tmpNewHistoryEntry;
												tmpNewHistoryEntry.identnumber = tmpDocumentListEntry.identnumber;
												tmpNewHistoryEntry.revision = sRevId;
												tmpNewHistoryEntry.timestamp = tmpDocumentListEntry.timestamp;
												vecPdBrowserExportHistory_NewEntries.push_back(tmpNewHistoryEntry);
											} else {
												printf(" > Failed to export '%s/%s' (error code: %i) \n", tmpDocumentListEntry.identnumber.c_str(), sRevId);
											}
										}
									} else {
										printf(" > Failed to create directory '%s' \n", sExportFileName.c_str());
									}
								} else {
									printf(" > Failed to create directory '%s' \n", sExportFileName.c_str());
								}
							} else {
								printf(" > No PDF_Reference found for PDF Dataset below %s/%s - skipping export \n", tmpDocumentListEntry.identnumber.c_str(), sRevId);
							}

							// Free
							if(taPDFReferences != NULL) MEM_free(taPDFReferences);
						}
					} else {
						printf(" > No PDF Datasets found for %s/%s - skipping export \n", tmpDocumentListEntry.identnumber.c_str(), sRevId);
					}

					// Free
					if(taPDFDatasets != NULL) MEM_free(taPDFDatasets);
				} else {
					// Export not required
				}
			}

			// Free
			if(taItemRevs != NULL) MEM_free(taItemRevs);
		} else {
			// TODO: Logging Entry
		}
	}
}

int getPDFDatasets(tag_t tItemRevision, int *iPDFDatasetCount, tag_t **taPDFDatasets){
	int		iSecondaryCount		= 0;
	tag_t	*taSecondaryObjects	= NULL;
	tag_t	tRelSpecification	= NULLTAG;

	ITK_CALL(GRM_find_relation_type("IMAN_specification", &tRelSpecification));

	// Get all secondary objects
	ITK_CALL(GRM_list_secondary_objects_only(tItemRevision, NULLTAG, &iSecondaryCount, &taSecondaryObjects));

	// Traverse secondary objects and filter for PDF-Datasets
	for(int iIdx1 = 0; iIdx1 < iSecondaryCount; iIdx1++){
		char sObjectType[WSO_name_size_c+1];

		if(isDerivedFromClass("Dataset", taSecondaryObjects[iIdx1])){
			// Ask object type of secondary object
			ITK_CALL(WSOM_ask_object_type(taSecondaryObjects[iIdx1], sObjectType));

			if(strcmp(sObjectType, "PDF") == 0){
				if((*iPDFDatasetCount) == 0){
					*taPDFDatasets = (tag_t*) MEM_alloc(((*iPDFDatasetCount) + 1) * sizeof(tag_t));
				} else {
					*taPDFDatasets = (tag_t*) MEM_realloc(*taPDFDatasets, ((*iPDFDatasetCount) + 1) * sizeof(tag_t));
				}

				(*taPDFDatasets)[*iPDFDatasetCount] = taSecondaryObjects[iIdx1];
				(*iPDFDatasetCount) = (*iPDFDatasetCount) + 1;
			}
		}
	}

	// Free
	if(taSecondaryObjects != NULL) MEM_free(taSecondaryObjects);

	return ITK_ok;
}


logical exportRequired(const char *sItemID, const char *sItemRevID){
	logical lExportRequired = true;

	// full or increment export ?
	if(strcmp(mode, "increment") == 0)
	{
		// Traverse Export History
		for(int iIdx1 = 0; iIdx1 < vecPdBrowserExportHistory.size(); iIdx1++){
			PD_BROWSER_EXPORT_HISTORY tmpExportHistoryEntry = vecPdBrowserExportHistory.at(iIdx1);
			if(tmpExportHistoryEntry.identnumber.compare(sItemID) == 0 && tmpExportHistoryEntry.revision.compare(sItemRevID) == 0){
				lExportRequired = false;
				break;
			}
		}
	}

	return lExportRequired;
}

void updateExportHistory(char *sExportHistoryFile){
	// Open XMl Document
	HRESULT hr = CoInitialize(NULL); 
	if (SUCCEEDED(hr)){
		//hr = xmlDoc.CreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
		MSXML2::IXMLDOMDocument2Ptr xmlDoc;
		xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));

		if (xmlDoc->load(_T(sExportHistoryFile)) != VARIANT_TRUE) {
			printf(" > Unable to load '%s' \n", sExportHistoryFile);
		} else {
			printf(" > XML '%s' successfully loaded\n", sExportHistoryFile);

			// Get Root Element
			MSXML2::IXMLDOMElementPtr pXMLRootElem = xmlDoc->GetdocumentElement();
			
			printf(" > vecPdBrowserExportHistory_NewEntries.size() = %i \n\n", vecPdBrowserExportHistory_NewEntries.size());

			// Traverse Document List
			for(int iIdx1 = 0; iIdx1 < vecPdBrowserExportHistory_NewEntries.size(); iIdx1++){
				PD_BROWSER_EXPORT_HISTORY_NEW_ENTRIES tmpHistoryNewEntries = vecPdBrowserExportHistory_NewEntries.at(iIdx1);

				// Create new XML elements
				MSXML2::IXMLDOMElementPtr pNewDocumentTag = xmlDoc->createElement(_T("document"));
				MSXML2::IXMLDOMElementPtr pNewIdentnumber = xmlDoc->createElement(_T("identnumber"));
				MSXML2::IXMLDOMElementPtr pNewRevision = xmlDoc->createElement(_T("revision"));
				MSXML2::IXMLDOMElementPtr pNewTimestamp = xmlDoc->createElement(_T("timestamp"));

				// Assign values
				pNewIdentnumber->Puttext(_T(tmpHistoryNewEntries.identnumber.c_str()));
				pNewRevision->Puttext(_T(tmpHistoryNewEntries.revision.c_str()));
				pNewTimestamp->Puttext(_T(tmpHistoryNewEntries.timestamp.c_str()));

				// Append new XML elements to the document
				pNewDocumentTag = pXMLRootElem->appendChild(pNewDocumentTag);
				pNewIdentnumber = pNewDocumentTag->appendChild(pNewIdentnumber);
				pNewRevision = pNewDocumentTag->appendChild(pNewRevision);
				pNewTimestamp = pNewDocumentTag->appendChild(pNewTimestamp);
			}

			xmlDoc->save(sExportHistoryFile);



			// Transform using XSLT

			//MSXML2::IXMLDOMDocument2Ptr xslStylesheet;
			//xslStylesheet.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		 //   
			//// We need to load the style sheet which will be used to indent the XMl properly.
			//if(xslStylesheet->load(_T("C:\\$SBT_Repository\\trunk\\Customization\\ITK\\SBT_Report_PD\\OP_Export\\config\\xml-beautifier.xsl")) == VARIANT_FALSE) {
			//	printf(" > ERROR: XSLT load failed! \n");
			//} else {

				/* Format the XML. This requires a style sheet
				MSXML2::IXMLDOMDocument2Ptr xsltStylesheetDoc;
				xsltStylesheetDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
			    
				// We need to load the style sheet which will be used to indent the XMl properly.
				if(xsltStylesheetDoc->load(variant_t("C:\\$SBT_Repository\\trunk\\Customization\\ITK\\SBT_Report_PD\\OP_Export\\config\\xml-beautifier.xsl")) == VARIANT_FALSE) {
					printf("load Stylesheet failed \n");
				}

				// Create the final document which will be indented properly
				MSXML2::IXMLDOMDocument2Ptr pXMLFormattedDoc;
				hr = pXMLFormattedDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));

				CComPtr<IDispatch> pDispatch;
				hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
			 
				_variant_t vtOutObject;
				vtOutObject.vt = VT_DISPATCH;
				vtOutObject.pdispVal = pDispatch;
				vtOutObject.pdispVal->AddRef();

				//Apply the transformation to format the final document    
				try {
					hr = xmlDoc->transformNodeToObject(xsltStylesheetDoc, vtOutObject);
				} catch (...) {
					printf("TRANSFORMATION FAILED \n\n\n");
				}


				MSXML2::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();
				// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
				MSXML2::IXMLDOMNamedNodeMapPtr pXMLAttributeMap =  pXMLFirstChild->Getattributes();
				MSXML2::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));    
				pXMLEncodNode->PutnodeValue(_T("UTF-8"));    //encoding = UTF-8

				const _variant_t sLocation = _T(sExportHistoryFile);
				hr = pXMLFormattedDoc->save(sLocation);


				// Save the translated Excel-File
				pXMLFormattedDoc->save(sExportHistoryFile);*/

//			}


			//printf("BEFORE XSLT TRANSFORMATION ... \n");

			//MSXML2::IXMLDOMElementPtr xslStylesheetRootElem = xslStylesheet->GetdocumentElement();

			////pXMLRootElem->transformNode(xslStylesheet);
			//_bstr_t str = pXMLRootElem->transformNode(xslStylesheetRootElem);

			//std::wstring ws(str, SysStringLen(str));


			//printf(" > ws.size() = %i ... ws.c_str() = %s \n", ws.size(), ws.c_str());


			//MSXML2::IXMLDOMDocument2Ptr formattedXML;
			//formattedXML.CreateInstance(__uuidof(MSXML2::DOMDocument60));
		 //   
			//// We need to load the style sheet which will be used to indent the XMl properly.
			//if(formattedXML->loadXML(str) == VARIANT_FALSE) {
			//	printf(" > ERROR: Formatted XML load failed! \n");
			//} else {
			//	formattedXML->save(sExportHistoryFile);
			//}
			//printf("AFTER XSLT TRANSFORMATION ... \n");

			//// Save
			////xmlDoc->save(sExportHistoryFile);
		}

   /* var srcTree = new ActiveXObject("Msxml2.DOMDocument.6.0"); 
    srcTree.async=false; 
    srcTree.loadXml(xml);  
    var xsltTree= new ActiveXObject("Msxml2.DOMDocument.6.0"); 
    xsltTree.async = false; 
    xsltTree.loadXml(xslt); 
    var resTree = document.getElementById("HTMLData"); 
    resTree.innerHTML = srcTree.transformNode(xsltTree);*/ 
	}
}

int findItem(const char *sItemID, tag_t *tResult){

	/* Find Item */
	tag_t tQryItem = NULLTAG;
	ITK_CALL ( QRY_find("Item...", &tQryItem));

	if(tQryItem != NULLTAG){
		int		iResultCount	= 0;
		tag_t	*taResults		= NULL;
		char	**saQryEntries = (char**) MEM_alloc(1 * sizeof(char*));//[]	= {"ItemID", "Revision"};
		char	**saQryValues = (char**) MEM_alloc(1 * sizeof(char*));//[]	= {"CSE-000054","01"};

		saQryEntries[0] = (char*) MEM_alloc(strlen("Item ID") + 1);
		strcpy(saQryEntries[0], "Item ID" );

		saQryValues[0] = (char*) MEM_alloc(strlen(sItemID) + 1);
		strcpy(saQryValues[0], sItemID );

		ITK_CALL ( QRY_execute(tQryItem, 1, saQryEntries, saQryValues, &iResultCount, &taResults));

		/* Generate Report */
		if(iResultCount == 1) {
			*tResult = taResults[0];
		} else {
			printf(" > Result count = %i for Item '%s' \n", iResultCount, sItemID);
		}
	}

	return ITK_ok;
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