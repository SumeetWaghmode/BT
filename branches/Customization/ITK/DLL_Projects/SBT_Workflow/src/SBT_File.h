/**
 * This file is to hold the methods used to deal with files.
 */

#ifndef _SBT_FILE_H
#define _SBT_FILE_H

#include <tc/iman.h>

/**
 * @brief This function reads the contense of a file tag and returns it in a
 *        string array.
 * @param file_tag    The file tag to read.
 * @param stringCount The number of array entries.
 * @param strings     The string array.
 */
int SBT_File_tag_to_string_array 
  (
    tag_t file_tag,                          /* <I> */
    int   *stringCount,                      /* <O> */
    char  ***strings                         /* <O> US_delete_string_array(count,list) */
  );

/**
 * @brief This function reads the contense of a file tag and returns it in a
 *        string array.
 * @param filePath    The path where the file resides.
 * @param stringCount The number of array entries.
 * @param strings     The string array.
 */
int SBT_File_file_to_string_array 
  (
    const char *filePath,                          /* <I> */
    int        *stringCount,                       /* <O> */
    char       ***strings                          /* <O> DELETE_STR... */
  );

#endif

/* End file SBT_File.h */
