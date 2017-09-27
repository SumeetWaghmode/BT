
int SBT_AH_assign_TM_id (
    EPM_action_message_t msg);

EPM_decision_t SBT_RH_checkRevsInProc ( 
	EPM_rule_message_t	msg	) ;	/* (I) */

EPM_decision_t SBT_RH_checkOwningGroup ( 
	EPM_rule_message_t	msg	) ;	/* (I) */


logical bObjInstanceOf (
    const tag_t objTag,       // (I)
    const char *sClassName);  // (I)

