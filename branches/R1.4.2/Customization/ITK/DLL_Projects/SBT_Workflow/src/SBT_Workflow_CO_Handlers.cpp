#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <epm/signoff.h>
#include <tccore/method.h>
#include <itk/mem.h>
#include <property/prop.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tc/folder.h>
#include <ics/ics.h>
#include <ae/dataset.h>
#include <ae/datasettype.h>
#include <tc/envelope.h>

#include <sub_mgr/tcactionhandler.h>
#include <sub_mgr/tceventmgr.h>
#include <sub_mgr/tceventtype.h>
#include <sub_mgr/standardtceventtypes.h>
#include <sub_mgr/subscription.h>
#include <tc/tc_util.h>
#include <tc/iman_arguments.h>

#include "sy_debug.h"

#include "SBT_Workspaceobject.h"
#include "SBT_ItemRevision.h"
#include "SBT_Utilities.h"
#include "SBT_Utilities_d.h"
#include "SBT_Workflow.h"
#include "SBT_Workflow_Attributes.h"
#include "SBT_File.h"

#include "SBT_Workflow_grouphandler.h"

#ifdef __cplusplus
}
#endif

// Include Common
#include "SBT_Common.h"


// Declarations
char *sChangeOrderSubProcess = "BT Change Order - Sub Process";
char *sChangeOrderProcess    = "BT Change Order";
char *sBT7_ChangeNoticeRev   = "BT7_ChangeNoticeRevision";
char *sEPMTask = "EPMTask";

int setStatusToTargetRev(tag_t targetObject, tag_t tNewStatus );


/**
 * @brief
 * Notify the Change Specialist after a Work Package has been finished
 */
