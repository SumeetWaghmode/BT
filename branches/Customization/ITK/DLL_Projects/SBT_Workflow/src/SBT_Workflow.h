/**
 * This is the general header file for the library SBT_Workflow
 */
#ifndef _SBT_WORKFLOW_H
#define _SBT_WORKFLOW_H


#include <tc/iman.h>
#include <epm/epm.h>

/**
 * @brief Lists the signoff team users of an EPM signoff task.
 * @param signoff_task_tag  [in]  The tag of the signoff task.
 * @param soteam_user_count [out] The number of team members found.
 * @param soteam_user_tags  [out] (OF) The tags of the team members.
 */
int SBT_WF_signofftask_ask_singoff_team_users
  (
    tag_t signoff_task_tag,                             /* I */
    int *soteam_user_count,                             /* O */
    tag_t **soteam_user_tags                            /* OF */
  );

/**
 * @brief Lists the signoff team of an EPM signoff task.
 * @param signoff_task_tag   [in]  The tag of the signoff task.
 * @param soteam_group_count [out] The number of team members found.
 * @param soteam_group_tags  [out] (OF) The tags of the team members.
 */
int SBT_WF_signofftask_ask_singoff_team_groups
  (
    tag_t signoff_task_tag,                             /* I */
    int *soteam_group_count,                            /* O */
    tag_t **soteam_group_tags                           /* OF */
  );



#endif

/* End file SBT_Workflow.h */