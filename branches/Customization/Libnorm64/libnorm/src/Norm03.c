/* -------------------------------------------------------------------------- */
/*      Modul:  norm03                                                        */
/*                                                                            */
/*      Entry:  norm03                                                        */
/*                                                                            */
/*      Aufgabe:        Aufbereitung MLFB                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Baller Arno                             */
/*                                                                */
/*                                                                            */
/*      Version:        1.0 vom 10.05.90                                      */
/*                      2.0 vom 11.01.2000                                    */
/*                      3.0 vom Juni 2001                                     */  
/*  ƒnderungen Version 3.0                                                    */
/*                                                                            */
/*            - eine MLFB besteht mind. aus Block 1 (Belegung Datenstelle 1-6 */
/*            - '9' oder 'Z' innerhalb der MLFB als Kennzeichen f¸r "Anormale */  
/*              Technische Grundangaben" und Kennzeichen "-Z" f¸r "Besondere  */
/*              Ausf¸hrungen"                                                 */
/*            - als Rumpf-MLFB wird eine Nummer erkannt, die ab Datenstelle 5 */
/*              an Buchstaben- oder Ziffernstellen einen oder mehrere Punkte  */
/*              Punkte f¸hrt.                                                 */
/*            - MLFB mit 4 oder 5 Stellen werden f¸r besondere Anwendungen    */
/*              benˆtigt und mit dem Normbyte "FX" gekennzeichnet. Diese MLFBS*/
/*              gelten nicht im Sinner einer Erzeugnisnummer und d¸rfen daher */
/*              f¸r diesen Zweck nicht angewandt werden. Sie sind an der 5.St.*/
/*              numerisch belegt, Kennzeichnungen f¸r "Besondere Ausf¸hrungen"*/
/*              und "Rumpf-MLFB" sind nicht vorgesehen.                       */
/*						3.20 Pr¸fung 4.Stelle AUF 0 RAUS BA2006.10.15         */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/*      Allgemeine Definitionen                                               */
/* -------------------------------------------------------------------------- */
#define OK 0
#define NOK 1
#define OK_ENDE 3

/* -------------------------------------------------------------------------- */
/*      INCLUDE                                                               */
/* -------------------------------------------------------------------------- */
#include "norm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __STDC__
#include <ctype.h>
#include <string.h>
#include <stdio.h>  
/* Prototypen*/
size_t norm03_feh(char *mlfb_feh,size_t feh_wert1,size_t feh_wert2);
size_t norm03_fp(char *zw_feld);
size_t norm03_auf(char *zw_feld);
size_t norm03_all(char *zw_feld);
size_t norm03_af1(norm_p1 *n03_p1);
size_t norm03_text(norm_p1 *n03_p1,norm_p2 *n03_p2);
size_t norm03_afm(char *aus_feld);
size_t norm03_afd(char *aus_feld);
size_t norm03_afs(char *aus_feld);
size_t norm03_afb(char *aus_feld);
size_t norm03_normby(norm_p1 *n03_p1);
size_t norm03_format(char *mlfb_txt,char *mlfb_feh);
size_t norm03_tabelle(char *mlfb_txt,char *mlfb_feh,char *wert_tab);
size_t norm03_tab_fuell(char *mlfb_feh,char *wert_tab,size_t wert);
#else
size_t norm03_feh();
size_t norm03_fp();
size_t norm03_auf();
size_t norm03_all();
size_t norm03_af1();
size_t norm03_text();
size_t norm03_afm();
size_t norm03_afd();
size_t norm03_afs();
size_t norm03_afb();
size_t norm03_normby();
size_t norm03_format();
size_t norm03_tabelle();
size_t norm03_tab_fuell();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      MLFB                                                  */
/*                                                                            */
/*      Aufrufe:        norm03_af1      Pruefen 1. Stelle                     */
/*                      norm03_af2      Umsetzen "O" und "I"                  */
/*                      norm03_af3      Pruefen 5. und 6. Stelle              */
/*                      norm03_af4      Pruefen 7. Stelle                     */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
char zw_ausf[19];
size_t anz_ezeilen=0;
size_t	anz_notw_ezeilen=0;
size_t anz_vory=0;
size_t anz_vor_oy=0;
size_t anz_kurz=0;
size_t	z_vorhanden=0;
size_t	mit_fl_z=0;
size_t notw_mit_fl_z=0;
size_t mlfb_ok=0;
size_t anz_9=0;
size_t anz_9z=0;
#ifdef __STDC__
size_t norm03(norm_p1 *n03_p1, char *zw_feld, norm_p2 n03_p2[])
#else
size_t norm03(n03_p1, zw_feld, n03_p2)
norm_p1 *n03_p1;
char *zw_feld;
norm_p2 n03_p2[];
#endif
{
	size_t       ret_code = NOK;
	size_t             i=0;


	/***************************************/
	/*  Fehlernummer mit NULL vorbelegen   */
	/***************************************/

	for (i=0; i< 7;i++)
	{
		n03_p2->mlfb_feh[i] = '0';
	}
	n03_p2->mlfb_feh[i] = '\0';

	/* ------------------------------------------------------------- 	*/
	/* Fehlerpruefungen                                               */
	/* ------------------------------------------------------------- 	*/
	if((ret_code = norm03_fp(zw_feld)) == NOK)
	{
			norm00_nb(n03_p1->normbyte,NB_NR);
					return(ret_code);
	}
	/* ------------------------------------------------------------- 	*/
	/* Aufbereiten MLFB                                               */
	/* ------------------------------------------------------------- 	*/
	if((ret_code = norm03_auf(zw_feld)) == NOK)
	{
		norm00_nb(n03_p1->normbyte,NB_NR);
                return(ret_code);
	}
	/* ------------------------------------------------------------- 	*/
	/* Allgemeine Verarbeitung                                        */
   /* ------------------------------------------------------------- */
	if((ret_code = norm03_all(zw_feld)) == NOK)
   {
		norm00_nb(n03_p1->normbyte,NB_NR);
                return(ret_code);
	}

	/* --------------------------------------------------------------- */
	/* Auswahl nach Steuerbyte                                         */
   /* ---------------------------------------------------------------*/
	if((ret_code = norm03_af1(n03_p1)) == NOK)
   {
		norm00_nb(n03_p1->normbyte,NB_NR);
                return(ret_code);
	}

	/* --------------------------------------------------------------- */
	/* Verarbeitung der zusÑtzlichen Textangaben                       */
	/* --------------------------------------------------------------- */

    norm03_text(n03_p1,n03_p2);

	return(ret_code);
}

