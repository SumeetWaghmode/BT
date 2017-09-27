/**
 * This is the general header file for the library SBT_Workflow.
 */

#ifndef _SBT_WORKFLOW_GROUPHANDLER_H
#define _SBT_WORKFLOW_GROUPHANDLER_H

#include <ict/ict_errors.h>
#include <tccore/workspaceobject.h>
#include <epm/epm.h>
#include <ss/ss_const.h>

#include <bmf/libuserext_exports.h>


#include "SBT_Dataset.h"



/** @brief The relation type with which the dataset is attached to the item revision holding the group dependent message for the workflow instruction. */
static const char *SBT_WF_groupdependent_rev_dataset_assignment = "IMAN_specification";

/** @brief The status which is set to release the propper Dataset for the group dependent message for the workflow instruction. */
//static const char *SBT_WF_groupdependent_rev_dataset_released_status = "Released";
//static const char *SBT_WF_groupdependent_rev_dataset_released_status = STATUS_TYPE_60;

/** @brief The name of the runtime property attached to the EPM task. */
static const char *SBT_WF_groupdependent_rtp_name = "group_task_string";

/** @brief The length of the runtime property attached to the EPM task. */
static const int SBT_WF_groupdependent_prop_len = 1023;




/** @brief The name of the runtime property attached to the ItemRevision. */
static const char *SBT_WF_revisionsuperseeded_rtp_name = "bt7_revisionsuperseeded";

/** @brief The length of the runtime property attached to the ItemRevision. */
static const int SBT_WF_revisionsuperseeded_prop_len = 2;





/** @brief The default message if no dataset is available */
static const char *SBT_WF_not_groupdependent_std_msg = "Für diese Gruppe gibt es keine eingenen Instructions!";

/** @brief The subject sent with the mail in the workflow handler */
static const char *SBT_WF_default_email_header = "Es wurde eine Freigabe durchgeführt.";

/** @brief The length of the attribute 'object_desc' on the EPMTask.
  * Hopefully we can alter it. */
static const int SBT_WF_object_desc_len = WSO_desc_size_c;

/*
 * The named references 
 */

/** @brief The dataset attached to the EPM task to hold the groupdependent information. */
static SBT_named_ref_t SBT_WF_groupdependent_info_attachment[] =
{
    {"Text", "Text", SS_TEXT, "Text", ".TXT" },
    {NULL,   NULL,   0,       NULL,   NULL   }
};

/**
 * @brief This is the array of the workflow names where we have to add groupdependent 
 *        datasets to.
 */
static const char *SBT_WF_groupdependent_wf_name_array[] =
{
    "00_Simple_WF_MR"
};
static const int SBT_WF_groupdependent_wf_number = sizeof(SBT_WF_groupdependent_wf_name_array) / sizeof(SBT_WF_groupdependent_wf_name_array[0]);

/**
 * The signature of the PLMeasy mail handler to call
 */
// Not able to find PLMEasy .lib files
//extern int TCPB_AH_send_mail(EPM_action_message_t msg);

int addReceiverToMail(tag_t mail, tag_t recipient);
int sendEmailNotification(tag_t tMail);
int createMail(const char *sMailSubject, const char *sMailMessage, tag_t *tMail);



/**
 * @brief
 * Processes the handlers given target objects [using the handlers given arguments].
 * The handler should give an instruction to the workflow target that is
 * dependent on the group the user has.
 * This task will add the datasets to the EPMTask so that the instructions
 * are available later on.
 *
 * @param[in] msg The message Object of the EPM-task
 */
int SBT_WF_instruction_by_group_action_handler
  (
	EPM_action_message_t msg
  );










char* SBT_WF_instruction_by_group_get_item_id(
                                             const char*    template_name,    /* I */
                                             const char*    group_name        /* I, null allowed */
                                             );


int SBT_WF_instruction_by_group_set_epmtask_args(
                                                tag_t        epmtask,        /* I */
                                                const char*  template_value, /* I */
                                                logical      group_value     /* I */
                                                );

int SBT_WF_instruction_by_group_get_epmtask_args(
                                                tag_t       epmtask,        /* I */
                                                char**      template_value, /* OF */
                                                logical*    group_value     /* O */
                                                );

