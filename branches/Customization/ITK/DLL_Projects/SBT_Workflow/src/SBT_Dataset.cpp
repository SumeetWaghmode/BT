/**
 * This file is to hold the implementation of the utility methods used to deal with datasets.
 */

#ifdef __cplusplus
extern "C" {
#endif

//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <io.h>

#include <tccore/grm.h>
#include <ae/ae.h>
#include <sa/imanfile.h>
#include <tc/emh.h>
#include <tccore/aom.h>
#include <user_exits/user_exits.h>

//#include <eds_itktool.h>
#include "sy_debug.h"

#include "SBT_Utilities.h"
#include "SBT_Utilities_d.h"
#include "SBT_Utilities_Common.h"

#include "SBT_Dataset.h"

#ifdef __cplusplus
}
#endif

/*
 * \brief Asks file of dataset
 * \param dataset_tag (I) tag of dataset
 * \param dataset_information (I) dataset information
 * \param type_index (I) type
 * \param file_tag (O) tag of file
 * \return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_Dataset_ask_assigned_file_tag(tag_t dataset_tag,
                                      SBT_named_ref_t *dataset_information,
                                      int type_index,
                                      tag_t *file_tag)
{
    int iRetcode = ITK_ok;

    int file_count = 0;
    tag_t *file_objects = NULL;

    const char* theFunction = "SBT_Dataset/SBT_Dataset_ask_assigned_file_tag";

    ENTER(theFunction);

    /* initialize result parameters */

    *file_tag = NULLTAG;

    /* look for the named reference */

    TRACE1("Looking for named reference '%s' in dataset...\n", dataset_information[type_index].named_reference);
    ITK(AE_ask_all_dataset_named_refs(dataset_tag,
                                                     dataset_information[type_index].named_reference,
                                                     &file_count,
                                                     &file_objects));

    TRACE1("Found %d named references in dataset\n", file_count);

    /* search for files with the required extension */

    for (int i = 0; iRetcode == ITK_ok && i < file_count; i++) {

        char file_name[IMF_filename_size_c + 1];

        /* compare the file extension */

        ITK(IMF_ask_original_file_name(file_objects[i], file_name));

        if (iRetcode == ITK_ok)
        {
            int n = (int) (strlen(file_name) - strlen(dataset_information[type_index].file_name_ext));
            char* ptr = file_name + n;

            TRACE2("File %d: file_name: %s\n", i, file_name);
            TRACE2("Comparing %s and %s\n", ptr?ptr:"NULL", dataset_information[type_index].file_name_ext);
            //if (strcasecmp(ptr, dataset_information[type_index].file_name_ext) == 0) {
            if (stricmp(ptr, dataset_information[type_index].file_name_ext) == 0)
            {
                if (*file_tag == NULLTAG)
                {
                    *file_tag = file_objects[i];
                }
                else
                {
                    char *id_string = NULL;
                    char file_name1[IMF_filename_size_c + 1];

                    /* already an other file found */

                    WSOM_ask_object_id_string(dataset_tag,
                                              &id_string);

                    IMF_ask_original_file_name(*file_tag,
                                               file_name1);

                    EMH_clear_errors();

                    SBT_EMH_store_error("Die Dateien koennen nicht maschinell verarbeitet werden.", NULL);
                    SBT_EMH_store_error("Datei 2: ", file_name1, NULL);
                    SBT_EMH_store_error("Datei 1: ", file_name, NULL);
                    SBT_EMH_store_error("Dataset: ", id_string, NULL);
                    SBT_EMH_store_error("gefunden, auf die die geforderten Kriterien zutreffen.", NULL);
                    SBT_EMH_store_error("An dem Dataset ", id_string, " wurden mehrere benannte Referenzen", NULL);

                    iRetcode = SBT_MULTIPLE_FILES;

                    if (id_string)
                        MEM_free(id_string);
                }
            } /* if strcasecmp */
        } /* if retcode */
    } /* for i */

    if (file_objects)
        MEM_free(file_objects);

    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_Dataset_ask_assigned_file_tag */


/*
 * @brief Import the given file into the current iman volume and 
 *        attachs it to the given dataset.
 * @param datasetTag    [in] The dataset where to add the file to.
 * @param filePath      [in] The file path.
 * @param referenceName [in] The name of the named reference 
 *                           (used e.g. by AE_add_dataset_named_ref).
 * cf. PPEA_UTIL_add_dataset_file.
 */
