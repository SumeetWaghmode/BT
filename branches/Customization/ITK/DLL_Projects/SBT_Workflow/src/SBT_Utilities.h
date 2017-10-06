/**
 * This file is to hold general utility methods.
 */

#ifndef _SBT_UTILITIES_H
#define _SBT_UTILITIES_H

#include <tc/iman.h>

/** @brief Error base */
#define SBT_ERROR_BASE                          919500
/** @brief Multiple files attached where one was expected */
#define SBT_MULTIPLE_FILES                      919501
/** @brief An .iman_env entry is missing */
#define SBT_IMAN_ENV_ENTRIES_MISSING            919502
/** @brief error message for free formatted text */
#define SBT_ERROR_DETAILS                       919599

/** @brief BT statuses */
// Status names
#define STATUS_TYPE_5						   "Locked"
#define STATUS_TYPE_10                         "Pre Release"
#define STATUS_TYPE_60                         "Serial Release"
#define STATUS_TYPE_90                         "Obsolete"
#define STATUS_TYPE_98                         "Annulled"
#define SBT_STATUS_BT7_ECO                     "BT7_ECO"

/** @brief ECO Workflow */
#define SBT_WF_JOBNAME_SERIALIZATION			"Serialization"

/** @brief BT Business object revision */
#define SBT_BT_CHANGENOTICEREVISION				"ChangeNoticeRevision"
#define SBT_BT_COMPPROGREVISION					"BT7_CompProgRevision"
#define SBT_BT_ELCADREVISION					"BT7_ELCADRevision"
#define SBT_BT_DOCUMENTREVISION					"BT7_DocumentRevision"
/** @brief TC Business object */
#define TC_ITEMREVISION							"ItemRevision"
#define TC_PARTREVISION							"Part_0_Revision_alt"
#define TC_ITEMREVISION							"ItemRevision"
#define TC_DATASET								"Dataset"

/** @brief TC ItemRevision's Custom Properties */
#define BT_PROP_BT7_DOCMNUMBER						"bt7_docmnumber"
/** @brief Definition of the preference setting for the directory used for temporary files. */
static const char *SBT_tmp_export_dir     = "SBT_tmp_export_dir";

/** @brief The seperator used by the operating system to assemble file pathes */
static const char *SBT_directory_seperator = "\\";

/**
 * @brief A convenience function to store errors.
 *        Insert text stringt, comma seperated, terminate with a null.
 */
int SBT_EMH_store_error
  (
    const char *text, 
    ...
  );

/**
 * @brief Reads one entry of the iman env entry specified.
 * @param iman_env_entry_name The iman_env entry to read.
 * @param index               The index of the value to read (starts at 0).
 * @param value               The value read.
 * @return ITK_ok on successfull execution, an error code otherwise.
 */
int SBT_UTIL_read_iman_env_entry
  (
    const char *iman_env_entry_name,                            /* I */
    int index,                                                  /* I */
    char **value                                                /* OF */
  );

/**
 * @breif Reads the entries of the iman env entry specified.
 * \param iman_env_entry_name the iman_env entry to read
 * \param count the number of entries found
 * \param values the array of entries found
 */
int SBT_UTIL_read_iman_env_entries
  (
    const char *iman_env_entry_name,                            /* I */
    int *count,                                                 /* O */
    char ***values                                              /* OF */
  );

/**
 * @brief This function creates a unique file name in the directory given
 *        by the .iman_env variable SBT_tmp_export_dir.
 * @param extension An extension (if desired, otherwise NULL).
 * @param file_path_name the path and name of the unique file created.
 */
int SBT_UTIL_create_unique_filename
  (
    const char *extension,
    char **file_path_name
  );

/**
 * @brief This function creates a unique file name in the directory given
 *        by the .iman_env variable specified.
 * @param config    The .iman_env variable where to red the directory from
 *                  where we should create the unique file.
 * @param extension An extension (if desired, otherwise NULL).
 * @param file_path_name the path and name of the unique file created.
 */
int SBT_UTIL_create_unique_filename_by_config
  (
    const char *config,
    const char *extension,
    char **file_path_name
  );

logical object_is_of_class_or_desc (
	const tag_t		objectTag ,				/* (I) */
	const char		* pcClassName ,			/* (I) */
	tag_t			* pObjectClassIdTag );	/* (O) */


char* getInternalIdOfTcObject(tag_t tTcObject);

int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount);

int CLOCK_compare_dates(const date_t *date1, const date_t *date2);

int getSecondaryObjects(tag_t tItemRevision, char *sRelationType, tag_t *tRelationType, int *iSecondaryCount, tag_t **taSecondaryObjects);

int getCurrentDateTime(char sDateTime[20]);

int createRelation(tag_t tPrimaryObject, tag_t tSecondaryObject, tag_t tRelationType);
int deleteRelation(tag_t tPrimary, tag_t tSecondary, tag_t tRelationType);
int returnStatusRealName(char *sStatusName, char **sLatestStatusName);
int prepareForModify(tag_t tObject);
int replace_bom_child(tag_t tParentRevision, tag_t revRuleTag, tag_t oldChildRev, tag_t newChildRev, tag_t bomView);
logical hasStatus(tag_t tTcObject, const char *sStatus);
logical isSoftware(tag_t tItemRevision);
logical isProgrammingData(tag_t tItemRevision);
logical isDerivedFromClass(char *className, tag_t tObjectTag);

/* create dataset not using default tool */
tag_t createDataset(tag_t task, tag_t txtFile, const char *name);

/* Clone Dataset */
void copyDatasets( tag_t tItemRevWithSuffix, tag_t tItemRevWithoutSuffix );

#endif

/* End file SBT_Utilities.h */