/**
* @brief
* Verifies if the form BT7_CCR contains at least one Manufacturer plus if the relation 
* to the Manufacturer contains the information of the Mfg. Part Number and Part Name
*/
EPM_decision_t SBT_RH_checkCCRConformity(EPM_rule_message_t msg);


/**
* @brief
* Verify if the current user is a privileged member of the project defined by the parameter 'projectid'
*/
EPM_decision_t SBT_RH_isPrivilegedMemberOfProject(EPM_rule_message_t msg);


/**
* @brief
* Read the Manufacturer information and generate the Commercial Part + Vendor Part
*/
int SBT_AH_generateCCRObjects(EPM_action_message_t msg);

/**
* @brief
* Generate a temporary Commercial Part
*/
int SBT_AH_generateTempCCObjects(EPM_action_message_t msg);

/**
* @brief
* Replace the CCR-classification with the CC
*/
int SBT_AH_replaceCcrClassification(EPM_action_message_t msg);

/**
* @brief
* Fills the Name of the Resp Party into an Attribute of an ItemType
*/
int SBT_AH_fill_In_Resp_Party_Name_In_Attr(EPM_action_message_t msg);

/**
* @brief
* Creates a BT6_Product with an BT7_Product as imput
*/
int SBT_AH_change_product_type(EPM_action_message_t msg);

/**
* @brief
* Removes the targes
*/
int SBT_AH_remove_target_attachments(EPM_action_message_t msg);

/**
* @brief
* Workflow check target datasets
*/
EPM_decision_t SBT_RH_check_target_attachments_dataset(EPM_rule_message_t msg);

extern void AM__set_application_bypass(logical bypass);
extern void AM__ask_application_bypass(logical* bypass);

/**
* @brief
* Generate CADIM Name
*/
int SBT_AH_generateCadimName(EPM_action_message_t msg);

/**
* @brief
* Notify the Change Specialist after a Work Package has been finished
*/
int SBT_AH_notify_change_specialist(EPM_action_message_t msg);

/**
* @brief
* Notify the assigned expert, not the change specialist!
*/
int SBT_AH_notify_expert(EPM_action_message_t msg);

/**
* @brief
* Copy the ECO Nr to the Solution Items
*/
int SBT_AH_co_copyEcoNrToSolutions(EPM_action_message_t msg);

/**
* @brief
* Start Change Order Work Processes
*/
int SBT_AH_co_start_wp(EPM_action_message_t msg);

/*
Add overlay icon for Serialization task
*/

int SBT_Solution_item_eco_status(EPM_action_message_t msg);

/**
* @brief
* Set status Pre Release to PDF 
*/
int SBT_AH_set_status_PR_PDF(EPM_action_message_t msg);

/**
* @brief
* Trigger Change Order main Work Processes
*/
int SBT_AH_co_trigger_main_wf(EPM_action_message_t msg);

/**
* @brief
* Change ownership of text dataset in task references
*/
int SBT_AH_co_change_reference_txt_owner(EPM_action_message_t msg);

/**
* @brief
* Verifies if all related work packages are closed
*/
EPM_decision_t SBT_RH_co_check_wp(EPM_rule_message_t msg);

/**
* @brief
* Verifies if solution items exist
*/
EPM_decision_t SBT_RH_co_solution_items_exist(EPM_rule_message_t msg);

/**
* @brief
* Verifies if the current user is logged in as "Change Control Manager"
*/
EPM_decision_t SBT_RH_co_is_change_control_manager(EPM_rule_message_t msg);

/**
* @brief
* Verifies if all impacted items have been revised in the solutions
*/
EPM_decision_t SBT_RH_co_checkSolutionForImpactedItemRev(EPM_rule_message_t msg);

/**
* @brief
* Verifies that all solution items have status 10 or 60 (does not have to be the latest status)
*/
EPM_decision_t SBT_RH_co_checkSolutionStatusBeforeEcoApproval(EPM_rule_message_t msg);

/**
* @brief
* Verifies if a UGMASTER is related to the target item revision
*/
EPM_decision_t  SBT_RH_add_drawing_pdf_required(EPM_rule_message_t msg);

/**
* @brief
* Verifies if the target object is a document revision (BT6 or BT7)
*/
EPM_decision_t  SBT_RH_is_document(EPM_rule_message_t msg);

/**
* @brief
* Verifies if the target is the status released.
*/
EPM_decision_t  SBT_RH_check_target_released(EPM_rule_message_t msg);

