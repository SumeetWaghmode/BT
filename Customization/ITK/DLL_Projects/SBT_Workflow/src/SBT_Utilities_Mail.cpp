#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <epm/signoff.h>
#include <epm/distributionlist.h>
#include <epm/aliaslist.h>
#include <tccore/method.h>
#include <itk/mem.h>
#include <property/prop.h>
#include <sa/sa.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tc/folder.h>
#include <ics/ics.h>
#include <ae/dataset.h>
#include <ae/datasettype.h>
#include <tc/envelope.h>

#include <sub_mgr/tcactionhandler.h>
#include <sub_mgr/tceventmgr.h>
#include <sub_mgr/tceventtype.h>
#include <sub_mgr/standardtceventtypes.h>
#include <sub_mgr/subscription.h>

#include "sy_debug.h"

#include "SBT_Workspaceobject.h"
#include "SBT_ItemRevision.h"
#include "SBT_Utilities.h"
#include "SBT_Utilities_d.h"
#include "SBT_Workflow.h"
#include "SBT_Workflow_Attributes.h"
#include "SBT_File.h"

#include "SBT_Workflow_grouphandler.h"

#ifdef __cplusplus
}
#endif

// Include Common
#include "SBT_Common.h"

// Declarations
int process_recipient (tag_t mail, tag_t recipient);
int process_user_recipient (tag_t tagMail, tag_t userTag);
int process_groupmember_recipient (tag_t mail, tag_t groupmemberTag);
int process_group_recipient(tag_t mail, tag_t groupTag);
int process_dlist_recipient (tag_t mail, tag_t dlist);
int process_alias_list_recipient(tag_t mail, tag_t alistTag);
int process_resource_pool_recipient(tag_t mail, tag_t recipient);

// --------------------------------------------------------------------

// Prepare Mail
int createMail(const char *sMailSubject, const char *sMailMessage, tag_t *tMail)
{
	int   iStatus = ITK_ok;

	if ( ( strlen(sMailSubject) <= 128 ) && ( strlen(sMailMessage) <= 240 ) )
	{
		//Create envelope
		ITK(MAIL_create_envelope(sMailSubject, sMailMessage, tMail) );
	} else {
		// TODO: error message
		iStatus = 999999;
	}

	return iStatus;
}

// Sent Mails
int sendEmailNotification(tag_t tMail)
{
	int   iStatus = ITK_ok;

	//Send email
	ITK(MAIL_send_envelope(tMail));

	return iStatus;
}


int addReceiverToMail(tag_t mail, tag_t recipient)
{
    int         retcode       = ITK_ok;
    tag_t       classIdOfInst = NULLTAG;

    static int   sinit = 0;
    static tag_t sUserClassId        = NULLTAG;
    static tag_t sGroupClassId       = NULLTAG;
    static tag_t sGroupmemberClassId = NULLTAG;
    static tag_t sDlistClassId       = NULLTAG;
    static tag_t sAlistClassId       = NULLTAG;
	static tag_t sResourcePoolId     = NULLTAG;

    if (sinit == 0)
    {
       ITK(POM_class_id_of_class("User", &sUserClassId));

       if (retcode == ITK_ok)
       {
           ITK(POM_class_id_of_class("Group", &sGroupClassId));
       }

       if (retcode == ITK_ok)
       {
           ITK(POM_class_id_of_class("DistributionList", &sDlistClassId));
       }

       if (retcode == ITK_ok)
       {

           ITK(POM_class_id_of_class("GroupMember", &sGroupmemberClassId));
       }

       if (retcode == ITK_ok)
       {
           ITK(POM_class_id_of_class("ImanAliasList", &sAlistClassId));
       }

       if (retcode == ITK_ok)
       {
           ITK(POM_class_id_of_class("ResourcePool", &sResourcePoolId));
       }

       if (retcode == ITK_ok)
       {
           sinit = 1;
       }
    }

    ITK(POM_class_of_instance(recipient, &classIdOfInst));

    if ((retcode == ITK_ok) && (classIdOfInst != NULLTAG))
    {
        if (classIdOfInst == sUserClassId )
        {
            ITK(process_user_recipient(mail, recipient));
        }
        else if (classIdOfInst == sGroupClassId )
        {
            ITK(process_group_recipient(mail, recipient));
        }
        else if (classIdOfInst == sGroupmemberClassId )
        {
            ITK(process_groupmember_recipient(mail, recipient));
        }
        else if (classIdOfInst == sDlistClassId )
        {
            ITK(process_dlist_recipient(mail, recipient));
        }
        else if (classIdOfInst == sAlistClassId )
        {
            ITK(process_alias_list_recipient(mail, recipient));
        }
        else if (classIdOfInst == sResourcePoolId )
        {
            ITK(process_resource_pool_recipient(mail, recipient));
        }
        else
        {
            retcode = POM_invalid_class_id;
        }
    }

    return (retcode);
}

int process_user_recipient (tag_t tagMail, tag_t userTag)
{
    int         retcode     = ITK_ok;

	ITK(MAIL_add_envelope_receiver(tagMail, userTag) );

    return ( retcode );
}


int process_groupmember_recipient (tag_t mail, tag_t groupmemberTag)
{
    int         retcode      = ITK_ok;
    char*       userEmail    = NULL;
    tag_t       userTag      = NULLTAG;

	// Get User
    ITK(SA_ask_groupmember_user(groupmemberTag, &userTag));

	// process user
	process_user_recipient(mail, userTag);

    return ( retcode );
}

