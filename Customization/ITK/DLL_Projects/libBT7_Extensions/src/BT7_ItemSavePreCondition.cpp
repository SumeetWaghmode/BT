#include "BT7_ServerExtensions.h"

// Issue 1025. Item creation: restriction of 40 characters in item name
extern USER_EXT_DLL_API int BT7_ItemSavePreCondition(METHOD_message_t* msg, va_list args) {

	int		status		= ITK_ok;
    tag_t	tItem		= va_arg(args, tag_t);
    logical	isNew		= va_arg(args, logical);

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_ItemSavePreCondition \n");
	if(DEBUG == 1) printf("################################################################ \n");

	// Skip rule ?
	if(lDisableRule(false) )
	{
		return status;
	}

	char		sObjectName[WSO_name_size_c + 1];
	std::string	strObjectName;

	// Get Object Name
	ITK_CALL(WSOM_ask_name(tItem, sObjectName));

	// Using naming rule to validate the object name
	status = NR_validate_field("Item", "object_name", sObjectName);

	if (status)
	{
		return status;
	}

	strObjectName = sObjectName;
	if(strObjectName.length() > 40)
	{
		// Cut Name
		strObjectName = strObjectName.substr(0, 40);

		// Set Name
		ITK_CALL(AOM_set_value_string(tItem, "object_name", strObjectName.c_str()));

		// Save Changes
		ITK_CALL(AOM_save(tItem));
	}

	return status;
}