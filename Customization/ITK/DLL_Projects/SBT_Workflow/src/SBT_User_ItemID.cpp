#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tc/iman_arguments.h>
#include <tccore/imantype.h>
#include <tccore/item.h>
#include <tccore/item_msg.h>
#include <tccore/method.h>
#include <sa/sa.h>
#include <property/nr.h>
#include <epm/epm_toolkit_tc_utils.h>
#include "sy_debug.h"
#include <bmf/libuserext_exports.h>
#include <math.h>

//#include <norm/norm.h>

extern "C"
{
#include <norm/norm.h>
}




// Declarations
void trim ( char * a );
int  siemens_checkNORMByte(char *pcProductAttributeValue,bool &isValidNORMBYTE,std::string &strNORMByte,std::string &strMASCH ,std::string &strSortProd1, std::string &strPrintProd1);
logical isValidTypeForNormmodul(char *item_type);
logical isProductForNormmodul(char *item_type);

// Constants
char	*sBT7_MATERIAL		= "BT7_Material";
char	*sBT7_COMPPROG		= "BT7_CompProg";
char	*sBT7_COMPONENT		= "BT7_Component";
char	*sBT7_DOCUMENT		= "BT7_Document";
char	*sBT7_ELCAD			= "BT7_ELCAD";
char	*sBT7_PRODUCT		= "BT7_Product";
char	*sBT7_SOFTWARE		= "BT7_Software";
char	*sBT7_COMMONCOMP	= "BT7_CommonComp";

/**
* @brief
* Enable Attachment of a counter for Documents after the TNS-ID has been assigned
*/
// Issue 843. Numbering ITK modification. Code commented out.
//extern DLLAPI int SBT_Workflow_new_item_id(int *decision, va_list args)
//{
//    int		ifail		= ITK_ok;
//    tag_t	old_item	= va_arg(args, tag_t);
//	tag_t	item_type	= va_arg(args, tag_t); 
//    logical	*mod		= va_arg(args, logical *);  
//    char	**id		= va_arg(args, char **);
//	char	*id_str		= (char *) MEM_alloc(ITEM_id_size_c + 1 );
//	char	*nextFreeID	= NULL;
//    char	type_name[TCTYPE_name_size_c+1];
//
//	// Init
//	id_str[0] = '\0';
//	*id = id_str;
//	ITK ( TCTYPE_ask_name ( item_type, type_name ) );	
//
//	/** If the current ItemType is 'isValidTypeForNormmodul(type_name) == true':
//		> Normalization of the input
//		> Enable modification of the ItemID after assignment 
//	*/
//	if ( ( strcmp ( type_name, sBT7_PRODUCT ) == 0 ) || ( strcmp ( type_name, sBT6_PRODUCT ) == 0 ) ||
//		 ( *id != NULL && strcmp(*id, "") != 0 && ( isValidTypeForNormmodul(type_name) ) ) ) {
//
//		// Normbyte declarations
//		bool		isValidNORMBYTE	= TRUE;
//		std::string strNORMByte;
//		std::string strMASCH;
//		std::string strSortProd1;
//		std::string strPrintProd1;
//		tag_t		tCurrentRole		= NULLTAG;
//		char		sRoleName[SA_name_size_c + 1];
//
//		if(*id != NULL && strcmp(*id, "") != 0){
//			siemens_checkNORMByte(*id, isValidNORMBYTE, strNORMByte, strMASCH, strSortProd1, strPrintProd1);
//		}
//
//		if (isValidNORMBYTE == false) {
//			// Invalid ID -> Abbort
//			ifail = 919701;
//			ITK ( EMH_clear_errors ( ) );
//			ITK ( EMH_store_error ( EMH_severity_error, ifail ) );
//		} else {
//			// Valid ID -> Normalization
//			strcpy(id_str, strPrintProd1.c_str());
//
//			*decision  = ONLY_CURRENT_CUSTOMIZATION;
//		}
//
//		// Enable modification of the ID after assignment
//		*mod = TRUE;
//	} else {
//		// Get next free ID
//		ITK ( NR_next_value( type_name, "item_id", NULLTAG,"", "", "", NULLTAG, ""," ", &nextFreeID ) );
//
//		// Disable modification of the ID after assignment
//		*mod = FALSE;
//	}
//
//	// If ID could be generated, suppress further customizations
//	if(nextFreeID != NULL && strcmp(nextFreeID,"") != 0) {
//		*decision = ONLY_CURRENT_CUSTOMIZATION;
//
//		// Assign ID
//		strcpy(id_str, nextFreeID);
//	} else if ( isValidTypeForNormmodul(type_name) ) {
//		// Do not override behaviour for types which are beeing analyzed by the Normmodul
//		*decision = ONLY_CURRENT_CUSTOMIZATION;
//	} else {
//		// ID could not be generated, allow further customizations
//		*decision = ALL_CUSTOMIZATIONS;
//	}
//
//	// Free
//	SAFE_SM_FREE (nextFreeID);
//
//    return ITK_ok;	  
//}


