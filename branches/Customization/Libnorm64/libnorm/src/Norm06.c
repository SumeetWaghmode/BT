/* -------------------------------------------------------------------------- */
/*      Modul:  norm06                                                        */
/*                                                                            */
/*      Entry:  norm06                                                        */
/*                                                                            */
/*      Aufgabe:        EAN - Verarbeitung                                    */
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
/*      Version:        1.0 vom 14.02.92                                      */
/*      Aenderung                                                             */
/* -------------------------------------------------------------------------- */
/*      Allgemeine Definitionen                                               */
/* -------------------------------------------------------------------------- */
#define OK 0
#define NOK 1

#include "norm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/* -------------------------------------------------------------------------- */
/*      INCLUDE                                                               */
/* -------------------------------------------------------------------------- */

#ifdef __STDC__
/* Prototypen */
size_t norm06_fp(char *zw_feld, size_t *z_laenge);
size_t norm06_af1(norm_p1 *n02_p1, char *zw_feld,size_t *z_laenge);
size_t norm06_afm(char *zw_feld, char *aus_feld,size_t *z_laenge);
size_t norm06_afd(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm06_afs(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm06_afb(char *zw_feld,char *aus_feld,size_t *z_laenge);
size_t norm06_normby(norm_p1 *n02_p1);
/* #include <ctype.h>
#include <string.h>
#include <stdlib.h>   */
#else
size_t norm06_fp();
size_t norm06_af1();
size_t norm06_afm();
size_t norm06_afd();
size_t norm06_afs();
size_t norm06_afb();
size_t norm06_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      TNS-Nummer                                            */
/*                                                                            */
/*      Aufrufe:        norm06_fp       Fehlerpruefung                        */
/*                                      EAN Berechnen der PrÅfziffer          */
/*                                                                           */
/*                      norm06_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm06(norm_p1 *n02_p1, char *zw_feld)
#else
size_t norm06(n02_p1, zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            auf_feld[25];
	char    	*auf_feld_1 = auf_feld;
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	        *z_laenge   =  &laenge;
	size_t             i;
	for (i=0;i<25;i++)
	{
	  auf_feld_1[i]=zw_feld[i];
	}

	if((ret_code = norm06_fp(zw_feld,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}
	if((ret_code = norm06_af1(n02_p1,zw_feld,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
								return(ret_code);
				}

			return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06_fp                                             */
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
size_t norm06_fp(char *zw_feld, size_t *z_laenge)
#else
size_t norm06_fp(zw_feld, z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	   size_t ret_code=NOK;
	   char *auf_feld_1=zw_feld;
	   size_t       i,k,ab;
	   size_t       a[14];
	   size_t    summe=0;
	   size_t       teiler=10;
	   size_t       pruef=0;
	   size_t       rest=0;
	   size_t       diff=0;     
	   size_t       zwi_summe=0;
	   char      feld0[31];
	   char      *feld0_1=feld0;
	   ab = strlen(zw_feld);
	   *z_laenge = ab;
		
	   k=0;
/* FÅr die Umwandlung eines 13-stelligen Stringes werden die Werte aus
   dem öbergabebereich in einen 26-stelligen Bereich mit einem '\0' zwischen
   jedem numerischen Zeichen gestellt. Dies wird fÅr die Umwandlung von String
   in einen size_teger-Wert benîtigt. */

	   for (i=0;i<14;i++)
       {
		 feld0_1[k]=auf_feld_1[i];
		 k++;
		 feld0_1[k]='\0';
		 k++;
       }
	   k=0; 
/* hier erfolgt die Belegung der Zeiger fÅr atoi
	 atoi nimmt den Wert bis zum '\0'-Zeichen */

	   if  (*z_laenge > 13)
	   {
	       ret_code = NOK;
	   }
	   else
	   {
			 ret_code = OK;

	   for (i=0;i<14;i++)
       {
	      a[i] = atoi(feld0_1);
          k=k+2;
	      if (k<28) 
		  {
		  *feld0_1=feld0[k]; 
		  }
	    } 

         zwi_summe = a[0]+a[1]*3+a[2]+a[3]*3+a[4]+a[5]*3+a[6]+a[7]*3+a[8]+
	     a[9]*3+a[10]+a[11]*3;
         summe=zwi_summe/teiler;
	     rest=zwi_summe%teiler;
         diff = (10 - rest);

      if ((diff == 10) && (rest == 0))
	  {
		  diff = 0;
	  }

      if (diff == a[12])
		{
			ret_code = OK;
		}
		else
		{
			ret_code = NOK;
		}
		 }
	 pruef=pruef; /* vermeidet Warnings*/
     summe=summe; /* vermeidet Warnings */
     return(ret_code);
}                             

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm06_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm06_afd      Aufbereitung DRUCK                    */
/*                      norm06_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm06_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm06_af1(norm_p1 *n02_p1, char *zw_feld,size_t *z_laenge)
#else
size_t norm06_af1(n02_p1, zw_feld, z_laenge)
norm_p1 *n02_p1;
char *zw_feld;
size_t *z_laenge;
#endif
{
				size_t       ret_code = NOK;

				/* Auswahl */

					norm06_afm(zw_feld,n02_p1->m_norm,z_laenge);
					norm06_afd(zw_feld,n02_p1->d_norm,z_laenge);
					norm06_afm(zw_feld,n02_p1->s_norm,z_laenge);
					norm06_afb(zw_feld,n02_p1->b_norm,z_laenge);
					norm06_afm(zw_feld,n02_p1->ma_norm,z_laenge);
					norm06_afm(zw_feld,n02_p1->sa_norm,z_laenge);
					ret_code = OK;


	norm06_normby(n02_p1);

				return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06_afm                                            */
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
size_t norm06_afm(char *zw_feld, char *aus_feld,size_t *z_laenge)
#else
size_t norm06_afm(zw_feld, aus_feld, z_laenge)
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
		      aus_feld[i] = zw_feld[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';

		
	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06_afd                                            */
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
size_t norm06_afd(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm06_afd(zw_feld, aus_feld, z_laenge)
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
		      aus_feld[i] = zw_feld[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';


 return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06_afs                                            */
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
size_t norm06_afs(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm06_afs(zw_feld, aus_feld, z_laenge)
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
		      aus_feld[i] = zw_feld[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm06_afb                                            */
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
size_t norm06_afb(char *zw_feld,char *aus_feld,size_t *z_laenge)
#else
size_t norm06_afb(zw_feld, aus_feld, z_laenge)
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
		      aus_feld[i] = zw_feld[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_normnb                                            */
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
size_t norm06_normby(norm_p1 *n02_p1)
#else
size_t norm06_normby(n02_p1)
norm_p1 *n02_p1;
#endif
{
		 size_t ret_code=OK;

	   norm00_nb(n02_p1->normbyte,"EA");

	 return (ret_code);
}
