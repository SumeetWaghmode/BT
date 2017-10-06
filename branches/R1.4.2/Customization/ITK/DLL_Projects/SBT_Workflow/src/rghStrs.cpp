#ifdef __cplusplus
extern "C" {
#endif


#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//#include "ugs_util_macros.h"
//#include "ugs_util_string.h"
#include <tc/iman.h>
#include "sy_debug.h"

#include "rghStrs.h"


/**
 * @brief
 * Processes a string and returns an array of substrings.
 * The processed input string has the format: "str1 cSep str2 cSep ... strN".
 * The returned string array has the format: { str1, str2, ..., strN }
 * The substrings within the input string are separated by space characters and
 * the delimiter character cSep that is specified as a parameter.
 *
 * @param[in]   str   the string to be processed    
 * @param[out]  nStrs the number of the substrings in the allocated string array
 * @param[in]   cSep  the delimiter character that separates the substrings
 *
 * @return  a pointer to the allocated string array. The whole string array is
 *          allocated in memory and contains new copies of the substrings.
 *          The string array is released by the function strStrs_free.
 */
char **rghStrs_newStrSep (
    char *str,  // (I)
    int *nStrs, // (I)
    char cSep)  // (I)
  {
  char **ret = NULL;
  *nStrs = 0;
  if (str == NULL) { return NULL; }
  int i;

  for (i = 0; str [i] != 0; i ++)
    {
//    printf ("%d : %d : %c\n", i, (*nStrs), str [i]);
    if (isspace (str [i]) || str [i] == cSep) { continue; }
    (*nStrs) ++;
    for (; str [i] != 0; i ++)
      {
//      printf ("\t%d : %d : %c\n", i, (*nStrs), str [i]);
      if (isspace (str [i]) || str [i] == cSep) { break; }
      }
	if(str[i]=='\0')break;
    }

  if (nStrs == 0) { return ret; }
  ret = (char **) MEM_alloc ((*nStrs) * sizeof (char *));
  int j = 0, tokLen = 0;
  char *sTok = NULL;
  for (i = 0; str [i] != 0; i ++)
    {
    if (isspace (str [i]) || str [i] == cSep) { continue; }
    tokLen = 0;
    sTok = &(str [i]);
    for (;
         str [i] != 0 && isspace (str [i]) == 0 && str [i] != cSep;
         i ++)
      { tokLen ++; }
    ret [j] = (char *) MEM_alloc ((tokLen + 1) * sizeof (char));
    strncpy (ret [j], sTok, tokLen);
    (ret [j]) [tokLen] = 0;
    j ++;
	if(str[i]=='\0')break;
    }
  return ret;
  }



/**
 * @brief
 * Releases the string array in memory.
 *
 * @param[in]   strs   the string array to be released
 * @param[in]   nStrs the number of the strings in the string array
 */
void rghStrs_free (
    char **strs,  // (I)
    int nStrs)    // (I)
  {
  if (strs == NULL) { return; }
  int i;
  for (i = 0; i < nStrs; i ++)
    { if (strs [i] != NULL) { MEM_free (strs [i]);} }
  MEM_free (strs);
  }


/**
 * @brief
 * Searches a string array for a given string.
 *
 * @param[in]   strs  the string array to be searched
 * @param[in]   nStrs the number of the strings in the string array
 * @param[in]   str   the string to be searched for in the string array
 *
 * @return  true, if the string array contains a string that is equal to str.
            false, otherwise.
 */
logical rghStrs_bStrElem (
    char **strs,  // (I)
    int nStrs,    // (I)
    char *str)    // (I)
  {
  if (strs == NULL) { return 0; }
  int i;
  for (i = 0; i < nStrs; i ++)
    { if (strs [i] != NULL && strcmp (strs [i], str) == 0) { return 1; } }
  return 0;
  }


/**
 * @brief
 * Prints the string array for debug purposes. Uses UGS_UTIL_trace().
 *
 * @param[in]   strs  the string array to be printed
 * @param[in]   n     the number of the strings in the string array
 * @param[in]   id    an optional string for informational purposes
 */
void rghStrs_trace (
    char **strs,  // (I)
    int n,        // (I)
    char *id)     // (I)
  {
  int i;
//  UGS_UTIL_trace ("n%s = %d\n", id, n);
  TRACE2 ("n%s = %d\n", id, n);
  if (strs == NULL) { return; }
  for (i = 0; i < n; i ++)
//    { UGS_UTIL_trace ("%s [%d] = \"%s\"\n", id, i, strs [i]); }
    { TRACE3 ("%s [%d] = \"%s\"\n", id, i, strs [i]); }
  }

#ifdef __cplusplus
}
#endif