/* ------------------------------------------------------------------------- */
/*      Funktion:       norm03_fp                                            */
/*                                                                            */
/*      Aufgabe:        Fehlerpruefungen                                      */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_fp(char *zw_feld)
#else
size_t norm03_fp(zw_feld)
char *zw_feld;
#endif
{
	size_t      ret_code = OK;
	size_t      i;
	char		c;

/*      Pruefen 1. Stelle               */

		if(zw_feld[0] == '1' ||
			  zw_feld[0] == '2' ||
			  zw_feld[0] == '3' ||
			  zw_feld[0] == '4' ||
			  zw_feld[0] == '5' ||
			  zw_feld[0] == '6' ||
			  zw_feld[0] == '7' ||
			  zw_feld[0] == '8' ||
			  zw_feld[0] == '9')
		{
			ret_code = OK;
		}
		else
		{
			ret_code = NOK;
			return (ret_code);
		}


/*       Umsetzen "O" nach "0" und "I" nach "J"                */

		anz_9 = 0;

	for  (i=4; zw_feld[i] != '\0';i++)
		 {
					 if(zw_feld[i] == 'O')
								zw_feld[i] = '0';

					 if(zw_feld[i] == 'I')
								zw_feld[i] = 'J';

						if (zw_feld[i] == '9')
						{
							anz_9++;
						}
	}

/*       Laengenpruefung        ge‰ndert: 05.06.01 Version  2.50    */

		 if (( i > 20) || (i < 4))
		 {
			 ret_code = NOK;
			 return (ret_code);
		 }

/* Pr¸fen 5. Stelle auf Punkt und L‰nge 5 Stellen */

if ((zw_feld[4] == '.') && (zw_feld[5] == '\0'))
		{
			ret_code = NOK;
			return (ret_code);
		}



/*           Pruefen Stelle 5. und 6                                         */
/*  wenn eine dieser Stellen belegt ist, Abfrage auf numerisch oder Punkt    */

for (i=4;(i < 6) && (zw_feld[i] != '\0');i++)
{
	if (zw_feld[i] != ' ')
	{
		if ((isdigit(c= zw_feld[i])) || (zw_feld[i] == '.'))
		{
			ret_code = OK;
		}
		else
		{
			ret_code = NOK;
			return (ret_code);
		}
	}
}

/*           Pruefen Stelle 7                                      */
/*  wenn eine dieser Stellen belegt ist, Abfrage auf numerisch oder Punkt    */


for (i=6;(i < 7) && (zw_feld[i] != '\0');i++)
{
	if (zw_feld[i] != ' ')
	{
		if ((isdigit(c= zw_feld[i])) || (zw_feld[i] == '.'))
		{
			ret_code = OK;
		}
		else
		{
			ret_code = NOK;
			return (ret_code);
		}
	}
}


/* Pr¸fung auf Umlaute */

for (i=0;zw_feld[i] != '\0';i++)
{
    if (  zw_feld[i] == KLEINAE
			     || zw_feld[i] == KLEINOE
			     || zw_feld[i] == KLEINUE
			     || zw_feld[i] == GROSSAE
			     || zw_feld[i] == GROSSOE
			     || zw_feld[i] == GROSSUE)
    {
		 ret_code = NOK;
	     return (ret_code);
	}
}

	c=c; /* vermeidet Warnings*/
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_auf                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereiten stellengerecht                            */
/*                                                                            */
/*      Aufrufe:        norm03_af1      Auswahl nach Steuerbyte               */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_auf(char *zw_feld)
#else
size_t norm03_auf(zw_feld)
char *zw_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i,j;

/* ------------------------------------ */
/*     '-Z'  - Verarbeitung             */
/* ------------------------------------ */

	for (i=0, j=0; zw_feld[i] != '\0'; i++)
	   {
	     if (zw_feld[i] == '-'
		 && zw_feld[i+1] != 'Z')
		 {
		 continue;
		 }
	     else
		{
		 zw_ausf[j] = zw_feld[i];
		 j++;
		}
	    }
	 zw_ausf[j] = zw_feld[i];

   return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_all                                            */
/*                                                                            */
/*      Aufgabe:        Allgemeine Verarbeitung der Stellen                */
/*                                                                            */
/*      Aufrufe:        norm03_af1      Auswahl nach Steuerbyte               */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_all(char *zw_feld)
#else
size_t norm03_all(zw_feld)
char *zw_feld;
#endif
{
	size_t      ret_code = OK;
	size_t      i;
	char		c;


/* ------------------------------------ */
/*     '-Z'  - Verarbeitung             */
/* ------------------------------------ */

	for (i=7; zw_ausf[i] != '\0'; i++)
			 {
					 if (zw_ausf[i] == '-')
					 {
					  if (zw_ausf[i+2]== '\0')
						{
						 break;
						}
					  else
						{
						 ret_code = NOK;
						 return (ret_code);
						}
					 }
			  }
/* ------------------------------------ */
/*     8. Stelle - Verarbeitung         */
/* ------------------------------------ */

		i=7;

		if  (( isdigit (c = zw_ausf[i])) || zw_ausf[i] == '.')
		 {
		 ;
		 }

	   else
		{
		 if (  zw_ausf[i] == '-'
		    || zw_ausf[i] == '\0')
		    {
		     ret_code = OK;
		     return (ret_code);
		    }
		 else
                    {
			  ret_code = NOK;
			  return (ret_code);
		    }
		}
/* ------------------------------------ */
/*     9. u. 10. Stelle - Verarbeitung  */
/* ------------------------------------ */

		for (i=8;i<10;i++)
		    {
			  if  (( isalpha (c = zw_ausf[i])) || zw_ausf[i] == '.')
			 {
			 ;
			 }
			  else
			 {
			 if (zw_ausf[i] == '-' || zw_ausf[i] == '\0')
				 {
				 ret_code = OK;
				 return (ret_code);
				 }
			 else
			    {
			    ret_code = NOK;
			    return (ret_code);
			    }
			 }
			  }
/* ------------------------------------ */
/*    11. u. 13. Stelle - Verarbeitung  */
/* ------------------------------------ */

		  for (i=10;i<13;i++)
		    {
			  if  (( isdigit (c = zw_ausf[i])) || zw_ausf[i] == '.')
			 {
			 ;
			 }
		     else
			 {
			 if (zw_ausf[i] == '-'
                            || zw_ausf[i] == '\0')
			    {
			    ret_code = OK;
			    return (ret_code);
			    }
			 else
			    {
			    if (i==12 && zw_ausf[i]== '\0')
			       {
			       ret_code = OK;
			       return (ret_code);
					 }
				 else
                               {
			       ret_code = NOK;
			       return (ret_code);
			       }
			    }
			 }
		     }

/* ------------------------------------ */
/*    14. u. 15. Stelle - Verarbeitung  */
/* ------------------------------------ */

			  for (i=13;i<15;i++)
		    {
			  if  (( isalpha (c = zw_ausf[i])) || zw_ausf[i] == '.')
			 {
			 ;
			 }
		     else
			 {
			 if (zw_ausf[i] == '-'
                            || zw_ausf[i] == '\0')
			    {
			    ret_code = OK;
			    return (ret_code);
				 }
			 else
			    {
			    ret_code = NOK;
			    return (ret_code);
			    }
			 }
		     }
/* ------------------------------------ */
/*    16. Stelle - Verarbeitung         */
/* ------------------------------------ */
		     i=15;
			  if  (( isdigit (c = zw_ausf[i])) || zw_ausf[i] == '.')
			 {
			 ;
			 }
		     else
			 {
			 if (zw_ausf[i] == '-'
                            || zw_ausf[i] == '\0')
			    {
			    ret_code = OK;
			    return (ret_code);
			    }
			 else
			    {
				 ret_code = NOK;
			    return (ret_code);
			    }
			 }
		     i++;
/* ------------------------------------ */
/*    17. Stelle - Verarbeitung         */
/*    Ende '\0'oder '-Z'  		*/
/* ------------------------------------ */

		     if (zw_ausf[i] == '\0')
			    {
			    ret_code = OK;
				 }
                     else
			 {
			 if (zw_ausf[i] == '-')
			    {
			    ret_code = OK;
			    }
			 else
			    {
			    ret_code = NOK;
			    }
			 }
   c=c; /* vermeidet Warnings*/
   *zw_feld = *zw_feld;
   return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm03_afm      Aufbereitung MASCHINELL neu und alt   */
/*                      norm03_afd      Aufbereitung DRUCK                    */
/*                      norm03_afs      Aufbereitung SORTIERT neu und alt     */
/*                      norm03_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_af1(norm_p1 *n03_p1)
#else
size_t norm03_af1(n03_p1)
norm_p1 *n03_p1;
#endif
{
		  size_t       ret_code = NOK;

 
                                     norm03_afm(n03_p1->m_norm);
                                     norm03_afd(n03_p1->d_norm);
                                     norm03_afs(n03_p1->s_norm);
                                     norm03_afb(n03_p1->b_norm);
                                     norm03_afm(n03_p1->ma_norm);
                                     norm03_afs(n03_p1->sa_norm);
                                     ret_code = OK;
 
	norm03_normby(n03_p1);
        return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_afm                                            */
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
size_t norm03_afm(char *aus_feld)
#else
size_t norm03_afm(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       j;

	for (j=0; zw_ausf[j] != '\0'; j++)
		{
		aus_feld[j] = zw_ausf[j];
		}

	for (; j<25; j++)
	     { 
	      aus_feld[j] = ' ';
	     }

	aus_feld[j] = '\0';

return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_afd                                            */
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
size_t norm03_afd(char *aus_feld)
#else
size_t norm03_afd(aus_feld)
char *aus_feld;
#endif
{
    size_t       ret_code = NOK;
	size_t       i,j;

	for (i=0,j=0; zw_ausf[i] != '\0'; i++,j++)
	    {
	     if (i==7 && (zw_ausf[i] != '-')) 
			{
				aus_feld[j] = '-';
				j++;
			}
		  if ((i == 12) && (zw_ausf[12] != '-') 
			 && (zw_ausf[11] != '-'))
			{
				aus_feld[j] = '-';
				j++;
			}

		aus_feld[j] = zw_ausf[i];
	     }

        for (; j<25; j++)
	     { 
	      aus_feld[j] = ' ';
	     }
	aus_feld[j] = '\0';


return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_afs                                            */
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
size_t norm03_afs(char *aus_feld)
#else
size_t norm03_afs(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
    size_t       j;

	for (j=0; zw_ausf[j] != '\0'; j++)
	    {
	    aus_feld[j] = zw_ausf[j];
	    }

	for (; j<25; j++)
	     { 
	      aus_feld[j] = ' ';
	     }

	aus_feld[j] = '\0';

return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_afb                                            */
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
size_t norm03_afb(char *aus_feld)
#else
size_t norm03_afb(aus_feld)
char *aus_feld;
#endif
{
 	size_t	ret_code = NOK;
	size_t  i,j;

	for (i=0,j=0; zw_ausf[i] != '\0'; i++,j++)
	    {
	     if (i==7 && (zw_ausf[i] != '-')) 
			{
				aus_feld[j] = '-';
				j++;
			}
		  if ((i == 12) && (zw_ausf[12] != '-') 
			 && (zw_ausf[11] != '-'))
			{
				aus_feld[j] = '-';
				j++;
			}
		aus_feld[j] = zw_ausf[i];
	     }

        for (; j<25; j++)
	     { 
	      aus_feld[j] = ' ';
	     }

	aus_feld[j] = '\0';

return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_normnb                                            */
/*                                                                            */
/*      Aufgabe:        Normbyte_Belegung                                      */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*      Aenderung:      19.1.94, Gerland                                      */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_normby(norm_p1 *n03_p1)
#else
size_t norm03_normby(n03_p1)
norm_p1 *n03_p1;
#endif
{
	   size_t	ret_code=OK;
	   size_t	i;
	   size_t	ilength;
		char	ziffer_9zfl[4];
        anz_9z = 0;
        strcpy(ziffer_9zfl,"000");

	    // ilength = strlen(ziffer_9zfl)+1;

//	strcpy(dummy, n02_p1->m_norm);
	  // strcpy_s(ziffer_9zfl, ilength, "000");



		norm00_nb(n03_p1->normbyte,"F ");

	 
for (i=0; zw_ausf[i] != '\0'; i++)
{
	if (i==6 || i==7 || i==10 || i==11 || i==12 || i == 15)
	{ 
		if (zw_ausf[i] == '9')
		{
			ziffer_9zfl[0] = '1';
			anz_9z++;
		}
	}

	if (i==8 || i==9 || i==13 || i==14)
	{ 
		if (zw_ausf[i] == 'Z' && zw_ausf[i+1] != ' ' && zw_ausf[i+1] != '\0' )
		{
			ziffer_9zfl[1] = '1';
            anz_9z++; 
		}
	    if (zw_ausf[i] == 'Z' && zw_ausf[i+1] == '\0' && zw_ausf[i-1] != '-' )
		{
			ziffer_9zfl[1] = '1';
			anz_9z++;
		}
   }

	if (zw_ausf[i] == 'Z' && zw_ausf[i+1] == '\0' && zw_ausf[i-1] == '-')
	{
		ziffer_9zfl[2] = '1';
		anz_9z++;
	}
}
	 
/* ohne Ziffer 9 und ohne Z innerhalb / ohne flie·endes Z */

	
	 if (strcmp(ziffer_9zfl,"000") == 0)
	 	{
		norm00_nb(n03_p1->normbyte,"F ");
        mlfb_ok = 1;
		}

/* mit Ziffer 9 und/oder mit Z innerhalb / ohne flie·endes Z */

	 if (strcmp(ziffer_9zfl,"100") == 0)
	 	{
		norm00_nb(n03_p1->normbyte,"F1");
	 	}
	 if (strcmp(ziffer_9zfl,"110") == 0)
	 	{
		norm00_nb(n03_p1->normbyte,"F1");
	 	}
	 if (strcmp(ziffer_9zfl,"010") == 0)
	 	{
		norm00_nb(n03_p1->normbyte,"F1");
	 	}

/* mit Ziffer 9 und/oder mit Z innerhalb / mit flie·endem Z */

	 if (strcmp(ziffer_9zfl,"111") == 0)
	 	{
		norm00_nb(n03_p1->normbyte,"F2");
	 	}

	 if (strcmp(ziffer_9zfl,"101") == 0)
	 	{
		norm00_nb(n03_p1->normbyte,"F2");
	 	}

	 if (strcmp(ziffer_9zfl,"011") == 0)
		{
		norm00_nb(n03_p1->normbyte,"F2");
    	}

/* ohne Ziffer 9 und ohne Z innerhalb / mit flie·endem Z */

	 if (strcmp(ziffer_9zfl,"001") == 0)
		{
		norm00_nb(n03_p1->normbyte,"F3");
		}


/*	 if (strlen(zw_ausf) == 7 || strlen(zw_ausf) == 8)
		{
		norm00_nb(n03_p1->normbyte,"F ");
		}
*/

/* F7 wenn 4.Stelle = 0 und Stellen 1 - 7 belegt und 8.Stelle = \0 */

/* Ba2006.10.15 
	 if (strlen(zw_ausf) == 7 && zw_ausf[3] == '0')
		{
		norm00_nb(n03_p1->normbyte,"F7");
		}
*/
/* F0 wenn 4.Stelle = 0 und mehr als 7 Stellen belegt */

/* Ba2006.10.15 
	 if (strlen(zw_ausf) > 7 && zw_ausf[3] == '0')
		{
		norm00_nb(n03_p1->normbyte,"F0");
		}
*/
/* F9 wenn mindestens 1 Punkt in der Nummer vorhanden */

/* 19.1.94, MG, Fehlermeldung Hr. Gojowsky v. 17.1.94     */
/* for (i=7; zw_ausf[i] != '\0'; i++)     geaendert in :  */

for (i=5; zw_ausf[i] != '\0'; i++)
{
		if (zw_ausf[i] == '.')
		{ 
			norm00_nb(n03_p1->normbyte,"F9");
			break;
		}
}

if ((zw_ausf[4] == '\0') || (zw_ausf[5] == '\0'))
{
			norm00_nb(n03_p1->normbyte,"FX");
}

return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_text                                           */
/*                                                                            */
/*      Aufgabe:        PrÅfung zusÑtzlicher Textzeilen                       */
/*                                                                            */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_text(norm_p1 *n03_p1,norm_p2 *n03_p2)
#else
size_t norm03_text(n03_p1,n03_p2)
norm_p1 *n03_p1;
norm_p2 *n03_p2;
#endif
{
		 size_t ret_code=OK;
		 size_t ret_wert=OK;

		 size_t       i,j,k,anz_bzeilen, anz_tab;
		 char      wert_tabelle[17];
		 size_t H_minusZ;
		 size_t H_Z;
		 size_t H_9;
		 notw_mit_fl_z = 0;
		 anz_ezeilen = 0;
		 anz_notw_ezeilen = 0;
		 anz_bzeilen = 0;
		 anz_tab=0;
         anz_vory=0;
		 anz_vor_oy=0;
         anz_kurz=0;
		 mit_fl_z = 0;
		 

	/*  Elemente der Werttabelle mit '0' belegen */
	
	for (i = 0;i < 16;i++)
	{
		wert_tabelle[i] = '0';
	}
	wert_tabelle[i] = '\0';

	/***************************************/
	/*  Fehlernummer mit NULL vorbelegen   */
	/***************************************/

	for (i=0; i< 7;i++)
	{
		n03_p2->mlfb_feh[i] = '0';
	}
	n03_p2->mlfb_feh[i] = '\0';

    /*******************************************************************/
	/*  fliessendes -Z vorhanden ?               							 */
	/*******************************************************************/

	if ((strcmp(n03_p1->normbyte,"F2") == 0) || (strcmp(n03_p1->normbyte,"F3") == 0))
	{
		mit_fl_z = 1;
	}

	for (i=0; zw_ausf[i] != '\0'; i++)
	{
	     if (zw_ausf[i] == '-'
		 && zw_ausf[i+1] == 'Z')
		 {
		 mit_fl_z = 1;
		 }
	}

	
	/*******************************************************************/
   /*  Pr¸fen, ob bei "F " kein Text vorhanden ist?  27.01.2000 Androleit	 */
   /*******************************************************************/
	
	
	if (strcmp(n03_p1->normbyte,"F ") == 0)
	{
       for (i=0; i < 41; i++)
	   {
    	  if ((toupper(n03_p2->zeile[i].mlfb_art[0]) == 'E') || (toupper(n03_p2->zeile[i].mlfb_art[0]) == 'C'))
		  {
				/*******************************************************************/
				/*  Pruefung raus CR Sonnleitner Fehlerprio   06.04.2005 Baller	 */
				/******************************************************************
			  ret_wert = norm03_feh(n03_p2->mlfb_feh,2,2);*/
		  }
	   }
    }


	for (i = 0;i < 41; i++)
	{
   	/************************************************/
	/* Umwandlung mlfb_art in Grossbuchstaben       */
	/************************************************/

		if (isalpha(n03_p2->zeile[i].mlfb_art[0]) &&
			 islower(n03_p2->zeile[i].mlfb_art[0]))
		{
			n03_p2->zeile[i].mlfb_art[0] = toupper(n03_p2->zeile[i].mlfb_art[0]);
		}
		/*******************************************/
		/*  Entfernen aller SPACES in B-Zeilen     */
		/*******************************************/

		if (n03_p2->zeile[i].mlfb_art[0] == 'B')
		{
			k=0;
			for (j = 0;(j < 32) && (n03_p2->zeile[i].mlfb_txt[j] != '\0'); j++)
			{
				if (n03_p2->zeile[i].mlfb_txt[j] != ' ')
				{
					n03_p2->zeile[i].mlfb_txt[k] = n03_p2->zeile[i].mlfb_txt[j];

					/*********************************/
					/* Umwandlung in Grossbuchstaben */
							 /*********************************/
					if (isalpha(n03_p2->zeile[i].mlfb_txt[k]) &&
						 islower(n03_p2->zeile[i].mlfb_txt[k]))
					{
						n03_p2->zeile[i].mlfb_txt[k] = toupper(n03_p2->zeile[i].mlfb_txt[k]);
					}

					k++;
				}
			}
				 /*********************************/
			/* Zeile mit '\0' auffuellen     */
				 /*********************************/
			for (;k < 32;k++)
			{
				n03_p2->zeile[i].mlfb_txt[k] = '\0';
			}
		}



		/*******************************************/
		/* PrÅfen auf gueltige Satzarten     ba08.09.2005      */
		/******************************************

		if ((n03_p2->zeile[i].mlfb_art[0] != 'B')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'D')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'E')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'F')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'N')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'H')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'C')
			&& (n03_p2->zeile[i].mlfb_art[0] != ' ')
			&& (n03_p2->zeile[i].mlfb_art[0] != '\0'))
		{
			   Fehler-Nummer belegen 

			ret_wert = norm03_feh(n03_p2->mlfb_feh,1,9);      BaFehler 19 
	} */

		/*********************************************/
		/* PrÅfen F-Zeile und 1. Stelle '*' bei Text */
		/*********************************************/
		/* Pruefung wurde gem. Problemmeldung 34 v. 10.02.97 entnommen   */


/*		if (n03_p2->zeile[i].mlfb_art[0] == 'F')         */
/*		{                                                */
/*			if (n03_p2->zeile[i].mlfb_txt[0] != '*')       */
/*			{                                              */
/*				ret_wert = norm03_feh(n03_p2->mlfb_feh,2,0); */
/*			}                                              */
/*		} */

		/*******************************************/
		/* Anzahl B-Zeilen ermitteln               */
		/*******************************************/

		if (n03_p2->zeile[i].mlfb_art[0] == 'B')
		{
			anz_bzeilen++;
		}


		/*******************************************/
		/* Anzahl E-Zeilen ermitteln               */
		/*******************************************/

		if (n03_p2->zeile[i].mlfb_art[0] == 'E')
		{
			anz_ezeilen++;
		}
		/***************************************************************/
		/* Anzahl C-Zeilen ermitteln                                   */
		/* C-Zeilen werden zu der Anzahl der E-Zeilen addiert          */
		/* C-Zeilen werden nur dann addiert, wenn in der voraus-       */
		/* gegangenen Zeile kein E an der ersten Stelle gestanden war. */
		/***************************************************************/

		if (n03_p2->zeile[i].mlfb_art[0] == 'C')
		{
						anz_ezeilen++;
		}
	}

	for (i = 0;i < 41; i++)
	{
		if (n03_p2->zeile[i].mlfb_art[0] == 'B')
		{
			/***************************************/
			/*  Format pruefen                     */
			/***************************************/

			ret_wert = norm03_format(n03_p2->zeile[i].mlfb_txt,n03_p2->mlfb_feh);

			/**********************************************************/
			/*  Tabelle mit Anzahl GHJKLMNOPQR und -Z aus ANA fuellen */
			/**********************************************************/

			ret_wert = norm03_tabelle(n03_p2->zeile[i].mlfb_txt,n03_p2->mlfb_feh,wert_tabelle);

		}
	}

	/*ba08.04.2005   prio  */

		if (notw_mit_fl_z > mit_fl_z)
	{
		ret_wert = norm03_feh(n03_p2->mlfb_feh,1,2);		/*BaFehler 12 */
	}

	/* Anzahl e- oder c-Zeilen > 0 */
	/*******************************************************************/
	/* Tabelle mit ermittelten Ziffern und Buchstabenstellen mit der   */
	/* Åbergebenen MLFB-Nummer vergleichen                             */
    /*******************************************************************/

	for (i = 0;i < 16;i++)
	{
		if (wert_tabelle[i] == '1')
		{
			anz_tab++;
			if ((zw_ausf[i] != '9') && (zw_ausf[i] != 'Z'))
			{
				ret_wert = norm03_feh(n03_p2->mlfb_feh,1,4);   /*ba18.08.2004*/      /*BaFehler 14 */
			} 
		}
	}

	/*******************************************************************/  /*ba18.08.2004*/
	/* Wenn -Z vorkaommt aber nicht 9 oder Z darf die text Zeile nur   */
	/*Format ANN und nicht AnA haben								   */
	/*******************************************************************/
	H_minusZ = -1;
	H_Z = -1;
	H_9 = -1;
	for (i = 0;i < 41; i++)
	{
		if (n03_p2->zeile[i].mlfb_art[0] == 'B')
		{
			/* arno*/
			for (k = 6;k < 16;k++)
			{
				if ((zw_ausf[k] == 'Z') && (zw_ausf[k-1] == '-'))
				{
				H_minusZ = 1;	
				}
			}
			for (k = 6;k < 16;k++)
			{
				if ((zw_ausf[k] == 'Z') && (zw_ausf[k-1] != '-'))
				{
				H_Z = 1;	
				}
			}
			for (k = 6;k <16;k++)
			{
				if ((zw_ausf[k] == '9'))
				{
				H_9 = 1;	
				}
			}

			if ((H_minusZ > 0) && (H_Z < 1) && (H_9 < 1))
			{
			/***************************************/
			/*nur Format ANN erlaubt               */
			/***************************************/
				for (j = 2;(j < 32) && (n03_p2->zeile[i].mlfb_art[j] != '\0');)

				{
					/*********************************************/
					/*  PrÅfen, ob 1.Stelle alpha                */
					/*********************************************/	
				
					if (!isalpha(n03_p2->zeile[i].mlfb_art[j]))
					{
						ret_wert = norm03_feh(n03_p2->mlfb_feh,1,4);       /*BaFehler 14 */
					}
					/*********************************************/
					/*  PrÅfen, ob 2.Stelle numerisch            */
					/*********************************************/
					if (!isdigit(n03_p2->zeile[i].mlfb_art[j+1]))
					{
						ret_wert = norm03_feh(n03_p2->mlfb_feh,1,4);		/*BaFehler 14 */
					}

					/*********************************************/
					/*  PrÅfen, ob 3.Stelle  numerisch			 */
					/*********************************************/
					if (((!isdigit(n03_p2->zeile[i].mlfb_art[j+2]))))
					{
						ret_wert = norm03_feh(n03_p2->mlfb_feh,1,4);		/*BaFehler 14 */
					}

					
					j = j + 4;
				}
			}	
			

		}
	}




	/*******************************************************************/
	/* MLFB mit Tabelle vergleichen (siehe oben)                       */
	/*******************************************************************/
	




	if (mit_fl_z == 1)
	{
		j = (strlen(zw_ausf) - 2);
	}
	else
	{
		j = strlen(zw_ausf);
	}

/*ba19.11.2004 */
	for (i = 6;i < j;i++)
	{
		if ((zw_ausf[i] == '9') || ((zw_ausf[i] == 'Z') && (zw_ausf[i-1] != '-')))
		{
			if ((wert_tabelle[i] == '0') && (anz_ezeilen == 0))
			{
				ret_wert = norm03_feh(n03_p2->mlfb_feh,0,4);			/*BaFehler 04 */
			} 
		}
	}

	
	/*ba06.04.2005   prio*/	
	if (mit_fl_z > notw_mit_fl_z)
    {
      if (anz_ezeilen == 0)
	  {
		ret_wert = norm03_feh(n03_p2->mlfb_feh,0,5);		/*BaFehler 05 */
	  }
    }


	/* ƒnderung 27.01.2000 Androleit */

	if ((anz_ezeilen > 0) && (anz_vory == 0))
	{
		if ((anz_tab == anz_kurz) && (anz_kurz != 0) && (mit_fl_z == 0) && (anz_9z == anz_kurz))
		{
/*ba03052004		   	ret_wert = norm03_feh(n03_p2->mlfb_feh,2,2);   */
		}
		if ((anz_kurz != 0) && (mit_fl_z > 0) && (anz_9z == anz_kurz))
		{
/*ba03052004 		   	ret_wert = norm03_feh(n03_p2->mlfb_feh,2,2);   */
		}
		if ((mit_fl_z == 1) && (notw_mit_fl_z == 1) && (anz_9 == 0))
		{
/*ba03052004		   	ret_wert = norm03_feh(n03_p2->mlfb_feh,2,2);   */
		}
	}

	/*******************************************************************/
	/* Anzahl ermittelte E-Zeilen mit notwendigen E-Zeilen vergleichen */
	/* C-Zeilen wurde hinzugezaehlt                                    */
    /* ƒnderung 11.01.2000 Androleit eine E- oder C-Zeile gen¸gt, um allen
	/* Anforderungen gerecht zu werden                                   
	/*******************************************************************/
    /* neu 11.01.2000 Androleit                                        */
 
	if ((anz_vory > 0) && (anz_ezeilen == 0))
	{
				/*   Fehler-Nummer belegen */

				ret_wert = norm03_feh(n03_p2->mlfb_feh,2,1);		/*BaFehler 21     */
	}


		/*******************************************/
		/* PrÅfen auf gueltige Satzarten       ba08.04.2005         */
		/*******************************************/
for (i = 0;i < 41; i++)
	{
		if ((n03_p2->zeile[i].mlfb_art[0] != 'B')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'D')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'E')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'F')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'N')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'H')
			&& (n03_p2->zeile[i].mlfb_art[0] != 'C')
			&& (n03_p2->zeile[i].mlfb_art[0] != ' ')
			&& (n03_p2->zeile[i].mlfb_art[0] != '\0'))
		{
			/*   Fehler-Nummer belegen */

			ret_wert = norm03_feh(n03_p2->mlfb_feh,1,9);      /*BaFehler 19 */
		}
	}	
	ret_wert=ret_wert;

 return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_format                                         */
/*                                                                            */
/*      Aufgabe:        PrÅfen auf Format ANN, ANA und "+" zwischen beiden    */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_format(char *mlfb_txt,char *mlfb_feh)
#else
size_t norm03_format(mlfb_txt,mlfb_feh)
char *mlfb_txt;
char *mlfb_feh;
#endif
{
	size_t	j=0;
	size_t	i=0;
	char	c;
	size_t	ret_wert = OK;

	for (j = 0;(j < 32) && (mlfb_txt[j] != '\0');)

	{
		/*********************************************/
		/*  PrÅfen, ob 1.Stelle alpha                */
		/*********************************************/
		if (!isalpha(c=mlfb_txt[j]))
		{
			ret_wert = norm03_feh(mlfb_feh,0,9);   /*BaFehler 09 */
		}

        /*********************************************/
		/*  PrÅfen, ob 2.Stelle numerisch            */
		/*********************************************/
        if (!isdigit(c=mlfb_txt[j+1]))
		{
			ret_wert = norm03_feh(mlfb_feh,0,9);   /*BaFehler 09 */
		}

        /*********************************************/
		/*  PrÅfen, ob 3.Stelle alpha oder numerisch */
		/*********************************************/
		if ((!isalpha(c=mlfb_txt[j+2])) && (!isdigit(c=mlfb_txt[j+2])))
		{
			ret_wert = norm03_feh(mlfb_feh,1,1);   /*BaFehler 11 */
		}

		/*****************************************************/
		/*  PrÅfen, ob zwischen 2 Dreierblîcken ein + steht  */
		/*****************************************************/
		if ((mlfb_txt[j+3] == '+') && (mlfb_txt[j+4] == '\0'))
		{
			ret_wert = norm03_feh(mlfb_feh,1,7); /*BaFehler 17 */
		}
			if ((mlfb_txt[j+3] == '+') && (!isalpha(c=mlfb_txt[j+4]))
				&& (mlfb_txt[j+4] != '\0'))
		{
			ret_wert = norm03_feh(mlfb_feh,0,8);
		}
      if ((mlfb_txt[j+3] != '+') && (mlfb_txt[j+3] != '\0'))
		{
			ret_wert = norm03_feh(mlfb_feh,0,8);   /*BaFehler 08 */
		}

		j = j + 4;
	}
  c=c; i=i; /* vermeidet Warnings*/
  return(ret_wert);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_tabelle                                        */
/*                                                                            */
/*      Aufgabe:        Fuellen Tabelle mit Anzahl Buchstaben aus ANA         */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_tabelle(char *mlfb_txt,char *mlfb_feh,char *wert_tab)
#else
size_t norm03_tabelle(mlfb_txt,mlfb_feh,wert_tab)
char *mlfb_txt;
char *mlfb_feh;
char *wert_tab;
#endif
{
	size_t	j=0;
	char	c;
	size_t	ret_wert = OK;

	 for (j = 0;(j < 32) && (mlfb_txt[j] != '\0');)
	{
		if ((isalpha(c=mlfb_txt[j])) && (isdigit(c=mlfb_txt[j+1]))
			&& (isalpha(c=mlfb_txt[j+2])))
		{
			switch (mlfb_txt[j])
			{
				case  'G':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,6);    /*BaFehler 10 */
					break;
				case  'H':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,7);
					break;
				case  'J':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,8);
					break;
				case  'K':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,9);
					break;
				case  'L':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,10);
					break;
				case  'M':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,11);
					break;
				case  'N':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,12);
					break;
				case  'P':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,13);
					break;
				case  'Q':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,14);
					break;
				case  'R':
					ret_wert = norm03_tab_fuell(mlfb_feh,wert_tab,15);
					break;
				default:
					ret_wert = norm03_feh(mlfb_feh,1,5);                 /*BaFehler 15 */
			}

		 if ((toupper(mlfb_txt[j+2]) == 'Y' && toupper(mlfb_txt[j]) != 'Y') ||
			 (toupper(mlfb_txt[j+2]) != 'Y' && toupper(mlfb_txt[j]) == 'Y'))
		 {
		     anz_vory++;
             anz_kurz++;
		 }
		 else
		 {
			 anz_vor_oy++;
			 anz_kurz++;
		 }

        }
		/*ba15.11.2004*/  /*ba19.11.2004*/
		if ((isalpha(c=mlfb_txt[j])) && (isdigit(c=mlfb_txt[j+1]))
			&& (isdigit(c=mlfb_txt[j+2])))
		{
			anz_kurz++;
			if ((toupper(mlfb_txt[j]) == 'O') ||
					(toupper(mlfb_txt[j]) == 'Z'))
			{
				ret_wert = norm03_feh(mlfb_feh,0,9);        /*BaFehler 09 */
			}
			else
			{
				notw_mit_fl_z = 1;
			}
			




/* Y am Ende von Kurzangabe feststellen */
			
			if (mlfb_txt[j] == 'Y')
			{
                anz_vory++;
			}

			if (mlfb_txt[j] == 'I')
			{
/*  			LÑnderzahlentabelle pruefen   */
			}
		}

		j=j+4;

	}
		c=c; /* vermeidet Warnings*/
		return(ret_wert);
}


