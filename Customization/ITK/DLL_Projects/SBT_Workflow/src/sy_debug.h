/*==========================================================================

          Copyright (c) 2003 Electronic Data Systems Corporation
              Copyright (c) 2003  Unigraphics Solutions GmbH
                    Unpublished - All rights reserved
                   IMAN Professional Services, Germany

============================================================================*/

#ifndef _SY_DEBUG_H
#define _SY_DEBUG_H

/***************************************************************************

    IMAN Germany (VPD) Debugging functions 
	(developed from IPS_debug.h)
    ----------------------------------------------

    Create By     : Tim Williams
    Date          : 24/04/97

    Version       : 2.4
 
    Description   : General DEBUG options for ITK programs.  There are
		    functions to write debug messages either to the screen
		    or stdout, see below, and functions to turn debugging on
		    and off.

    Usages        :

	TRACE_ON
	--------
	  Turns on DEBUG.  It takes 1 argument which is a binary addition
	  integer to determine which debug features are turned on, where the
	  values for each feature are:-

	    FUNCTIONS	1  ( To log when entering and leaving functions )
	    TRACE	2  ( To log errors to std_err )
	    TIME	4  ( To log time to std_err )
	    SPY		8  ( To write messages to the screen )
	    TRACE_ITK  16  ( To log ITK calls to std error )

	  e.g.
	    TRACE_ON ( 3 )          turns on TRACE + FUNCTIONS
	    TRACE_ON ( DEBUG_MAX )  turns on ALL features


	TRACE_OFF
	---------
	  Turns all trace features off.


	TRACE  ( If TRACE on )
	-----
	  Write message to stdout, e.g.

	    TRACE(("Reading Preferences"));


	TRACE1  ( If TRACE on )
	------
	  Write message with 1 variable parameter to stdout, e.g.

	    TRACE1("pathname = [%s]\n", psz_pathname );


	TRACE2  ( If TRACE on )
	------
	  Write message with 2 variable parameters to stdout, e.g.

	    TRACE2(("filename = [%s/%s]\n", szPathName, szFileName ));


	TRACE3  ( If TRACE on )
	------
	  Write message with 3 variable parameters to stdout, e.g.


	TRACE4  ( If TRACE on )
	------
	  Write message with 4 variable parameters to stdout, e.g.


	SPY  ( If SPY on )
	---
	  Write message to screen, e.g.

	    SPY(("Separator character = [%s]\n", psz_separator));


	ENTER  ( If FUNCTIONS on )
	-----
	  Write "--> ENTER Function "Function_name"" to stdout, e.g.

	    ENTER(("Function_name"));


	LEAVE  ( If FUNCTIONS on )
	-----
	  Write "<-- LEAVE Function "Function_name"" to stdout, e.g.
	    LEAVE(("Function_name"));


	TIME  ( If TIME on )
	----
	  Write time to stdout, e.g.

	    TIME("STARTED");


Modifications:

Ver  Date      Name               Description of mod
=============================================================================

2.0  29/04/97  Tim Williams       Added TRACE1 and TRACE2 to take 1 and 2
                                  arguments.

2.1  20/05/97  Tim Williams    1) Initialised vars retcode and debug_level.
				  So can now send level to TRACE_ON as a
				  number only instead of needing a var.

                               2) Added TRACE3 and TRACE4.

                               3) ITK now prints "****" before call.

                               4) TRACE_ON now recognises env var DEBUG_MAX
				  to turn on all DEBUG options.

                               5) Added TRACE_ITK level to log ITK calls.
				  So TRACE no longer does this on its own!
				  TRACE_ITK turns on TRACE as a dependency.

2.2  30/05/97  Tim Williams    1) Added TIME function to write time to stdout.

2.3  05/06/97  Tim Williams    1) Modified TIME function to display elapsed
				  time since last call to TIME and total
				  time in seconds since last TRACE_ON.

2.4  29/09/97  John Hicks      1) Added LEAVE_ON_ERROR to display ITK error
				  as for ITK but uses return(ITK error)
				  to exit function. Calles LEAVE() to maintain
				  debug indentation.

                               2) Removed initialisation on debug_level, retcode
                                  and debug_indent to 0 as user_exits will not
                                  link if more than one object file has these set.

2.5	29/08/2000	Cemal Salman	1) Added TRACE5 to take 5 arguments
	12/12/2000	Cemal Salman	1) Added ERROR HANDLING MACROS used in user_part_no
	23/03/2001	CS				added macro MACHINE_TYPE
	14/08/2001	CS				moved ERROR HANDLING MACROS used in user_part_no to vpdg_user_part_no.h
								moved macros VPDG_SAFE_FREEx to vpdg_utils.h
								   

***************************************************************************/
#include <time.h>

