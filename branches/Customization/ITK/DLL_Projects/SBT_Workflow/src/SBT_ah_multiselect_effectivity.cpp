/*=============================================================================
Copyright (c) 2008 Siemens PLM Software
Unpublished - All rights reserved
=============================================================================*/
/**
 * \file    SBT_ah_multiselect_effectifity
 * \brief   
 *
 * \author  Matthias Schoelch
 * \version 8.3
 * \date    15-Mar-2011
 */
/*
 * History:
 * Name		Date          TC-Rel		Description of changes
 * Schoelch 01-Dec-2009   TCEng2007		initial version
 * Schoelch 15-Mar-2011   TC83			Migrated to TC 8.3 and added to SBT_Workflow.dll
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <epm/epm.h>
#include <ict/ict_userservice.h>
#include <itk/mem.h>
#include <tc/emh.h>
#include <tc/preferences.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/item.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>

#include "SBT_ah_multiselect_effectivity.h"

#ifdef __cplusplus
}
#endif

/* this sequence is very common */
#define CHECK_FAIL if (ifail != 0) { LOG ("line %d\n", __LINE__); CdgLogger_log_error_stack(); m_nNumberOfErrors++;}


static int m_nNumberOfErrors = 0;
static char LOG_Buffer[10240];

/*
-------------------------------------------------------------------------
--- Methods 
-------------------------------------------------------------------------
*/



int SBT_AH_multiselect_effectivity( EPM_action_message_t msg )
{
	LOG("************************************\n");
	LOG("Enter SBT_AH_multiselect_effectivity\n");
	LOG("************************************\n");

	m_nNumberOfErrors = 0;
    int ifail = 0;
	tag_t l_tagRootJob = NULL;
	tag_t l_tagRootTask = NULL;
	int l_nAttachmentType = EPM_target_attachment;
	int l_nCountItemRevs = 0;
	tag_t * l_tagListItemRevs = NULL;
	int l_nCountReferences = 0;
	tag_t * l_tagListReferences = NULL;
	char* l_pcharArgument = NULL;
	char* l_pcharFlag     = NULL;
	char* l_pcharArgumentValue = NULL;
	char* l_pcharPreferenceName = NULL;
	date_t l_dateStart; 
	date_t l_dateEnd;
	InitDate(&l_dateStart, 0,0,0);
	InitDate(&l_dateEnd, 0,0,0);
	date_t* l_pdateStart = &l_dateStart;
	date_t* l_pdateEnd = &l_dateEnd;

	ifail = EPM_ask_job (msg.task, &l_tagRootJob);
	CHECK_FAIL;

	ifail = EPM_ask_root_task(l_tagRootJob, &l_tagRootTask);
	CHECK_FAIL;

	ifail = EPM_ask_attachments  	(   	
		l_tagRootTask,
		l_nAttachmentType,
		& l_nCountItemRevs,
		& l_tagListItemRevs
	);
	CHECK_FAIL;

	l_nAttachmentType = EPM_reference_attachment;
	ifail = EPM_ask_attachments  	(   	
		l_tagRootTask,
		l_nAttachmentType,
		& l_nCountReferences,
		& l_tagListReferences
	);
	CHECK_FAIL;

	while ( (l_pcharArgument = TC_next_argument(msg.arguments)) != NULL )
	{
		ifail = ITK_ask_argument_named_value( l_pcharArgument, &l_pcharFlag, &l_pcharArgumentValue );
		if ( l_pcharFlag == NULL || l_pcharArgumentValue == NULL )
		{
			continue;
		}

		// read the -preference
		if ( strcmp(l_pcharFlag, "preference")==0){
			l_pcharPreferenceName = (char *)MEM_alloc(sizeof(char)*(strlen(l_pcharArgumentValue)+ 1));
			strcpy( l_pcharPreferenceName , l_pcharArgumentValue );
		}
	}

	if (l_nCountReferences > 0)
	{
		ifail = AOM_ask_value_date (l_tagListReferences[0],
			"bt7_RevEffective_StartDate",
			&l_dateStart);
		CHECK_FAIL;
		if (l_dateStart.year == 0)
			l_pdateStart = NULL;

		ifail = AOM_ask_value_date (l_tagListReferences[0],
			"bt7_RevEffective_EndDate",
			&l_dateEnd);
		CHECK_FAIL;
		if (l_dateEnd.year == 0)
			l_pdateEnd = NULL;
	}

    int iRetcode = Process(l_nCountItemRevs, l_tagListItemRevs, l_pcharPreferenceName, l_pdateStart, l_pdateEnd);

	MEM_free(l_tagListItemRevs);
	MEM_free(l_pcharPreferenceName);
	return iRetcode;

   
}


