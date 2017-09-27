/* -------------------------------------------------------------------------- */
/*      Modul:  norm14							*/
/*                                                                            */
/*      Entry:  norm14							*/
/*                                                                            */
/*      Aufgabe:        Sachnummer (SSU)                                      */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:                                                               */
/*                      SNI AP 626                                            */
/*                                                                            */
/*      Version:        1.0 vom 15.09.93                                      */
/*      Žnderung            am                                                                                 */
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


#ifdef __STDC__
/* Prototypen */
size_t norm14_auf(char *zw_feld,char *block1);
size_t norm14_fp(char *zw_feld,size_t *z_laenge);
size_t norm14_af1(norm_p1 *n02_p1,char *block1,size_t *z_laenge);
size_t norm14_afm(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm14_afd(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm14_afs(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm14_afb(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm14_normby(norm_p1 *n02_p1);
/* #include <ctype.h>
#include <string.h> */
#else
size_t norm14_auf();
size_t norm14_fp();
size_t norm14_af1();
size_t norm14_afm();
size_t norm14_afd();
size_t norm14_afs();
size_t norm14_afb();
size_t norm14_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14						*/
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      SSU                                                   */
/*                                                                            */
/*      Aufrufe:        norm14_fp       Fehlerpruefung                        */
/*                      norm14_all      Allgemeine Verarbeitung               */
/*                                      TNS mit ANA                           */
/*                      norm14_af1      Auswahl nach Steuerbyte               */
/*                      norm14_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm14(norm_p1 *n02_p1, char *zw_feld)
#else
size_t norm14(n02_p1, zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            zblock1_1[26];
	char            *block1=zblock1_1;
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	        *z_laenge   =  &laenge;

	if((ret_code = norm14_fp(zw_feld,z_laenge)) == NOK)
        {
                norm00_nb(n02_p1->normbyte,NB_NR);
                return(ret_code);
        }


	if((ret_code = norm14_auf(zw_feld,block1)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
								return(ret_code);
        }
	if((ret_code = norm14_af1(n02_p1,block1,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
                return(ret_code);
        }

      return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_fp                                             */
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
size_t norm14_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm14_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	   size_t ret_code=OK;
	   size_t       b;

	   b = strlen(zw_feld);
	   *z_laenge = b;
      return (ret_code);
}
/*----------------------------------------------*/
/* Aufbereiten der Nummer                   */
/*                                              */
/*----------------------------------------------*/
#ifdef __STDC__
size_t norm14_auf(char *zw_feld,char *block1)
#else
size_t norm14_auf(zw_feld,block1)
char *zw_feld;
char *block1;
#endif
{
     size_t     ret_code = OK;
     size_t i,j;

	 for (i=0;i<25; i++)
	 {
	 block1[i] = ' ';
	 }
	 block1[i] = '\0';
	i=0;
	j=0;
	while (zw_feld[i] != '\0')
	{
	    block1[j] = zw_feld[i];
			i++;j++;
	}

	for (; j<25; j++)
	{
	    block1[j] = ' ';
	}
	block1[j] = '\0';

     return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm11_afm      Aufbereitung MASCHINELL alt und neu               */
/*                      norm11_afd      Aufbereitung DRUCK                    */
/*                      norm11_afs      Aufbereitung SORTIERT alt und neu                */
/*                      norm11_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm14_af1(norm_p1 *n02_p1,char *block1,size_t *z_laenge)
#else
size_t norm14_af1(n02_p1,block1,z_laenge)
norm_p1 *n02_p1;
char *block1;
size_t *z_laenge;
#endif

{
				size_t       ret_code = NOK;
				/* Auswahl */

 
					norm14_afm(block1,n02_p1->m_norm,z_laenge);
					norm14_afd(block1,n02_p1->d_norm,z_laenge);
					norm14_afm(block1,n02_p1->s_norm,z_laenge);
					norm14_afb(block1,n02_p1->b_norm,z_laenge);
					norm14_afm(block1,n02_p1->ma_norm,z_laenge);
					norm14_afm(block1,n02_p1->sa_norm,z_laenge);
					ret_code = OK;


	norm14_normby(n02_p1);

        return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_afm                                            */
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
size_t norm14_afm(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm14_afm(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i;

		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		      aus_feld[i] = block1[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';
	
	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_afd                                            */
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
size_t norm14_afd(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm14_afd(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;



		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		      aus_feld[i] = block1[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';

return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_afs                                            */
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
size_t norm14_afs(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm14_afs(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;

		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		      aus_feld[i] = block1[i];
		   else
		      aus_feld[i] = ' ';
			 }
		   aus_feld[i]='\0';

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_afb                                            */
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
size_t norm14_afb(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm14_afb(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;

		for (i = 0; i < 25; i++)
		  {
		   if (i < *z_laenge)
		      aus_feld[i] = block1[i];
		   else
		      aus_feld[i] = ' ';
		   }
		   aus_feld[i]='\0';
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm14_normnb                                            */
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
size_t norm14_normby(norm_p1 *n02_p1)
#else
size_t norm14_normby(n02_p1)
norm_p1 *n02_p1;
#endif
{
	   size_t ret_code=OK;

				norm00_nb(n02_p1->normbyte,"SS");

	 return (ret_code);
}
