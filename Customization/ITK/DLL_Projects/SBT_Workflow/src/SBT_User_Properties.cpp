/**
 * This file holds the implementation of the TCE user properties.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <property/prop.h>
#include <sa/sa.h>
#include <tccore/method.h>

#include "sy_debug.h"

#include "SBT_User_Properties.h"

#ifdef __cplusplus
}
#endif

/**
 * @brief
 * Return the email address of the given iMAN user.
 * The email address is the property named "PA9" of the person associated
 * to the iMAN user. If this property is empty, we attempt to get the 
 * property "PA8", the external email address, instead. If this is also 
 * empty, this empty property is returned.
 * Note : parameter iMANUserTag can be either a tag of class 'User' or 'Person'
 */
int SBT_UP_ask_iMAN_user_email_address 
  (
    tag_t  iMANUserTag,                              /* I */ 
    char **iMANUserEmailAddress                      /* OF */
  )
{
    int retcode = ITK_ok;

    const char* theFunction = "SBT_User_Properties/SBT_UP_ask_iMAN_user_email_address";
    TRACE_ON(0);
    ENTER(theFunction);

    // preset return values
    *iMANUserEmailAddress = NULL;

    ITK(SBT_UP_ask_iMAN_user_person_property(iMANUserTag, "PA9", iMANUserEmailAddress));
    if(iMANUserEmailAddress == NULL || *iMANUserEmailAddress == NULL || strlen(*iMANUserEmailAddress) == 0)
    {
        ITK(SBT_UP_ask_iMAN_user_person_property(iMANUserTag, "PA8", iMANUserEmailAddress));
    }
    
    LEAVE(theFunction);
    return  retcode;

} /* End SBT_UP_ask_iMAN_user_email_address */


/**
 * @brief
 * Return the value of the given property of the person associated to the 
 * given iMAN user.
 * The person property is retrieved by send the message 'PROP_ask_value_string_msg' to
 * the method registered for the required property.
 * Note : parameter iMANUserTag can be either a tag of class 'User' or 'Person'
 */
int SBT_UP_ask_iMAN_user_person_property 
  (
    tag_t userTag,                                  /* I */ 
    const char *propertyName,                       /* I */ 
    char **propertyValue                            /* OF */
  )
{

    int   retcode     = ITK_ok;
    int   messageId   = 0;
    tag_t personTag   = NULLTAG;
    tag_t propertyTag = NULLTAG;
    tag_t classId     = NULLTAG;

    static int   sInit          = 0;
    static tag_t sPersonClassId = NULLTAG;
    static tag_t sUserClassId   = NULLTAG;

    const char* theFunction = "SBT_User_Properties/SBT_UP_ask_iMAN_user_person_property";
    TRACE_ON(0);
    ENTER(theFunction);

    if (sInit == 0)
    {
        ITK(POM_class_id_of_class("Person", &sPersonClassId));

        if (retcode == ITK_ok)
        {
            ITK(POM_class_id_of_class("User", &sUserClassId));
        }

        if (retcode == ITK_ok) {
            sInit = 1;
        }
    }

    *propertyValue = NULL;

    /* Get the class of instance 'userTag' */
    ITK(POM_class_of_instance(userTag, &classId));

    /* Check the class is either 'User' or 'Person' */
    if (retcode == ITK_ok)
    {
        if (classId == sUserClassId) { /* Get the person associated to the given user */
            ITK(SA_ask_user_person(userTag, &personTag));
        }
        else if (classId == sPersonClassId) {
            personTag = userTag;
        }
        else {
            retcode = POM_invalid_class_id;
        }
    }

    /* Get the identifier of the message to be sent to the registered method */
    if (retcode == ITK_ok)
    {
        ITK(METHOD_get_prop_message_id(PROP_ask_value_string_msg, propertyName, &messageId));
    }

    /* Get the tag person's instance property tag */
    if (retcode == ITK_ok)
    {
        ITK(PROP_ask_property_by_name(personTag, propertyName, &propertyTag));
    }

    /* Send the mesage to person's instance property to execute the registered method */
    if (retcode == ITK_ok)
    {
        ITK(METHOD_execute(propertyTag, messageId, propertyTag, propertyValue, NULL));
    }

    LEAVE(theFunction);
    return retcode;

} /* End SBT_UP_ask_iMAN_user_person_property */


/* End file SBT_User_Properties.c */
