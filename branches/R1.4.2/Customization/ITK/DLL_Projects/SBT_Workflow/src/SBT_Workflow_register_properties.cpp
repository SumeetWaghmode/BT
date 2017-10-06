/**
 * This file registers the properties used in library SBT_Workflow.
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <tc\iman.h>
#include <user_exits\user_exits.h>
#include <tccore\imantype.h>
#include <ict\ict_userservice.h>
#include <tccore\custom.h>
#include <property\propdesc.h>
#include <tccore\workspaceobject.h>
#include <tccore\item.h>

//#include <eds_itktool.h>
#include <epm\epm_task_template_itk.h>
#include "sy_debug.h"

#include "SBT_File.h"
#include "SBT_Utilities_d.h"

#include "SBT_Workflow_Attributes.h"
#include "SBT_Workflow_grouphandler.h"

#define DllExport extern  __declspec(dllexport)

/**
 * @brief The function to register the EPM task runtime property 
 */
static int sbt_epm_init_runtime_props(METHOD_message_t* m,
                                      va_list args);

/**
 * @brief The function to register the EPM task runtime property 
 */
static int sbt_itemrevision_init_runtime_props(METHOD_message_t* m,
                                      va_list args);

/**
 * @brief This method initialises the runtime property of the EPM task
 */
static int sbt_wf_init_epm_task_props(tag_t type_tag,
                                      char *type_name);

/**
 * @brief This method initialises the runtime property of the EPM task
 */
static int sbt_wf_init_itemrevision_props(tag_t type_tag,
										  char *type_name);

/**
 * @brief The method to read the runtime property 
 */
 int sbt_wf_get_epm_group_task_string(METHOD_message_t* m,
                                            va_list args);

/**
 * @brief The method to read the runtime property 
 */
 int sbt_wf_get_itemrevision_superseeded_string(METHOD_message_t* m,
                                            va_list args);


#ifdef __cplusplus
}
#endif


/**
 * @brief this function handles the registration of the runtime properties.
 */
int SBT_Workflow_register_properties(int *pDecision , 
                                     va_list arg)
{
    int ifail    = ITK_ok;

    // preset return variables
	*pDecision = ALL_CUSTOMIZATIONS ;

    if ( ifail == ITK_ok )
    {

        /*
         * You may declare your function prototypes here or in another file
         * #included by this one. Example:
         *
         * extern int       SMPTYPE_prop_init_module();
         */
    
        /*
         * This array is used for registering Methods which initialize properties
         * on types. The message name is IMANTYPE_init_user_props_msg. No coding
         * is necessary in this function. Just add an entry in this array to
         * indicate the object Type, the function which implements the property
         * additions/modifications and the list of arguments.
         *
         * The array should be terminated with a NULL entry.
         */
        USER_prop_init_entry_t user_types_methods[] =
        {
            /*
            { "SampleType", SMPTYPE_prop_init_module        , NULL },
            */
//            { (char*)"GDELine",                EE_SIGRTP_register_FN_Port_interface_run_time_props , NULL },
//            { (char*)"FN_Port_Interface",      EE_SIGRTP_register_FN_Port_relations , NULL },
            { (char *)"ItemRevision", sbt_itemrevision_init_runtime_props, NULL },
            { (char *)"EPMPerformSignoffTask", sbt_epm_init_runtime_props, NULL },
            { (char*)"",                       0       , NULL }
        };
    
        int n_types = sizeof(user_types_methods)/sizeof(USER_prop_init_entry_t);
    
        /*
         * Do not edit this call. This call is not to be used elsewhere.
         */
        ifail = IMANTYPE_register_properties(user_types_methods, n_types);
    }

    return ifail;

} /* End SBT_Workflow_register_properties */





/**
 * @brief The function to register the ItemRevision runtime property 
 */
