/* -------------------------------------------------------------------------- */
/*      Modul:  norm08                                                        */
/*                                                                            */
/*      Entry:  norm08                                                        */
/*                                                                            */
/*      Aufgabe:        Aufbereitung Sachnummer fÅr militÑrisches GerÑt       */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Androleit                                             */
/*                      DI AP 623                                             */
/*                                                                            */
/*      Version:        1.0 vom 07.04.92                                      */
/*      ƒnderung        2,60 Besetigung von Warnungen 05.10.01 Androleit      */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/*      Allgemeine Definitionen                                               */
/* -------------------------------------------------------------------------- */
#define OK 0
#define NOK 1

#define NULL 0

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
size_t norm08_fp(char *zw_feld,size_t *z_laenge);
size_t norm08_auf(char *einfeld,char *block1,char *block3);
size_t norm08_af1(norm_p1 *n01_p1);
size_t norm08_all(char *block1,char *block3);
size_t norm08_afm(char *sachnr,char *aus_feld);
size_t norm08_afd(char *aus_feld);
size_t norm08_afs(char *sachnr,char *aus_feld);
size_t norm08_afb(char *aus_feld);
size_t norm08_normby(norm_p1 *n01_p1);

/* #include <stdio.h>
#include <string.h>
#include <ctype.h> */
#else
size_t norm08_fp();
size_t norm08_auf();
size_t norm08_af1();
size_t norm08_all();
size_t norm08_afm();
size_t norm08_afd();
size_t norm08_afs();
size_t norm08_afb();
size_t norm08_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      Sachnummer fÅr militÑrisches GerÑt                    */
/*                                                                            */
/*      Aufrufe:        norm08_fp       Fehlerpruefung                        */
/*                      norm08_auf      Aufbereitung der Bloecke               */
/*			- Entfernen der SPACES in Block1 und 2                */
/*                      norm08_all      Allgemeine Verarbeitung               */
/*                      norm08_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
char sach_nr[26];
char sach_nr_alt[26];
#ifdef __STDC__
size_t norm08(norm_p1 *n01_p1, char *zw_feld)
#else
size_t norm08(n01_p1, zw_feld)
norm_p1 *n01_p1;
char *zw_feld;
#endif
{
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	        *z_laenge   =  &laenge;
	char            zblock1_2[14];
	char            *block1 = zblock1_2;
	char            zblock3_5[13];
	char            *block3 = zblock3_5;

	if((ret_code = norm08_fp(zw_feld,z_laenge)) == NOK)
	 {
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	 }
	if((ret_code = norm08_auf(n01_p1->eingfeld,block1,block3)) == NOK)
	 {
		norm00_nb(n01_p1->normbyte,NB_NR);
								return(ret_code);
	 }
	if((ret_code = norm08_all(block1,block3)) == NOK)
	 {
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	 }


	if((ret_code = norm08_af1(n01_p1)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
								return(ret_code);
	}


			return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_fp                                             */
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
size_t norm08_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm08_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	size_t	ret_code=NOK;
	size_t	i,a,st;
	char	c;

	st=0;

/*  PrÅfung:   */

if (((zw_feld[0] == '1')
	 && (zw_feld[1] == '2'))
		|| ((zw_feld[0] == '2')
	 && (zw_feld[1] == '5'))
		|| ((zw_feld[0] == '4')
	 && (zw_feld[1] == '2'))
	 || ((zw_feld[0] == '4')
	 && (zw_feld[1] == '9'))
	 || ((zw_feld[0] == '5')
	 && (zw_feld[1] == '3'))
	 || ((zw_feld[0] == '5')
	 && (zw_feld[1] == '8'))
	 || ((zw_feld[0] == '5')
	 && (zw_feld[1] == '9'))
	 || ((zw_feld[0] == '6')
	 && (zw_feld[1] == '1'))
	 || ((zw_feld[0] == '6')
	 && (zw_feld[1] == '2'))
	 || ((zw_feld[0] == '6')
	 && (zw_feld[1] == '3'))
	 || ((zw_feld[0] == '6')
	 && (zw_feld[1] == '5'))
	 || ((zw_feld[0] == '6')
	 && (zw_feld[1] == '6'))
	 || ((zw_feld[0] == '6')
	 && (zw_feld[1] == '9'))
	 || ((zw_feld[0] == '8')
	 && (zw_feld[1] == '1')))

				{
				}
			else
				{
				ret_code = NOK;
				return (ret_code);
				}

	 if ((zw_feld[2] == 'B')
	 || (zw_feld[2] == 'D')
	 || (zw_feld[2] == 'E')
	 || (zw_feld[2] == 'J')
	 || (zw_feld[2] == 'M')
	 || (zw_feld[2] == 'R')
	 || (zw_feld[2] == 'V')
	 || (zw_feld[2] == 'Y'))

	 {
	 }
	 else
	 {
				ret_code = NOK;
				return (ret_code);
	 }

    if ((isdigit (c=zw_feld[3]))
	    && (isdigit (c=zw_feld[4]))
	    && (isdigit (c=zw_feld[5])))
	{
	}
		else
	{
			ret_code = NOK;
			return (ret_code);
	}

		if ((zw_feld[3] == '0') && (zw_feld[4] == '0') && (zw_feld[5] == '0'))
		{
				ret_code = NOK;
				return (ret_code);
		}
		a = strlen(zw_feld);
		*z_laenge = a;


