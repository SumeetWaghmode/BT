#include <tccore/item.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/tc_msg.h>
#include <tccore/custom.h>
#include <tccore/item_msg.h>
#include <tc/emh.h>
#include <user_exits/user_exit_msg.h>
#include <tc/iman.h>
#include <tccore/custom.h>
#include <tccore/tctype.h>
#include <tccore/item_errors.h>
#include <sa/person.h>
#include <tccore/grm.h>
#include <property/nr.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <sa/am_bypass.h>
#include <sa/am.h>
#include <res/res_itk.h>
#include <bom/bom.h>
#include <tc/preferences.h>
#include <vm\vms.h>


#include <bmf/libuserext_exports.h>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
//#include <norm/norm.h>

/*
* namespaces
*/
using namespace std;
using std::string;
using std::vector;

extern "C"
{
#include <norm/norm.h>
}

// --------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Common Functions
logical isDerivedFromClass(char *className, tag_t tObjectTag);
int askLatestReleaseStatus(tag_t tObject, char **sLatestStatusName, int *iCompleteStatusCount, tag_t *tLatestStatus);
logical isStatusEmptyOr5(tag_t tLastetStatus);
//logical isStatus5(tag_t tLatestStatus);
logical isStatus10(tag_t tLatestStatus);
logical isStatus60(tag_t tLatestStatus);
logical isStatus90(tag_t tLatestStatus);
logical isStatus98(tag_t tLatestStatus);
logical isStatusPrereleaseplus(tag_t tLatestStatus);
logical isSoftware(tag_t tItemRevision);
logical isCommonComponent(tag_t tItemRevision);
logical isStatusEmpty(tag_t tLatestStatus);

int createRelation(tag_t tPrimaryObject, tag_t tSecondaryObject, tag_t tRelationType);
int deleteRelation(tag_t tPrimary, tag_t tSecondary, tag_t tRelationType);
logical lDisableRule(logical lNeedByPass = true);
//logical lCurrentUserFromRastatt();
int revisePart(tag_t new_rev, tag_t parent_rev, tag_t tRelTypeConcerned, tag_t tRelTypeSpecified, tag_t tRelTypeIncluded, tag_t tRelTypeSupplierSpec);
int reviseDocument(tag_t new_rev, tag_t parent_rev, tag_t tRelTypeConcerned, tag_t tRelTypeSpecified, tag_t tRelTypeIncluded, tag_t tRelTypeSupplierSpec);
int getNewerRevisions(tag_t tItemRevision, vector<tag_t> *vecNewerRevisions);
logical isStatus90Plus10or60(tag_t tParentRev, tag_t tChildRev);
int setLogicalPOMAttribute(tag_t tObject, logical lValue, char *sAttributeName);
int setRelationBypassAttribute(tag_t tItemRevision, int iValue);
int askRelationBypassAttribute(tag_t tItemRevision);
logical isProgrammingData(tag_t tItemRevision);

// Server Extensions
extern USER_EXT_DLL_API int BT7_ProductCreatePreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_ProductSaveAsPreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_CommonComponentRequestsSavePostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_CMHasImpactedItemPreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_CMHasSolutionItemPreCondition(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_CMHasSolutionItemPostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_CMHasSolutionItemDeletePreAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_CMHasProblemItemPreCondition(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_ItemRevisionDeepCopyPostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_ItemRevisionSavePostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_ItemSavePreCondition(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_ItemCreatePreCondition(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_ItemCreateFromRevPreCondition(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_RelConcernedCreatePreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_RelSpecifiedCreatePreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_RelConcernedDeletePreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_RelSpecifiedDeletePreCondition(METHOD_message_t* method, va_list arg);
extern USER_EXT_DLL_API int BT7_RelConcernedCreatePostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_RelSpecifiedCreatePostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_BomWindowSavePreAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_SetReleaseStatusListPostAction(METHOD_message_t* msg, va_list args);
extern USER_EXT_DLL_API int BT7_RestrictAttibuteSize(METHOD_message_t* msg, va_list args);

#ifdef __cplusplus
}
#endif

// Status names
#define STATUS_TYPE_5						   "Locked"
#define STATUS_TYPE_10                         "Pre Release"
#define STATUS_TYPE_60                         "Serial Release"
#define STATUS_TYPE_90                         "Obsolete"
#define STATUS_TYPE_98                         "Annulled"
#define STATUS_TYPE_PRERELEASEPLUS             "Pre Release+"

#define DEBUG 1

#define BT7_DOCUMENTREVISION					"BT7_DocumentRevision"
// --------------------------------------------------------------------------

#define ITK_CALL(x){														\
    int stat;																\
    char *err_string;														\
    if((stat =(x))!= ITK_ok)												\
    {																		\
    EMH_get_error_string(NULLTAG, stat, &err_string);						\
	printf("ERROR: %d ERROR MSG: %s.\n", stat, err_string);					\
    printf("FUNCTION: %s\nFILE: %s LINE: %d\n", #x, __FILE__, __LINE__);	\
	TC_write_syslog("ERROR: %d ERROR MSG: %s.\n", stat, err_string);					\
    TC_write_syslog("FUNCTION: %s\nFILE: %s LINE: %d\n", #x, __FILE__, __LINE__);	\
	if(err_string)MEM_free(err_string);										\
    exit(EXIT_FAILURE);														\
    }																		\
}

// --------------------------------------------------------------------------