static int sbt_itemrevision_init_runtime_props(METHOD_message_t* m,
                                      va_list args)
{
    int iRetcode = ITK_ok;

    tag_t type_tag = va_arg(args, tag_t);
    char type_name[IMANTYPE_name_size_c + 1];

    const char* theFunction = "SBT_WF_register_properties/sbt_itemrevision_init_runtime_props";

    ENTER(theFunction);

    memset(type_name, 0, sizeof(type_name));

    ITK(IMANTYPE_ask_name(type_tag, type_name));

    if (iRetcode == ITK_ok)
    {
        TRACE2("###### Registering type with tag %d, type name %s\n", type_tag, type_name);
    }

    if (iRetcode == ITK_ok) 
    {
		//tag_t	tItemRevisionClassIdTag = NULLTAG;
		//tag_t	tObjectClassIdTag		= NULLTAG;
		//logical	lIsDescendant			= false;

		//// Get Class IDs
		//ITK(POM_class_id_of_class("ItemRevision", &tItemRevisionClassIdTag));
		//ITK(POM_class_id_of_class(type_name, &tObjectClassIdTag ) ) ;

		//// Is object derived from ItemRevision?
		//ITK(POM_is_descendant(tItemRevisionClassIdTag, tItemRevisionClassIdTag, &lIsDescendant));

  //      if (lIsDescendant == true)
  //      {
            ITK(sbt_wf_init_itemrevision_props(type_tag, type_name));
		//}
    } 

    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_itemrevision_init_runtime_props */






/**
 * @brief The function to register the EPM task runtime property 
 */
static int sbt_epm_init_runtime_props(METHOD_message_t* m,
                                      va_list args)
{
    int iRetcode = ITK_ok;

    tag_t type_tag = va_arg(args, tag_t);
    char type_name[IMANTYPE_name_size_c + 1];

    const char* theFunction = "SBT_WF_register_properties/sbt_epm_init_runtime_props";

    ENTER(theFunction);

    memset(type_name, 0, sizeof(type_name));

    ITK(IMANTYPE_ask_name(type_tag, type_name));

    if (iRetcode == ITK_ok)
    {
        TRACE2("###### Registering type with tag %d, type name %s\n", type_tag, type_name);
    }

    if (iRetcode == ITK_ok) 
    {
        if (strcmp(type_name, "EPMPerformSignoffTask") == 0)
        {
            ITK(sbt_wf_init_epm_task_props(type_tag, type_name));
        }
    } 

    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_epm_init_runtime_props */




/**
 * @brief This method initialises the runtime property of the EPM task
 */
static int sbt_wf_init_itemrevision_props(tag_t type_tag,
                                      char *type_name)
{
    int iRetcode = ITK_ok;

    tag_t pd_tag = NULLTAG;
    METHOD_id_t method;

    const char* theFunction = "SBT_WF_register_properties/sbt_wf_init_itemrevision_props";
    
    ENTER(theFunction);

    ITK(IMANTYPE_add_runtime_property(type_tag,
		SBT_WF_revisionsuperseeded_rtp_name,
		PROP_string,
		SBT_WF_revisionsuperseeded_prop_len+1,
		&pd_tag));

    if (iRetcode == ITK_ok) 
    {
        ITK(PROPDESC_set_display_name(pd_tag, SBT_WF_revisionsuperseeded_rtp_name));
    }

    if (iRetcode == ITK_ok) {

        ITK(METHOD_register_prop_method((const char *) type_name,
                                                       SBT_WF_revisionsuperseeded_rtp_name,
                                                       PROP_ask_value_string_msg,
                                                       sbt_wf_get_itemrevision_superseeded_string,
                                                       0,
                                                       &method));
    }

    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_wf_init_itemrevision_props */





/**
 * @brief This method initialises the runtime property of the EPM task
 */
static int sbt_wf_init_epm_task_props(tag_t type_tag,
                                      char *type_name)
{
    int iRetcode = ITK_ok;

    tag_t pd_tag = NULLTAG;
    METHOD_id_t method;

    const char* theFunction = "SBT_WF_register_properties/sbt_wf_init_epm_task_props";
    
    ENTER(theFunction);

    ITK(IMANTYPE_add_runtime_property(type_tag,
                                                    SBT_WF_groupdependent_rtp_name,
                                                    PROP_string,
                                                    SBT_WF_groupdependent_prop_len+1,
                                                    &pd_tag));

    if (iRetcode == ITK_ok) 
    {
        ITK(PROPDESC_set_display_name(pd_tag, SBT_WF_groupdependent_rtp_name));
    }

    if (iRetcode == ITK_ok) {

        ITK(METHOD_register_prop_method((const char *) type_name,
                                                       SBT_WF_groupdependent_rtp_name,
                                                       PROP_ask_value_string_msg,
                                                       sbt_wf_get_epm_group_task_string,
                                                       0,
                                                       &method));
    }

    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_wf_init_epm_task_props */



/**
 * @brief The method to read the runtime property 
 */
int sbt_wf_get_itemrevision_superseeded_string(METHOD_message_t *m, va_list args)
{
    tag_t   prop_tag		= va_arg(args, tag_t);
    char**  value			= va_arg(args, char **);

    int		iRetcode		= ITK_ok;
	tag_t	tItem			= NULLTAG;
	tag_t	tItemRev		= NULLTAG;
	tag_t	tLatestItemRev	= NULLTAG;

    const char* theFunction = "SBT_WF_register_properties/sbt_wf_get_itemrevision_superseeded_string";

    ENTER(theFunction);

    /* Initialization */
    *value = NULL;

    /* get the owning object of the property */
    ITK(PROP_ask_owning_object(prop_tag, &tItemRev));

	/* Get Item */
	ITK(ITEM_ask_item_of_rev(tItemRev, &tItem));

	/* Get Latest Revision */
	ITK(ITEM_ask_latest_rev(tItem, &tLatestItemRev));

	/* Current Rev == Latest Rev ? */
	if (tItemRev == tLatestItemRev)
    {
        *value = SBT_string_copy("");
    } else {
        *value = SBT_string_copy(" !");
    }

    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_wf_get_itemrevision_superseeded_string */




/**
 * @brief The method to read the runtime property 
 */
int sbt_wf_get_epm_group_task_string(METHOD_message_t *m, va_list args)
{
    tag_t   prop_tag = va_arg(args, tag_t);
    char**  value = va_arg(args, char **);

    tag_t task_tag = NULLTAG;
    tag_t job_tag = NULLTAG;
    tag_t grp_dataset_tag = NULLTAG;
    tag_t grp_file_tag    = NULLTAG;

    int     str_array_count = 0;
    char**  str_array = NULL;

    int	iRetcode = ITK_ok;

    const char* theFunction = "SBT_WF_register_properties/sbt_wf_get_epm_group_task_string";

    ENTER(theFunction);

    /* Initialization */
    *value = NULL;

    /* get the owning object of the property */

    ITK(PROP_ask_owning_object(prop_tag, &task_tag));

    if (iRetcode == ITK_ok)
    {
        TRACE("SBT: Looking for group specific dataset attached to task...\n");
        ITK(SBT_WF_attr_task_ask_group_dataset(task_tag, &grp_dataset_tag));
    }
    if (grp_dataset_tag == NULLTAG)
    {
        TRACE("SBT: Group specific dataset tag not found!!!\n");
    }
    else
    {
        TRACE("SBT: Group specific dataset tag successfully found.\n");
    }

    // use the attachment found
    if (iRetcode == ITK_ok && grp_dataset_tag != NULLTAG)
    {
        TRACE("SBT: Looking for ImanFile in dataset...\n");
        ITK(SBT_Dataset_ask_assigned_file_tag(grp_dataset_tag,
                                                             SBT_WF_groupdependent_info_attachment,
                                                             0,   /* index */
                                                             &grp_file_tag));
    }
    if (grp_file_tag == NULLTAG)
    {
        TRACE("SBT: ImanFile not found!!!\n");
    }
    else
    {
        TRACE("SBT: ImanFile successfully found.\n");
    }

    if (iRetcode == ITK_ok && grp_file_tag != NULLTAG)
    {
        ITK(SBT_File_tag_to_string_array(grp_file_tag,
                                                        &str_array_count,
                                                        &str_array));
    }

    if (str_array_count > 0)
    {
        *value = SBT_string_copy(str_array[0]);
    }

    for (int i=1; i < str_array_count; ++i)
    {
        if ((int)strlen(*value) > SBT_WF_groupdependent_prop_len)
            break;

        *value = SBT_string_append(*value, "\n");
        *value = SBT_string_append(*value, str_array[i]);
    }

    if (iRetcode == ITK_ok && *value == 0)
    {
        //*value = SM_string_copy_persistent(SBT_WF_not_groupdependent_std_msg);
        *value = SBT_string_copy(" ");
    }

    //cope with texts that are too long
    if (iRetcode == ITK_ok && *value && ((int) strlen(*value) > SBT_WF_groupdependent_prop_len))
    {
        (*value)[SBT_WF_groupdependent_prop_len] = '\0';
    }

    // clean up
    for (int i = 0; i < str_array_count; ++i)
    {
        MEM_free(str_array[i]);
    }
    MEM_free(str_array);


    LEAVE(theFunction);
    return iRetcode;

} /* End sbt_wf_get_epm_group_task_string */


/* End file SBT_Workflow_register_properties.c */
