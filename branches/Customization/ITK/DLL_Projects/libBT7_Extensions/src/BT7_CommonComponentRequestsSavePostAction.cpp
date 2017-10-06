#include "BT7_ServerExtensions.h"


void getCurrentPerson(tag_t	*tPerson) {
	char	*sUserName	= NULL;
	tag_t	tUser		= NULLTAG;

	ITK_CALL(POM_get_user(&sUserName, &tUser));
	ITK_CALL(SA_ask_user_person(tUser, tPerson));

	if(sUserName != NULL) MEM_free(sUserName);
}

extern USER_EXT_DLL_API int BT7_CommonComponentRequestsSavePostAction(METHOD_message_t* msg, va_list args) {
	int		status	= ITK_ok;
    tag_t	tForm	= va_arg(args, tag_t);
    logical	isNew	= va_arg(args, logical);

	tag_t	tPerson	= NULLTAG;
	char	*sValue			= NULL;
	char	*sPropRequestor = "bt7_Requestor";
	char	*sPropPhone		= "bt7_Phone";
	char	*sPropEmail		= "bt7_Email";
	logical	lModified		= false;

	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_CommonComponentRequestsSavePostAction \n");
	if(DEBUG == 1) printf("################################################################ \n");

	// Set object name on initial create
	if (isNew == true) {
		char *sNextFreeCCR	= NULL;
		
		ITK_CALL(NR_next_item_id("BT7_CCR_DUMMY", &sNextFreeCCR));
		//ITK_CALL(NR_next_value("BT7_CCR_DUMMY", "item_id", NULLTAG,"", "", "", NULLTAG, ""," ",  &sNextFreeCCR));
		//ITK_CALL(NR_next_value("BT7_CCR_DUMMY", "item_id", NULLTAG,"", "", "", NULLTAG, "CCR-NNNNNNNN"," ",  &sNextFreeCCR));
		printf("Next free CCR-ID = %s \n", sNextFreeCCR);
		ITK_CALL(WSOM_set_name(tForm, sNextFreeCCR));

		if(sNextFreeCCR != NULL) MEM_free(sNextFreeCCR);
	}

	// Get person
	getCurrentPerson(&tPerson);

	// Get Requestor
	ITK_CALL(AOM_ask_value_string(tForm, sPropRequestor, &sValue));

	if(strcmp(sValue, "") == 0) {
		if(tPerson != NULLTAG) {
			char person_name[SA_person_name_size_c + 1] = "";

			ITK_CALL(SA_ask_person_name(tPerson, person_name));

			if(strcmp(person_name, "") != 0) {
				ITK_CALL(AOM_set_value_string(tForm, sPropRequestor, person_name));
				lModified = true;
			}
		}
	}

	// Get Phone
	sValue = NULL;
	ITK_CALL(AOM_ask_value_string(tForm, sPropPhone, &sValue));
	if(strcmp(sValue, "") == 0) {
		if(tPerson != NULLTAG) {
			ITK_CALL(SA_ask_person_attr(tPerson, "PA10", &sValue));
			
			if(strcmp(sValue, "") != 0) {
				ITK_CALL(AOM_set_value_string(tForm, sPropPhone, sValue));
				lModified = true;
			}
		}
	}

	// Get Email
	sValue = NULL;
	ITK_CALL(AOM_ask_value_string(tForm, sPropEmail, &sValue));
	if(strcmp(sValue, "") == 0) {
		if(tPerson != NULLTAG) {
			ITK_CALL(SA_ask_person_attr(tPerson, "PA9", &sValue));
			
			if(strcmp(sValue, "") != 0) {
				ITK_CALL(AOM_set_value_string(tForm, sPropEmail, sValue));
				lModified = true;
			}
		}
	}

	// Save changes
	if(lModified == true) {
		ITK_CALL(AOM_save(tForm));
	}

	// Free
	if(sValue != NULL) MEM_free(sValue);

	return status;
}