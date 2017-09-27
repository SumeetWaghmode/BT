/**
 * This file is to hold the implementation of the utility methods used to deal with 
 * workspaceobjects in general.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <tccore/grm.h>
#include <tc/emh.h>

#include "sy_debug.h"

#include "SBT_Workspaceobject.h"

#ifdef __cplusplus
}
#endif

/*
 * @brief get the workspaceobject assigned to a given parent with a given reference name.
 * @param parent_tag [in] The parent where the workspaceobject is assigned to.
 * @param reference_name [in] The name of the relation the workspaceobject is assigned with.
 * @param assigned_wso_tag [out] The assigned workspaceobject found.
 */
int SBT_Workspaceobject_ask_assigned_wso
 (
  tag_t parent_tag,
  const char *reference_name,
  tag_t *assigned_wso_tag
 )
{

  int retcode = ITK_ok;

  int count = 0;
  tag_t reference_assignment = NULLTAG;
  tag_t *relations = NULL;

  const char* theFunction = "SBT_Workspaceobject/SBT_Workspaceobject_ask_assigned_wso";
  //TRACE_ON(0);
  ENTER(theFunction);

  // preset return variable
  *assigned_wso_tag = NULLTAG;

  ITK(GRM_find_relation_type(reference_name, &reference_assignment));

  if (retcode == ITK_ok && reference_assignment != NULLTAG) 
  {
    ITK(GRM_list_relations(parent_tag,
                    NULLTAG,
                    reference_assignment,
                    NULLTAG,
                    &count,
                    &relations));
  }

  if (retcode == ITK_ok && count > 0) 
  {
    ITK(GRM_ask_secondary(relations[0], assigned_wso_tag));
  }    

  // clean up
  if (relations)
    MEM_free(relations);

  LEAVE(theFunction);
  return (retcode);

} /* End SBT_Workspaceobject_ask_assigned_wso */


/* End file SBT_Workspaceobject.h */
