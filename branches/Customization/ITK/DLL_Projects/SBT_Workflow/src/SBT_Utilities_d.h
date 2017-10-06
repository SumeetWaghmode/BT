/**
 * This file is to hold general utility methods.
 */

#ifndef _SBT_UTILITIES_D_H
#define _SBT_UTILITIES_D_H

#include <tc/iman.h>

/** @brief Abstract type for a string stack */
typedef void *SBT_UTIL_stack_p_t;

/** @brief General purpose stack */
typedef struct generic_stack_s {
    void                   *userItem;
    struct generic_stack_s *next;
} generic_stack_t, *generic_stack_p_t;

/**
 * @brief Removes heading and tailing blanks from inputString and return and
 *        allocated trimed string.
 * @param inputString The string to trim.
 */
char * SBT_UTIL_trim_blanks 
  (
    const char *inputString                        /* <I> */
  );

/**
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
  );

/**
 * @brief Add a new item into a generic stack. if the given stack is NULL, 
 *        then return the first stack item.
 * CAUTION : the given itemto be stacked must have been previously 
 *           allocated.
 */
int SBT_UTIL_stack_new_item 
  (
    SBT_UTIL_stack_p_t *theStack,                          /* <I/O> */
    void         *userItem                                  /* <I> */
  );

/**
 * @brief Return the number of item into the stack.
 * @param theStack The stack to count.
 */
int SBT_UTIL_stack_ask_count 
  (
    SBT_UTIL_stack_p_t *theStack                         /* <I> */
  );

/**
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
  );

/**
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
  );

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
    void*** theArray                                 /* OF SM_free */
  );

/**
 * @breif Free an array of string.
 * @param numberOfStrings [in] The number of strings in the array.
 * @param stringArray     [in] The string array to free.
 */
void SBT_UTIL_free_string_array 
  (
    int numberOfStrings,                             /* I */
    char **stringArray                               /* I */
  );

#endif

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
    );

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
    );

/* End file SBT_Utilities_d.h */
