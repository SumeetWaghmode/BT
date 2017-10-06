/**
 * This file is the implementation of the query module.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <tc/iman.h>
#include <pom/enq/enq.h>
#include <tc/emh.h>
#include <ss/ss_errors.h>

#include <string.h>
#include <stdlib.h>

#include "SBT_Query.h"
#include "sy_debug.h"

static logical use_bind();

#ifdef __cplusplus
}
#endif

static int query_counter = 0;

int SBT_QUERY_init_query(sbt_query_t* query)
{
    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_init_query");
    retcode = ITK_ok;

    if(query != NULL)
    {
        sprintf(query->enq_id, "ec_qry_%d", query_counter++);
        query->expr_counter = 0;
        query->val_counter = 0;
        query->table_counter = 0;
        query->rows = 0;
        query->cols = 0;
        query->result = NULL;

        ITK(POM_enquiry_create(query->enq_id));
    }
    else
        retcode = SS_NOMEM;

    LEAVE("sbt_query/SBT_QUERY_init_query");
    return retcode;
}

int SBT_QUERY_init_sub_query(sbt_query_t* query,
                              sbt_query_t* sub_query)
{
    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_init_sub_query");
    retcode = ITK_ok;

    if(sub_query != NULL)
    {
        sprintf(sub_query->enq_id, "ec_qry_%d", query_counter++);
        sub_query->expr_counter = 0;
        sub_query->val_counter = 0;
        sub_query->table_counter = 0;
        sub_query->rows = 0;
        sub_query->cols = 0;
        sub_query->result = NULL;

        ITK(POM_enquiry_set_sub_enquiry(query->enq_id, sub_query->enq_id));
    }
    else
        retcode = SS_NOMEM;

    LEAVE("sbt_query/SBT_QUERY_init_sub_query");
    return retcode;
}

int SBT_QUERY_free_query(sbt_query_t* query)
{
    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_free_query");
    retcode = ITK_ok;

    if(query != NULL)
    {
        MEM_free(query->result);
        POM_enquiry_delete(query->enq_id);

        query->enq_id[0] = '\0';
        query->expr_counter = 0;
        query->val_counter = 0;
        query->table_counter = 0;
        query->rows = 0;
        query->cols = 0;
        query->result = NULL;
    }

    LEAVE("sbt_query/SBT_QUERY_free_query");
    return retcode;
}


int SBT_QUERY_set_attr_expr(sbt_query_t* query,
                             const char* table,
                             const char* attr,
                             int operator_,
                             const char* value)
{
    char expr_id[128];

    sprintf(expr_id, "expr_%d", query->expr_counter++);
    ITK(POM_enquiry_set_attr_expr(query->enq_id, expr_id, (char*)table, (char*)attr, operator_, (char*)value));

    return retcode;
}


int SBT_QUERY_set_expr_proprety(sbt_query_t* query,
                                 int property_)
{
    char expr_id[128];

    sprintf(expr_id, "expr_%d", query->expr_counter - 1);
    ITK(POM_enquiry_set_expr_proprety(query->enq_id, expr_id, property_));

    return retcode;
}


int SBT_QUERY_set_tag_attrs(sbt_query_t* query,
                             const char* table,
                             const char* attr,
                             int operator_,
                             int count,
                             tag_t* values)
{
    char val_id[128];

    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);

    if (use_bind()) {
        ITK(POM_enquiry_set_tag_value(query->enq_id, val_id, count, values, POM_enquiry_bind_value));
    } else {
        ITK(POM_enquiry_set_tag_value(query->enq_id, val_id, count, values, POM_enquiry_const_value));
    }

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_set_attr_expr(query, table, attr, operator_, val_id));

    return retcode;
}

int SBT_QUERY_set_tag_attr(sbt_query_t* query,
                            const char* table,
                            const char* attr,
                            int operator_,
                            tag_t value)
{
    return SBT_QUERY_set_tag_attrs(query, table, attr, operator_, 1, &value);
}

int SBT_QUERY_set_int_attr(sbt_query_t* query,
                            const char* table,
                            const char* attr,
                            int operator_,
                            int value)
{
    char val_id[128];

    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);
    if (use_bind()) {
        ITK(POM_enquiry_set_int_value(query->enq_id, val_id, 1, &value, POM_enquiry_bind_value));
    } else {
    ITK(POM_enquiry_set_int_value(query->enq_id, val_id, 1, &value, POM_enquiry_const_value));
    }

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_set_attr_expr(query, table, attr, operator_, val_id));

    return retcode;
}

int SBT_QUERY_set_logical_attr(sbt_query_t* query,
                                const char* table,
                                const char* attr,
                                int operator_,
                                logical value)
{
    return SBT_QUERY_set_logical_attrs(query, table, attr, operator_, 1, &value);
}


int SBT_QUERY_set_logical_attrs(sbt_query_t* query,
                                 const char* table,
                                 const char* attr,
                                 int operator_,
                                 int count,
                                 logical* values)
{
    char val_id[128];

    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);
    if (use_bind()) {
        ITK(POM_enquiry_set_logical_value(query->enq_id, val_id, count, values, POM_enquiry_bind_value));
    } else {
        ITK(POM_enquiry_set_logical_value(query->enq_id, val_id, count, values, POM_enquiry_const_value));
    }

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_set_attr_expr(query, table, attr, operator_, val_id));

    return retcode;
}


int SBT_QUERY_set_string_attrs(sbt_query_t* query,
                                const char* table,
                                const char* attr,
                                int operator_,
                                int count,
                                const char** value)
{
    char val_id[128];

    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);
    if (use_bind()) {
        ITK(POM_enquiry_set_string_value(query->enq_id, val_id, count, value, POM_enquiry_bind_value));
    } else {
        ITK(POM_enquiry_set_string_value(query->enq_id, val_id, count, value, POM_enquiry_const_value));
    }

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_set_attr_expr(query, table, attr, operator_, val_id));

    return retcode;
}

int SBT_QUERY_set_string_attr(sbt_query_t* query,
                               const char* table,
                               const char* attr,
                               int operator_,
                               const char* value)
{
    return SBT_QUERY_set_string_attrs(query,
                                       table,
                                       attr,
                                       operator_,
                                       1,
                                       &value);
}

int SBT_QUERY_set_like_string_attr(sbt_query_t* query,
                                    const char* table,
                                    const char* attr,
                                    const char* value)
{
    char expr_id[128];
    char val_id[128];

    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);
    sprintf(expr_id, "expr_%d", query->expr_counter++);

    /* Hint: Do NEVER use const values at this point or substitute 
     * wildcard characters for your own. Bind takes care about wildcard characters. */

    ITK(POM_enquiry_set_string_value(query->enq_id,
                                              val_id,
                                              1,
                                              &value,
                                              POM_enquiry_bind_value));

    if(retcode == ITK_ok) {

        const char *p = strchr(value, (int)'*');

        if (p == NULL) {

            ITK(POM_enquiry_set_attr_expr(query->enq_id,
                                                   expr_id,
                                                   (char*)table,
                                                   (char*)attr,
                                                   POM_enquiry_equal,
                                                   val_id));

        } else {

            ITK(POM_enquiry_set_attr_expr(query->enq_id,
                                                   expr_id,
                                                   (char*)table,
                                                   (char*)attr,
                                                   POM_enquiry_like,
                                                   val_id));
        }
    }

    if (retcode == ITK_ok) {

        ITK(POM_enquiry_set_expr_proprety(query->enq_id,
                                                   expr_id,
                                                   POM_case_insensitive));
    }

    return retcode;
}

