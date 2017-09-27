#include "BT7_ServerExtensions.h"

int BT7_ProceesParentBomline(tag_t tParentBomline, logical bEnableExtension);
int BT7_pref_get_string_values(char* pszPrefName,int* piPrefValCnt,char*** pppszPrefVals);


/**
    Copy attribute UG NAME to the attribute bl_ref_designator.
	This is used for IDF import to synchronize these two attributes.
	This happens only for BT7_ComponentRevision as root bom line with 
	part type PCB-A (LOV real name: l7LGCodPEq).

    @param METHOD_message_t*  msg
    @param va_list			  args
*/
extern USER_EXT_DLL_API int BT7_BomWindowSavePreAction(METHOD_message_t* msg, va_list args) {

	int						status		= ITK_ok;
    tag_t					tBomWindow	= va_arg(args, tag_t);
	tag_t					tTopBomLine = NULLTAG;
	logical bEnableExtension = false;

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_BomWindowSavePreAction \n");
	if(DEBUG == 1) printf("################################################################ \n");
	
	ITK_CALL(BOM_ask_window_top_line(tBomWindow, &tTopBomLine));

	if (NULLTAG != tTopBomLine)
	{
		logical lPCBAFound			= false;
		
		// Issue 968. Synchronise from UGNAME to bl_ref_designator. 
		// This functionality must apply only to root bom line of type BT7_CompomentRevision
		// with the attribute bt7_part_type=PCB-A	
		int		iBomLineObject		= 0;
		tag_t	tClass				= NULLTAG;
		tag_t	tItemRev			= NULLTAG;
		char	*sClassName			= NULL;
		char	*sSaveFromNX		= NULL;
		logical sSaveFromNXFound	= false;
		char* pszExtensionName  = NULL;
		int iCountOfPrefValues = 0;
		char**  ppszExtensionStatus = NULL;
		char* values[] = {"true", "false"};
		int value_count = 0;
		int iPrefCnt   = 0;		

		// Get the revision of the top bom line
		ITK_CALL(BOM_line_look_up_attribute("bl_line_object", &iBomLineObject));
		ITK_CALL(BOM_line_ask_attribute_tag(tTopBomLine, iBomLineObject , &tItemRev));

		pszExtensionName = "BT7_Save_From_NX_to_Teamcenter";
		ITK_CALL(BT7_pref_get_string_values(pszExtensionName, &iCountOfPrefValues, &ppszExtensionStatus));
		
		if (iCountOfPrefValues > 0 && status == ITK_ok )
		{
			/* Single valued preference only first index is valid */
			if ( tc_strcmp(ppszExtensionStatus[0], "true") == 0 )
			{
				bEnableExtension = true;
			}
			
			if (bEnableExtension == true)
			{
				// Ask item revision type
				ITK_CALL(POM_class_of_instance(tItemRev, &tClass));
				ITK_CALL(POM_name_of_class(tClass, &sClassName));

				// Process only for type BT7_CompomentRevision with part type PCB-A (LOV real name: l7LGCodPEq)
				if (sClassName != NULL && stricmp(sClassName, "BT7_ComponentRevision") == 0 )
				{
					logical	lAttributeExists	= false;
					char	*sPartType			= NULL;

					ITK_CALL(POM_attr_exists("bt7_parttype", sClassName, &lAttributeExists));
					if(lAttributeExists)
					{
						ITK_CALL(AOM_ask_value_string(tItemRev, "bt7_parttype", &sPartType));

						if (sPartType != NULL && stricmp(sPartType, "l7LGCodPEq") == 0)
						{
							lPCBAFound = true;
						}
					}
					// Free
					SAFE_SM_FREE(sPartType);
				}
				// Free
				SAFE_SM_FREE(sClassName);

				// Do nothing if rev type not matches
				if (!lPCBAFound)
				{
					return status;
				}				
				// copy value of attribute 'UG name' to attribute 'bl_ref_designator' if root obj is 'BT7_ComponentRevision' 				
				// and 'bt7_parttype' has value 'PCB-A'. Process the bom structure recursively
				ITK_CALL(BT7_ProceesParentBomline(tTopBomLine,bEnableExtension));
			}
			else if (bEnableExtension == false)
			{			
				//copy value of attribute 'bl_ref_designator' to attribute 'UG name'
				ITK_CALL(BT7_ProceesParentBomline(tTopBomLine,bEnableExtension));
			}			
		}
		else if(iCountOfPrefValues == 0 && status == ITK_ok)
		{	
			bEnableExtension = false;
			//copy value of attribute 'bl_ref_designator' to 'UG name'
			ITK_CALL(BT7_ProceesParentBomline(tTopBomLine,bEnableExtension));

			TC_preference_search_scope_t prefSearchScope = TC_preference_user;    
			ITK_CALL(PREF_initialize());
			ITK_CALL(PREF_ask_search_scope( &prefSearchScope));
			ITK_CALL(PREF_set_search_scope( TC_preference_user ));
			ITK_CALL(PREF_ask_value_count( pszExtensionName, &iPrefCnt ));

			if( iPrefCnt == 0 )
			{
				ITK_CALL(PREF_set_char_values(pszExtensionName,1,&values[1]));				
			}
		}
		
	}

	return status;
}


