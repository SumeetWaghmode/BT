/* -------------------------------------------------------------------------- */
/*      Modul:  norm12                                                        */
/*                                                                            */
/*      Entry:  norm12                                                        */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SIEMENS - Sachnummer  mit Kennung "M"    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:                                                               */
/*                      SNI AP 621                                            */
/*                                                                            */
/*      Version:        1.0 vom 22.03.93                                      */
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
size_t norm12_fp(char *zw_feld,size_t *z_laenge);
size_t norm12_fp1(size_t *z_laenge);
size_t norm12_normby(norm_p1 *n01_p1);
size_t norm12_auf(char *einfeld, char *block1, char *block3);
size_t norm12_all(char *block1, char *block3);
size_t norm12_af1(norm_p1 *n01_p1);
size_t norm12_afm(char *sachnr,char *aus_feld);
size_t norm12_afd(char *aus_feld);
size_t norm12_afs(char *sachnr,char *aus_feld);
size_t norm12_afb(char *aus_feld);
#else
size_t norm12_fp();
size_t norm12_fp1();
size_t norm12_normby();
size_t norm12_auf();
size_t norm12_all();
size_t norm12_af1();
size_t norm12_afm();
size_t norm12_afd();
size_t norm12_afs();
size_t norm12_afb();
#endif


/* -------------------------------------------------------------------------- */
/*      Funktion:       norm12                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      SIEMENS-SACHNUMMER mit Kennung "M"                    */
/*                                                                            */
/*      Aufrufe:        norm12_fp       Fehlerpruefung                        */
/*                      norm12_auf      Aufbereitung der Bloecke               */
/*			- Entfernen der SPACES in Block1 und 2                */
/*                      norm12_all      Allgemeine Verarbeitung               */
/*                      norm12_af1      Auswahl nach Steuerbyte               */
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
size_t norm12(norm_p1 *n01_p1, char *zw_feld)
#else
size_t norm12(n01_p1, zw_feld)
norm_p1 *n01_p1;
char *zw_feld;
#endif
{
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	          *z_laenge   =  &laenge;
	char            zblock1_2[12];
	char            *block1 = zblock1_2;
	char            zblock3_5[28];
	char            *block3 = zblock3_5;
	size_t             i;
  
	for (i=0;i<26;i++)
	{
	  sach_nr[i] = '\0';
	}


	if((ret_code = norm12_fp(zw_feld,z_laenge)) == NOK)
  {
		norm00_nb(n01_p1->normbyte,NB_NR);  
		return(ret_code);
	}

	if((ret_code = norm12_auf(zw_feld,block1,block3)) == NOK)
  {
	  	norm00_nb(n01_p1->normbyte,NB_NR);
      return(ret_code);
  }

  if((ret_code = norm12_all(block1,block3)) == NOK)
  {
	  	norm00_nb(n01_p1->normbyte,NB_NR);
      return(ret_code);
  }
	if((ret_code = norm12_fp1(z_laenge)) == NOK)
  {
	  	norm00_nb(n01_p1->normbyte,NB_NR);
		  return(ret_code);
	}


	if((ret_code = norm12_af1(n01_p1)) == NOK)
  {
	  	norm00_nb(n01_p1->normbyte,NB_NR);
      return(ret_code);
	}


  return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm12_fp                                             */
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
size_t norm12_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm12_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
     size_t ret_code=NOK;
     size_t i,a;
	 char	c;
     size_t d,e,f,g;
     if (zw_feld[0] == 'X')
	      {
	      ret_code = NOK;
	      return (ret_code);
	      }


	   if (((zw_feld[0] == 'M')
	      && (isdigit(c=zw_feld[1]))
	      && (isdigit(c=zw_feld[2])))
        || ((zw_feld[0] == 'M')
	      && (zw_feld[1] == '*')              /* 8.3.94, Gerland, || in && */
	      || (zw_feld[1] == '.')))            /* geaendert                 */

	      {
	      }
	    else
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
    c=c; /* vermeidet Warnings */
	  return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm12_fp1                                            */
/*                                                                            */
/*      Aufgabe:        Fehlerpruefung Stellen gem. Richtlinien               */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*      Aenderung:      19.1.94, Gerland                                      */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm12_fp1(size_t *z_laenge)
#else
size_t norm12_fp1(z_laenge)
size_t *z_laenge;
#endif
{
     size_t ret_code=OK;


/* 7.Stelle ungleich "X" */
/* 19.1.94, MG, Fehlermeldung v. Hr.Gojowsky v. 17.1.94  */
/*	   if (sach_nr[6] == 'X')
	   {
	      ret_code = NOK;
	      return (ret_code);
	   }

	   if ((sach_nr[11] == 'G')
        || (sach_nr[11] == 'X'))
	   {
	      ret_code = NOK;
	      return (ret_code);
	   }
*/

	   if (sach_nr[15] != '\0')
	   {
        if ((sach_nr[16] == 'Z')
         && (sach_nr[17] == '\0'))
         {
           sach_nr[15] = 'Z';
           sach_nr[16] = '\0';
           sach_nr_alt[15] = 'Z';
           sach_nr_alt[16] = '\0';
         }
         else
         {
	        ret_code = NOK;
	        return (ret_code);
         }
	   }
    *z_laenge = *z_laenge;
	  return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm12_auf                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der SIEMENS - Sachnummer                 */
/*                      Entfernen der Leerstellen in Block 1 und 2            */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*      Aenderung:      19.1.94, Gerland                                      */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm12_auf(char *einfeld, char *block1, char *block3)
#else
size_t norm12_auf(einfeld, block1, block3)
char *einfeld;
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

	for (i = 31; (((einfeld[i] == ' ') || (einfeld[i] == '\0'))
               && (i > 0)); i--)
	{
		if  (einfeld[i] == ' ')
		    {
        einfeld[i] = '\0';
        }
  }


/*  19.1.94, MG, Fehlermeldung v. Hr. Gojowsky, 17.1.94          */
/*	for (i = 0; einfeld[i] != '\0'; i++)     geaendert in :      */




	for (i = 0; (einfeld[i] != '\0') && (i <= 11); i++)
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
			   if (   einfeld[i+1] == '\0'
			   || isdigit(c = einfeld[i+1]))
			   {
			      ret_code = NOK;
			      return(ret_code);
			   }
			   continue;
		    }

		    if (einfeld[i] == '.')
		    {
		       block1[k] = '*';
		    }
		    else
		    {
		       block1[k] = einfeld[i];
		    }
		    k++;
		   }
	   }

		 block1[k] = '\0';
		 block3[0] = '\0';

     if (einfeld[i] == '\0')
		 {
		  return (ret_code);
		 }

