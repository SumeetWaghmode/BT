/**
 * This file is to hold the implementation of the utility methods used to deal with 
 * items.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <pom/enq/enq.h>
#include <tc/emh.h>
#include <ss/ss_errors.h>

#include "sy_debug.h"

#include "SBT_Query.h"

#include "SBT_Item.h"

#ifdef __cplusplus
}
#endif

/*
 * @brief Get the revisions of an item which have the specified status.
 * @param item_tag    [in]  The item where to get the rev list from.
 * @param status_name [in]  The status the returned revisions should have.
 * @param rev_count   [out] The number of found revisions.
 * @param rev_tags    [out] (OF) list of found revisions.
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_ITEM_list_release_status_revs(tag_t item_tag,
                                      const char *status_name,
                                      int *rev_count,
                                      tag_t **rev_tags)
{
/*
SELECT ItemRevision.puid
FROM   Item, ItemRevision, ReleaseStatus
WHERE  Item.puid = <item_tag>
AND    ItemRevision.items_tag = Item.puid
AND    ItemRevision.release_status_list = ReleaseStatus.puid
AND    ReleaseStatus.name = <status>
*/

    sbt_query_t query;

    const char* theFunction = "SBT_Item/SBT_ITEM_list_release_status_revs";
    TRACE_ON(0);
    ENTER(theFunction);

    ITK(SBT_QUERY_init_query(&query));

    if (retcode == ITK_ok)
    {
        ITK(POM_enquiry_add_select_attrs(query.enq_id,
                                                  (char*) "ItemRevision",
                                                  1,
                                                  &SBT_QUERY_attr_puid));
    }
    if (retcode == ITK_ok)
    {
        ITK(SBT_QUERY_attr_relation(&query,
                                              "Item",
                                              "ItemRevision",
                                              "items_tag"));
    }
    if (retcode == ITK_ok)
    {
        ITK(SBT_QUERY_set_tag_attr(&query,
                                             "Item",
                                             "puid",
                                             POM_enquiry_equal,
                                             item_tag));
    }

    if (retcode == ITK_ok)
    {
        ITK(SBT_QUERY_attr_relation(&query,
                                              "ReleaseStatus",
                                              "ItemRevision",
                                              "release_status_list"));
    }

    if (retcode == ITK_ok)
    {
        ITK(SBT_QUERY_set_string_attr(&query,
                                               "ReleaseStatus",
                                               "name",
                                               POM_enquiry_equal,
                                               status_name));
    }

    if (retcode == ITK_ok)
    {
        ITK(POM_enquiry_add_order_attr(query.enq_id,
                                                (char *)"ItemRevision",
                                                (char *)"creation_date",
                                                POM_enquiry_asc_order));
    }

    if (retcode == ITK_ok)
    {
        ITK(SBT_QUERY_execute(&query));
    }

    if (retcode == ITK_ok && query.rows > 0)
    {
        *rev_tags = (tag_t*) MEM_alloc(query.rows * sizeof(tag_t));

        if (*rev_tags != NULL)
        {
            int i;
            for (i = 0; i < query.rows; i++)
            {
                (*rev_tags)[i] = *((tag_t*) (query.result[i][0]));
            }
            *rev_count = query.rows;
        }
        else
        {
            retcode = SS_NOMEM;
        }
    }

    SBT_QUERY_free_query(&query);

    LEAVE(theFunction);
    return retcode;

} /* End SBT_ITEM_list_release_status_revs */


/* End file SBT_Item.c */
