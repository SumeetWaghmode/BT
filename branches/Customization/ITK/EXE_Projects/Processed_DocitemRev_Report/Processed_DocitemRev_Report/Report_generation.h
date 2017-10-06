//Include the required header files


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
#include <fclasses/tc_string.h>
#include <fclasses/tc_date.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tc/envelope.h>
#include <tccore/aom.h>
#include <tccore/project.h>
#include <tccore/aom_prop.h>
#include <property/nr.h>
#include <epm/epm.h>
#include <qry/qry.h>
#include <tccore/grm.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tccore/uom.h>
#include <user_exits\user_exits.h>
#include <tccore/grmtype.h>
#include <tccore/item.h>
#include <res/res_itk.h>
#include<epm\epm_toolkit_tc_utils.h>
#include<pom\enq\enq.h>
#include<pom\enq\enq_errors.h>
#include <sa/am_bypass.h>

#include <iostream>
#include <string>
#include<fstream>
#include <sstream>
#include <algorithm>
#include <map>

#define WRITE_ACCESS ("WRITE")
using namespace std;




int Loading_DataStorage_From_File();
int Loading_ProcessedUIDvalue_From_File();
int Loading_UnprocessedCombinations_From_File();
int Writing_UID_In_File(const char*,int&);
int& UnprocessedElementsincrementer();
int& ProcessedElementsincrementer();

int& keyincrementer();
int Loading_Combinations_From_File(const char* );
int Loading_ProcessedCombinations_From_File();
int Loading_UnprocessedCombinations_From_File();





