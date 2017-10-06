/*
	13-May-2009		Hartmuth Petzold	deactivated registration of SBT_WF_show_instruction_by_group_obj_desc_action_handler(...)
	27-May-2009		Hartmuth Petzold	changed version from 2.01 to 2.02
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

//#include "ugs_util_macros.h"
//#include "ugs_util_string.h"
//#define DEBUG_VARIABLES_DECLARE 
#include "sy_debug.h"

#include "SBT_Workflow_grouphandler.h"
#include "SBT_01.h"

#include "rghStrs.h"

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




extern DLLAPI int SBT_01_register_callbacks ()
  {
  int retcode = ITK_ok;

  const char *theFunction = __FUNCTION__;
  TRACE_ON(0)
  ENTER (theFunction);


  fprintf(stderr,"\n  SBT_Workflow.dll %s build %s at %s for Teamcenter 8.3\n\n","8.3.01",__DATE__,__TIME__);


  // custom library, user exit, custom exit

  LEAVE (theFunction);
  return retcode ;
  }


/*
________________________________
TCE Handler: SBT-AH-assign-TM-id

ITK Funktion: SBT_AH_assign_TM_id()

Handler-Argumente:

-source_type  Typname des Quell-Items
              (bei nicht-Angabe Handlerabbruch)

-target_types  Komma-getrennte Liste von Typnamen der Ziel-Items
		       (bei nicht-Angabe Handlerabbruch)

-target_attribute  Name des Ziel-Attributes in den Itemrevision
                   Masterforms der unter -target_types 
                   angegebenen Itemtypen
                   (bei nicht-Angabe Handlerabbruch)

Beschreibung: 

Targetobjekte, die weder Quell- noch Ziel-ItemRevision sind, d.h.
deren Typnamen weder unter -source_type noch unter -target_types
aufgeführt sind, werden ignoriert.

Ziel-Itemrevisionen, deren Masterforms kein Attribut mit dem 
unter -target_attribute angegebenen Namen haben, werden ignoriert.

Falls unter den Targetobjekten keine Quell-Itemrevision vorhanden
ist, bricht der Handler ab.

Falls unter den Targetobjekten  mehrere Quell-Itemrevisionen 
vorhanden sind, wird die erste verwendet.

Die Id des Items der Quell-ItemRevision wird in das unter 
-target_attribute angegebene Attribut der Ziel-Itemrevisionen
geschrieben.


*/
int SBT_AH_assign_TM_id (
    EPM_action_message_t msg)
  {
  int retcode = ITK_ok;
  int i = 0,
      nArgs = 0;
  char *sArg = NULL,
       *sParentType = NULL,
       *sChildTypes = NULL,
       *sChildAttr = NULL;

  int nChildTypes = 0;
  char **psChildTypes = NULL;

  int nTrgts = 0;
  tag_t rootTaskTag = NULLTAG,
        *pTrgtTags = NULL;

  char sItemType [ITEM_type_size_c + 1],
       sIdParent [ITEM_type_size_c + 1]; 
  tag_t itemTag;
  int idxParent = -1;
  const char *theFunction = __FUNCTION__;
  TRACE_ON(0)
  ENTER (theFunction);



  // parse arguments
  if (retcode == ITK_ok && msg.arguments != NULL)
    {
    IMAN_init_argument_list (msg.arguments);
    nArgs = IMAN_number_of_arguments (msg.arguments);
    }
  for (i = 0; i < nArgs; i ++)
    {
    sArg = IMAN_next_argument (msg.arguments); 
    TRACE2 ("args [%d] = \"%s\"\n", i, sArg);

    if (strstr (sArg, "-source_type=") == sArg)
      { sParentType = strchr (sArg, '=') + 1; }

    if (strstr (sArg, "-target_types=") == sArg)
      { sChildTypes = strchr (sArg, '=' ) + 1; }

    if (strstr (sArg, "-target_attribute=") == sArg)
      { sChildAttr = strchr (sArg, '=' ) + 1; }

    }
  TRACE1 ("sParentType = \"%s\"\n", sParentType);
  TRACE1 ("sChildTypes = \"%s\"\n", sChildTypes);
  TRACE1 ("sChildAttr = \"%s\"\n", sChildAttr);
  if (sParentType == NULL || sChildTypes == NULL || sChildAttr == NULL)
    { goto __FUNCTION__return; }
  psChildTypes = rghStrs_newStrSep (sChildTypes, &nChildTypes, ',');
  rghStrs_trace (psChildTypes, nChildTypes, "ChildTypes");

  if ((retcode == ITK_ok) && (msg.task != NULLTAG))
    { ITK (EPM_ask_root_task (msg.task, &rootTaskTag)); }
  if ((retcode == ITK_ok) && (rootTaskTag != NULLTAG))
    { ITK (EPM_ask_attachments (rootTaskTag, EPM_target_attachment, &nTrgts,
                                &pTrgtTags)); }

  // target objects: get item id of parent
  for (i = 0; i < nTrgts; i ++) 
    {
    if (! bObjInstanceOf (pTrgtTags [i], "ItemRevision")) { continue; }
    ITK (ITEM_ask_item_of_rev (pTrgtTags [i], &itemTag));
    ITK (ITEM_ask_type (itemTag, sItemType));
    if (strcmp (sItemType, sParentType) == 0)
      {
      idxParent = i;
      ITK (ITEM_ask_id (itemTag, sIdParent));
      TRACE2 ("idxParent = %d, sIdParent = \"%s\"\n",idxParent, sIdParent);
      }
    }
  if (idxParent < 0) { goto __FUNCTION__return; }

  // target objects: set attribute value of children with item id of parent
  for (i = 0; i < nTrgts; i ++) 
    {
    if (! bObjInstanceOf (pTrgtTags [i], "ItemRevision")) { continue; }
    ITK (ITEM_ask_item_of_rev (pTrgtTags [i], &itemTag));
    ITK (ITEM_ask_type (itemTag, sItemType));
    if (rghStrs_bStrElem (psChildTypes, nChildTypes, sItemType))
      {
      tag_t relType, *secObjs = NULL;
      int nSecObjs = 0;
      ITK (GRM_find_relation_type (IMAN_master_form_rtype, &relType));
      ITK (GRM_list_secondary_objects_only (pTrgtTags [i], relType, &nSecObjs,
                                            &secObjs));
      if (nSecObjs > 0)
        {
        char **psPrpNames = NULL;
        int nPrps = 0;
        ITK (AOM_ask_prop_names (secObjs [0], &nPrps, &psPrpNames));
        if (rghStrs_bStrElem (psPrpNames, nPrps, sChildAttr))
          {
          ITK (AOM_refresh (secObjs [0], POM_modify_lock));
          ITK (AOM_set_value_string (secObjs [0], sChildAttr, sIdParent));
          ITK (AOM_save (secObjs [0])); 
          ITK (AOM_refresh (secObjs [0], POM_no_lock));
          }
        rghStrs_free (psPrpNames, nPrps);
        }
      MEM_free (secObjs);
      }
    }

__FUNCTION__return:


#undef SINGLE_UNIT_TEST
#ifdef SINGLE_UNIT_TEST
  /* For cleaning up, delete the job */
{
  tag_t  jobTag = NULLTAG;
  ITK (EPM_ask_job (rootTaskTag, &jobTag));
  ITK (AOM_refresh (jobTag, POM_modify_lock));
  ITK (AOM_delete (jobTag));
}
#endif

  MEM_free (pTrgtTags);  
  rghStrs_free (psChildTypes, nChildTypes);
  LEAVE (theFunction);
  return retcode;
  }