/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_tab_fuell                                      */
/*                                                                            */
/*      Aufgabe:        Fuellen Tabelle mit Anzahl Buchstaben aus ANA         */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_tab_fuell(char *mlfb_feh,char *wert_tab,size_t wert)
#else
size_t norm03_tab_fuell(mlfb_feh,wert_tab,wert)
char *mlfb_feh;
char *wert_tab;
size_t wert;
#endif
{
size_t ret_wert = OK;

		if (wert_tab[wert] == '0')
		{
			wert_tab[wert] = '1';
		}
		else
		{
			ret_wert = norm03_feh(mlfb_feh,1,0); /*BaFehler 10 */
		}

   return(ret_wert);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm03_feh                                            */
/*                                                                            */
/*      Aufgabe:        Belegen Fehler-Kennzeichen                            */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm03_feh(char *mlfb_feh,size_t feh_wert1,size_t feh_wert2)
#else
size_t norm03_feh(mlfb_feh,feh_wert1,feh_wert2)
char *mlfb_feh;
size_t feh_wert1;
size_t feh_wert2;
#endif
{
  size_t ret_wert = OK;
  size_t prioneu = 0;
  size_t prioalt;
  size_t prioalt1;
  char feh_wert1_asc[2];
  char feh_wert2_asc[2];
  char feh_wert1_asc09[2];
  char feh_wert1_asc15[2];
  char feh_wert1_asc10[2];
  char feh_wert1_asc12[2];
  char feh_wert1_asc14[2];
  char feh_wert1_asc11[2];
  char feh_wert1_asc04[2];
  char feh_wert1_asc05[2];
  char feh_wert1_asc21[2];
  char feh_wert1_asc08[2];
  char feh_wert1_asc17[2];
  char feh_wert1_asc19[2];  
  char feh_wert1_asc00[2];

#ifdef WINNT
	char size_t2char1[11],size_t2char2[11];
#endif
	/* Verschieben von 2-stelligen Fehlernummern   */
	/* die Stellen 6 und 7 werden mit der zweistelligen öbergabenummer belegt */

#ifdef _Windows
#ifdef WINNT
	_itoa(feh_wert1,size_t2char1,10);
#else
  wsprintf(feh_wert1_asc,"%d",feh_wert1);
#endif
#else
#ifdef _PASCAL
  itoa(feh_wert1,feh_wert1_asc);
#endif
	sprintf(feh_wert1_asc,"%d",feh_wert1);
#endif

#ifdef _Windows
#ifdef WINNT
  _itoa(feh_wert2,size_t2char2,10);
#else
  wsprintf(feh_wert2_asc,"%d",feh_wert2);
#endif
#else
#ifdef _PASCAL
  itoa(feh_wert2,feh_wert2_asc);
#endif
	sprintf(feh_wert2_asc,"%d",feh_wert2);
#endif

feh_wert1_asc09[0] = '0';
feh_wert1_asc09[1] = '9';

feh_wert1_asc15[0] = '1';
feh_wert1_asc15[1] = '5';

feh_wert1_asc10[0] = '1';
feh_wert1_asc10[1] = '0';

feh_wert1_asc12[0] = '1';
feh_wert1_asc12[1] = '2';

feh_wert1_asc14[0] = '1';
feh_wert1_asc14[1] = '4';

feh_wert1_asc11[0] = '1';
feh_wert1_asc11[1] = '1';

feh_wert1_asc04[0] = '0';
feh_wert1_asc04[1] = '4';

feh_wert1_asc05[0] = '0';
feh_wert1_asc05[1] = '5';

feh_wert1_asc21[0] = '2';
feh_wert1_asc21[1] = '1';

feh_wert1_asc08[0] = '0';
feh_wert1_asc08[1] = '8';

feh_wert1_asc17[0] = '1';
feh_wert1_asc17[1] = '7';

feh_wert1_asc19[0] = '1';
feh_wert1_asc19[1] = '9';

feh_wert1_asc00[0] = '0';
feh_wert1_asc00[1] = '0';

prioneu = 1;
prioalt = 50;
prioalt1 = 50;
if ((feh_wert1_asc[0] == feh_wert1_asc09[0] ) && (feh_wert2_asc[0] == feh_wert1_asc09[1] ))
	{
	prioneu = 1;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc15[0] ) && (feh_wert2_asc[0] == feh_wert1_asc15[1] ))
	{
	prioneu = 3;
	}	
if ((feh_wert1_asc[0] == feh_wert1_asc10[0] ) && (feh_wert2_asc[0] == feh_wert1_asc10[1] ))
	{
	prioneu = 5;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc12[0] ) && (feh_wert2_asc[0] == feh_wert1_asc12[1] ))
	{
	prioneu = 7;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc14[0] ) && (feh_wert2_asc[0] == feh_wert1_asc14[1] ))
	{
	prioneu = 9;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc11[0] ) && (feh_wert2_asc[0] == feh_wert1_asc11[1] ))
	{
	prioneu = 11;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc04[0] ) && (feh_wert2_asc[0] == feh_wert1_asc04[1] ))
	{
	prioneu = 13;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc05[0] ) && (feh_wert2_asc[0] == feh_wert1_asc05[1] ))
	{
	prioneu = 15;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc21[0] ) && (feh_wert2_asc[0] == feh_wert1_asc21[1] ))
	{
	prioneu = 17;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc08[0] ) && (feh_wert2_asc[0] == feh_wert1_asc08[1] ))
	{
	prioneu = 19;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc17[0] ) && (feh_wert2_asc[0] == feh_wert1_asc17[1] ))
	{
	prioneu = 21;
	}