int SBT_AH_notify_change_specialist(EPM_action_message_t msg)
{
	const char *theFunction      = "SBT_AH_notify_change_specialist";
	int         retcode          = ITK_ok;
	int         iTargetCount     = 0;
	tag_t       rootTaskTag      = NULLTAG;
	tag_t *     pTargetTags      = NULL;
	tag_t       tChangeNoticeRev = NULLTAG;
	tag_t       tOwningUser      = NULLTAG;

	/* Avoid infinite loops */
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask + Attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Get owning user of the root task */
	ITK(AOM_ask_value_tag(rootTaskTag, "owning_user", &tOwningUser) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			tag_t tItem = NULLTAG;
			char  sECOid[ITEM_id_size_c + 1];
			char  sECORevID[ITEM_id_size_c + 1];
			char  sECOObjName[ITEM_name_size_c + 1];

			// Get ItemID
			ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tItem) );
			ITK(ITEM_ask_id(tItem, sECOid) );
			ITK(ITEM_ask_rev_id(pTargetTags[iIdx1], sECORevID) );
			ITK(ITEM_ask_name(tItem,sECOObjName));

			// Change Notice identified ...
			// 1.) Identify all non-finished sub-tasks
			// 2.) Generate a message string to inform the Change Specialist which tasks are WIP

			int    n_referencers = 0;
			int *  levels        = NULL;
			tag_t *referencers   = NULL;
			char **relations     = NULL;
			ITK(WSOM_where_referenced(pTargetTags[iIdx1], 1, &n_referencers, &levels, &referencers, &relations) );

			/* Mail*/
			int          iOpenTasks = 0;
			stringstream ss_MailOpenTasks;
			stringstream ss_MailContent;
			stringstream ss_MailSubject;

			ss_MailSubject << sECOid << " task '";
			ss_MailContent << "The following task has been closed: ";

			// Loop referencers
			for ( int iIdx2 = 0; iIdx2 < n_referencers; iIdx2++ )
			{
				char sObjectType2[WSO_name_size_c + 1];
				ITK(WSOM_ask_object_type(referencers[iIdx2], sObjectType2) );

				if ( strcmp(sObjectType2, sEPMTask) == 0 )
				{
					char sName[WSO_name_size_c + 1];
					ITK(EPM_ask_name(referencers[iIdx2], sName) );

					if ( strcmp(sName, sChangeOrderSubProcess) == 0 )
					{
						EPM_state_t wfState;
						tag_t       tJob;
						char        sTaskName[WSO_name_size_c + 1];
						char        sJobDesc[WSO_desc_size_c + 1];

						ITK(EPM_ask_state(referencers[iIdx2], &wfState) );
						ITK(EPM_ask_name(referencers[iIdx2], sTaskName) );

						ITK(EPM_ask_job(referencers[iIdx2], &tJob) );
						ITK(WSOM_ask_description(tJob, sJobDesc) );

						if ( referencers[iIdx2] == rootTaskTag )
						{
							//current task
							printf(" >>> Closed Task '%s'/'%s' found \n", sTaskName, sJobDesc);

							ss_MailSubject << sJobDesc << "' closed";
							ss_MailContent << sJobDesc;
						}
						else if ( wfState != EPM_completed )
						{
							// open task
							printf(" >>> Open Task '%s'/'%s' found \n", sTaskName, sJobDesc);

							ss_MailOpenTasks << " > '" << sJobDesc << "'" << endl;
							iOpenTasks++;
						}
					}
					else if ( strcmp(sName, sChangeOrderProcess) == 0 )
					{
						/* Target attachment of the current Solution Items to the parent process */
						//tag_t  tSolutionItemsRelType = NULLTAG;
						//tag_t *taSolutionItems       = NULL;
						//int    iSolutionItemsCount   = 0;

						//ITK(GRM_find_relation_type("CMHasSolutionItem", &tSolutionItemsRelType) );
						//ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tSolutionItemsRelType, &iSolutionItemsCount, &taSolutionItems) );

						//for ( int iIdx3 = 0; iIdx3 < iSolutionItemsCount; iIdx3++ )
						//{
						//	tag_t tUserTag         = NULLTAG;
						//	tag_t tGroupTag        = NULLTAG;
						//	tag_t tCurrentUser     = NULLTAG;
						//	char *sCurrentUserName = NULL;

						//	ITK(POM_get_user(&sCurrentUserName, &tCurrentUser) );
						//	ITK(POM_ask_owner(taSolutionItems[iIdx3], &tUserTag, &tGroupTag) );

						//	if ( tUserTag == tCurrentUser )
						//	{
						//		tag_t *taAttachments     = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );
						//		int *  iaAttachmentTypes = (int *) MEM_alloc(1 * sizeof( int ) );

						//		taAttachments[0]     = taSolutionItems[iIdx3];
						//		iaAttachmentTypes[0] = EPM_target_attachment;

						//		/* Attach Referenced Dataset to Parent Task and refresh */
						//		ITK(EPM_add_attachments(referencers[iIdx2], 1, taAttachments, iaAttachmentTypes) );
						//		ITK(AOM_refresh(referencers[iIdx2], 0) );
						//	}

						//	// Free
						//	SAFE_SM_FREE(sCurrentUserName);
						//}

						//// Free
						//SAFE_SM_FREE(taSolutionItems);

						/* Get Text Datasets from Sub-Process and attach them to the Main-Process */
						int    iReferenceCount = 0;
						tag_t *pReferenceTags  = NULL;
						ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );

						for ( int iIdx3 = 0; iIdx3 < iReferenceCount; iIdx3++ )
						{
							char sReferenceTagType[WSO_name_size_c + 1];

							/* */
							ITK(WSOM_ask_object_type(pReferenceTags[iIdx3], sReferenceTagType) );

							if ( strcmp(sReferenceTagType, "Text") == 0 )
							{
								tag_t *taAttachments     = (tag_t *) MEM_alloc(1 * sizeof( tag_t ) );
								int *  iaAttachmentTypes = (int *) MEM_alloc(1 * sizeof( int ) );

								taAttachments[0]     = pReferenceTags[iIdx3];
								iaAttachmentTypes[0] = EPM_reference_attachment;

								/* Attach Referenced Dataset to Parent Task and refresh */
								ITK(EPM_add_attachments(referencers[iIdx2], 1, taAttachments, iaAttachmentTypes) );
								ITK(AOM_refresh(referencers[iIdx2], 0) );

								/* Free */
								SAFE_SM_FREE(taAttachments);
								SAFE_SM_FREE(iaAttachmentTypes);
							}
						}

						/* Free */
						SAFE_SM_FREE(pReferenceTags);
					}
				}
			}                         /* end for*/


			/* Mail */
			ss_MailContent << "\n\n> " << sECOid << " | "
						   << sECORevID << " | " << sECOObjName;

			// Append the link of the target items in the email
			char*   url = NULL;
			
			// Change 15.10.2015: Pref of Mailserver is not set in some Sites.
			int iPrefCount = 0;
			ITK(PREF_ask_value_count("WEB_default_site_server", &iPrefCount));
			if ( iPrefCount > 0 )
			{
				ITK(TC_tag_to_url(pTargetTags[iIdx1], DHTML, &url));
			}
			
			if ( url != NULL )
			{
				ss_MailContent << "\n\nTarget Object: " << url << endl;
			}
			else
			{
				TRACE1("url is NULL for tag %i", pTargetTags[iIdx1]);
			}

			MEM_free(url);

			ss_MailContent << endl << endl << "Following tasks are open:" << endl;

			if ( iOpenTasks == 0 )
			{
				ss_MailContent << " > None";
			}
			else
			{
				ss_MailContent << ss_MailOpenTasks.str( );
			}

			ss_MailContent << endl << "**********";
			ss_MailContent << endl << "Please ignore Rich Client Link";
			ss_MailContent << endl << "**********";

			/* Shorten subject and content */
			string sMailSubject = ss_MailSubject.str( );

			if ( sMailSubject.length( ) > 128 )
			{
				sMailSubject = sMailSubject.substr(0, 128);
			}

			string sMailContent = ss_MailContent.str( );

			if ( sMailContent.length( ) > 240 )
			{
				sMailContent = sMailContent.substr(0, 240);
			}

			/* Send Mail */
			tag_t tMail = NULLTAG;
			createMail(sMailSubject.c_str( ), sMailContent.c_str( ), &tMail);
			addReceiverToMail(tMail, tOwningUser);
			sendEmailNotification(tMail);

			/* Free */
			SAFE_SM_FREE(levels);
			SAFE_SM_FREE(referencers);
			SAFE_SM_FREE(relations);

			/* Break */
			break;
		}
		else
		{
			//
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	// Clear Error Stack
	ITK(EMH_clear_errors());

	/* Exit */
	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_notify_change_specialist */


/**
 * @brief
 * Copy the ECO Nr to the Solution Items
 */
int SBT_AH_co_copyEcoNrToSolutions(EPM_action_message_t msg)
{
	const char *theFunction      = "SBT_AH_co_copyEcoNrToSolutions";
	int         retcode          = ITK_ok;
	int         iTargetCount     = 0;
	tag_t       rootTaskTag      = NULLTAG;
	tag_t *     pTargetTags      = NULL;
	tag_t       tChangeNoticeRev = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask + Attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			tag_t  tSolutionItemsRelType = NULLTAG;
			tag_t  tECOItem        = NULLTAG;
			tag_t *taSolutionItems = NULL;
			int    iSolutionCount  = 0;
			char   sECOID[ITEM_id_size_c + 1];

			// Get ECO-ID
			ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tECOItem) );
			ITK(ITEM_ask_id(tECOItem, sECOID) );

			// Get Solution Items
			ITK(GRM_find_relation_type("CMHasSolutionItem", &tSolutionItemsRelType) );
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tSolutionItemsRelType, &iSolutionCount, &taSolutionItems) );

			//Loop Solution Items
			for ( int iIdx2 = 0; iIdx2 < iSolutionCount; iIdx2++ )
			{
				tag_t		tClass1;
				tag_t		tClass2;
				char        sObjectType[WSO_name_size_c + 1];
				std::string strObjectType;

				ITK(WSOM_ask_object_type(taSolutionItems[iIdx2], sObjectType) );

				if ( object_is_of_class_or_desc(taSolutionItems[iIdx2], TC_PARTREVISION, &tClass1) 
					|| object_is_of_class_or_desc(taSolutionItems[iIdx2],SBT_BT_DOCUMENTREVISION, &tClass2)  )/* Fix for Issue:
																											  0SBT-DO-01PDM4BT
																											  |SBTDOP-86 
																											  "Serial Release" ECO */
				{
					char *sAttrName = BT_PROP_BT7_DOCMNUMBER;
					char *sValue    = NULL;

					// Write ECO-ID into the attribute if empty or has the default value "No ECO"
					ITK(AOM_ask_value_string(taSolutionItems[iIdx2], sAttrName, &sValue) );

					if ( strcmp(sValue, "") == 0 || strcmp(sValue, "No ECO") == 0 )
					{
						ITK(AOM_refresh(taSolutionItems[iIdx2], true) );
						ITK(AOM_set_value_string(taSolutionItems[iIdx2], sAttrName, sECOID) );
						ITK(AOM_save(taSolutionItems[iIdx2]) );
						ITK(AOM_refresh(taSolutionItems[iIdx2], false) );
					}
					else
					{
						// Do not overwrite existing values - TODO: CORRECT?
					}

					// Free
					SAFE_SM_FREE(sValue);
				}
				else if ( strObjectType.compare(0, 3, "BT7") == 0 )
				{
					// Write ECO-ID to Rev Master Form
					tag_t  tMasterFormRelType = NULLTAG;
					tag_t *taMasterForms      = NULL;
					int    iMasterFormCount   = 0;

					ITK(GRM_find_relation_type("IMAN_master_form", &tMasterFormRelType) );
					ITK(GRM_list_secondary_objects_only(taSolutionItems[iIdx2], tMasterFormRelType, &iMasterFormCount, &taMasterForms) );

					if ( iMasterFormCount == 1 )
					{
						char *sAttrName = "bt7_docmnumber";
						char *sValue    = NULL;

						// Write ECO-ID into the attribute if empty
						ITK(AOM_ask_value_string(taMasterForms[0], sAttrName, &sValue) );

						if ( strcmp(sValue, "") == 0 )
						{
							ITK(AOM_refresh(taMasterForms[0], true) );
							ITK(AOM_set_value_string(taMasterForms[0], sAttrName, sECOID) );
							ITK(AOM_save(taMasterForms[0]) );
							ITK(AOM_refresh(taMasterForms[0], false) );
						}
						else
						{
							// Do not overwrite existing values - TODO: CORRECT?
						}

						// Free
						SAFE_SM_FREE(sValue);
					}
					else
					{
						printf(" > ERROR: Not exactly one IRMF found ... TODO: Error message \n");
					}

					// Free
					SAFE_SM_FREE(taMasterForms);
				}
				else
				{
					printf(" > %s -> something else! \n", sObjectType);
				}
			}

			// Free
			SAFE_SM_FREE(taSolutionItems);
		}
	}         /* end for*/

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	/* Exit */
	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_co_copyEcoNrToSolutions */


