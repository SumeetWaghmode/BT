/**
* @file  sbt_rh_checkRevsInProc.c
*
* @brief  
* ...
*
* @author Hartmuth Petzold
*
* @version 1.0
*
* @date  5-Sep-2008
*/
/*----------------------------------------------------------------------------*/
/*
*          Copyright (c) 2008 Siemens PLM
*          Unpublished - All rights reserved
*
*  THIS PROGRAM IS AN UNPUBLISHED WORK FULLY PROTECTED BY THE UNITED STATES
*               COPYRIGHT LAWS.
*/
/*----------------------------------------------------------------------------*/
/*
* Changes :
*	Name	Date			Description of changes
*	<HP>	5-Sep-2008		Created.

10-Mar-2011		Matthias Schölch		Migriert auf 8.3

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
#include <tccore/aom.h>

#include "sy_debug.h"

#include "SBT_Utilities.h"

#define DllExport extern __declspec(dllexport)

	/**					
	* @brief
	* ...
	*
	* libuser_exits.ini entry:
	*			
	* [EPM_register_rule_handler_...]
	* Handler=SBT-RH-checkRevsInProc
	* Description=Rule Handler
	* Module=sbt_rh.dll
	* Function=SBT_RH_checkRevsInProc
	*
	* @param[in]	msg		target objects / [-message=<text in case of EPM_nogo>]
	*/


	//struct vecstruct
	//{
	//     //vector<std::string> vec;
	//	list<std::string> lst;
	//}st;

	EPM_decision_t SBT_RH_checkRevsInProc( 
		EPM_rule_message_t	msg	)	/* (I) */
	{
		int		retcode = ITK_ok		;
		int		iArgCount = 0			;
		int		iIdx0 = 0				;
		int		iTargetCount = 0		;
		int		iIdx1 = 0				;
		int		iRevisionCount = 0		;
		int		iIdx2 = 0				;
		int		iProcessCount = 0		;
		int		iRevInActProcCount = 0	;
		int		iIdx3 = 0				;
		int   	num = 0                 ;
		int     i = 0                   ;
		int     count=0                 ;
		
		tag_t	rootTaskTag = NULLTAG		;
		tag_t	* pTargetTags = NULLTAG		;
		tag_t	objectClassIdTag = NULLTAG	;
		tag_t	itemTag = NULLTAG			;
		tag_t	* pRevisionTags = NULL		;
		tag_t	* pProcessTags = NULL		;
		tag_t	* pRevInActProcTags = NULL	;
		tag_t   * propValuesTags=NULL       ;

		char	* pcArgName = NULL		;
		char	* pcArgValue = NULL		;
		char	* pcNogoMessage = NULL	;		
		char    *name=NULL              ;
		char    *WFname=NULL            ;
		char    *valtype_n              ;
		logical value=false             ;
		logical WIP=false               ;
		char    *d_value=NULL           ;


		EPM_decision_t	decision = EPM_undecided	;

		const char* theFunction = "SBT_RH_checkRevsInProc";
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


		/* Loop over all target attachments */
		for ( iIdx1 = 0 ; ( retcode == ITK_ok ) && ( iIdx1 < iTargetCount ) ; iIdx1 ++ )
		{
			count=0;
			/* Skip every target object which is not of class item revision or a class descendent from item */
			if ( ! object_is_of_class_or_desc ( pTargetTags[iIdx1] , "ItemRevision" , &objectClassIdTag ) ) continue ;


			/* Get all revisions of the item which owns the current target object */
			ITK ( ITEM_ask_item_of_rev ( pTargetTags[iIdx1] , &itemTag ) ) ;
			if ( ( retcode == ITK_ok ) && ( itemTag != NULLTAG ) ) 
			{
				ITK ( ITEM_list_all_revs ( itemTag , &iRevisionCount , &pRevisionTags ) ) ;
			}


			/* Loop over all item revisions starting with the oldest in time, 
			ending with immediate predecessor of the current target object */
			for ( iIdx2 = 0 ; ( retcode == ITK_ok ) && ( iIdx2 < iRevisionCount ) ; iIdx2 ++ )
			{
			
				if ( pRevisionTags[iIdx2] == pTargetTags[iIdx1] )
				{
					//Get logical value of property fnd0InProcess
					ITK(  AOM_ask_value_logical(pRevisionTags[iIdx2],"fnd0InProcess",&value));

					// Get the workflow name 
					ITK ( AOM_ask_name (rootTaskTag,&name));

					//Get number and tag of values in property fnd0StartedWorkflowTasks
					ITK ( AOM_ask_value_tags(pRevisionTags[iIdx2],"fnd0StartedWorkflowTasks",&num,&propValuesTags));
					
					//Loop all over number of values to found same workflow name
					for(i=0;i<num;i++)
					{								
						ITK ( AOM_ask_name (propValuesTags[i],&WFname));
						//compare workflow name with that values in property fnd0StartedWorkflowTasks
						if (strcmp(name,WFname)==0)
						{
							count++;
						}

					}

					if(count>1)
					{						
						WIP=true;
					}
					
				}


				else
				{
					/* If the current item revision is in an active process, remember it */
					ITK ( AOM_ask_value_tags ( pRevisionTags[iIdx2] , "process_stage_list" , &iProcessCount , &pProcessTags ) ) ;
					if ( iProcessCount > 0)
					{
						WIP = true;						
					}
				}

				if(WIP==true)
				{
					WIP=false;
					iRevInActProcCount = iRevInActProcCount + 1 ;
					pRevInActProcTags = (tag_t*) MEM_realloc ( pRevInActProcTags , iRevInActProcCount * sizeof(tag_t) ) ;
					pRevInActProcTags[iRevInActProcCount-1] = pRevisionTags[iIdx2] ;

				}

				MEM_free ( pProcessTags ) ; iProcessCount = 0 ; pProcessTags = NULL ;

			}

			MEM_free ( pRevisionTags ) ; iRevisionCount = 0 ; pRevisionTags = NULL ;
		} 


		/* Report the item revisions which are in an active process and set EPM_nogo */
		if ( iRevInActProcCount > 0 )
		{
			int		iErrorMessageLength = 0		;
			char	* pcObjectIdString = NULL	;
			char	* pcErrorMessage = NULL		;

			const char	cErrorMessage[] = "Der laufende Prozess ist abgebrochen worden, weil unter seinen Targetobjekten Item-Revisionen mit Vorgänger-Revisionen sind, die sich in einem aktiven Prozess befinden. Es handelt sich um folgende Vorgänger-Revisionen:"	;	

			if ( pcNogoMessage == NULL ) pcNogoMessage = (char*) cErrorMessage ;

			iErrorMessageLength = strlen ( pcNogoMessage ) + 1;
			pcErrorMessage = (char*) MEM_realloc ( pcErrorMessage , iErrorMessageLength * sizeof(char) ) ;
			strcpy ( pcErrorMessage , pcNogoMessage ) ;

			for ( iIdx3 = 0 ; iIdx3 < iRevInActProcCount ; iIdx3 ++ )
			{
				ITK ( WSOM_ask_object_id_string ( pRevInActProcTags[iIdx3] , &pcObjectIdString ) ) ;

				iErrorMessageLength = iErrorMessageLength + strlen ( pcObjectIdString ) + 1 ;
				pcErrorMessage = (char*) MEM_realloc ( pcErrorMessage , iErrorMessageLength * sizeof(char) ) ;
				strcat ( pcErrorMessage , "\n" ) ;
				strcat ( pcErrorMessage , pcObjectIdString ) ;

				MEM_free ( pcObjectIdString ) ; pcObjectIdString = NULL ;
			}

			ITK ( EMH_store_error_s1 ( EMH_severity_user_error , EMH_GENERIC_ERROR , pcErrorMessage ) ) ;   	
			MEM_free ( pcErrorMessage ) ;

			decision = EPM_nogo ;
		}


		MEM_free ( pcArgName ) ;
		MEM_free ( pcArgValue ) ;
		MEM_free ( pTargetTags ) ;
		MEM_free ( pRevInActProcTags ) ;


		LEAVE(theFunction);
		return decision ;
	}

#ifdef __cplusplus
}
#endif











