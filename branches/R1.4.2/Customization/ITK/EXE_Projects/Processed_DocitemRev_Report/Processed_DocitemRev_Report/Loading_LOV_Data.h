#include "Report_generation.h"


class Final_Data
{
	string identifier;
	string entdoc_type_old;
	string doctype_old;
	string docdossier_old;
	string documentClass_entdoc;
	string documentSubClass_doctype;
	string documentSNType_docdossier;
	string object_name;
	string bt7_Subtitle_1;
	string owning_Group;
	string owning_Site;
	string revisionlaststatus;
	string matching_status;
	string UIDvalue;
 

public:
	Final_Data();

	void setidentifier(const char*);
	void setentdoc_type_old(const char*);
	void setdoctype_old(const char*);
	void setdocdossier_old(const char*);
	void setdocumentClass_entdoc(const char*);
	void setdocumentSubClass_doctype(const char*);
	void setdocumentSNType_docdossier(const char*);
	void setobject_name(const char*);
	void setbt7_Subtitle_1(const char*);
	void setowning_Group(const char*);
	void setowning_Site(const char*);
	void setrevisionlaststatus(const char*);
	void setmatching_status(const char*);
	void setUIDvalue(const char*);

	string& getidentifier();
	string& getentdoc_type_old();
	string& getdoctype_old();
    string& getdocdossier_old();
	string& getdocumentClass_entdoc();
	string& getdocumentSubClass_doctype();
	string& getdocumentSNType_docdossier();
	string& getobject_name();
	string& getbt7_Subtitle_1();
	string& getowning_Group();
	string& getowning_Site();
	string& getrevisionlaststatus();
	string& getmatching_status();
	string& getUIDvalue();

   ~Final_Data();
};

class LOV_Data_Mapping
{
	string entdoc_type_old;
	string doctype_old;
	string docdossier_old;
	string documentClass_entdoc;
	string documentSubClass_doctype;
	string documentSNType_docdossier;

public:
	LOV_Data_Mapping();


	void setentdoc_type_old(string&);
	void setdoctype_old(string&);
	void setdocdossier_old(string&);
	void setdocumentClass_entdoc(string&);
	void setdocumentSubClass_doctype(string&);
	void setdocumentSNType_docdossier(string&);

	string& getentdoc_type_old();
	string& getdoctype_old();
    string& getdocdossier_old();
	string& getdocumentClass_entdoc();
	string& getdocumentSubClass_doctype();
	string& getdocumentSNType_docdossier();

   ~LOV_Data_Mapping();
};
 int& UIDincrementer();