/**
 * @brief
 * Start Change Order Work Processes
 */
int SBT_AH_co_start_wp(EPM_action_message_t msg)
{
	const char *theFunction      = "SBT_AH_co_start_wp";
	int         retcode          = ITK_ok;
	int         iTargetCount     = 0;
	tag_t       rootTaskTag      = NULLTAG;
	tag_t *     pTargetTags      = NULL;
	tag_t       tChangeNoticeRev = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask + Attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1] = "";

		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			tChangeNoticeRev = pTargetTags[iIdx1];
			break;
		}
	}

	if ( tChangeNoticeRev != NULLTAG )
	{
		tag_t tChangeNoticeItem = NULLTAG;
		tag_t tProcessTemplate  = NULLTAG;
		tag_t tNewProcess       = NULLTAG;
		tag_t tmpRootTaskTag    = NULLTAG;
		char  sChangeNoticeItemID[ITEM_id_size_c + 1];

		int iSubProcessCount = 7;

		//char	*saSubProcessNames[6]	= {	"Hardware Mechanics",
		//									"Hardware PCB",
		//									"Hardware ECE (Electronic Component Engineering)",
		//									"Technical documentation",
		//									"Firmware",
		//									"Conformity check" };
		char *saSubProcessNames[7] =
		{
			"Mechanical Development",
			"Hardware Development",
			"Component Engineering",
			"User Documentation",
			"Software Development",
			"Approval and Conformity",
			"Serialization"

		};

		// Get ECO-ItemID
		ITK(ITEM_ask_item_of_rev(tChangeNoticeRev, &tChangeNoticeItem) );
		ITK(ITEM_ask_id(tChangeNoticeItem, sChangeNoticeItemID) );

		// Find process template and start sub-processes
		ITK(EPM_find_process_template(sChangeOrderSubProcess, &tProcessTemplate) );

		for ( int iIdx1 = 0; iIdx1 < iSubProcessCount; iIdx1++ )
		{
			// Sub-Workflow Name
			char sSubWFName[200];
			strcpy(sSubWFName, "Task for '");
			strcat(sSubWFName, sChangeNoticeItemID);
			strcat(sSubWFName, "' : ");
			strcat(sSubWFName, saSubProcessNames[iIdx1]);
			puts(sSubWFName);

			// Sub-Workflow Task Instruction Name
std: stringstream ss_SubWFTaskInstructionsName;
			ss_SubWFTaskInstructionsName << sChangeNoticeItemID << " - " << saSubProcessNames[iIdx1];
			std::string s_SubWFTaskInstructionsName       = ss_SubWFTaskInstructionsName.str( );
			std::string s_SubWFTaskInstructionsName_Short = ss_SubWFTaskInstructionsName.str( );

			if ( s_SubWFTaskInstructionsName_Short.length( ) > 40 )
			{
				s_SubWFTaskInstructionsName_Short = s_SubWFTaskInstructionsName_Short.substr(0, 37);
				s_SubWFTaskInstructionsName_Short.append("...");
			}

			//char sSubWFTaskInstructionsName[200];
			//strcpy ( sSubWFTaskInstructionsName , "Task-Instructions for '" ) ;
			//strcat ( sSubWFTaskInstructionsName , sChangeNoticeItemID ) ;
			//strcat ( sSubWFTaskInstructionsName , "'" ) ;
			//puts ( sSubWFTaskInstructionsName ) ;

			// Create Dataset
			tag_t tDatasetType             = NULLTAG;
			tag_t tDatasetTool             = NULLTAG;
			tag_t tTaskInstructionsDataset = NULLTAG;

			ITK(AE_find_datasettype("TEXT", &tDatasetType) );
			ITK(AE_ask_datasettype_def_tool(tDatasetType, &tDatasetTool) );

			ITK(AE_create_dataset_with_id(tDatasetType, s_SubWFTaskInstructionsName_Short.c_str( ), s_SubWFTaskInstructionsName.c_str( ), "Dataset ID", "Dataset Rev", &tTaskInstructionsDataset) );
			ITK(AE_set_dataset_tool(tTaskInstructionsDataset, tDatasetTool) );
			ITK(AE_set_dataset_format(tTaskInstructionsDataset, "BINARY_REF") );
			ITK(AOM_save(tTaskInstructionsDataset) );

			// Assign attachments
			tag_t taAttachments[2] = { tChangeNoticeRev, tTaskInstructionsDataset };
			int   iaAttachTypes[2] = { EPM_target_attachment, EPM_reference_attachment };

			// Create Sub-Workflow and attach the ECO (target) and Instruction-Dataset (reference)
			ITK(EPM_create_process(sSubWFName, saSubProcessNames[iIdx1], tProcessTemplate, 2, taAttachments, iaAttachTypes, &tNewProcess) );
		}
	}
	else
	{
		retcode = 123456;                 // TODO
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_co_start_wp */


/**
 * @brief
 * Trigger Change Order main Work Processes
 */
int SBT_AH_co_trigger_main_wf(EPM_action_message_t msg)
{
	const char *theFunction      = "SBT_AH_co_trigger_main_wf";
	int         retcode          = ITK_ok;
	int         iTargetCount     = 0;
	tag_t       rootTaskTag      = NULLTAG;
	tag_t *     pTargetTags      = NULL;
	tag_t       tChangeNoticeRev = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask + Attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			int    n_referencers;
			int *  levels;
			tag_t *referencers;
			char **relations;

			ITK(WSOM_where_referenced(pTargetTags[iIdx1], 1, &n_referencers, &levels, &referencers, &relations) );

			for ( int iIdx2 = 0; iIdx2 < n_referencers; iIdx2++ )
			{
				char sObjectType2[WSO_name_size_c + 1];
				ITK(WSOM_ask_object_type(referencers[iIdx2], sObjectType2) );

				if ( strcmp(sObjectType2, sEPMTask) == 0 )
				{
					char sName[WSO_name_size_c + 1];
					ITK(EPM_ask_name(referencers[iIdx2], sName) );

					if ( strcmp(sName, sChangeOrderProcess) == 0 )
					{
						int    iSubTaskCount = 0;
						tag_t *taTasks       = NULL;

						ITK(EPM_ask_sub_tasks(referencers[iIdx2], &iSubTaskCount, &taTasks) );

						for ( int iIdx3 = 0; iIdx3 < iSubTaskCount; iIdx3++ )
						{
							EPM_state_t state;

							ITK(EPM_ask_state(taTasks[iIdx3], &state) );

							//printf("state = %i \n", state);
							if ( state == EPM_started )
							{
								//ITK ( AOM_lock( taTasks[iIdx3] ) );
								//ITK ( EPM_set_task_result ( taTasks[iIdx3], EPM_RESULT_True ) );
								//ITK ( AOM_set_value_int ( taTasks[iIdx3], "condition_result", EPM_RESULT_TRUE ) );
								//ITK ( AOM_save ( taTasks[iIdx3] ) );
								//ITK ( AOM_refresh ( taTasks[iIdx3], true ) );
								//ITK ( EPM_set_task_result ( taTasks[iIdx3], EPM_RESULT_Completed ) );
								//ITK ( EPM_trigger_action ( taTasks[iIdx3], EPM_complete_action, "Triggered by custom code ..." ) );
							}

							//break;
						}

						// Free
						if ( taTasks != NULL )
						{
							MEM_free(taTasks);
						}
					}
				}
			}                         /* end for*/
		}
	}

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_co_trigger_main_wf */