if ((feh_wert1_asc[0] == feh_wert1_asc19[0] ) && (feh_wert2_asc[0] == feh_wert1_asc19[1] ))
	{
	prioneu = 23;
	}



if ((mlfb_feh[5] == feh_wert1_asc09[0] ) && (mlfb_feh[6] == feh_wert1_asc09[1] ))
	{
	prioalt = 1;
	}
if ((mlfb_feh[5] == feh_wert1_asc15[0] ) && (mlfb_feh[6] == feh_wert1_asc15[1] ))
	{
	prioalt = 3;
	}	
if ((mlfb_feh[5] == feh_wert1_asc10[0] ) && (mlfb_feh[6] == feh_wert1_asc10[1] ))
	{
	prioalt = 5;
	}
if ((mlfb_feh[5] == feh_wert1_asc12[0] ) && (mlfb_feh[6] == feh_wert1_asc12[1] ))
	{
	prioalt = 7;
	}
if ((mlfb_feh[5] == feh_wert1_asc14[0] ) && (mlfb_feh[6] == feh_wert1_asc14[1] ))
	{
	prioalt = 9;
	}
if ((mlfb_feh[5] == feh_wert1_asc11[0] ) && (mlfb_feh[6] == feh_wert1_asc11[1] ))
	{
	prioalt = 11;
	}
