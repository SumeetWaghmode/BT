#include "BT7_ServerExtensions.h"

/**
    Create a new item based on an existing item revision.

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
// Issue 1025. Item creation: restriction of 40 characters in item name
extern USER_EXT_DLL_API int BT7_ItemCreateFromRevPreCondition(METHOD_message_t* msg, va_list args) {

	int		status		= ITK_ok;
    tag_t	tOldItem	= va_arg(args, tag_t);
    tag_t	tOldRev		= va_arg(args, tag_t);
	char*	sItemId		= va_arg(args, char*);
    char*	sRevId		= va_arg(args, char*);
    tag_t*	tNewItem	= va_arg(args, tag_t*);
    tag_t*	tNewRev		= va_arg(args, tag_t*);
	char*	sItemName	= va_arg(args, char*);

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_ItemCreateFromRevPreCondition \n");
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