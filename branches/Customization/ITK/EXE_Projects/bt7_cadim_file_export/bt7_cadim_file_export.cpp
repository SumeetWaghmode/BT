#import <MSXML6.dll>

#include <ctype.h>
#include <atlbase.h>
#include <msxml.h>
#include <cstdio>
#include <cfloat>
#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <sstream>
#include <itk/mem.h>
#include <unidefs.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <sa/tcfile.h>
#include <sa/user.h>
#include <string.h>
#include <fclasses/tc_string.h>
#include <fclasses/tc_date.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tc/envelope.h>
#include <tccore/aom.h>
#include <tccore/project.h>
#include <tccore/aom_prop.h>
#include <property/nr.h>
#include <qry/qry.h>
#include <tccore/grm.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tccore/uom.h>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <user_exits\user_exits.h>
#include <property/prop.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <Shlwapi.h>


// --------------------------------------------------------------------------

#define ITK_CALL(x){                                                                                                                                                                                                                     \
   int stat;                                                                                                                                                                                                                                                             \
    char *err_string;                                                                                                                                                                                                                          \
    if((stat =(x))!= ITK_ok)                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                                                          \
    EMH_get_error_string(NULLTAG, stat, &err_string);                                                                                  \
    printf(" > ERROR: %d ERROR MSG: %s.\n", stat, err_string);                                                     \
    printf(" >  FUNCTION: %s\nFILE: %s LINE: %d\n", #x, __FILE__, __LINE__);\
    if(err_string)MEM_free(err_string);                                                                                                                                                   \
    exit(EXIT_FAILURE);                                                                                                                                                                                                                   \
    }                                                                                                                                                                                                                                                                                          \
}

// --------------------------------------------------------------------------

using namespace std;

// --------------------------------------------------------------------------

int getDatasetNamedReferences(tag_t dataset, char reference_name[AE_reference_size_c + 1], tag_t *reference_object);

// --------------------------------------------------------------------------
int prepareForModify(tag_t tObject)
{
	tag_t	tClassIdTag					= NULLTAG;
	int		iInstanceState				= POM_inst_is_not_loaded;
	tag_t	*taObjects					= (tag_t*) MEM_alloc(1 * sizeof(tag_t));

	taObjects[0] = tObject;

	// Get Class ID
	ITK_CALL(POM_class_id_of_class("WorkspaceObject", &tClassIdTag));

	// Ask state
	ITK_CALL(POM_ask_instance_state(tObject, &iInstanceState));
	if(iInstanceState != POM_inst_is_loaded_for_modify)
	{
		logical lIsLoaded = false;

		// Is already loaded ?
		ITK_CALL(POM_is_loaded(tObject, &lIsLoaded));

		if(lIsLoaded == true)
		{
			// Unload
			ITK_CALL(POM_unload_instances(1, taObjects));
		}

		// Load instance
		ITK_CALL(POM_load_instances(1, taObjects, tClassIdTag, POM_modify_lock));
	}

	// Free
	if(taObjects != NULL) MEM_free(taObjects);

	return ITK_ok;
}