if ((mlfb_feh[5] == feh_wert1_asc04[0] ) && (mlfb_feh[6] == feh_wert1_asc04[1] ))
	{
	prioalt = 13;
	}
if ((mlfb_feh[5] == feh_wert1_asc05[0] ) && (mlfb_feh[6] == feh_wert1_asc05[1] ))
	{
	prioalt = 15;
	}
if ((mlfb_feh[5] == feh_wert1_asc21[0] ) && (mlfb_feh[6] == feh_wert1_asc21[1] ))
	{
	prioalt = 17;
	}
if ((mlfb_feh[5] == feh_wert1_asc08[0] ) && (mlfb_feh[6] == feh_wert1_asc08[1] ))
	{
	prioalt = 19;
	}
if ((mlfb_feh[5] == feh_wert1_asc17[0] ) && (mlfb_feh[6] == feh_wert1_asc17[1] ))
	{
	prioalt = 21;
	}
if ((mlfb_feh[5] == feh_wert1_asc19[0] ) && (mlfb_feh[6] == feh_wert1_asc19[1] ))
	{
	prioalt = 23;
	}
	
if ((mlfb_feh[5] == feh_wert1_asc00[0]) &&	(mlfb_feh[6] == feh_wert1_asc00[1]))
	{
	prioalt = 50;
	}



