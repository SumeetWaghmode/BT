/* -------------------------------------------------------------------------- */
/*      Modul:  norm02                                                        */
/*                                                                            */
/*      Entry:  norm02                                                        */
/*                                                                            */
/*      Aufgabe:        Aufbereitung Fremdsachnummer                          */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Ludwig                                                */
/*                      SNI AP 623                                            */
/*                                                                            */
/*      Version:        1.0 vom 10.05.90                                      */
/*      Žnderung            am  23.07.91                                                                       */
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
size_t norm02_fp(char *zw_feld, size_t *z_laenge);
size_t norm02_all(char *zw_feld, char *ein_feld, char *auf_feld_1,
                     char *auf_feld_2,size_t *z_laenge);
size_t norm02_af1(norm_p1 *n02_p1,char *zw_feld,char *auf_feld_1,
                     char *auf_feld_2,size_t *z_laenge);
size_t norm02_afm(char *zw_feld,char *aus_feld,
                     char *auf_feld,size_t *z_laenge);
size_t norm02_afd(char *zw_feld,char *aus_feld,
                     char *auf_feld_1,size_t *z_laenge);
size_t norm02_afs(char *zw_feld,char *aus_feld,
                     char *auf_feld,size_t *z_laenge);
size_t norm02_afb(char *zw_feld,char *aus_feld,
                     char *auf_feld_1,size_t *z_laenge);
size_t norm02_normby(norm_p1 *n02_p1,char *auf_feld_1);

