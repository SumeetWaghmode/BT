/* -------------------------------------------------------------------------- */
/*      Modul:  norm04                                                        */
/*                                                                            */
/*      Entry:  norm04                                                        */
/*                                                                            */
/*      Aufgabe:        Aufbereitung Siemens Fabrikatenummer                  */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Androleit                                             */
/*                      SNI AP 623                                            */
/*                                                                            */
/*      Version:        1.0 vom 21.03.91                                      */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*      Allgemeine Definitionen                                               */
/* -------------------------------------------------------------------------- */
#define OK 0
#define NOK 1

#define D_KONST 'D'
#define N_KONST 'N'
#define S_KONST 'S'
#define C_KONST 'C'
#define P_KONST 'P'
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
size_t norm04_fp(char *zw_feld,size_t *z_laenge);
size_t norm04_d(char *zw_feld,char *auf_feld_1,size_t *z_laenge);
size_t norm04_s(char *zw_feld,char *auf_feld_1,size_t *z_laenge);
size_t norm04_n(char *zw_feld,char *auf_feld_1,size_t *z_laenge);
size_t norm04_p(char *zw_feld,char *auf_feld_1,size_t *z_laenge);
size_t norm04_af1(norm_p1 *n02_p1,char *zw_feld,char *auf_feld_1,size_t *z_laenge);
size_t norm04_afm(char *aus_feld);
size_t norm04_afd(char *aus_feld);
size_t norm04_afs(char *aus_feld);
size_t norm04_afb(char *aus_feld,char *auf_feld_1,size_t *z_laenge);
size_t norm04_normby(norm_p1 *n02_p1,char *auf_feld_1);
/* #include <ctype.h>
#include <string.h>  */
#else
size_t norm04_fp();
size_t norm04_d();
size_t norm04_s();
size_t norm04_n();
size_t norm04_p();
size_t norm04_af1();
size_t norm04_afm();
size_t norm04_afd();
size_t norm04_afs();
size_t norm04_afb();
size_t norm04_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      Siemens-Fabrikatenummer                               */
/*                      (D, S, N und C)                                       */
/*      Aufrufe:        norm04_fp       Fehlerpruefung                        */
/*                      norm04_d        Verarbeitung Fabrikat D               */
/*                                      Fremdsachnummer mit ':'               */
/*                      norm04_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
char sach_nr[26];

#ifdef __STDC__
size_t norm04(norm_p1 *n02_p1, char *zw_feld)
#else
size_t norm04(n02_p1, zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            auf_feld[9];
	char    	*auf_feld_1 = auf_feld;
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	        *z_laenge   =  &laenge;
	size_t             i;
	for (i=0;i < 25;i++)
	{
		sach_nr[i] = ' ';
	}

	if((ret_code = norm04_fp(zw_feld,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}

    /*  Auswahl  */

    switch(*(zw_feld+0))
    {
	case D_KONST:
	{
		if((ret_code = norm04_d(zw_feld,auf_feld_1,z_laenge)) == NOK)
		{
			norm00_nb(n02_p1->normbyte,NB_NR);
			return(ret_code);
		}
		break;
	}
	case S_KONST:
	{
		if((ret_code = norm04_s(zw_feld,auf_feld_1,z_laenge)) == NOK)
		{
			norm00_nb(n02_p1->normbyte,NB_NR);
			return(ret_code);
		}
	break;
	}

	case N_KONST:
	{
		if((ret_code = norm04_n(zw_feld,auf_feld_1,z_laenge)) == NOK)
		{
			norm00_nb(n02_p1->normbyte,NB_NR);
			return(ret_code);
		}
	break;
	}

	case C_KONST:
	{
		if((ret_code = norm04_d(zw_feld,auf_feld_1,z_laenge)) == NOK)
		{
			norm00_nb(n02_p1->normbyte,NB_NR);
			return(ret_code);
		}
	break;
	}
	case P_KONST:
	{
		if((ret_code = norm04_p(zw_feld,auf_feld_1,z_laenge)) == NOK)
		{
			norm00_nb(n02_p1->normbyte,NB_NR);
			return(ret_code);
		}
	break;
	}

    }

	if((ret_code = norm04_af1(n02_p1,zw_feld,auf_feld_1,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}

      return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_fp                                             */
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
size_t norm04_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm04_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
           size_t ret_code=NOK;
           size_t pruef,i;
	/*  Auswahl  */

	switch(*(zw_feld+0))
	{

	case D_KONST:
	{
  pruef=0;
  for (i=2;i < 8 && zw_feld[i] != '\0' ;i++)
  {
  if (zw_feld[i] == '-')
  {
  pruef++;
  }
  }
	   if ((*(zw_feld+1)     ==  ':'
	      || *(zw_feld+1)    ==  '-')
	      && ((strlen(zw_feld) < 13       /* 10.11.93, mg, Fehlermeldung fuer  */
        && strlen(zw_feld) > 3        /* Nr. D:KKKHLMN, Aenderung von < 13 */
	      && pruef == 1)                /* auf < 12                          */
        || (strlen(zw_feld) < 8        /* wurde am 23.03.94 wieder auf 13   */
        && strlen(zw_feld) > 3         /* geaendert - Bindestrich zwischen  */
        && pruef == 0)))               /* 1. und 2. Block zwingend;Androleit*/
	   {
	       ret_code = OK;
	       *z_laenge = strlen(zw_feld);
	   }
	   break;
	 }

	case N_KONST:
	{
	   if ((*(zw_feld+1)     ==  ':'
	      || *(zw_feld+1)    ==  '-')
	      && strlen(zw_feld) < 23
	      && strlen(zw_feld) > 3)
	   {
	       ret_code = OK;
	       *z_laenge = strlen(zw_feld);
	   }
	   break;
	}
	
	case S_KONST:
	{
	   if ((*(zw_feld+1)     ==  ':'
	      || *(zw_feld+1)    ==  '-')
	      && strlen(zw_feld) < 26
	      && strlen(zw_feld) > 3)
	   {
	       ret_code = OK;
	       *z_laenge = strlen(zw_feld);
	   }
	break;
	}


	case C_KONST:
	{
	   if ((*(zw_feld+1)     ==  ':'
	      || *(zw_feld+1)    ==  '-')
	      && strlen(zw_feld) < 13
	      && strlen(zw_feld) > 3)
	   {
	       ret_code = OK;
	       *z_laenge = strlen(zw_feld);
	   }
	   break;
	}
	case P_KONST:
	{
	   if ((*(zw_feld+2)     ==  ':'
	      || *(zw_feld+2)    ==  '-')
	      && strlen(zw_feld) < 12
	      && strlen(zw_feld) > 3
	      && *(zw_feld+1)     == 'N')
	   {
	       ret_code = OK;
	       *z_laenge = strlen(zw_feld);
	   }
	   break;
	}
       }
     
       return (ret_code);
}                             
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_D                                              */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Fabrikatenummer  Sachnummer KZ D:    */
/*                                                                   KZ C:    */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm04_d(char *zw_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm04_d(zw_feld,auf_feld_1,z_laenge)
char *zw_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t	ret_code = OK;
	size_t	i, i11, i12, k, i13;
	char	c;
	auf_feld_1[0] = ' ';
	k=0;
/* -------------------------------------------------------------------------- */
/*    Verarbeitung : "Kein Barcode"                                           */
/*                                                                            */
/*    Errechnen LÑnge 1. Block                                                */
/* -------------------------------------------------------------------------- */
/*   Errechnen LÑnge 1. Block und PrÅfen auf numerisch                        */

/*	if (zw_feld[1] == ':')          */
/*	{                               */

	i=0;
	while (i<2)
	{
	    sach_nr[i]=zw_feld[i];
	    i++;
	}

	i12 = 1;

	for (i = 2; i < 8; i++)
	{
	    i11 = i;
	    if (i == *z_laenge)
	    {
	      i11 = *z_laenge;
	      i12 = i;
	      break;
	    }

	    if (   zw_feld[i] == '-')
		  {
		     i11 = *z_laenge;
		     i12 = i;
		     break;
		  }
	    if ((isdigit (c = zw_feld[i]))
	       || (isalpha (c=zw_feld[i])))
	       {
	       sach_nr[i]=zw_feld[i];
	       }
	    else
	       {
		ret_code = NOK;
		return (ret_code);
	       }
	}

/*  Ausrichten  1. Block  */

    if (i != 8)
    {
       k = 8 - i;
    }
    i=i-1;
    switch(k)
	   {
	    case 0:
		      i12=6;
		      break;
	    case 1:   sach_nr[i]    =  zw_feld[i];
		      sach_nr[i-1]  =  zw_feld[i-1];
		      sach_nr[i-2]  =  zw_feld[i-2];
		      sach_nr[i-3]  =  zw_feld[i-3];
		      sach_nr[i-4]  =  zw_feld[i-4];
		      break;
	    case 2:   sach_nr[i+1]  =  zw_feld[i];
		      sach_nr[i]    =  zw_feld[i-1];
		      sach_nr[i-1]  =  zw_feld[i-2];
		      sach_nr[i-2]  =  zw_feld[i-3];
		      sach_nr[i-3]  =   ' ';
		      break;
	    case 3:   sach_nr[i+2]  =  zw_feld[i];
		      sach_nr[i+1]  =  zw_feld[i-1];
		      sach_nr[i]    =  zw_feld[i-2];
		      sach_nr[i-1]  =  ' ';
		      sach_nr[i-2]  =  ' ';
		      break;
	    case 4:   sach_nr[i+3]  =  zw_feld[i];
		      sach_nr[i+2]  =  zw_feld[i-1];
		      sach_nr[i+1]  =  ' ';
		      sach_nr[i]    =  ' ';
		      sach_nr[i-1]  =  ' ';
		      break;
	    case 5:   sach_nr[i+4]  =  zw_feld[i];
		      sach_nr[i+3]  =  ' ';
		      sach_nr[i+2]  =  ' ';
		      sach_nr[i+1]  =  ' ';
		      sach_nr[i]    =  ' ';

	    }

	if (i11 > i12)
	{
	sach_nr[7] = '-';
	i12++;
	  if ((isdigit (c = zw_feld[i12]))
		|| (isalpha (c = zw_feld[i12])))
	  {
	     sach_nr[8] = zw_feld[i12];
	  }
	  else
	  {
	     ret_code = NOK;
	     return (ret_code);
	  }

	  i12++;

	  for (i = 9; i < 12; i++)
	  {
	     if (zw_feld[i12] != ' ')
	     {
     		sach_nr[i] = zw_feld[i12];
	     }
	     else
	     {
		    sach_nr[i] = ' ';
	     }
	     i12++;
	  }
    /* geaendert wenn Block 2 > 4  RETURN_CODE = NOK  23.03.94 AND  */

    if (zw_feld[i12] != '\0')
    {
	     ret_code = NOK;
	     return (ret_code);
    }

	  i13=strlen(sach_nr);
	  for (i=i13;i < 26;i++)
	  {
	    sach_nr[i] = ' ';
	  }
	  sach_nr[25] = '\0';

/*      }     */

     }
/* -------------------------------------------------------------------------- */
/*    Verarbeitung : " Barcode"                                            */
/* -------------------------------------------------------------------------- */
       if (sach_nr[1] == '-')
       {

					   
	   for (i=2;i < *z_laenge; i++)
	       {
	
	    if (   sach_nr[i] == KLEINAE
			|| sach_nr[i] == KLEINOE
			|| sach_nr[i] == KLEINUE
			|| sach_nr[i] == GROSSAE
			|| sach_nr[i] == GROSSOE
			|| sach_nr[i] == GROSSUE
			|| sach_nr[i] == ':'
			|| sach_nr[i] == '*')

                        {
			ret_code = NOK;
			return (ret_code);		
		        }
	       }
       }
  c=c; /* vermeidet Warnings */
  return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_N                                              */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Fabrikatenummer  Sachnummer KZ N:    */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm04_n(char *zw_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm04_n(zw_feld,auf_feld_1,z_laenge)
char *zw_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t ret_code = OK;
	size_t i,i11,i12,i13,i14,i15;
/*	char zw_ein_feld[8];    */
	auf_feld_1[0] = ' ';

/* -------------------------------------------------------------------------- */
/*    Verarbeitung : "Kein Barcode"                                           */
/*                                                                            */
/*    Errechnen LÑnge 1. Block                                                */
/* -------------------------------------------------------------------------- */
/*   Errechnen LÑnge 1. Block und PrÅfen auf 3-6 kein "+"                     */


  	for (i = 31; (((zw_feld[i] == ' ') || (zw_feld[i] == '\0'))
               && (i > 0)); i--)
	{
		if  (zw_feld[i] == ' ')
		    {
        zw_feld[i] = '\0';
        }
  }


	i=0;
	while (i<2)
	{
	sach_nr[i]=zw_feld[i];
	i++;
	}

	i12 = 1;

	for (i = 2; i < 6 ; i++)
	{
		if ( zw_feld[i] == '+')
	   { 
			ret_code = NOK;
			return (ret_code);
	   }
	}

	for (i = 2; i < 13; i++)
	{

	    if (   zw_feld[i] == '-')
		 {
		     i12 = i;
		     break;
		 }
	       sach_nr[i]=zw_feld[i];
	}

/* 9.12.93, mg, Fehlermeldung v. Hr. Sobotta fuer folgende Nummer:
   n:49957.00.7.28        richtig ist nb = x
*/

 if ( i12 == 1)
  {
    i12=i;
  }

   i11 = *z_laenge;

/*	if (i11 > i12) */ /*  10.03.94, androleit siehe 9.12.93 an 13.Stelle
    ein Bindestrich  */

  if ((i11 > i12) && (zw_feld[i] == '-'))
	{
		sach_nr[12] = '-';
		i12++;
		i14=i12;
		i15=0;
		for (i=i12; i < i11;i++)
		{
			if (zw_feld[i] != ' ')
	   	{
	   		i15++;
	   	}
		}
		if ((i15 > 10) || (i15 == 0))
		{
	  		ret_code = NOK;
			return (ret_code);
		}

		for (i = 13; ((i < 23) && (zw_feld[i12] != '\0')); i++)
	  	{
	     if (zw_feld[i12] != ' ')
	     {
				sach_nr[i] = zw_feld[i12];
	     }
	     else
	     {
				sach_nr[i] = ' ';
	     }
	     i12++;
		}
		sach_nr[i] = '\0';
		i13=strlen(sach_nr);

	  	if (i13 > 23)
	  	{
			ret_code=NOK;
		  	return (ret_code);
	  	}
	  	for (i=i13;i < 26;i++)
	  	{
	    	sach_nr[i] = ' ';
	  	}
		sach_nr[25] = '\0';
	}
	else
	{
/* folgenden If am 10.03.94 gem. o.a. Fehler vom 09.12.93 eingefuegt */
  if (i11 >= i12)
  {
     ret_code=NOK;
     return(ret_code);
  }

	for (i=i11;i < 26;i++)
	  	{
	    	sach_nr[i] = ' ';
	  	}
		sach_nr[25] = '\0';
	}
/* }     */

/* -------------------------------------------------------------------------- */
/*    Verarbeitung : " Barcode"                                            */
/* -------------------------------------------------------------------------- */
       if (sach_nr[1] == '-')
       {
       
					   
	   for (i=2;i < *z_laenge; i++)
	       {
	
		    if (   sach_nr[i] == KLEINAE
			|| sach_nr[i] == KLEINOE
			|| sach_nr[i] == KLEINUE
			|| sach_nr[i] == GROSSAE
			|| sach_nr[i] == GROSSOE
			|| sach_nr[i] == GROSSUE
			|| sach_nr[i] == ':'
			|| sach_nr[i] == '*')

                        {
			ret_code = NOK;
			return (ret_code);		
		        }
  
	       }
       }
  i14=i14; /* vermeidet Warnings */
  return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_S                                              */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Fabrikatenummer  Sachnummer KZ S:    */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm04_s(char *zw_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm04_s(zw_feld,auf_feld_1,z_laenge)
char *zw_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t ret_code = OK;
	size_t i,i13;
	auf_feld_1[0] = ' ';

/* -------------------------------------------------------------------------- */
/*    Verarbeitung : "Kein Barcode"                                           */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */

		 

	for (i = 0; i < 26; i++)
	  {
	    if ( zw_feld[i] != ' ')
	      { 
	       sach_nr[i]=zw_feld[i];
	      }
	     else
	     {
	      sach_nr[i] = ' ';
	     }
	  }
	  
	  i13=strlen(sach_nr);
	  for (i=i13;i < 26;i++)
	  {
	    sach_nr[i] = ' ';
	  }

	  sach_nr[25] = '\0';


/* -------------------------------------------------------------------------- */
/*    Verarbeitung : " Barcode"                                            */
/* -------------------------------------------------------------------------- */
       if (sach_nr[1] == '-')
       {
       
					   
	   for (i=2;i < *z_laenge; i++)
	       {

		    if (   sach_nr[i] == KLEINAE
			|| sach_nr[i] == KLEINOE
			|| sach_nr[i] == KLEINUE
			|| sach_nr[i] == GROSSAE
			|| sach_nr[i] == GROSSOE
			|| sach_nr[i] == GROSSUE
			|| sach_nr[i] == ':'
			|| sach_nr[i] == '*')

                        {
			ret_code = NOK;
			return (ret_code);		
		        }
  
	       }
       }

return (ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_P                                              */
/*                                                                            */
/*      Aufgabe:        Aufbereitung der Produkstammnummer  (PSTNR)           */
/*                                                                            */
/*      Aufrufe:        norm                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm04_p(char *zw_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm04_p(zw_feld,auf_feld_1,z_laenge)
char *zw_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t ret_code = OK;
	size_t i,i13;
	auf_feld_1[0] = ' ';

/* -------------------------------------------------------------------------- */
/*    Verarbeitung : "Kein Barcode"                                           */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */

		 

	for (i = 0; i < 26; i++)
	  {
	    if ( zw_feld[i] != ' ')
	      { 
	       sach_nr[i]=zw_feld[i];
	      }
	     else
	     {
	      sach_nr[i] = ' ';
	     }  
	  }
	  
	  i13=strlen(sach_nr);
	  for (i=i13;i < 26;i++)
	  {
	    sach_nr[i] = ' ';
	  }

	  sach_nr[25] = '\0';


/* -------------------------------------------------------------------------- */
/*    Verarbeitung : " Barcode"                                            */
/* -------------------------------------------------------------------------- */
       if ((sach_nr[2] == '-')
	  || (sach_nr[2] == ':'))
       {
       
					   
	   for (i=3;i < *z_laenge; i++)
	       {
	
	    if (   sach_nr[i] == KLEINAE
			|| sach_nr[i] == KLEINOE
			|| sach_nr[i] == KLEINUE
			|| sach_nr[i] == GROSSAE
			|| sach_nr[i] == GROSSOE
			|| sach_nr[i] == GROSSUE
			|| sach_nr[i] == ':'
			|| sach_nr[i] == '*')

			{
			ret_code = NOK;
			return (ret_code);
			}

	       }
       }

return (ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm04_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm04_afd      Aufbereitung DRUCK                    */
/*                      norm04_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm04_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm04_af1(norm_p1 *n02_p1,char *zw_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm04_af1(n02_p1,zw_feld,auf_feld_1,z_laenge)
norm_p1 *n02_p1;
char *zw_feld;
char *auf_feld_1;
size_t *z_laenge;
#endif
{
	size_t       ret_code = NOK;

        /* Auswahl */

 
					norm04_afm(n02_p1->m_norm);
					norm04_afd(n02_p1->d_norm);
					norm04_afs(n02_p1->s_norm);
					norm04_afb(n02_p1->b_norm,auf_feld_1,z_laenge);
					norm04_afm(n02_p1->ma_norm);
					norm04_afs(n02_p1->sa_norm);
					ret_code = OK;

	norm04_normby(n02_p1,auf_feld_1);
	*zw_feld = *zw_feld;

        return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_afm                                            */
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
size_t norm04_afm(char *aus_feld)
#else
size_t norm04_afm(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i;

	i=0;
	while (i<2)
	    {
	    aus_feld[i]=sach_nr[i];
	    i++;
	    }
	if (aus_feld[1] == '-')
	{
	    aus_feld[1] = ':';
        }

		for (i = 2; sach_nr[i] != '\0'; i++)
	  {
	      if (sach_nr[i] != ' ')
	      {
		   aus_feld[i] = sach_nr[i];
	      }
	      else
	      {
		  aus_feld[i] = ' ';
	      }
	   }

			

	 aus_feld[25] = '\0';

	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_afd                                            */
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
size_t norm04_afd(char *aus_feld)
#else
size_t norm04_afd(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       i2=0;
	size_t       j=0;

	while (i<2)
	    {
	    aus_feld[i]=sach_nr[i];
	    i++;
	    }

	if (aus_feld[1] == '-')
	{
	    aus_feld[1] = ':';
	}
		j=2;

		for (i = 2; sach_nr[i] != '\0'; i++)
		{
		     if (sach_nr[i] != ' ')
		     {
			 aus_feld[j] = sach_nr[i];
			 j++;
		     }
		     else
		     {
		     }
		}
		for (;i<26;i++)
		{
		 aus_feld[i] = ' ';
		}



	     aus_feld[25] = '\0';

	i2=i2; /* vermeidet Warnings */
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_afs                                            */
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
size_t norm04_afs(char *aus_feld)
#else
size_t norm04_afs(aus_feld)
char *aus_feld;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
	size_t       i2=0;
	size_t       j=0;


	i=0;
	while (i<2)
	    {
	    aus_feld[i]=sach_nr[i];
	    i++;
	    }
	if (aus_feld[1] == '-')
	{
	    aus_feld[1] = ':';
        }


		for (i = 2; sach_nr[i] != '\0'; i++)
		{
	      if (sach_nr[i] != ' ')
	      {
		   aus_feld[i] = sach_nr[i];
	      }
	      else
	      {
		  aus_feld[i] = ' ';
	      }

	   }


	 aus_feld[25] = '\0';
   j=j; i2=i2;/* vermeidet Warnings */
	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_afb                                            */
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
size_t norm04_afb(char *aus_feld,char *auf_feld_1,size_t *z_laenge)
#else
size_t norm04_afb(aus_feld,auf_feld_1,z_laenge)
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

	while (i<2)
	    {
	    aus_feld[i] = sach_nr[i];
	    i++;
	    }
	if (aus_feld[1] == ':')
	    {
	    aus_feld[1] = '-';
	    }

	if (auf_feld_1[0] != ':')
		{
		j=2;
		for (i = 2; sach_nr[i] != '\0'; i++)
		  {
		    if (   (isalpha (c = sach_nr[i]))
			|| (isdigit (c = sach_nr[i]))
			|| sach_nr[i] ==  '-'
			|| sach_nr[i] ==  '.'
			|| sach_nr[i] ==  '$'
			|| sach_nr[i] ==  '/'
			|| sach_nr[i] ==  '+'
			|| sach_nr[i] ==  '%')
			{
				aus_feld[j] = sach_nr[i];
				j++;
			}
		     else
			{
			 if (sach_nr[i] == ':')
			 {
				aus_feld[j] = '-';
				j++;
			 }
			}
		   }

		i=j;
		for (; i < 25; i++)
		  {
		  aus_feld[i] = ' ';
		  }

		  aus_feld[i]='\0';

		}
	else
	{
		for (i=2; sach_nr[i]!=':'; i++)
		{
		    if (   (isalpha (c = sach_nr[i]))
			|| (isdigit (c = sach_nr[i]))
			|| sach_nr[i] ==  '-'
			|| sach_nr[i] ==  '.'
			|| sach_nr[i] ==  '$'
			|| sach_nr[i] ==  '/'
			|| sach_nr[i] ==  '+'
			|| sach_nr[i] ==  '%')
			{
			 aus_feld[i] = sach_nr[i];
			}
		     else
			{
			 aus_feld[i] = '-';
			}
		}


		i2 = i;

		for (++i2; i2 < *z_laenge; i2++)
		{
		if (sach_nr[i2] == ':')
		   {
		   aus_feld[i] = '-';

		   for (++i; sach_nr[i]!=':'; i++)
		       {
		       if (   (isalpha (c = sach_nr[i]))
			|| (isdigit (c = sach_nr[i]))
			|| sach_nr[i] ==  '-'
			|| sach_nr[i] ==  '.'
			|| sach_nr[i] ==  '$'
			|| sach_nr[i] ==  '/'
			|| sach_nr[i] ==  '+'
			|| sach_nr[i] ==  '%')
			{
			 aus_feld[i] = sach_nr[i];
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
			if ( j == 3)
			{
			aus_feld[i] = '-';
			i++;
			}

			if (auf_feld_1[j] ==  ' ')
			{
			continue;
			}
			else
			{
			 aus_feld[i] = auf_feld_1[j];
			 if (aus_feld[i] == '*')
			 {
			 aus_feld[i] = '.';
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

	     aus_feld[i] = '\0';

	}
  c=c; /* vermeidet Warnings */
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm04_normnb                                            */
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
size_t norm04_normby(norm_p1 *n02_p1,char *auf_feld_1)
#else
size_t norm04_normby(n02_p1,auf_feld_1)
norm_p1 *n02_p1;
char *auf_feld_1;
#endif
{
	   size_t ret_code=OK;
/*  Auswahl  */

    switch(sach_nr[0])
    {
	case D_KONST:
	{

	   if (auf_feld_1[1] != ':'
	       && auf_feld_1[1] != '-')
	      {
	      norm00_nb(n02_p1->normbyte,"CD");
	      }
	      break;
	} 
	case S_KONST:
	{
	   if (auf_feld_1[1] != ':'
	       && auf_feld_1[1] != '-')
	      {
	      norm00_nb(n02_p1->normbyte,"CS");
	      }
	break;	
	}
	
	case N_KONST:
	{
	   if (auf_feld_1[1] != ':'
	       && auf_feld_1[1] != '-')
	      {
	      norm00_nb(n02_p1->normbyte,"CN");
	      }
	      break;
	}
	
	case C_KONST:
	{
	   if (auf_feld_1[1] != ':'
	       && auf_feld_1[1] != '-')
	      {
	      norm00_nb(n02_p1->normbyte,"CC");
	      }
	      break;

	}
	case P_KONST:
	{
	   if (sach_nr[2] == '-'
	       && (sach_nr[1] == 'N'))
	      {
	      norm00_nb(n02_p1->normbyte,"K ");
	      }
	      break;

	}
   }
 return (ret_code);
}