/**
 * @brief
 * Change ownership of text dataset in task references
 */
int SBT_AH_co_change_reference_txt_owner(EPM_action_message_t msg)
{
	const char *theFunction      = "SBT_AH_co_trigger_main_wf";
	int         iRetcode         = ITK_ok;
	int         iReferenceCount  = 0;
	tag_t       rootTaskTag      = NULLTAG;
	tag_t *     pReferenceTags   = NULL;
	tag_t       tChangeNoticeRev = NULLTAG;
	logical		lFromSignoff	 = false;
	logical		lFromRootTask	 = false;
	tag_t		tOwner			 = NULLTAG;
	tag_t		tGroup			 = NULLTAG;

	// avoid infinite loops
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

    const int   arg_count = TC_number_of_arguments(msg.arguments);

    TRACE1("SBT: Found %d arguments\n", arg_count);

    if (arg_count <= 0 )
    {
      	iRetcode = EPM_missing_req_arg;
    }

  	if (iRetcode == ITK_ok)
    {
      	ITK(EPM_setup_parser(msg.task));
    }

    for (int i = 0; i < arg_count && iRetcode == ITK_ok; i++)
    {
        char* arg = TC_next_argument(msg.arguments);
        char* flag = NULL;
        char* value = NULL;

        ITK(ITK_ask_argument_named_value((const char*) arg, &flag, &value));
        TRACE3("SBT: Argument %d: flag: %s, value: %s\n", i, flag ? flag:"NULL", value ? value:"NULL");

        if (strcmp(flag, "source") == 0)
        {
            if (stricmp(value, "signoff") == 0)
            {
                lFromSignoff = true;

                int 		iSignOffcount 	= 0;
            	tag_t *     pSignOffTags    = NULL;

                ITK(EPM_ask_attachments(msg.task, EPM_signoff_attachment, &iSignOffcount, &pSignOffTags) );
                if (iSignOffcount > 0)
                {
                	tag_t tSignOff = pSignOffTags[0];
                	ITK(AOM_ask_owner(tSignOff, &tOwner));
                	ITK(AOM_ask_group(tSignOff, &tGroup));
                }
            }
            else if (stricmp(value, "task") == 0)
            {
            	lFromRootTask = true;

            	tag_t tRootTask	= NULLTAG;

            	ITK(EPM_ask_root_task(msg.task, &tRootTask) );

                if (tRootTask != NULLTAG)
                {
                	ITK(AOM_ask_owner(tRootTask, &tOwner));
                	ITK(AOM_ask_group(tRootTask, &tGroup));
                }
            }
            else
            {
            	iRetcode = EPM_invalid_argument;
            }
        }
        else
        {
            iRetcode = EPM_invalid_argument;
        }

        MEM_free(flag);
      	MEM_free(value);
    }
    
  	if (iRetcode == ITK_ok && tOwner != NULLTAG && tGroup != NULLTAG)
    {
		/* Get RootTask + Attachments */
		ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
		ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );
		ITK(ITK_set_bypass(true));
		
		/* Loop over all target attachments */
		for ( int iIdx1 = 0; iIdx1 < iReferenceCount; iIdx1++ )
		{
			char sObjectType[WSO_name_size_c + 1];
			ITK(WSOM_ask_object_type(pReferenceTags[iIdx1], sObjectType) );

			if ( strcmp(sObjectType, "Text") == 0 )
			{
				ITK(AOM_refresh(pReferenceTags[iIdx1], true));
				ITK(AOM_set_ownership( pReferenceTags[iIdx1], tOwner, tGroup));
				ITK(AOM_refresh(pReferenceTags[iIdx1], false));
			}
		}
    }

	/* Free */
	SAFE_SM_FREE(pReferenceTags);

	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_co_change_reference_txt_owner */


/**
* @brief
* Verifies if the current user is logged in as "Change Control Manager"
*/
EPM_decision_t SBT_RH_co_is_change_control_manager(EPM_rule_message_t msg)
{
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	int         iIdx1              = 0;
	int         iIdx2              = 0;
	const char *theFunction        = "SBT_RH_co_is_change_control_manager";
	EPM_decision_t	decision	   = EPM_go;
	tag_t		tCurrentRole	   = NULLTAG;
    char		sRoleName[SA_name_size_c + 1];

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Return if msg.tag is one of the attachment types */
    if ( msg.proposed_action == EPM_add_attachment_action )
    {
        if ( ( msg.tag == EPM_target_attachment ) ||
             ( msg.tag == EPM_reference_attachment ) ||
             ( msg.tag == EPM_signoff_attachment ) ||
             ( msg.tag == EPM_release_status_attachment ) ||
             ( msg.tag == EPM_comment_attachment ) ||
             ( msg.tag == EPM_instruction_attachment ) ||
             ( msg.tag == EPM_interprocess_task_attachment ) ||
             ( msg.tag == EPM_project_task_attachment ) ||
             ( msg.tag == EPM_user_attachment ) 
           )
		{
			return EPM_go;
		}
	}

	// Issue 964. ECO - Solution Item WIP, ECO can not be approved, TXT file in Ref Folder is missing.
	// Return if the "Error Report" is to be cut from task reference
    if (msg.proposed_action == EPM_remove_attachment_action)
    {
		if (msg.tag == EPM_reference_attachment)
		{
			char sTaskName[WSO_name_size_c+1]	= "";
			tag_t tTask = msg.task;

			ITK(EPM_ask_name(tTask, sTaskName));

			if (stricmp(sTaskName, "BT Change Order") == 0)
			{
				// Check the task status of Task ECO accepted?
				tag_t			tSubTask	= NULLTAG;
				EPM_state_t		sTaskStatus	= EPM_unassigned;

				ITK(EPM_ask_sub_task(tTask, "ECO accepted?", &tSubTask));
				ITK(EPM_ask_state(tSubTask, &sTaskStatus ));

				if (sTaskStatus == EPM_started)
				{
					return EPM_go;
				}

				// Check the task status of Task Solution Items OK?
				tSubTask	= NULLTAG;
				sTaskStatus	= EPM_unassigned;

				ITK(EPM_ask_sub_task(tTask, "Solution Items OK?", &tSubTask));
				ITK(EPM_ask_state(tSubTask, &sTaskStatus ));

				if (sTaskStatus == EPM_started)
				{
					return EPM_go;
				}
			}

		}
	}

	// Ask current role
	ITK(SA_ask_current_role(&tCurrentRole));

	// Ask role name
	ITK(SA_ask_role_name(tCurrentRole, sRoleName));

	// Set result
	if ( strcmp(sRoleName, "Change Control Manager") != 0)
	{
		ITK(EMH_store_initial_error(EMH_severity_error, 919724));
		decision = EPM_nogo;
	}

	LEAVE(theFunction);
	return decision;
} /* End SBT_RH_co_is_change_control_manager */