int process_group_recipient(tag_t mail, tag_t groupTag)
{
    int         i;
    int         retcode      = ITK_ok;
    int         memberCount  = 0;
    tag_t*      memberList  = NULL;

	// Get Groupmembers
    ITK(SA_find_groupmembers_by_group(groupTag, &memberCount, &memberList));

    for (int iIdx1 = 0; iIdx1 < memberCount; iIdx1++)
    {
		// Process Groupmembers
		process_groupmember_recipient(mail, memberList[iIdx1]);
    }

    MEM_free(memberList);

    return (retcode);
}

int process_dlist_recipient (tag_t mail, tag_t dlist)
{
    int         retcode       = ITK_ok;
    int         membersCount  = 0;
    int         i             = 0;
    tag_t*      members       = NULL;
    tag_t       classIdOfInst = NULLTAG;

    static int   sinit               = 0;
    static tag_t sUserClassId        = NULLTAG;
    static tag_t sGroupClassId       = NULLTAG;
    static tag_t sGroupmemberClassId = NULLTAG;
    static tag_t sDlistClassId       = NULLTAG;

    if (sinit == 0)
    {
       ITK(POM_class_id_of_class("User", &sUserClassId));

       if (retcode == ITK_ok)
       {
           ITK(POM_class_id_of_class("Group", &sGroupClassId));
       }

       if (retcode == ITK_ok)
       {
           ITK(POM_class_id_of_class("DistributionList", &sDlistClassId));
       }

       if (retcode == ITK_ok)
       {

           ITK(POM_class_id_of_class("GroupMember", &sGroupmemberClassId));
       }

       if (retcode == ITK_ok)
       {
           sinit = 1;
       }
    }

    if (retcode == ITK_ok)
    {
        ITK(MAIL_ask_dist_list_members(dlist, &membersCount, &members));
    }

    for ( i = 0; (retcode == ITK_ok) && (i < membersCount); i++ )
    {
        ITK(POM_class_of_instance( members[i], &classIdOfInst));

        if ((retcode == ITK_ok) && (classIdOfInst != NULLTAG) )
        {
            if (classIdOfInst == sUserClassId )
            {
                ITK(process_user_recipient(mail, members[i]));
            }
            else if (classIdOfInst == sGroupClassId )
            {
                ITK(process_group_recipient(mail, members[i]));
            }
            else if (classIdOfInst == sGroupmemberClassId )
            {
                ITK(process_groupmember_recipient(mail, members[i]));
            }
            else if (classIdOfInst == sDlistClassId )
            {
                ITK(process_dlist_recipient(mail, members[i]));
            }
            else
            {
                retcode = POM_invalid_class_id;
            }
        }
    }

    MEM_free( members );

    return (retcode);
}

int process_alias_list_recipient(tag_t mail, tag_t alistTag)
{
    int         retcode      = ITK_ok;
    int         i            = 0;
    tag_t       userTag      = NULLTAG;
    tag_t       groupTag     = NULLTAG;
    int         addressCount = 0;
    char**      addressList  = NULL;

    ITK(MAIL_ask_alias_list_members(alistTag , &addressCount, &addressList));

    for (i = 0; retcode == ITK_ok && i < addressCount; i++)
    {
          retcode = SA_find_user(addressList[i], &userTag);
          if (retcode == ITK_ok && userTag != NULLTAG)
          {
			  process_user_recipient(mail, userTag);
          }
          else
          {
              retcode = SA_find_group(addressList[i], &groupTag);
              if (retcode == ITK_ok && groupTag != NULLTAG)
              {
				  process_group_recipient(mail, groupTag);
              }
              else if (strchr(addressList[i], '@'))
              {
                  // TODO: Handler external emails!
              }
          }
    }

    if ((addressList != NULL) && (retcode == ITK_ok))
    {
        int j;

        for (j = 0; (j < addressCount); j++) {
            MEM_free(addressList[j]);
        }

        MEM_free(addressList);
    }

    return retcode;
}

int process_resource_pool_recipient(tag_t mail, tag_t recipient)
{
    int		retcode			= ITK_ok;
	logical	lAllowSubGroups	= false;
	tag_t	tTmpGroup		= NULLTAG;
	tag_t	tTmpRole		= NULLTAG;
	
	int		iMemberCount	= 0;
	tag_t	*taMembers		= NULL;

	// Get Group / Role
	ITK(EPM_ask_resource_pool_group_role(recipient, &tTmpGroup, &tTmpRole, &lAllowSubGroups) );

	if(tTmpGroup != NULLTAG && tTmpRole != NULLTAG)
	{
		// Get Groupmembers by Group and Role
		ITK(SA_find_groupmember_by_role(tTmpRole, tTmpGroup, &iMemberCount, &taMembers) );
	} else if(tTmpGroup != NULLTAG) {
		// Get Groupmembers by Group
		ITK(SA_find_groupmembers_by_group(tTmpGroup, &iMemberCount, &taMembers));
	}

	// Traverse Group/Role Members
	for ( int iIdx2 = 0; iIdx2 < iMemberCount; iIdx2++ )
	{
		process_groupmember_recipient(mail, taMembers[iIdx2]);
	}

	// Free
	SAFE_SM_FREE(taMembers);

    return retcode;
}