for (i=0;i <= a;i++)
	{
		if (zw_feld[i] == 'I')
		 {
		 zw_feld[i] = 'J';
		 }
		if (zw_feld[i] == 'O')
		 {
		 zw_feld[i] = '0';
		 }
	}

	if ((zw_feld[6] == '-')
		 && (zw_feld[7] == '\0'))
	{
	  ret_code = NOK;
	  return (ret_code);
	}


	 if  (*z_laenge > 25)
			{
			 ret_code = NOK;
			}
	 else
		 {
				ret_code = OK;
		 }


/* Zus‰tzliche Pr¸fung auf alpha im 2. und 3. Block */

	for (i=3 ; i < a; i++)
	{
		if ((zw_feld[i] == ':') || (zw_feld[i] == '-'))
		{
          i = 26;
		}
		{
		if (isalpha (c = zw_feld[i]))
		{
			ret_code = NOK;
			return (ret_code);
		}
		}
	}

	 c=c;
	 st=st;
	 return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_auf                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Sachnummer fÅr militÑrisches GerÑt  */
/*                      Entfernen der Leerstellen in Block 1 und 2                                                      */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm08_auf(char einfeld[],char *block1,char *block3)
#else
size_t norm08_auf(einfeld, block1, block3)
char einfeld[];
char *block1;
char *block3;
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,z;
	char	c;

/* ---------------------------- */
/*      B l o c k  1  und  2    */
/* -----------------------------*/

	k = 0;
	z = 0;


	for (i = 0; einfeld[i] != '\0'; i++)
	{
		if  (einfeld[i] != ' ')
				{
				 if (einfeld[i] != ':')
				 {
				 if (einfeld[i] == '-')
				 {
					z++;
				 }
				 if (( z == 2 )
						|| (einfeld[i] == ':'))
				 {
					 break;
				 }
				 if ((i>12)
						|| ((i==6) && (isdigit(c=einfeld[i]))))
				 {
				 ret_code=NOK;
				 return(ret_code);
				 }
				 block1[k] = einfeld[i];
/*				 if (islower (c = block1[k]))
						{
						block1[k] = toupper(c);
						} */
				 k++;
					}
					else
					{
				break;
					}
				}
				continue;
		}


		 block1[k] = '\0';
		 block3[0] = '\0';
k++;
							if (einfeld[i] == '\0')
		 {
		 return (ret_code);
		 }

/* ----------------------------------------- */
/*      B l o c k  3  bis  5  - ab Stelle 1  */
/* ------------------------------------------*/
k=0;

		for (; ((einfeld[i] != '\0') && (k < 12));i++,k++)
		{

		 block3[k] = einfeld[i];
		 if (islower (c = block3[k]))
		    {	       
				 block3[k] = toupper(c);
		    }
		}

		    block3[k] = '\0';

/* Loeschen der Blanks am Ende der Sachnummer                 */

		    for (k--; block3[k] == ' ' ;k--)
			{
			block3[k] = block3[k+1];
			}

		    if (k > 11)
			{
			ret_code = NOK;
			}