int ITK_user_main(int argc , char* argv[]) {
                logical    lInputError                                          = false;
                logical    lUseInputFile                     = false;
    int                       retcode                                                                = ITK_ok;
    char   *user_id                                              = NULL;
    char   *user_passwd                  = NULL;
    char   *user_group                                      = NULL;
                char       *file_uid                                              = NULL;
                char       *cadim_db_entry_uid   = NULL;
                char       *export_loc                                       = NULL;

                char       reference_name[AE_reference_size_c + 1];
                tag_t     reference_object            = NULLTAG;
                char       *sDestinationPath           = NULL;
                string     strDestinationPath          = "";

				
                /* Gets the user ID */
    user_id = ITK_ask_cli_argument("-u=");

    /* Gets the user Password */
    user_passwd = ITK_ask_cli_argument("-p=");

    /* Gets the user group */
    user_group = ITK_ask_cli_argument("-g=");

                /* Gets the file_uid */
    file_uid = ITK_ask_cli_argument("-file_uid=");

                /* Gets the cadim_db_entry_uid */
                cadim_db_entry_uid = ITK_ask_cli_argument("-cadim_db_entry_uid=");

                /* Gets the export location */
                export_loc = ITK_ask_cli_argument("-export_loc=");


                /* Validate parameters */
                if(user_id == NULL || strcmp(user_id, "") == 0){
                                printf(" > ERROR: Parameter '-u' should not be empty! \n");
                                lInputError = true;
                }
                
                if(user_passwd == NULL || strcmp(user_passwd, "") == 0){
                                printf(" > ERROR: Parameter '-p' should not be empty! \n");
                                lInputError = true;
                }
                
                if(user_group == NULL || strcmp(user_group, "") == 0){
                                printf(" > ERROR: Parameter '-g' should not be empty! \n");
                                lInputError = true;
                }
                
                if(file_uid == NULL || strcmp(file_uid, "") == 0){
                                printf(" > ERROR: Parameter '-file_uid' should not be empty! \n");
                                lInputError = true;
                }
                
                if(cadim_db_entry_uid == NULL || strcmp(cadim_db_entry_uid, "") == 0){
                                printf(" > ERROR: Parameter '-cadim_db_entry_uid' should not be empty! \n");
                                lInputError = true;
                }

                if(export_loc == NULL || strcmp(export_loc, "") == 0){
                                printf(" > ERROR: Parameter '-export_loc' should not be empty! \n");
                                lInputError = true;
                }


                /* Exit if parameters are not correct */
                if(lInputError == true) {
                                return 9999;
                }


                /* Logging into Teamcenter */
                printf(" > Login to Teamenter (this might take a moment ...) \n");
                ITK_CALL ( ITK_init_module(user_id, user_passwd, user_group));
                ITK_CALL ( ITK_set_bypass(true));
                printf(" > Login successful \n");


                // Get TC-Objects by parameters
                tag_t tFile = NULLTAG;
                tag_t tCadimDbEntry = NULLTAG;

                ITK__convert_uid_to_tag(file_uid, &tFile);
                ITK__convert_uid_to_tag(cadim_db_entry_uid, &tCadimDbEntry);

                if(tFile == NULLTAG || tCadimDbEntry == NULLTAG){
                                printf(" > ERROR: Tc-Ojects could not be identified by parameters (%s and %s) ! \n", file_uid, cadim_db_entry_uid);
                                return 999999;
                }

                // Get Named Reference
                getDatasetNamedReferences(tFile, reference_name, &reference_object);

                // Get Destination Path
                ITK_CALL(AOM_ask_value_string(tCadimDbEntry, "bt7_CDM_RELATIVEPATH", &sDestinationPath));
                strDestinationPath.append(export_loc);
                strDestinationPath.append("/");
                strDestinationPath.append(sDestinationPath);

                // Export Named Reference
                printf(" > reference_name     = %s \n", reference_name);
                printf(" > strDestinationPath = %s \n", strDestinationPath.c_str());

				if( PathFileExists(strDestinationPath.c_str()) == 1 )
				{
					remove(strDestinationPath.c_str());
				}
                ITK_CALL(AE_export_named_ref(tFile, reference_name, strDestinationPath.c_str()));

                // Update CADIM DB-Entry
				// prepareForModify(tCadimDbEntry);
				ITK_CALL(AOM_refresh(tCadimDbEntry, true));
                ITK_CALL(AOM_set_value_string(tCadimDbEntry, "bt7_CDM_L5_TRANS_STAT", "INIT"));
				
                ITK_CALL(AOM_save(tCadimDbEntry));
				ITK_CALL(AOM_refresh(tCadimDbEntry, false));
				
                // Free
                SAFE_SM_FREE(sDestinationPath);

                // Logout
                printf(" > Logout \n");
                ITK_CALL ( ITK_exit_module(true));

    return retcode;
}

int getDatasetNamedReferences(tag_t dataset, char reference_name[AE_reference_size_c + 1], tag_t *reference_object) {
                int iRetValue = -1;
    int iReference_count = 0;
    tag_t prop_tag = NULLTAG;
    AE_reference_type_t reference_type;

                /*  Make sure no one messes with it while we're reading it */
    ITK_CALL(AOM_refresh(dataset, false));

    ITK_CALL(AE_ask_dataset_ref_count(dataset, &iReference_count));
    if (iReference_count > 0) {
        for (int iIdx1 = 0; iIdx1 < iReference_count; iIdx1++) {
                                                tag_t     prop_tag             = NULLTAG;
                                                char       *prop_value      = NULL;

            ITK_CALL(AE_find_dataset_named_ref(dataset, 0, reference_name, &reference_type, reference_object));

                                                ITK_CALL(PROP_ask_property_by_name(*reference_object, "object_type", &prop_tag) );
                                                ITK_CALL(PROP_UIF_ask_value(prop_tag, &prop_value));

            if (strcmp(prop_value, "ImanFile") == 0) {
                iRetValue = ITK_ok;
                                                                SAFE_SM_FREE(prop_value);
                                                                break;
           }

            SAFE_SM_FREE(prop_value);
        }
    }

    return iRetValue;
}