#define DEBUG 1

#define DEBUG_FUNCTIONS  1
#define DEBUG_TRACE      2
#define DEBUG_TIME       4
#define DEBUG_SPY        8
#define DEBUG_TRACE_ITK 16

#define DEBUG_MAX    (DEBUG_FUNCTIONS + DEBUG_TRACE + DEBUG_TIME + DEBUG_SPY + DEBUG_TRACE_ITK)


//#define DEBUG_VARIABLES_DECLARE 


#if defined (WIN32) || defined (_WIN32)
  #define SEPARATOR_CHR      '\\'
  #define SEPARATOR_STR      "\\"
  #define SEPARATOR_STR_STR      "\\\\"
  #define DELETE_COMMAND "del "
  #define COPY_COMMAND "copy "
  #define MOVE_COMMAND "ren "
  #define MACHINE_TYPE	SS_WNT_MACHINE
#else
  #define SEPARATOR_CHR      '/'
  #define SEPARATOR_STR      "/"
  #define SEPARATOR_STR_STR      "/"
  #define DELETE_COMMAND "rm -f "
  #define COPY_COMMAND "cp "
  #define MOVE_COMMAND "mv "
  #define MACHINE_TYPE	SS_UNIX_MACHINE
#endif



#ifdef DEBUG_VARIABLES_DECLARE

unsigned debug_options;

int debug_level;

int debug_functions_flag; 
int debug_trace_flag; 
int debug_time_flag;
int debug_spy_flag; 
int debug_trace_itk_flag; 
int debug_indent; 
int debug_i;
int retcode;

time_t ttLastTime;
time_t ttStartTime;

#else

extern unsigned debug_options;

extern int debug_level;

extern int debug_functions_flag;
extern int debug_trace_flag;
extern int debug_time_flag;
extern int debug_spy_flag;
extern int debug_trace_itk_flag; 
extern int debug_indent;
extern int debug_i;
extern int retcode;

extern time_t ttLastTime;
extern time_t ttStartTime;

#endif





/*====================================*/
#if defined(DEBUG) || defined(_DEBUG)

/*
#define DEBUG_SET(options) \
*/
#define TRACE_ON(options) \
{ \
   int level; \
   \
   debug_options = 0; \
   debug_level = options; \
   level = debug_level; \
   time(&ttLastTime); \
   time(&ttStartTime); \
   \
   if (level >= DEBUG_TRACE_ITK) \
     { \
     debug_trace_itk_flag = 1; \
     debug_trace_flag = 1; \
     level -= DEBUG_TRACE_ITK; \
   } \
   \
   if (level >= DEBUG_SPY) \
     { \
     debug_spy_flag = 1; \
     level -= DEBUG_SPY; \
   } \
   \
   if (level >= DEBUG_TIME) \
     { \
     debug_time_flag = 1; \
     level -= DEBUG_TIME; \
   } \
   \
   if (level >= DEBUG_TRACE) \
     { \
     debug_trace_flag = 1; \
     level -= DEBUG_TRACE; \
   } \
   \
   if (level >= DEBUG_FUNCTIONS) \
     { \
     debug_functions_flag = 1; \
     level -= DEBUG_FUNCTIONS; \
   } \
   \
   /* Now check for environment vars */ \
   \
   if ( getenv("DEBUG_FUNCTIONS") ) \
      { debug_functions_flag = 1; debug_level+=DEBUG_FUNCTIONS;} \
   if ( getenv("DEBUG_TRACE")     ) \
      { debug_trace_flag = 1;     debug_level+=DEBUG_TRACE;} \
   if ( getenv("DEBUG_TIME")      ) \
      { debug_time_flag = 1;      debug_level+=DEBUG_TIME;} \
   if ( getenv("DEBUG_SPY")       ) \
      { debug_spy_flag = 1;       debug_level+=DEBUG_SPY;} \
   if ( getenv("DEBUG_TRACE_ITK") ) \
      { debug_trace_itk_flag = 1; debug_level+=DEBUG_TRACE_ITK; \
        debug_trace_flag = 1;     debug_level+=DEBUG_TRACE; \
      } \
  \
   if ( getenv("DEBUG_MAX")      ) \
      { \
		debug_functions_flag = 1; debug_level+=DEBUG_FUNCTIONS; \
        debug_trace_flag = 1;     debug_level+=DEBUG_TRACE; \
        debug_time_flag = 1;      debug_level+=DEBUG_TIME; \
        debug_spy_flag = 1;       debug_level+=DEBUG_SPY; \
        debug_trace_itk_flag = 1; debug_level+=DEBUG_TRACE_ITK; \
  	  } \
}

