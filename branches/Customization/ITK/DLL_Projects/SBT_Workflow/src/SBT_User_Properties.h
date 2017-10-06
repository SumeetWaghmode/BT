/**
 * This file holds the declaration of the TCE user properties.
 */
#ifndef _SBT_USER_PROPERTIES_H
#define _SBT_USER_PROPERTIES_H

#include <tc/iman.h>


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
  tag_t iMANUserTag,               /* I */ 
  char **iMANUserEmailAddress           /* OF */
 );

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
  tag_t userTag,                 /* I */ 
  const char *propertyName,            /* I */ 
  char **propertyValue              /* OF */
 );

#endif

/* End file SBT_User_Properties.h */
