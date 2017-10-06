/* -------------------------------------------------------------------------- */
/*      Modul:  norm01                                                        */
/*                                                                            */
/*      Entry:  norm01                                                        */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SIEMENS - Sachnummer                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Ludwig                                                */
/*                      DI AP 623                                             */
/*                                                                            */
/*      Version:        1.0 vom 30.07.90                                      */
/*                                                                            */
/* -------------------------------------------------------------------------- */
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
/* #include <ctype.h>
#include <string.h> */
/* Prototypen */
size_t norm01_fp(char *zw_feld,size_t *z_laenge);
size_t norm01_normby(norm_p1 *n01_p1);
size_t norm01_auf(char *einfeld, char *block1, char *block3);
size_t norm01_all(char *block1, char *block3);
size_t norm01_af1(norm_p1 *n01_p1);
size_t norm01_afm(char *sachnr,char *aus_feld);
size_t norm01_afd(char *aus_feld);
size_t norm01_afs(char *sachnr,char *aus_feld);
size_t norm01_afb(char *aus_feld);
#else
size_t norm01_fp();
size_t norm01_normby();
size_t norm01_auf();
size_t norm01_all();
size_t norm01_af1();
size_t norm01_afm();
size_t norm01_afd();
size_t norm01_afs();
size_t norm01_afb();
#endif


/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      SIEMENS-SACHNUMMER                                    */
/*                                                                            */
/*      Aufrufe:        norm01_fp       Fehlerpruefung                        */
/*                      norm01_auf      Aufbereitung der Bloecke               */
/*			- Entfernen der SPACES in Block1 und 2                */
/*                      norm01_all      Allgemeine Verarbeitung               */
/*                      norm01_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */

char sach_nr[26];
char sach_nr_alt[26];
char zblock1_2[12];
char *block1 = zblock1_2;
char zblock3_5[28];
char *block3 = zblock3_5;

