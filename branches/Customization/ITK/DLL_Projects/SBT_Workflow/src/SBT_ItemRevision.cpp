/**
 * This file is to hold the implementation of the utility methods used to deal with 
 * item revisions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <tc/emh.h>

#include "sy_debug.h"

#include "SBT_Item.h"

#include "SBT_ItemRevision.h"

#ifdef __cplusplus
}
#endif

/*
 * @brief Get the latest revision of an item with the specified status.
 * @param item_tag    [in]  The item where to get the latest rev from
 * @param status_name [in]  The status the returned revisions should have.
 * @param rev_tag     [out] The revision found.
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_ITEMREV_ask_latest_rev_with_status(tag_t item_tag,          /* I */
                                           const char *status_name, /* I */
                                           tag_t *rev_tag)          /* O */
{
    int retcode = ITK_ok;

    int count = 0;
    tag_t *rev_tags = NULL;

    const char* theFunction = "sbt_item/SBT_ITEMREV_ask_latest_rev_with_status";
    TRACE_ON(0);
    ENTER(theFunction);

    // preset return variable
    *rev_tag = NULLTAG;

    ITK(SBT_ITEM_list_release_status_revs(item_tag,
                                                   status_name,
                                                   &count,
                                                   &rev_tags));

    if (retcode == ITK_ok && count > 0)
    {
        *rev_tag = rev_tags[count-1];
    }

    // clean up
    MEM_free(rev_tags);

    LEAVE(theFunction);
    return retcode;

} /* End SBT_ITEMREV_ask_latest_rev_with_status */

/* End file SBT_ItemRevision.c */
