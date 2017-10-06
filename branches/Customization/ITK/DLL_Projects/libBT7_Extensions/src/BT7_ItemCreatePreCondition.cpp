#include "BT7_ServerExtensions.h"

/**
    Create a new item with initial working revision.

    @param const char* item_id
    @param const char* item_name
    @param const char* type_name
    @param const char* rev_id
    @param tag_t*      new_item
    @param tag_t*      new_rev
    @param tag_t       item_master_form
    @param tag_t       item_rev_master_form
*/
// Issue 1025. Item creation: restriction of 40 characters in item name
extern USER_EXT_DLL_API int BT7_ItemCreatePreCondition(METHOD_message_t* msg, va_list args) {

	int		status		= ITK_ok;
    char*	sItemId		= va_arg(args, char*);
    char*	sItemName	= va_arg(args, char*);

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_ItemCreatePreCondition \n");
	if(DEBUG == 1) printf("################################################################ \n");

	// Skip rule ?
	if(lDisableRule(false) )
	{
		return status;
	}

	// Using naming rule to validate the object name
	status = NR_validate_field("Item", "object_name", sItemName);

	return status;
}