/**
* @brief
* Override UserExit USER_validate_item_rev_id to normalization of SSNs

    This user exit is called when a user chooses OK or Apply in the Item Creation or Save As dialog boxes.
    It can verify that the IDs meet certain criteria and, if necessary, return modified IDs.
    If this function returns with status set to invalid,
    then the user can either accept the modified IDs (if supplied), or cancel the create.
    If this function returns with status set to modified,
    then the user can choose whether to use the modified IDs,
    force the system to use the originally entered IDs, or cancel the create.
    If this function returns with status set to override,
    then the modified IDs will be used without informing the user
    (this is helpful for performing case conversions).
*/
extern DLLAPI int SBT_Workflow_validate_id( int *decision, va_list args ) {
	int			ifail				= ITK_ok;
    char*       item_id				= va_arg(args, char*);		/**< (I) Item ID entered in the dialog box by the user. */
    char*       rev_id				= va_arg(args, char*);		/**< (I) Item Revision ID entered in the dialog box by the user. */
    char*       item_type			= va_arg(args, char*);		/**< (I) Item type entered into the dialog box by the user. This may be NULL. */
    char**      modified_item_id	= va_arg(args, char**);		/**< (OF) New modified Item ID. Set this to NULL to indicate that no modification has been made. */
	char**      modified_rev_id		= va_arg(args, char**);		/**< (OF) New, modified Item Revision ID. Set this to NULL to indicate that no modification has been made. */
    IDValid_e*  status				= va_arg(args, IDValid_e*);	/**< (O) Indicates the result of the checks. May be one of the following: <br>
																	#USER_valid_id - IDs are OK. <br>
																	#USER_invalid_id - IDs are not acceptable.<br>
																	#USER_modified_id - IDs are not ideal, but can be used if the user really wants them. <br>
																	#USER_override_id - IDs are not acceptable. Silently use the modified ones instead. */
	// Normbyte declarations
	bool		isValidNORMBYTE;
	std::string strNORMByte;
	std::string strMASCH;
	std::string strSortProd1;
	std::string strPrintProd1;

	tag_t	tCurrentGroupmember	= NULLTAG;
	tag_t	tCurrentGroup		= NULLTAG;
    char    sGroupname[SA_name_size_c + 1];

	// Does the item already exist? If yes, skip the validation because the user currently tries to revise a migrated part
	int    iItemsFound       = 0;
	tag_t *taItemsFound      = NULL;
	char **attrs  = (char **) MEM_alloc(1 * sizeof( char * ) );
	char **values = (char **) MEM_alloc(1 * sizeof( char * ) );

	attrs[0]  = "item_id";
	values[0] = item_id;

	ITK(ITEM_find_items_by_key_attributes(1, (const char **) attrs, (const char **) values, &iItemsFound, &taItemsFound) );

	// Get Groupname
	ITK(SA_ask_current_groupmember(&tCurrentGroupmember));
	ITK(SA_ask_groupmember_group(tCurrentGroupmember, &tCurrentGroup));
	ITK(SA_ask_group_name(tCurrentGroup, sGroupname));

	// Issue 843. Numbering ITK modification.
	// Execute if type is BT6_Product or BT7_Product.
	// Execute if current group == '100_Admin' and isValidTypeForNormmodul(item_type) == true
	if ( ( iItemsFound == 0 ) && ( isProductForNormmodul(item_type) ) || ( strcmp ( sGroupname, "100_Admin" ) == 0 ) && ( isValidTypeForNormmodul(item_type) ) && ( strcmp ( sGroupname, "dba" ) != 0 ) )
	{
		// Validate ItemID
		siemens_checkNORMByte(item_id, isValidNORMBYTE, strNORMByte, strMASCH, strSortProd1, strPrintProd1);

		if (isValidNORMBYTE == false) {
			// Invalid ID -> Abbort
			*status = USER_invalid_id;
			*decision  = ONLY_CURRENT_CUSTOMIZATION;
			
			ifail = 919701;
			ITK ( EMH_clear_errors ( ) );
			ITK ( EMH_store_error ( EMH_severity_error, ifail ) );
		} else {
			// Valid ID -> Normalization
			char id_str[1024] = {"\0"};
			sprintf(id_str, "%s", strPrintProd1.c_str());	// Q: strSortProd1 || strPrintProd1 ?

			*modified_item_id = (char*) MEM_alloc( (strlen(id_str) + 1) * sizeof(char) );
			strcpy(*modified_item_id, id_str);

			*modified_rev_id = (char*) MEM_alloc( (strlen(rev_id) + 1) * sizeof(char) );
			strcpy(*modified_rev_id, rev_id);

			*status = USER_override_id;
			*decision  = ONLY_CURRENT_CUSTOMIZATION;
		}
	} else {
		*decision  = ALL_CUSTOMIZATIONS;
	}

	// Free
	SAFE_SM_FREE(taItemsFound);

	return ifail;
}

