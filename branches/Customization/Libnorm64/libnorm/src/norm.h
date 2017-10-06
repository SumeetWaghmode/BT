/* -------------------------------------------------------------------------- */
/*      INCLUDE norm.h                                                        */
/*                                                                            */
/*      Inhalt:         Allgemeine Definitionen und                           */
/*                      TYPEDEF fuer Parameteruebergabe an NORM-Modul         */
/*                                                                            */
/*                                                                            */
/*      Author:         Baller												  */
/*                                                                            */
/*                                                                            */
/*      Version:        3.10A vom 21.07.2006                                  */
/*                                                                            */
/* -------------------------------------------------------------------------- */
 
/* -------------------------------------------------------------------------- */
/*      Allgemeine Definitionen                                               */
/* -------------------------------------------------------------------------- */
#define MLFB_MAX 41
 
#define NB_F9 "F9"
#define NB_IZ "IZ"
#define NB_NR "X "
#define N_FE 'P'

/*04.02.2008, Nana Beseka: Max. Länge für TNS */
#define MAXLEN1 14
#define MAXLEN2 18
/******************/

#define KLEINAE 'ä'
#define KLEINOE 'ö'
#define KLEINUE 'ü'
#define GROSSAE 'Ä'
#define GROSSOE 'Ö'
#define GROSSUE 'Ü'

#ifdef VMS
   #include <ctype.h>
#endif
typedef struct norm_pr1
                {
                char    eingfeld[32];
                char    steubyte[3];
                char    m_norm[26];
                char    ma_norm[26];
                char    d_norm[26];
                char    s_norm[26];
                char    sa_norm[26];
                char    b_norm[26];
                char    normbyte[3];
                } norm_p1;


typedef struct norm_pr2_tab
                {
                char    mlfb_art[2];
                char    mlfb_txt[32];
                } norm_p2_tab;


typedef struct norm_pr2
                {
                norm_p2_tab zeile[41];
                char    mlfb_feh[8];
                } norm_p2;

// __declspec( dllexport ) size_t cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);

/* exportierte Methode     11.09.97 */
/* Funktionsdeklarationen  */
#ifdef WIN32
#define DLLFAR __declspec( dllexport ) __stdcall
#endif

#ifdef _Windows
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#endif

/* Prototypen */
#if defined __STDC__
	#ifndef _PASCAL
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <ctype.h>
	#endif
	size_t       norm00_nb(char *normbyte,char *nb_wert);

	#ifdef __cplusplus
	extern "C"
	{
	#endif
	#ifdef WIN32
		/* __declspec( dllexport ) size_t DLLFAR cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2); */
		size_t DLLFAR cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);
	#else
		#ifdef _Windows
			/* size_t _export FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2); */
		/*	size_t __declspec( dllexport ) FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2); /*NNB, Sept. 2007 Declaration changed */
			__declspec( dllexport ) size_t FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2); /*NNB, Sept. 2007 Declaration changed */
		#else
			size_t cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);
		#endif
	#endif
	#ifdef __cplusplus
	}
	#endif

	size_t norm01(norm_p1 *n01_p1, char *zw_feld);
	size_t norm02(norm_p1 *n02_p1, char *zw_feld);
	size_t norm03(norm_p1 *n03_p1, char *zw_feld, norm_p2 *n03_p2);
	size_t norm04(norm_p1 *n02_p1, char *zw_feld);
	size_t norm05(norm_p1 *n02_p1, char *zw_feld);
	size_t norm051(norm_p1 *n02_p1, char *zw_feld);	/*04.02.2008, Njomo Nana Beseka: Erweiterung für TNS Max. Länge = 18 */
	size_t norm06(norm_p1 *n02_p1, char *zw_feld);
	size_t norm07(norm_p1 *n02_p1, char *zw_feld);
	size_t norm08(norm_p1 *n02_p1, char *zw_feld);
	size_t norm09(norm_p1 *n02_p1, char *zw_feld);
	size_t norm10(norm_p1 *n02_p1, char *zw_feld);
	size_t norm11(norm_p1 *n02_p1, char *zw_feld);
	size_t norm12(norm_p1 *n02_p1, char *zw_feld);
	size_t norm13(norm_p1 *n02_p1, char *zw_feld);
	size_t norm14(norm_p1 *n02_p1, char *zw_feld);
	size_t norm15(norm_p1 *n02_p1, char *zw_feld);   /*Ba19.12.2002*/
	#ifdef _PASCAL
		extern int strlen(char *s);
		extern char *strcpy(char *s1,char *s2);
		extern int strcmp(char *s1,char *s2);
		extern int ispunct(char c);
		extern long atol(char s[]);
		extern int atoi(char *s);
		extern char *itoa(size_t i,char *s);
		extern int isdigit(char c);
		extern int isalpha(char c);
		extern int isalnum(char c);
		extern int islower(char c);
		extern char toupper(char c);
	#endif
	extern size_t  norm00_nb(char *s1,char *s2);
	
