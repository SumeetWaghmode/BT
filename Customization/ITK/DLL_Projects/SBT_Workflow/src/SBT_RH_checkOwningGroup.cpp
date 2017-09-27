/**
 * @file  SBT_RH_checkOwningGroup.cpp
 *
 * @brief  
 * ...
 *
 *
 *
 * @date  19-oct-2016
 */
/*----------------------------------------------------------------------------*/
/*
 *              Unpublished - All rights reserved
 *
 *  THIS PROGRAM IS AN UNPUBLISHED WORK FULLY PROTECTED BY THE UNITED STATES
 *               COPYRIGHT LAWS.
 */
/*----------------------------------------------------------------------------*/
/*
 * Changes :
 *	Name	Date			Description of changes
 *	Sumeet	19-oct-2016		Created.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <tc/iman.h>
#include <epm/epm.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tc/emh.h>
#include <tc/emh_errors.h>
#include <sa/sa.h>
#include "sy_debug.h"

#include "SBT_Utilities.h"

#define DllExport extern __declspec(dllexport)

//	SBT_RH_checkOwningGroup

EPM_decision_t SBT_RH_checkOwningGroup( 
	EPM_rule_message_t	msg	)	/* (I) */
{
	int		retcode = ITK_ok		;
	int		iArgCount = 0			;
	int		iIdx0 = 0				;
	int		iTargetCount = 0		;
	int		iIdx1 = 0				;

	tag_t	rootTaskTag = NULLTAG		;
	tag_t	* pTargetTags = NULLTAG		;
	tag_t	objectClassIdTag = NULLTAG	;
	tag_t	groupTag = NULLTAG			;

	tag_t	tCurrentGroupMember	= NULLTAG;
	tag_t	tGroup				= NULLTAG;
	tag_t	tRole				= NULLTAG;
	char	sGroupName[SA_name_size_c + 1];
	char	* pcArgName = NULL		;
	char	* pcArgValue = NULL		;
	char	* pcNogoMessage = NULL	;
	char 	* groupName = NULL	;
	EPM_decision_t	decision = EPM_undecided	;

  const char* theFunction = "SBT_RH_checkOwningGroup";
	//TRACE_ON(0);
  ENTER(theFunction);


	/* Initialize output */
	decision = EPM_go ;

	ITK ( EMH_clear_errors () ) ;


	/* Check handler arguments for customized message in case of EPM_nogo */
 	iArgCount = TC_number_of_arguments ( msg.arguments ) ;
	for ( iIdx0 = 0 ; iIdx0 < iArgCount ; iIdx0 ++ )
	{
		ITK ( ITK_ask_argument_named_value ( TC_next_argument ( msg.arguments ) , &pcArgName , &pcArgValue ) ) ;
		if ( ( pcArgName != NULL ) && ( strcmp ( pcArgName , "message" ) == 0 ) && 
       ( pcArgValue != NULL ) && ( strlen ( pcArgValue      ) > 0 )  ) pcNogoMessage = pcArgValue ;
	}


	/* Get target attachments */
	ITK ( EPM_ask_root_task ( msg.task , &rootTaskTag ) ) ;
	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) ) 
	{
		ITK ( EPM_ask_attachments ( rootTaskTag , EPM_target_attachment , &iTargetCount , &pTargetTags ) ) ; 
	}

	/* Get the current group */
	
	// Get current group member
	ITK(SA_ask_current_groupmember(&tCurrentGroupMember));

	// Get Group
	ITK(SA_ask_groupmember_group(tCurrentGroupMember, &tGroup));

	// Get Role
	ITK(SA_ask_groupmember_role(tCurrentGroupMember, &tRole));

	// Ask Group Name
	ITK(SA_ask_group_name(tGroup, sGroupName));

	/* Loop over all target attachments */
	for ( iIdx1 = 0 ; ( retcode == ITK_ok ) && ( iIdx1 < iTargetCount ) ; iIdx1 ++ )
	{

		/* Skip every target object which is not of class item revision or a class descendent from item */
		if ( ! object_is_of_class_or_desc ( pTargetTags[iIdx1] , "ItemRevision" , &objectClassIdTag ) ) continue ;


		/* Get owning group of current target object */
		ITK (AOM_get_value_tag(pTargetTags[iIdx1],"owning_group",&groupTag));
		ITK(AOM_UIF_ask_value(groupTag,"object_string", &groupName));
	
	} 


	/* Check current group and owning group and set EPM_nogo */ //error message 
	if(strcmp(sGroupName, groupName) != 0)
	{
		int		iErrorMessageLength = 0		;

		char	* pcObjectIdString = NULL	;
		char	* pcErrorMessage = NULL		;

		const char	cErrorMessage[] = "Current group and object owning group is not same , you can not initiate process"	;	

		if ( pcNogoMessage == NULL ) pcNogoMessage = (char*) cErrorMessage ;

		iErrorMessageLength = strlen ( pcNogoMessage ) + 1;
		pcErrorMessage = (char*) MEM_realloc ( pcErrorMessage , iErrorMessageLength * sizeof(char) ) ;
		strcpy ( pcErrorMessage , pcNogoMessage ) ;

		iErrorMessageLength = iErrorMessageLength + 1 ;
		pcErrorMessage = (char*) MEM_realloc ( pcErrorMessage , iErrorMessageLength * sizeof(char) ) ;
		strcat ( pcErrorMessage , "\n" ) ;
		

		ITK ( EMH_store_error_s1 ( EMH_severity_user_error , EMH_GENERIC_ERROR , pcErrorMessage ) ) ;   	
		MEM_free ( pcErrorMessage ) ;

		decision = EPM_nogo ;
	}


	MEM_free ( pcArgName ) ;
	MEM_free ( pcArgValue ) ;
	MEM_free ( pTargetTags ) ;	
	MEM_free(groupName);


  LEAVE(theFunction);
	return decision ;
}

#ifdef __cplusplus
}
#endif