/**
* Trims leading and trailing spaces and carriage returns
*/
void trim ( char * a ) { 
    int i=0,j=0;
    if (a) {   
        for(i=0;(a[i]==32)||(a[i]==9)||(a[i]==10);++i); 
        for(j=0;a[j];++j)a[j]=a[j+i];  
        for(i=0;a[i];++i);  
        for(j=i-1;(a[j]==32)||(a[j]==9)||(a[j]==10);--j)a[j]=0;
    }

    return;
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
int  siemens_checkNORMByte(char *pcProductAttributeValue,bool &isValidNORMBYTE,std::string &strNORMByte,std::string &strMASCH ,std::string &strSortProd1, std::string &strPrintProd1) {
    int iStatus = ITK_ok;

    norm_p1 p1;
    norm_p2 p2;

	//// print input string
	//char sRetVal[32];
	//strcpy(sRetVal, pcProductAttributeValue);
	//printf("\n");
	//printf(" WORKFLOW: \n");
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

// Issue 843. Numbering ITK modification. Valid item types for norm module check.
logical isValidTypeForNormmodul(char *item_type)
{
	return (
			strcmp ( item_type, sBT7_DOCUMENT ) == 0 ||
			strcmp ( item_type, sBT7_MATERIAL ) == 0 ||
			strcmp ( item_type, sBT7_COMMONCOMP ) == 0 ||
			strcmp ( item_type, sBT7_COMPPROG ) == 0 ||
			strcmp ( item_type, sBT7_COMPONENT ) == 0 ||
			strcmp ( item_type, sBT7_DOCUMENT ) == 0 ||
			strcmp ( item_type, sBT7_ELCAD ) == 0 ||
			strcmp ( item_type, sBT7_SOFTWARE ) == 0
		   );
}

// Issue 843. Numbering ITK modification. Valid product type for norm module check.
logical isProductForNormmodul(char *item_type)
{
	return (
			strcmp ( item_type, sBT7_PRODUCT ) == 0
		   );
}

#ifdef __cplusplus
}
#endif