int SBT_QUERY_set_date_attr(sbt_query_t* query,
                                const char* table,
                                const char* attr,
                                int operator_,
                                date_t value)
{
    return SBT_QUERY_set_date_attrs(query, table, attr, operator_, 1, &value);
}


int SBT_QUERY_set_date_attrs(sbt_query_t* query,
                                 const char* table,
                                 const char* attr,
                                 int operator_,
                                 int count,
                                 date_t* values)
{
    char val_id[128];

    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);
    if (use_bind()) {
        ITK(POM_enquiry_set_date_value(query->enq_id, val_id, count, values, POM_enquiry_bind_value));
    } else {
        ITK(POM_enquiry_set_date_value(query->enq_id, val_id, count, values, POM_enquiry_const_value));
    }

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_set_attr_expr(query, table, attr, operator_, val_id));

    return retcode;
}


int SBT_QUERY_grm_relation_ex(sbt_query_t* query,
                               const char* parent,
                               const char* child,
                               const char* user_data,
                               const char* view_type_name)
{
    char relation_alias[128];
    char expr_id[128];

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_grm_relation_ex");

    retcode = ITK_ok;

    /* aliases */
    sprintf(relation_alias, "ImanRelation$%d", query->table_counter++);

    if(retcode == ITK_ok)
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"ImanRelation", TRUE, relation_alias));

    /* view type */
    if(retcode == ITK_ok && view_type_name != NULL)
    {
        char type_alias[128];
        sprintf(type_alias, "ImanType$%d", query->table_counter++);

        if(retcode == ITK_ok)
            ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"ImanType", TRUE, type_alias));

        if(retcode == ITK_ok)
        {
            sprintf(expr_id, "expr_%d", query->expr_counter++);
            ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, relation_alias, (char*)"relation_type", POM_enquiry_equal,
                type_alias, (char*)SBT_QUERY_attr_puid));
        }

        if(retcode == ITK_ok)
            ITK(SBT_QUERY_set_string_attr(query,
                                                    type_alias,
                                                    "type_name",
                                                    POM_enquiry_equal,
                                                    view_type_name));
    }
    
    /* user_data */
    if (retcode == ITK_ok)
    {
        if (user_data != NULL)
        {
            ITK(SBT_QUERY_attr_relation(query,
                                                  user_data,
                                                  relation_alias,
                                                  "user_data"));
        }
        else /* user_data == NULL */
        {
            ITK(SBT_QUERY_set_tag_attr(query,
                                                 relation_alias,
                                                 "user_data",
                                                 POM_enquiry_equal,
                                                 NULLTAG));
        }
    }

    /* joins */
    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, (char*)parent, (char*)SBT_QUERY_attr_puid,
            POM_enquiry_equal, relation_alias, (char*)"primary_object"));
    }

    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, relation_alias,
            (char*)"secondary_object", POM_enquiry_equal, (char*)child, (char*)SBT_QUERY_attr_puid));
    }

    LEAVE("sbt_query/SBT_QUERY_grm_relation_ex");
    return retcode;
}