int SBT_Dataset_add_dataset_file 
  (
    tag_t       datasetTag,                          /* <I> */
    const char* filePath,                            /* <I> */
    const char* referenceName                        /* <I> */
  )
{
    int           iRetcode = ITK_ok;
    int           fd = 0;
    unsigned char rbuf[256];
    char*         newImanFileName = NULL;
    size_t        rbufSize = sizeof(rbuf);
    int           fileType = SS_TEXT;
    tag_t         fileTag  = NULLTAG;

    const char* theFunction = "SBT_Dataset/SBT_Dataset_add_dataset_file";

    ENTER(theFunction);

    if ((fd = open(filePath, O_RDONLY)) == -1) 
    {
        iRetcode = SBT_EMH_store_error("Unable to open input file '", filePath, "'", NULL);
    }

    /* Read the first sizeof(rbuf) characters from the file */
    memset(rbuf, 0, sizeof(rbuf));
    if ((iRetcode == ITK_ok) && (read(fd, (void*)rbuf, (int)rbufSize) == -1)) 
    {
        iRetcode = SBT_EMH_store_error("Unable to read file '", filePath, "*", NULL);
    }

    if (fd != -1) 
    {
        close(fd);
    }

    /* Determine the file type (SS_TEXT or SS_BINARY) */
    for (int i = 0; (i < (int)rbufSize) && rbuf[i] != '\0' && rbuf[i] != EOF; i++) 
    {
        if (!SBT_UTIL_is_ascii(rbuf[i])) 
        {
            fileType = SS_BINARY;
            break;
        }
    }

    /* Compute a new ImanFile name */
    if (iRetcode == ITK_ok)
    {
        int  revId = 0;
        char dsName[WSO_name_size_c + 1];
        const char* extension = NULL;

        ITK(WSOM_ask_name(datasetTag, dsName));

        if (iRetcode == ITK_ok)
        {
            ITK(WSOM_ask_revision(datasetTag, &revId));
        }

        if (iRetcode == ITK_ok)
        {
            if ((extension = strrchr(filePath, '.')) != NULL) {
                extension++;
            }

            newImanFileName = USER_new_file_name((const char*)dsName, referenceName, extension, revId);
            if (newImanFileName == NULL) {
                iRetcode = SBT_EMH_store_error("Unable to obtain new iMAN file name", NULL);
            }
        }
    }

    /* Import the file */
    if (iRetcode == ITK_ok)
    {
        IMF_file_t fileDesc = NULL;

        ITK(IMF_import_file(filePath, newImanFileName, fileType, &fileTag, &fileDesc));

        if ((iRetcode == ITK_ok) && (fileDesc != NULL)) 
        {
            ITK(IMF_close_file(fileDesc));
        }
    }

    /* Save and unlock the newly created ImanFile */
    if ((iRetcode == ITK_ok) && (fileTag != NULLTAG))
    {
        ITK(AOM_save(fileTag));

        if (iRetcode == ITK_ok)
        {
            ITK(AOM_refresh(fileTag, POM_no_lock));
        }
    }

    /* Lock the dataset for modification */
    if ((iRetcode == ITK_ok) && (fileTag != NULLTAG))
    {
        ITK(AOM_refresh(datasetTag, POM_modify_lock));
    }

    /* Add the new reference */
    if ((iRetcode == ITK_ok) && (fileTag != NULLTAG))
    {
        ITK(AE_add_dataset_named_ref(datasetTag, referenceName, AE_PART_OF, fileTag));
    }

    /* Save and unlock the dataset */
    if (iRetcode == ITK_ok && fileTag != NULLTAG)
    {
        ITK(AE_save_myself(datasetTag));

        if (iRetcode == ITK_ok)
        {
            ITK(AOM_refresh(datasetTag, POM_no_lock));
        }
    }
    
    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_Dataset_add_dataset_file */


/**
 * @brief Create a new empty dataset of the given type.
 * @param datasetName   [in]  The name of the dataset to create.
 * @param datasetType   [in]  The type name of the datast to create.
 * @param newDatasetTag [out] The dataset created.
 */
int SBT_Dataset_new_dataset 
  (
    const char *datasetName,                /* I */
    const char *datasetType,                /* I */
    tag_t *newDatasetTag                    /* O */
  )
{

    int   iRetcode = ITK_ok;
    char  dsName[WSO_name_size_c + 1];
    tag_t dsTag  = NULLTAG;
    tag_t dsTypeTag = NULLTAG;
    tag_t toolTag = NULLTAG;

    const char* theFunction = "SBT_Dataset/SBT_Dataset_new_dataset";

    ENTER(theFunction);

    // preset return variable
    *newDatasetTag = NULLTAG;

    /* Find the datasettype */
    ITK(AE_find_datasettype(datasetType, &dsTypeTag));

    if (iRetcode != ITK_ok || dsTypeTag == NULLTAG)
    {
        iRetcode = SBT_EMH_store_error("Unable to find dataset type '", datasetType, "'", NULL);
    }

    /* Get the default tool for the dataset type */
    if (iRetcode == ITK_ok)
    {
        ITK(AE_ask_datasettype_def_tool(dsTypeTag, &toolTag));
    }

    /* Create the new dataset */
    if (iRetcode == ITK_ok)
    {
        strncpy(dsName, datasetName, WSO_name_size_c);
        ITK(AE_create_dataset_with_id(dsTypeTag, dsName, dsName, NULL, NULL, &dsTag));
    }

    /* Set the dataset tool */
    if (iRetcode == ITK_ok)
    {
        ITK(AE_set_dataset_tool(dsTag, toolTag));
    }

    /* Set the dataset format */
    if (iRetcode == ITK_ok)
    {
        ITK(AE_set_dataset_format(dsTag, TEXT_REF));
    }

    /* Save the newly created dataset */
    if (iRetcode == ITK_ok && dsTag != NULLTAG)
    {
        ITK(AOM_save(dsTag));

        if (iRetcode == ITK_ok)
        {
            ITK(AOM_refresh(dsTag, POM_no_lock));
        }
    }

    if (iRetcode == ITK_ok) {
        *newDatasetTag = dsTag;
    }

    LEAVE(theFunction);
    return iRetcode;

} /* End SBT_Dataset_new_dataset */


/* End file SBT_Dataset.c */
