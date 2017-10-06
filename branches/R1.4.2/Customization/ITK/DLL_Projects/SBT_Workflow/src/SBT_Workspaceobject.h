/**
 * This file is to hold all utility methods used to deal with workspaceobjects
 * in general.
 */

#ifndef _SBT_WORKSPACEOBJECT_H
#define _SBT_WORKSPACEOBJECT_H

#include <tc/iman.h>

/**
 * @brief Get the workspaceobject assigned to a given parent with a given reference name.
 *    Only the first object is returned, so if there are more,
 *    do not use this function!
 * @param parent_tag [in] The parent where the dataset is assigned to.
 * @param reference_name [in] The name of the relation the dataset is assigned with.
 * @param assigned_wso_tag [out] The assigned wso found.
 *                A NULLTAG if no workspaceobject was found
 */
int SBT_Workspaceobject_ask_assigned_wso
 (
  tag_t parent_tag,
  const char *reference_name,
  tag_t *assigned_wso_tag
 );

#endif

/* End file SBT_Workspaceobject.h */