/**
* @brief
* Verifies if the target is of the specified object type.
*/
EPM_decision_t  SBT_RH_check_target_type(EPM_rule_message_t msg);

/**
* @brief
* Prevents minor revisions as targets.
*/
EPM_decision_t  SBT_RH_prevent_minor_revision(EPM_rule_message_t msg);

/**
* @brief
* Only allow Common Component Engineers.
*/
EPM_decision_t  SBT_RH_current_user_is_common_comp_eng(EPM_rule_message_t msg);

/**
* @brief
* Checks if the target is a PartRevision or a Document with the status None, Prerelease(10) or Rejected(90).
* Attach the incorrect items to a text file and exit
*/
EPM_decision_t  SBT_RH_valid_for_release(EPM_rule_message_t msg);

/**
* @brief
* Verfifies if the target is a Part, checks if an Item or BOM is attached and has status Prerelease(10) or Released(60).
*/
EPM_decision_t  SBT_RH_part_has_released_bom_attached(EPM_rule_message_t msg);



/**
* @brief
* Export the meta data of an electronic component to identify later changes
*/
int SBT_AH_elec3d_export_meta_data(EPM_action_message_t msg);

/**
 * @brief
 * If WF Target has no status -> no action on WF Target
 * If WF Target has status 10 -> no action on WF Target
 * If WF Target has status 60 -> create new Minor Revision (e.g.: B -> B1, A2->A3), set Status 10 on new created Minor Revision, replace WF Target by new created Minor Revision as new WF Target
 */
int SBT_AH_elec3d_newMinorRevisionIfRequired(EPM_action_message_t msg);

/**
* @brief
* Assigns the responsible MCAD Designer based on the following rule:
* - If process owner is a member of the CCR-Designers -> Use the Ressource Pool "3D_EL-CAD_Design"
* - Otherwise assign the task to the process owner (who will be a MCAD Designer)
*/
int SBT_AH_elec3d_assignMcadDesigner(EPM_action_message_t msg);

/**
* @brief
* Verfifies if two meta data exports exists + if they are different
*/
EPM_decision_t SBT_RH_elec3d_compare_meta_data(EPM_rule_message_t msg);

/**
* @brief
* Verfifies if the 3D-workflow can by skipped (if the process owner is a member of the group PCBDesign + the target is a PCB-raw)
*/
EPM_decision_t SBT_RH_elec3d_bypass_check_for_pcbraw_possible(EPM_rule_message_t msg);

/**
* @brief
* Check whether selected item can be annulled
*/
EPM_decision_t SBT_RH_check_item_annullable(EPM_action_message_t msg);

/**
* @brief
* Attach die BOM childs for anulling
*/
int SBT_AH_attach_bom_childs_for_anulling(EPM_action_message_t msg);

/**
 * @brief
 * Distribute Documents in context of the Workflow "BT7_60_Production (ReleaseAndDistribute)"
 */
int SBT_AH_distribute_docs(EPM_action_message_t msg);

/**
 * @brief
 * Attach Item and all ItemRevisions for reactivation
 */
int SBT_AH_attach_item_and_all_revs(EPM_action_message_t msg);


/**
 * @brief
 * Assign the current task to the approver-role of the same group like the requestor is logged in to
 */
int SBT_AH_assign_to_approver_of_same_group(EPM_action_message_t msg);

/**
 * @brief
 * Set a release status only to specific object-types within the WF-targets and -references
 */
int SBT_AH_set_status_for_object_type(EPM_action_message_t msg);

/**
 * @brief
 * Assign the workflow targets to the project defined in the parameter
 */
int SBT_AH_assign_to_project(EPM_action_message_t msg);

/**
 * @brief
 * Send notification including target details
 */
int SBT_AH_send_notification(EPM_action_message_t msg);

/**
 * @brief
 * Attach all PDFs + all DocumentRevisions which are related as "BT6_Rel_Specified"
 */
int SBT_AH_attach_pdf_and_document_revs(EPM_action_message_t msg);

/**
 * @brief
 * Attach the Item(s) of the Revision(s) + Detach the Revision(s)
 */
int SBT_AH_attach_item_detach_revision(EPM_action_message_t msg);

/**
 * @brief
 * Cleanup specifies / concerns relations
 */
int SBT_AH_cleanup_relations(EPM_action_message_t msg);

