/* -------------------------------------------------------------------------- */
/*      Modul:  norm07                                                        */
/*                                                                            */
/*      Entry:  norm07                                                        */
/*                                                                            */
/*      Aufgabe:        VKN-Verkehrnummer                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Androleit                                             */
/*                      SNI AP 621                                            */
/*                                                                            */
/*      Version:        1.0 vom 29.05.92                                      */
/*      �nderung            am 05.10.01 Warnungen beseitigt  Androleit        */
/* -------------------------------------------------------------------------- */
/*      Allgemeine Definitionen                                               */
/* -------------------------------------------------------------------------- */
#define OK 0
#define NOK 1

/* -------------------------------------------------------------------------- */
/*      INCLUDE                                                               */
/* -------------------------------------------------------------------------- */
#include "norm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Prototypen */
#ifdef __STDC__
size_t norm07_fp(char *zw_feld, size_t *z_laenge);
size_t norm07_af1(norm_p1 *n02_p1,char *zw_feld,size_t *z_laenge);
size_t norm07_afm(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm07_afd(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm07_afs(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm07_afb(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm07_normby(norm_p1 *n02_p1);

/* #include <ctype.h>
#include <stdlib.h>
#include <string.h> */
#else
size_t norm07_fp();
size_t norm07_af1();
size_t norm07_afm();
size_t norm07_afd();
size_t norm07_afs();
size_t norm07_afb();
size_t norm07_normby();
#endif

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      VKN-Verkehrsnummer                                    */
/*                                                                            */
/*      Aufrufe:        norm07_fp       Fehlerpruefung                        */
/*                                      SNI Berechnen der Pr�fziffer          */
/*                                                                           */
/*                      norm07_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07(norm_p1 *n02_p1, char *zw_feld)
#else
size_t norm07(n02_p1, zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            auf_feld[26];
	char    	*auf_feld_1 = auf_feld;
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	        *z_laenge   =  &laenge;
	size_t             i;
	for (i=0;i<25;i++)
	{
	  auf_feld_1[i]=zw_feld[i];
	}

	if((ret_code = norm07_fp(zw_feld,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}
	if((ret_code = norm07_af1(n02_p1,zw_feld,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
								return(ret_code);
				}

      return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_fp                                             */
/*                                                                            */
/*      Aufgabe:        Fehlerpruefung                                        */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_fp(char *zw_feld, size_t *z_laenge)
#else
size_t norm07_fp(zw_feld, z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
   size_t ret_code=NOK;
   char *auf_feld_1=zw_feld;
   size_t       i,k,ab;
   size_t       a[14];   /*    krachts 'ba21.02.2003 arno m��te 13 sein */
   char      feld0[27];
   char      *feld0_1=feld0;
   char      vergleich[2];
   char      *vergleich1=vergleich;
   ab = strlen(zw_feld);
   *z_laenge = ab;

	   k=0;
/* F�r die Umwandlung eines 13-stelligen Stringes werden die Werte aus
   dem �bergabebereich in einen 26-stelligen Bereich mit einem '\0' zwischen
   jedem numerischen Zeichen gestellt. Dies wird f�r die Umwandlung von String
   in einen size_teger-Wert ben�tigt. */

	   for (i=0;i<14;i++)
	     {
		feld0_1[k]=auf_feld_1[i];
	     
		k++;
		feld0_1[k]='\0';
		k++;
	    }
	    k=2;
/* hier erfolgt die Belegung der Zeiger f�r atoi
   atoi nimmt den Wert bis zum '\0'-Zeichen */
 /*    krachts ba21.02.2003 arno m��te 13 sein */
	for (i=0;i<14;i++)                           
    {
	      a[i] = atoi(feld0_1);  
	      *feld0_1=feld0[k];  
	      k=k+2;
    }

     if  (*z_laenge > 13)   
    {
       ret_code = NOK;
	}
    else  
    {
				ret_code = OK;
    }
/*       zwi_summe = a[0]*10+a[1]*4+a[2]*9+a[3]*3+a[4]*7+a[11]*5+a[12]*12;
       i=0;
       diff=(zwi_summe-((zwi_summe/10)*10));
       *vergleich=auf_feld_1[9];
       vergleich[1] = '\0';
			 ac = atoi(vergleich);
       if (diff == ac)
	  {
	  ret_code = OK;
	  }
	  else
		{
		ret_code = NOK;
		}     */
	   
		vergleich1=vergleich1; /* vermeidet Warnings */
		return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm07_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm07_afd      Aufbereitung DRUCK                    */
/*                      norm07_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm07_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_af1(norm_p1 *n02_p1,char *zw_feld,size_t *z_laenge)
#else
size_t norm07_af1(n02_p1,zw_feld,z_laenge)
norm_p1 *n02_p1;
char *zw_feld;
size_t *z_laenge;
#endif
{
				size_t       ret_code = NOK;

				/* Auswahl */

					norm07_afm(zw_feld,n02_p1->m_norm,z_laenge);
					norm07_afd(zw_feld,n02_p1->d_norm,z_laenge);
					norm07_afm(zw_feld,n02_p1->s_norm,z_laenge);
					norm07_afb(zw_feld,n02_p1->b_norm,z_laenge);
					norm07_afm(zw_feld,n02_p1->ma_norm,z_laenge);
					norm07_afm(zw_feld,n02_p1->sa_norm,z_laenge); 
					ret_code = OK;


	norm07_normby(n02_p1);

				return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_afm                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung MASCHINELL                               */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_afm(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm07_afm(zw_feld,aus_feld,z_laenge)
char *zw_feld;
char *aus_feld;
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i;


		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		      if (i < 13)
		      {
		      aus_feld[i] = zw_feld[i];
		      }
		      else
					{
		      aus_feld[i] = ' ';
		      }

		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';

		
	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_afd                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung DRUCK                                    */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_afd(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm07_afd(zw_feld,aus_feld,z_laenge)
char *zw_feld;
char *aus_feld;
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;

		for (i = 0; i < 25; i++)
			{
			 if (i < *z_laenge)
				 if (i < 13)
		     {
		      aus_feld[i] = zw_feld[i];
		     }
		     else
		     {
				 aus_feld[i] = ' ';
		     } 
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';


 return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_afs                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SORTIERT                                 */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_afs(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm07_afs(zw_feld,aus_feld,z_laenge)
char *zw_feld;
char *aus_feld;
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;

		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		     if (i < 13)
		     {
		      aus_feld[i] = zw_feld[i];
		     }
		     else
		     {
				 aus_feld[i] = ' ';
		     }
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_afb                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung BARCODE                                  */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_afb(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm07_afb(zw_feld,aus_feld,z_laenge)
char *zw_feld;
char *aus_feld;
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;

		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		     if (i < 13)
		     {
		      aus_feld[i] = zw_feld[i];
		     }
		     else
		     {
				 aus_feld[i] = ' ';
		     }
		   else
		      aus_feld[i] = ' ';
		   }
		      aus_feld[i]='\0';
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm07_normnb                                            */
/*                                                                            */
/*      Aufgabe:        Normbyte_Belegung                                      */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm07_normby(norm_p1 *n02_p1)
#else
size_t norm07_normby(n02_p1)
norm_p1 *n02_p1;
#endif
{
	   size_t ret_code=OK;

	   norm00_nb(n02_p1->normbyte,"WV");

	 return (ret_code);
}
