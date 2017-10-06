/**
 * This file is to hold the implementation of the methods used to deal with files.
 */
/*
	08-May-2009		Hartmuth Petzold		In SBT_File_file_to_string_array(...) replaced buggy SBT_UTIL_embed_nulls(...) 
											which is used nowhere else by direct code 
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <tc/emh.h>
#include <sa/imanfile.h>

#include "sy_debug.h"

#include "SBT_Utilities.h"
#include "SBT_Utilities_d.h"

#include "SBT_File.h"

#ifdef __cplusplus
}
#endif

/*
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
  )
{

    char *unique_file_name = 0;

    const char* theFunction = "SBT_File/SBT_File_tag_to_string_array";
    TRACE_ON(0);
    ENTER(theFunction);

    ITK(SBT_UTIL_create_unique_filename(NULL, &unique_file_name));

    if (retcode == ITK_ok && file_tag != NULLTAG)  
    {
        ITK(IMF_export_file(file_tag,
                                     unique_file_name));
    }

    if (retcode == ITK_ok)
    {
        ITK(SBT_File_file_to_string_array(unique_file_name,
                                                   stringCount,
                                                   strings));
    }


TRACE1("SBT: unique_file_name = %s\n",unique_file_name);
for(int i=0;i<*stringCount;i++) {TRACE2("SBT: strings[%d] = %s\n",i,(*strings)[i]);}



    // clean up
    MEM_free(unique_file_name);

    LEAVE(theFunction);
    return (retcode);

} /* End SBT_File_tag_to_string_array */


/*
 * @brief This function reads the contense of a file and returns it in a
 *        string array.
 * @param filePath    The path where the file resides.
 * @param stringCount The number of array entries.
 * @param strings     The string array.
 */
int SBT_File_file_to_string_array 
  (
    const char *filePath,                          /* <I> */
    int        *stringCount,                       /* <O> */
    char       ***strings                          /* <O> US_delete_string_array(count,list) */
  )
{
    /*
     * Description :
     * Load the given file into a string array.
     */

    int  retcode = ITK_ok;
    FILE *fp = NULL;

    // preset return variables 
    *stringCount = 0;
    *strings = NULL;


    /* Open the input file */
    if ((fp = fopen(filePath, "r")) != NULL)
    {
        int          newStringCount = 0;

        char         line[BUFSIZ];
        size_t       lineSize = sizeof(line);
        SBT_UTIL_stack_p_t fileStack = NULL;

        /* Read the file */

        memset(line, 0, lineSize);
        while ((fgets(line, (int) lineSize, fp) != NULL) && (retcode == ITK_ok))
        {
            char *newString,
                 *pt;


            /* Replace null char with blanks before the end of the buffer. */
//hfhp 08-May-2009 preplaced buggy function which is used nowhere else by direct code
//			SBT_UTIL_embed_nulls(line, lineSize, ' ');
			{
				size_t		idx = 0	;
				for ( idx = 0 ; idx < lineSize - 1 ; idx ++ )
				{
					if ( line[idx] == '\0' ) line[idx] = ' ' ;
				}
			}


            newString = NULL;
            pt = strchr(line, '\n');

            /* Trim the enf of line */
            if (pt != NULL) 
                *pt = '\0';    

            /* Trim heading and trailing blanks */
            newString = SBT_UTIL_trim_blanks(line);

			TRACE1("SBT: newString  =  [%s]\n",newString);

            /* Stack in the current line */
//hfhp 08-May-2009 replace nullstring with blank 
			if ( newString == NULL )
			{
				newString = (char*)MEM_alloc(2*sizeof(char));
				strcpy ( newString , " " ) ;
			}
            ITK(SBT_UTIL_stack_new_item(&fileStack, newString));
			memset(line, 0, lineSize);
       }

        fclose(fp);

        /* Flush the stack into a string array */
        if ((newStringCount = SBT_UTIL_stack_ask_count(&fileStack)) > 0)
        {
            char **strArray = (char **)MEM_alloc(newStringCount * sizeof(char *));

            if (strArray == NULL)
            {
                fprintf(stderr, "Memory Error while allocating %s at %d in %s\n",
                                "strArray", __LINE__, __FILE__);
                retcode = -1;
            }
            else 
            {
                int i;

                for (i = 0; (i < newStringCount); i++)
                {
                    strArray[i] = 
                        (char *)SBT_UTIL_stack_ask_user_item_at_pos(&fileStack, i);
                }

                *stringCount = newStringCount;
                *strings = strArray;
            }
        }

        if (fileStack != NULL) {
            SBT_UTIL_stack_delete(&fileStack, NULL);
        }
    }
    else {
        fprintf(stderr, "Unable to open file '%s'\n", filePath);
        retcode = -1;
    }







    return retcode;

} /* End SBT_File_file_to_string_array */


/* End file SBT_File.c */
