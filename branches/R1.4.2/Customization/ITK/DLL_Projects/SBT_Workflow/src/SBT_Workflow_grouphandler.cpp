/**
 * This file is to hold the code needed to display the instruction of a 
 * Workflow dependent on the group the user is in.
 */

/*
	01-Apr-2009		Hartmuth Petzold		Replaced c++ constructs by C construct
											In SBT_WF_show_instruction_by_group_obj_desc_action_handler(...) 
											deactivated locking and unlocking of msg.task
											In sbt_wf_list_all_signoff_email_adresses(...) replaced obsolet function
											SA_ask_person_attribute(...) by SA_ask_person_attr(...)

	27-May-2009		Hartmuth Petzold		In SBT_WF_instruction_by_group_action_handler(...)
											added check for multiple occurrences of the same group

	10-Mar-2011		Matthias Schölch		Migriert auf 8.3

*/


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <tccore/method.h>
#include <itk/mem.h>
#include <property/prop.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <epm/epm_toolkit_tc_utils.h>

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


/**
 * @brief lists all email adresses attached to any signoff tasks attached to 
 *        the root taks given.
 * @param root_task_tag            [in]  The tag of the root task for which to list all 
 *                                       signoff email adresses.
 * @param task_type                [in]  The type of the task from which to read the emails.
 * @param signoff_email_string_set [out] The string set of the signoff emails found.
 */
static int sbt_wf_list_all_signoff_email_adresses
  (
    tag_t root_task_tag,                                /* I */
    EPM_task_type_t task_type,                          /* I */
//hfhp 02-Apr-2009 replaced by C construct
//  std::set<std::string> &signoff_email_string_set     /* O */
	int		* piStringCount ,							/* O */
	char	*** pppcSignoffEmailStringSet				/* OF */
  );

/**
 * @brief This function sends the email via the PLMeasy handler
 * VPDG-AH-sendMail.
 * @param signoff_email_string_set [in] The string set of the signoff 
 *        emails where to send the mail to.
 */
static int sbt_wf_send_plme_mail
  (
    EPM_action_message_t msg,
//hfhp 02-Apr-2009 replaced by C construct
//  const std::set<std::string> &signoff_email_string_set   /* I */
	const int	iStringCount ,								/* I */
	const char	** ppcSignOffEmailStringSet				    /* I */
  );

/**
 * @brief Evaluates arguments for SBT_WF_instruction_by_group_action_handler.
 * @param msg [in] EPMTask for which arguments should be evaluated
 * @param arg_template [out] value of -template argument
 * @param arg_group [out] value of -group argument
 */
static int sbt_wf_instruction_by_group_evaluate_args(
                                                    EPM_action_message_t  msg,            /* I */
                                                    char**                arg_template,   /* OF */
                                                    logical*              arg_group       /* O */
                                                    );

/**
 * @brief Looks for a dataset which must be attached to an item_rev of an item, which
 *        has to be located in a folder defined in .iman_env preference.
 * @param item_id [in] item_id of item which is to be found
 * @param dataset_tag [out] tag of a dataset if found or NULLTAG if not found
 */
static int sbt_wf_find_instruction_dataset(
                                          const char*    item_id,       /* I */
                                          tag_t*         dataset_tag    /* O */    
                                          );

/**
 * @brief Attaches a dataset to EPMTask
 * @param msg [in] EPMTask
 * @param dataset_tag [in] tag of a dataset to be attached
 */

static int sbt_wf_epmtask_attach_dataset(
                                        EPM_action_message_t    msg,        /* I */
                                        tag_t                   dataset_tag /* I */
                                        );

/**
 * @brief Gets name of a folder for instruction templates,
 *        defined in preference VPDG_WF_INSTRUCTION_FOLDER
 * @param folder_name [out] name of folder from preference or NULL if no preference found
 */
static int sbt_wf_get_instruction_folder_name(
                                             char** folder_name /* OF */
                                             );

/**
 * @brief Checks if an item is referenced in specified folder
 * @param item_tag [in] tag of an item to be checked
 * @param folder_name [in] name of the folder
 * @param reference_ok [out] true if item is referenced in folder, false if not
 */

static int sbt_wf_check_item_folder_reference(
                                             tag_t          item_tag,       /* I */
                                             const char*    folder_name,    /* I */
                                             logical*       reference_ok    /* O */
                                             );



/*
 * @brief Tests the Runtime Property (rudimentarily)
 */
void sbt_test_rtp(tag_t epm_task)
{
    int iRetcode = ITK_ok;

    char *contense = NULL;

    tag_t rtp_prop_tag = NULLTAG;

    if (iRetcode == ITK_ok)
    {
        ITK(PROP_ask_property_by_name(epm_task, 
                                                     SBT_WF_groupdependent_rtp_name,
                                                     &rtp_prop_tag));
    }

    if (iRetcode == ITK_ok)
    {
        ITK(PROP_ask_value_string(rtp_prop_tag, &contense));
    }

    if (iRetcode == ITK_ok)
    {
        fprintf(stderr, "TEST: rtp contense: '%s'\n", (contense?contense:"null"));
    }

} /* End sbt_test_rtp */