int SBT_QUERY_grm_relation(sbt_query_t* query,
                            const char* parent,
                            const char* child,
                            const char* view_type_name)
{
    char relation_alias[128];
    char expr_id[128];

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_grm_relation");

    retcode = ITK_ok;

    /* aliases */
    sprintf(relation_alias, "ImanRelation$%d", query->table_counter++);

    if(retcode == ITK_ok)
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"ImanRelation", TRUE, relation_alias));

    /* view type */
    if(retcode == ITK_ok && view_type_name != NULL)
    {
        char type_alias[128];
        sprintf(type_alias, "ImanType$%d", query->table_counter++);

        if(retcode == ITK_ok)
            ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"ImanType", TRUE, type_alias));

        if(retcode == ITK_ok)
        {
            sprintf(expr_id, "expr_%d", query->expr_counter++);
            ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, relation_alias, (char*)"relation_type", POM_enquiry_equal,
                type_alias, (char*)SBT_QUERY_attr_puid));
        }

        if(retcode == ITK_ok)
            ITK(SBT_QUERY_set_string_attr(query,
                                                    type_alias,
                                                    "type_name",
                                                    POM_enquiry_equal,
                                                    view_type_name));
    }

    /* joins */
    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, (char*)parent, (char*)SBT_QUERY_attr_puid,
            POM_enquiry_equal, relation_alias, (char*)"primary_object"));
    }

    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, relation_alias,
            (char*)"secondary_object", POM_enquiry_equal, (char*)child, (char*)SBT_QUERY_attr_puid));
    }

    LEAVE("sbt_query/SBT_QUERY_grm_relation");
    return retcode;
}