/* #include <ctype.h>
#include <string.h> */
#else
size_t norm02_fp();
size_t norm02_all();
size_t norm02_af1();
size_t norm02_afm();
size_t norm02_afd();
size_t norm02_afs();
size_t norm02_afb();
size_t norm02_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      FREMD-SACHNUMMER                                      */
/*                                                                            */
/*      Aufrufe:        norm02_fp       Fehlerpruefung                        */
/*                      norm02_all      Allgemeine Verarbeitung               */
/*                                      Fremdsachnummer mit ':'               */
/*                      norm02_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm02(norm_p1 *n02_p1, char *zw_feld)
#else
size_t norm02(n02_p1, zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{

	char            auf_feld[9];
  char            auf_feld_alt[9];
	char    	      *auf_feld_1  = auf_feld;
  char            *auf_feld_2 = auf_feld_alt;
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	          *z_laenge   =  &laenge;

	if((ret_code = norm02_fp(zw_feld,z_laenge)) == NOK)
  {
      norm00_nb(n02_p1->normbyte,NB_NR);
      return(ret_code);
  }

	if((ret_code = norm02_all(zw_feld, n02_p1->eingfeld, auf_feld_1,
                            auf_feld_2,z_laenge)) == NOK)
  {
		  norm00_nb(n02_p1->normbyte,NB_NR);
      return(ret_code);
  }

	if((ret_code = norm02_af1(n02_p1,zw_feld,auf_feld_1,
                            auf_feld_2,z_laenge)) == NOK)
  {
		  norm00_nb(n02_p1->normbyte,NB_NR);
      return(ret_code);
  }

      return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_fp                                             */
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
size_t norm02_fp(char *zw_feld, size_t *z_laenge)
#else
size_t norm02_fp(zw_feld, z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
		size_t ret_code=OK;
		size_t       a,i;

/*
		if (*(zw_feld+4) ==  ':')
		{
				ret_code = NOK;
				return (ret_code);
		}
				else ret_code = OK;
*/
		a = strlen(zw_feld);
		*z_laenge = a;

		if  (*z_laenge > 25)
		{
				/*ba09.05.2003 */
				if ( zw_feld[3] == '.')    
				{
					ret_code = OK;
						for (i=25;i < (*z_laenge) ;i++)
						{
							if (zw_feld[i] != ' ')
							{
								ret_code =NOK;
							}

						}

				}
				else
				{
				ret_code = NOK;
				}
		}
		else
		{
				ret_code = OK;
		}
		for (i=4;i < (*z_laenge+1) ;i++)
		{
			if (zw_feld[i] == '$')
			{
				zw_feld[i] = '*';
			}
		}
/*Ba23.04.2003 */ 
		for (i=4;i < (*z_laenge+1) ;i++)
		{
			if ( zw_feld[i] == ':')
			{
		    
				if ( zw_feld[3] == '.')    
				{	
				}
				else
				{
					if (i > 17)
					{
					ret_code = NOK;
					}
				}
	        break;
			}  

		} 

				return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_all                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Fremdsachnummern mit 2. Doppelpunkt */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */

#ifdef __STDC__
size_t norm02_all(char *zw_feld, char *ein_feld, char *auf_feld_1,
                     char *auf_feld_2,size_t *z_laenge)
#else
size_t norm02_all(zw_feld, ein_feld, auf_feld_1,
                     auf_feld_2,z_laenge)
char *zw_feld;
char *ein_feld;
char *auf_feld_1;
char *auf_feld_2;
size_t *z_laenge;

#endif

{
	size_t	ret_code = OK;
	size_t	i,i11,i12,j,k,l;
	char	c;
	char	zw_ein_feld[8];
	auf_feld_1[0] = ' ';
	auf_feld_2[0] = ' ';

/* -------------------------------------------------------------------------- */
/*    Verarbeitung : "Kein Barcode"                                            */
/* -------------------------------------------------------------------------- */
/*ba14.01.2004 AA8:1234567890124:99888-K   bringt fehler*/
	if (zw_feld[3] == ':')
	{
	    for (i11 = (*z_laenge - 1); i11 > 3 ; i11--)
		  {
		    if (   zw_feld[i11] == ':')
		    {
		      break;
		    }
      }
			if (i11 == 3)
	    {
		    i12 = *z_laenge;
			}
	    else
	    {
		    i12 = i11;
		    ret_code = NOK;
	    }

	    for (i=4;i < i12 ;i++)
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
  }
/* -------------------------------------------------------------------------- */
/*    Verarbeitung : " Barcode"                                            */
/* -------------------------------------------------------------------------- */
   if (zw_feld[3] == '-')
   {
	   zw_feld[3] = ':';

	   for (i=4;i < *z_laenge; i++)
	   {

		    if (   zw_feld[i] == KLEINAE
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
   }

/* -------------------------------------------------------------------------- */
/*    Entscheidung ueber weitere Verarbeitung - 4. und. 5. Block              */
/* -------------------------------------------------------------------------- */

     /* ret_code == OK : es gibt keinen 4. und 5. Block  */

		 if (ret_code == OK)
		 {
		    return(ret_code);
		 }
		 else
		 {
				ret_code = OK;
		 }
/* -------------------------------------------------------------------------- */
/*    2. Doppelpunkt uebertragen                                              */
/* -------------------------------------------------------------------------- */

			j=0;

			auf_feld_1[j] = zw_feld[i];
			auf_feld_2[j] = zw_feld[i];

/* -------------------------------------------------------------------------- */
/*    B l o c k  4 - 5 Analoge Verarbeitung wie bei SSN , Zugriff auf         */
/*    auf das Eingabefeld (mit SPACE)                                          */
/*    1. Schritt Adresse vom 2. Doppelpunkt sichern                            */
/* -------------------------------------------------------------------------- */

			i = i11;

/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  Umwandeln 'I' nach 'J' und 'O'nach NULL              */
/* -------------------------------------------------------------------------- */


		 j++;
		 i++;
/*  Aenderung am 27.9.93, Gerland, ein_feld in zw_feld geaendert   */
/*  21.3.94, Gerland, Aenderung vom 27.9.94 rueckgaengig gemacht   */
/*  aufgrund Fehlermeldg v. Hr. Weckerle zur Nummer :              */
/*  lgw:aa01-a00000a:a 1234                                        */

		 if (ein_feld[i] == '\0')
		 {
				ret_code = NOK;
				return (ret_code);
		 }
		 i--;
		 for (; ein_feld[i] != '\0'; i++)
		 {
				 if (ein_feld[i] == ':')
					 {
						 i++;
						 break;
					 }
     }


     for (k=i, l=0; ein_feld[k] != '\0' && l < 7; k++,l++)
		 {
		    if (isalpha (c = ein_feld[k]))
		    {
					zw_ein_feld[l] = toupper (c = ein_feld[k]);
					continue;
		    }

				if (ein_feld[k] == 'I')
		    {
		      zw_ein_feld[l] = 'J';
		      continue;
		    }

				zw_ein_feld[l] = ein_feld[k];

				if (zw_ein_feld[l] == '$')
					{
						 zw_ein_feld[l] = '*';
					}
		 }

		zw_ein_feld[l] = '\0';

/* 21.3.94, Gerland, siehe oben  */

/*   if (zw_feld[i] == '\0')
	   {
		    ret_code = NOK;
		    return (ret_code);
		 }


     for (k=i, l=0; zw_feld[k] != '\0' && l < 7; k++,l++)
		 {
		    if (isalpha (c = zw_feld[k]))
		    {
		      zw_ein_feld[l] = toupper (c = zw_feld[k]);
		      continue;
		    }

		    if (zw_feld[k] == 'I')
		    {
		      zw_ein_feld[l] = 'J';
		      continue;
		    }

		    zw_ein_feld[l] = zw_feld[k];
		 }

		zw_ein_feld[l] = '\0';
*/

/* -------------------------------------------------------------------------- */
/* Loeschen der Blanks am Ende der Sachnummer                                 */
/* -------------------------------------------------------------------------- */

		if (zw_ein_feld[l-1] ==  ' ')
		{
        for (l--;zw_ein_feld[l] == ' ';l--)
			  {
			    zw_ein_feld[l] = zw_ein_feld[l+1];
			  }
		}

/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  Stelle 1                                             */
/* -------------------------------------------------------------------------- */
   i = 0;
   if (zw_ein_feld[i] == '*')
   {
      auf_feld_1[j] = ' ';
      auf_feld_2[j] = ' ';
      j++;
	    auf_feld_1[j] = zw_ein_feld[i];
	    auf_feld_2[j] = zw_ein_feld[i];
	    i++;
      i++;
	    j++;
      goto BLOCK_5;
   }
   else
   {
	   if  ((isalpha (c = zw_ein_feld[i]))
	       || (isdigit (c = zw_ein_feld[i]))
	       || zw_ein_feld[i] == '+'
	       || zw_ein_feld[i] == ' ')
	   {
	       auf_feld_1[j] = zw_ein_feld[i];
	       auf_feld_2[j] = zw_ein_feld[i];
	       i++;
	       j++;
	   }
	   else
	   {
	       ret_code = NOK;
	       return (ret_code);
	   }
   }
/* -------------------------------------------------------------------------- */
/*	 2.Stelle '-'                                                         */
/* -------------------------------------------------------------------------- */

   if (zw_ein_feld[i] == '-')
	 {
	    if (  auf_feld_1[j-1] == ' '
	       || auf_feld_1[j-1] == '0')
	    {
	        ret_code=NOK;
		      return (ret_code);
	    }
	    if (auf_feld_1[j-1] == '*'
	       || (isdigit (c=auf_feld_1[j-1])))
	    {
	        auf_feld_1[j]   = auf_feld_1[j-1];
		      auf_feld_1[j-1] = ' ';
	        auf_feld_2[j]   = auf_feld_2[j-1];
		      auf_feld_2[j-1] = ' ';
	    }
	    else
	    {
	        auf_feld_1[j]   = auf_feld_1[j-1];
		      auf_feld_1[j-1] = ' ';
	        auf_feld_2[j] = ' ';
	    }

	    goto BLOCK_5;
   }

/* -------------------------------------------------------------------------- */
/*	 2.Stelle kein Bindestrich                                            */
/* -------------------------------------------------------------------------- */

   if (zw_ein_feld[i] == '\0')
	 {
      if (auf_feld_1[j-1] == ' ')
		  {
		      ret_code=NOK;
		      return (ret_code);
		  }

      if (auf_feld_1[j-1] == '0')
		  {
		      auf_feld_1[j-2] = ' ';
		      auf_feld_2[j-2] = ' ';
		      zw_feld[i11] = ' ';
          zw_feld[i11+1] = ' ';
		      ret_code = OK;
		      return (ret_code);
		  }
      /* Ausrichtung nach alter Form */
	    if (    auf_feld_2[j-1] == '*'
		         || (isdigit (c=auf_feld_2[j-1])))
		  {
		        auf_feld_2[j]   = auf_feld_2[j-1];
		        auf_feld_2[j-1] = ' ';
		  }
	    else
		  {
		        auf_feld_2[j] = ' ';
		  }
      /* Ausrichtung nach neuer Form */
		  auf_feld_1[j]   = auf_feld_1[j-1];
		  auf_feld_1[j-1] = ' ';

		  auf_feld_1[3] = ' ';
		  auf_feld_1[4] = ' ';
		  auf_feld_1[5] = ' ';
		  auf_feld_1[6] = ' ';
		  auf_feld_1[7] = '\0';

      auf_feld_2[3] = ' ';
		  auf_feld_2[4] = ' ';
		  auf_feld_2[5] = ' ';
		  auf_feld_2[6] = ' ';
		  auf_feld_2[7] = '\0';
		  ret_code=OK;
		  return(ret_code);

	  }

	  if ((isalpha (c = zw_ein_feld[i]))
	       || (isdigit (c = zw_ein_feld[i]))
	       || zw_ein_feld[i] == '+'
	       || zw_ein_feld[i] == '*'
	       || zw_ein_feld[i] == ' ')
	  {
	       auf_feld_1[j] = zw_ein_feld[i];
	       auf_feld_2[j] = zw_ein_feld[i];
	       i++;
	       j++;
	  }
	  else
	  {
	       ret_code = NOK;
	       return (ret_code);
	  }
/* -------------------------------------------------------------------------- */
/*    Block 4   Fuehrende Nullen entfernen                                     */
/* -------------------------------------------------------------------------- */

   if (   auf_feld_1[j-1] == '0'
		      && auf_feld_1[j-2] == '0' )
	 {
		 if (zw_ein_feld[i] == '\0')
		 {
		    auf_feld_1[j-3] = ' ';
		    auf_feld_2[j-3] = ' ';
		    zw_feld[i11] = ' ';
		    zw_feld[i11+1] = ' ';
		    zw_feld[i11+2] = ' ';
		    ret_code = OK;
		    return (ret_code);
		 }
		 else
     {
	      ret_code = NOK;
	      return (ret_code);
	   }
	 }

	 if (auf_feld_1[j-2] == '0')
	 {
		 auf_feld_1[j-2] =  ' ';
		 auf_feld_2[j-2] =  ' ';
	 }

/* -------------------------------------------------------------------------- */
/*    Block 4   Ausrichten bei nur einem Zeichen                              */
/*              rechtsbuendig bei numerischen Werten und Sonderzeichen        */
/*              rechtsbuendig bei alphanumer. Werten und Sonderzeichen        */
/* -------------------------------------------------------------------------- */
/*	    if (auf_feld_1[j-2] == ' '
		     && auf_feld_1[j-1] != ' ')
		  {
		    auf_feld_1[j-2] =  auf_feld_1[j-1];
		    auf_feld_1[j-1] =  ' ';
		  }
*/


      /* Ausrichtung sach_nr (neue Form)                              */

	    if (auf_feld_1[j-1] == ' '
				 && auf_feld_1[j-2] != ' ')
		  {
		    auf_feld_1[j-1] =  auf_feld_1[j-2];
		    auf_feld_1[j-2] =  ' ';
		  }

      /* Ausrichtung sach_nr_alt (alte Form)                          */

      if (auf_feld_2[j-2] == ' ')
      {
        if (isalpha (c = auf_feld_2[j-1])
           || auf_feld_2[j-1] == '+')
        {
          auf_feld_2[j-2] = auf_feld_2[j-1];
          auf_feld_2[j-1] = ' ';
        }
      }



/* -------------------------------------------------------------------------- */
/*    Block 4   Ausrichten bei 'A0' = ' A'           			      */
/*              rechtbuendig bei NULL nach dem alphanumerischen Wert          */
/* -------------------------------------------------------------------------- */

/*  Ausrichtung nach alter Form */

   if (auf_feld_2[j-1] == '0')
	 {
		 if ( isalpha (c = auf_feld_2[j-2])
		      || auf_feld_2[j-1] == '+')
		 {
		    auf_feld_2[j-1] =  ' ';
		 }
	 }
/* Ausrichtung nach neuer Form */

   if (auf_feld_1[j-1] == '0')
	 {
		 if ( isalpha (c = auf_feld_1[j-2])
					|| auf_feld_1[j-1] == '+')
		 {
        auf_feld_1[j-1] = auf_feld_1[j-2];
		    auf_feld_1[j-2] = ' ';

		 }
	 }




BLOCK_5:
       auf_feld_1[3] = '\0';
       auf_feld_2[3] = '\0';

/* -------------------------------------------------------------------------- */
/*    B l o c k  5    -  Stelle 1                                             */
/* -------------------------------------------------------------------------- */
   if (zw_ein_feld[i] == '-')
	 {
			if (zw_ein_feld[i+1] != '\0')
	    {
	       i++;
	    }
	    else
	    {
	       ret_code = NOK;
	       return(ret_code);
	    }
	 }




		for (j=3 ;zw_ein_feld[i] != '\0' && j < 8; i++)

		{
		    if ((isalpha (c = zw_ein_feld[i]))
			     || (isdigit (c = zw_ein_feld[i]))
			     || zw_ein_feld[i] == '+'
					 || zw_ein_feld[i] == '*'
			     || zw_ein_feld[i] == '-'
			     || zw_ein_feld[i] == '.')
			  {
			    auf_feld_1[j] = zw_ein_feld[i];
			    auf_feld_2[j] = zw_ein_feld[i];
          j++;
			  }
		    else
			  {
          if (zw_ein_feld[i] != ' ')
          {
			      ret_code = NOK;
			      return (ret_code);
          }
			  }
		}
		if (j > 7)
		{
			ret_code = NOK;
			return (ret_code);
		}
		else
		{
			k = 7 - j;
		}

		switch (k)
    {
		  case 3:   auf_feld_1[j+3] = auf_feld_1[j-1];
					      auf_feld_1[j+2] = ' ';
					      auf_feld_1[j+1] = ' ';
					      auf_feld_1[j]   = ' ';
					      auf_feld_1[j-1] = '-';
                auf_feld_1[j+4] = '\0';
                auf_feld_2[j+3] = auf_feld_2[j-1];
					      auf_feld_2[j+2] = ' ';
					      auf_feld_2[j+1] = ' ';
					      auf_feld_2[j]   = ' ';
								auf_feld_2[j-1] = '-';
                auf_feld_2[j+4] = '\0';
					      break;
			case 2:   auf_feld_1[j+2] = auf_feld_1[j-1];
					      auf_feld_1[j+1] = auf_feld_1[j-2];
					      auf_feld_1[j-1] = ' ';
					      auf_feld_1[j]   = ' ';
					      auf_feld_1[j-2] = '-';
                auf_feld_1[j+3] = '\0';
                auf_feld_2[j+2] = auf_feld_2[j-1];
					      auf_feld_2[j+1] = auf_feld_2[j-2];
					      auf_feld_2[j]   = ' ';
					      auf_feld_2[j-1] = ' ';
					      auf_feld_2[j-2] = '-';
                auf_feld_2[j+3] = '\0';
					      break;
			case 1:   auf_feld_1[j+1] = auf_feld_1[j-1];
					      auf_feld_1[j]   = auf_feld_1[j-2];
					      auf_feld_1[j-1] = auf_feld_1[j-3];
					      auf_feld_1[j-2] = ' ';
								auf_feld_1[j-3] = '-';
                auf_feld_1[j+2] = '\0';
                auf_feld_2[j+1] = auf_feld_2[j-1];
					      auf_feld_2[j]   = auf_feld_2[j-2];
					      auf_feld_2[j-1] = auf_feld_2[j-3];
					      auf_feld_2[j-2] = ' ';
					      auf_feld_2[j-3] = '-';
                auf_feld_2[j+2] = '\0';
					      break;
			case 0:   auf_feld_1[j]   = auf_feld_1[j-1];
					      auf_feld_1[j-1] = auf_feld_1[j-2];
					      auf_feld_1[j-2] = auf_feld_1[j-3];
					      auf_feld_1[j-3] = auf_feld_1[j-4];
					      auf_feld_1[j-4] = '-';
                auf_feld_1[j+1] = '\0';
                auf_feld_2[j]   = auf_feld_2[j-1];
					      auf_feld_2[j-1] = auf_feld_2[j-2];
					      auf_feld_2[j-2] = auf_feld_2[j-3];
					      auf_feld_2[j-3] = auf_feld_2[j-4];
					      auf_feld_2[j-4] = '-';
								auf_feld_2[j+1] = '\0';
					      break;


		}


/* -------------------------------------------------------------------------- */
/*    Block 5   Fuehrende Nullen entfernen                                     */
/* -------------------------------------------------------------------------- */
		for (j=4;j<8;j++)
		{
		    if (auf_feld_1[j] == '0'
			     || auf_feld_1[j] == ' ')
			  {
			    auf_feld_1[j] = ' ';
			    auf_feld_2[j] = ' ';
			  }
		    else
			  {
					break;
			  }
		}
    if ((auf_feld_1[3] == '-') &&
        (auf_feld_1[4] == ' ') &&
        (auf_feld_1[5] == ' ') &&
        (auf_feld_1[6] == ' ') &&
        (auf_feld_1[7] == ' '))
    {
      ret_code = NOK;
      return(ret_code);
    }

/* -------------------------------------------------------------------------- */
/*    Block 5   Fehlerpruefung auf 5.Block groesser 1 Stelle                 */
/* -------------------------------------------------------------------------- */
/*		    if (auf_feld_1[6] == ' ')
			{
			ret_code = NOK;
			return (ret_code);
			}
*/
/* -------------------------------------------------------------------------- */
/*    Block 5   Ende - Bearbeitung                                           */
/* -------------------------------------------------------------------------- */

   auf_feld_1[8] = '\0';
   auf_feld_2[8] = '\0';


  c=c; /*vermeidet Warnings*/
  return (ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm02_afm      Aufbereitung MASCHINELL               */
/*                      norm02_afd      Aufbereitung DRUCK                    */
/*                      norm02_afs      Aufbereitung SORTIERT                 */
/*                      norm02_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm02_af1(norm_p1 *n02_p1,char *zw_feld,char *auf_feld_1,
                     char *auf_feld_2,size_t *z_laenge)
#else
size_t norm02_af1(n02_p1,zw_feld,auf_feld_1,auf_feld_2,z_laenge)
norm_p1 *n02_p1;
char *zw_feld;
char *auf_feld_1;
char *auf_feld_2;
size_t *z_laenge;
#endif
{
    size_t       ret_code = NOK;

        /* Auswahl */

 
		     norm02_afm(zw_feld,n02_p1->m_norm,auf_feld_1,z_laenge);
   		     norm02_afm(zw_feld,n02_p1->ma_norm,auf_feld_2,z_laenge);
             norm02_afd(zw_feld,n02_p1->d_norm,auf_feld_1,z_laenge);
		     norm02_afs(zw_feld,n02_p1->s_norm,auf_feld_1,z_laenge);
		     norm02_afs(zw_feld,n02_p1->sa_norm,auf_feld_2,z_laenge);
		     norm02_afb(zw_feld,n02_p1->b_norm,auf_feld_1,z_laenge);
             ret_code = OK;
 

	norm02_normby(n02_p1,auf_feld_1);

  return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_afm                                            */
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
size_t norm02_afm(char *zw_feld,char *aus_feld,char *auf_feld,size_t *z_laenge)
#else
size_t norm02_afm(zw_feld,aus_feld,auf_feld,z_laenge)
char *zw_feld;
char *aus_feld;
char *auf_feld;
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i,i2,j;

	i=0;
	while (i<4)
	{
	    aus_feld[i]=zw_feld[i];
			i++;
	}


	if (auf_feld[0] != ':')
	{

		for (i = 4; i < 25; i++)
		{
			 if (i < *z_laenge)
					aus_feld[i] = zw_feld[i];
			 else
					aus_feld[i] = ' ';
		}
			 aus_feld[i]='\0';
	}
	else
	{

		for (i=4; (zw_feld[i]!=':'); i++)
		{
					aus_feld[i]=zw_feld[i];
		}

		i2 = i;

		for (++i2; i2 < *z_laenge; i2++)
		{
		if (zw_feld[i2] == ':')
		{
			 aus_feld[i] = zw_feld[i];

			 for (++i; zw_feld[i]!=':'; i++)
			 {
					 aus_feld[i]=zw_feld[i];
			 }
		}
	}



	if (i<17)
	{
			for (;i<17;i++)
			{
					aus_feld[i] = ' ';
			}
	}

	if (i == 17)
	{
			for(j=0;j<8;i++, j++)
			{
				if (auf_feld[j] == '\0')
				{
					break;
				}
				if (auf_feld[j] == '-')
				{
					aus_feld[i] = '-';
				}
				else
				{
			if (auf_feld[j] == '$')
			{
				aus_feld[i] = '*';
			}
			else
			{
				aus_feld[i]=auf_feld[j];
			}
				}
			}
		}
		else
		{

		  for(j=0;j<8;j++)
		  {
		    if (auf_feld[j] == '\0')
		    {
		      break;
		    }
		    if (auf_feld[j] != ' ')
		    {
		      aus_feld[i]=auf_feld[j];
		      i++;
		    }
		    if (i==25)
		    {
		      break;
		    }
		  }
		}
	}
	for (; i<25; i++)
	{
	    aus_feld[i] = ' ';
	}

	aus_feld[25] = '\0';


	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_afd                                            */
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
size_t norm02_afd(char *zw_feld,char *aus_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm02_afd(zw_feld,aus_feld,auf_feld_1,z_laenge)
char *zw_feld;
char *aus_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       i2=0;
	size_t       j=0;

	while (i<4)
	{
	    aus_feld[i]=zw_feld[i];
	    i++;
	}


	if (auf_feld_1[0] != ':')
	{
		for (i = 4; i < 25; i++)
		{
		   if (i < *z_laenge)
		      aus_feld[i] = zw_feld[i];
		   else
		      aus_feld[i] = ' ';
		}
		   aus_feld[i]='\0';
	}
	else
	{
		for (i=4; zw_feld[i]!=':'; i++)
		{
		  aus_feld[i]=zw_feld[i];
		}


		i2 = i;

	  if (i < 25)
		{
		   for (++i2; i2 < *z_laenge; i2++)
		   {
		      if (zw_feld[i2] == ':')
		      {
		         aus_feld[i] = zw_feld[i];

		         for (++i; zw_feld[i]!=':'; i++)
			 {
								aus_feld[i]=zw_feld[i];
			 }
					}
		   }
		   for (j=0; auf_feld_1[j] != '\0'; j++)
		   {

/*			if ( j == 3  )
			{
			   aus_feld[i] = '-';
			   i++;
			}
*/
			if (auf_feld_1[j] ==  ' ')
		  {
			   continue;
		  }
			else
		  {
			    if (auf_feld_1[j] == '$')
					{
			      aus_feld[i] = '*';
					}
			    else
			    {
			      aus_feld[i] = auf_feld_1[j];
			    }
			    i++;
			}
	  }
	  if (aus_feld[i-1] == '-')
		{
		    i--;
		}

	  for (; i<25; i++)
	  {
				aus_feld[i] = ' ';
		}
	 }
	 aus_feld[25] = '\0';
	}


	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_afs                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SORTIERT                                 */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*      Aenderung:      19.1.94,Gerland, Fehlermeldung v. Hr.Gojowsky,17.1.94 */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm02_afs(char *zw_feld,char *aus_feld,char *auf_feld,size_t *z_laenge)
#else
size_t norm02_afs(zw_feld,aus_feld,auf_feld,z_laenge)
char *zw_feld;
char *aus_feld;
char *auf_feld;
size_t *z_laenge;
#endif
{
	size_t	ret_code = OK;
	size_t	i=0;
	size_t	i2=0;
	size_t	j=0;
	size_t	k;
	size_t	a=0;
	k  = *z_laenge;

	while (i<4)
	{
			aus_feld[i]=zw_feld[i];
	    i++;
	}
	k -= 1;
/* 19.1.94, Gerland, folgenden if eingefgt, kopiert aus norm02_afm()  */
  if (auf_feld[0] != ':')
	{

		for (i = 4; i < 25; i++)
		{
		   if (i < *z_laenge)
		      aus_feld[i] = zw_feld[i];
			 else
		      aus_feld[i] = ' ';
		}
		   aus_feld[i]='\0';
	}

/* 19.1.94,Gerland, folgenden if entfernt :
	if (auf_feld[0] != ':')
	{
	   for (i=24;i>3;i--,k--)
	   {
		  if (k > 3)
		      aus_feld[i] = zw_feld[k];
		  else
		      aus_feld[i] = ' ';
		 }
	   aus_feld[25] = '\0';
	}
*/
	else
	{
	  k=4;
	  while (zw_feld[k] != ':')
	  {
			k++;
	  }
	  i2 = k;
    ++i2;
		while (zw_feld[i2] != '\0')
	  {
	    if (zw_feld[i2] == ':')
	    {
	      k=i2;
	      break;
	    }
	    else
	    {
	      i2++;
			}
	  }
		a=k;
	  k--;
    for (i=k;i>3;i--,k--)
	  {
			if (k > 3)
			   aus_feld[i] = zw_feld[k];
	    else
		      aus_feld[i] = ' ';
		}

		if (i<17)
		  i=a;
		{
		  if (auf_feld[0] == ':')
		  {
		    for (;i<17;i++)
		    aus_feld[i] = ' ';
		    a=i;
			}
		}
			 i=a;
	 if (i==17)
	 {
	 for (i=a,j=0;j<8;j++,i++)
			 {
	       if (auf_feld[j] == '$')
	     {
	       aus_feld[i] = '*';
			 }
	     else
	     {
	     aus_feld[i] = auf_feld[j];
	     }
	     if (auf_feld[j] == '\0')
		{
		break;
		}
	     if (i==25)
			 {
	     break;
			 }
	     }
	}
	else
	{
	 for (i=a,j=0;j<8;j++)
	 {
	  if (auf_feld[j] != ' ')
		{
	     if (auf_feld[j] == '\0')
		  {
		      break;
		  }
	     aus_feld[i] = auf_feld[j];
	     i++;
	     }
		 if (i==25)
		 {
				break;
	         }
			 }
	   }
	   for (; i<25; i++)
	   {
				aus_feld[i] = ' ';
		 }
			aus_feld[25] = '\0';
	}

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm02_afb                                            */
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
size_t norm02_afb(char *zw_feld,char *aus_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm02_afb(zw_feld,aus_feld,auf_feld_1,z_laenge)
char *zw_feld;
char *aus_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t	ret_code = OK;
	size_t	i=0;
	size_t	i2=0;
	size_t	j=0;
	char	c;

	while (i<4)
			{
	    aus_feld[i] = zw_feld[i];
	    i++;
			}
	if (aus_feld[3] == ':')
	    {
	    aus_feld[3] = '-';
	    }

	if (auf_feld_1[0] != ':')
		{
		for (i = 4; zw_feld[i] != '\0'; i++)
		  {
		    if (   (isalpha (c = zw_feld[i]))
			|| (isdigit (c = zw_feld[i]))
			|| zw_feld[i] ==  ' '
			|| zw_feld[i] ==  '-'
			|| zw_feld[i] ==  '.'
			|| zw_feld[i] ==  '$'
			|| zw_feld[i] ==  '/'
			|| zw_feld[i] ==  '+'
			|| zw_feld[i] ==  '.')
			{
			 aus_feld[i] = zw_feld[i];
			}
		     else
			{
			 if (zw_feld[i] == '*')
			 {
			  aus_feld[i] = '$';
			 }
			 else
			 {
			  aus_feld[i] = '-';
			 }
			}
		   }


		for (; i < 25; i++)
			{
		  aus_feld[i] = ' ';
		  }

		  aus_feld[i]='\0';

		}
	else
	{
		for (i=4; zw_feld[i]!=':'; i++)
		{
			 if (   (isalpha (c = zw_feld[i]))
				|| (isdigit (c = zw_feld[i]))
				 || zw_feld[i] ==  '-'
				 || zw_feld[i] ==  '.'
				 || zw_feld[i] ==  '$'
				 || zw_feld[i] ==  '/'
				 || zw_feld[i] ==  '+'
				 || zw_feld[i] ==  '.')
			{
				 aus_feld[i] = zw_feld[i];
			}
		   else
			{
			    if (zw_feld[i] == '*')
			    {
		    	   aus_feld[i] = '$';
			    }
					else
			    {
						aus_feld[i] = '-';
			    }
			}
		}
		
      i2 = i;

		for (++i2; i2 < *z_laenge; i2++)
		{
			 if (zw_feld[i2] == ':')
		   {
		      aus_feld[i] = '-';

					for (++i; zw_feld[i]!=':'; i++)
		      {
		          if (   (isalpha (c = zw_feld[i]))
			       || (isdigit (c = zw_feld[i]))
                || zw_feld[i] ==  '-'
						 || zw_feld[i] ==  '.'
			       || zw_feld[i] ==  '$'
						 || zw_feld[i] ==  '/'
			       || zw_feld[i] ==  '+'
			       || zw_feld[i] ==  '.')
			       {
			         aus_feld[i] = zw_feld[i];
                }
		          else
			       {
			         aus_feld[i] = '-';
						 }
		      }
		   }
		}

		auf_feld_1[0] = '-';

		for (j=0; auf_feld_1[j] != '\0'; j++)
		{
/*			if ( j == 3 )
         {
				 aus_feld[i] = '-';
			   i++;
			}
*/
			if (auf_feld_1[j] ==  ' ')
		   {
      		continue;
		   }
			else
			 {
			    aus_feld[i] = auf_feld_1[j];
             if (aus_feld[i] == '*')
			    {
						 aus_feld[i] = '$';
					}
					i++;
				}
				}
			 if (aus_feld[i-1] == '-')
		{
		i--;
		}

		 for (; i<25; i++)
		 {
				aus_feld[i] = ' ';
		 }

		 aus_feld[25] = '\0';
	}

	for (i=0;i<25;i++)
	{
	if (aus_feld[i] == '*')
	{
			aus_feld[i] = '$';
	}
	}



	c=c;/*vermeidet Warnings*/
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_normnb                                            */
/*                                                                            */
/*      Aufgabe:        Normbyte_Belegung                                      */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*       BA16.12.2002    IB bei "%" an der 4. Stelle                          */
/*       BA08.04.2003    IB bei "." an der 4. Stelle Änderung des CR vom 16.12  */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm02_normby(norm_p1 *n02_p1,char *auf_feld_1)
#else
size_t norm02_normby(n02_p1,auf_feld_1)
norm_p1 *n02_p1;
char *auf_feld_1;
#endif
{
	   size_t ret_code=OK;
	if (n02_p1->m_norm[3] == '.')		
	{
	  norm00_nb(n02_p1->normbyte,"IB");
	}
	else
		{
		if (auf_feld_1[0] != ':'
			   && auf_feld_1[0] != '-')
		{
				  norm00_nb(n02_p1->normbyte,"IZ");
		}
		else
		{
		   if (n02_p1->m_norm[17] == ':')
		   {
			  
			   if (n02_p1->m_norm[20] == '-')
				{
					 norm00_nb(n02_p1->normbyte,"I4");
				}
			   else
				{
							 norm00_nb(n02_p1->normbyte,"I1");
				}
		   }
		   else
		   {
			  norm00_nb(n02_p1->normbyte,"IZ");
		   }
		}
	}
return (ret_code);
}
