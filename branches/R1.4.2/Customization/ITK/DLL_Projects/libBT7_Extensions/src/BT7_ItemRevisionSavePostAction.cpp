#include "BT7_ServerExtensions.h"

extern USER_EXT_DLL_API int BT7_ItemRevisionSavePostAction(METHOD_message_t* msg, va_list args) {
	int		status		= ITK_ok;
    tag_t	tItemRev	= va_arg(args, tag_t);
    logical	isNew		= va_arg(args, logical);

	logical	lModified	= false;
    char    sObjectName[WSO_name_size_c + 1];
	std::string	strObjectName;

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_ItemRevisionSavePostAction \n");
	if(DEBUG == 1) printf("################################################################ \n");

	// Get Object Name
	ITK_CALL(WSOM_ask_name(tItemRev, sObjectName));
	strObjectName = sObjectName;

	if(strObjectName.length() > 40)
	{
		// Cut Name
		strObjectName = strObjectName.substr(0, 40);

		// Set Name
		ITK_CALL(AOM_set_value_string(tItemRev, "object_name", strObjectName.c_str()));

		// Save Changes
		ITK_CALL(AOM_save(tItemRev));
	}

	return status;
}