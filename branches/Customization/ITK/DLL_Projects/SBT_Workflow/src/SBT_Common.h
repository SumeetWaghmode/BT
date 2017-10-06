#ifndef SBT_COMMON_H
#define SBT_COMMON_H

#import <MSXML6.dll>

#include <ctype.h>
#include <atlbase.h>
#include <msxml2.h>
#include <cstdio>
#include <cfloat>
#include <cmath>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iterator>
#include <algorithm>
#include <sstream>  
#include <functional> 
#include <cctype>
#include <locale>
#include <hash_map>


/*
* namespaces
*/
// Removed namespace - This is must for autobuild and windows 2012 server machines. windows server 2012 does not support msxml
// header files.
using namespace std;
using std::string;
using std::vector;
using std::list;
using std::ofstream;
using std::ios;
using std::map;

#endif //SBT_COMMON_H





//#ifndef AM_BYPASS_H
//#define AM_BYPASS_H
//
//#include <sa/libsa_exports.h>
//
//#ifdef __cplusplus
//    extern "C"{
//#endif
//
//SA_API void AM__ask_application_bypass
//  (
//     logical *  has_bypass      /*<O>*/
//  );
//
//SA_API void AM__set_application_bypass
//  (
//     logical  bypass_on         /*<I>*/
//  );
//
//#ifdef __cplusplus
//}
//#endif
//#include <sa/libsa_undef.h>
//
//#endif /* AM_BYPASS_H */