static int Process(int in_nNumberOfRevs, tag_t * in_ptagRevisions, char* in_pcharPreferenceName, date_t *in_pdateStart, date_t *in_pdateEnd)
{
    if (in_pcharPreferenceName == NULL || in_pcharPreferenceName == "")
    {
        LOG("No Preference was set. Cancelling workflow!\n");
        return ITK_ok;
    }

    if (in_pdateStart == NULL && in_pdateEnd == NULL)
    {
        LOG("No Date was set. Cancelling workflow!\n");
        return ITK_ok;
    }

    int ifail = 0;
    int l_nPreferenceCount = 0;
    char** l_pcharPreferences = NULL;

    ifail = PREF_ask_char_values(in_pcharPreferenceName,
		&l_nPreferenceCount,
		&l_pcharPreferences);
    if (ifail != ITK_ok)
    {
        CHECK_FAIL;
        MEM_free(l_pcharPreferences);
        return ITK_ok;
    }

    // get current date
    time_t now = time(0);
    struct tm* tm = localtime(&now);
    date_t* l_pdateCurrent = (date_t*) MEM_alloc(sizeof(date_t));
    InitDate(l_pdateCurrent, (*tm).tm_year + 1900, (*tm).tm_mon, (*tm).tm_mday);

	date_t* l_pdateStartWithZeroTime = (date_t*) MEM_alloc(sizeof(date_t));
	if (in_pdateStart != NULL) 
		InitDate(l_pdateStartWithZeroTime, (*in_pdateStart).year, (*in_pdateStart).month, (*in_pdateStart).day);
	else
		l_pdateStartWithZeroTime = NULL;

	date_t* l_pdateEndWithZeroTime = (date_t*) MEM_alloc(sizeof(date_t));
    if (in_pdateEnd != NULL)
		InitDate(l_pdateEndWithZeroTime, (*in_pdateEnd).year, (*in_pdateEnd).month, (*in_pdateEnd).day);
	else
		l_pdateEndWithZeroTime = NULL;
	

    tag_t l_tagtypeItemRev;
    ifail = TCTYPE_find_type("ItemRevision", "ItemRevision", &l_tagtypeItemRev);
    CHECK_FAIL;

    for (int i = 0; i < in_nNumberOfRevs; i++)
    {
        int l_nCurrentNumberOfErrors = m_nNumberOfErrors;
        tag_t l_tagCurrentRev = in_ptagRevisions[i];
        char* l_pcharRevObjectId = NULL; 
        ifail = WSOM_ask_object_id_string  	( l_tagCurrentRev, &l_pcharRevObjectId);
        CHECK_FAIL;
        LOG("Processing Object [%s]\n", l_pcharRevObjectId);
        
        char l_charT [ITEM_type_size_c+1];
        ifail = ITEM_ask_rev_type  	( l_tagCurrentRev,l_charT);
        CHECK_FAIL;
        
        tag_t l_tagtypeCurrentRev;
        ifail =  TCTYPE_ask_object_type ( l_tagCurrentRev, &l_tagtypeCurrentRev);
        CHECK_FAIL;

        logical l_bIsKindOf = false;
        ifail = TCTYPE_is_type_of(l_tagtypeCurrentRev, l_tagtypeItemRev, & l_bIsKindOf);
        CHECK_FAIL;

        if (l_bIsKindOf == false)
        {
            LOG("[%s] is not a ItemRevision!\n", l_pcharRevObjectId);
            continue;
        }

            LOG("[%s] is a ItemRevision! Getting Release Status!\n", l_pcharRevObjectId);


        int l_nStatusCount = 0;
        tag_t* l_pStatusList = NULL;
        ifail = WSOM_ask_release_status_list  	(
            l_tagCurrentRev,
	        &l_nStatusCount,
	        &l_pStatusList);
        CHECK_FAIL;

        LOG("Number of Release Status [%d]\n", l_nStatusCount);
        for (int k = 0; k <l_nStatusCount; k++)
        {
            tag_t l_tagCurrentStatus = l_pStatusList[k];
            char* l_pcharStatusName = NULL;
            ifail = AOM_ask_name(l_tagCurrentStatus, &l_pcharStatusName);
            CHECK_FAIL;

            LOG("Processing Status [%s]\n", l_pcharStatusName);
            
            logical l_bFound = false;
            for (int j = 0; j <l_nPreferenceCount && l_bFound == false ; j++)
            {
                if (strcmp(l_pcharPreferences[j], l_pcharStatusName) == 0)
                {
                    l_bFound = true;
                }
            }
            
            // if not in Pref continue with next status
            if (l_bFound == false)
                continue;

            LOG("Status [%s] is in Preference. Getting Effectivities and Dates!\n", l_pcharStatusName);
            
            int l_nCountEffectivities = 0;
            tag_t* l_ptagEffectivities = NULL;
            tag_t l_tagCurrentEffectivity = NULL;
            ifail = WSOM_status_ask_effectivities (l_tagCurrentStatus, &l_nCountEffectivities, &l_ptagEffectivities);
            CHECK_FAIL;
            
            if (l_nCountEffectivities > 0)
            {
                for (int j = 0; j < l_nCountEffectivities && l_tagCurrentEffectivity == NULL; j++)
                {
                    tag_t l_tagRevision = NULL;
                    ifail = WSOM_eff_ask_end_item_rev(l_tagCurrentStatus, l_ptagEffectivities[j], &l_tagRevision);
                    CHECK_FAIL;
                    if (l_tagRevision == NULL)
                    //  if (l_tagRevision != NULL && l_tagRevision == l_tagCurrentRev)
                    {
                        l_tagCurrentEffectivity = l_ptagEffectivities[j];
                    }
                }
            }

            if (l_nCurrentNumberOfErrors != m_nNumberOfErrors)
            {
                LOG("Error while trying to get neccessary Information. Effectrivity would not created!");
                continue;
            }

            if (l_tagCurrentEffectivity == NULL)
            {
                LOG("No Effectivity found! Creating new!\n");
                ifail = AOM_lock(l_tagCurrentStatus);
                CHECK_FAIL;
                ifail = WSOM_effectivity_create_empty(l_tagCurrentStatus, &l_tagCurrentEffectivity);
                CHECK_FAIL;
                ifail = AOM_save(l_tagCurrentStatus);
                CHECK_FAIL;
                ifail = AOM_unlock(l_tagCurrentStatus);
                CHECK_FAIL;
            }

            if (l_tagCurrentEffectivity == NULL)
            {
                LOG("Error while creating new Effectivity. Proceeding with next ReleaseStatus!\n");
                continue;
            }

            date_t* l_pdateStartDateInEff = NULL;
            date_t* l_pdateEndDateInEff = NULL;
            int l_nCountDates = 0;
            date_t* l_pdateRange = NULL;
            WSOM_open_ended_status_t l_oStatus = EFFECTIVITY_closed;
            
            ifail = WSOM_eff_ask_dates(l_tagCurrentStatus, l_tagCurrentEffectivity,
                &l_nCountDates,&l_pdateRange, &l_oStatus); 
            CHECK_FAIL;
            
            if (l_nCountDates > 0)
                l_pdateStartDateInEff = &l_pdateRange[0];
            
            ifail = WSOM_eff_clear_ranges(l_tagCurrentStatus, l_tagCurrentEffectivity);
            CHECK_FAIL;

            date_t* l_pdateStartDateToSet = NULL;
			if (l_pdateStartDateInEff != NULL)
            {
                LOG ("Startdate was set before! No Change!\n");
                l_pdateStartDateToSet = l_pdateStartDateInEff;
            }
            else if(l_pdateStartWithZeroTime != NULL)
            {
                LOG ("Set Startdate to new Startdate!\n");
                l_pdateStartDateToSet = l_pdateStartWithZeroTime;
            }
            else
            {
                LOG ("Set Startdate to current date!\n");
                l_pdateStartDateToSet = l_pdateCurrent;
            }

            l_oStatus = EFFECTIVITY_closed;
            int l_nNumberOfDate = 0;
            date_t* l_pdateDates = NULL;

            if (l_pdateEndWithZeroTime != NULL) // case one. User set start and end date
            {
                LOG ("Set Enddate to new Enddate!\n");
                l_nNumberOfDate = 2;
                l_pdateDates = (date_t*) MEM_alloc(l_nNumberOfDate * sizeof(date_t));
                l_pdateDates[0] = *l_pdateStartDateToSet;
                l_pdateDates[1] = *l_pdateEndWithZeroTime;
            }
            else
            {
                LOG ("Set Enddate to open end!\n");
                l_nNumberOfDate = 1;
                l_pdateDates = (date_t*) MEM_alloc(l_nNumberOfDate * sizeof(date_t));
                l_pdateDates[0] = *l_pdateStartDateToSet;
                l_oStatus = EFFECTIVITY_open_ended;
            }

            ifail = AOM_lock(l_tagCurrentStatus);
            CHECK_FAIL;
            ifail = WSOM_eff_set_dates (l_tagCurrentStatus, l_tagCurrentEffectivity,
                l_nNumberOfDate,
                l_pdateDates, 
                l_oStatus,
                false); // append yes or no
            CHECK_FAIL;
            ifail = AOM_save(l_tagCurrentStatus);
            CHECK_FAIL;
            ifail = AOM_unlock(l_tagCurrentStatus);
            CHECK_FAIL;


            MEM_free(l_pdateDates);
            MEM_free(l_ptagEffectivities);
        }
        MEM_free(l_pStatusList);
    }
    MEM_free(l_pcharPreferences);
    MEM_free(l_pdateCurrent);
	MEM_free(l_pdateStartWithZeroTime);
	MEM_free(l_pdateEndWithZeroTime);
	
    LOG ("Exiting workflow with %d errors!\n", m_nNumberOfErrors);
    return ITK_ok;
}