/**
    Process recursively for each bom line and 
	all its children.

    @param tag_t  tParentBomline
*/
int BT7_ProceesParentBomline(tag_t tParentBomline,logical bEnableExtension)
{
	int				status			= ITK_ok;
	vector<tag_t>	vPackedTag;

	// Get all children
	int childBOMLinesCount;
	tag_t *taChildBOMLines = NULL;

	ITK_CALL(BOM_line_ask_all_child_lines(tParentBomline , &childBOMLinesCount , &taChildBOMLines));

	// Loop BOM-Lines to unpack them
	for(int iIdx1 = 0; iIdx1 < childBOMLinesCount; iIdx1++)
	{
		// Issue 1208. 1PDM4BT: BOM - Add Object in PSE - Save Error. Process only node with write privilege.
		int 	iBomLineObj				= 0;
		tag_t	tBOMRev					= NULLTAG;

		ITK_CALL(BOM_line_look_up_attribute("bl_line_object", &iBomLineObj));
		ITK_CALL(BOM_line_ask_attribute_tag(taChildBOMLines[iIdx1], iBomLineObj , &tBOMRev));

		if (tBOMRev != NULLTAG )
		{
			logical lIsWritable	= false;
			ITK_CALL(AM_check_privilege(tBOMRev, "WRITE", &lIsWritable ));

			if (lIsWritable)
			{
				// Process the bom structure recursively
				ITK_CALL(BT7_ProceesParentBomline(taChildBOMLines[iIdx1],bEnableExtension));
			}
		}

		logical lIsPacked	= false;

		ITK_CALL(BOM_line_is_packed(taChildBOMLines[iIdx1], &lIsPacked));

		if (lIsPacked)
		{
			vPackedTag.push_back(taChildBOMLines[iIdx1]);
			ITK_CALL(BOM_line_unpack(taChildBOMLines[iIdx1]));
		}
	}

	// Free
	SAFE_SM_FREE(taChildBOMLines);
	childBOMLinesCount = 0;

	ITK_CALL(BOM_line_ask_all_child_lines(tParentBomline , &childBOMLinesCount , &taChildBOMLines));

	if(bEnableExtension == true)
	{
		// Loop BOM-Lines
		for(int iIdx2 = 0; iIdx2 < childBOMLinesCount; iIdx2++)
		{
			int		iBomLineUGNameAttribute	= 0;
			int		iBomLineRefDesAttribute	= 0;
			char	*sBOMLineUgName				= NULL;
			char	*sBomLineDesignator			= NULL;


			// get BOM Line attributes
			ITK_CALL(BOM_line_look_up_attribute("UG NAME", &iBomLineUGNameAttribute));
			ITK_CALL(BOM_line_ask_attribute_string(taChildBOMLines[iIdx2], iBomLineUGNameAttribute , &sBOMLineUgName));

			ITK_CALL(BOM_line_look_up_attribute("bl_ref_designator", &iBomLineRefDesAttribute));
			ITK_CALL(BOM_line_ask_attribute_string(taChildBOMLines[iIdx2], iBomLineRefDesAttribute , &sBomLineDesignator));

			// Only if current ChildLine is the Child we're looking for: Replace BOM-Line
			// Issue 944. ref.def. are set empty during save action as for migrated ASM UGNAME is empty. 
			// If UGName is empty then do nothing.
			if( sBOMLineUgName != NULL && strcmp(sBOMLineUgName, "") != 0 && strcmp(sBOMLineUgName, sBomLineDesignator) != 0)
			{
				// Duplicate property value from "UG NAME" to "bl_ref_designator"
				BOM_line_set_attribute_string(taChildBOMLines[iIdx2], iBomLineRefDesAttribute , sBOMLineUgName);
			}

			// Free
			SAFE_SM_FREE(sBOMLineUgName);
			SAFE_SM_FREE(sBomLineDesignator);
		}
	}
	else if(bEnableExtension == false)
	{

		// Loop BOM-Lines
		for(int iIdx2 = 0; iIdx2 < childBOMLinesCount; iIdx2++)
		{
			int		iBomLineRefDesAttribute	= 0;
			int		iBomLineUGNameAttribute	= 0;
			char	*sBomLineDesignator		= NULL;
			char	*sBOMLineUgName			= NULL;


			// get BOM Line attributes
			ITK_CALL(BOM_line_look_up_attribute("bl_ref_designator", &iBomLineRefDesAttribute));
			ITK_CALL(BOM_line_ask_attribute_string(taChildBOMLines[iIdx2], iBomLineRefDesAttribute , &sBomLineDesignator));

			ITK_CALL(BOM_line_look_up_attribute("UG NAME", &iBomLineUGNameAttribute));
			ITK_CALL(BOM_line_ask_attribute_string(taChildBOMLines[iIdx2], iBomLineUGNameAttribute , &sBOMLineUgName));

			// Only if current ChildLine is the Child we're looking for: Replace BOM-Line
			// Issue 944. ref.def. are set empty during save action as for migrated ASM UGNAME is empty. 
			// If UGName is empty then do nothing.
			if( sBomLineDesignator != NULL && strcmp(sBomLineDesignator, "") != 0 && strcmp(sBomLineDesignator, sBOMLineUgName) != 0)
			{
				// Duplicate property value from "bl_ref_designator" to "UG NAME".
				BOM_line_set_attribute_string(taChildBOMLines[iIdx2], iBomLineUGNameAttribute , sBomLineDesignator);
			}

			// Free
			SAFE_SM_FREE(sBomLineDesignator);
			SAFE_SM_FREE(sBOMLineUgName);
		}
	}	

	// Free
	SAFE_SM_FREE(taChildBOMLines);
	childBOMLinesCount = 0;

	// Loop BOM-Lines to pack them
	for (vector<tag_t>::iterator it = vPackedTag.begin() ; it != vPackedTag.end(); ++it)
	{
		ITK_CALL(BOM_line_pack(*it));
	}

	return status;

}

int BT7_pref_get_string_values
(
    char*   pszPrefName, 
    int*    piPrefValCnt, 
    char*** pppszPrefVals
)
{
    int iRetCode   = ITK_ok;
    int iPrefCnt   = 0;

    TC_preference_search_scope_t prefSearchScope = TC_preference_user;
    
    ITK_CALL(PREF_initialize());

    ITK_CALL(PREF_ask_search_scope( &prefSearchScope));

    ITK_CALL(PREF_set_search_scope( TC_preference_user ));

    ITK_CALL(PREF_ask_value_count( pszPrefName, &iPrefCnt ));

    if( iPrefCnt != 0 )
        ITK_CALL(PREF_ask_char_values( pszPrefName, piPrefValCnt, pppszPrefVals ));

    return iRetCode;
}
