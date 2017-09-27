/**
 * This file holds the basic implementation of the Project SBT_Workflow
 */

/*
	10-Mar-2011		Matthias Schölch		Migriert auf 8.3
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <tc/emh.h>
#include <sa/sa.h>
#include <epm/signoff.h>
#include <ss/ss_errors.h>

#include "sy_debug.h"

#include "SBT_Workflow.h"

#ifdef __cplusplus
}
#endif

/**
 * @brief Lists the signoff team groups of an EPM signoff task.
 * @param signoff_task_tag   [in]  The tag of the signoff task.
 * @param soteam_group_count [out] The number of team members found.
 * @param soteam_group_tags  [out] (OF) The tags of the team members.
 */
int SBT_WF_signofftask_ask_singoff_team_groups
  (
    tag_t signoff_task_tag,                             /* I */
    int *soteam_group_count,                            /* O */
    tag_t **soteam_group_tags                           /* OF */
  )
{
    int ii = 0;

    int local_so_count = 0;
    tag_t * local_so_tags = NULL;

    int attachment_count = 0;
    tag_t *attachment_tags = NULLTAG;

    tag_t member_tag = NULLTAG;
    SIGNOFF_TYPE_t member_type;
    tag_t user_tag  = NULLTAG;
    tag_t group_tag = NULLTAG;

    const char* theFunction = "SBT_Workflow/SBT_WF_signofftask_ask_singoff_team_groups";
    //TRACE_ON(0);
    ENTER(theFunction);

    // preset return variables
    *soteam_group_count = 0;
    *soteam_group_tags  = NULL;


    if (retcode == ITK_ok && signoff_task_tag != NULLTAG)
    {
        ITK(EPM_ask_attachments(signoff_task_tag,
                                         EPM_signoff_attachment,
                                         &attachment_count,
                                         &attachment_tags));
    }

    if (retcode == ITK_ok && attachment_count > 0)
    {
        local_so_tags = (tag_t*) MEM_alloc(attachment_count * sizeof(tag_t));
    }
    if(local_so_tags == NULL) 
    {
        retcode = SS_NOMEM;
    }


    for (ii=0; ii<attachment_count && retcode == ITK_ok; ++ii)
    {
        ITK(EPM_ask_signoff_member(attachment_tags[ii], 
                                            &member_tag, 
                                            &member_type));

        if (member_type == SIGNOFF_GROUPMEMBER)
        {

            ITK(SA_ask_groupmember_group(member_tag, &group_tag));

            local_so_tags[local_so_count++] = group_tag;
        }
    }

    // clean up
    MEM_free(attachment_tags);


    // set return variables
    *soteam_group_count = local_so_count;
    *soteam_group_tags  = local_so_tags;


    LEAVE(theFunction);
    return retcode;

} /* End SBT_WF_signofftask_ask_singoff_team_groups */


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
  )
{
    int ii = 0;

    int local_so_count = 0;
    tag_t *local_so_tags = NULL;

    int attachment_count = 0;
    tag_t *attachment_tags = NULLTAG;

    tag_t member_tag = NULLTAG;
    SIGNOFF_TYPE_t member_type;
    tag_t user_tag  = NULLTAG;

    const char* theFunction = "SBT_Workflow/SBT_WF_signofftask_ask_singoff_team_users";
    //TRACE_ON(0);
    ENTER(theFunction);

    // preset return variables
    *soteam_user_count = 0;
    *soteam_user_tags  = NULL;

    if (retcode == ITK_ok && signoff_task_tag != NULLTAG)
    {
        ITK(EPM_ask_attachments(signoff_task_tag,
                                         EPM_signoff_attachment,
                                         &attachment_count,
                                         &attachment_tags));
    }

    if (retcode == ITK_ok && attachment_count > 0)
    {
        local_so_tags = (tag_t*) MEM_alloc(attachment_count * sizeof(tag_t));
        if(local_so_tags == NULL) 
        {
            retcode = SS_NOMEM;
        }
    }

    for (ii=0; ii<attachment_count && retcode == ITK_ok; ++ii)
    {
        ITK(EPM_ask_signoff_member(attachment_tags[ii], 
                                            &member_tag, 
                                            &member_type));

        if (member_type == SIGNOFF_GROUPMEMBER)
        {

            ITK(SA_ask_groupmember_user(member_tag, &user_tag));

            local_so_tags[local_so_count++] = user_tag;
        }
    }

    // clean up
    MEM_free(attachment_tags);

    // set return variables
    *soteam_user_count = local_so_count;
    *soteam_user_tags  = local_so_tags;

    LEAVE(theFunction);
    return retcode;

} /* End SBT_WF_signofftask_ask_singoff_team_users */


/* End file SBT_Workflow.cpp */
