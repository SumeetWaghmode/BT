/**
 * This file is to hold the implementation of the utility methods from Didier.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <ctype.h>

#include <tccore/grm.h>
#include <ae/ae.h>
#include <tc/emh.h>
#include <ss/ss_errors.h>

#include "sy_debug.h"

#include "SBT_Utilities_d.h"

#ifdef __cplusplus
}
#endif


/*
 * @brief Replace null char with 'nullSubstitute' before the end of the buffer. 
 * @param buffer The buffer
 * @param bufferSize The size of the buffer
 * @param nullSubstitute The character to replace the null with.
 */
void SBT_UTIL_embed_nulls 
  (
    char  *buffer,                                 /* <I/O> */
    size_t bufferSize,                             /* <I> */
    char   nullSubstitute                          /* <I> */
  )
{
    /*
     * Description :
     * Replace null char with 'nullSubstitute' before the end of the buffer. 
     */
    int i;

    i = (int)bufferSize;

    while ((buffer[i] == '\0') && (i >= 0)) i--;
    while (i >= 0)
    {
        buffer[i] = (buffer[i] == '\0') ? (nullSubstitute) : (buffer[i]);
        i--;
    }
} /* End SBT_UTIL_embed_nulls */


/*
 * @brief Removes heading and tailing blanks from inputString and return and
 *        allocated trimed string.
 * @param inputString The string to trim.
 */
char * SBT_UTIL_trim_blanks 
  (
    const char *inputString                        /* <I> */
  )
{
    /*
     * Description :
     * Removes heading and tailing blanks from inputString and return and
     * allocated trimed string.
     */

    char *trimedString = NULL,
         *pt = NULL;

    for (pt = (char *)inputString; (*pt == ' '); pt++);

    if (strlen(pt) > 0) 
    {
        trimedString = SBT_string_copy(pt);

        for (pt = trimedString + strlen(trimedString) - 1; (*pt == ' '); pt--);
        *(pt+1) = '\0';
    }

    return trimedString;

} /* End SBT_UTIL_trim_blanks */


/*
 * @brief Add a new item into a generic stack. if the given stack is NULL, 
 *        then return the first stack item.
 * CAUTION : the given itemto be stacked must have been previously 
 *           allocated.
 */
int SBT_UTIL_stack_new_item 
  (
    SBT_UTIL_stack_p_t *theStack,                           /* <I/O> */
    void         *userItem                                  /* <I> */
  )
{
    /*
     * Description :
     * Add a new item into a generic stack. if the given stack is NULL, 
     * then return the first stack item.
     * CAUTION : the given itemto be stacked must have been previously 
     *           allocated.
     */

    int               retcode = ITK_ok;
    generic_stack_p_t *stack = (generic_stack_p_t *)theStack;
    generic_stack_p_t newItemStack;

    /* Allocates the the item stack */
    newItemStack = (generic_stack_p_t)MEM_alloc(sizeof(generic_stack_t));

    if (newItemStack == NULL)
    {
        fprintf(stderr, 
               "Unable to allocate pointer newItemStack in %s at %d\n",
               __FILE__, __LINE__);
        retcode = -1;
    }
    else
    {
        newItemStack->userItem = userItem;
        newItemStack->next     = NULL;
    }

    if (retcode == ITK_ok)
    {
        if (*stack == NULL) /* Empty stack initialized by the first item */
            *stack = newItemStack;
        else                /* Append the new item at the end of the stack */
        {
            generic_stack_p_t pItem,
                              pLast;

            for (pLast = NULL, pItem = *stack; (pItem); 
                 pLast = pItem, pItem = pItem->next);
            
            if (pLast != NULL)
                pLast->next = newItemStack;
        }
    }

    return retcode;

} /* End SBT_UTIL_stack_new_item */


/*
 * @brief Return the number of item into the stack.
 * @param theStack The stack to count.
 */
int SBT_UTIL_stack_ask_count 
  (
    SBT_UTIL_stack_p_t *theStack                         /* <I> */
  )
{
    /*
     * Description :
     * Return the number of item into the stack.
     */

    int               count;
    generic_stack_p_t *stack = (generic_stack_p_t *)theStack;
    generic_stack_p_t pItem;

    count = 0;
    for (count = 0, pItem = *stack; (pItem); count++, pItem = pItem->next);

    return count;

} /* End SBT_UTIL_stack_ask_count */


/*
 * @brief Return the user item of the item stack at the given position. If the 
 *        item is not found, then return NULL.
 * CAUTION : the position start at 0.
 * @param theStack The stack where to get the object from.
 * @param position The position of the object to get.
 */
void * SBT_UTIL_stack_ask_user_item_at_pos 
  (
    SBT_UTIL_stack_p_t *theStack,                   /* <I> */
    int          position                           /* <I> */
  )
{
    /*
     * Description :
     * Return the user item of the item stack at the given position. If the 
     * item is not found, then return NULL.
     * CAUTION : the position start at 0.
     */

    int               i;
    void              *userItem = NULL;
    generic_stack_p_t *stack = (generic_stack_p_t *)theStack;
    generic_stack_p_t pItem;

    userItem = NULL;
    for (i = 0, pItem = *stack; (pItem); i++, pItem = pItem->next)
    {
        if (i == position)
        {
            userItem = pItem->userItem;
            break;
        }
    }

    return userItem;

} /* End SBT_UTIL_stack_ask_user_item_at_pos */