if ((mlfb_feh[3] == feh_wert1_asc09[0] ) && (mlfb_feh[4] == feh_wert1_asc09[1] ))
	{
	prioalt1 = 1;
	}
if ((mlfb_feh[3] == feh_wert1_asc15[0] ) && (mlfb_feh[4] == feh_wert1_asc15[1] ))
	{
	prioalt1 = 3;
	}	
if ((mlfb_feh[3] == feh_wert1_asc10[0] ) && (mlfb_feh[4] == feh_wert1_asc10[1] ))
	{
	prioalt1 = 5;
	}
if ((mlfb_feh[3] == feh_wert1_asc12[0] ) && (mlfb_feh[4] == feh_wert1_asc12[1] ))
	{
	prioalt1 = 7;
	}
if ((mlfb_feh[3] == feh_wert1_asc14[0] ) && (mlfb_feh[4] == feh_wert1_asc14[1] ))
	{
	prioalt1 = 9;
	}
if ((mlfb_feh[3] == feh_wert1_asc11[0] ) && (mlfb_feh[4] == feh_wert1_asc11[1] ))
	{
	prioalt1 = 11;
	}
if ((mlfb_feh[3] == feh_wert1_asc04[0] ) && (mlfb_feh[4] == feh_wert1_asc04[1] ))
	{
	prioalt1 = 13;
	}