/**
 * @brief
 * Processes the handlers given target objects [using the handlers given arguments].
 * The handler should give an instruction to the workflow target that is
 * dependent on the group with the user is logged in.
 * This task will attach the dataset to the EPMTask so that the instructions
 * are available later on.
 *
 * @param[in] msg The message Object of the EPM-task
 */
int SBT_WF_instruction_by_group_action_handler(EPM_action_message_t msg)
{
    const char* theFunction = "SBT_WF_instruction_by_group_action_handler";
    int	iRetcode = ITK_ok;

    char*   arg_template = NULL;
    logical arg_group = NULL;

    // avoid infinite loops
    if (msg.action > EPM_perform_action)
    {
        return ITK_ok;
    }

    ENTER(theFunction);

    ITK(sbt_wf_instruction_by_group_evaluate_args(msg,
                                                                 &arg_template,
                                                                 &arg_group));
    if (iRetcode == ITK_ok)
    {
        if (arg_group == true)
        {
            int     group_count = 0;
            tag_t*  group_tags = NULL;

            logical		vIgnoreMultipleOccurrencesOfGroup = false	;


            ITK(SBT_WF_signofftask_ask_singoff_team_groups(msg.task, 
                                                                          &group_count,
                                                                          &group_tags));
            TRACE1("SBT: found %d team groups.\n", group_count);

            for (int i = 0; i < group_count && iRetcode == ITK_ok; i++)
            {
                char    group_name[SA_name_size_c+1];
                tag_t   dataset_tag = NULLTAG;

                ITK(SA_ask_group_name(group_tags[i], group_name));
                TRACE1("SBT: group name '%s'.\n", group_name);



//hfhp 27-May-2009 added check for multiple occurrences of the same group
				vIgnoreMultipleOccurrencesOfGroup = false ;
				for ( int iIdx = 0 ; iIdx < i ; iIdx ++ ) 
				{
					if ( group_tags[iIdx] == group_tags[i] ) 
					{
						TRACE1("SBT: ignore multiple occurrences of group '%s'.\n", group_name);
						vIgnoreMultipleOccurrencesOfGroup = true ;
						break ;
					}
				}
				if ( vIgnoreMultipleOccurrencesOfGroup ) continue ;




                char*   item_id = SBT_WF_instruction_by_group_get_item_id(arg_template, group_name);

                TRACE1("SBT: looking for dataset for item_id '%s' ...\n", item_id);
                ITK(sbt_wf_find_instruction_dataset(item_id, &dataset_tag));
                
                if (iRetcode == ITK_ok && dataset_tag != NULLTAG)
                {
                    ITK(sbt_wf_epmtask_attach_dataset(msg, dataset_tag));
                    ITK(SBT_WF_instruction_by_group_set_epmtask_args(msg.task,
                                                                                    arg_template,
                                                                                    arg_group));
	            }
                MEM_free(item_id);

            }
            MEM_free(group_tags);

        }
        else // arg_group == 'false' (gruppenunabhängiges Template)
        {
            tag_t   dataset_tag = NULLTAG;
            char*   item_id = SBT_WF_instruction_by_group_get_item_id(arg_template, NULL);

            TRACE1("SBT: looking for dataset for item_id '%s' ...\n", item_id);
            ITK(sbt_wf_find_instruction_dataset(item_id, &dataset_tag));

            if (iRetcode == ITK_ok && dataset_tag != NULLTAG)
            {
                ITK(sbt_wf_epmtask_attach_dataset(msg, dataset_tag));
                ITK(SBT_WF_instruction_by_group_set_epmtask_args(msg.task,
                                                                                arg_template,
                                                                                arg_group));
            }
            MEM_free(item_id);
        }
    }

    MEM_free(arg_template);

    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_WF_instruction_by_group_action_handler */


/**
 * @brief
 * Composes an item_id which will be used to search for dataset with instruction
 * @param[in] template_name Name of the template as defined for argument -template
 * in the handler
 * @param[in] group_name Name of the group for which an instruction should be searched
 * for. NULL for group independent instructions (if handler argument -group=false).
 * @param[out] item_id which contains a desired instruction (actually a dataset 
 * which is attached which is attached to the latest released revision of an item).
 */
char* SBT_WF_instruction_by_group_get_item_id(
                                             const char*    template_name,    /* I */
                                             const char*    group_name        /* I, null allowed */
                                             )
{
    //int iRetcode = ITK_ok;
    //const char* theFunction = "SBT_WF_instruction_by_group_get_dataset_item_id";

    //ENTER(theFunction);
    char*   item_id = NULL;

    item_id = SBT_string_copy(template_name);

    if (group_name != NULL)
    {
        item_id = SBT_string_append(item_id, "_");
        item_id = SBT_string_append(item_id, group_name);
    }

    return item_id;

    //LEAVE(theFunction);
    //return iRetcode;
}



/**
 * @brief
 * Evalueates handler arguments -template and -group
 * @param[in] msg
 * @param[out] arg_template value of argument -template for the handler
 * @param[out] arg_group value of argument -group for the handler
 */
static int sbt_wf_instruction_by_group_evaluate_args(
                                                    EPM_action_message_t  msg,            /* I */
                                                    char**                arg_template,   /* OF */
                                                    logical*              arg_group       /* O */
                                                    )
{
    int iRetcode = ITK_ok;
    const char* theFunction = "sbt_wf_instruction_by_group_evaluate_args";

    ENTER(theFunction);

    const int   arg_count = TC_number_of_arguments(msg.arguments);

    TRACE1("SBT: Found %d arguments\n", arg_count);

    if (arg_count <= 0)
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

        if (strcmp(flag, "template") == 0)
        {
            *arg_template = SBT_string_copy(value);
        }
        else if (strcmp(flag, "group") == 0)
        {
            if (stricmp(value, "true") == 0)
            {
                *arg_group = true;
            }
            else
            {
                *arg_group = false;
            }
        }
        else
        {
            iRetcode = EPM_invalid_argument;
        }

        MEM_free(flag);
      	MEM_free(value);
    }

    TRACE2("SBT: returning template='%s', group='%s'\n", *arg_template?*arg_template:"NULL", *arg_group?"true":"false");

    LEAVE(theFunction);
    return iRetcode;
}