#else
	#if defined __BORLANDC__
		#ifndef _PASCAL
		#include <stdlib.h>
		#include <stdio.h>
		#include <string.h>
		#include <ctype.h>
		#endif
		size_t       norm00_nb(char *normbyte,char *nb_wert);
		#ifdef _Windows
			#ifdef __cplusplus
			extern "C"
			{
			#endif
				__declspec( dllexport )size_t FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);
			#ifdef __cplusplus
			}
			#endif
			/*size_t FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);*/
		#else
			size_t cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);
		#endif

		size_t norm01(norm_p1 *n01_p1, char *zw_feld);
		size_t norm02(norm_p1 *n02_p1, char *zw_feld);
		size_t norm03(norm_p1 *n03_p1, char *zw_feld, norm_p2 *n03_p2);
		size_t norm04(norm_p1 *n02_p1, char *zw_feld);
		size_t norm05(norm_p1 *n02_p1, char *zw_feld);
		size_t norm051(norm_p1 *n02_p1, char *zw_feld);	/*04.02.2008, Njomo Nana Beseka: Erweiterung für TNS Max. Länge = 18 */
		size_t norm06(norm_p1 *n02_p1, char *zw_feld);
		size_t norm07(norm_p1 *n02_p1, char *zw_feld);
		size_t norm08(norm_p1 *n02_p1, char *zw_feld);
		size_t norm09(norm_p1 *n02_p1, char *zw_feld);
		size_t norm10(norm_p1 *n02_p1, char *zw_feld);
		size_t norm11(norm_p1 *n02_p1, char *zw_feld);
		size_t norm12(norm_p1 *n02_p1, char *zw_feld);
		size_t norm13(norm_p1 *n02_p1, char *zw_feld);
		size_t norm14(norm_p1 *n02_p1, char *zw_feld);
		size_t norm15(norm_p1 *n02_p1, char *zw_feld);   /*Ba19.12.2002*/
		#ifdef _PASCAL
		extern size_t strlen(char *s);
		extern char *strcpy(char *s1,char *s2);
		extern size_t strcmp(char *s1,char *s2);
		extern size_t ispunct(char c);
		extern long atol(char s[]);
		extern size_t atoi(char *s);
		extern char *itoa(size_t i,char *s);
		extern size_t isdigit(char c);
		extern size_t isalpha(char c);
		extern size_t isalnum(char c);
		extern size_t islower(char c);
		extern char toupper(char c);
		#endif
		extern size_t  norm00_nb(char *s1,char *s2);
	#else
		#ifdef _PASCAL
		extern size_t strlen();
		extern char *strcpy();
		extern size_t strcmp();
		extern size_t ispunct();
		extern long atol(char s[]);
		extern size_t atoi();
		extern char *itoa();
		extern size_t isdigit();
		extern size_t isalpha();
		extern size_t isalnum();
		extern size_t islower();
		extern char toupper();
		#endif

		extern size_t  norm00_nb();
		#ifdef _Windows
			#ifdef __cplusplus
			extern "C"
			{
			#endif
				size_t FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);
			#ifdef __cplusplus
			}
			#endif
		#endif

		#ifdef _WIN64
			//__declspec( dllexport ) size_t cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);		
			size_t DLLFAR cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2);		
		#else
			size_t cnorm();
		#endif

		size_t norm01();
		size_t norm02();
		size_t norm03();
		size_t norm04();
		size_t norm05();
		size_t norm051();	/*04.02.2008, Njomo Nana Beseka: Erweiterung für TNS Max. Länge = 18 */
		size_t norm06();
		size_t norm07();
		size_t norm08();
		size_t norm09();
		size_t norm10();
		size_t norm11();
		size_t norm12();
		size_t norm13();
		size_t norm14();
		size_t norm15();

	#endif
#endif

#ifdef WIN32
#define KLEINAE 'ä'
#define KLEINOE 'ö'
#define KLEINUE 'ü'
#define GROSSAE 'Ä'
#define GROSSOE 'Ö'
#define GROSSUE 'Ü'
#endif

#ifdef SINIX
#define KLEINAE 0173
#define KLEINOE 0174
#define KLEINUE 0175
#define GROSSAE 0133
#define GROSSOE 0134
#define GROSSUE 0135
#endif

#ifdef BS2000
#define KLEINAE '\373'
#define KLEINOE '\117'
#define KLEINUE '\375'
#define GROSSAE '\273'
#define GROSSOE '\274'
#define GROSSUE '\275'
#endif

#ifdef MVS
#define KLEINAE 0300
#define KLEINOE 0152
#define KLEINUE 0320
#define GROSSAE 0112
#define GROSSOE 0340
#define GROSSUE 0132
#endif

#ifdef VMS
#define KLEINAE 0xE4
#define KLEINOE 0xF6
#define KLEINUE 0xFC
#define GROSSAE 0xC4
#define GROSSOE 0xD6
#define GROSSUE 0cDC
#endif

#ifdef HP
#define KLEINAE 0300
#define KLEINOE 0152
#define KLEINUE 0320
#define GROSSAE 0112
#define GROSSOE 0340
#define GROSSUE 0132
#endif

#ifdef SUN
#define KLEINAE '\344'
#define KLEINOE '\366'
#define KLEINUE '\374'
#define GROSSAE '\304'
#define GROSSOE '\326'
#define GROSSUE '\334'
#endif

#ifdef WS30
#define KLEINAE '\344'
#define KLEINOE '\366'
#define KLEINUE '\374'
#define GROSSAE '\304'
#define GROSSOE '\326'
#define GROSSUE '\334'
#endif

#ifdef AS400
#define KLEINAE 0xE4
#define KLEINOE 0xF6
#define KLEINUE 0xFC
#define GROSSAE 0xC4
#define GROSSOE 0xD6
#define GROSSUE 0cDC
#endif
