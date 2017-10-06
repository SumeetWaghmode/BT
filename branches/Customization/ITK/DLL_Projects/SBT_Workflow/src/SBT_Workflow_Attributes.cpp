/**
 * This file holds the implementation of attributes used in the Workflows 
 * used in library SBT_Workflow.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <tccore/workspaceobject.h>
#include <epm/epm.h>
#include <tc/emh.h>
//#include <eds_itktool.h>

#include "sy_debug.h"

#include "SBT_Workflow_Attributes.h"
#include "SBT_Workflow_grouphandler.h"

#ifdef __cplusplus
}
#endif

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
int SBT_WF_attr_task_ask_group_dataset(
                                      tag_t     task_tag,   /* I */
                                      tag_t*    ds_tag      /* I */
                                      )
{
    int iRetcode = ITK_ok;

    int     attachment_count = 0;
    tag_t*  attachment_tags = NULL;

    char*   ds_name = NULL;
    char    attached_ds_name[WSO_name_size_c+1];

    char*   template_arg = NULL;
    logical group_arg;

    const char* theFunction = "SBT_Workflow_Attributes/SBT_WF_attr_task_ask_group_dataset";

    ENTER(theFunction);

    // preset return value
    *ds_tag = NULLTAG;

    if (iRetcode == ITK_ok && task_tag != NULLTAG)
    {


/*
{
char*pcOIS=NULL;
char cOT[WSO_name_size_c+1]="";
TRACE1("hfhp ask task_tag = %d\n",task_tag);
WSOM_ask_object_type(task_tag,cOT);
TRACE1("hfhp ask cOT = %s\n",cOT);
WSOM_ask_object_id_string(task_tag,&pcOIS);
TRACE1("hfhp ask pcOIS = %s\n",pcOIS);
MEM_free(pcOIS);
}
*/


        ITK(EPM_ask_attachments(task_tag,
                                               EPM_instruction_attachment,
                                               &attachment_count,
                                               &attachment_tags));
    }
    
    TRACE1("SBT: Number of attachments found: %d\n", attachment_count);

    // find the right attachment
    ITK(SBT_WF_instruction_by_group_get_epmtask_args(task_tag,
                                                                    &template_arg,
                                                                    &group_arg));
    if (group_arg == true)
    {
        char*   group_name = 0;
        tag_t   group_tag  = NULLTAG;

        if (iRetcode == ITK_ok)
        {
            ITK(POM_ask_group(&group_name, &group_tag));
        }
        if (iRetcode == ITK_ok)
        {
            ds_name = SBT_WF_instruction_by_group_get_item_id(template_arg, group_name);
        }
        MEM_free(group_name);
    }
    else //group_arg == false -> gruppenunabhängige Instruction
    {
        //do not consider group
        ds_name = SBT_WF_instruction_by_group_get_item_id(template_arg, NULL);
    }

    TRACE1("SBT: Looking for dataset named '%s'...\n", ds_name?ds_name:"NULL");

    for (int i = 0; i < attachment_count && iRetcode == ITK_ok && ds_name != NULL; ++i)
    {
        ITK(WSOM_ask_name(attachment_tags[i], attached_ds_name));
        TRACE2("SBT: Comparing '%s' with '%s'...\n", ds_name, attached_ds_name);

        if (strncmp(ds_name, attached_ds_name, strlen(ds_name)) == 0)
        {
            TRACE("SBT: Compare matched.\n");
            *ds_tag = attachment_tags[i];
        }
    }

    // clean up
    MEM_free(attachment_tags);
    MEM_free(template_arg);
    MEM_free(ds_name);

    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_WF_attr_task_ask_group_dataset */


/* End file SBT_Workflow_Attributes.c */