/**
* @brief
* Verifies if solution items exist
*/
EPM_decision_t SBT_RH_co_solution_items_exist(EPM_rule_message_t msg)
{
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	int         iIdx1              = 0;
	int         iIdx2              = 0;
	const char *theFunction        = "SBT_RH_co_solution_items_exist";
	char *      sDecision          = EPM_RESULT_False;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       objectClassIdTag   = NULLTAG;
	tag_t *     pTargetTags        = NULLTAG;
	logical     lSolutionItemsExist = false;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			int		iSolutionCount			= 0;
			tag_t	*taSolutionItems		= NULL;
			tag_t	tRelTypeSolutionItems	= NULLTAG;

			// Find Solution Items
			ITK(GRM_find_relation_type("CMHasSolutionItem", &tRelTypeSolutionItems));
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tRelTypeSolutionItems, &iSolutionCount, &taSolutionItems));

			// Solution Items exist ?
			if(iSolutionCount > 0)
			{
				lSolutionItemsExist = true;
			}

			// Free
			SAFE_SM_FREE(taSolutionItems);

			// Break
			if ( lSolutionItemsExist == true )
			{
				break;
			}
		}
	}

	// Set result
	if ( lSolutionItemsExist == true )
	{
		sDecision = EPM_RESULT_True;
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
} /* End SBT_RH_co_solution_items_exist */


/**
 * @brief
 * Verifies if all related work packages are closed
 */
EPM_decision_t SBT_RH_co_check_wp(EPM_rule_message_t msg)
{
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	int         iIdx1              = 0;
	int         iIdx2              = 0;
	const char *theFunction        = "SBT_RH_co_check_wp";
	char *      sDecision          = EPM_RESULT_False;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       objectClassIdTag   = NULLTAG;
	tag_t *     pTargetTags        = NULLTAG;
	logical     lAllSubWFsComplete = true;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Loop over all target attachments */
	for ( iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			int    n_referencers;
			int *  levels;
			tag_t *referencers;
			char **relations;

			ITK(WSOM_where_referenced(pTargetTags[iIdx1], 1, &n_referencers, &levels, &referencers, &relations) );

			for ( iIdx2 = 0; iIdx2 < n_referencers; iIdx2++ )
			{
				char sObjectType2[WSO_name_size_c + 1];
				ITK(WSOM_ask_object_type(referencers[iIdx2], sObjectType2) );

				if ( strcmp(sObjectType2, sEPMTask) == 0 )
				{
					char sName[WSO_name_size_c + 1];
					ITK(EPM_ask_name(referencers[iIdx2], sName) );

					if ( strcmp(sName, sChangeOrderSubProcess) == 0 )
					{
						EPM_state_t wfState;

						// Refresh WF-Task
						ITK(AOM_refresh(referencers[iIdx2], false));

						// Ask Status
						ITK(EPM_ask_state(referencers[iIdx2], &wfState) );

						// WF-Task completed ?
						if ( wfState != EPM_completed )
						{
							lAllSubWFsComplete = false;
							break;
						}
					}
				}
			}                         /* end for*/

			if ( lAllSubWFsComplete == false )
			{
				break;
			}
		}
	}         /* end for*/

	// Set result
	if ( lAllSubWFsComplete == true )
	{
		sDecision = EPM_RESULT_True;
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
}

/**
* @brief
* Verifies that all solution items have status 10 or 60 (does not have to be the latest status)
*/
EPM_decision_t SBT_RH_co_checkSolutionStatusBeforeEcoApproval(EPM_rule_message_t msg)
{
	int					retcode            = ITK_ok;
	int					iTargetCount       = 0;
	int					iReferenceCount      = 0;
	const char		   *theFunction        = "SBT_RH_co_checkSolutionStatusBeforeEcoApproval";
	char *				sDecision          = EPM_RESULT_True;
	tag_t				rootTaskTag        = NULLTAG;
	tag_t				objectClassIdTag   = NULLTAG;
	tag_t *				pTargetTags        = NULLTAG;
	tag_t *				pReferenceTags     = NULLTAG;
	logical				lAllSolutionsOk	   = true;
	std::stringstream	txtStringStream;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get root task and target attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Init Relation Types */
	tag_t tSolutionItemsRelType = NULLTAG;
	ITK(GRM_find_relation_type("CMHasSolutionItem", &tSolutionItemsRelType) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			tag_t	*taSolutions					= NULL;
			int		iSolutionCount					= 0;
			char	*sChangeObjId					= NULL;

			// Get the change object id and revision
			ITK(WSOM_ask_object_id_string(pTargetTags[iIdx1], &sChangeObjId) );

			// Identify Solution Items
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tSolutionItemsRelType, &iSolutionCount, &taSolutions) );

			// Loop over Solution Items
			for ( int iIdx2 = 0; iIdx2 < iSolutionCount; iIdx2++ )
			{
				int		iRevisionCountOfSolutionItem	= 0;
				tag_t	*taRevisionsOfSolutionItem		= NULL;
				tag_t	tSolutionItem					= NULLTAG;
				tag_t	tLatestSolutionItemRevision		= NULLTAG;

				// If more than one Item Revision from the same Item Revision is attached, only check the latest one
				ITK(ITEM_ask_item_of_rev(taSolutions[iIdx2], &tSolutionItem));
				ITK(ITEM_list_all_revs(tSolutionItem, &iRevisionCountOfSolutionItem, &taRevisionsOfSolutionItem));

				// Loop backwards over Revisions of Solution Items
				for ( int iIdx3 = iRevisionCountOfSolutionItem - 1; iIdx3 >= 0; iIdx3-- )
				{
					// Loop over Solution Items
					for ( int iIdx4 = 0; iIdx4 < iSolutionCount; iIdx4++ )
					{
						if(taSolutions[iIdx4] == taRevisionsOfSolutionItem[iIdx3])
						{
							tLatestSolutionItemRevision = taRevisionsOfSolutionItem[iIdx3];
							break;
						}
					}					

					// Exit Loop ?
					if(tLatestSolutionItemRevision != NULLTAG)
					{
						break;
					}
				}

				// Current object is the latest solution revision of this particular item?
				if(tLatestSolutionItemRevision == taSolutions[iIdx2])
				{
					char	*sStatusName	= NULL;
					int		iStatusCount	= 0;
					char	*sObjectId		= NULL;

					// Get the change object id and revision
					ITK(WSOM_ask_object_id_string(taSolutions[iIdx2], &sObjectId) );

					askLatestReleaseStatus(taSolutions[iIdx2], &sStatusName, &iStatusCount);

					if(iStatusCount != 0)
					{						
						txtStringStream << "status name is <" << sStatusName<<endl;
					}
					else
					{
						txtStringStream << "no status name or object is in processs <" <<endl;
					}
															
					// Allowed status are 10,60,BT7_ECO,98
					if(iStatusCount == 0 || (strcmp(sStatusName, STATUS_TYPE_10) != 0 && strcmp(sStatusName, STATUS_TYPE_60) != 0 && strcmp(sStatusName, STATUS_TYPE_98) != 0 && strcmp(sStatusName, "Pre Release+") != 0))
					{
						lAllSolutionsOk = false;
						txtStringStream << "Under Change Object id <" << sChangeObjId
							<< ">, the solution object id <" << sObjectId
							<< "> doesn't contain release status: Pre Release or Serial Release or Pre Release+ or Annulled." << endl;
					}

					// Free
					SAFE_SM_FREE(sStatusName);
				}

				// Free
				SAFE_SM_FREE(taRevisionsOfSolutionItem);
			}

			// Free
			SAFE_SM_FREE(taSolutions);
		}

		// Exit loop if an invalid solution has been found
		if(lAllSolutionsOk == false)
		{			
			//printf(" > ERROR: Impacted IR %s/%s does not have a higher Solution IR! \n", sImpactedID, sImpactedRevID);

			// Set result
			sDecision = EPM_RESULT_False;

			//break;
		}
	}

	// Issue 964. ECO - Solution Item WIP, ECO can not be approved, TXT file in Ref Folder is missing.
	// Create error report and attacht as text dataset to the reference of the WF.

	// Remove all old error report dataset under reference
	ITK(EPM_ask_attachments(rootTaskTag, EPM_reference_attachment, &iReferenceCount, &pReferenceTags) );

	int		iErrorReportCount	=0;
	tag_t	*taAttachments		= (tag_t*)MEM_alloc(iReferenceCount * sizeof(tag_t));

	/* Loop over all reference attachments */
	for ( int iIdx2 = 0; iIdx2 < iReferenceCount; iIdx2++ )
	{
		char	sObjectName[WSO_name_size_c+1]	= "";

		ITK(WSOM_ask_name(pReferenceTags[iIdx2], sObjectName));

		if (strcmp(sObjectName, "Error Report") == 0)
		{
			// Remove Attachment
			taAttachments[iErrorReportCount] = pReferenceTags[iIdx2];
			iErrorReportCount++;
		}
	}

	// Remove the datasets from the task reference
	ITK(EPM_remove_attachments(rootTaskTag, iErrorReportCount, taAttachments));

	int iLockType = -1;
	ITK(POM_ask_instance_lock(rootTaskTag, &iLockType));

	if (iLockType != POM_modify_lock)
	{
		ITK(AOM_lock(rootTaskTag));
		ITK(AOM_save(rootTaskTag));
		ITK(AOM_unlock(rootTaskTag));
	}
	else
	{
		ITK(AOM_save(rootTaskTag));
	}

	//  Try to delete the removed datasets
	for ( int iIdx3 = 0; iIdx3 < iErrorReportCount; iIdx3++ )
	{
		int iCanBeLocked = AOM_lock_for_delete(taAttachments[iIdx3]);

		if (iCanBeLocked == ITK_ok)
		{
			// try to delete, if error happens just ignore it.
			AOM_delete(taAttachments[iIdx3]);
		}
	}

	/* Free */
	SAFE_SM_FREE(pReferenceTags);
	SAFE_SM_FREE(taAttachments);

	// Attach a new error dataset if error occurres
	if (stricmp(sDecision, EPM_RESULT_False) == 0)
	{
		//create a new ImanFile
		tag_t      txtFile     = NULLTAG;
		IMF_file_t txtFileDesc = NULL;

		ITK(IMF_create_file(NULL, SS_TEXT, &txtFile, &txtFileDesc) );
		ITK(IMF_open_file(txtFileDesc, SS_RDWRITE) );

		//write the XML-Stream to the new ImanFile
		std::string txtStringLine = "";

		while ( getline(txtStringStream, txtStringLine, '\n') )
		{
			if ( txtStringLine.length( ) <= SS_MAXLLEN )
			{
				ITK(IMF_write_file_line(txtFileDesc, txtStringLine.c_str( ) ) );
			}
		}

		//Close and save the new ImanFile
		ITK(IMF_close_file(txtFileDesc) );
		ITK(AOM_save(txtFile) );
		ITK(AOM_unload(txtFile) );

		//create the dataset and upload the dataset
		createDataset(msg.task, txtFile, "Error Report");
	}

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;			
}

