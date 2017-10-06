/* -------------------------------------------------------------------------- */
/*      Modul:  norm13                                                        */
/*                                                                            */
/*      Entry:  norm13                                                        */
/*                                                                            */
/*      Aufgabe:        Bundeswehrsachnummer                                  */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:                                                               */
/*                     SNI AP 621                                             */
/*                                                                            */
/*      Version:        1.0 vom 07.04.93                                      */
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
size_t norm13_fp(char *zw_feld);
size_t norm13_auf(char einfeld[]);
size_t norm13_af1(norm_p1 *n01_p1);
size_t norm13_afm(char *sachnr,char    *aus_feld);
size_t norm13_afd(char    *aus_feld);
size_t norm13_afs(char    *sachnr,char    *aus_feld);
size_t norm13_afb(char *aus_feld);
size_t norm13_normby(norm_p1 *n01_p1);

/* #include <stdio.h>
#include <string.h>
#include <ctype.h> */
#else
size_t norm13_fp();
size_t norm13_auf();
size_t norm13_af1();
size_t norm13_afm();
size_t norm13_afd();
size_t norm13_afs();
size_t norm13_afb();
size_t norm13_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      Bundeswehrsachnummer                                  */
/*                                                                            */
/*      Aufrufe:        norm13_fp       Fehlerpruefung                        */
/*                      norm13_auf      Aufbereitung der Bloecke               */
/*			- Entfernen der SPACES in sachnr und 2                */
/*                      norm13_all      Allgemeine Verarbeitung               */
/*                      norm13_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
char sach_nr[26];
#ifdef __STDC__
size_t norm13(norm_p1 *n01_p1, char *zw_feld)
#else
size_t norm13(n01_p1, zw_feld)
norm_p1 *n01_p1;
char *zw_feld;
#endif
{
	size_t       ret_code = NOK;

	if((ret_code = norm13_fp(zw_feld)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	}
	if((ret_code = norm13_auf(zw_feld)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	}

	if((ret_code = norm13_af1(n01_p1)) == NOK)
	{
		norm00_nb(n01_p1->normbyte,NB_NR);
		return(ret_code);
	}


	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13_fp                                             */
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
size_t norm13_fp(char *zw_feld)
#else
size_t norm13_fp(zw_feld)
char *zw_feld;
#endif
{
	size_t	ret_code=NOK;
	size_t  i,j,k;
	char	c;
	size_t	laenge = 0;

	/* Beginn des 1. Blocks ermitteln */
	 for (i=0;( (i < 3) && (isalpha(c = zw_feld[i])  )  )  ; i++)
	 {
   }
   j = i;
   k = 0;
   for (;(k < 7) && (isdigit(c = zw_feld[j])); j++,k++)
   {
   }
   if ((zw_feld[j] == '-' ||
       zw_feld[j] == '\0') &&
			 k > 3)
       {
          /* o.k. */
       }
       else
       {
          return NOK;
       }
/*  PrÅfung auf zugelassene Werte der ersten beiden Stellen:    */

	 for (i=0;((i < 3) && (isalpha(c = zw_feld[i])))  ; i++)
	 {
   }

	if ((((zw_feld[i] == '1')
			&& (zw_feld[i+1] == '2'))
			|| ((zw_feld[i] == '2')
			&& (zw_feld[i+1] == '5'))
			|| ((zw_feld[i] == '4')
			&& (zw_feld[i+1] == '2'))
			|| ((zw_feld[i] == '4')
			&& (zw_feld[i+1] == '9'))
			|| ((zw_feld[i] == '5')
			&& (zw_feld[i+1] == '3'))
			|| ((zw_feld[i] == '5')
			&& (zw_feld[i+1] == '8'))
			|| ((zw_feld[i] == '5')
			&& (zw_feld[i+1] == '9'))
			|| ((zw_feld[i] == '6')
			&& (zw_feld[i+1] == '1'))
			||((zw_feld[i] == '6')
			&& (zw_feld[i+1] == '2'))
			|| ((zw_feld[i] == '6')
			&& (zw_feld[i+1] == '3'))
			|| ((zw_feld[i] == '6')
			&& (zw_feld[i+1] == '5'))
			|| ((zw_feld[i] == '6')
			&& (zw_feld[i+1] == '6'))
			|| ((zw_feld[i] == '6')
			&& (zw_feld[i+1] == '9')))
			|| ((zw_feld[i] == '8')
			&& (zw_feld[i+1] == '1')))
	{
/*  Stellenpruefung ok */
	}
	else
	{
		ret_code = NOK;
		return (ret_code);
	}

	laenge = strlen(zw_feld);


	if  (laenge < 26)
	{
		ret_code = OK;
	}
	else
	{
		ret_code = NOK;
	}
	 c=c;
	 return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13_auf                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Bundeswehrsachnummer                 */
/*                      Entfernen der Leerstellen in Block 1 und 2            */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm13_auf(char einfeld[])
#else
size_t norm13_auf(einfeld)
char einfeld[];
#endif
{
		size_t i, k, merk, punkt, sonst;
		char c;

/* ------------------------------------ */
/*      B l o c k  1 , 2 , 3 , 4 und 5  */
/* ------------------------------------ */

/* Verarbeitung Block 1  */

   merk = 0;

	 for (i = 0; einfeld[i] != '\0'; i++)
	 {
     sach_nr[i] = einfeld[i];

     if (einfeld[i] == '-')
     {
       merk = 1;
       i++;
       break;
     }
   }

   if (merk == 0)
   {
     if (i <= 10)
     {
			 sach_nr[i] = '-';
       i++;
       sach_nr[i] = '0';
       i++;
       sach_nr[i] = '\0';
       return OK;
     }
     else
     {
       return NOK;
     }
   }
/* Verarbeitung 2. Block */

   k=0;
   merk = 0;

   for (k = 0; einfeld[i] != '\0'; i++, k++)
   {
     if (einfeld[i] == '.')                /* Ende 2. Block           */
     {
        merk = 1;
        break;
     }
     if (isdigit (c = einfeld[i]))         /* 2. Block rein numerisch */
     {
				sach_nr[i] = einfeld[i];
     }
     else
     {
        return NOK;
     }
   }
   if (merk == 0)                           /* keinen Punkt gefunden,  */
   {                                        /* d.h. kein 3. Block      */
     if (k <= 6)                           /* 2. Block max. 6 Stellen */
     {
        sach_nr[i] = '\0';
        return OK;
		 }
     else
     {
        return NOK;
     }
   }
   if (isdigit (c = einfeld[i+1]))         /* Stelle nach dem Punkt numerisch */
   {
     sach_nr[i] = einfeld[i];               /* Punkt uebertragen */
     i++;
   }
   else
   {
		 return NOK;
   }

/* Verarbeitung Block 3  */

   k=0;
   punkt = 0;
   sonst = 0;

   for (k = 0; einfeld[i] != '\0'; i++, k++)
   {
     if (einfeld[i] == '.')                /* Ende 3. Block           */
     {
/*				punkt = 1;      */
				sonst = 1;
        break;
     }
     if ((einfeld[i] == '-') ||                /* Ende 3. Block           */
         (einfeld[i] == '/') ||
         (einfeld[i] == '_'))
     {
        sonst = 1;
        break;
     }

     if (isdigit (c = einfeld[i]))         /* 3. Block rein numerisch */
     {
				sach_nr[i] = einfeld[i];
     }
     else
     {
        return NOK;
     }
   }
   if ((punkt == 0) && (sonst == 0))        /* Punkt gefunden,  */
   {
     if (k <= 3)                           /* 3. Block max. 3 Stellen */
     {
        sach_nr[i] = '\0';
        return OK;
		 }
     else
     {
        return NOK;
     }
   }
   if (punkt == 1)
   {
     if ((isdigit(c = einfeld[i+1])) &&
         (einfeld[i+2] == '\0'))         /* Stelle nach dem Punkt numerisch */
     {
        sach_nr[i] = einfeld[i];               /* Punkt uebertragen */
        i++;
				sach_nr[i] = einfeld[i];
        i++;
        sach_nr[i] = '\0';
        return OK;
     }
     else
     {
     if ((isdigit(c = einfeld[i+1])) &&
         (einfeld[i+2] != '\0') &&
         (einfeld[i+3] == '\0'))         /* Stelle nach dem Punkt numerisch */
     {
        sach_nr[i] = einfeld[i];               /* Punkt uebertragen */
        i++;
				sach_nr[i] = einfeld[i];
        i++;
        sach_nr[i] = einfeld[i];
        i++;
        sach_nr[i] = '\0';
        return OK;
     }
     else
     {
       return NOK;
     }
    }
   }
   if (sonst == 1)
   {
      sach_nr[i] = einfeld[i];
      i++;
      for (k=0; (k<=1) && (einfeld[i] != '\0'); k++, i++)
      {
         if (!isalnum(c = einfeld[i]))
         {
            return NOK;
         }
         sach_nr[i] = einfeld[i];
      }
      if (einfeld[i] != '\0')
      {
         return NOK;
      }
      sach_nr[i] = '\0';
      return OK;
   }
   c=c;
	 return NOK;
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm13_afm      Aufbereitung MASCHINELL               */
/*                      norm13_afd      Aufbereitung DRUCK                    */
/*                      norm13_afs      Aufbereitung SORTIERT                 */
/*                      norm13_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm13_af1(norm_p1 *n01_p1)
#else
size_t norm13_af1(n01_p1)
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
       for (;i<25;i++)
       {
	         sach_nr[i] = ' ';
       }
	  }
	}

  sach_nr[25] = '\0';


					norm13_afm(sach_nr,n01_p1->m_norm);
					norm13_afm(sach_nr,n01_p1->ma_norm);
					norm13_afs(sach_nr,n01_p1->s_norm );
					norm13_afs(sach_nr,n01_p1->sa_norm );
					norm13_afd(n01_p1->d_norm );
					norm13_afb(n01_p1->b_norm );
					ret_code = OK;


	norm13_normby(n01_p1);

  return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13_afm                                            */
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
size_t norm13_afm(char *sachnr,char *aus_feld)
#else
size_t norm13_afm(sachnr,aus_feld)
char  *sachnr;
char  *aus_feld;
#endif
{
	size_t      ret_code = OK;
	size_t      i,j,z,zaehl;
	char		c;
   z=0;

   for (i=0;((i < 3) && (isalpha(c = sachnr[i])))  ; i++)
	 {
   z++;
   }
   i=0;j=0;
   for (i=0;(i < 4-z); i++,j++)
	 {
   aus_feld[j] = ' ';
   }

  i=0;
  zaehl = 0;

	while (sachnr[i] != '\0')
	{
      if (((sach_nr[i] == '.') ||
          (sach_nr[i] == '-') ||
          (sach_nr[i] == '_') ||
          (sach_nr[i] == '/')) &&
          (j > 11))
      {
          zaehl++;
      }
      if ((sach_nr[i] == '-') && (j < 11))
      {
        for (;j < 11;j++)
        {
          aus_feld[j] = ' ';
        }
      }
      if (zaehl == 1)
      {
        for (;j < 18;j++)
        {
          aus_feld[j] = ' ';
        }
      }
      if (zaehl == 2)
      {
        for (;j < 22;j++)
        {
          aus_feld[j] = ' ';
        }
      }

	    aus_feld[j] = sachnr[i];
	    i++;j++;
	}

	for (; j<25; j++)
	{
	    aus_feld[j] = ' ';
	}
	aus_feld[25] = '\0';

  c=c;

  return (ret_code);



}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13_afd                                            */
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
size_t norm13_afd(char    *aus_feld)
#else
size_t norm13_afd(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       j=0;

	i=0;j=0;

	while (sach_nr[i] != '\0')
	{
			if ((sach_nr[i] == '-') && (sach_nr[i+1] == '0') &&
				 (sach_nr[i+2] == ' '))
				 {
					 sach_nr[i]   = ' ';
					 sach_nr[i+1] = ' ';
					 sach_nr[i+2] = ' ';
				 }



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
/*      Funktion:       norm13_afs                                            */
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
size_t norm13_afs(char *sachnr,char  *aus_feld)
#else
size_t norm13_afs(sachnr,aus_feld)
char  *sachnr;
char  *aus_feld;
#endif
{

	size_t      ret_code = OK;
	size_t      i,j,z,zaehl;
	char		c;
   z=0;


   for (i=0;((i < 3) && (isalpha(c = sachnr[i])))  ; i++)
	 {
   z++;
   }
   i=0;j=0;

   if (z == 1)
    {
       z=3;
    }
    else
    {
    if (z == 3)
     {
	z = 1;
     }
    }
    if (z == 0)
    {
      z = 4;
    }

  i=0;
  zaehl = 0;

	while (sachnr[i] != '\0')
	{
      if (((sach_nr[i] == '.') ||
	  (sach_nr[i] == '-') ||
	  (sach_nr[i] == '_') ||
	  (sach_nr[i] == '/')) &&
	  (j > (11-z)))
      {
	  zaehl++;
      }
      if ((sach_nr[i] == '-') && (j < 11))
      {
	for (;j < (11-z);j++)
	{
	  aus_feld[j] = ' ';
	}
      }
      if (zaehl == 1)
      {
	for (;j < (18-z);j++)
	{
	  aus_feld[j] = ' ';
	}
      }
      if (zaehl == 2)
      {
	for (;j < (22-z);j++)
	{
	  aus_feld[j] = ' ';
	}
      }

	    aus_feld[j] = sachnr[i];
	    i++;j++;
	}

	for (; j<25; j++)
	{
	    aus_feld[j] = ' ';
	}
	aus_feld[25] = '\0';

  c=c;



	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm13_afb                                            */
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
size_t norm13_afb(char *aus_feld)
#else
size_t norm13_afb(aus_feld)
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
			if ((sach_nr[i] == '-') && (sach_nr[i+1] == '0') &&
				 (sach_nr[i+2] == ' '))
				 {
					 sach_nr[i]   = ' ';
					 sach_nr[i+1] = ' ';
					 sach_nr[i+2] = ' ';
				 }



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
/*      Funktion:       norm13_normnb                                            */
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
size_t norm13_normby(norm_p1 *n01_p1)
#else
size_t norm13_normby(n01_p1)
norm_p1 *n01_p1;
#endif
{
	   size_t	ret_code=OK;
	   size_t   i;
	   char		c;


	   norm00_nb(n01_p1->normbyte,"V ");

         i=0;
	       if (isdigit(c = sach_nr[i]))
		     {
	          norm00_nb(n01_p1->normbyte,"V ");
         }
         else
         {
	         norm00_nb(n01_p1->normbyte,"VU");
         }
	 c=c;
	 return (ret_code);
}
