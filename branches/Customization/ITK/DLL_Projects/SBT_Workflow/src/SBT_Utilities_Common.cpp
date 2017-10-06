/**
 * This file holds the implementation of common utilities,
 * not directly linked to TCE, but needed for progressing.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "SBT_Utilities_Common.h"

#ifdef __cplusplus
}
#endif


/**
 * @brief determines wheather this is an Ascii file (cope with german letters!)
 * @return true if it is an Ascii file.
 */
int SBT_UTIL_is_ascii
  (
    char c
  )
{
    return (isascii(c) || 
            (c == 'ä') || (c == 'Ä') || (c == 'ö') || (c == 'Ö') ||
            (c == 'ü') || (c == 'Ü') || (c == 'ß') || 
            (c == 196) || (c == 214) || (c == 220) || (c == 223) || 
            (c == 228) || (c == 246) || (c == 252) || 
            (c == EOF));

} /* End SBT_UTIL_is_ascii */

/**
 * @brief Return the basename of a file given its full path.
 */
const char* SBT_UTIL_basename 
  (
    const char*  filePath                            /* <I> */
  )
{
    const char *basename = strrchr(filePath, SBT_UTIL_DIRECTORY_SEPERATOR);

    if (basename == NULL) {
        basename = filePath;
    }
    else {
        basename++;
    }

    return basename;

} /* End SBT_UTIL_basename */


/**
 * @brief removes leading and tailing white chars from string 
 */
const char *SBT_UTIL_trim
  (
    char *str                           /* I */
  )
{
    int len = 0;
    int i   = 0;

    /* precondition check */
    if (!str)
    {
        return (char*) "";
    }

    /* leading white chars */
    len = (int)strlen(str);

    while ((*str == ' ' || *str == '\t' || *str == '\n') && len > 0)
    {
        str++;
        len--;
    }

    /* tailing white chars */
    i = len - 1;
    
    while (i > 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
    {
        str[i--] = '\0';
    }

    return str;

} /* End SBT_UTIL_trim */


/* End file SBT_Utilities_Common.c */
