/**
 * This file holds the functions used for the query module.
 */

#ifndef _1_sbt_query_h_H
#define _1_sbt_query_h_H

#include <tc/iman.h>

/** 
 * @brief these are some macro definitions which help
 * to save time in creating iman queries.
 * Deprecated: look at the query components described at the
 * end of this header.
 *
 * Use them as follows:
 * 1. define as many char-pointers as needed for joins,
 *    attributes and expressions (do *NOT* forget to delete
 *    them with SM_free)
 * char* val1_id = NULL;
 * char* attr1_id = NULL;
 * char* join1_id = NULL;
 * int rows, cols;
 * void*** report;
 *
 * 2. define your query with (only one is allowed per block)
 *   DEF_QUERY("my_query");
 *
 * 3. create your query as usual
 * POM_enquiry_create(enq_id);
 *
 * 4. you can use every function from POM_enquiry: use the
 *    variable enq_id as the query identifier e.g.
 * RETURN_ERROR(POM_enquiry_create_class_alias(enq_id, (char*)"ImanRelation", TRUE, (char*)"ImanRelation2"));
 *
 * 5. UNIQUE_EXPR creates a new unique identifier e.g.
 * UNIQUE_EXPR(val1_id);
 *
 * 6. set your attributes with SET_ATTR
 * SET_ATTR(attr1_id, "S_SG_Member", "Diagnoseadresse", POM_enquiry_equal, val1_id);
 *
 * 7. set your join conditions with SET_JOIN
 * SET_JOIN(join1_id, "S_SG_Member", "puid", POM_enquiry_equal, "Form", "data_file");
 *
 * 8. concat your expressions with SET_EXPR
 * SET_EXPR(expr1_id, attr1_id, POM_enquiry_and, join1_id);
 *
 * 9. execute your query and inspect the return values as usual
 * POM_enquiry_set_where_expr(enq_id, expr1_id);
 * POM_enquiry_execute(enq_id, &rows, &cols, &report);
 *
 * 10. free all allocated ressources
 * SM_free(val1_id);
 * SM_free(attr1_id);
 * SM_free(join1_id);
 * SM_free(report);
 */

#define DEF_QUERY(query) \
    const char* enq_id = query; \
    const int __enqLen__ = strlen(enq_id); \
    int __exprCounter__ = 0

#define UNIQUE_EXPR(exprId) \
    exprId = (char*)SM_alloc_persistent((__enqLen__ + 12) * sizeof(char)); \
    sprintf(exprId, "%s_expr_%.5d", enq_id, __exprCounter__++)

#define SET_JOIN(joinId, cls1, fld1, oper, cls2, fld2) \
if(retcode == ITK_ok) \
{\
    UNIQUE_EXPR(joinId); \
    RETURN_ERROR(POM_enquiry_set_join_expr(enq_id, joinId, (char*)cls1, (char*)fld1, oper, (char*)cls2, (char*)fld2)); \
}

#define SET_ATTR(attrId, cls, fld, oper, val) \
if(retcode == ITK_ok) \
{ \
    UNIQUE_EXPR(attrId); \
    RETURN_ERROR(POM_enquiry_set_attr_expr(enq_id, attrId, (char*)cls, (char*)fld, oper, val)); \
}

#define SET_EXPR(exprNew, expr1, oper, expr2) \
if(retcode == ITK_ok) \
{ \
    UNIQUE_EXPR(exprNew); \
    RETURN_ERROR(POM_enquiry_set_expr(enq_id, exprNew, expr1, oper, expr2)); \
}

/** @brief The string representing the puid column */
static const char *SBT_QUERY_attr_puid = "puid";

/**
* The following query components help to build complex queries
* by providing simple functions to create relations between tables.
* The basic idea is to generate unique expressions and table
* identifiers automatically. They are stored in the struct sbt_query_t,
* which is the first parameter in all functions of this module.
* These autogenerated ids have one drawback: you can not use "or" in
* your queries, all expressions are connected by "and".
*
* You can call other POM-functions by using the enq_id of the structure
* eg. for defining aliases etc.
*
* Short description:
* 1. create a query with SBT_QUERY_init_query
* 2. define your select clause by calling ITK functions
* 3. define aliases if necessary by calling ITK functions
* 4. set parameters by calling SBT_QUERY_set_string_attr, SBT_QUERY_set_tag_attr, etc.
* 5. define the relationship by calling SBT_QUERY_grm_relation, SBT_QUERY_bom_relation, etc.
* 6. execute the query with SBT_QUERY_execute
* 7. loop through the result set by using the "result", "rows" and "cols" from "sbt_query_t"
* 8. free allocated memory with SBT_QUERY_free_query
*/