int SBT_QUERY_bom_relation(sbt_query_t* query,
                            const char* parent,
                            const char* child,
                            const char* view_type_name)
{
    char expr_id[128];

    char bom_view_rev[128];
    char occurrence[128];

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_bom_relation");

    retcode = ITK_ok;

    /* aliases */
    sprintf(bom_view_rev, "PSBOMViewRevision$%d", query->table_counter++);
    sprintf(occurrence, "PSOccurrence$%d", query->table_counter++);

    if(retcode == ITK_ok)
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"PSBOMViewRevision", TRUE, bom_view_rev));
    if(retcode == ITK_ok)
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"PSOccurrence", TRUE, occurrence));

    /* view type */
    if(retcode == ITK_ok && view_type_name != NULL)
    {
        char bom_view[128];
        char view_type[128];

        sprintf(bom_view, "PSBOMView$%d", query->table_counter++);
        sprintf(view_type, "PSViewType$%d", query->table_counter++);

        if(retcode == ITK_ok)
            ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"PSBOMView", TRUE, bom_view));
        if(retcode == ITK_ok)
            ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"PSViewType", TRUE, view_type));

        if(retcode == ITK_ok)
        {
            sprintf(expr_id, "expr_%d", query->expr_counter++);
            ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, bom_view_rev, (char*)"bom_view",
                POM_enquiry_equal, bom_view, (char*)SBT_QUERY_attr_puid));
        }

        if(retcode == ITK_ok)
        {
            sprintf(expr_id, "expr_%d", query->expr_counter++);
            ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, bom_view, (char*)"view_type",
                POM_enquiry_equal, view_type, (char*)SBT_QUERY_attr_puid));
        }

        if(retcode == ITK_ok)
            ITK(SBT_QUERY_set_string_attr(query,
                                                    view_type,
                                                    "name",
                                                    POM_enquiry_equal,
                                                    view_type_name));
    }

    /* joins */
    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, (char*)parent,
            (char*)"structure_revisions", POM_enquiry_equal, bom_view_rev, (char*)SBT_QUERY_attr_puid));
    }

    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, bom_view_rev, (char*)SBT_QUERY_attr_puid,
            POM_enquiry_equal, occurrence, (char*)"parent_bvr"));
    }

    if(retcode == ITK_ok)
    {
        sprintf(expr_id, "expr_%d", query->expr_counter++);
        ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, occurrence, (char*)"child_item",
            POM_enquiry_equal, (char*)child, (char*)SBT_QUERY_attr_puid));
    }

    LEAVE("sbt_query/SBT_QUERY_bom_relation");
    return retcode;
}



int SBT_QUERY_attr_relation_ex(sbt_query_t* query,
                                const char* parent,
                                const char* parent_attr,
                                const char* child,
                                const char* child_attr)
{
    char expr_id[128];

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_attr_relation_ex");

    sprintf(expr_id, "expr_%d", query->expr_counter++);

#if 0
    fprintf(stderr, "seb log: expr_id=%s, parent=%s, parent_attr=%s, child=%s, child_attr=%s\n",
        expr_id, (char*)parent, (char*)parent_attr,
        (char*)child, (char*)child_attr);
#endif

    ITK(POM_enquiry_set_join_expr(query->enq_id, expr_id, (char*)parent, (char*)parent_attr,
        POM_enquiry_equal, (char*)child, (char*)child_attr));

    LEAVE("sbt_query/SBT_QUERY_attr_relation_ex");
    return retcode;
}

