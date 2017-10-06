/**
 * This file holds all utility functions concerning item revisions.
 */

#ifndef _SBT_ITEMREVISION_H
#define _SBT_ITEMREVISION_H

#include <tc/iman.h>

/**
 * @brief Get the latest revision of an item with the specified status.
 * @param item_tag    [in]  The item where to get the latest rev from
 * @param status_name [in]  The status the returned revisions should have.
 * @param rev_tag     [out] The revision found.
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_ITEMREV_ask_latest_rev_with_status(tag_t item_tag,          /* I */
                                           const char *status_name, /* I */
                                           tag_t *rev_tag);         /* O */
#endif

/* End file SBT_ItemRevision.h */