/**
 * @brief
 * Attach the dataset from the targets to the Item Revision in the targets and set the appropriate status
 */
int SBT_AH_add_dataset_and_set_status(EPM_action_message_t msg);

/**
 * @brief
 * Assign the responsible party of the parent process to the current task
 */
int SBT_AH_assign_responsible_party_of_parent_workflow(EPM_action_message_t msg);

/**
 * @brief
 * Write UID of current Workflow to file and attach it as target (-> read by sub-process)
 */
int SBT_AH_write_uid_of_current_workflow_to_file_and_attach_it(EPM_action_message_t msg);

/**
 * @brief
 * Generate a cancel form and attach it to the workflow reference during cancelling process
 */
int SBT_AH_generate_cancel_form_and_attach_to_workflow(EPM_action_message_t msg);

/**
 * @brief
 * Transfer information and attached datasets from cancel info form to target objects
 */
int SBT_AH_transfer_cancel_form_info_to_targets(EPM_action_message_t msg);

/**
 * @brief
 * Remove the current revision from the targets and add all based-on revisions to the targets
 * Background: If a revision is set to status 10 or 60, all based-on revisions must be set to 90
 */
int SBT_AH_remove_current_revision_add_based_on_revisions(EPM_action_message_t msg);

/**
 * @brief
 * Sets the latest release Status to a specific property of a specific item type
 */
int SBT_AH_set_latest_release_status(EPM_action_message_t msg);

/**
 * @brief
 * Changes the Object Type of an Object in Teamcenter to another
 */
int SBT_AH_change_object_type(EPM_action_message_t msg);

/**
 * @brief
 * Checks if the part revision is a part of the ECO solution folder
 */
EPM_decision_t SBT_RH_part_rev_is_solution_item_of_ECO(EPM_action_message_t msg);

/**
 * @brief
 * Fill the EPMTask attribute object_desc with the correct message for the group
 * to be displayed in the Instruction filed of the task.
 * Will be cut by 240 chars! (size of the attribute).
 *
 * @param[in] msg The message Object of the EPM-task
 */
int SBT_WF_show_instruction_by_group_obj_desc_action_handler
  (
	EPM_action_message_t msg
  );

/**
* @brief
* Unlock the root task
*/
int SBT_AH_unlock_task(EPM_action_message_t msg);

/**
 * @brief
 * Processes the handlers given target objects [using the handlers given arguments].
 * The handler will send an email to all persons that were attached to any signoff 
 * task in the Workflow.
 *
 * @param[in] msg The message Object of the EPM-task
 */
int SBT_WF_sendmail_to_all_signoff_members_action_handler
  (
	EPM_action_message_t msg
  );

/**
 * @brief this function handles the registration of the runtime properties.
 * @param pDecision The decision taken.
 * @param arg the list of arguments.
 */
int SBT_Workflow_register_properties
  (
    int *pDecision , 
    va_list arg
  );


/**
 * @brief this function handles the registration of the workflow handler.
 * @param pDecision The decision taken.
 * @param arg the list of arguments.
 */
int SBT_Workflow_register_handler
  (
    int *piDecision,
    va_list args
  );


logical object_is_of_class_or_desc (
	const tag_t		objectTag ,				/* (I) */
	const char		* pcClassName ,			/* (I) */
	tag_t			* pObjectClassIdTag );


/**
* @brief
* Enable Attachment of a counter for Documents after the TNS-ID has been assigned
*/
// Issue 843. Numbering ITK modification. Code commented out.
//extern DLLAPI int SBT_Workflow_new_item_id(int *decision, va_list args);

/**
* @brief
* Override UserExit USER_validate_item_rev_id to normalization of SSNs
*/
extern int SBT_Workflow_validate_id( int *decision, va_list args );

/**
* @brief
* Override UserExit USER_invoke_pdm_server to deal with UGName and Ref Des sync
*/
extern int SBT_invoke_pdm_server( int *decision, va_list args );

extern int SBT_Workflow_define_bom_compare_mode(int* decision, va_list args);

//#1705
EPM_decision_t SBT_RH_Materials_for_release(EPM_rule_message_t msg);

/**
* @brief
* Verifies if the target revisions have same released status.
*/
EPM_decision_t  SBT_RH_check_targets_same(EPM_rule_message_t msg);

#endif

/* End file SBT_Workflow_grouphandler.h */