/**
 * @brief
 * Sets values of handler arguments -template and -group in AOM argument "object_desc"
 * of EPM_Task.
 * Values will be later read out in runtime property sbt_wf_get_epm_group_task_string.
 * @param[in] EPM_Task for which "object_desc" is to be set
 * @param[in] template_value value of -template argument
 * @param[in] group_value value of -group argument
 */
int SBT_WF_instruction_by_group_set_epmtask_args(
                                                tag_t        epmtask,           /* I */
                                                const char*  template_value,    /* I */
                                                logical      group_value        /* I */
                                                )
{
    int iRetcode = ITK_ok;
    const char* theFunction = "SBT_WF_instruction_by_group_set_epmtask_args";

    ENTER(theFunction);

    char    object_desc[240] = {'\0'};
    sprintf(object_desc, "-t=%s;-g=%s", template_value, group_value?"true":"false");

    TRACE1("SBT: object_desc to set='%s'\n", object_desc);
    ITK(AOM_set_value_string(epmtask,
                                            "object_desc",
                                            object_desc));

    ITK(AOM_save(epmtask));
    LEAVE(theFunction);

    return iRetcode;
}



/**
 * @brief
 * Reads values of handler arguments -template and -group from AOM argument "object_desc"
 * of EPM_Task.
 * Values have to be set before using function SBT_WF_instruction_by_group_set_epmtask_args.
 * @param[in] EPM_Task for which "object_desc" is to be read
 * @param[out] template_value value of -template argument
 * @param[out] group_value value of -group argument
 */
int SBT_WF_instruction_by_group_get_epmtask_args(
                                                tag_t       epmtask,        /* I */
                                                char**      template_value, /* OF */
                                                logical*    group_value     /* O */
                                                )
{
    int iRetcode = ITK_ok;
    const char* theFunction = "SBT_WF_instruction_by_group_get_epmtask_args";

    ENTER(theFunction);

    char*   object_desc = NULL;

    ITK(AOM_get_value_string(epmtask,
                                            "object_desc",
                                            &object_desc));

    TRACE1("SBT: value of object_desc = '%s'\n", object_desc?object_desc:"NULL");
    TRACE("SBT: parsing args...\n");

    char*       ptr = strtok(object_desc, ";");
    const char* template_token = "-t=";
    const char* group_token = "-g=";

    if (ptr != NULL && strncmp(ptr, template_token, strlen(template_token)) == 0)
    {
        //template arg found
        ptr += strlen(template_token);
        *template_value = SBT_string_copy(ptr);
    }

    ptr = strtok(NULL, ";");
    if (ptr != NULL && strncmp(ptr, group_token, strlen(group_token)) == 0)
    {
        //group found
        ptr += strlen(group_token);
        if (stricmp(ptr, "true") == 0)
        {
            *group_value = true;
        }
        else
        {
            *group_value = false;
        }
    }

    MEM_free(object_desc);

    TRACE2("SBT: returning template='%s', group='%s'\n", *template_value, *group_value?"true":"false");
    LEAVE(theFunction);

    return iRetcode;
}



