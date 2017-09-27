/**
 * This is the general header file for the library SBT_Workflow.
 */
#ifndef _SBT_WORKFLOW_ATTRIBUTES_H
#define _SBT_WORKFLOW_ATTRIBUTES_H

#include <tc/iman.h>

/**
 * @brief
 * This method asks the group-dependent dataset task we attached below an EPMTask 
 * (EPMSignoffTask).
 * The dataset is named <RoutTaskName>_<GroupName>.
 * If no dataset with this name is found, a NULLTAG is returned.
 *
 * @param task_tag [in]  The task where the dataset is attached to.
 * @param ds_tag   [out] The dataset found (a NULLTAG if no dataset has been found).
 */
int SBT_WF_attr_task_ask_group_dataset
  (
    tag_t task_tag,
    tag_t *ds_tag
  );

#endif

/* End file SBT_Workflow_grouphandler.h */