/**
 * @brief
 * 
 * Checks whether a given object is of the given class or of one of its 
 * descendants
 *
 * @param[in]  objTag       tag of object to check     
 * @param[in]  sClassName  name of class to check
 *
 * @return  true if the object is of the given class or of one of its 
 *               descendants,
 *          false otherwise
 */
logical bObjInstanceOf (
    const tag_t objTag,     // (I)
    const char *sClassName) // (I)
  {
  logical ret = FALSE;
  int retcode = ITK_ok;
  tag_t classIdTag = NULLTAG,
        objClassIdTag = NULLTAG;
  const char *theFunction = __FUNCTION__;
  TRACE_ON(0)
  ENTER (theFunction);
  // check args
  if (objTag == NULLTAG ||
      sClassName == NULL || strlen (sClassName) == 0)
    { goto __FUNCTION__return; }

  ITK (POM_class_id_of_class (sClassName, &classIdTag));
  if (retcode == ITK_ok && classIdTag != NULLTAG)
    {
    ITK (POM_class_of_instance (objTag, &objClassIdTag));
    if (retcode == ITK_ok && objClassIdTag != NULLTAG)
      { ITK (POM_is_descendant (classIdTag, objClassIdTag, &ret)); }
    }
__FUNCTION__return:
  LEAVE (theFunction);
  return ret;
  }



#ifdef __cplusplus
}
#endif