/*
 *@brief This function delete the stack and all its items. If a delete
 *       function is provide to delete user data stacked, then this fonction
 *       will be applied.
 * @param theStack The stack to be deleted.
 * @param 
 */
void SBT_UTIL_stack_delete 
  (
    SBT_UTIL_stack_p_t *theStack,                          /* <I> */
    void         (*deleteUserItem)(void *)                  /* <I> */
  )
{
    /*
     * Description :
     * This function delete the stack and all its items. If a delete
     * function is provide to delete user data stacked, then this fonction
     * will be applied.
     */

    generic_stack_p_t *stack = (generic_stack_p_t *)theStack;
    generic_stack_p_t pItem,
                      toBeDeleted;

    pItem = *stack;
    while (pItem != NULL)
    {
        toBeDeleted = pItem;
        pItem = pItem->next;

        if ((toBeDeleted->userItem != NULL) && (deleteUserItem != NULL))
        {
            deleteUserItem(toBeDeleted->userItem);
        }

        MEM_free(toBeDeleted);
    }
} /* End SBT_UTIL_stack_delete */


/**
 * @brief 
 * Add a new item in an array.
 * If the array is NULL before adding the first item, then array is allocated to theItemExtent * theItemSize.
 * if the array reach its memory limit, the number of item is a multiple of the initial extend. In this case,
 * space for 'theItemExtent' more item is added to the array memory using the function SM_realloc_persistent.
 * This function returns SS_NOMEM when the momory cannot be allocated.
 * WARNING : theArray must be set to NULL and theNumberOfItems must be set to 0 before calling this function.
 */
int SBT_UTIL_add_item_in_array 
  (
    void*   theNewItem,                              /* I */
    size_t  theItemSize,                             /* I */
    int     theItemExtent,                           /* I */
    int*    theNumberOfItems,                        /* O */
    void*** theArray                                 /* OF MEM_free */
  )
{
    int    i = 0;
    int    retcode = ITK_ok;
    int    count = 0;
    void **list = NULL;

    count = *theNumberOfItems;
    list  = *theArray;

    /* First allocation of the array */
    if (list == NULL) 
    {
        int local_size = (int)theItemSize;
        if ((list = (void **)MEM_alloc(theItemExtent * local_size)) == NULL) {
            retcode = SS_NOMEM;
        }
        else {
            for (i = 0; (i < theItemExtent); i++)  {
                list[i] = (void *)NULL;
            }
        }
    }
    else { /* Array already exits -> check if we are not running out of space */
        if ((count % theItemExtent) == 0) { /* The actual number of item is a multiple of the item extent */
            size_t newSize = (count + theItemExtent) * theItemSize; /* Space for theItemExtent more items */
            if ((list = (void **)MEM_realloc(list, (int)newSize)) == NULL) {
                retcode = SS_NOMEM;
            }
        }
    }

    /* Add the new element in the array */
    if (retcode == ITK_ok)
    {
        list[count] = theNewItem;
        count++;

        *theNumberOfItems = count;
        *theArray         = list;
    }

    return retcode;

} /* End SBT_UTIL_add_item_in_array */


/**
 * @breif Free an array of string.
 * @param numberOfStrings [in] The number of strings in the array.
 * @param stringArray     [in] The string array to free.
 */
void SBT_UTIL_free_string_array 
  (
    int numberOfStrings,                             /* I */
    char **stringArray                               /* I */
  )
{

    if (stringArray != NULL) 
    {
        int i;

        for (i = 0; (i < numberOfStrings); i++) 
        {
            if (stringArray[i] != NULL) {
                MEM_free(stringArray[i]);
            }
        }
    }

    return;
}

/**
 * @brief
 * Replace to SM_string_copy/SM_string_copy_persistent based on MEM_alloc.
 * @note: use MEM_free to free the copied string.
 *
 * @param[in] inputString  Pointer to the string to be copied
 * @return    A pointer to the copied string or NULL
 */
extern char* SBT_string_copy (
    const char* inputString                          /* (I) */
    )
{
    char* result = NULL;

    if (inputString != (const char*)NULL) {
        if ((result = (char*)MEM_alloc((strlen(inputString)+1) * sizeof(char))) != (char*)NULL) {
            strcpy(result, inputString);
        }
    }

    return result;
}     

/**
 * @brief
 * Replacement to SM_string_append base of MEM_alloc.
 * @note: Use MEM_free to free the appended memory
 *
 * @param[in] str        Pointer to the current string
 * @param[in] appendage  Pointer to the string to append
 * @return    A pointer to the appended string or NULL
 */
extern char* SBT_string_append (
    char*       str,                                 /* (I) */
    const char* appendage                            /* (I) */
    )
{
    char* result = NULL;

    if ((str != NULL) && (appendage != NULL))
    {
        size_t len = ((strlen(str)+1) + (strlen(appendage)+1)) * sizeof(char);

        if ((result = (char*)MEM_alloc(len)) != NULL) {
            memset(result, 0, len);
            strcpy(result, str);
            strcat(result, appendage);
        }
    }

    return result;
}     

/* End file SBT_Utilities_d.c */