if ((mlfb_feh[3] == feh_wert1_asc05[0] ) && (mlfb_feh[4] == feh_wert1_asc05[1] ))
	{
	prioalt1 = 15;
	}
if ((mlfb_feh[3] == feh_wert1_asc21[0] ) && (mlfb_feh[4] == feh_wert1_asc21[1] ))
	{
	prioalt1 = 17;
	}
if ((mlfb_feh[3] == feh_wert1_asc08[0] ) && (mlfb_feh[4] == feh_wert1_asc08[1] ))
	{
	prioalt1 = 19;
	}
if ((mlfb_feh[3] == feh_wert1_asc17[0] ) && (mlfb_feh[4] == feh_wert1_asc17[1] ))
	{
	prioalt1 = 21;
	}
if ((mlfb_feh[3] == feh_wert1_asc19[0] ) && (mlfb_feh[4] == feh_wert1_asc19[1] ))
	{
	prioalt1 = 23;
	}
	
if ((mlfb_feh[3] == feh_wert1_asc00[0]) &&	(mlfb_feh[4] == feh_wert1_asc00[1]))
	{
	prioalt1 = 50;
	}	
	

	if (prioneu < prioalt  || prioneu < prioalt1)
	{
		if ( prioalt1  < prioalt)
		{
			if ((feh_wert1_asc[0] != mlfb_feh[5]) ||
				(feh_wert2_asc[0] != mlfb_feh[6]))
			{
				mlfb_feh[5] = feh_wert1_asc[0];
				mlfb_feh[6] = feh_wert2_asc[0];
			}

		}
		else
		{
			if ((feh_wert1_asc[0] != mlfb_feh[5]) ||
				(feh_wert2_asc[0] != mlfb_feh[6]))
			{
				mlfb_feh[3] = mlfb_feh[5];
				mlfb_feh[4] = mlfb_feh[6];

				mlfb_feh[5] = feh_wert1_asc[0];
				mlfb_feh[6] = feh_wert2_asc[0];
			}
		}
	}
	mlfb_feh[7] = '\0';

  return(ret_wert);

}