#ifdef __STDC__
size_t norm01(norm_p1 *n01_p1, char *zw_feld)
#else
size_t norm01(n01_p1, zw_feld)
norm_p1 *n01_p1;
char *zw_feld;
#endif
{
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	          *z_laenge   =  &laenge;
/* 25.07.2007, NNB, Variable als global 
umdefiniert wegen Fehler im norm100 (RFC)*/
/*	char            zblock1_2[12];		
	char            *block1 = zblock1_2;
	char            zblock3_5[28];
	char            *block3 = zblock3_5;
*/

	if((ret_code = norm01_fp(zw_feld,z_laenge)) == NOK)
  {
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	}
	if((ret_code = norm01_auf(n01_p1->eingfeld,block1,block3)) == NOK)
  {
		norm00_nb(n01_p1->normbyte,NB_NR);
    return(ret_code);
	}
  if((ret_code = norm01_all(block1,block3)) == NOK)
  {
		norm00_nb(n01_p1->normbyte,NB_NR);
    return(ret_code);
  }


	if((ret_code = norm01_af1(n01_p1)) == NOK)
  {
		norm00_nb(n01_p1->normbyte,NB_NR);
    return(ret_code);
	}

  return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_fp                                             */
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
size_t norm01_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm01_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	 size_t		ret_code=NOK;
     size_t		i,a;
	 char		c;
     size_t		d,e,f,g;
	 size_t		pruef,j,pruef_n;
     char		zw_pruef[6]; 

     if (zw_feld[0] == 'X')
	 {
	      ret_code = NOK;
	      return (ret_code);
	 }

   for (i=0;i < 6;i++)
   {
       zw_pruef[i] = '\0';
   }


/*  Prfung:  1.) 1.Stelle ungleich "U", ungleich "D" und ungleich "N"
                  und alpha, 2. und 3. Stelle numerisch oder '*'
              2.) 1.Stelle "D", "U"  oder "N" und 2. Stelle numerisch  */

/*  20.12.93, mg,  zw_feld[2] kann auch '*' sein                       */

	   if ((zw_feld[0] != 'U'
	      && zw_feld[0] != 'D'
				&& zw_feld[0] != 'N'
				&& (isdigit(c=zw_feld[1]))
				&& (isdigit(c=zw_feld[2]) || zw_feld[2] == '*'))
				|| (zw_feld[0] == 'U'
				&& (isdigit(c=zw_feld[1])))
				|| (zw_feld[0] == 'N'
				&& (isdigit(c=zw_feld[1])))
				|| (zw_feld[0] == 'B'
				&& (isdigit(c=zw_feld[1])))
				|| (zw_feld[0] == 'D'
				&& (isdigit(c=zw_feld[1])))
				|| (zw_feld[1] == '*')
				|| (zw_feld[1] == '.'))
		 {
        /*  o.k.  */
		 }
		 else
	   {
				ret_code = NOK;
				return (ret_code);
		 }

	   if (zw_feld[0] == 'O')
		 {
	       ret_code = NOK;
	       return (ret_code);
	   }

		 a = strlen(zw_feld);
		 *z_laenge = a;

/*	   prsize_tf("Laenge: %d \n",*z_laenge);  */

	for (i=0;i<*z_laenge;i++)
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
/* Pruefung auf "="    */

   for (i=0;i < *z_laenge;i++)
   {
	    if (zw_feld[i] == '=')
	    {
				ret_code = NOK;
				return (ret_code);
	    }
   }

/* Prüfung auf Umlaute */

		for (i=0;i < *z_laenge;i++)
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
 
	 
	 d=0;
	 e=0;
	 f=0;
	 g=0;

   for (i=1;i < *z_laenge;i++)
	 {
		 if (isalpha(c = zw_feld[i]))
		 {
				break;
		 }
		 d++;
   }
   g=i;
   for (i=g;i < *z_laenge;i++)
   {
		 if (isalpha(c = zw_feld[i]))
		 {
			if (e > 1)
			{
				break;
			}
		 }
		 e++;
   }
   g=i;
   for (i=g;i < *z_laenge;i++)
   {
			if ((isalpha(c = zw_feld[i]))
	    || (zw_feld[i] == '-'))
	    {
		    if (f > 1)
		    {
			    break;
		    }
	    }
	    f++;
	 }
   if ((d > 5)
		  && (e > 5)
		  && (f > 4))
   {
	    ret_code = NOK;
   }


   if  (*z_laenge > 25)
   {
			ret_code = NOK;
   }
   else
   {
      ret_code = OK;
   }


   for (i=0;i < 6;i++)
   {
       zw_pruef[i] = '\0';
   }

   if  ((zw_feld[0] != 'D')
    &&  (zw_feld[0] != 'N')
		&&  (zw_feld[0] != 'B')
		&&  (zw_feld[0] != 'U'))
   {
     j=0;
     pruef=0;
	 pruef_n=0;


  for (i=1;((i < 6) && (zw_feld[i] != '\0') && ((isdigit(c=zw_feld[i])) ||
                        zw_feld[i] == '*' || zw_feld[i] == '.'));i++)
  {
       if (((zw_feld[i] == '0') && (zw_feld[i+1] != '0'))
       || ((zw_feld[i] == '0') && (zw_feld[i+1] == '0')))
       {
        pruef=1;
       }
       if (zw_feld[i] != '0')
       {
         pruef_n = 1;
       }
       if ((pruef != 1) || (pruef_n == 1))
       {
          zw_pruef[j] = zw_feld[i];
          j++;
       }
	}
  if (j < 2) /* laenge muss groesser 1 sein !!!*/
     {
       return NOK;
     }
   }


   c=c; /* vermeidet Warnings */
	 return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_auf                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der SIEMENS - Sachnummer                */
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
size_t norm01_auf(char *einfeld, char *block1, char *block3)
#else
size_t norm01_auf(einfeld, block1, block3)
char *einfeld;
char *block1;
char *block3;
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,z;
	char	c;
	size_t	b1,b2;

/* ---------------------------- */
/*      B l o c k  1  und  2    */
/* -----------------------------*/

	k = 0;
	z = 0;
	b1 = 0;
	b2 = 0;

	for (i = 0; einfeld[i] != '\0'; i++)
	{
		if  (einfeld[i] != ' ')
		{
			if (isalpha(c = einfeld[i]))
			{
			    z++;
			    if (z < 3)
				{
					block1[k] = einfeld[i];
				    if (islower (c=block1[k]))
					{
						block1[k] = toupper(c);
					}
					k++;
			    }
			    else
			    {
					break;
			    }
			    continue;
			}
		    if (einfeld[i] == '+')
		    {
			    z++;
			    if (z < 3)
			    {
					block1[k] = einfeld[i];
				    k++;
			    }
			    else
			    {
					break;
			    }
			    continue;
		    }
		    if (einfeld[i] == '-')
		    {
			    if (   einfeld[i+1] == '\0'|| isdigit(c = einfeld[i+1]))
			    {
					ret_code = NOK;
					return(ret_code);
			    }
				continue;
		    }

		    
			/* Ba17.12.2003  hier wird . umgesetzt */
			if (einfeld[i] == '.')
			{
				block1[k] = '*';
		    }
		    else
		    {
				if (k == 12)
				{
					ret_code = NOK;
				    return (ret_code);
				}
				else
				{
					block1[k] = einfeld[i];
				}
		    }
		    k++;
			if (z == 1)
			{
				b1++;
			}
			if (z == 2)
			{
				b2++;
			}
		}
	}

	block1[k] = '\0';
	block3[0] = '\0';

/*  Fehlermeldung 09.05.94 behoben am 17.05.94 androleit   */
/*  Abprfung Laenge Block 1 und Block 2                    */

    if ((b1 > 5) || (b2 > 4))	
    {
		ret_code = NOK;
		return (ret_code);
    }

    if (einfeld[i] == '\0')
	{
		return (ret_code);
	}

/* ----------------------------------------- */
/*      B l o c k  3  bis  5  - Stelle 1     */
/* ------------------------------------------*/
	if (block1[0] == 'W' &&
	       block1[1] == '1' &&
	       block1[2] == '9' &&
	       block1[6] == 'Z' &&
	       einfeld[i] != '\0')
	{
		ret_code = NOK;
	    return (ret_code);
	}
	k = 0;

	if (isalpha(c = einfeld[i]) || einfeld[i] == '+')
	{
		block3[k] = einfeld[i];
		if (islower (c = block3[k]))
		{
			block3[k] = toupper(c);
		}
		k++;
		i++;
	}
	else
	{
	   if (einfeld[i] == '-')
	   {
	      i++;
	   }
	   else
	   {
	      ret_code = NOK;
	      return(ret_code);
	   }
    }
/* ----------------------------------------- */
/*      B l o c k  3  bis  5  - ab Stelle 1  */
/* ------------------------------------------*/

		for (; einfeld[i] != '\0';i++,k++)
		{
      if (isalpha(c = einfeld[i]))
			{
       block3[k] = einfeld[i];
       if (islower (c = block3[k]))
		   {
          block3[k] = toupper(c);
       }
		  }
		  else
		  {
        if (einfeld[i] == '.')
		    {
		       block3[k] = '*';
		    }
		    else
		    {
		       block3[k] = einfeld[i];
				}
		  }
		}

		block3[k] = einfeld[i];

/* Loeschen der Blanks am Ende der Sachnummer                 */

   for (k--; block3[k] == ' ' ;k--)
	 {
			block3[k] = block3[k+1];
	 }

	 if (k > 13)
	 {
			ret_code = NOK;
	 }

   return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_all                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der SIEMENS - Sachnummer                */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm01_all(char *block1, char *block3)
#else
size_t norm01_all(block1, block3)
char *block1;
char *block3;
#endif
{
	size_t	ret_code = OK;
	size_t	i,j,k,n;
	char	c;
  k=0;

/* -------------------------------------------------------------------------- */
/*    B l o c k    1     Stelle 1                                             */
/* -------------------------------------------------------------------------- */
   if (block1[0] == 'I')
	 {
	    block1[0] = 'J';
	 }
	 sach_nr[0]     = block1[0];
   sach_nr_alt[0] = block1[0];
/* -------------------------------------------------------------------------- */
/*    B l o c k  1    -  Stelle 2 - 6                                         */
/* -------------------------------------------------------------------------- */

	for (i=1; i<6 ;i++)
	{
		if ((isdigit(c = block1[i]))
				 || block1[i] == '*')
		{
			sach_nr[i]     = block1[i];
			sach_nr_alt[i] = block1[i];
		}
		else
		{
		    if (isalpha((c = block1[i]))
			     || block1[i] == '-'
			     || block1[i] == '+'
			     || block1[i] == '\0')
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
	 if (i != 6)
	 {
      switch(i)
      {
				   case 2:  sach_nr[5] = sach_nr[i-1];
					          sach_nr[4] = ' ';
					          sach_nr[3] = ' ';
					          sach_nr[2] = ' ';
					          sach_nr[1] = ' ';
                    sach_nr_alt[5] = sach_nr_alt[i-1];
					          sach_nr_alt[4] = ' ';
					          sach_nr_alt[3] = ' ';
					          sach_nr_alt[2] = ' ';
					          sach_nr_alt[1] = ' ';
					          break;
				   case 3:  sach_nr[5] = sach_nr[i-1];
					          sach_nr[4] = sach_nr[i-2];
					          sach_nr[3] = ' ';
					          sach_nr[2] = ' ';
					          sach_nr[1] = ' ';
                    sach_nr_alt[5] = sach_nr_alt[i-1];
					          sach_nr_alt[4] = sach_nr_alt[i-2];
					          sach_nr_alt[3] = ' ';
					          sach_nr_alt[2] = ' ';
					          sach_nr_alt[1] = ' ';
					          break;
				   case 4:  sach_nr[5] = sach_nr[i-1];
					          sach_nr[4] = sach_nr[i-2];
					          sach_nr[3] = sach_nr[i-3];
								    sach_nr[2] = ' ';
					          sach_nr[1] = ' ';
                    sach_nr_alt[5] = sach_nr_alt[i-1];
					          sach_nr_alt[4] = sach_nr_alt[i-2];
					          sach_nr_alt[3] = sach_nr_alt[i-3];
								    sach_nr_alt[2] = ' ';
					          sach_nr_alt[1] = ' ';
					          break;
				    case 5: sach_nr[5] = sach_nr[i-1];
								    sach_nr[4] = sach_nr[i-2];
					          sach_nr[3] = sach_nr[i-3];
					          sach_nr[2] = sach_nr[i-4];
					          sach_nr[1] = ' ';
                    sach_nr_alt[5] = sach_nr_alt[i-1];
								    sach_nr_alt[4] = sach_nr_alt[i-2];
					          sach_nr_alt[3] = sach_nr_alt[i-3];
					          sach_nr_alt[2] = sach_nr_alt[i-4];
					          sach_nr_alt[1] = ' ';
					          break;
	    }
   }
/* -------------------------------------------------------------------------- */
/*    Fuehrende Nullen entfernen                                               */
/* -------------------------------------------------------------------------- */

	if (sach_nr[5] > 0)
	{
		for (n=1;n<6;n++)
		{
			if (sach_nr[n] == '0' || sach_nr[n] == ' ')
			{
				sach_nr[n]     = ' ';
				sach_nr_alt[n] = ' ';
			}
			else
				break;
		}
	}

	if (block1[i] == '\0')
	{
		sach_nr[6]     = '\0';
		sach_nr_alt[6] = '\0';
		return (ret_code);
	}
/* -------------------------------------------------------------------------- */
/*    B l o c k 2  - Stelle 1                                                 */
/* -------------------------------------------------------------------------- */
	j = 6;

	if (block1[i] == 'O')
	{
	    ret_code = NOK;
	    return (ret_code);
	}
	if (block1[i] == 'I')
	{
	    block1[i] = 'J';
	}


	if (isalpha(c = block1[i]) || block1[i] == '+')
	{
		sach_nr[j]     = block1[i];
		sach_nr_alt[j] = block1[i];
	    i++;
	    j++;
	 }
	 else
	 {
	    ret_code = NOK;
	    return (ret_code);
	 }

/* -------------------------------------------------------------------------- */
/*    B l o c k  2    -  Stelle 2 - 5  - Auswahl                              */
/* -------------------------------------------------------------------------- */
	for (; block1[i] != '\0';i++,j++)
	{
		if (isdigit ((c = block1[i]))
		     || block1[i] == '*')
		{
			sach_nr[j]     = block1[i];
			sach_nr_alt[j] = block1[i];
		}
		else
		{
			if (isalpha (c = block1[i])
			      || block1[i] == '-'
			      || block1[i] == '+'
			      || block1[i] == '\0')
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

/* -------------------------------------------------------------------------- */
/*    B l o c k  2    -  Stelle 2 - 5  stellengerechte Aufbereitung           */
/* -------------------------------------------------------------------------- */

	if (j != 11)
	{
		k = 11 - j;

		switch(k)
		{
			case 4:    
				sach_nr[j]   = ' ';
				sach_nr[j+1] = ' ';
				sach_nr[j+2] = ' ';
				sach_nr[j+3] = ' ';
                sach_nr_alt[j]   = ' ';
				sach_nr_alt[j+1] = ' ';
				sach_nr_alt[j+2] = ' ';
				sach_nr_alt[j+3] = ' ';
			break;
			case 3:    
				sach_nr[j+2] = sach_nr[j-1];
				sach_nr[j]   = ' ';
				sach_nr[j+1] = ' ';
				sach_nr[j-1] = ' ';
                sach_nr_alt[j+2] = sach_nr_alt[j-1];
				sach_nr_alt[j]   = ' ';
				sach_nr_alt[j+1] = ' ';
				sach_nr_alt[j-1] = ' ';
			break;
			case 2:    
				sach_nr[j+1] = sach_nr[j-1];
				sach_nr[j]   = sach_nr[j-2];
				sach_nr[j-1] = ' ';
				sach_nr[j-2] = ' ';
                sach_nr_alt[j+1] = sach_nr_alt[j-1];
				sach_nr_alt[j]   = sach_nr_alt[j-2];
				sach_nr_alt[j-1] = ' ';
				sach_nr_alt[j-2] = ' ';
			break;
			case 1:    
				sach_nr[j]   = sach_nr[j-1];
				sach_nr[j-1] = sach_nr[j-2];
				sach_nr[j-2] = sach_nr[j-3];
				sach_nr[j-3] = ' ';
                sach_nr_alt[j]   = sach_nr_alt[j-1];
				sach_nr_alt[j-1] = sach_nr_alt[j-2];
				sach_nr_alt[j-2] = sach_nr_alt[j-3];
				sach_nr_alt[j-3] = ' ';
			break;
		}
	}
/* -------------------------------------------------------------------------- */
/*    Fuehrende Nullen entfernen                                               */
/* -------------------------------------------------------------------------- */

	if(sach_nr[10] > 0)
	{
		for (n=7;n<11;n++)
		{
			if (sach_nr[n] == '0' || sach_nr[n] == ' ')
			{
				sach_nr[n] = ' ';
				sach_nr_alt[n] = ' ';
			}
			else
				break;
		}
	}
                     
/*	 BLOCK_3:    */
/* -------------------------------------------------------------------------- */
/*    B l o c k  3    -  Stelle 1                                             */
/* -------------------------------------------------------------------------- */
	j = 11;
	i =  0;

	if (block3[i] == '\0')
	{
		sach_nr[11] = '\0';
		sach_nr_alt[11] = '\0';

	    ret_code = OK;
	    return (ret_code);
	}

	if (block3[i] == 'O')
	{
	    ret_code = NOK;
	    return (ret_code);
	}

	if (block3[i] == 'I')
	{
	    block3[i] = 'J';
	}

	if (isalpha (c = block3[i])
				 || block3[i] == '+')
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
/*    B l o c k  3    -  Stelle 2 - Auswahl                                   */
/* -------------------------------------------------------------------------- */
	for (j=12; j<15; i++,j++)
	{
		if (isdigit (c = block3[i])
           || block3[i] == '*')         /* 20.12.93, mg '*' anstelle einer */
                                        /*              Ziffer zugelassen  */
		{
			sach_nr[j]     = block3[i];
			sach_nr_alt[j] = block3[i];
			 continue;
		}

/*		 if (block3[i] == '*')
		    {
                    sach_nr[j] = ' ';
                     j++;
                     sach_nr[j] = block3[i];
                     j++;
                     i++;
                     goto BLOCK_AUF;               */
		 /*  continue;  */
		     /* Normbyte setzen )*/
		/*    }  */

        if (block3[i] == ' ')
		{
			sach_nr[j]     = block3[i];
            sach_nr_alt[j] = block3[i];
		    continue;
		}


		if (isalpha (c = block3[i])
			     || block3[i] == '-'
			     || block3[i] == '*'
           || block3[i] == '+'
			     || block3[i] == '\0')
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

	if (j != 15)
	{
		k = 15 - j;

		switch(k)
        {
			case 3:    
				sach_nr[j]   = ' ';
				sach_nr[j+1] = ' ';
				sach_nr[j+2] = ' ';
                sach_nr_alt[j]   = ' ';
				sach_nr_alt[j+1] = ' ';
				sach_nr_alt[j+2] = ' ';
			break;
			case 2:    
				sach_nr[j+1] = sach_nr[j-1];
				sach_nr[j]   = ' ';
				sach_nr[j-1] = ' ';
                sach_nr_alt[j+1] = sach_nr_alt[j-1];
				sach_nr_alt[j]   = ' ';
				sach_nr_alt[j-1] = ' ';
			break;
			case 1:    
				sach_nr[j]   = sach_nr[j-1];
				sach_nr[j-1] = sach_nr[j-2];
				sach_nr[j-2] = ' ';
                sach_nr_alt[j]   = sach_nr_alt[j-1];
				sach_nr_alt[j-1] = sach_nr_alt[j-2];
				sach_nr_alt[j-2] = ' ';
			break;

		}
	}
/* -------------------------------------------------------------------------- */
/*    Block 3   Fuehrende Nullen entfernen                                    */
/* -------------------------------------------------------------------------- */

	if (isdigit (c = sach_nr[14]) || sach_nr[14] == '*')
	{
		for (n=12;n<15;n++)
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
	}

/* -------------------------------------------------------------------------- */
/*    Block 3   Bei Eingabe linksbuendig von Zahlen mit SPACE                 */
/* -------------------------------------------------------------------------- */

	if (   sach_nr[13] == ' '
          && sach_nr[14] == ' ' )
	{
	    sach_nr[14] = sach_nr[12];
	    sach_nr[13] = ' ';
	    sach_nr[12] = ' ';
        sach_nr_alt[14] = sach_nr_alt[12];
	    sach_nr_alt[13] = ' ';
	    sach_nr_alt[12] = ' ';
	}

    if (sach_nr[14] == ' ')
	{
	    sach_nr[14] = sach_nr[13];
	    sach_nr[13] = sach_nr[12];
	    sach_nr[12] = ' ';
        sach_nr_alt[14] = sach_nr_alt[13];
	    sach_nr_alt[13] = sach_nr_alt[12];
	    sach_nr_alt[12] = ' ';
	   }

/*      BLOCK_4:     */
	    sach_nr[15]     = '\0';
      sach_nr_alt[15] = '\0';
/* -------------------------------------------------------------------------- */
/*    B l o c k  4    -  Umwandeln 'I' nach 'J' und 'O'nach NULL              */
/* -------------------------------------------------------------------------- */
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
		j = 15;
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


		if  ((isalpha (c = block3[i]))
	       || (isdigit (c = block3[i]))
         || block3[i] == '*'          /* 19.11.93 mg, '*' wie num. Zeichen */
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
/* 19.11.93 mg, '*' wie numerisches Zeichen behandeln */
/*        if (block3[i] == '*')
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

	       if    (sach_nr[j-1] == '*'
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
        	  sach_nr[j]   = sach_nr[j-1];
		        sach_nr[j-1] = ' ';
            sach_nr_alt[j]  = ' ';
		      }

/*          if    (sach_nr[j-1] == '*'
		         || sach_nr[j-1] == '='
		         || sach_nr[j-1] == '?'
		         || (isdigit (c=sach_nr[j-1]))
			       || (isalpha (c=sach_nr[j-1])))
		      {
		        sach_nr[j]   = sach_nr[j-1];
		        sach_nr[j-1] = ' ';
		      }
	        else
		      {
		        sach_nr[j] = ' ';
		      }
*/
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

       if    (sach_nr[j-1] == '*'
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
        	  sach_nr[j]   = sach_nr[j-1];
		        sach_nr[j-1] = ' ';
            sach_nr_alt[j]  = ' ';

		      }

/*		  if (sach_nr[j-1] == '*'
		     || sach_nr[j-1] == '='
		     || sach_nr[j-1] == '?'
		     || (isdigit (c=sach_nr[j-1]))
		     || (isalpha (c=sach_nr[j-1])))
		  {
		    sach_nr[j]   = sach_nr[j-1];
		    sach_nr[j-1] = ' ';
		  }
		  else
		  {
		    sach_nr[j] = ' ';
		  }
*/
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
		    sach_nr[j-2]     =  '\0';
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
		 sach_nr[j-2]     =  ' ';
		 sach_nr_alt[j-2] =  ' ';
	 }
/* -------------------------------------------------------------------------- */
/*    Block 4   Ausrichten bei nur einem Zeichen                              */
/*              rechtsbuendig bei numerischen Werten und Sonderzeichen        */
/*              rechtsbuendig bei alphanumer. Werten und Sonderzeichen        */
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
/*    Block 4   Ausrichten bei 'A0' = ' A'  (neue Form)         			      */
/*              rechtbuendig bei NULL nach dem alphanumerischen Wert          */
/*                                                                            */
/* -------------------------------------------------------------------------- */
	     if (sach_nr[j-1] == '0')
       {
          if ( isalpha (c = sach_nr[j-2])
		         || sach_nr[j-2] == '='
		         || sach_nr[j-2] == '?'
		         || sach_nr[j-2] == '+')
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
       sach_nr[17] = '\0';
       sach_nr_alt[17] = '\0';

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

		for (j=17;block3[i] != '\0'; i++, j++)

		{
		  if ((isalpha (c = block3[i]))
			   || (isdigit (c = block3[i]))
			   || block3[i] == '+'
			   || block3[i] == '*'
			   || block3[i] == '='
			   || block3[i] == '?'
			   || block3[i] == ' ')
			{
			  sach_nr[j] = block3[i];
        sach_nr_alt[j] = block3[i];
			}
		    else
			{
			  ret_code = NOK;
			  return (ret_code);
			}
		}
		if (j > 21)
    {
			ret_code = NOK;
			return (ret_code);
		}
		else
		{
			k = 21 - j;
		}

		switch (k)
    {
		    case 3: sach_nr[j+2] = sach_nr[j-1];
								sach_nr[j]   = ' ';
								sach_nr[j+1] = ' ';
								sach_nr[j-1] = ' ';

                sach_nr_alt[j+2] = sach_nr_alt[j-1];
								sach_nr_alt[j]   = ' ';
								sach_nr_alt[j+1] = ' ';
								sach_nr_alt[j-1] = ' ';
								break;
				case 2: sach_nr[j+1] = sach_nr[j-1];
								sach_nr[j]   = sach_nr[j-2];
								sach_nr[j-1] = ' ';
								sach_nr[j-2] = ' ';

                sach_nr_alt[j+1] = sach_nr_alt[j-1];
								sach_nr_alt[j]   = sach_nr_alt[j-2];
								sach_nr_alt[j-1] = ' ';
								sach_nr_alt[j-2] = ' ';
								break;
				case 1: sach_nr[j]   = sach_nr[j-1];
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
		for (j=17;j<21;j++)
		{
		  if (sach_nr[j] == '0'
			   || sach_nr[j] == ' ')
			{
        if ((sach_nr[j+1] == '\0') &&
           (sach_nr [j] == '0')   &&
           (sach_nr [j-1] == ' ') &&
           (sach_nr [j-2] == ' ') &&
           (sach_nr [j-3] == ' '))
        {
         ret_code = NOK;
         return (ret_code);
        }
			  sach_nr[j] = ' ';
        sach_nr_alt[j] = ' ';
			}
		  else
			{
			  break;
			}
		}

/* -------------------------------------------------------------------------- */
/*    Block 5   Fehlerpruefung auf 5.Block groesser 1 Stelle                 */
/* -------------------------------------------------------------------------- */
/*		    if (sach_nr[20] == ' ')
			{
			ret_code = NOK;
			return (ret_code);
			}
*/
/* -------------------------------------------------------------------------- */
/*    Block 5   Ende - Bearbeitung                                           */
/* -------------------------------------------------------------------------- */

       sach_nr[21] = '\0';
       sach_nr_alt[21] = '\0' ;
       c=c; /* vermeidet Warnings */
       return (ret_code);

}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm01_afm      Aufbereitung MASCHINELL               */
/*                      norm01_afd      Aufbereitung DRUCK                    */
/*                      norm01_afs      Aufbereitung SORTIERT                 */
/*                      norm01_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm01_af1(norm_p1 *n01_p1)
#else
size_t norm01_af1(n01_p1)
norm_p1 *n01_p1;
#endif
{
    size_t       ret_code = NOK;

        /* Auswahl */


  
                         norm01_afm(sach_nr,n01_p1->m_norm);
                         norm01_afd(n01_p1->d_norm );
                         norm01_afs(sach_nr,n01_p1->s_norm );
                         norm01_afb(n01_p1->b_norm );
                         norm01_afm(sach_nr_alt,n01_p1->ma_norm);
                         norm01_afs(sach_nr_alt,n01_p1->sa_norm);
                         ret_code = OK;
 

    norm01_normby(n01_p1);

    return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_afm                                            */
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
size_t norm01_afm(char *sachnr,char *aus_feld)
#else
size_t norm01_afm(sachnr,aus_feld)
char *sachnr;
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i,j;

	i=0;j=0;
	while (j<4)
	{
	  aus_feld[j] = ' ';
	  j++;
	}
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
/*      Funktion:       norm01_afd                                            */
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
size_t norm01_afd(char *aus_feld)
#else
size_t norm01_afd(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;
/*ba19.02.2004	*/ /* hier muss änderung aus dock rein*/

	for (i=0,j=0; sach_nr[i] != '\0'; i++)
	{
			if ( i ==  6 || i == 11 || i == 15 || i == 17)
			{
				aus_feld[j] = '-';
				j++;
			}

	    if (sach_nr[i] ==  ' ')
	    {
				continue;
	    }
	    else
	    {
		    aus_feld[j] = sach_nr[i];
		    j++;
	    }
	}
	if (aus_feld[j-1] == '-')
	{
		j--;
	}

  for (; j<25; j++)
	{
	  aus_feld[j] = ' ';
	}

	aus_feld[j] = '\0';


	return(ret_code);

}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm01_afs                                            */
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
size_t norm01_afs(char *sachnr,char *aus_feld)
#else
size_t norm01_afs(sachnr,aus_feld)
char *sachnr;
char *aus_feld;
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
/*      Funktion:       norm01_afb                                            */
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
size_t norm01_afb(char *aus_feld)
#else
size_t norm01_afb(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;
	/*ba19.02.2004	*/ /* Hier müsste die Änderung rein laut docu */

	for (i=0,j=0; sach_nr[i] != '\0'; i++)
	{
	    if ( i ==  6 || i == 11 || i == 15 || i == 17)
		  {
		    aus_feld[j] = '-';
			  j++;
		  }

			if (sach_nr[i] ==  ' ')
	    {
		    continue;
	    }
	    else
	    {
		    if (sach_nr[i] == '*')
		    {
		      aus_feld[j] = '.';
		    }
		    else
		    {
		      aus_feld[j] = sach_nr[i];
		    }
		    j++;
	    }
	}
	if (aus_feld[j-1] == '-')
	{
		j--;
	}

  for (; j<25; j++)
	{
	      aus_feld[j] = ' ';
	}

	aus_feld[j] = '\0';


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
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm01_normby(norm_p1 *n01_p1)
#else
size_t norm01_normby(n01_p1)
norm_p1 *n01_p1;
#endif
{
	   size_t ret_code=OK;
	   size_t       i;

	   norm00_nb(n01_p1->normbyte,"S ");

	   for (i=0; sach_nr[i] != '\0'; i++)
		 {
	       if (   sach_nr[i] == '+'
		         || sach_nr[i] == '*')
		     {
		        norm00_nb(n01_p1->normbyte,"S*");
		        break;
		     }
     }

	   if (sach_nr[0] == 'W' &&
	       sach_nr[1] == '1' &&
	       sach_nr[2] == '9' &&
	       sach_nr[6] == 'Z')
     {
	       norm00_nb(n01_p1->normbyte,"S7");
     }

	   if (sach_nr[15] == 'Z')
     {
	       norm00_nb(n01_p1->normbyte,"S3");
		 }
	   return (ret_code);
}
