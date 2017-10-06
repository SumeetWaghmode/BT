/**
 * This file registers the properties used in library SBT_Workflow.
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

#include "sy_debug.h"

#include "SBT_Workflow_grouphandler.h"
#include "SBT_01.h"
#include "SBT_ah_multiselect_effectivity.h"

#include "rghStrs.h"
#ifdef __cplusplus
}
#endif

/**
 * @brief
 * Register handlers and type methods (condition/pre/post)
 *
 * @param[out]  piDecision ...
 * @param[in/out] args   see below
 */
int SBT_Workflow_register_handler(
	int *   piDecision,
	va_list args)
{
	int retcode   = ITK_ok,
		iDecision = NO_CUSTOMIZATION;

	const char *theFunction = __FUNCTION__;

	TRACE_ON(0)
	ENTER(theFunction);

	/* Initialize output values */
	iDecision   = ALL_CUSTOMIZATIONS;
	*piDecision = iDecision;

	iDecision = ALL_CUSTOMIZATIONS;

	/* General Handlers */
	ITK(EPM_register_rule_handler("SBT-RH-add-drawing-pdf-required", "SBT-RH-add-drawing-pdf-required", (EPM_rule_handler_t) SBT_RH_add_drawing_pdf_required) );
	ITK(EPM_register_rule_handler("SBT-RH-check-target-released", "SBT-RH-check-target-released", (EPM_rule_handler_t) SBT_RH_check_target_released) );
	ITK(EPM_register_rule_handler("SBT-RH-check-item-annullable", "SBT-RH-check-item-annullable", (EPM_rule_handler_t) SBT_RH_check_item_annullable) );
	ITK(EPM_register_action_handler("SBT-AH-attach-bom-childs-for-anulling", "SBT-AH-attach-bom-childs-for-anulling", (EPM_action_handler_t) SBT_AH_attach_bom_childs_for_anulling) );
	ITK(EPM_register_action_handler("SBT-AH-attach-item-and-all-revs", "SBT-AH-attach-item-and-all-revs", (EPM_action_handler_t) SBT_AH_attach_item_and_all_revs) );
	ITK(EPM_register_action_handler("SBT-AH-assign-to-approver-of-same-group", "SBT-AH-assign-to-approver-of-same-group", (EPM_action_handler_t) SBT_AH_assign_to_approver_of_same_group) );
	ITK(EPM_register_action_handler("SBT-AH-set-status-for-object-type", "SBT-AH-set-status-for-object-type", (EPM_action_handler_t) SBT_AH_set_status_for_object_type) );
	ITK(EPM_register_action_handler("SBT-AH-remove-current-revision-add-based-on-revisions", "SBT-AH-remove-current-revision-add-based-on-revisions", (EPM_action_handler_t) SBT_AH_remove_current_revision_add_based_on_revisions) );
	ITK(EPM_register_action_handler("SBT-AH-assign-to-project", "SBT-AH-assign-to-project", (EPM_action_handler_t) SBT_AH_assign_to_project) );
	ITK(EPM_register_action_handler("SBT-AH-send-notification", "SBT-AH-send-notification", (EPM_action_handler_t) SBT_AH_send_notification) );
	ITK(EPM_register_action_handler("SBT-AH-attach-item-detach-revision", "SBT-AH-attach-item-detach-revision", (EPM_action_handler_t) SBT_AH_attach_item_detach_revision) );
	ITK(EPM_register_action_handler("SBT-AH-cleanup-relations", "SBT-AH-cleanup-relations", (EPM_action_handler_t) SBT_AH_cleanup_relations) );
	ITK(EPM_register_action_handler("SBT-AH-add-dataset-and-set-status", "SBT-AH-add-dataset-and-set-status", (EPM_action_handler_t) SBT_AH_add_dataset_and_set_status) );
	ITK(EPM_register_action_handler("SBT-AH-assign-responsible-party-of-parent-workflow", "SBT-AH-assign-responsible-party-of-parent-workflow", (EPM_action_handler_t) SBT_AH_assign_responsible_party_of_parent_workflow) );
	ITK(EPM_register_action_handler("SBT-AH-write-uid-of-current-workflow-to-file-and-attach-it", "SBT-AH-write-uid-of-current-workflow-to-file-and-attach-it", (EPM_action_handler_t) SBT_AH_write_uid_of_current_workflow_to_file_and_attach_it) );
	ITK(EPM_register_action_handler("SBT-AH-generate-cancel-form-and-attch-to-workflow", "SBT-AH-generate-cancel-form-and-attch-to-workflow", (EPM_action_handler_t) SBT_AH_generate_cancel_form_and_attach_to_workflow) );
	ITK(EPM_register_action_handler("SBT-AH-transfer-cancel-form-info-to-targets", "SBT-AH-transfer-cancel-form-info-to-targets", (EPM_action_handler_t) SBT_AH_transfer_cancel_form_info_to_targets) );
	ITK(EPM_register_action_handler("SBT-AH-unlock-task", "SBT-AH-unlock-task", (EPM_action_handler_t) SBT_AH_unlock_task) );
	ITK(EPM_register_action_handler("SBT-AH-set-latest-release-status", "SBT-AH-set-latest-release-status", (EPM_action_handler_t) SBT_AH_set_latest_release_status) );
	ITK(EPM_register_action_handler("SBT-AH-fill-In-Resp-Party-Name-In-Attr", "SBT-AH-fill-In-Resp-Party-Name-In-Attr", (EPM_action_handler_t) SBT_AH_fill_In_Resp_Party_Name_In_Attr) );
	ITK(EPM_register_action_handler("SBT-AH-change-product-type", "SBT-AH-change-product-type", (EPM_action_handler_t) SBT_AH_change_product_type) );
	ITK(EPM_register_action_handler("SBT-AH-remove-target-attachments", "SBT-AH-remove-target-attachments", (EPM_action_handler_t) SBT_AH_remove_target_attachments) );
	ITK(EPM_register_action_handler("SBT-AH-set-status-PR-PDF", "SBT-AH-set-status-PR-PDF", (EPM_action_handler_t) SBT_AH_set_status_PR_PDF) );
	ITK(EPM_register_action_handler("SBT-AH-change-object-type", "SBT-AH-change-object-type", (EPM_action_handler_t) SBT_AH_change_object_type) );

	/* Dispatcher check valid dataset under item revision */
	ITK(EPM_register_rule_handler("SBT-RH-check-target-attachments-dataset", "SBT-RH-check-target-attachments-dataset", (EPM_rule_handler_t) SBT_RH_check_target_attachments_dataset) );

	/* SAP-WF Handlers */
	ITK(EPM_register_rule_handler("SBT-RH-is-document", "SBT-RH-is-document", (EPM_rule_handler_t) SBT_RH_is_document) );
	ITK(EPM_register_action_handler("SBT-AH-attach-pdf-and-document-revs", "SBT-AH-attach-pdf-and-document-revs", (EPM_action_handler_t) SBT_AH_attach_pdf_and_document_revs) );

	/* Exit Handlers */
	ITK(EPM_register_rule_handler("SBT-RH-valid-for-release", "SBT-RH-valid-for-release", (EPM_rule_handler_t) SBT_RH_valid_for_release) );
	ITK(EPM_register_rule_handler("SBT-RH-part-has-released-bom-attached", "SBT-RH-part-has-released-bom-attached", (EPM_rule_handler_t) SBT_RH_part_has_released_bom_attached) );
	ITK(EPM_register_rule_handler("SBT-RH-check-target-type", "SBT-RH-check-target-type", (EPM_rule_handler_t) SBT_RH_check_target_type) );
	ITK(EPM_register_rule_handler("SBT-RH-prevent-minor-revision", "SBT-RH-prevent-minor-revision", (EPM_rule_handler_t) SBT_RH_prevent_minor_revision) );
	ITK(EPM_register_rule_handler("SBT-RH-current-user-is-common-comp-eng", "SBT-RH-current-user-is-common-comp-eng", (EPM_rule_handler_t) SBT_RH_current_user_is_common_comp_eng) );
	ITK(EPM_register_rule_handler("SBT-RH-List-Materials-for-release", "SBT-RH-List-Materials-for-release", (EPM_rule_handler_t) SBT_RH_Materials_for_release) );

	/* Electronic 3D Representation */
	ITK(EPM_register_rule_handler("SBT-RH-elec3d-bypass-check-for-pcbraw-possible", "SBT RH electronic 3d bypas check for PCB-raw possible", (EPM_rule_handler_t) SBT_RH_elec3d_bypass_check_for_pcbraw_possible) );
	ITK(EPM_register_rule_handler("SBT-RH-elec3d-compare-meta-data", "SBT RH electronic 3d compare meta data", (EPM_rule_handler_t) SBT_RH_elec3d_compare_meta_data) );
	ITK(EPM_register_action_handler("SBT-AH-elec3d-export-meta-data", "SBT AH electronic 3d export meta data", (EPM_action_handler_t) SBT_AH_elec3d_export_meta_data) );
	ITK(EPM_register_action_handler("SBT-AH-elec3d-new-minor-revision", "SBT AH electronic 3d new minor revision", (EPM_action_handler_t) SBT_AH_elec3d_newMinorRevisionIfRequired) );
	ITK(EPM_register_action_handler("SBT-AH-elec3d-assign-mcad-designer", "SBT AH electronic 3d assign mcad designer", (EPM_action_handler_t) SBT_AH_elec3d_assignMcadDesigner) );

	/* ECO Handler */
	ITK(EPM_register_rule_handler("SBT-RH-co-check-solution-status-before-eco-approval", "SBT RH checkSolutionStatusBeforeEcoApproval", (EPM_rule_handler_t) SBT_RH_co_checkSolutionStatusBeforeEcoApproval) );
	ITK(EPM_register_rule_handler("SBT-RH-co-check-solution-for-impacted-item-revs", "SBT RH checkSolutionForImpactedItemRev", (EPM_rule_handler_t) SBT_RH_co_checkSolutionForImpactedItemRev) );
	ITK(EPM_register_rule_handler("SBT-RH-co-check-work-packages", "SBT RH change order check work packages", (EPM_rule_handler_t) SBT_RH_co_check_wp) );
	ITK(EPM_register_rule_handler("SBT-RH-co-solution-items-exist", "SBT RH solution items exist", (EPM_rule_handler_t) SBT_RH_co_solution_items_exist) );
	ITK(EPM_register_rule_handler("SBT-RH-co-is-change-control-manager", "SBT RH is change control manager", (EPM_rule_handler_t) SBT_RH_co_is_change_control_manager) );
	ITK(EPM_register_rule_handler("SBT-RH-part-rev-is-solution-item-of-ECO","Checks if the part revision is in the solution item of ECO, before starting status 60 WF",(EPM_rule_handler_t) SBT_RH_part_rev_is_solution_item_of_ECO) );
	ITK(EPM_register_action_handler("SBT-AH-co-copy-eco-nr-to-solutions", "SBT AH copyEcoNrToSolutions", (EPM_action_handler_t) SBT_AH_co_copyEcoNrToSolutions) );
	ITK(EPM_register_action_handler("SBT-AH-co-start-work-packages", "SBT AH change order start work packages", (EPM_action_handler_t) SBT_AH_co_start_wp) );

	
	ITK(EPM_register_action_handler("SBT-Solution-item-eco-status", "SBT Solution item eco status", (EPM_action_handler_t) SBT_Solution_item_eco_status) );

	ITK(EPM_register_action_handler("SBT-AH-co-notify-change-specialist", "SBT AH notify change specialist", (EPM_action_handler_t) SBT_AH_notify_change_specialist) );
	ITK(EPM_register_action_handler("SBT-AH-co-notify-expert", "SBT AH notify expert", (EPM_action_handler_t) SBT_AH_notify_expert) );
	ITK(EPM_register_action_handler("SBT-AH-co-trigger-main-wf", "SBT AH change order trigger main workflow", (EPM_action_handler_t) SBT_AH_co_trigger_main_wf) );
	ITK(EPM_register_action_handler("SBT-AH-co-change-reference-txt-owner", "SBT AH change ownership of references text ", (EPM_action_handler_t) SBT_AH_co_change_reference_txt_owner) );

	/* CCR Handlers */
	ITK(EPM_register_rule_handler("SBT-RH-checkCCRConformity", "SBT RH checkCCRConformity", (EPM_rule_handler_t) SBT_RH_checkCCRConformity) );
	ITK(EPM_register_action_handler("SBT-AH-generateCCRObjects", "SBT AH generateCCRObjects", (EPM_action_handler_t) SBT_AH_generateCCRObjects) );
	ITK(EPM_register_action_handler("SBT-AH-generateTempCCRObjects", "SBT AH generateTempCCRObjects", (EPM_action_handler_t) SBT_AH_generateTempCCObjects) );
	ITK(EPM_register_action_handler("SBT-AH-replaceCcrClassification", "SBT AH replace CCR classification", (EPM_action_handler_t) SBT_AH_replaceCcrClassification) );
	ITK(EPM_register_action_handler("SBT-AH-generateCadimName", "SBT AH generate CADIM name", (EPM_action_handler_t) SBT_AH_generateCadimName) );
	ITK(EPM_register_rule_handler("SBT-RH-is-privileged-member-of-project", "SBT RH is privileged member of project", (EPM_rule_handler_t) SBT_RH_isPrivilegedMemberOfProject) );

	ITK(EPM_register_action_handler("SBT-AH-assign-TM-id", "SBT AH assign TM id", (EPM_action_handler_t) SBT_AH_assign_TM_id) );
	ITK(EPM_register_rule_handler("SBT-RH-checkRevsInProc", "SBT RH checkRevsInProc", (EPM_rule_handler_t) SBT_RH_checkRevsInProc) );
	ITK(EPM_register_rule_handler("SBT-RH-checkOwningGroup", "SBT RH checkOwningGroup", (EPM_rule_handler_t) SBT_RH_checkOwningGroup) );
	ITK(EPM_register_action_handler("SBT_Add_Instruction_by_group", "Adds the instruction dependent on group action handler", (EPM_action_handler_t) SBT_WF_instruction_by_group_action_handler) );
	ITK(EPM_register_action_handler("SBT_sendmail_to_all_signoff_members", "Sends an email to all Members listed in any signoff task.", (EPM_action_handler_t) SBT_WF_sendmail_to_all_signoff_members_action_handler) );
	ITK(EPM_register_action_handler("SBT-AH-multiselect-effectivity", "Set effectivity for all item revision in target", (EPM_action_handler_t) SBT_AH_multiselect_effectivity) );

//hfhp 13-May-2009 Is not used any more
// ITK ( EPM_register_action_handler ( "SBT_Show_Instruction_from_object_desc" , "Show the Instruction dependent on group action handler by object_desc." , (EPM_action_handler_t)SBT_WF_show_instruction_by_group_obj_desc_action_handler ) ) ;
	
	//Added Rule handler for Fix of Issue #SBTPRPDMBT-203:PR, PR+ and SR should check if all Targets have same status or no status
	ITK(EPM_register_rule_handler("SBT-RH-check-targets-same", "SBT RH check targets same", (EPM_rule_handler_t) SBT_RH_check_targets_same) );


	/* Set output values */
	*piDecision = iDecision;

	LEAVE(theFunction);
	return retcode;
}
