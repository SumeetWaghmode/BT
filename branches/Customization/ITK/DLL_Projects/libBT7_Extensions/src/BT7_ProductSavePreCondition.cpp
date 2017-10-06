#include "BT7_ServerExtensions.h"

int  siemens_checkNORMByte(char *pcProductAttributeValue,bool &isValidNORMBYTE,std::string &strNORMByte,std::string &strMASCH ,std::string &strSortProd1, std::string &strPrintProd1);
//void trim ( char * a );
//logical cutSuffix_CAM(char *sItemID, char *sItemTypeName, char sNewItemID[ITEM_id_size_c + 1]);

void trim ( char * a ) // trims leading and trailing spaces and carriage returns
{ 
    int i=0,j=0;
    if (a)
    {   
        for(i=0;(a[i]==32)||(a[i]==9)||(a[i]==10);++i); 
        for(j=0;a[j];++j)a[j]=a[j+i];  
        for(i=0;a[i];++i);  
        for(j=i-1;(a[j]==32)||(a[j]==9)||(a[j]==10);--j)a[j]=0;
    }

    return;
}


logical cutSuffix_CAM(char *sItemID, char *sItemTypeName, char sNewItemID[ITEM_id_size_c + 1])
{
	logical l_CAM_SuffixExists = false;

	if(strcmp(sItemTypeName, "BT7_Component") == 0 )
	{
		std::string strItemID = sItemID;
		if(strItemID.length() >= 4)
		{
			std::string strSubString = strItemID.substr(strItemID.length() - 4, 4);
			if(strSubString.compare("_CAM") == 0)
			{
				std::string strSubStringItemID = strItemID.substr(0, strItemID.length() - 4);
				strcpy(sNewItemID, strSubStringItemID.c_str());
				l_CAM_SuffixExists = true;
			}
		}
	}

	if(l_CAM_SuffixExists == false)
	{
		strcpy(sNewItemID, sItemID);
	}

	return l_CAM_SuffixExists;
}

/***********************************************************************************************************
* Function Name : siemens_checkNORMByte
* Description       : This function extracts NORMBYTE for given x_MLFB value and stores the values in output
*                    parameters in the following manner:-
*                    if NORMBYTE = 'F' then isValidNORMBYTE is set to 'true' else 'false'
*                     the value of MASCH extracted from NORM library is stored in x_MLFBMasch attribute
*
*
* REQUIRED HEADERS  :
* INPUT PARAMS      :
*
* RETURN VALUE      :int : 0/error code
* GLOBALS USED      :
* FUNCTIONS CALLED  :
*
* ALGORITHM     :
*
* History
*-------------------------------------------------------------------------------------------------------------
* Date          Name                Description of Change
* 31-Dec-2009   Sameer Nashikkar        Initial Code
* 14-Dec-2012   Javeed Shaikh           Task 6906: Changed the check from if(strcmp(pcNormbyte,"F")==0)
*                                       to if(strcmp(pcNormbyte,"X") !=0 ) as for some strings it was retruning vallue F3
* 24-Apr-2012   Christian Froelich      PL00022343: adapted code to switch attributes s4_PrintProdNo to s4_SortProdNo1 and vice versa.
*-------------------------------------------------------------------------------------------------------------
*
**************************************************************************************************************/
int  siemens_checkNORMByte(char *pcProductAttributeValue,bool &isValidNORMBYTE,std::string &strNORMByte,std::string &strMASCH ,std::string &strSortProd1, std::string &strPrintProd1)
{
    int iStatus = ITK_ok;

    norm_p1 p1;
    norm_p2 p2;

	//// print input string
	//char sRetVal[32];
	//strcpy(sRetVal, pcProductAttributeValue);
	//printf("\n");
	//printf(" EXTENSION: \n");
	//for(int iIdx1 = 0; iIdx1 < 32; iIdx1++)
	//{
	//	printf(" > %c \n", sRetVal[iIdx1]);
	//}
	//printf("\n");


    // declare the NORMBYTE and MASCH-ERZNR1 attributes
    char* pcNormbyte = NULL;
    char* pcMasch = NULL;
    char* pcSortProd1 = NULL;
    char* pcPrintProd1 = NULL;

    strcpy( p1.b_norm,"");
    strcpy( p1.d_norm,"");
    strcpy( p1.m_norm,"");
    strcpy( p1.ma_norm,"");
    strcpy( p1.normbyte,"");
    strcpy( p1.s_norm,"");
    strcpy( p1.sa_norm,"");

    strcpy(p2.mlfb_feh,"");

    for(int i=0; i<41;i++)
    {
        strcpy(p2.zeile[i].mlfb_art,"");
        strcpy(p2.zeile[i].mlfb_txt,"");
    }

    // copy the MLFB as the p1 input value
    strcpy( p1.eingfeld, pcProductAttributeValue );
    strcpy(p1.steubyte, "aa");

    // Function from NORM library that calculates NORMBYTE and MASCH values
    cnorm(&p1, &p2);
	printf("\n");
	

    // extract required values from the p1 struct
    pcNormbyte = p1.normbyte;
    pcMasch = p1.m_norm;

    // PL00022343: adapted code to switch attributes s4_PrintProdNo to s4_SortProdNo1 and vice versa.
    /*pcSortProd1 = p1.d_norm;
    pcPrintProd1 = p1.ma_norm;*/
    pcSortProd1 = p1.ma_norm;
    pcPrintProd1 = p1.d_norm;

    // Trim the trailing spaces which are added by cnorm function
    trim(pcNormbyte);
    trim(pcMasch);
    trim(pcSortProd1);
    trim(pcPrintProd1);

    // Assign NORMBYTE extracted from NORM library to output parameter
    strNORMByte.assign(pcNormbyte);
    strSortProd1.assign(pcSortProd1);
    strPrintProd1.assign(pcPrintProd1);

	printf("pcSortProd1 = %s \n\n", pcSortProd1);
	printf("pcPrintProd1 = %s \n\n", pcPrintProd1);


    // Check if NORMBYTE is 'F'
    //if(strcmp(pcNormbyte,"F")==0)

    if(strcmp(pcNormbyte,"X") !=0 )
    {
        isValidNORMBYTE = true;

        // Assign MASCH value extracted from NORM library to output parameter
        strMASCH.assign(pcMasch);
		printf("pcMasch = %s \n\n", pcMasch);
    }
    else
    {
        isValidNORMBYTE = false;
    }

    return iStatus;
}