typedef struct
{
    char enq_id[128];   /* unique identifier, can be used for other POM functions */
    int expr_counter;   /* internal use ONLY */
    int val_counter;    /* internal use ONLY */
    int table_counter;  /* internal use ONLY */
    int rows;           /* row counter of the result */
    int cols;           /* col counter of the result */
    void ***result;     /* query result */
} sbt_query_t;

/**
* @brief inits a new query struct
* @param query [in] the query to initialize
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_init_query(sbt_query_t *query);

/**
* @brief inits a new query struct as sub query. The query must be freed
*        using SBT_QUERY_free_sub_query
* @param query [in] the parent query
* @param query [in] the sub query to initialize
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_init_sub_query(sbt_query_t *query,
                             sbt_query_t *sub_query);

/**
* @brief frees allocated memory
* @param query [in] the query to be freed
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_free_query(sbt_query_t *query);


/**
* @brief sets a string (one) attribute for a query
* @param query     [in] the query
* @param table     [in] a table name or an alias
* @param attr      [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value     [in] the searched value (pointer must be valid until SBT_QUERY_execute)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_string_attr(sbt_query_t *query,
                              const char *table,
                              const char *attr,
                              int operator_,
                              const char *value);

/**
* @brief sets a string (many) attribute for a query
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_in (see ITK help)
* @param count [in] size of array
* @param value [in] the searched values (pointer must be valid until SBT_QUERY_execute)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_string_attrs(sbt_query_t *query,
                               const char *table,
                               const char *attr,
                               int operator_,
                               int count,
                               const char **values);

/**
* @brief Sets a string attribute for a query. The comparison between the value and
*        the database column is done by a like operator. The comparisin is case
*        insensitive.
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr  [in] the name of an attribute
* @param value [in] the searched value (pointer must be valid until SBT_QUERY_execute)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_like_string_attr(sbt_query_t *query,
                                   const char *table,
                                   const char *attr,
                                   const char *value);

/**
* @brief sets an expression property e.g. POM_case_insensitive.
*        Must be called right after SBT_QUERY_set_string_attr
* @param query [in] the query
* @param property_ [in] eg. POM_case_insensitive (see ITK help)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_expr_proprety(sbt_query_t *query,
                                int property_);



/**
* @brief sets a tag (one) attribute for a query
* @param query     [in] the query
* @param table     [in] a table name or an alias
* @param attr      [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value     [in] the searched value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_tag_attr(sbt_query_t *query,
                           const char *table,
                           const char *attr,
                           int operator_,
                           tag_t value);

/**
* @brief sets a tag (many) attribute for a query
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_in (see ITK help)
* @param count [in] size of array
* @param value [in] the searched values
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_tag_attrs(sbt_query_t* query,
                            const char* table,
                            const char* attr,
                            int operator_,
                            int count,
                            tag_t* values);

/**
* @brief sets a int (one) attribute for a query
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value [in] the searched value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_int_attr(sbt_query_t* query,
                           const char* table,
                           const char* attr,
                           int operator_,
                           int value);

/**
* @brief sets a logical (one) attribute for a query
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value [in] the searched value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_logical_attr(sbt_query_t* query,
                               const char* table,
                               const char* attr,
                               int operator_,
                               logical value);

/**
* @brief sets a logical (many) attribute for a query
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_in (see ITK help)
* @param count [in] size of array
* @param values [in] the searched value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_logical_attrs(sbt_query_t* query,
                                const char* table,
                                const char* attr,
                                int operator_,
                                int count,
                                logical* values);


/**
* @brief sets an attribute expression
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value [in] a value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_date_attr(sbt_query_t* query,
                            const char* table,
                            const char* attr,
                            int operator_,
                            date_t value);

/**
* @brief sets a date attribute expression
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value [in] a value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_date_attrs(sbt_query_t* query,
                             const char* table,
                             const char* attr,
                             int operator_,
                             int count,
                             date_t* values);


/**
* @brief sets an attribute expression
* @param query [in] the query
* @param table [in] a table name or an alias
* @param attr [in] the name of an attribute
* @param operator_ [in] eg. POM_enquiry_equal (see ITK help)
* @param value [in] a value
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_set_attr_expr(sbt_query_t* query,
                            const char* table,
                            const char* attr,
                            int operator_,
                            const char* value);
                             
/**
* @brief joins to tables with a grm relation
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent (primary)
* @param child [in] a table name or an alias of the child (secondary)
* @param type_name [in] name of the relation (NULL is allowed, all relations are valid)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_grm_relation(sbt_query_t* query,
                           const char* parent,
                           const char* child,
                           const char* type_name);

/**
* @brief joins to tables with a grm relation using user_data
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent (primary)
* @param child [in] a table name or an alias of the child (secondary)
* @param user_data [in] a table name or an alias of the user_data
* @param type_name [in] name of the relation (NULL is allowed, all relations are valid)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_grm_relation_ex(sbt_query_t* query,
                              const char* parent,
                              const char* child,
                              const char* user_data,
                              const char* type_name);

/**
* @brief joins to tables with a bom relation
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param child [in] a table name or an alias of the child
* @param view_type_name [in] name of the relation (NULL is allowed, all boms are valid)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_bom_relation(sbt_query_t* query,
                           const char* parent,
                           const char* child,
                           const char* view_type_name);

/**
* @brief joins to tables with a attribut relation: parent.puid = child.attr_name
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param child [in] a table name or an alias of the child
* @param attr_name [in] attribute in child table
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_attr_relation(sbt_query_t* query,
                            const char* parent,
                            const char* child,
                            const char* attr_name);

/**
* @brief joins to tables with a attribut relation: parent.parent_attr = child.child_attr
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param parent_name [in] attribute in parent table
* @param child [in] a table name or an alias of the child
* @param child_attr [in] attribute in child table
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_attr_relation_ex(sbt_query_t* query,
                               const char* parent,
                               const char* parent_attr,
                               const char* child,
                               const char* child_attr);


/**
* @brief Joins two tables with a attribut relation: parent.parent_attr = child.child_attr.
*        Instead of the POM_enquiry_equal, the POM_enquiry_like expression is used
*        and the comparison is case insensitive.
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param parent_name [in] attribute in parent table
* @param child [in] a table name or an alias of the child
* @param child_attr [in] attribute in child table
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_attr_relation_like(sbt_query_t* query,
                                 const char* parent,
                                 const char* parent_attr,
                                 const char* child,
                                 const char* child_attr);

/**
* @brief joins to tables with a masterform relation
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param child [in] a table name or an alias of the child (storage class)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_masterform_relation(sbt_query_t* query,
                                  const char* parent,
                                  const char* child);

/**
* @brief joins to tables with a form relation
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param child [in] a table name or an alias of the child (storage class)
* @param rel_type [in] name of relation type (e.g. Form_assignment)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_form_relation(sbt_query_t* query,
                            const char* parent,
                            const char* child,
                            const char* rel_type);

/**
* @brief joins a tables with a sachnummer containing attribute to a TaisPart_Rev
*
* @param query [in] the query
* @param parent [in] a table name or an alias of the parent
* @param parent_name [in] attribute in parent table
* @param child [in] a table name or an alias of the child (SR_Proxy)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_sachno_relation(sbt_query_t* query,
                              const char* parent,
                              const char* parent_attr,
                              const char* child);

/**
* @brief joins to tables with a history relation (plink)
* @param query [in] the query
* @param parent [in] a table name or an alias of the predecessor
* @param child [in] a table name or an alias of the successor
* @param type [in] a type of the history (NULL is NOT allowed)
* @param fzg_prj_tag [in] fzg_prj of the history (NULLTAG is allowed)
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_history_relation(sbt_query_t* query,
                               const char* predecessor,
                               const char* successor,
                               const char* type,
                               tag_t fzg_prj_tag);

/**
 * @brief combine all expressions of the query
 *
 * This method is called by SBT_QUERY_execute. Its pubclic interface 
 * should be used for subqueries.
 *
 * @param query [in] the query
 * @return ITK_ok on successfull execution, an error code otherwise
 */
int SBT_QUERY_assemble(sbt_query_t* query);

/**
* @brief executes the query. After a successfull execution the elements
* "result", "cols", "rows" are set and valid
* @param query [in] the query
* @return ITK_ok on successfull execution, an error code otherwise
*/
int SBT_QUERY_execute(sbt_query_t* query);

#endif

/* End file SBT_Query.h */