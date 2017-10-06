#include "BT7_ServerExtensions.h"

/**
    Create a new GRM relation of the given type,
    linking the specified primary and secondary objects.

    @param tag_t  primary_object
    @param tag_t  secondary_object
    @param tag_t  relation_type
    @param tag_t  user_data
    @param tag_t* new_relation
*/
extern USER_EXT_DLL_API int BT7_RelConcernedCreatePostAction(METHOD_message_t* msg, va_list args) {
	int		iStatus			= ITK_ok;
	tag_t	tPrimaryTag		= va_arg(args, tag_t);
    tag_t	tSecondaryTag	= va_arg(args, tag_t);
    tag_t	tRelTypeTag		= va_arg(args, tag_t);
    tag_t	tUserDataTag	= va_arg(args, tag_t);
    tag_t	*tNewRelation	= va_arg(args, tag_t*);

	if(DEBUG == 1) printf(" > \n");
	if(DEBUG == 1) printf(" > BT7_RelConcernedCreatePostAction \n");
	if(DEBUG == 1) printf(" > \n");

	// Skip rule ?
	if(lDisableRule())
	{
		return ITK_ok;
	}

	// Clear Error Stack
	ITK_CALL(EMH_clear_errors());

	// Logic
	/*if(lCurrentUserFromRastatt())
	{
		ITK_CALL(AOM_set_value_logical(*tNewRelation, "bt7_rel_sbt_zug_logic", false));
	} else {
		ITK_CALL(AOM_set_value_logical(*tNewRelation, "bt7_rel_sbt_zug_logic", true));
	}*/

	return iStatus;	
}