/**
	@param const char* item_id
	@param const char* item_name
	@param const char* type_name
	@param const char* rev_id
	@param tag_t*      new_item
	@param tag_t*      new_rev
	@param tag_t       item_master_form
	@param tag_t       item_rev_master_form
*/
extern USER_EXT_DLL_API int BT7_ProductCreatePreCondition(METHOD_message_t* msg, va_list args) {
	int		status				= ITK_ok;
    char	*sItemID			= va_arg(args, char*);
    char	*sItemName			= va_arg(args, char*);
    char	*sItemType			= va_arg(args, char*);
	char	*sItemRevID			= va_arg(args, char*);
	tag_t	tCurrentGroupmember	= NULLTAG;
	tag_t	tCurrentGroup		= NULLTAG;
    char    sGroupname[SA_name_size_c + 1];

	bool isValidNORMBYTE;
	std::string strNORMByte;
	std::string strMASCH;
	std::string strSortProd1;
	std::string strPrintProd1;

	if(DEBUG == 1) printf("#################################################### \n");
	if(DEBUG == 1) printf("#######Entering method BT7_ProductCreatePreCondition \n");
	if(DEBUG == 1) printf("#################################################### \n");

	// Get Groupname
	ITK_CALL(SA_ask_current_groupmember(&tCurrentGroupmember));
	ITK_CALL(SA_ask_groupmember_group(tCurrentGroupmember, &tCurrentGroup));
	ITK_CALL(SA_ask_group_name(tCurrentGroup, sGroupname));

	// Issue 843. Numbering ITK modification.
	// 100_Admin or item type BT6/7_Product -> Normmodul
	if ( strcmp ( sGroupname, "100_Admin" ) == 0 ||
		 strcmp ( sItemType, "BT7_Product" ) == 0) {

		siemens_checkNORMByte(sItemID, isValidNORMBYTE, strNORMByte, strMASCH, strSortProd1, strPrintProd1);

		// Set error if Normmodul returned "isValidNORMBYTE == false"
		if(isValidNORMBYTE == false) {
			status = 919701;
			ITK_CALL(EMH_store_error(EMH_severity_error, status));
		}
	}

	return status;
}


/**
    @param tag_t       old_item
    @param tag_t       old_rev
    @param const char* new_item_id
    @param const char* new_rev_id
    @param tag_t*      new_item
    @param tag_t*      new_rev
    @param const char* new_name
    @param const char* new_description
    @param tag_t       item_master_form
    @param tag_t       item_rev_master_form
*/
extern USER_EXT_DLL_API int BT7_ProductSaveAsPreCondition(METHOD_message_t* msg, va_list args) {
	int		status			= ITK_ok;
	tag_t	tOldItem		= va_arg(args, tag_t);
	tag_t	tOldItemRev		= va_arg(args, tag_t);
    char	*sItemID		= va_arg(args, char*);
	tag_t	tCurrentGroupmember	= NULLTAG;
	tag_t	tCurrentGroup		= NULLTAG;
    char    sGroupname[SA_name_size_c + 1] = "";
	char	sItemType[ITEM_type_size_c+1] = "";

	bool isValidNORMBYTE;
	std::string strNORMByte;
	std::string strMASCH;
	std::string strSortProd1;
	std::string strPrintProd1;

	if(DEBUG == 1) printf("#################################################### \n");
	if(DEBUG == 1) printf("#######Entering method BT7_ProductSaveAsPreCondition \n");
	if(DEBUG == 1) printf("#################################################### \n");

	// Get Groupname
	ITK_CALL(SA_ask_current_groupmember(&tCurrentGroupmember));
	ITK_CALL(SA_ask_groupmember_group(tCurrentGroupmember, &tCurrentGroup));
	ITK_CALL(SA_ask_group_name(tCurrentGroup, sGroupname));

	// Get Item Type
	if ( tOldItem != NULLTAG)
	{
		ITEM_ask_type(tOldItem, sItemType);
	}

	// Issue 843. Numbering ITK modification.
	// 100_Admin or item type BT6/7_Product -> Normmodul
	if ( strcmp ( sGroupname, "100_Admin" ) == 0 ||
		 strcmp ( sItemType, "BT7_Product" ) == 0) {

		// 100_Admin -> Normmodul
		siemens_checkNORMByte(sItemID, isValidNORMBYTE, strNORMByte, strMASCH, strSortProd1, strPrintProd1);

		// Set error if Normmodul returned "isValidNORMBYTE == false"
		if(isValidNORMBYTE == false) {
			status = 919701;
			ITK_CALL(EMH_store_error(EMH_severity_error, status));
		}
	}

	return status;
}