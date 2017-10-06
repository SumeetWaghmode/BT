/**
 * This file holds all utility functions concerning items.
 */

#ifndef _SBT_ITEM_H
#define _SBT_ITEM_H

#include <tc/iman.h>

/**
 * @brief Get the revisions of an item which have the specified status.
 * @param item_tag    [in]  The item where to get the rev list from.
 * @param status_name [in]  The status the returned revisions should have.
 * @param rev_count   [out] The number of found revisions.
 * @param rev_tags    [out] (OF) list of found revisions.
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_ITEM_list_release_status_revs(tag_t item_tag,          /* I */
                                      const char *status_name, /* I */
                                      int *rev_count,          /* O */
                                      tag_t **rev_tags);       /* OF */

#endif

/* End file SBT_Item.h */