static int InitDate(date_t *in_date_t, short year, byte month, byte day)
{
    if (in_date_t == NULL)
        return ITK_ok;

    (*in_date_t).day = day;
    (*in_date_t).month = month;
    (*in_date_t).year = year;
    (*in_date_t).hour = 0;
    (*in_date_t).minute = 0;
    (*in_date_t).second = 0;
    return ITK_ok;
}


static int CdgLogger_log_error_stack ()
{
    int retcode = ITK_ok;

    int error_num;
    const int* error_severities;
    const int* error_codes;
    const char** error_texts;
	int i = 0;

    retcode = EMH_ask_errors (&error_num, &error_severities, &error_codes, &error_texts);
    
    if (retcode == ITK_ok)
    {
        LOG("ERROR stack:\n");
        for (i=0; i<error_num; i++)
        {
            if (error_severities[i]==EMH_severity_error)
            {
                LOG ("severity=[ERROR]\n");
            }
            else if (error_severities[i]==EMH_severity_warning)
            {
                LOG ("severity=[WARNING]\n");
            }
            else if (error_severities[i]==EMH_severity_information)
            {
                LOG ("severity=[INFORMATION]\n");
            }
            else
            {
                LOG ("severity=[?]\n");
            }
            LOG ("code=[%d]\n", error_codes[i]);
            LOG ("text=[%s]\n", error_texts[i]);

        }
    }
    else
    {
        LOG ("Error [%d] occured while displaying error stack\n", retcode);
    }

    retcode = EMH_clear_errors();
    if (retcode != ITK_ok)
        LOG("Could not clear errors\n");

    /* return ITK_ok otherwise we could get to infinite loop when the error would occur in error display */
    return ITK_ok;
}


static int LOG (const char* text, ...)
{
    va_list args;

    va_start(args, text);
    if (args == NULL)
    {
        sprintf(LOG_Buffer, text);
    }
    else
    {
		vsprintf(LOG_Buffer, text, args);
    }
#ifdef _CONSOLE
    printf(LOG_Buffer);
#endif
    TC_write_syslog(LOG_Buffer);

    va_end(args);

    return ITK_ok;
}