/* ----------------------------------------- */
/*      B l o c k  3  bis  5  - Stelle 1     */
/* ------------------------------------------*/
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
    /*  14.3.94, Gerland, nach diesem else haben die Klammern gefehlt !! */
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
/*      Funktion:       norm12_all                                            */
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
size_t norm12_all(char *block1, char *block3)
#else
size_t norm12_all(block1, block3)
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
		 if ((isdigit (c = block1[i]))
		     || block1[i] == '*')
		 {
		     sach_nr[i]     = block1[i];
		     sach_nr_alt[i] = block1[i];
		 }
		 else
		 {
		  if (isalpha ((c = block1[i]))
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

        switch(i) {

				   case 2:  sach_nr[5] = sach_nr[i-1];
					      sach_nr[4] = '0';
					      sach_nr[3] = '0';
					      sach_nr[2] = '0';
					      sach_nr[1] = '0';
                sach_nr_alt[5] = sach_nr_alt[i-1];
					      sach_nr_alt[4] = '0';
					      sach_nr_alt[3] = '0';
					      sach_nr_alt[2] = '0';
					      sach_nr_alt[1] = '0';
					      break;
				   case 3:  sach_nr[5] = sach_nr[i-1];
					      sach_nr[4] = sach_nr[i-2];
					      sach_nr[3] = '0';
					      sach_nr[2] = '0';
					      sach_nr[1] = '0';
                sach_nr_alt[5] = sach_nr_alt[i-1];
					      sach_nr_alt[4] = sach_nr_alt[i-2];
					      sach_nr_alt[3] = '0';
					      sach_nr_alt[2] = '0';
					      sach_nr_alt[1] = '0';
					      break;
				   case 4:  sach_nr[5] = sach_nr[i-1];
					      sach_nr[4] = sach_nr[i-2];
					      sach_nr[3] = sach_nr[i-3];
					      sach_nr[2] = '0';
					      sach_nr[1] = '0';
                sach_nr_alt[5] = sach_nr_alt[i-1];
					      sach_nr_alt[4] = sach_nr_alt[i-2];
					      sach_nr_alt[3] = sach_nr_alt[i-3];
					      sach_nr_alt[2] = '0';
					      sach_nr_alt[1] = '0';
					      break;
				    case 5: sach_nr[5] = sach_nr[i-1];
					      sach_nr[4] = sach_nr[i-2];
					      sach_nr[3] = sach_nr[i-3];
					      sach_nr[2] = sach_nr[i-4];
					      sach_nr[1] = '0';
                sach_nr_alt[5] = sach_nr_alt[i-1];
					      sach_nr_alt[4] = sach_nr_alt[i-2];
					      sach_nr_alt[3] = sach_nr_alt[i-3];
					      sach_nr_alt[2] = sach_nr_alt[i-4];
					      sach_nr_alt[1] = '0';
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
/*       if (sach_nr[n] == '0' || sach_nr[n] == ' ')
        {
				  sach_nr[n]     = ' ';
				  sach_nr_alt[n] = ' ';
        }
			  else
        {
				   break;
			  }
*/
      }
  }

		  if (block1[i] == '\0')
			{
			   sach_nr[6] = '\0';
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


	   if (isalpha (c = block1[i])
	       || block1[i] == '+')
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

			 switch(k) {

				   case 4:    sach_nr[j]   = '0';
					      sach_nr[j+1] = '0';
					      sach_nr[j+2] = '0';
					      sach_nr[j+3] = '0';
                sach_nr_alt[j]   = '0';
					      sach_nr_alt[j+1] = '0';
					      sach_nr_alt[j+2] = '0';
					      sach_nr_alt[j+3] = '0';
					      break;
				   case 3:    sach_nr[j+2] = sach_nr[j-1];
					      sach_nr[j]   = '0';
					      sach_nr[j+1] = '0';
					      sach_nr[j-1] = '0';
                sach_nr_alt[j+2] = sach_nr_alt[j-1];
					      sach_nr_alt[j]   = '0';
					      sach_nr_alt[j+1] = '0';
					      sach_nr_alt[j-1] = '0';
					      break;
				   case 2:    sach_nr[j+1] = sach_nr[j-1];
					      sach_nr[j]   = sach_nr[j-2];
					      sach_nr[j-1] = '0';
					      sach_nr[j-2] = '0';
                sach_nr_alt[j+1] = sach_nr_alt[j-1];
					      sach_nr_alt[j]   = sach_nr_alt[j-2];
					      sach_nr_alt[j-1] = '0';
					      sach_nr_alt[j-2] = '0';
					      break;
           case 1:    sach_nr[j]   = sach_nr[j-1];
					      sach_nr[j-1] = sach_nr[j-2];
					      sach_nr[j-2] = sach_nr[j-3];
					      sach_nr[j-3] = '0';
                sach_nr_alt[j]   = sach_nr_alt[j-1];
					      sach_nr_alt[j-1] = sach_nr_alt[j-2];
					      sach_nr_alt[j-2] = sach_nr_alt[j-3];
					      sach_nr_alt[j-3] = '0';
					      break;

				    }
          }
/* -------------------------------------------------------------------------- */
/*    Fuehrende Nullen entfernen                                               */
/* -------------------------------------------------------------------------- */

   if (sach_nr[10] > 0)
	 {
			for (n=7;n<11;n++)
			{
/*	    if (sach_nr[n] == '0' || sach_nr[n] == ' ')
        {
				   sach_nr[n] = ' ';
				   sach_nr_alt[n] = ' ';
        }
			  else
				   break;
*/
      }
   }

/*	    BLOCK_3: */
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
	       sach_nr[j] = block3[i];
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
		 if (isdigit (c = block3[i]))
		 {
		     sach_nr[j] = block3[i];
		     sach_nr_alt[j] = block3[i];
		     continue;
		 }

		 if (block3[i] == '*')
		 {
        sach_nr[j] = block3[i];
        sach_nr_alt[j] = block3[i];
		    continue;
		     /* Normbyte setzen )*/
		 }

		 if (block3[i] == ' ')
		 {
        sach_nr[j]     = block3[i];
        sach_nr_alt[j] = block3[i];
		    continue;
		 }


		 if (isalpha (c = block3[i])
			 || block3[i] == '-'
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

			 switch(k) {

				   case 3:    sach_nr[j]   = '0';
					      sach_nr[j+1] = '0';
					      sach_nr[j+2] = '0';
                sach_nr_alt[j]   = '0';
					      sach_nr_alt[j+1] = '0';
					      sach_nr_alt[j+2] = '0';
					      break;
				   case 2:    sach_nr[j+1] = sach_nr[j-1];
					      sach_nr[j]   = '0';
					      sach_nr[j-1] = '0';
                sach_nr_alt[j+1] = sach_nr_alt[j-1];
					      sach_nr_alt[j]   = '0';
					      sach_nr_alt[j-1] = '0';
					      break;
           case 1:    sach_nr[j]   = sach_nr[j-1];
					      sach_nr[j-1] = sach_nr[j-2];
					      sach_nr[j-2] = '0';
                sach_nr_alt[j]   = sach_nr_alt[j-1];
					      sach_nr_alt[j-1] = sach_nr_alt[j-2];
					      sach_nr_alt[j-2] = '0';
					      break;

				    }
			     }
/* -------------------------------------------------------------------------- */
/*    Block 3   bei Stelle 11 = "Z" und weitere 3 Stellen = "0" diese auf space*/
/* -------------------------------------------------------------------------- */

   if ((sach_nr[11] == 'Z')
			&& (sach_nr[12] == '0')
			&& (sach_nr[13] == '0')
			&& (sach_nr[14] == '0'))
	 {
        sach_nr[12] = ' ';
        sach_nr[13] = ' ';
        sach_nr[14] = ' ';
        sach_nr_alt[12] = ' ';
        sach_nr_alt[13] = ' ';
        sach_nr_alt[14] = ' ';

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
/*    BLOCK_4: */
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
         || block3[i] == '*'   /* 19.11.93 mg, '*' wie num. Zeichen behandeln */
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
/* 19.11.93 mg, '*' wie numerische Zeichen behandeln */
/*         if (block3[i] == '*')
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
		        sach_nr[j]   = sach_nr[j-1];
		        sach_nr[j-1] = ' ';
            sach_nr_alt[j]  = ' ';
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
          sach_nr[j]   = sach_nr[j-1];
		      sach_nr[j-1] = ' ';
          sach_nr_alt[j]  = ' ';
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
       sach_nr[17]     = '\0';
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
		for (j=17;j<21;j++)
		{
		  if (sach_nr[j] == '0'
			|| sach_nr[j] == ' ')
			{
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

       sach_nr[21]     = '\0';
       sach_nr_alt[21] = '\0';
       c=c; /* vermeidet Warnings */
       return (ret_code);

}                           
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm12_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm12_afm      Aufbereitung MASCHINELL               */
/*                      norm12_afd      Aufbereitung DRUCK                    */
/*                      norm12_afs      Aufbereitung SORTIERT                 */
/*                      norm12_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm12_af1(norm_p1 *n01_p1)
#else
size_t norm12_af1(n01_p1)
norm_p1 *n01_p1;
#endif
{
        size_t       ret_code = NOK;

        /* Auswahl */


					                 norm12_afm(sach_nr,n01_p1->m_norm);
					                 norm12_afm(sach_nr_alt,n01_p1->ma_norm);
					                 norm12_afd(n01_p1->d_norm );
					                 norm12_afs(sach_nr,n01_p1->s_norm );
					                 norm12_afs(sach_nr_alt,n01_p1->sa_norm );
					                 norm12_afb(n01_p1->b_norm );
                           ret_code = OK;
 

	norm12_normby(n01_p1);

   return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm12_afm                                            */
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
size_t norm12_afm(char *sachnr,char *aus_feld)
#else
size_t norm12_afm(sachnr,aus_feld)
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
/*      Funktion:       norm12_afd                                            */
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
size_t norm12_afd(char *aus_feld)
#else
size_t norm12_afd(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;

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
/*      Funktion:       norm12_afs                                            */
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
size_t norm12_afs(char *sachnr,char *aus_feld)
#else
size_t norm12_afs(sachnr,aus_feld)
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
/*      Funktion:       norm12_afb                                            */
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
size_t norm12_afb(char *aus_feld)
#else
size_t norm12_afb(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;

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
/*      Funktion:       norm12_normnb                                            */
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
size_t norm12_normby(norm_p1 *n01_p1)
#else
size_t norm12_normby(n01_p1)
norm_p1 *n01_p1;
#endif
{
	   size_t ret_code=OK;
  	 char norm12_pruef[5];

		 strcpy(norm12_pruef,"0000");

	//size_t length = 5;

//	strcpy(dummy, n02_p1->m_norm);
	  // strcpy_s(norm12_pruef, length, "0000");


		 norm00_nb(n01_p1->normbyte,"SM");

	if  (((sach_nr[6] == '+')
		 || (sach_nr[11] == '+'))
		 || ((sach_nr[7] == '*')
		 ||  (sach_nr[8] == '*')
		 ||  (sach_nr[9] == '*')
		 ||  (sach_nr[10] == '*')
		 ||  (sach_nr[12] == '*')
		 ||  (sach_nr[13] == '*')
		 ||  (sach_nr[14] == '*')))
	{
			norm12_pruef[0] = '1';
	}

	if  (sach_nr[11] == 'Z')
	{
			norm12_pruef[1] = '1';
	}

	if (((sach_nr[12] == ' ') && (sach_nr[13] == ' ') && (sach_nr[14] == ' '))
		 ||  ((sach_nr[12] == '0') && (sach_nr[13] == '0') && (sach_nr[14] == '0')))
	{
			norm12_pruef[2] = '1';
	}

	if  (sach_nr[15] == 'Z')
	{
			norm12_pruef[3] = '1';
	}

	 if (strcmp(norm12_pruef,"0000") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SM");
		}
	 if (strcmp(norm12_pruef,"0001") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SC");
		}
	 if (strcmp(norm12_pruef,"0010") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SM");
		}
	 if (strcmp(norm12_pruef,"0011") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SC");
		}
	 if (strcmp(norm12_pruef,"0100") == 0)
		{
		norm00_nb(n01_p1->normbyte,"X ");
		}
	 if (strcmp(norm12_pruef,"0101") == 0)
		{
		norm00_nb(n01_p1->normbyte,"X ");
		}
	 if (strcmp(norm12_pruef,"0110") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SC");
		}
	 if (strcmp(norm12_pruef,"0111") == 0)
		{
		norm00_nb(n01_p1->normbyte,"X ");
		}
	 if (strcmp(norm12_pruef,"1000") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SA");
		}
	 if (strcmp(norm12_pruef,"1001") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SB");
		}
	 if (strcmp(norm12_pruef,"1010") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SA");
		}
	 if (strcmp(norm12_pruef,"1011") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SB");
		}
	 if (strcmp(norm12_pruef,"1100") == 0)
		{
		norm00_nb(n01_p1->normbyte,"X ");
		}
	 if (strcmp(norm12_pruef,"1101") == 0)
		{
		norm00_nb(n01_p1->normbyte,"X ");
		}
	 if (strcmp(norm12_pruef,"1110") == 0)
		{
		norm00_nb(n01_p1->normbyte,"SB");
		}
	 if (strcmp(norm12_pruef,"1111") == 0)
		{
		norm00_nb(n01_p1->normbyte,"X ");
		}

	 return (ret_code);
}