int SBT_QUERY_attr_relation_like(sbt_query_t* query,
                                  const char* parent,
                                  const char* parent_attr,
                                  const char* child,
                                  const char* child_attr)
{
    char expr_id[128];


    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_attr_relation");

    sprintf(expr_id, "expr_%d", query->expr_counter++);

#if 0
    fprintf(stderr, "seb log: expr_id=%s, parent=%s, parent_attr=%s, child=%s, child_attr=%s\n",
        expr_id, (char*)parent, (char*)parent_attr,
        (char*)child, (char*)child_attr);
#endif

    ITK(POM_enquiry_set_join_expr(query->enq_id,
                                           expr_id,
                                           (char*)parent,
                                           (char*)parent_attr,
                                           POM_enquiry_like,
                                           (char*)child,
                                           (char*)child_attr));

    if (retcode == ITK_ok) {

        ITK(POM_enquiry_set_expr_proprety(query->enq_id,
                                                   expr_id,
                                                   POM_case_insensitive));
    }

    LEAVE("sbt_query/SBT_QUERY_attr_relation");
    return retcode;
}

int SBT_QUERY_attr_relation(sbt_query_t* query,
                             const char* parent,
                             const char* child,
                             const char* attr_name)
{
    return SBT_QUERY_attr_relation_ex(query,
                                       parent,
                                       SBT_QUERY_attr_puid,
                                       child,
                                       attr_name);
}

int SBT_QUERY_masterform_relation(sbt_query_t* query,
                                   const char* parent,
                                   const char* child)
{
    return SBT_QUERY_form_relation(query, parent, child, "IMAN_master_form");
}

int SBT_QUERY_form_relation(sbt_query_t* query,
                             const char* parent,
                             const char* child,
                             const char* rel_type)
{
    char form[128];

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_form_relation");
    retcode = ITK_ok;

    /* aliases */
    sprintf(form, "Form$%d", query->table_counter++);

    /* joins */
    if(retcode == ITK_ok)
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"Form", TRUE, form));

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_grm_relation(query, parent, form, rel_type));

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_attr_relation(query, child, form, "data_file"));

    LEAVE("sbt_query/SBT_QUERY_form_relation");
    return retcode;
}


/*
  parent.from = substring(SR_Proxy.External_id, 1, 7)
  masterform_relation SR_proxy ->TP_rev
  TP_rev.type = "TaisPart_Rev"
*/
int SBT_QUERY_sachno_relation(sbt_query_t* query,
                               const char* parent,
                               const char* parent_attr,
                               const char* child)
{
    const int int_list[] = {1, 7};

    char substr_id[128];
    char val_id[128];
    char expr_id[128];


    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_sachno_relation");
    retcode = ITK_ok;

    sprintf(expr_id, "expr_%d", query->expr_counter++);
    sprintf(val_id, "%s_val_%d", query->enq_id, query->val_counter++);
    sprintf(substr_id, "%s_substr_expr$%d", query->enq_id, query->val_counter++);


    /*  parent.from = substring(SR_Proxy.External_id, 1, 7) */
    if(retcode == ITK_ok)
        ITK(POM_enquiry_set_int_value(query->enq_id, val_id, 2, int_list, POM_enquiry_const_value));

    if (retcode == ITK_ok)
        ITK(POM_enquiry_set_attr_expr(query->enq_id, 
                                               substr_id, 
                                               (char*)"SR_Proxy", 
                                               (char*)"External_Id",
                                               POM_enquiry_substr, 
                                               val_id));

    if (retcode == ITK_ok)
        ITK(POM_enquiry_set_attr_expr(query->enq_id, 
                                               expr_id, 
                                               (char*)parent, 
                                               (char*)parent_attr,
                                               POM_enquiry_equal, 
                                               substr_id));

    /* joins */
    if(retcode == ITK_ok)
        ITK(SBT_QUERY_masterform_relation(query, child, "SR_Proxy"));

    if(retcode == ITK_ok)
        ITK(SBT_QUERY_set_string_attr(query, child, "object_type", POM_enquiry_equal, "TaisPart_Rev"));

    LEAVE("sbt_query/SBT_QUERY_sachno_relation");
    return retcode;
}


