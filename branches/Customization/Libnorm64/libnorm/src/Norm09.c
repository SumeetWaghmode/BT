/* -------------------------------------------------------------------------- */
/*      Modul:  norm09							*/
/*                                                                            */
/*      Entry:  norm09							*/
/*                                                                            */
/*      Aufgabe:        Aufbereitung Sachnummer Vertriebliches Nummernsystem  */
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
/*      Version:        1.0 vom 17.08.92                                      */
/*                                                                            */
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
size_t norm09_fp(char *zw_feld,size_t *z_laenge);
size_t norm09_auf(char einfeld[],char block1[]);
size_t norm09_all(char *block1);
size_t norm09_af1(norm_p1 *n01_p1);
size_t norm09_afm(char *aus_feld);
size_t norm09_afd(char *aus_feld);
size_t norm09_afs(char *aus_feld);
size_t norm09_afb(char *aus_feld);
size_t norm09_normby(norm_p1 *n01_p1);

/* #include <ctype.h>
#include <string.h> */
#else
size_t norm09_fp();
size_t norm09_auf();
size_t norm09_all();
size_t norm09_af1();
size_t norm09_afm();
size_t norm09_afd();
size_t norm09_afs();
size_t norm09_afb();
size_t norm09_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09						*/
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      Sachnummer vertriebliches Nummernsystem               */
/*                                                                            */
/*      Aufrufe:        norm09_fp       Fehlerpruefung                        */
/*                      norm09_auf      Aufbereitung der Bloecke               */
/*			- Entfernen der SPACES in Block1 und 2                */
/*                      norm09_all      Allgemeine Verarbeitung               */
/*                      norm09_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
char sach_nr[26];
#ifdef __STDC__
size_t norm09(norm_p1 *n01_p1, char *zw_feld)
#else
size_t norm09(n01_p1, zw_feld)
norm_p1 *n01_p1;
char *zw_feld;
#endif
{
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	        *z_laenge   =  &laenge;
	char            zblock1_2[26];
	char            *block1 = zblock1_2;

	if((ret_code = norm09_fp(zw_feld,z_laenge)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	}
	if((ret_code = norm09_auf(n01_p1->eingfeld,block1)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
								return(ret_code);
	}
		if((ret_code = norm09_all(block1)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	}


	if((ret_code = norm09_af1(n01_p1)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
								return(ret_code);
	}


			return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09_fp                                             */
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
size_t norm09_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm09_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	size_t	ret_code=NOK;
	size_t	i,a;
	char	c;


