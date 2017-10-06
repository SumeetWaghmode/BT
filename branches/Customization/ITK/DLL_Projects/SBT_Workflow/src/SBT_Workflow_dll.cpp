/*
	10-Mar-2011		Matthias Schölch		Migriert auf 8.3
*/


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

#define DEBUG_VARIABLES_DECLARE 
#include "sy_debug.h"

#include "SBT_Workflow_grouphandler.h"
#include "SBT_01.h"

#include "rghStrs.h"

static int SBT_user_new_current_group ( 
	int			* piDecision , 
	va_list		args ) ;


extern DLLAPI int SBT_Workflow_register_callbacks ()
  {
  int retcode = ITK_ok;

  const char *theFunction = __FUNCTION__;

  ENTER (theFunction);

  fprintf(stderr,"************************************************************************************");
  fprintf(stderr,"\n  SBT_Workflow.dll %s build %s at %s for Teamcenter 11.2\n\n","11.2",__DATE__,__TIME__);
  fprintf(stderr,"************************************************************************************");

  // custom library, user exit, custom exit
  ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_register_properties" , (CUSTOM_EXIT_ftn_t)SBT_Workflow_register_properties));

  ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_init_module" , (CUSTOM_EXIT_ftn_t)SBT_Workflow_register_handler ) ) ;

  // Issue 843. Numbering ITK modification. Code commented out.
  //ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_new_item_id" , (CUSTOM_EXIT_ftn_t)SBT_Workflow_new_item_id ) ) ;

  ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_validate_item_rev_id", (CUSTOM_EXIT_ftn_t)SBT_Workflow_validate_id ) ) ;

  ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_define_bom_compare_modes", (CUSTOM_EXIT_ftn_t)SBT_Workflow_define_bom_compare_mode ) );

  ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_invoke_pdm_server", (CUSTOM_EXIT_ftn_t)SBT_invoke_pdm_server ) );

//hfhp 14-May-2009 According to Mr. Franz this user exit should not be implemented 
//  ITK ( CUSTOM_register_exit ( "SBT_Workflow" , "USER_new_current_group" , (CUSTOM_EXIT_ftn_t)SBT_user_new_current_group));


  LEAVE (theFunction);
  return retcode ;
  }


/**
 * \brief	Is called when the current group/role is changed
 *
 * Finds all EPMPerformSignoffTask objects and checks if any of it
 * has a group specific instruction dataset attached of which the group
 * is equal to the current new group.
 *
 * If such a EPMPerformSignoffTask object is found, its runtime
 * property group_task_string must be refreshed in order to reflect
 * the new current group.
 *
 * It is questionable, whether this will too update the display of 
 * the Instruction field in the client, which is nothing else than 
 * the object_desc attribute of the EPMPerformSignoffTask object.
 *
 * Remember: it is the client extension which is repsonsible to map 
 * the content of the runtime attribute group_task_string to the
 * object_desc attribute.
 *
 * \param[out]  piDecision  ...
 * \param[in]   args        see below
 * \param[out]  args        see below
 *
 * \return	ITK_ok or error code
 *                             
 */
int SBT_user_new_current_group ( 
	int			* piDecision , 
	va_list		args )
{
	int		retcode = ITK_ok				;
	int		iDecision = ALL_CUSTOMIZATIONS	;
	int		iPstCnt = 0						;
	int		iIdx0 = 0						;

	tag_t	* pPstTags = NULL		;
	tag_t	instDsTag = NULLTAG		;

	char  	cGroupName[SA_name_size_c+1] = ""	;
	char  	* pcTemplateValue = NULL			;
	char	* pcItemId = NULL					;

	logical		bGroupValue = false		;

	WSO_search_criteria_t	searchCriteria ;

	const tag_t		groupTag = va_arg ( args , tag_t ) ; /* (I) tag of new group */
    const tag_t		roleTag  = va_arg ( args , tag_t ) ; /* (I) tag of new role */

	const char *theFunction = __FUNCTION__;
	//TRACE_ON(0)
	ENTER (theFunction);

	* piDecision = iDecision ;


	if ( groupTag != NULLTAG ) {
		ITK ( SA_ask_group_name ( groupTag , cGroupName ) ) ;
	}
printf("cGroupName = %s\n",cGroupName);


	if ( strlen ( cGroupName) > 0 ) 
	{
		ITK ( WSOM_clear_search_criteria  ( &searchCriteria ) ) ;
		strcpy ( searchCriteria.pom_class_name , "EPMTask" ) ;
		strcpy ( searchCriteria.class_name , "EPMPerformSignoffTask" ) ;
		ITK ( WSOM_search ( searchCriteria , &iPstCnt , &pPstTags ) ) ;
	}
printf("iPstCnt = %d\n",iPstCnt);


	for ( iIdx0 = 0 ; ( retcode == ITK_ok ) && ( iIdx0 < iPstCnt ) ; iIdx0 ++ )
	{

		ITK ( SBT_WF_instruction_by_group_get_epmtask_args ( pPstTags[iIdx0] , &pcTemplateValue , &bGroupValue ) ) ;
printf("pcTemplateValue = %s bGroupValue = %d\n",pcTemplateValue,bGroupValue);


		pcItemId = NULL ;
		if ( ( pcTemplateValue != NULL ) && ( strlen ( pcTemplateValue ) > 0 ) ) {
			pcItemId = SBT_WF_instruction_by_group_get_item_id ( pcTemplateValue , cGroupName ) ;
		}
printf("pcItemId = %s\n",pcItemId);


		if ( ( pcItemId != NULL ) && ( strlen ( pcItemId ) > 0 ) ) {
//			ITK ( sbt_wf_find_instruction_dataset ( pcItemId , &instDsTag ) ) ; ... is in SBT_01.dll ...
		}




		MEM_free ( pcItemId ) ; pcItemId = NULL ;
	}


	MEM_free ( pPstTags ) ; pPstTags = NULL ; iPstCnt = 0 ;

  LEAVE (theFunction);
  return retcode ;
}




#ifdef __cplusplus
}
#endif