/**
 * @brief
 * Verifies if all impacted items have been revised in the solutions
 */
EPM_decision_t SBT_RH_co_checkSolutionForImpactedItemRev(EPM_rule_message_t msg)
{
	int         retcode            = ITK_ok;
	int         iTargetCount       = 0;
	const char *theFunction        = "SBT_RH_co_checkSolutionForImpactedItemRev";
	char *      sDecision          = EPM_RESULT_True;
	tag_t       rootTaskTag        = NULLTAG;
	tag_t       objectClassIdTag   = NULLTAG;
	tag_t *     pTargetTags        = NULLTAG;
	logical     lAllSubWFsComplete = true;

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get reference attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );

	if ( ( retcode == ITK_ok ) && ( rootTaskTag != NULLTAG ) )
	{
		ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );
	}

	/* Init Relation Types */
	tag_t tSolutionItemsRelType = NULLTAG;
	tag_t tImpactedItemsRelType = NULLTAG;
	ITK(GRM_find_relation_type("CMHasSolutionItem", &tSolutionItemsRelType) );
	ITK(GRM_find_relation_type("CMHasImpactedItem", &tImpactedItemsRelType) );

	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			tag_t *taImpacted     = NULL;
			int    iImpactedCount = 0;

			// Identify Impacted Items
			ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tImpactedItemsRelType, &iImpactedCount, &taImpacted) );

			// Loop over Impacted Items
			for ( int iIdx2 = 0; iIdx2 < iImpactedCount; iIdx2++ )
			{
				// Identify corresponding Solution Item Revision
				char    sImpactedRevID[ITEM_id_size_c + 1];
				char    sImpactedID[ITEM_id_size_c + 1];
				int     iSolutionCount      = 0;
				tag_t * taSolutions         = NULL;
				tag_t   tImpactedItem       = NULLTAG;
				logical bValidSolutionFound = false;

				// Init Impacted
				ITK(ITEM_ask_item_of_rev(taImpacted[iIdx2], &tImpactedItem) );
				ITK(ITEM_ask_id(tImpactedItem, sImpactedID) );
				ITK(ITEM_ask_rev_id(taImpacted[iIdx2], sImpactedRevID) );

				// Identify Solution Items
				ITK(GRM_list_secondary_objects_only(pTargetTags[iIdx1], tSolutionItemsRelType, &iSolutionCount, &taSolutions) );

				for ( int iIdx3 = 0; iIdx3 < iSolutionCount; iIdx3++ )
				{
					char  sSolutionRevID[ITEM_id_size_c + 1];
					tag_t tSolutionItem = NULLTAG;

					// Init Soution
					ITK(ITEM_ask_rev_id(taSolutions[iIdx3], sSolutionRevID) );
					ITK(ITEM_ask_item_of_rev(taSolutions[iIdx3], &tSolutionItem) );

					// Matching Item found ?
					if ( tImpactedItem == tSolutionItem )
					{
						if ( strcmp(sImpactedRevID, sSolutionRevID) < 0 )
						{
							bValidSolutionFound = true;
							break;
						}
					}
				}

				if ( bValidSolutionFound == false )
				{
					printf(" > ERROR: Impacted IR %s/%s does not have a higher Solution IR! \n", sImpactedID, sImpactedRevID);

					// Set result
					sDecision = EPM_RESULT_False;

					break;
				}

				// Free
				SAFE_SM_FREE(taSolutions);
			}

			// Free
			SAFE_SM_FREE(taImpacted);
		}     /* end if*/
	}         /* end for*/

	// Set the task result
	ITK(EPM_set_task_result(msg.task, sDecision) );
	ITK(AOM_save(msg.task) );
	ITK(AOM_unlock(msg.task) );

	/* Free */
	SAFE_SM_FREE(pTargetTags);

	LEAVE(theFunction);
	return EPM_go;
}


