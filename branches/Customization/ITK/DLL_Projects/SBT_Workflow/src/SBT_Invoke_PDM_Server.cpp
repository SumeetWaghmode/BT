#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tc/iman_arguments.h>
#include <tccore/imantype.h>
#include <tccore/item.h>
#include <tccore/item_msg.h>
#include <tccore/method.h>
#include <sa/sa.h>
#include <property/nr.h>
#include <epm/epm_toolkit_tc_utils.h>
#include "sy_debug.h"
#include <math.h>
#include <tc/preferences.h>
#include <norm/norm.h>


int BT7_pref_get_string_values
(
    char*   pszPrefName,
    int*    piPrefValCnt,
    char*** pppszPrefVals
)
{
    int retcode     = ITK_ok;
    int iPrefCnt   = 0;

    TC_preference_search_scope_t prefSearchScope = TC_preference_user;

    ITK(PREF_initialize());

    ITK(PREF_ask_search_scope( &prefSearchScope ));

    ITK(PREF_set_search_scope( TC_preference_user ));

    ITK(PREF_ask_value_count( pszPrefName, &iPrefCnt ));

    if( iPrefCnt != 0 )
    {
    	ITK(PREF_ask_char_values( pszPrefName, piPrefValCnt, pppszPrefVals ));
    }

    ITK(PREF_set_search_scope( prefSearchScope ));

    return retcode;
}

int BT7_pref_set_string_values
(
    char*  pszPrefName,
    int    iPrefValCnt,
    char** ppszPrefVals
)
{
    int retcode     = ITK_ok;

    TC_preference_search_scope_t prefSearchScope = TC_preference_user;

    ITK(PREF_initialize());

    ITK(PREF_ask_search_scope( &prefSearchScope));

    ITK(PREF_set_search_scope( TC_preference_user ));

    ITK(PREF_set_char_values( pszPrefName, iPrefValCnt, ppszPrefVals ));

    ITK(PREF_set_search_scope( prefSearchScope ));

    return retcode;
}


/**
    Invoke pdm server
*/
extern DLLAPI int SBT_invoke_pdm_server( int *decision, va_list args )
{
	int			retcode			= ITK_ok;
    int			input_code 		= va_arg(args, int);
    char*		input_string	= va_arg(args, char*);
    int*		output_code		= va_arg(args, int*);
    char**		output_string	= va_arg(args, char**);

	char* pszExtensionName  = NULL;
	int iCountOfPrefValues = 0;
	char**  ppszExtensionStatus = NULL;
	char* values[] = {"true", "false"};
	int value_count = 0;
	int iPrefCnt   = 0;
	pszExtensionName = "BT7_Save_From_NX_to_Teamcenter";

    if(input_code == 1001)
	{
		ITK(BT7_pref_get_string_values(pszExtensionName, &iCountOfPrefValues, &ppszExtensionStatus));
		//printf("Current Value of User Preference at input_code == 1001 %s\n", ppszExtensionStatus[0]);

		ITK(BT7_pref_set_string_values(pszExtensionName, 1, &values[0]));
		//ITK(PREF_set_char_values(pszExtensionName,1,&values[0]));
		//printf("New Value of User Preference at input_code == 1001 %s\n", values[0]);

		return retcode;
	}

	else if(input_code == 2001)
	{
		ITK(BT7_pref_get_string_values(pszExtensionName, &iCountOfPrefValues, &ppszExtensionStatus));
		//printf("Current Value of User Preference at input_code == 2001 %s\n", ppszExtensionStatus[0]);

		ITK(BT7_pref_set_string_values(pszExtensionName, 1, &values[1]));
		//ITK(PREF_set_char_values(pszExtensionName,1,&values[1]));
		//printf("New Value of User Preference at input_code == 2001 %s\n", values[1]);

		return retcode;
	}

    return retcode;;

}

#ifdef __cplusplus
}
#endif