int SBT_QUERY_history_relation(sbt_query_t* query,
                                const char* predecessor,
                                const char* successor,
                                const char* type,
                                tag_t fzg_prj_tag)
{
    char plink[128];
    char line[128];
    char history[128];
    char s_history[128];

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_history_relation");
    retcode = ITK_ok;

    if(retcode == ITK_ok) {
        sprintf(plink, "P_Link$%d", query->table_counter++);
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"P_Link", TRUE, plink));
    }

    if(retcode == ITK_ok) {
        sprintf(line, "Line$%d", query->table_counter++);
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"Line", TRUE, line));
    }

    if(retcode == ITK_ok) {
        sprintf(history, "History$%d", query->table_counter++);
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"Form", TRUE, history));
    }

    if(retcode == ITK_ok) {
        sprintf(s_history, "S_History$%d", query->table_counter++);
        ITK(POM_enquiry_create_class_alias(query->enq_id, (char*)"S_History", TRUE, s_history));
    }

    /* predecessor != NULL is used for kmm gen, do not remove it */
    if(retcode == ITK_ok && predecessor != NULL) {
        ITK(SBT_QUERY_attr_relation(query, predecessor, plink, "vorgaenger"));
    }

    if(retcode == ITK_ok) {
        ITK(SBT_QUERY_attr_relation(query, successor, plink, "nachfolger"));
    }

    if(retcode == ITK_ok) {
        ITK(SBT_QUERY_attr_relation(query, line, plink, "line"));
    }

    if(retcode == ITK_ok) {
        ITK(SBT_QUERY_attr_relation(query, history, line, "history"));
    }

    if(retcode == ITK_ok) {
        ITK(SBT_QUERY_attr_relation(query, s_history, history, "data_file"));
    }

    if(retcode == ITK_ok) {
        ITK(SBT_QUERY_set_string_attr(query, s_history, "type", POM_enquiry_equal, type));
    }

    if(retcode == ITK_ok) {
        ITK(SBT_QUERY_set_string_attr(query, history, "object_type", POM_enquiry_equal, "History"));
    }

    if(retcode == ITK_ok && fzg_prj_tag != NULLTAG) {
        ITK(SBT_QUERY_set_tag_attr(query, s_history, "Fzg_Projekt", POM_enquiry_equal, fzg_prj_tag));
    }

    LEAVE("sbt_query/SBT_QUERY_history_relation");
    return retcode;
}


int SBT_QUERY_assemble(sbt_query_t* query)
{
    int i;

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_assemble");
    retcode = ITK_ok;

    /* combine all expressions */
    if(query->expr_counter > 0)
    {
        char expr1_id[128];
        strcpy(expr1_id, "expr_0");

        for(i=1; retcode == ITK_ok && i<query->expr_counter; i++)
        {
            char expr2_id[128];
            char expr3_id[128];

            sprintf(expr2_id, "expr_%d", i);
            sprintf(expr3_id, "expr_%d", query->expr_counter+i);

            ITK(POM_enquiry_set_expr(query->enq_id, expr3_id, expr1_id, POM_enquiry_and, expr2_id));

            strcpy(expr1_id, expr3_id);
        }

        if (retcode == ITK_ok)
            ITK(POM_enquiry_set_where_expr(query->enq_id, expr1_id));
    }
    LEAVE("sbt_query/SBT_QUERY_assemble");
    return retcode;
}

int SBT_QUERY_execute(sbt_query_t* query)
{

    TRACE_ON(0);
    ENTER("sbt_query/SBT_QUERY_execute");
    retcode = ITK_ok;

    /* combine all expressions */
    ITK(SBT_QUERY_assemble(query));

    /* execute the query */
    if(retcode == ITK_ok)
        ITK(POM_enquiry_execute(query->enq_id, &(query->rows), &(query->cols), &(query->result)));

    LEAVE("sbt_query/SBT_QUERY_execute");
    return retcode;
}

static logical use_bind()
{
    if (getenv("PPEA_USE_CONST_QUERY_PARAMETERS"))
        return false;

    return true;
}

/* End file SBT_Query.c */