/**
 * @brief
 * Notify the assigned expert, not the change specialist!
 */
int SBT_AH_notify_expert(EPM_action_message_t msg)
{
	const char *theFunction       = "SBT_AH_notify_expert";
	char *      sUserName         = NULL;
	int         retcode           = ITK_ok;
	int         iTargetCount      = 0;
	int         iSignoffCount     = 0;
	tag_t *     taSignoffs        = NULL;
	tag_t       rootTaskTag       = NULLTAG;
	tag_t *     pTargetTags       = NULL;
	tag_t       tChangeNoticeRev  = NULLTAG;
	tag_t       tOwningUser       = NULLTAG;
	tag_t       tSignoffUser      = NULLTAG;
	tag_t       tResponsibleParty = NULLTAG;

	/* Avoid infinite loops */
	if ( msg.action > EPM_perform_action )
	{
		return ITK_ok;
	}

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	/* Get RootTask + Attachments */
	ITK(EPM_ask_root_task(msg.task, &rootTaskTag) );
	ITK(EPM_ask_attachments(rootTaskTag, EPM_target_attachment, &iTargetCount, &pTargetTags) );

	/* Get Signoff User */
	ITK(EPM_ask_attachments(msg.task, EPM_signoff_attachment, &iSignoffCount, &taSignoffs) );

	for ( int iIdx1 = 0; iIdx1 < iSignoffCount; iIdx1++ )
	{
		tag_t          tMember = NULLTAG;
		SIGNOFF_TYPE_t tSignoffType;

		ITK(EPM_ask_signoff_member(taSignoffs[iIdx1], &tMember, &tSignoffType) );

		if ( tSignoffType == SIGNOFF_GROUPMEMBER )
		{
			ITK(SA_ask_groupmember_user(tMember, &tSignoffUser) );
			break;
		}
	}

	/* Set responsible party */
	if ( tSignoffUser != NULLTAG )
	{
		ITK(EPM_assign_responsible_party(msg.task, tSignoffUser) );
		ITK(EPM_assign_responsible_party(rootTaskTag, tSignoffUser) );
	}

	/* Get responsible party*/
	ITK(EPM_ask_responsible_party(msg.task, &tResponsibleParty) );

	/* Get owning user of the root task */
	ITK(AOM_ask_value_tag(rootTaskTag, "owning_user", &tOwningUser) );
	ITK(AOM_ask_value_string(tOwningUser, "user_name", &sUserName) );

	/* Owning user != responsible party ? */

	//if(tOwningUser != tResponsibleParty) {
	/* Loop over all target attachments */
	for ( int iIdx1 = 0; iIdx1 < iTargetCount; iIdx1++ )
	{
		char sObjectType[WSO_name_size_c + 1];
		ITK(WSOM_ask_object_type(pTargetTags[iIdx1], sObjectType) );

		if ( strcmp(sObjectType, sBT7_ChangeNoticeRev) == 0 )
		{
			tag_t tItem = NULLTAG;
			char  sECOid[ITEM_id_size_c + 1];
			char  sECORevID[ITEM_id_size_c + 1];
            char  sECOObjName[ITEM_name_size_c + 1];

			// Get ItemID
			ITK(ITEM_ask_item_of_rev(pTargetTags[iIdx1], &tItem) );
			ITK(ITEM_ask_id(tItem, sECOid) );
			ITK(ITEM_ask_rev_id(pTargetTags[iIdx1], sECORevID) );
			ITK(ITEM_ask_name(tItem,sECOObjName));

			// Change Notice identified ...
			// 1.) Identify all non-finished sub-tasks
			// 2.) Generate a message string to inform the Change Specialist which tasks are WIP

			int    n_referencers = 0;
			int *  levels        = NULL;
			tag_t *referencers   = NULL;
			char **relations     = NULL;
			ITK(WSOM_where_referenced(pTargetTags[iIdx1], 1, &n_referencers, &levels, &referencers, &relations) );

			/* Mail*/
			int          iOpenTasks = 0;
			stringstream ss_MailOpenTasks;
			stringstream ss_MailContent;
			stringstream ss_MailSubject;

			ss_MailSubject << sECOid << " task '";
			ss_MailContent << "The following task has been assigned to you by '" << sUserName << "': ";

			// Loop referencers
			for ( int iIdx2 = 0; iIdx2 < n_referencers; iIdx2++ )
			{
				char sObjectType2[WSO_name_size_c + 1];
				ITK(WSOM_ask_object_type(referencers[iIdx2], sObjectType2) );

				if ( strcmp(sObjectType2, sEPMTask) == 0 )
				{
					char sName[WSO_name_size_c + 1];
					ITK(EPM_ask_name(referencers[iIdx2], sName) );

					if ( strcmp(sName, sChangeOrderSubProcess) == 0 )
					{
						EPM_state_t wfState;
						tag_t       tJob;
						char        sTaskName[WSO_name_size_c + 1];
						char        sJobDesc[WSO_desc_size_c + 1];

						ITK(EPM_ask_state(referencers[iIdx2], &wfState) );
						ITK(EPM_ask_name(referencers[iIdx2], sTaskName) );

						ITK(EPM_ask_job(referencers[iIdx2], &tJob) );
						ITK(WSOM_ask_description(tJob, sJobDesc) );

						if ( referencers[iIdx2] == rootTaskTag )
						{
							//current task
							ss_MailSubject << sJobDesc << "' assigned to you";
							ss_MailContent << sJobDesc;

							break;
						}
					}
				}
			}                                 /* end for*/

			ss_MailContent << "\n\n> " << sECOid << " | "
						   << sECORevID << " | " << sECOObjName;

			// Append the link of the target items in the email
			char*   url = NULL;
			
			// Change 15.10.2015: Pref of Mailserver is not set in some Sites.
			int iPrefCount = 0;
			ITK(PREF_ask_value_count("WEB_default_site_server", &iPrefCount));
			if ( iPrefCount > 0 )
			{
				ITK(TC_tag_to_url(pTargetTags[iIdx1], AWC, &url));
			}

			if ( url != NULL )
			{
				ss_MailContent << "\n\nTarget Object: " << url << endl;
			}
			else
			{
				TRACE1("url is NULL for tag %i", pTargetTags[iIdx1]);
			}

			MEM_free(url);

			ss_MailContent << endl << "**********";
			ss_MailContent << endl << "Please ignore Rich Client Link";
			ss_MailContent << endl << "**********";

			/* Shorten subject and content */
			string sMailSubject = ss_MailSubject.str( );

			if ( sMailSubject.length( ) > 128 )
			{
				sMailSubject = sMailSubject.substr(0, 128);
			}

			string sMailContent = ss_MailContent.str( );

			if ( sMailContent.length( ) > 240 )
			{
				sMailContent = sMailContent.substr(0, 240);
			}

			/* Send Mail */
			if ( tSignoffUser != NULLTAG )
			{
				tag_t tMail = NULLTAG;
				createMail(sMailSubject.c_str( ), sMailContent.c_str( ), &tMail);
				addReceiverToMail(tMail, tSignoffUser);
				sendEmailNotification(tMail);
			}
			else
			{
				printf(" > ERROR: tSignoffUser == NULLTAG !!! \n");
			}

			/* Free */
			SAFE_SM_FREE(levels);
			SAFE_SM_FREE(referencers);
			SAFE_SM_FREE(relations);

			/* Break */
			break;
		}
		else
		{
			//
		}
	}

	//} else {
	/* Handler shall be executed if the task is assigned to another user */

	//(&msg)->proposed_state = EPM_unassigned;
	//(&msg)->action = EPM_notify_action;
	//POMRef<Teamcenter::EPMTaskImpl> task(msg.task);
	//}

	//ITK ( TCEVENTMGR_post_event (msg.task, TC_WORKFLOW_NOTIFY_EVENT ) );


	/* Free */
	SAFE_SM_FREE(pTargetTags);
	SAFE_SM_FREE(sUserName);
	SAFE_SM_FREE(taSignoffs);

	// Clear Error Stack
	ITK(EMH_clear_errors());

	/* Exit */
	LEAVE(theFunction);
	return retcode;
} /* End SBT_AH_notify_expert */


