/**
 * This file holds the declarations of common utilities,
 * not directly linked to TCE, but needed for progressing.
 */

#ifdef UNX
static const char SBT_UTIL_DIRECTORY_SEPERATOR = '/';
#else
static const char SBT_UTIL_DIRECTORY_SEPERATOR = '\\';
#endif

/**
 * @brief determines wheather this is an Ascii file (cope with german letters!)
 * @return true if it is an Ascii file.
 */
int SBT_UTIL_is_ascii
  (
    char c
  );

/**
 * @brief Return the basename of a file given its full path.
 */
const char* SBT_UTIL_basename 
  (
    const char*  filePath                            /* <I> */
  );

/**
 * @brief removes leading and tailing white chars from string 
 */
const char *SBT_UTIL_trim
  (
    char *str                           /* I */
  );

/* End file SBT_Utilities_Common.h */