/*
#define TRACE_ON(flags) \
{ \
   if ( ((flags & DEBUG_FUNCTIONS) != 0)  && getenv("DEBUG_FUNCTIONS") ) debug_functions_flag = 1; \
   if ( ((flags & DEBUG_TRACE)     != 0)  && getenv("DEBUG_TRACE") )    debug_trace_flag = 1; \
   if ( ((flags & DEBUG_SPY)       != 0)  && getenv("DEBUG_SPY") )      debug_spy_flag = 1; \
   if ( ((flags & DEBUG_TIME)      != 0)  && getenv("DEBUG_TIME") )     debug_time_flag = 1; \
}
*/


#define TRACE_OFF() debug_functions_flag = debug_trace_flag = debug_spy_flag = debug_time_flag = 0;

#define TRACE(params) \
   if ( debug_trace_flag == 1 ) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf( "%s\n", params); \
   }

#define TRACE1(format, params) \
   if ( debug_trace_flag == 1 ) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf( format, params); \
   }

#define TRACE2(format, param1, param2) \
   if ( debug_trace_flag == 1 ) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf( format, param1, param2); \
   }

#define TRACE3(format, param1, param2, param3) \
   if ( debug_trace_flag == 1 ) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf( format, param1, param2, param3); \
   }

#define TRACE4(format, param1, param2, param3, param4) \
   if ( debug_trace_flag == 1 ) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf( format, param1, param2, param3, param4); \
   }

#define TRACE5(format, param1, param2, param3, param4, param5) \
   if ( debug_trace_flag == 1 ) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf( format, param1, param2, param3, param4, param5); \
   }

#define SPY(params) \
   if (debug_spy_flag == 1) \
   { \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      printf (params); \
   }

#define TIME(params) \
   if ( debug_time_flag == 1 ) \
   { \
      time_t tNow; \
      double dDiff1; \
      double dDiff2; \
      time(&tNow); \
      for ( debug_i=0; debug_i<debug_indent+1; debug_i++ ) \
         printf( "  " ); \
      dDiff1 = difftime(tNow, ttLastTime); \
      dDiff2 = difftime(tNow, ttStartTime); \
      printf( "%s [elapsed %1.0f, total %1.0f] %s\n", params, dDiff1, dDiff2, asctime(localtime(&tNow))); \
      ttLastTime = tNow; \
   }


#define ENTER(params) \
   if ( debug_functions_flag == 1 ) \
   { \
      debug_indent++; \
      printf("\n"); \
      for ( debug_i=0; debug_i<debug_indent; debug_i++ ) \
         printf("  "); \
      printf("--> ENTER Function \"%s()\"\n\n",params); \
   }



#define LEAVE(params) \
   if ( debug_functions_flag == 1 ) \
   { \
      printf("\n"); \
      for ( debug_i=0; debug_i<debug_indent; debug_i++ ) \
         printf( "  " ); \
      printf("<-- LEAVE Function \"%s()\"\n\n",params); \
      debug_indent--; \
   }



