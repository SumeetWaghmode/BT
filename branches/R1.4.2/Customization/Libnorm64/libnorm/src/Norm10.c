/* -------------------------------------------------------------------------- */
/*      Modul:  norm10							*/
/*                                                                            */
/*      Entry:  norm10							*/
/*                                                                            */
/*      Aufgabe:        Medizinische Sachnummer  (MED)                        */
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
/*      Version:        1.0 vom 17.08.92                                      */
/*      ƒnderung            am   28.03.01 Androleit                           */
/*                      fuer Firma Acuson wurde eine neue Med-Nummer eingef¸gt*/
/*                      1. Block: 082 2, Block 5-stellig numerisch            */
/*                                              keine Pr¸fung                 */ 
/*      ƒnderung        V.2.3.0 Sept. 2007 NNB: Pruefziffernermittlung wurde  */
/*                      auskommentiert.                                       */
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
size_t norm10_fp(char *zw_feld,size_t *z_laenge);
size_t norm10_auf(char *zw_feld,char *block1);
size_t norm10_af1(norm_p1 *n02_p1,char *block1,size_t *z_laenge);
size_t norm10_afm(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm10_afd(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm10_afs(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm10_afb(char block1[],char aus_feld[],size_t *z_laenge);
size_t norm10_normby(norm_p1 *n02_p1);

/* #include <ctype.h>
#include <string.h>
#include <stdlib.h> */
#else
size_t norm10_fp();
size_t norm10_auf();
size_t norm10_af1();
size_t norm10_afm();
size_t norm10_afd();
size_t norm10_afs();
size_t norm10_afb();
size_t norm10_normby();
#endif
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10						*/
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul                    */
/*                      MED-Nummer                                            */
/*                                                                            */
/*      Aufrufe:        norm10_fp       Fehlerpruefung                        */
/*                      norm10_all      Allgemeine Verarbeitung               */
/*                                      TNS mit ANA                           */
/*                      norm10_af1      Auswahl nach Steuerbyte               */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm10(norm_p1 *n02_p1, char *zw_feld)
#else
size_t norm10(n02_p1, zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            zblock1_1[26];
	char            *block1=zblock1_1;
	size_t       ret_code = NOK;
	size_t             laenge = 0;
	size_t 	          *z_laenge   =  &laenge;

/* Falls 10 f¸hrende Nullen mitgegeben werden, m¸ssen diese f¸r die weitere  */
	/* Verarbeitung abgeschnitten werden  */
	/* ƒnderung vom 14.01.2000 Androleit   */

    size_t j=0;
	size_t i=0;

if    (zw_feld[0] == '0' && zw_feld[1] == '0' && zw_feld[2] == '0' && zw_feld[3] == '0'
	&& zw_feld[4] == '0' && zw_feld[5] == '0' && zw_feld[6] == '0' && zw_feld[7] == '0'
	&& zw_feld[8] == '0' && zw_feld[9] == '0')
{

	for (i = 0; (zw_feld[i] != '\0'); i++)
     {
		if (i > 9)
		{
       		zw_feld[j]=zw_feld[i];
      		j++;
		}
	 }
    zw_feld[j] = '\0';
}


	if((ret_code = norm10_fp(zw_feld,z_laenge)) == NOK)
        {
                norm00_nb(n02_p1->normbyte,NB_NR);
                return(ret_code);
        }
	if((ret_code = norm10_auf(zw_feld,block1)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
                return(ret_code);
        }
	if((ret_code = norm10_af1(n02_p1,block1,z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
                return(ret_code);
        }

      return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10_fp                                             */
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
size_t norm10_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm10_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	   size_t	ret_code=NOK;
	   size_t	i,b,k;
	   char		c;
	 /*  size_t ab;*/
	   size_t	a[8];                    /* arno  war vorher auf 8  */
	   size_t	diff=0;
	   size_t	zwi_summe=0;

	 /*  size_t       zwi_sum1;*/

/* 9.12.93, mg, core auf  mx300 bei nr.  5801218  */
/*	 char      feld0[17];                         */

       char      feld0[19];
	   char      *feld0_1=feld0;
	   char      vergleich[2];
	   char      *vergleich1=vergleich;

	   b = strlen(zw_feld);
	   *z_laenge = b;

	   if  (*z_laenge > 8)
	      {
	       ret_code = NOK;
	      }
	     else
	     {
	      ret_code = OK;
	     }
	   if (*z_laenge == 8)
	    {
	   if (isdigit(c=zw_feld[7])
	      && (zw_feld[0] == '0')) 
	   {
	   }
	   else
	   {
	   ret_code=NOK;
	   }
	   }
/* f¸r Firma Acuson keine Pr¸fung  Stand: Mai 2001 */

	   	if ((zw_feld[0] == '0')
		 && (zw_feld[1] == '8')
		 && (zw_feld[2] == '2')
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && isdigit(c = zw_feld[7])
		 && zw_feld[8] == '\0')
		{
		 ret_code=OK;
		 return (ret_code);
		}
/* f¸r Firma Acuson keine Pr¸fung  Stand: Mai 2001 */

	  	if ((zw_feld[0] == '8')
		 && (zw_feld[1] == '2')
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && zw_feld[7] == '\0'
		 && zw_feld[8] == '\0')
		{
		 ret_code=OK;
		 return (ret_code);
		}		
/* f¸r MED keine Pr¸fung  Stand: 06.2003  BA24.06.2003 */

	   	if (isdigit(c = zw_feld[0])
		 && isdigit(c = zw_feld[1])
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && isdigit(c = zw_feld[7])
		 && zw_feld[8] == '\0')
		{
				if 	(zw_feld[0] != '0')
				{
				 
					
				 zw_feld[8] = 'X';
				 zw_feld[9] = '\0';
					
				 ret_code=OK;
				 return (ret_code);
				}
		}
/*  PrÅfung Modula11-Verfahren, ob OK */

	   k=0;
/* FÅr die Umwandlung eines 8-stelligen Stringes werden die Werte aus
   dem ‹bergabebereich in einen 16-stelligen Bereich mit einem '\0' zwischen
   jedem numerischen Zeichen gestellt. Dies wird f¸r die Umwandlung von String
   in einen size_teger-Wert benˆtigt. */

	   for (i=0;i<9;i++)
     {
       	feld0_1[k]=zw_feld[i];
      	k++;
		    feld0_1[k]='\0';
		    k++;
	   }
	   k=2;

/* hier erfolgt die Belegung der Zeiger fÅr atoi
   atoi nimmt den Wert bis zum '\0'-Zeichen */
/* Ba07.09.2004 i auf 8 vorher 9 gesetzt da bei 00209704 abbruch in FDB*/
/*Ba10.01.2005 wieder auf 9 da probleme mit med nr mit 10 nullen */

/*if 	(zw_feld[0] == '0' && zw_feld[1] == '0')
				{ */
	   for (i=0;i<8;i++)
	    {
	      a[i] = atoi(feld0_1);
	      *feld0_1=feld0[k];
	      k=k+2;
	    }        
/*} else
{
		for (i=0;i<9;i++)
	    {
	      a[i] = atoi(feld0_1);
	      *feld0_1=feld0[k];
	      k=k+2;
		}
}*/

/* Aenderung Sept. 2007 NNB: Pruefziffernermittlung soll raus*/
 /*     if (*z_laenge == 8)
      {  
/* beginn for-schleife */

 /*     for (i=0;i<7;i++)
       {
         zwi_sum1 = a[i]*2;
	       if (zwi_sum1 > 9)
	       {
	        switch (zwi_sum1)
	        {
	          case 10:
	               a[i] = 1;
	               break;
	          case 12:
	               a[i] = 3;
	               break;
	          case 14:
	               a[i] = 5;
	               break;
	          case 16:
	               a[i] = 7;
	               break;
	          case 18:
	               a[i] = 9;
	               break;
	        }
	      }
	      else
	      {
	        a[i] = zwi_sum1;
	      }
	      i++;
      }
/*  Ende for-schleife */
/*
       zwi_summe = a[6]+a[5]+a[4]+a[3]+a[2]+a[1]+a[0];
       *vergleich=zw_feld[7];
    }
    else
    {
/* beginn for-schleife */
/*
    for (i=1;i<6;i++)
    {
	    zwi_sum1 = a[i]*2;
	    if (zwi_sum1 > 9)
	    {
	     switch (zwi_sum1)
	     {
	     case 10:
	      a[i] = 1;
	      break;
	     case 12:
	      a[i] = 3;
	      break;
	     case 14:
	      a[i] = 5;
	      break;
	     case 16:
	      a[i] = 7;
	      break;
	     case 18:
	      a[i] = 9;
	      break;
	     }
	    }
	    else
	    {
	      a[i] = zwi_sum1;
	 }
	 i++;
 } 
/* ende for-schleife  */
 /*  zwi_summe = a[5]+a[4]+a[3]+a[2]+a[1]+a[0];
   *vergleich=zw_feld[6];
}

  /*     i=0;
       diff=((zwi_summe+10)/10)*10-zwi_summe;
       if (diff == 10)
       {
        	diff = 0;
       }

       vergleich[1] = '\0';
       ab = atoi(vergleich);
       if (diff == ab)
	     {
	        ret_code = OK;
	     }
	     else
	     {
	      if ((diff > 5) || (diff == 5))
	      {
	        diff=diff-5;
	        if (diff == ab)
	        {
	          ret_code = OK;
	        }
	        else
	        {
	          ret_code = NOK;
	        }
	      }
	      else
	      {
	      if (diff < 5)
	      {
	        diff=diff+5;
	        if (diff == ab)
	        {
	          ret_code = OK;
	        }
	        else
	        {
	          ret_code = NOK;
	        }
	      }
	      else
	      {
	        if (diff == ab)
	        {
	          ret_code = OK;
	        }
	        else
	        {
	          ret_code = NOK;
	        }
	      }
	     }
	  }
   vergleich1=vergleich1;c=c; /* vermeidet Warnings */
/*------ Ende von Pr¸fziffernermittlung -----*/
	 return (ret_code);
}
/*----------------------------------------------*/
/* Aufbereiten der Nummer                   */
/*                                              */
/*----------------------------------------------*/
#ifdef __STDC__
size_t norm10_auf(char *zw_feld,char *block1)
#else
size_t norm10_auf(zw_feld,block1)
char *zw_feld;
char *block1;
#endif
{
     size_t     ret_code = OK;
     size_t i,j,b;

	   b = strlen(zw_feld);


	 for (i=0;i<25; i++)
	 {
	 block1[i] = ' ';
	 }
	 block1[i] = '\0';
	i=0;
	j=0;

/*ba09.12.2004    ba10.01.2005 */	
	if  (b == 7)
  {
    block1[j] = '0';
    j=1;
  }

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
/*      Funktion:       norm10_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm10_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm10_afd      Aufbereitung DRUCK                    */
/*                      norm10_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm10_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm10_af1(norm_p1 *n02_p1,char *block1,size_t *z_laenge)
#else
size_t norm10_af1(n02_p1,block1,z_laenge)
norm_p1 *n02_p1;
char *block1;
size_t *z_laenge;
#endif
{
        size_t       ret_code = NOK;

        
		/* Auswahl */
		/* BA25.06.2003 Reihenfolge wegen neuer mednr ge‰ndert */
					/*

					*/
					norm10_afd(block1,n02_p1->d_norm,z_laenge);
					norm10_afm(block1,n02_p1->m_norm,z_laenge);
					norm10_afs(block1,n02_p1->s_norm,z_laenge);
					
					norm10_afb(block1,n02_p1->b_norm,z_laenge);
					norm10_afm(block1,n02_p1->ma_norm,z_laenge);
					norm10_afm(block1,n02_p1->sa_norm,z_laenge);
					
					
					
				
					
					
					
					
					
					ret_code = OK;

				

	norm10_normby(n02_p1);

        return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10_afm                                            */
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
size_t norm10_afm(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm10_afm(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;

#endif
{
	size_t       ret_code = OK;
	size_t       i;				

	
	for (i = 0; i < 24; i++)		 
	{
	/*Ba10.01.2005 */
		
					if (block1[i] == '\0') {
					block1[i] = ' ';
					}  
			aus_feld[i] = block1[i];
	  }
	
		
	aus_feld[24]=' ';
	aus_feld[25]='\0';
	/*block1[24] = '\0';	*/
		/*ba09.12.2004  */
					if (aus_feld[8] == '\0') {
						aus_feld[8] = ' ';
					}

			

	    
	
     z_laenge=z_laenge;
	
	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10_afd                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung DRUCK                                    */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/* B24.06.2003                                                                           */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm10_afd(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm10_afd(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
  size_t       j=0;


    aus_feld[i] = ' ';
    j=0;
	if (block1[8] == 'X')
      {
		/*ba1.01.2005 */
		block1[8] = ' ';
		block1[9] = ' ';
		/*aus_feld[0] = ' ';*/
		for (i = 0; i < 25; i++)
			  {
				  if ((i==3) || (i==6))
				  {
				  /*ba24.06.2003   aus_feld[i] = ' ';   */
						   aus_feld[i] = block1[j];
					  j++;
				  }
				  else
				  {
						  aus_feld[i] = block1[j];
					  j++;
				  }
			}
			for (;i<25;i++)
			{
			  aus_feld[i] = ' ';
			}
				aus_feld[24]=' ';
				aus_feld[25]='\0';
			z_laenge=z_laenge;
	}
	else
	{
			/*ba10.01.2005 j auf 0 vorher auf 1 bei 10 f¸hrenden Nullen Problem */
			/* 20.02.2008 NNB: Die Leerzeichen sind nicht mehr erwuenscht */
		j=0;
			i=0;
			aus_feld[0] = ' ';
		/*	if (block1[0] == '0')
				{
				j=1;
				}*/
			for (i = 0; (i < 25); i++)
			{
				aus_feld[i] = block1[j];
					j++; 
			/*	if ((i==3) || (i==6))
				  {
				  
				  aus_feld[i] = ' ';  
						
					
				  }
				  else
				  {
					if (block1[j] == '\0') {
					block1[j] = ' ';
					}	  
					  aus_feld[i] = block1[j];
					j++;  
				  }	 */ 
			}
			for (;i<24;i++)
			{
			  aus_feld[i] = ' ';
			}
			aus_feld[24]=' ';
			aus_feld[25]='\0';
			/*aus_feld[25]='\0';*/
			/*block1[24]='\0';*/
		z_laenge=z_laenge;
	}







	


return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10_afs                                            */
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
size_t norm10_afs(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm10_afs(block1, aus_feld, z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;
 i= 0;
		for (i = 0; i < 24; i++)
		  {
		    if (block1[i] == '\0') {
					block1[i] = ' ';
					}  
			aus_feld[i] = block1[i];
		 }
		aus_feld[0] = block1[0];
		aus_feld[24]=' ';
		aus_feld[25]='\0';
		/*block1[20]= '\0';*/ 
       z_laenge=z_laenge;

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10_afb                                            */
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
size_t norm10_afb(char block1[],char aus_feld[],size_t *z_laenge)
#else
size_t norm10_afb(block1, aus_feld,z_laenge)
char block1[];
char aus_feld[];
size_t *z_laenge;
#endif
{
	size_t       ret_code = OK;
	size_t       i=0;

		for (i = 0; i < 25; i++)
		  {
		      aus_feld[i] = block1[i];
		   }
		   aus_feld[24]=' ';
			aus_feld[25]='\0';
		   	/*ba14.12.2004*/
					if (aus_feld[8] == '\0') {
					aus_feld[8] = ' ';
					}

       z_laenge=z_laenge;
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm10_normnb                                            */
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
size_t norm10_normby(norm_p1 *n02_p1)
#else
size_t norm10_normby(n02_p1)
norm_p1 *n02_p1;
#endif
{
	   size_t ret_code=OK;

				norm00_nb(n02_p1->normbyte,"M ");

	 return (ret_code);
}