static int sbt_wf_get_instruction_folder_name(
                                             char** folder_name /* OF */
                                             )
{
    int iRetcode = ITK_ok;
    const char* theFunction = "sbt_wf_get_instruction_folder_preference";

    *folder_name = NULL;

    ENTER(theFunction);

    char    preference[] = "VPDG_WF_INSTRUCTION_FOLDER";
    char*   preference_value = NULL;
    int     preference_count = 0;

    ITK(PREF_ask_value_count(preference, &preference_count));
    
    if (preference_count > 0)
    {
        TRACE2("SBT: preference '%s' found, count=%d\n", preference, preference_count);
    }
    else
    {
        TRACE1("SBT: preference '%s' cound not be found!!!\n", preference);
    }
	if (iRetcode == ITK_ok && preference_count > 0)
    {
        ITK(PREF_ask_char_value(preference, 0, &preference_value));
        TRACE2("SBT: value of preference '%s' == '%s'\n", preference, preference_value);
	}
    if (iRetcode == ITK_ok && preference_value != NULL)
    {
        *folder_name = SBT_string_copy(preference_value);
    }

    MEM_free(preference_value);
    LEAVE(theFunction);

    return iRetcode;
}



static int sbt_wf_check_item_folder_reference(
                                             tag_t          item_tag,       /* I */
                                             const char*    folder_name,    /* I */
                                             logical*       reference_ok    /* O */
                                             )
{
    int iRetcode = ITK_ok;
    const char* theFunction = "sbt_wf_check_item_folder_reference";

    *reference_ok = false;

    int     n_referencers = 0;
    int*    levels = NULL;
    tag_t*  referencers = NULL;
    char**  relations = NULL;

    ITK(WSOM_where_referenced(item_tag, 1, &n_referencers, &levels, &referencers, &relations));
        
    for (int i = 0; i < n_referencers; i++)
    {
        char    object_type[WSO_name_size_c + 1];

        ITK(WSOM_ask_object_type(referencers[i], object_type));

        if (iRetcode == ITK_ok && strcmp(object_type, "Folder") == 0)
        {
            char*   object_name = NULL;

            ITK(WSOM_ask_object_id_string(referencers[i], &object_name));
                
            if (iRetcode == ITK_ok && strcmp(object_name, folder_name) == 0)
            {
                TRACE1("SBT: item correctly referenced in folder '%s'\n", folder_name);
                *reference_ok = true;
            }
            MEM_free(object_name);
        }
    }

    MEM_free(levels);
    MEM_free(referencers);
    MEM_free(relations);

    LEAVE(theFunction);
    return iRetcode;

}


static int sbt_wf_find_instruction_dataset(
                                          const char*    item_id,       /* I */
                                          tag_t*         dataset_tag    /* O */    
                                          )
{
    const char* theFunction = "sbt_wf_find_instruction_dataset";
    int         iRetcode = ITK_ok;

    tag_t   item_tag = NULLTAG;
    tag_t   item_rev_tag = NULLTAG;

    logical reference_ok = false; //is item correctly in preference folder referenced?

    ENTER(theFunction);

    *dataset_tag = NULLTAG;

    if (iRetcode == ITK_ok && item_id != NULL)
    {
        TRACE1("SBT: looking for item_id '%s'...\n", item_id);
        ITK(ITEM_find_item(item_id, &item_tag));
    }

    if (item_tag == NULLTAG)
    {
        TRACE1("SBT: item with id not found: '%s'!!!\n", item_id);
    }
    else
    {
        TRACE1("SBT: item with id '%s' found.\n", item_id);
    }
    
    if (iRetcode == ITK_ok && item_tag != NULL)
    {
        TRACE("SBT: checking item reference (must be content of preference folder)...\n");

        char*   folder_name = NULL;
        ITK(sbt_wf_get_instruction_folder_name(&folder_name));

        if (iRetcode == ITK_ok && folder_name != NULL)
        {
            ITK(sbt_wf_check_item_folder_reference(item_tag, folder_name, &reference_ok));
        }

        MEM_free(folder_name);
	}

    if (iRetcode == ITK_ok && reference_ok == true)
    {
        TRACE("SBT: looking for latest released item_rev...\n");
        ITK(SBT_ITEMREV_ask_latest_rev_with_status(item_tag, 
                                                   STATUS_TYPE_60,
                                                   &item_rev_tag));
        if (item_rev_tag == NULLTAG)
        {
            TRACE("SBT: item rev could not be found!!!\n");
        }
        else
        {
            char* id = NULL;
            WSOM_ask_object_id_string(item_tag, &id);
            TRACE1("SBT: latest released item rev found: %s.\n", id);
            MEM_free(id);
        }
    }

    if (iRetcode == ITK_ok && item_rev_tag != NULLTAG)
    {
        TRACE("SBT: looking for dataset attached to item rev...\n");
        
        ITK(SBT_Workspaceobject_ask_assigned_wso(item_rev_tag,
                                                 SBT_WF_groupdependent_rev_dataset_assignment,
                                                 dataset_tag));

        if (*dataset_tag == NULLTAG)
        {
            TRACE("SBT: attached dataset not found!!!\n");
        }
        else
        {
            TRACE("SBT: attached dataset successfully found.\n");
        }
    }

    LEAVE(theFunction);
    return iRetcode;
}


