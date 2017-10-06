
int SBT_AH_multiselect_effectivity (
    EPM_action_message_t msg
	);

static int Process(
	int in_nNumberOfRevs, 
	tag_t * in_ptagRevisions, 
	char* in_pcharPreferenceName, 
	date_t *in_pdateStart, 
	date_t *in_pdateEnd
);

static int CdgLogger_log_error_stack ();
static int LOG (const char* text, ...);
static int InitDate(date_t *in_date_t, short year, byte month, byte day);