/*  Prfung:   */
/*  erster Block kann alleine stehen, ohne Bindestrich                */
/*  es duerfen nicht vorkommen I, O oder Umlaute                      */
/*  wenn 1. Block alleine steht, an 7. Stelle nur A bis T zugelassen  */
/*  wenn 2. Block vorhanden, an 7. Stelle 0 - 9 oder Z zugelassen     */

	 if (((isdigit(c=zw_feld[3]) || isalpha(c=zw_feld[3]))
	   && (isdigit(c=zw_feld[4]) || isalpha(c=zw_feld[4]))
	   && (isdigit(c=zw_feld[5]) || isalpha(c=zw_feld[5]))
	   && (isdigit(c=zw_feld[6]) || isalpha(c=zw_feld[6]))
     && (zw_feld[7] == '\0' && (zw_feld[6] >= 'A' && zw_feld[6] <= 'T'))

	   || ((isdigit (c=zw_feld[3])) || isalpha (c=zw_feld[3]))
	   && (isdigit (c=zw_feld[4]) || isalpha (c=zw_feld[4]))
	   && (isdigit (c=zw_feld[5]) || isalpha (c=zw_feld[5]))
	   && (isdigit (c=zw_feld[6]) || zw_feld[6] == 'Z')
	   && (zw_feld[7] == '-')
     && (isdigit (c=zw_feld[8]) || isalpha (c=zw_feld[8]))))

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
	   if (zw_feld[i] == 'I' || zw_feld[i] == 'O')
		 {
		    ret_code = NOK;
        return (ret_code);
     }
  }
  if ((zw_feld[7] == '-')
     && (zw_feld[8] == '\0'))
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
   c=c; /* vermeidet Warnings */
	 return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09_auf                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Vertriebliches Sachnummer  */
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
size_t norm09_auf(char einfeld[],char block1[])
#else
size_t norm09_auf(einfeld, block1)
char einfeld[];
char block1[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k;
	char	c;

/* ---------------------------- */
/*      B l o c k  1  und  2    */
/* -----------------------------*/

	k = 0;


	for (i = 0; einfeld[i] != '\0'; i++)
	{
		if  (einfeld[i] != ' ')
		    {
			   block1[k] = einfeld[i];
			   if (islower (c = block1[k]))
			      {	       
			      block1[k] = toupper(c);
			      }
			   k++;
		      }
		      else
		    {
		    }
		    continue;
	  }


		 block1[k] = '\0';

              if (einfeld[i] == '\0')
		 {
		 return (ret_code);
		 }

return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09_all                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Vertrieblichen Sachnummer           */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm09_all(char *block1)
#else
size_t norm09_all(block1)
char *block1;
#endif
{
	size_t ret_code = OK;
	size_t i,j;
	char	c;
/*	char wert[7]; */

/* -------------------------------------------------------------------------- */
/*    B l o c k    1                                                          */
/* -------------------------------------------------------------------------- */
	 j=0;
	 for (i=0;i<25; i++)
	 {
	 sach_nr[i] = ' ';
	 }
	 sach_nr[i] = '\0';
	i=0;
	while (block1[i] != '\0')
	    {
			sach_nr[j] = block1[i];
	    i++;j++;
	    }

	for (; j<25; j++)
	    {
	    sach_nr[j] = ' ';
	    }
	sach_nr[j] = '\0';
       sach_nr[25] = '\0';

	i=8;
       for (i=8;i<25;i++)
       {
       if (isalpha(c=sach_nr[i])
	  || isdigit(c=sach_nr[i])
	  || (sach_nr[i] == ' '))
	 {
	 }
	 else
	 {
	 ret_code = NOK;
	 }
       }
       if ((sach_nr[7] == '-')
	  && (sach_nr[8] == ' '))
	  {
	  ret_code = NOK;
	  }
   c=c; /* vermeidet Warnings */
   return (ret_code);
}
				     
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm09_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm09_afd      Aufbereitung DRUCK                    */
/*                      norm09_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm09_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm09_af1(norm_p1 *n01_p1)
#else
size_t norm09_af1(n01_p1)
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


					norm09_afm(n01_p1->m_norm);
					norm09_afd(n01_p1->d_norm );
					norm09_afs(n01_p1->s_norm );
					norm09_afb(n01_p1->b_norm );
					norm09_afm(n01_p1->ma_norm);
					norm09_afs(n01_p1->sa_norm);
					ret_code = OK;


	norm09_normby(n01_p1);

        return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09_afm                                            */
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
size_t norm09_afm(char *aus_feld)
#else
size_t norm09_afm(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i,j;

	i=0;j=0;

	while (sach_nr[i] != '\0')
			{
	    aus_feld[j] = sach_nr[i];
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
/*      Funktion:       norm09_afd                                            */
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
size_t norm09_afd(char *aus_feld)
#else
size_t norm09_afd(aus_feld)
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
/*      Funktion:       norm09_afs                                            */
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
size_t norm09_afs(char *aus_feld)
#else
size_t norm09_afs(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i,j;

	i=0;
	j=0;


	while (sach_nr[i] != '\0')
	    {
	    aus_feld[j] = sach_nr[i];
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
/*      Funktion:       norm09_afb                                            */
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
size_t norm09_afb(char *aus_feld)
#else
size_t norm09_afb(aus_feld)
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

	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm09_normnb                                            */
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
size_t norm09_normby(norm_p1 *n01_p1)
#else
size_t norm09_normby(n01_p1)
norm_p1 *n01_p1;
#endif
{
	   size_t ret_code=OK;
	   
	   norm00_nb(n01_p1->normbyte,"A ");

	 return (ret_code);
}