static int sbt_wf_epmtask_attach_dataset(
                                        EPM_action_message_t    msg,
                                        tag_t                   dataset_tag
                                        )
{
    int     iRetcode = ITK_ok;
    char*   theFunction = "sbt_wf_epmtask_attach_dataset";

    ENTER(theFunction);

    int attachment_type = EPM_instruction_attachment;


/*
{
char*pcOIS=NULL;
char cOT[WSO_name_size_c+1]="";
TRACE1("hfhp add msg.task = %d\n",msg.task);
WSOM_ask_object_type(msg.task,cOT);
TRACE1("hfhp add cOT = %s\n",cOT);
WSOM_ask_object_id_string(msg.task,&pcOIS);
TRACE1("hfhp add pcOIS = %s\n",pcOIS);
MEM_free(pcOIS);
}
*/



    TRACE("SBT: attaching dataset to EPM_task...\n");
    ITK(EPM_add_attachments(msg.task,
                                           1,
                                           &dataset_tag,
                                           &attachment_type));
    if (iRetcode == ITK_ok)
    {
        TRACE("SBT: dataset successfully attached.\n");
    }

    LEAVE(theFunction);
    return iRetcode;
}


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
  )
{
    // TCH: nicht verwendet

    int iRetcode = ITK_ok;

    int ii=0;

    tag_t grp_dataset_tag = NULLTAG,
          grp_file_tag    = NULLTAG;

    int str_array_count = 0;
    char **str_array = 0;

    char *value = 0;

    tag_t obj_desc_attr_id = NULLTAG;

    const char* theFunction = "SBT_WF_show_instruction_by_group_obj_desc_action_handler";
    ENTER(theFunction);

    if (iRetcode == ITK_ok)
    {
        ITK(SBT_WF_attr_task_ask_group_dataset(msg.task,
                                                        &grp_dataset_tag));
    }

    // use the attachment found
    if (iRetcode == ITK_ok && grp_dataset_tag != NULLTAG)
    {
        ITK(SBT_Dataset_ask_assigned_file_tag(grp_dataset_tag,
                                                       SBT_WF_groupdependent_info_attachment,
                                                       0,   /* index */
                                                       &grp_file_tag));
    }

    if (iRetcode == ITK_ok && grp_file_tag != NULLTAG)
    {
        ITK(SBT_File_tag_to_string_array(grp_file_tag,
                                                  &str_array_count,
                                                  &str_array));
    }

    if (str_array_count > 0)
    {
        value = SBT_string_copy(str_array[0]);
    }

    for (ii=1; ii<str_array_count; ++ii)
    {
        if ((int)strlen(value) > SBT_WF_groupdependent_prop_len)
            break;

        value = SBT_string_append(value, "\n");
        value = SBT_string_append(value, str_array[ii]);
    }

    if (iRetcode == ITK_ok && value == 0)
    {
        value = SBT_string_copy(SBT_WF_not_groupdependent_std_msg);
    }

    //cope with texts that are too long
    if (iRetcode == ITK_ok && value &&
        ((int)strlen(value) > SBT_WF_object_desc_len))
    {
        value[SBT_WF_object_desc_len] = '\0';
    }

    // set the attribute
    if (iRetcode == ITK_ok)
    {
//hfhp 01-Apr-2009 deactivated
//        ITK(AOM_refresh(msg.task, POM_modify_lock));
    }

    if (iRetcode == ITK_ok)
    {
        ITK(POM_attr_id_of_attr("object_desc",
                                         "EPMTask",
                                         &obj_desc_attr_id));
    }

    if (iRetcode == ITK_ok && obj_desc_attr_id != NULLTAG && value != NULL)
    {
        ITK(POM_set_attr_string(1,
                                         &msg.task,
                                         obj_desc_attr_id,
                                         value));
    }

    if (iRetcode == ITK_ok)
    {
        ITK(AOM_save(msg.task));

        if (iRetcode == ITK_ok)
        {
//hfhp 01-Apr-2009 deactivated
//           ITK(AOM_refresh(msg.task, POM_no_lock));
        }
    }

    // clean up
    for (ii=0; ii < str_array_count; ++ii)
        MEM_free(str_array[ii]);
    MEM_free(str_array);

    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_WF_show_instruction_by_group_obj_desc_action_handler */




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
  )
{
    int iRetcode = ITK_ok;

	int		iStringCount = 0 ;						
	char	** ppcSignOffEmailStringSet	= NULL ;
			
	int		iAuxStringCount = 0 ;						
	char	** ppcAuxSignOffEmailStringSet	= NULL ;			

    tag_t root_task_tag = NULLTAG;
    char task_name[WSO_name_size_c+1];
	
    // avoid infinite loops
    if (msg.action > EPM_perform_action)
        return ITK_ok;
    
    const char* theFunction = "SBT_WF_sendmail_to_all_signoff_members_action_handler";
    ENTER(theFunction);

    if (iRetcode == ITK_ok && msg.task != NULLTAG) 
    {
        ITK(EPM_ask_root_task(msg.task, &root_task_tag));
    }

    if (iRetcode == ITK_ok && root_task_tag != NULLTAG)
    {
        task_name[0] = '\0';
        ITK(EPM_ask_name(root_task_tag, task_name));
    }

    // get the mail adresses
//hfhp 02-Apr-2009 replaced by C construct
//  std::set<std::string> signoff_email_string_set;
    if (iRetcode == ITK_ok)
    {
        ITK(sbt_wf_list_all_signoff_email_adresses(root_task_tag,
                                                                  eEPMPerformSignoffTask,
//hfhp 02-Apr-2009 replaced by C construct
//                                       						  signoff_email_string_set));
																  &iStringCount , &ppcSignOffEmailStringSet ) ) ;                                                               
    }

    // Ask the SBT-route-task, too.
    if (iRetcode == ITK_ok)
    {
        ITK(sbt_wf_list_all_signoff_email_adresses(root_task_tag,
                                                                  eEPMNotifyTask,
//hfhp 02-Apr-2009 replaced by C construct
//                                                                signoff_email_string_set));
																  &iAuxStringCount , &ppcAuxSignOffEmailStringSet ) ) ;
    }
    
	// Append mail addresses 
	for ( int i=0 ; i<iAuxStringCount ; i++ ) 
	{
		iStringCount = iStringCount + 1 ;	
		ppcSignOffEmailStringSet = (char**)MEM_realloc ( ppcSignOffEmailStringSet , iStringCount * sizeof(char*) ) ;	
		ppcSignOffEmailStringSet[iStringCount-1] = (char*)MEM_alloc ( (strlen(ppcAuxSignOffEmailStringSet[i])+1) * sizeof(char) ) ;
		ppcSignOffEmailStringSet[iStringCount-1][0] = '\0' ;
		strcpy ( ppcSignOffEmailStringSet[iStringCount-1] , ppcAuxSignOffEmailStringSet[i] ) ;
	}
	
    if (iRetcode == ITK_ok)
    {

		ITK(sbt_wf_send_plme_mail(msg, 
//hfhp 02-Apr-2009 replaced by C construct
//									   signoff_email_string_set));
									   iStringCount ,  (const char**)ppcSignOffEmailStringSet ) ) ;

    }

	for(int i=0;i<iAuxStringCount;i++)MEM_free(ppcAuxSignOffEmailStringSet[i]);
	MEM_free(ppcAuxSignOffEmailStringSet);ppcAuxSignOffEmailStringSet=NULL;iAuxStringCount=0;

	for(int i=0;i<iStringCount;i++)MEM_free(ppcSignOffEmailStringSet[i]);
	MEM_free(ppcSignOffEmailStringSet);ppcSignOffEmailStringSet=NULL;iStringCount=0;

    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_WF_sendmail_to_all_signoff_members_action_handler */


/**
 * @brief This function sends the email via the PLMeasy handler
 * VPDG-AH-sendMail.
 * @param signoff_email_string_set [in] The string set of the signoff 
 *        emails where to send the mail to.
 */
static int sbt_wf_send_plme_mail
  (
    EPM_action_message_t msg,
//hfhp 02-Apr-2009 replaced by C construct
//  const std::set<std::string> &signoff_email_string_set   /* I */
	const int	iStringCount ,								/* I */
	const char	** ppcSignOffEmailStringSet				    /* I */
  )
{
    int iRetcode = ITK_ok;

    const char* theFunction = "sbt_wf_send_plme_mail";

    ENTER(theFunction);

    // set up the message type
    EPM_action_message_t mail_msg;
    mail_msg.task           = msg.task;                 // tag_t, O.K.
    mail_msg.action         = msg.action;               // int, O.K.
    mail_msg.proposed_state = msg.proposed_state;       // enum, O.K.
    mail_msg.data           = msg.data;                 // tag_t, O.K.
    int arg_count = 0;

    logical subject_passed  = false,
            report_passed   = false;

    TC_argument_list_t mail_arguments;
    int num_of_arg =   1                                        // subject
                     + 1                                        // report
//hfhp 02-Apr-2009 replaced by C construct
//                   + (int) signoff_email_string_set.size();   // recipient.
                     + iStringCount;                            // recipient.

    int potential_num_of_arg = num_of_arg +
                               msg.arguments->number_of_arguments;

    mail_arguments.i = 0;   // an internal counter, needs to be 0 (in the beginning).
    mail_arguments.arguments = (TC_argument_t *)MEM_alloc(potential_num_of_arg * sizeof(TC_argument_t));
    if (iRetcode == ITK_ok && mail_arguments.arguments == 0) 
    {
        iRetcode = SS_NOMEM;
    }

    if (iRetcode == ITK_ok)
    {
//hfhp 02-Apr-2009 replaced by C construct
//      TRACE1("SBT: signoff_email_string_set.size (send): %d\n", (int) signoff_email_string_set.size());
        TRACE1("SBT: iStringCount (send): %d\n", iStringCount);
        // recipients
//hfhp 02-Apr-2009 replaced by C construct
//      std::set<std::string>::const_iterator soSI;
//      for (soSI  = signoff_email_string_set.begin();
//           soSI != signoff_email_string_set.end();
//           ++soSI)
		for ( int i = 0 ; i < iStringCount ; i++ )
        {
            TC_argument_t recipient_arg;
            recipient_arg.array_size = 1;
            recipient_arg.type = 2008;
            recipient_arg.val_union.int_value = 1;
            recipient_arg.val_union.str_value = SBT_string_copy("-recipient=OS:");
//hfhp 02-Apr-2009 replaced by C construct
//          recipient_arg.val_union.str_value = SBT_string_append(recipient_arg.val_union.str_value, soSI->c_str());
            recipient_arg.val_union.str_value = SBT_string_append(recipient_arg.val_union.str_value, ppcSignOffEmailStringSet[i]);

//hfhp 02-Apr-2009 replaced by C construct
//          TRACE1("SBT: Sending OS-mail to recipient %s\n", soSI->c_str());
            TRACE1("SBT: Sending OS-mail to recipient %s\n", ppcSignOffEmailStringSet[i]);

            mail_arguments.arguments[arg_count++ /* 2 ... */] = recipient_arg;
        }

        TRACE1("SBT: msg.arguments->number_of_arguments: %d\n", msg.arguments->number_of_arguments);

        // now handle the arguments passed from the task.
        // PLMeasy arguments shall be passed to the handler
        for (int ii=0; ii<msg.arguments->number_of_arguments; ++ii)
        {
            TRACE2("SBT: evaluate msg.arguments->arguments[%d].val_union.str_value: %s\n", ii, msg.arguments->arguments[ii].val_union.str_value);
            
            if (strncmp(msg.arguments->arguments[ii].val_union.str_value, "-subject", 8) == 0    ||
                strncmp(msg.arguments->arguments[ii].val_union.str_value, "-report", 7) == 0     ||
                strncmp(msg.arguments->arguments[ii].val_union.str_value, "-recipient", 10) == 0 ||
                strncmp(msg.arguments->arguments[ii].val_union.str_value, "-comments", 9) == 0   ||
                strncmp(msg.arguments->arguments[ii].val_union.str_value, "-text", 5) == 0          )
            {
                TC_argument_t passed_arg;
                passed_arg.array_size = msg.arguments->arguments[ii].array_size;
                passed_arg.type = msg.arguments->arguments[ii].type;
                passed_arg.val_union.int_value = msg.arguments->arguments[ii].val_union.int_value;
                passed_arg.val_union.str_value = SBT_string_copy(msg.arguments->arguments[ii].val_union.str_value);

                TRACE2("SBT: argument msg.arguments->arguments[%d].val_union.str_value: '%s'\n", ii, msg.arguments->arguments[ii].val_union.str_value);

                mail_arguments.arguments[arg_count++ /* n ... */] = passed_arg;
            }

            if (strncmp(msg.arguments->arguments[ii].val_union.str_value, "-subject", 8) == 0)
            {
                TRACE("SBT: subject passed\n");
                subject_passed  = true;
            }
            if (strncmp(msg.arguments->arguments[ii].val_union.str_value, "-report", 7) == 0)
            {
                TRACE("SBT: report passed\n");
                report_passed   = true;
            }
        }

        // subject arg (if not already handled)
        if (subject_passed == false)
        {
            TC_argument_t subject_arg;
            subject_arg.array_size = 1;
            subject_arg.type = 2008;
            subject_arg.val_union.int_value = 1;
            subject_arg.val_union.str_value = SBT_string_copy("-subject=New Status given for Object.");

            TRACE("SBT: set subject\n");
            mail_arguments.arguments[arg_count++] = subject_arg;
        }

        // report (if not already handled)
        if (report_passed == false)
        {
            TC_argument_t report_arg;
            report_arg.array_size = 1;
            report_arg.type = 2008;
            report_arg.val_union.int_value = 1;
            report_arg.val_union.str_value = SBT_string_copy("-report=progress");

            TRACE("SBT: set report\n");
            mail_arguments.arguments[arg_count++] = report_arg;
        }
    }

    // set the correct number of arguments.
    mail_arguments.number_of_arguments = arg_count;

    mail_msg.arguments = &mail_arguments;

    // trace the arguments
    for (int ii = 0; ii < mail_msg.arguments->number_of_arguments; ++ii)
    {
        TRACE2("SBT: mail_msg.arguments->arguments[%d].array_size: %d\n", ii, mail_msg.arguments->arguments[ii].array_size);
        TRACE2("SBT: mail_msg.arguments->arguments[%d].type: %d\n", ii, mail_msg.arguments->arguments[ii].type);
        TRACE2("SBT: mail_msg.arguments->arguments[%d].val_union.int_value: %d\n", ii, mail_msg.arguments->arguments[ii].val_union.int_value);
        TRACE2("SBT: mail_msg.arguments->arguments[%d].val_union.str_value: %s\n", ii, mail_msg.arguments->arguments[ii].val_union.str_value);
    }

    // and now call the action-handler
    if (iRetcode == ITK_ok)
    {
        TRACE("SBT: Calling action handler...\n");
      
		// Not able to find PLMEasy .lib files
		//ITK(TCPB_AH_send_mail(mail_msg));
    }

    TRACE1("SBT: Retcode at the end after sending an Email: %d\n", iRetcode);
 
    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_wf_send_plme_mail */


/**
 * @brief lists all email adresses attached to any signoff tasks attached to 
 *        the root taks given.
 * @param root_task_tag            [in]  The tag of the root task for which to list all 
 *                                       signoff email adresses.
 * @param task_type                [in]  The type of the task from which to read the emails.
 * @param signoff_email_string_set [out] The string set of the signoff emails found.
 */
static int sbt_wf_list_all_signoff_email_adresses
  (
    tag_t root_task_tag,                                /* I */
    EPM_task_type_t task_type,                          /* I */
//hfhp 02-Apr-2009 replaced by C construct
//  std::set<std::string> &signoff_email_string_set     /* O */
	int		* piStringCount ,							/* O */
	char	***  pppcSignOffEmailStringSet				/* OF */
  )
{
    int iRetcode = ITK_ok;

	int		iStringCount = 0 ;						
	char	** ppcSignOffEmailStringSet = NULL ;

    const char* theFunction = "sbt_wf_list_all_signoff_email_adresses";

    ENTER(theFunction);

	* piStringCount = iStringCount ;
	* pppcSignOffEmailStringSet = ppcSignOffEmailStringSet ;

    tag_t job_tag = NULLTAG;
    ITK(EPM_ask_job(root_task_tag, &job_tag));

    int signoff_task_count = 0;
    tag_t *signoff_task_tags = NULL;
    if (iRetcode == ITK_ok && job_tag != NULLTAG)
    {
        ITK(EPM_get_type_tasks(job_tag, 
                                              task_type, 
                                              &signoff_task_count, 
                                              &signoff_task_tags));
    }

    int soteam_user_count = 0;
    tag_t *soteam_user_tags = NULL;
    tag_t person_tag = NULLTAG;
    char  *pcUserEmail = NULL	;


    for (int ii=0; 
         ii<signoff_task_count && iRetcode == ITK_ok; 
         ++ii)
    {
        ITK(SBT_WF_signofftask_ask_singoff_team_users(signoff_task_tags[ii],
                                                                     &soteam_user_count,
                                                                     &soteam_user_tags));
        for (int jj = 0; jj < soteam_user_count && iRetcode == ITK_ok; ++jj)
        {
            if (iRetcode == ITK_ok && soteam_user_tags[jj] != NULLTAG)
            {
                ITK(SA_ask_user_person(soteam_user_tags[jj],
                                                      &person_tag));
            }


            /* Get the email address from the PA9 attribute */
            pcUserEmail = NULL ;
            if (iRetcode == ITK_ok && person_tag != NULLTAG)
            {
//hfhp 02-Apr-2009 replaced obsolet function
//              ITK(SA_ask_person_attribute(person_tag, 
//                                                       (char*)("PA9"), 
//                                                       userEmail));
				ITK(SA_ask_person_attr(person_tag, 
                                                         (char*)("PA9"), 
                                                         &pcUserEmail));
            }

            if ( ( iRetcode == ITK_ok ) && ( pcUserEmail != NULL ) && ( strcmp ( pcUserEmail , "") != 0 ) )
            {
//hfhp 02-Apr-2009 replaced by C construct
//				signoff_email_string_set.insert(userEmail);
				iStringCount = iStringCount + 1 ;
				ppcSignOffEmailStringSet = (char**)MEM_realloc ( ppcSignOffEmailStringSet , iStringCount * sizeof(char*) ) ;
				ppcSignOffEmailStringSet[iStringCount-1] = (char*)MEM_alloc ( (strlen(pcUserEmail)+1) * sizeof(char) ) ;
				strcpy ( ppcSignOffEmailStringSet[iStringCount-1] , pcUserEmail ) ;
				MEM_free ( pcUserEmail ) ;
            }
        }

        // clean up
        MEM_free(soteam_user_tags);
        soteam_user_tags = 0;
    }

    // clean up
    MEM_free(signoff_task_tags);

	* piStringCount = iStringCount ;
	* pppcSignOffEmailStringSet = ppcSignOffEmailStringSet ;

    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_wf_list_all_signoff_email_adresses */


/* End file SBT_Workflow_grouphandler.c */