return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_all                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Sachnummer fÅr militÑrisches GerÑt  */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm08_all(char *block1,char *block3)
#else
size_t norm08_all(block1,block3)
char *block1;
char *block3;
#endif
{
	size_t	ret_code = OK;
	size_t	i,j,k,l,n,b,merk;
	char	wert[7];
	char	c;

/* -------------------------------------------------------------------------- */
/*    B l o c k    1     Stelle 1, 2 und 3                                    */
/* -------------------------------------------------------------------------- */
	 for (i=0;i<25; i++)
	 {
	 sach_nr[i]     = ' ';
	 sach_nr_alt[i] = ' ';
	 }
	 sach_nr[i]     = '\0';
	 sach_nr_alt[i] = '\0';

	 for (i=0; i<6; i++)
	   {
	      sach_nr[i]     = block1[i];
	      sach_nr_alt[i] = block1[i];
	   }
	 
/* -------------------------------------------------------------------------- */
/*    B l o c k  2    -  Stelle 1 - 6  - Auswahl                              */
/* -------------------------------------------------------------------------- */

if (block1[6] == '-')
{
   sach_nr[i]     = '-';
   sach_nr_alt[i] = '-';
   i++;
	for (; block1[i] != '\0';i++)
	{
		if (isdigit ((c = block1[i]))
		     || block1[i] == '*')
		{
		     sach_nr[i]     = block1[i];
		     sach_nr_alt[i] = block1[i];
		}
		else
		{
		    if (block1[i] == '-')
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
}
/* -------------------------------------------------------------------------- */
/*    Fuehrende Nullen entfernen                                               */
/* -------------------------------------------------------------------------- */
		     l=i;
		     if (i > 0)
			{
			b=0;
			for (n=7;n<l;n++,b++)
			  {
			  wert[b] = sach_nr[n];
			  }
			  wert[b] = '\0';
			b=0;
			merk=0;
			for (b=0;b<6 && wert[b] != '\0';b++)
			{
				if (wert[b] == '0')
					{
					merk++;   
					}
				if ((isdigit(wert[b]))
				&& (merk-1) != b)
					{
					break;
					}
			}

			}
			 if (strlen(wert) == 1)
			 {
			 merk=0;
			 }
			 else
			 {
			 }
			 switch(merk) {

				   case 1:    sach_nr[7] = sach_nr[8];
					      sach_nr[8] = sach_nr[9];
					      sach_nr[9] = sach_nr[10];
					      sach_nr[10] = sach_nr[11];
					      sach_nr[11] = sach_nr[12];
					      sach_nr[12] = ' ';
                sach_nr_alt[7] = sach_nr_alt[8];
					      sach_nr_alt[8] = sach_nr_alt[9];
					      sach_nr_alt[9] = sach_nr_alt[10];
					      sach_nr_alt[10] = sach_nr_alt[11];
					      sach_nr_alt[11] = sach_nr_alt[12];
					      sach_nr_alt[12] = ' ';
					      break;
				   case 2:    sach_nr[7] = sach_nr[9];
					      sach_nr[8] = sach_nr[10];
								sach_nr[9] = sach_nr[11];
					      sach_nr[10] =sach_nr[12];
					      sach_nr[11] = ' ';
					      sach_nr[12] = ' ';
                sach_nr_alt[7] = sach_nr_alt[9];
					      sach_nr_alt[8] = sach_nr_alt[10];
								sach_nr_alt[9] = sach_nr_alt[11];
					      sach_nr_alt[10] =sach_nr_alt[12];
					      sach_nr_alt[11] = ' ';
					      sach_nr_alt[12] = ' ';
					      break;
				   case 3:    sach_nr[7] = sach_nr[10];
					      sach_nr[8] = sach_nr[11];
					      sach_nr[9] = sach_nr[12];
					      sach_nr[10] = ' ';
					      sach_nr[11] = ' ';
					      sach_nr[12] = ' ';
                sach_nr_alt[7] = sach_nr_alt[10];
					      sach_nr_alt[8] = sach_nr_alt[11];
					      sach_nr_alt[9] = sach_nr_alt[12];
					      sach_nr_alt[10] = ' ';
					      sach_nr_alt[11] = ' ';
					      sach_nr_alt[12] = ' ';
					      break;
				   case 4:    sach_nr[7] = sach_nr[11];
					      sach_nr[8] = sach_nr[12];
					      sach_nr[9] =  ' ';
					      sach_nr[10] = ' ';
					      sach_nr[11] = ' ';
					      sach_nr[12] = ' ';
                sach_nr_alt[7] = sach_nr_alt[11];
					      sach_nr_alt[8] = sach_nr_alt[12];
					      sach_nr_alt[9] =  ' ';
					      sach_nr_alt[10] = ' ';
					      sach_nr_alt[11] = ' ';
					      sach_nr_alt[12] = ' ';
					      break;
				   case 5:    sach_nr[7] = sach_nr[12];
								sach_nr[8] = ' ';
					      sach_nr[9] = ' ';
					      sach_nr[10] = ' ';
					      sach_nr[11] = ' ';
					      sach_nr[12] = ' ';
                sach_nr_alt[7] = sach_nr_alt[12];
								sach_nr_alt[8] = ' ';
					      sach_nr_alt[9] = ' ';
					      sach_nr_alt[10] = ' ';
					      sach_nr_alt[11] = ' ';
					      sach_nr_alt[12] = ' ';
					      break;
				   case 6:    sach_nr[7] = sach_nr[12];
					      sach_nr[8] = ' ';
					      sach_nr[9] = ' ';
					      sach_nr[10] = ' ';
					      sach_nr[11] = ' ';
					      sach_nr[12] = ' ';
                sach_nr_alt[7] = sach_nr_alt[12];
					      sach_nr_alt[8] = ' ';
					      sach_nr_alt[9] = ' ';
					      sach_nr_alt[10] = ' ';
					      sach_nr_alt[11] = ' ';
					      sach_nr_alt[12] = ' ';
					      break;
					}
			
					     
/*	    BLOCK_3: */
/* -------------------------------------------------------------------------- */
/*    B l o c k  3    -  Stelle 1-3 - Auswahl                                   */
/* -------------------------------------------------------------------------- */
		 i=8;
	   for (; i<13;i++)
	   {
	   if (sach_nr[i] == '\0')
	   {
	    sach_nr[i]     = ' ';
 	    sach_nr_alt[i] = ' ';
           }
	   }

	   k=i;
	   l=i+4;
	   for (j=0; j<l; i++,j++)
		{
 if ((j==4) && (block3[j] != '\0') && (block3[j] != ':'))
		 {
		ret_code = NOK;
		return (ret_code); 
		 }  
		 if (isdigit (c = block3[j])
		   || (block3[j] == '-'))
			 {
		     sach_nr[i]     = block3[j];
		     sach_nr_alt[i] = block3[j];
		     continue;
		    }

		 if (block3[j] == '*')
		    {
					sach_nr[i]     = block3[j];
					sach_nr_alt[i] = block3[j];
		      continue;
		     /* Normbyte setzen )*/
		    }

		 if (block3[j] == ' ')
		    {
		     sach_nr[i]     = block3[j];
		     sach_nr_alt[i] = block3[j];
		     continue;
		    }


		     if ((block3[j] == ':')
					|| (block3[j] == '\0'))
			{
			break;
			}
		     else
			{
		ret_code = NOK;
		return (ret_code); 

			}
		 }

/* -------------------------------------------------------------------------- */
/*    B l o c k  3    -  Stelle 2 - 4 stellengerechte Aufbereitung            */
/* -------------------------------------------------------------------------- */

			if (i != 17)
			   {
			    k = 17 - i;

			 switch(k) {

				   case 1:    sach_nr[i] = sach_nr[i-1];
					      sach_nr[i-1]  = sach_nr[i-2];
					      sach_nr[i-2] = ' ';
					      sach_nr[i-3] = '-';
                sach_nr_alt[i] = sach_nr_alt[i-1];
					      sach_nr_alt[i-1]  = sach_nr_alt[i-2];
					      sach_nr_alt[i-2] = ' ';
					      sach_nr_alt[i-3] = '-';
					      break;
				   case 2:    sach_nr[i+1] = sach_nr[i-1];
					      sach_nr[i] = ' ';
					      sach_nr[i-1] = ' ';
					      sach_nr[i-2] = '-';
                sach_nr_alt[i+1] = sach_nr_alt[i-1];
					      sach_nr_alt[i] = ' ';
					      sach_nr_alt[i-1] = ' ';
					      sach_nr_alt[i-2] = '-';
					      break;

				    }
			     }
/* -------------------------------------------------------------------------- */
/*    Block 3   Fuehrende Nullen entfernen                                     */
/* -------------------------------------------------------------------------- */

      if (isdigit (c = sach_nr[16])
			    || sach_nr[16] == '*')
			{
			  for (n=14;n<16;n++)
			  {
			    if (sach_nr[n] != '0' && sach_nr[n] !=' ')
			    {
			       break;
			    }
			    if (sach_nr[n] == '0' || sach_nr[n] == ' ')
          {
				     sach_nr[n]     = ' ';
				     sach_nr_alt[n] = ' ';
          }
			    else
				     break;
			  }
		    if ((sach_nr[16] == '0')
          			&& (sach_nr[15] == ' ')
			          && (sach_nr[14] == ' '))
			  {
		      ret_code = NOK;
		      return (ret_code);
			  }
			}

/* -------------------------------------------------------------------------- */
/*    Block 3   Bei Eingabe linksbuendig von Zahlen mit SPACE                 */
/* -------------------------------------------------------------------------- */
    i=j;
/*    BLOCK_4: */

/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  Umwandeln 'I' nach 'J' und 'O'nach NULL              */
/* -------------------------------------------------------------------------- */

		if (block3[i] == '\0')
		{
		ret_code = OK;
		return (ret_code); 
		} 
		else
		{
		if (block3[i] != ':')
		{
		ret_code = NOK;
		return (ret_code); 
		}
		}

		if (block3[i] == '\0')
		{
		ret_code = OK;
		return (ret_code);
		}

/* -------------------------------------------------------------------------- */
/*    Aenderung am 15.3.91                                                    */
/* -------------------------------------------------------------------------- */
/*
		if (block3[i] != '-')
		{
		ret_code = NOK;
		return (ret_code);
		}
*/
/* -------------------------------------------------------------------------- */
	    j = i;

	    for (j = i; block3[j] != '\0'; j++)
		{
		 if (block3[j] == 'O')
		    {
		     block3[j] = '0';
		    }
		 if (block3[j] == 'I')
		    {
		     block3[j] = 'J';
		    }
		}
/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  Stelle 1                                             */
/* -------------------------------------------------------------------------- */
	   j = 17;
		 if (block3[i] == '-')
	      {
	       if (block3[i+1] != '\0')
	       {
	       i++;
	       }
	       else
	       {
	       ret_code = NOK;
	       return(ret_code);
	       } 
	      }
	   if (block3[i] == ':')
	   {
	      sach_nr[j]     = block3[i];
	      sach_nr_alt[j] = block3[i];
	      j++;
	      i++;
	   }


		 if  ((isalpha (c = block3[i]))
	       || (isdigit (c = block3[i]))
         || block3[i] == '*'     /* 19.11.93 mg, '*' wie num. Zeichen behandeln */
	       || block3[i] == '+'
	       || block3[i] == '='
	       || block3[i] == '?'
	       || block3[i] == ' ')     
	       {
	       sach_nr[j]     = block3[i];
	       sach_nr_alt[j] = block3[i];
	       i++;
	       j++;
	       }
	    else
      {
/* 19.11.93 mg, '*' wie numersiches Zeichen behandeln */
/*           if (block3[i] == '*')
         {
            sach_nr[j]     = ' ';
            sach_nr_alt[j] = ' ';
            j++;
            sach_nr[j]     = block3[i];
            sach_nr_alt[j] = block3[i];
            i++;
            goto BLOCK_5;
         }
         else
         {
*/
	          ret_code = NOK;
	          return (ret_code);
/*	       }
*/
     }
/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  2. Stelle = '-'                                      */
/* -------------------------------------------------------------------------- */

	    if (block3[i] == '-')
	    {
	       if (   sach_nr[j-1] == ' '
		         || sach_nr[j-1] == '0')
		     {
		        ret_code=NOK;
		        return (ret_code);
		     }
	       if (sach_nr[j-1] == '*'
		        || sach_nr[j-1] == '='
		        || sach_nr[j-1] == '?'
		        || (isdigit (c=sach_nr[j-1])))
		     {
		        sach_nr[j]   = sach_nr[j-1];
		        sach_nr[j-1] = ' ';
		        sach_nr_alt[j]   = sach_nr_alt[j-1];
		        sach_nr_alt[j-1] = ' ';
		     }
	       else
		     {
            sach_nr[j]     = sach_nr[j-1];
			      sach_nr[j-1]   = ' ';
			      sach_nr_alt[j] = ' ';
		     }

	       goto BLOCK_5; 
	    }
/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  2.Stelle ist kein Bindestrich                        */
/* -------------------------------------------------------------------------- */


	if (block3[i] == '\0')
	{
		 if (sach_nr[j-1] == ' ')
	   {
		   ret_code=NOK;
		   return (ret_code);
	   }

     if (sach_nr[j-1] == '0')
	   {
			 sach_nr[j-1]     = '\0';
			 sach_nr_alt[j-1] = '\0';
		   ret_code=OK;
		   return (ret_code);
	   }

		 if (sach_nr[j-1] == '*'
		   || sach_nr[j-1] == '='
		   || sach_nr[j-1] == '?'
		   || (isdigit (c=sach_nr[j-1])))
		   {
       		   sach_nr[j]   = sach_nr[j-1];
		         sach_nr[j-1] = ' ';
       		   sach_nr_alt[j]   = sach_nr_alt[j-1];
		         sach_nr_alt[j-1] = ' ';
		   }
		 else
		   {
             sach_nr[j]     = sach_nr[j-1];
		         sach_nr[j-1]   = ' ';
		         sach_nr_alt[j] = ' ';
		   }

	     goto BLOCK_5;
		 }

	   if ((isalpha (c = block3[i]))
	       || (isdigit (c = block3[i]))
	       || block3[i] == '+'
	       || block3[i] == '*'
	       || block3[i] == '='
	       || block3[i] == '?'
	       || block3[i] == ' ')
	       {
	           sach_nr[j]     = block3[i];
	           sach_nr_alt[j] = block3[i];
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

	if (   sach_nr[j-1] == '0'
	&& sach_nr[j-2] == '0' )
	{
		 if (block3[i] == '\0')
		 {
		    sach_nr[j-2] =  '\0';
		    sach_nr_alt[j-2] =  '\0';
		    ret_code = OK;
		    return (ret_code);
		 }
		 else
     {
	      ret_code = NOK;
	      return (ret_code);
	   }
	}

	  if (sach_nr[j-2] == '0')
		{
	  	 sach_nr[j-2] =  ' ';
	  	 sach_nr_alt[j-2] =  ' ';
		}
/* -------------------------------------------------------------------------- */
/*    Block 4   Ausrichten bei nur einem Zeichen                              */
/*              rechtbuendig bei numerischen Werten und Sonderzeichen         */
/*              linksbuendig bei alphanumer. Werten und Sonderzeichen         */
/* -------------------------------------------------------------------------- */

      /* Ausrichtung sach_nr (neue Form)                              */

	    if (sach_nr[j-1] == ' '
		     && sach_nr[j-2] != ' ')
		  {
		    sach_nr[j-1] =  sach_nr[j-2];
		    sach_nr[j-2] =  ' ';
		  }

      /* Ausrichtung sach_nr_alt (alte Form)                          */

      if (sach_nr_alt[j-2] == ' ')
      {
        if (isalpha (c = sach_nr_alt[j-1])
           || sach_nr_alt[j-1] == '+')
        {
          sach_nr_alt[j-2] = sach_nr_alt[j-1];
          sach_nr_alt[j-1] = ' ';
        }
      }
/* -------------------------------------------------------------------------- */
/*    Block 4   Ausrichten bei 'A0' = ' A'           			      */
/*              rechtbuendig bei NULL nach dem alphanumerischen Wert          */
/*                                                                            */
/* -------------------------------------------------------------------------- */
	if (sach_nr[j-1] == '0')
	{
		 if ( isalpha (c = sach_nr[j-2])
		      || sach_nr[j-2] == '='
		      || sach_nr[j-2] == '?'
		      || sach_nr[j-1] == '+')
		 {
		       sach_nr[j-1] = sach_nr[j-2];
		       sach_nr[j-2] = ' ';
		 }
	}

         /* Ausrichtung 'A0'  >>>   'A '  (fuer alte Form)    */

  if (sach_nr_alt[j-1] == '0')
  {
    if ( isalpha (c = sach_nr_alt[j-2])
		     || sach_nr_alt[j-2] == '='
		     || sach_nr_alt[j-2] == '?'
		     || sach_nr_alt[j-2] == '+')
		{
		    sach_nr_alt[j-1] = ' ';
		}
	}

BLOCK_5:
       sach_nr[20]     = '\0';
       sach_nr_alt[20] = '\0';

/* -------------------------------------------------------------------------- */
/*    B l o c k  5    -  Stelle 1                                             */
/* -------------------------------------------------------------------------- */


		if (block3[i] == '-')
		{
		   if (block3[i+1] != '\0')
	     {
	             i++;
	     }
	     else
	     {
	             ret_code = NOK;
	             return(ret_code);
	     }
		}
    j=20;
		if (block3[i-1] == '-')
		{
		  sach_nr[j] = '-';
		  sach_nr_alt[j] = '-';
		  j++;
		}

		for (j=21;block3[i] != '\0'; i++, j++)
    {
			if ((isalpha (c = block3[i]))
			|| (isdigit (c = block3[i]))
			|| block3[i] == '+'
			|| block3[i] == '*'
			|| block3[i] == '='
			|| block3[i] == '?'
			|| block3[i] == ' ')
			{
			  sach_nr[j]     = block3[i];
			  sach_nr_alt[j] = block3[i];
			}
		    else
			{
		  	ret_code = NOK;
			  return (ret_code);
			}
		}
		if (j > 25)
		{
			ret_code = NOK;
			return (ret_code);
		}
		else
		{
			k = 25 - j;
		}

		switch (k) {
				  case 3:     sach_nr[j+2] = sach_nr[j-1];
					      sach_nr[j]   = ' ';
					      sach_nr[j+1] = ' ';
					      sach_nr[j-1] = ' ';
                sach_nr_alt[j+2] = sach_nr_alt[j-1];
					      sach_nr_alt[j]   = ' ';
					      sach_nr_alt[j+1] = ' ';
					      sach_nr_alt[j-1] = ' ';
					      break;
				   case 2:    sach_nr[j+1] = sach_nr[j-1];
					      sach_nr[j]   = sach_nr[j-2];
					      sach_nr[j-1] = ' ';
					      sach_nr[j-2] = ' ';
                sach_nr_alt[j+1] = sach_nr_alt[j-1];
					      sach_nr_alt[j]   = sach_nr_alt[j-2];
					      sach_nr_alt[j-1] = ' ';
					      sach_nr_alt[j-2] = ' ';
					      break;
           case 1:    sach_nr[j]   = sach_nr[j-1];
					      sach_nr[j-1] = sach_nr[j-2];
					      sach_nr[j-2] = sach_nr[j-3];
								sach_nr[j-3] = ' ';
                sach_nr_alt[j]   = sach_nr_alt[j-1];
					      sach_nr_alt[j-1] = sach_nr_alt[j-2];
					      sach_nr_alt[j-2] = sach_nr_alt[j-3];
								sach_nr_alt[j-3] = ' ';
					      break;


		}
/* -------------------------------------------------------------------------- */
/*    Block 5   Fuehrende Nullen entfernen                                     */
/* -------------------------------------------------------------------------- */
		for (j=21;j<25;j++)
	  {
		    if (sach_nr[j] == '0'
	  		|| sach_nr[j] == ' ')
		  	{
			    sach_nr[j]     = ' ';
			    sach_nr_alt[j] = ' ';
			  }
		    else
			  {
			    break;
			  }
		}
    if ((sach_nr[21] == ' ') &&
        (sach_nr[22] == ' ') &&
        (sach_nr[23] == ' ') &&
        (sach_nr[24] == ' ') &&
        (sach_nr[20] == '-'))
    {
      ret_code=NOK;
      return(ret_code);
    }

/* -------------------------------------------------------------------------- */
/*    Block 5   Fehlerpruefung auf 5.Block groesser 1 Stelle                 */
/* -------------------------------------------------------------------------- */
/*		    if (sach_nr[25] == ' ')
			{
			ret_code = NOK;
			return (ret_code);
			}
*/
/* -------------------------------------------------------------------------- */
/*    Block 5   Ende - Bearbeitung                                           */
/* -------------------------------------------------------------------------- */

  for (i=0;i<25;i++)
  {
	  if (sach_nr[i] == '\0')
	  {
	     sach_nr[i]     = ' ';
	     sach_nr_alt[i] = ' ';
	  }
	}

  sach_nr[25]     = '\0';
  sach_nr_alt[25] = '\0';
  c=c; /*vermeidet Warnings */
  return (ret_code);

}
				     
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm08_afm      Aufbereitung MASCHINELL               */
/*                      norm08_afd      Aufbereitung DRUCK                    */
/*                      norm08_afs      Aufbereitung SORTIERT                 */
/*                      norm08_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm08_af1(norm_p1 *n01_p1)
#else
size_t norm08_af1(n01_p1)
norm_p1 *n01_p1;
#endif
{
  size_t       ret_code = NOK;
  size_t       i;

        /* Auswahl */
  for (i=0;i<25;i++)
  {
	  if (sach_nr[i] == '\0')
	  {
	     sach_nr[i] = ' ';
	  }
	}

  sach_nr[25] = '\0';


					norm08_afm(sach_nr,n01_p1->m_norm);
					norm08_afd(n01_p1->d_norm );
					norm08_afs(sach_nr,n01_p1->s_norm );
					norm08_afb(n01_p1->b_norm );
					norm08_afm(sach_nr_alt,n01_p1->ma_norm);
					norm08_afs(sach_nr_alt,n01_p1->sa_norm);
					ret_code = OK;


	norm08_normby(n01_p1);

  return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_afm                                            */
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
size_t norm08_afm(char *sachnr,char *aus_feld)
#else
size_t norm08_afm(sachnr,aus_feld)
char  *sachnr;
char  *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i,j;

	i=0;j=0;

	while (sachnr[i] != '\0')
	{
	    aus_feld[j] = sachnr[i];
	    i++;j++;
	}

	for (; j<25; j++)
	{
			aus_feld[j] = ' ';
	}
	aus_feld[j] = '\0';

  return (ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_afd                                            */
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
size_t norm08_afd(char    *aus_feld)
#else
size_t norm08_afd(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;

	i=0;j=0;

	while (sach_nr[i] != '\0')
	{
	    if (sach_nr[i] != ' ')
	    {
	        aus_feld[j] = sach_nr[i];
	        j++;
	    }
	    i++;
	}

	for (; j<25; j++)
	{
	    aus_feld[j] = ' ';
	}
	aus_feld[j] = '\0';


	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_afs                                            */
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
size_t norm08_afs(char *sachnr,char *aus_feld)
#else
size_t norm08_afs(sachnr,aus_feld)
char  *sachnr;
char  *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i,j;

	i=0;
	j=0;


	while (sachnr[i] != '\0')
	{
	    aus_feld[j] = sachnr[i];
	    i++;
			j++;
	}

      for (; j<25; j++)
	    {
	        aus_feld[j] = ' ';
	    }
	    aus_feld[j] = '\0';

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_afb                                            */
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
size_t norm08_afb(char *aus_feld)
#else
size_t norm08_afb(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;

	i=0;
	j=0;

	while (sach_nr[i] != '\0')
	{
	    if (sach_nr[i] != ' ')
	    {
	      aus_feld[j] = sach_nr[i];
	      j++;
	    }
	    i++;
	}


      for (; j<25; j++)
	    {
	       aus_feld[j] = ' ';
	    }
	    aus_feld[j] = '\0';

  i=0;

  while (aus_feld[i] != '\0')
  {
      if (aus_feld[i] == ':')
      {
        aus_feld[i] = '-';
      }
      i++;
  }


	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm08_normnb                                            */
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
size_t norm08_normby(norm_p1 *n01_p1)
#else
size_t norm08_normby(n01_p1)
norm_p1 *n01_p1;
#endif
{
	   size_t ret_code=OK;
	   size_t       i;

		 norm00_nb(n01_p1->normbyte,"Y ");

	   for (i=0; sach_nr[i] != '\0'; i++)
	   {
	       if (   sach_nr[i] == ':' )
		     {
            if ((sach_nr[18] != '\0')
            && (sach_nr[19] != '\0')
            && (sach_nr[20] == ' ')
            && (sach_nr[21] == ' ')
            && (sach_nr[22] == ' ')
            && (sach_nr[23] == ' ')
            && (sach_nr[24] == ' '))
            {
		          norm00_nb(n01_p1->normbyte,"Y ");
		          break;
            }
            else
            {
		          norm00_nb(n01_p1->normbyte,"YU");
							break;
            }
		     }
		 }
	 return (ret_code);
}