/*==========================================================================*/
/*                                                                          */
/* #define    : ITK                                                */
/*              ============                                                */
/*                                                                          */
/* Description: Check and display any errors returned by function calls     */
/*              and return its status.                                      */
/*                                                                          */
/* Parameters : Function =  Fuction call return status.                     */
/*                                                                          */
/* Returns    : Sets retcode to result of status call.                      */
/*                                                                          */
/*==========================================================================*/

#define ITK(Function)\
  {\
  int status;\
  char *err_str;\
  if (debug_trace_itk_flag)\
    TRACE1("  ****  %s\n", #Function);\
  if ((status = (Function)) != ITK_ok)\
    {\
    EMH_get_error_string (NULLTAG, status, &err_str);\
    printf( "ERROR: %d ERROR MSG:%s.\n", status, err_str);\
    printf( "Function: %s FILE: %s, LINE: %d\n", #Function, __FILE__, __LINE__);\
    MEM_free(err_str);\
  }\
  retcode = status;\
}

/*==========================================================================*/
/*                                                                          */
/* #define    : EXIT_ON_ERROR                                               */
/*              =============                                               */
/*                                                                          */
/* Description: Check and display any errors and exit on error.             */
/*                                                                          */
/* Parameters : Function =  Fuction call return status.                     */
/*                                                                          */
/*==========================================================================*/

#define EXIT_ON_ERROR(Function)\
  {\
  int status;\
  char *err_str;\
  if (debug_trace_itk_flag)\
    TRACE1("  ****  %s\n", #Function);\
  if ((status = (Function)) != ITK_ok)\
    {\
    EMH_get_error_string (NULLTAG, status, &err_str);\
    printf( "ERROR: %d ERROR MSG:%s.\n", status, err_str);\
    printf( "Function: %s FILE: %s, LINE: %d\n", #Function, __FILE__, __LINE__);\
    MEM_free(err_str);\
    printf("EXIT_ON_ERROR: Exiting...\n\n");\
    exit(0);\
  }\
}

#else  /* else not DEBUG */


#define ENTER(params)
#define LEAVE(params)
#define TRACE(params)
#define SPY(params)
#define TRACE_ON(flags)
#define TRACE_OFF()
#define TRACE(params) \
{}
#define TRACE1(format, params) \
{}
#define TRACE2(format, param1, param2) \
{}
#define TRACE3(format, param1, param2, param3) \
{}
#define TRACE4(format, param1, param2, param3, param4) \
{}
#define TRACE5(format, param1, param2, param3, param4, param5) \
{}

#define ITKCALL( argument ) \
{ \
    int i_should_never_be_used; \
    int i_should_never_be_used_retcode; \
    i_should_never_be_used_retcode = argument; \
    if ( i_should_never_be_used_retcode != ITK_ok ) \
    { \
      int i_should_never_be_used_2; \
      int i_error; \
      int i_num_errors; \
      int * api_severities; \
      int * api_error_codes; \
      char ** ppsz_errors; \
      printf( "ERROR: "#argument "\n" );  \
      printf( "  returns [%d]\n", i_should_never_be_used_retcode );\
      i_error = EMH_ask_errors ( &i_num_errors, &api_severities, &api_error_codes, &ppsz_errors ); \
      for ( i_should_never_be_used_2=0; i_should_never_be_used_2<i_num_errors; i_should_never_be_used_2++ ) \
      { \
        for ( i_should_never_be_used=0; i_should_never_be_used<debug_indent; i_should_never_be_used++ ) \
           printf( "  "); \
        printf( "    ERROR %d - %s\n", api_error_codes[i_should_never_be_used_2], ppsz_errors[i_should_never_be_used_2]); \
      } \
      printf( "  in file ["__FILE__"], line [%d]\n\n", __LINE__ );\
    } \
}

/*==========================================================================*/
/*                                                                          */
/* #define    : ITK                                                */
/*              ============                                                */
/*                                                                          */
/* Description: Check and display any errors returned by function calls     */
/*              and return its status.                                      */
/*                                                                          */
/* Parameters : Function =  Fuction call return status.                     */
/*                                                                          */
/* Returns    : Sets retcode to result of status call.                      */
/*                                                                          */
/*==========================================================================*/

#define ITK(Function)\
  {\
  retcode = (Function)\
}

#endif /* end if else DEBUG */

#endif


