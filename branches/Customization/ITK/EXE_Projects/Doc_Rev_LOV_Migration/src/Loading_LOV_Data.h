#include "Doc_Rev_LOV_Migration.h"


class LOV_Data_Mapping
{
	string primary_UID;
	string documentClass_entdoc;
	string documentSubClass_doctype;
	string documentSNType_docdossier;

public:
	LOV_Data_Mapping();

	void setprimary_UID(string&);
	void setdocumentClass_entdoc(string&);
	void setdocumentSubClass_doctype(string&);
	void setdocumentSNType_docdossier(string&);

	string& getprimary_UID();
	string& getdocumentClass_entdoc();
	string& getdocumentSubClass_doctype();
	string& getdocumentSNType_docdossier();

   ~LOV_Data_Mapping();
};

/* Functions Declarations */
int& keyincrementerUID();
int& keyincrementer();
int DoucmentRevisionCount(int *,tag_t**);
int FreeingLOVDatamemory();
string& CheckProcessedUIDValue(string&,tag_t&);
int ReadingTableFromDatabase(int *,char **,char* );
int ProcessingloadedElements();
string& ReplaceCommaInattribute(string& );