/*Custom ECO release */






/* Added by Saranya S
Date : 29-07-2016
*/

int SBT_Solution_item_eco_status(EPM_action_message_t msg)
{
	const char	*theFunction			= "SBT_Solution_item_eco_status";
	char		sParamReleaseStatus[ITEM_id_size_c + 1] = "";
	char*		objectType				= NULL;//(OF)
	char*		jobName					= NULL;//(OF)
	//char		*prop_value				= NULL;
	char*		itemIdValue				= NULL;//(OF)
	int         retcode					= ITK_ok;	
	int			count					= 0;
	int			attachCount				= 0;
	tag_t		rootTask				= NULLTAG;
	tag_t		job						= NULLTAG;	
	tag_t		tRelationTag			= NULLTAG ;
	tag_t*		targetAttachments		= NULL;	//(OF)	          
	//tag_t		*sec_objects			= {NULLTAG};	
	

	ENTER(theFunction);

	ITK(EMH_clear_errors( ) );

	EPM_decision_t decision = EPM_go;
	
    ITK(EPM_ask_job (msg.task,&job));
	ITK(EPM_ask_root_task (job, &rootTask));
	ITK(EPM_ask_attachments(rootTask, EPM_target_attachment, &attachCount, &targetAttachments));
	printf("\nAction handler Created");
	

	ITK(WSOM_ask_name2(job,&jobName));
	printf("******job name is %s\n", jobName);

	//check if target attachment count > 0 &  current job name is Serialization
	if( attachCount >0 && tc_strstr ( jobName, SBT_WF_JOBNAME_SERIALIZATION) != '\0' )	
	{
		tag_t tNewStatus = NULLTAG;
		tag_t	tSpRelationType			=	NULLTAG;

		ITK(RELSTAT_create_release_status(SBT_STATUS_BT7_ECO, &tNewStatus) );
		ITK(AOM_save(tNewStatus)) ;
		

		ITK(GRM_find_relation_type("IMAN_specification", &tSpRelationType));
		for(int i=0;i<attachCount;i++)
		{
				//tag_t	pPartRevClassIdTag			= NULLTAG;
				tag_t	pItemRevClassIdTag			= NULLTAG;
				tag_t	pCNRevClassIdTag			= NULLTAG;
				logical	isChild_OfItemRevision		= false;
				//logical	isORChildOfPartRev			= false;
				ITK( WSOM_ask_object_type2 (targetAttachments[i],&objectType));

				//check if target is of Class or descendent of ChangeNoticeRevision
				if(!(object_is_of_class_or_desc(targetAttachments[i],SBT_BT_CHANGENOTICEREVISION,&pCNRevClassIdTag)))
				{
					//isORChildOfPartRev = object_is_of_class_or_desc(targetAttachments[i],TC_PARTREVISION,&pPartRevClassIdTag);
					isChild_OfItemRevision = object_is_of_class_or_desc(targetAttachments[i],TC_ITEMREVISION,&pItemRevClassIdTag);

					//printf("Object type =  %s ,isORChildOfPartRev =  [%s]",objectType,isChild_OfItemRevision?"true":"false" );					
					if(isChild_OfItemRevision && !( tc_strcmp(objectType,SBT_BT_COMPPROGREVISION) ==0 || tc_strcmp(objectType,SBT_BT_ELCADREVISION) ==0 ))
					{
						//printf("Object type is %s \n", objectType);
						ITK(AOM_UIF_ask_value(targetAttachments[i],"object_string", &itemIdValue));
						//printf("\nitem id value %s \n", itemIdValue);

						int releaseCount = 0;
						char** releaseStatusListOfRev = NULL;//(OF)
						ITK(AOM_UIF_ask_values(targetAttachments[i],"release_status_list", &releaseCount, &releaseStatusListOfRev));

						for(int releaseCtr=0;releaseCtr<releaseCount;releaseCtr++)
						{
							//check if iterated release status is 10
							if( tc_strcmp ( releaseStatusListOfRev[releaseCtr], STATUS_TYPE_10) == '\0' )
							{
								printf("***********************");
								printf("\n\nitem id value %s \n", itemIdValue);

								ITK(setStatusToTargetRev(targetAttachments[i], tNewStatus ));
									
								//get tag of IMAN_specification
								int		iSpSecondaryCount		=	0;								
								tag_t*	taSpSecondaryObjects	=	NULL;//(OF)
								
								ITK(GRM_list_secondary_objects_only(targetAttachments[i], tSpRelationType, &iSpSecondaryCount, &taSpSecondaryObjects));
								for(int idx1=0;idx1<iSpSecondaryCount; idx1++)
								{
									ITK(setStatusToTargetRev(taSpSecondaryObjects[idx1], tNewStatus ));
								}

								// get BVR

								int iViews = 0;
								tag_t* tBOMViewRevs= NULL;
								ITK(ITEM_rev_list_bom_view_revs(targetAttachments[i],&iViews,&tBOMViewRevs));

								for(int i=0;i<iViews;i++)
								{
									ITK(setStatusToTargetRev(tBOMViewRevs[i], tNewStatus ));
								}




								SAFE_SM_FREE(taSpSecondaryObjects);								

							}// end of check of status 10
						}// end of for loop

						for(int i=0;i<releaseCount;i++)
						{
							SAFE_SM_FREE(releaseStatusListOfRev[i]);
						}// end of for loop for mem free
					}// end of check for object type 

				}// end of check for class or Desc of ChangeNoticeRevision			

		}// end of for loop for iteration of WF targets
	}// end of check for Serialization

	/* Free */
	SAFE_SM_FREE(jobName);
	SAFE_SM_FREE(itemIdValue);
	SAFE_SM_FREE(objectType);
	SAFE_SM_FREE(targetAttachments);
	
	// Clear Error Stack
	ITK(EMH_clear_errors());

	/* Exit */
	LEAVE(theFunction);
	return retcode;
} //end of handler function

int setStatusToTargetRev(tag_t targetObject, tag_t tNewStatus )
{
	int		retcode				=	ITK_ok;
	tag_t	*taReleaseObjects	=	(tag_t*)MEM_alloc(1 * sizeof(tag_t));		

	taReleaseObjects[0]= targetObject;

	if(taReleaseObjects != NULL)
	{
		ITK(RELSTAT_add_release_status(tNewStatus, 1, taReleaseObjects, false));
	}// check for taReleaseObjects not null

	return retcode;
}
