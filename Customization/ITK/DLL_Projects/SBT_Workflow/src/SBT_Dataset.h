/**
 * This file is to hold all utility methods used to deal with datasets.
 */

#ifndef _SBT_DATASET_H
#define _SBT_DATASET_H

#include <tc/iman.h>

/**
 * @brief The named reference typedef 
 */
typedef struct
{
    const char *dataset_type;          /* dataset type of the assigned dataset */
    const char *dataset_format;        /* format of the dataset */
    const int file_type;               /* type of the file. might be SS_TEXT or SS_BINARY */
    const char *named_reference;       /* type of the named reference used to reference the file */
    const char *file_name_ext;         /* extension of the assigned file */
} SBT_named_ref_t;

/**
 * @brief Asks file of dataset
 * @param dataset_tag         [in] tag of dataset
 * @param dataset_information [in] dataset information
 * @param type_index          [in] type
 * @param file_tag            [out] tag of file
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_Dataset_ask_assigned_file_tag
  (
    tag_t dataset_tag,                          /* I */
    SBT_named_ref_t *dataset_information,       /* I */
    int type_index,                             /* I */
    tag_t *file_tag                             /* O */
  );

/**
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
    tag_t       datasetTag,                          /* I */
    const char* filePath,                            /* I */
    const char* referenceName                        /* I */
  );

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
  );

#endif

/* End file SBT_Dataset.h */
