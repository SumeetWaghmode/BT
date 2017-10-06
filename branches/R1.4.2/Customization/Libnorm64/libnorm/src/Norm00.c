/* -------------------------------------------------------------------------- */
/*      Modul:  norm00                                                        */
/*                                                                            */
/*      Entry:  cnorm                                                          */
/*                                                                            */
/*      Aufgabe:        Steuermodul fuer NORM                                 */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*      Author:         Kollmansperger / Ludwig                               */
/*                      DI AP 623                                             */
/*                                                                            */
/*      Version:        1.1 vom 15.01.91                                      */
/*                      2.0 vom 20.12.99                                      */ 
/*                                                                            */  
/*                      Normbyte "A2" mit BW-Sachnummer                       */
/*                      3.0 vom 28.03.01 Androleit                            */
/*                      size_tegration einer zusätzlichen Med-Nummer             */    
/*                      akuelle Norm-Version 2,60    01.10.01  Androleit      */
/*      Version:        3.30 Sept. 2007 Nana Beseka                           */
/*                      Med beschränkt sich auf 8-stelligen Nr.und            */
/*                      18-stelligen Nr., bei der die ersten 10 Stellen mit 0 */
/*                      (Null) belegt sind.                                   */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/*      size_terne Definitionen                                                  */
/* -------------------------------------------------------------------------- */
#define OK 0
#define NOK 1

#define SSNR  2
#define FRNR  3
#define MLFB  4
#define SFN   5
#define PSTNR 6
#define TNS   7
#define EAN   8
#define VKN   9
#define SMG  10
#define VNS  11
#define MED  12
#define VERS 13
#define MSNR 14
#define BWSNR 15
#define SSU 16
#define UPC 17
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
size_t norm00_strfl(char eingabe[]);
size_t norm00_st1(norm_p1 *n00_st1_p1);
size_t norm00_feh(char *steubyte,size_t feh_wert);
size_t norm00_st2(char *steubyte);
size_t norm00_st3(char *eingfeld,char *zw_feld);
size_t norm00_st4(char *zw_feld);
size_t norm00_strfx(char ausgabe[26],char x_feld[32]);
size_t norm00_st5(char *zw_feld);
size_t norm00_st6(char *zw_feld,char *steubyte);
/* #include <ctype.h> */
/* #include <string.h> */
#else
#ifdef __BORLANDC__
size_t norm00_strfl(char eingabe[]);
size_t norm00_st1(norm_p1 *n00_st1_p1);
size_t norm00_feh(char *steubyte,size_t feh_wert);
size_t norm00_st2(char *steubyte);
size_t norm00_st3(char *eingfeld,char *zw_feld);
size_t norm00_st4(char *zw_feld);
size_t norm00_strfx(char ausgabe[26],char x_feld[32]);
size_t norm00_st5(char *zw_feld);
size_t norm00_st6(char *zw_feld,char *steubyte);
/* #include <ctype.h>
#include <string.h> */
#else
size_t norm00_strfl();
size_t norm00_st1();
size_t norm00_feh();
size_t norm00_st2();
size_t norm00_st3();
size_t norm00_st4();
size_t norm00_strfx();
size_t norm00_st5();
size_t norm00_st6();
#endif
#endif

/* -------------------------------------------------------------------------- */
/*      Funktion:       cnorm                                                 */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer NORM                                  */
/*                                                                            */
/*      Aufrufe:        norm00_st1      Pruefen Uebergabefelder               */
/*                      norm00_st2      Pruefen Steuerbyte                    */
/*                      norm00_st3      Entfernen Leerzeichen aus Eingabefeld */
/*                      norm00_st4      Umsetzen Klein- in Grossbuchstaben    */
/*                      norm00_st5      Erste Stelle Sonderzeichen            */
/*                      norm00_st6      Erkennen Nummernkreis                 */
/*                      norm00_feh      Versorgen Fehler-Kennzeichen          */
/*                      norm01          Siemens-Sachnummer                    */
/*                      norm02          Fremdsachnummer                       */
/*                      norm03          MLFB                                  */
/*                      norm04          SFN, PSTNR                            */
/*                      norm05          TNS                                   */
/*                      norm06          EAN                                   */
/*                      norm07          SNI-Verkehrsnummer  (VKN)             */
/*                      norm08          Sachnr f. milit„risches Ger„t         */
/*                      norm09          VNS                                   */
/*                      norm10          Medizinische Sachnummer               */
/*                      norm11          Versorgungsnummer (VERSNR)            */
/*                      norm12          M-Sachnummer                          */
/*                      norm13          Bundeswehr-Sachnummer                 */
/*                      norm14          Sachnummer SSU                        */
/*                      norm15          UPC                        */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */

#ifdef __STDC__
#ifdef _Windows
size_t FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2)
#else
size_t cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2)
#endif
#else
#ifdef __BORLANDC__
#ifdef _Windows
size_t FAR PASCAL cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2)
#else
size_t cnorm(norm_p1 *n00_p1,norm_p2 *n00_p2)
#endif
#else
#ifdef _Windows
size_t FAR PASCAL cnorm(n00_p1,n00_p2)
norm_p1 *n00_p1;
norm_p2 *n00_p2;
#else
size_t cnorm(n00_p1,n00_p2)
norm_p1 *n00_p1;
norm_p2 *n00_p2;
#endif
#endif
#endif
{
	size_t       ret_code = NOK;
	char            zw_feld[32];
	char            feld_NOK[3];
	size_t             i=0;
	char            hnormbyte[3];
	size_t				z_laenge;
	size_t ilength;

	/***************************************/
	/*  Fehlernummer mit NULL vorbelegen geändert auf space 16.03.00   / geändert am 22.03.00 auf NULL */
	/***************************************/

	printf("Entered cnorm function");
	for (i=0; i< 7;i++)
	{
		n00_p2->mlfb_feh[i] = '0';
	}
	n00_p2->mlfb_feh[i] = '\0';

	/***************************************/
	/*  Zwischenfeld initialisieren		   */
	/***************************************/

	for (i=0; i< 32;i++)
	{
		zw_feld[i] = '\0';
	}

	/**********************************************/
	/*  Umsetzen Steuerbyte A1 in AA 30.01.2000   */
	/**********************************************/

	if (n00_p1->steubyte[0] == 'a')
	{
		n00_p1->steubyte[0] = 'A';
	}

	if ((n00_p1->steubyte[1] == '1') && (n00_p1->steubyte[0] == 'A'))
	{
		n00_p1->steubyte[1] = 'A';
	}

	if (n00_p1->steubyte[1] == 'a')
	{
		n00_p1->steubyte[1] = 'A';
	}

	/* ------------------------- */
	/* Belegung der Uebergabefelder mit Endemarke */
	/* Parameteruebergabe aus COBOL-Programmen  */
	/* Ende wird nicht automatisch durch C erkannt */
	/* ------------------------- */

	n00_p1->eingfeld[31] = '\0';
	n00_p1->steubyte[2]  = '\0';

	for (i=0; i<41;i++)
	{
			n00_p2->zeile[i].mlfb_art[1] = '\0';
			n00_p2->zeile[i].mlfb_txt[31] = '\0';
	}

	/* ------------------------- */
	/* Loeschen Rueckgabefelder  */
	/* ------------------------- */
	 norm00_strfl(n00_p1->m_norm);
	 norm00_strfl(n00_p1->s_norm);
	 norm00_strfl(n00_p1->d_norm);
	 norm00_strfl(n00_p1->b_norm);
	 norm00_strfl(n00_p1->ma_norm);
	 norm00_strfl(n00_p1->sa_norm);


	/*  normbyte vorbelegen mit x    */
		norm00_nb(n00_p1->normbyte,NB_NR);


	/* ---------------------- */
				/* Parameter ueberpruefen */
				/* ---------------------- */
	if((ret_code = norm00_st1(n00_p1)) == NOK)
	{
	    norm00_feh(n00_p1->steubyte,N_FE);
			return(ret_code);
	}

				/* ----------------------- */
				/* Steuerbyte ueberpruefen */
				/* ----------------------- */
	if((ret_code = norm00_st2(n00_p1->steubyte)) == NOK)
	{
	    norm00_feh(n00_p1->steubyte,N_FE);
			return(ret_code);
  }

        /* --------------------- */
				/* Leerzeichen entfernen */
        /* --------------------- */
	
		  if((ret_code = norm00_st3(n00_p1->eingfeld,zw_feld)) == NOK) 
		  {
					norm00_feh(n00_p1->steubyte,N_FE); 
					return(ret_code);
		  }


				/* ---------------------------------- */
				/* Umsetzen Klein- in Grossbuchstaben */
				/* ---------------------------------- */
	if((ret_code = norm00_st4(zw_feld)) == NOK)
	{
			norm00_feh(n00_p1->steubyte,N_FE);
			/* ------------------------- */
			/* šbergabe Eingabe->ausgabe */
			/* ------------------------- */
			norm00_strfx(n00_p1->m_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->s_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->d_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->b_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->ma_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->sa_norm,n00_p1->eingfeld);

			return(ret_code);
	}

      /* -------------------------- */
      /* Erste Stelle Sonderzeichen */
			/* -------------------------- */
	if((ret_code = norm00_st5(zw_feld)) == NOK)
	{
			norm00_nb(n00_p1->normbyte,NB_NR);

			/* ------------------------- */
			/* šbergabe Eingabe->ausgabe */
			/* ------------------------- */
			norm00_strfx(n00_p1->m_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->s_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->d_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->b_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->ma_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->sa_norm,n00_p1->eingfeld);
      return(ret_code);
	}

      /* ---------------------- */
      /* Selektion Nummernkreis */
	    /* ---------------------- */
		ret_code = norm00_st6(zw_feld,n00_p1->steubyte);
		switch(ret_code)
		{
          case NOK:
				 norm00_nb(n00_p1->normbyte,NB_NR);
			     break;
		  case SSNR:
			     ret_code = norm01(n00_p1,zw_feld);
           if (n00_p1->normbyte[0] == 'X')
           {
              ret_code = norm13(n00_p1,zw_feld);
           }
           break;
		  case FRNR:
			     ret_code = norm02(n00_p1,zw_feld);
					 break;
			case MLFB:
					 ret_code = norm03(n00_p1,zw_feld,n00_p2);
					 break;
			case SFN:
					 ret_code = norm04(n00_p1,zw_feld);
					 break;
		  case PSTNR:
			     ret_code = norm04(n00_p1,zw_feld);
			     break;
		  case TNS:
			     z_laenge = strlen(zw_feld);
				 if(z_laenge <= MAXLEN1)
				 {
					ret_code = norm05(n00_p1,zw_feld);
				 }
				 else
				 {
					ret_code = norm051(n00_p1,zw_feld);
				 }
			     break;
		  case UPC:
/*BA17.12.2002  hier muß neuer aufruf rein */
			     ret_code = norm15(n00_p1,zw_feld);
			     break;
		  case EAN:
			     ret_code = norm06(n00_p1,zw_feld);
			     break;
		  case VKN:
			     ret_code = norm07(n00_p1,zw_feld);
			     break;
		  case SMG:
			     ret_code = norm08(n00_p1,zw_feld);
			     break;
		  case VNS:
			     ret_code = norm09(n00_p1,zw_feld);
           if (n00_p1->normbyte[0] == 'X')
					 {
              ret_code = norm13(n00_p1,zw_feld);
           }
			     break;
		  case MED:
			     ret_code = norm10(n00_p1,zw_feld);
           if (n00_p1->normbyte[0] == 'X')
           {
              ret_code = norm13(n00_p1,zw_feld);
           }
			     break;
		  case VERS:
			     ret_code = norm11(n00_p1,zw_feld);
			     break;
		  case MSNR:
			     ret_code = norm12(n00_p1,zw_feld);
			     break;
		  case BWSNR:
			     ret_code = norm13(n00_p1,zw_feld);
           break;
			case SSU:
			     ret_code = norm14(n00_p1,zw_feld);
			     break;
    }
	  if (ret_code == NOK)
	  {
			/* ------------------------- */
			/* šbergabe Eingabe->ausgabe */
			/* ------------------------- */
			norm00_strfx(n00_p1->m_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->s_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->d_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->b_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->ma_norm,n00_p1->eingfeld);
			norm00_strfx(n00_p1->sa_norm,n00_p1->eingfeld);
			

		  if (strlen(zw_feld) == 0)
		  {
			  feld_NOK[0] = 'X';
			  feld_NOK[1] = 'X';
				feld_NOK[2] = '\0';
			  norm00_nb(n00_p1->normbyte,feld_NOK);
		  }

    }

/* FDB auf PC Ba +  OB 05.05.2004   */
	   
	  hnormbyte[2] ='\0';
	  strcpy(hnormbyte,n00_p1->normbyte);

	  // ilength = strlen(hnormbyte)+1;

//	strcpy(dummy, n02_p1->m_norm);
	  // strcpy_s(hnormbyte, ilength, n00_p1->normbyte);


	  norm00_nb(n00_p1->normbyte,hnormbyte);
	   printf("Exit from cnorm function");

		  return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_strfl                                           */
/*                                                                            */
/*      Aufgabe:        Loeschen der Ausgabefelder                            */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:     eingabe = geloeschtes Ausgabefeld						 */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_strfl(char eingabe[])
#else
#ifdef __BORLANDC__
size_t norm00_strfl(char eingabe[])
#else
size_t norm00_strfl(eingabe)
char eingabe[];
#endif
#endif
{
	size_t       i;
	size_t       ret_code = OK;


        /* ------------------------ */
        /* Loeschen Rueckgabefelder */
        /* ------------------------ */

	for (i=0; i < 25; i++)
	{
			eingabe[i] = ' ';
  }
	eingabe[i] = '\0';

  return(ret_code);
}    
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_strfx                                           */
/*                                                                            */
/*      Aufgabe:        šbergabe Eingabe in Ausgabefelder                     */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:     ausgabefeld mit daten aus eingabefeld                  */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_strfx(char ausgabe[26],char x_feld[32])
#else
#ifdef __BORLANDC__
size_t norm00_strfx(char ausgabe[26],char x_feld[32])
#else
size_t norm00_strfx(ausgabe,x_feld)
char ausgabe[26];
char x_feld[32];
#endif
#endif
{
	size_t       i;
	size_t       i11;
  size_t       ret_code = OK;
        /* ------------------------ */
	/* šbergabe Eingabe -> Ausgabe */
	/* ------------------------ */

	for (i=0; i < 25; i++)
	{
	    if (x_feld[i] == '\0')
			{
	       break;
	    }
	    ausgabe[i] = x_feld[i];
	}
  i11=i;

	for (i=i11;i<25;i++)
	{
	  ausgabe[i] = ' ';
	}

	ausgabe[25] = '\0';

  return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_st1                                            */
/*                                                                            */
/*      Aufgabe:        Pruefung der uebergebenen Parameter                   */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_st1(norm_p1 *n00_st1_p1)
#else
#ifdef __BORLANDC__
size_t norm00_st1(norm_p1 *n00_st1_p1)
#else
size_t norm00_st1(n00_st1_p1)
norm_p1 *n00_st1_p1;
#endif
#endif
{
	size_t       ret_code = NOK;
	size_t       i;


	/* ------------------------- */
	/* Pruefen Eingabesachnummer */
	/* ------------------------- */

	for(i=0;i<32 && n00_st1_p1->eingfeld[i] != '\0';i++)
		;

	if(i == 32)
	{
		return(NOK);
	}
	else
	{
		ret_code = OK;
	}

	/* ------------------ */
	/* Pruefen Steuerbyte */
	/* ------------------ */

	for(i=0;i<3 && n00_st1_p1->steubyte[i] != '\0';i++)
		;

	if(i == 3)
	{
		return(NOK);
	}
	else
	{
		ret_code = OK;
	}

	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_st2                                            */
/*                                                                            */
/*      Aufgabe:        Pruefung des Steuerbytes                              */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_st2(char *steubyte)
#else
#ifdef __BORLANDC__
size_t norm00_st2(char *steubyte)
#else
size_t norm00_st2(steubyte)
char *steubyte;
#endif
#endif
{
	size_t       ret_code = NOK;
	char       c;

	/* ------------------ */
  /* Steuerbyte pruefen */
  /* ------------------ */

	if(isalpha(c = steubyte[0]) && islower(c = steubyte[0]))
                steubyte[0] = toupper(c = steubyte[0]);

  if(isalpha(c = steubyte[1]) && islower(c = steubyte[1]))
                steubyte[1] = toupper(c = steubyte[1]);

  switch(steubyte[0])
  {
        case ' ':
                 ret_code = OK;
								 break;

   		case 'A':
				switch(steubyte[1])
				{
					case '2':
						ret_code = OK;
						break;
					case 'A':
						ret_code = OK;
						break;
					default:
						ret_code = NOK;
						break;
                 }
								 break;
        default:
                  ret_code = NOK;
                  break;
  }
  c=c; /* vermeidet Warnings */
	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_st3                                            */
/*                                                                            */
/*      Aufgabe:        Entfernen Leerzeichen aus Eingabefeld                 */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_st3(char *eingfeld,char *zw_feld)
#else
#ifdef __BORLANDC__
size_t norm00_st3(char *eingfeld,char *zw_feld)
#else
size_t norm00_st3(eingfeld,zw_feld)
char *eingfeld;
char *zw_feld;
#endif
#endif
{
				size_t       ret_code = OK;
				size_t       i;
				size_t       k;
				char         c;

				k = 0;
 /*     ba17.12.2002  ba08.04.2003 */		  	
				
					for(i = 0;(c = eingfeld[i]) != '\0';i++)
					{
						if (eingfeld[3] !=  '.')
						{
							if(c != ' ')
								{ 
								
									zw_feld[k] = eingfeld[i];									
									k++;
								}
						}
						else
						{
							zw_feld[k] = eingfeld[i];									
							k++;
						}
					}
					zw_feld[k] = '\0';
			
				return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_st4                                            */
/*                                                                            */
/*      Aufgabe:        Umsetzen Klein- in Grossbuchstaben                    */
/*                                                                            */
/*      Aufrufe:        islower()                                             */
/*                      isalpha()                                             */
/*                      toupper()                                             */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_st4(char *zw_feld)
#else
#ifdef __BORLANDC__
size_t norm00_st4(char *zw_feld)
#else
size_t norm00_st4(zw_feld)
char *zw_feld;
#endif
#endif
{
		size_t     ret_code = OK;
        size_t     i;
		char       c;

        for(i=0;(c = zw_feld[i]) != '\0';i++)
        {
                if(islower(c) != 0 && isalpha(c) != 0)
                        zw_feld[i] = toupper(c);
				}

				return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_st5                                            */
/*                                                                            */
/*      Aufgabe:        Erste Stelle Sonderzeichen                            */
/*                                                                            */
/*      Aufrufe:        ispunct()                                             */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_st5(char *zw_feld)
#else
#ifdef __BORLANDC__
size_t norm00_st5(char *zw_feld)
#else
size_t norm00_st5(zw_feld)
char *zw_feld;
#endif
#endif
{
        size_t       ret_code = NOK;
        char       c;

        if(ispunct((c = zw_feld[0])) == 0)
												ret_code = OK;
        c=c; /* vermeidet Warnings */
				return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_st6                                            */
/*                                                                            */
/*      Aufgabe:        Selektion Nummernkreis                                */
/*                                                                            */
/*      Aufrufe:        isalpha()                                             */
/*                      isdigit()                                             */
/*                                                                            */
/*      Rueckgabe:      "1" => Ende mit Fehler                                */
/*                      "2" => Siemens Sachnummer                             */
/*                      "3" => Fremdsachnummer                                */
/*                      "4" => MLFB                                           */
/*                      "5" => SFN                                            */
/*                      "6" => PSTNR                                          */
/*                      "7" => TNS                                            */
/*                      "8" => EAN                                            */
/*                      "9" => SMG                                            */
/*                      "10"=> VNS                                            */
/*                      "11"=> MED                                            */
/*                      "12"=> Versorgungsnummer                              */
/*                      "13"=> M-Sachnummer                                   */
/*                      "14"=> Bundeswehrsachnummer                           */
/*                      "15"=> Sachnummer SSU                                 */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_st6(char *zw_feld,char *steubyte)
#else
#ifdef __BORLANDC__
size_t norm00_st6(char *zw_feld,char *steubyte)
#else
size_t norm00_st6(zw_feld,steubyte)
char *zw_feld;
char *steubyte;
#endif
#endif
{
	size_t	i,j,k;
	char	c;
	char vergl_feld[6];
	long l_vergl_feld = 0L;
	long l_feld1 = 59999L;
	long l_feld2 = 80000L;
	long l_feld3 = 91999L;
	long l_feld4 = 97000L;

/* Prüfung, ob Erzeugnis-Nummer einen zehnstelligen Nuller-Block besitzt */
/* dann prüfen, ob Med-Nummer */
/* anschließend in MED verzweigen */
/* Änderung von 13.01.99 Androleit */
  /*Änderung von Sept. 2007 NNB: Med beschränkt sich auf 18-stelligen Nr., 
   bei der die ersten 10 Stellen mit 0 (Null) belegt sind*/

 /*  if    (zw_feld[0] == '0' && zw_feld[1] == '0' && zw_feld[2] == '0' && zw_feld[3] == '0'
	   && zw_feld[4] == '0' && zw_feld[5] == '0' && zw_feld[6] == '0' && zw_feld[7] == '0'
	   && zw_feld[8] == '0' && zw_feld[9] == '0'
	   && isdigit(c = zw_feld[10])
	   && isdigit(c = zw_feld[11])
	   && isdigit(c = zw_feld[12])
	   && isdigit(c = zw_feld[13])
	   && isdigit(c = zw_feld[14])
	   && isdigit(c = zw_feld[15])
	   && isdigit(c = zw_feld[16])
	   && zw_feld[17] == '\0')
		 return(MED);*/

   if    (zw_feld[0] == '0' && zw_feld[1] == '0' && zw_feld[2] == '0' && zw_feld[3] == '0'
	   && zw_feld[4] == '0' && zw_feld[5] == '0' && zw_feld[6] == '0' && zw_feld[7] == '0'
	   && zw_feld[8] == '0' && zw_feld[9] == '0'
	   && isdigit(c = zw_feld[10])
	   && isdigit(c = zw_feld[11])
	   && isdigit(c = zw_feld[12])
	   && isdigit(c = zw_feld[13])
	   && isdigit(c = zw_feld[14])
	   && isdigit(c = zw_feld[15])
	   && isdigit(c = zw_feld[16])
	   && isdigit(c = zw_feld[17])
	   && zw_feld[18] == '\0')
		 return(MED);

  /*  char *ptr; */

  /* ----------------------------- */
  /*  SSU-SIEMENS-SACHNUMMER       */
  /*  1. Block:  U60000 bis U79999 */
  /*             U92000 bis U96999 */
  /*  Nummernstrukturen:           */
  /*  UNNNNN-J-NNNN                */
  /*  UNNNNN-JN-NNNN               */
  /*  UNNNNN-JNN-NNNN              */
  /*  UNNNNN/33-J                  */
  /*  UNNNNN/33-J-NNNN             */
  /*  UNNNNN/33-JNN                */
  /*  UNNNNN/33-JNN-NNNN           */
  /* ----------------------------- */


  for (i=0; i<5; i++)
  {
      vergl_feld[i] = zw_feld[i+1];
	}
  vergl_feld[i] = '\0';
  l_vergl_feld = atol(vergl_feld);

	if   (zw_feld[0] == 'U'
     && (((l_vergl_feld > l_feld1) && (l_vergl_feld < l_feld2))
	   ||  ((l_vergl_feld > l_feld3) && (l_vergl_feld < l_feld4))))
     {
        if ((zw_feld[6] == '-') && (zw_feld[7] == 'J') && (zw_feld[8] == '-')
         && (isdigit(c=zw_feld[9])) && (isdigit(c=zw_feld[10]))
         && (isdigit(c=zw_feld[11])) && (isdigit(c=zw_feld[12]))
         && (zw_feld[13] == '\0'))
               return(SSU);

				 if ((zw_feld[6] == '-') && (zw_feld[7] == 'J') && (isdigit(c=zw_feld[8]))
         && (zw_feld[9] == '-')
         && (isdigit(c=zw_feld[10])) && (isdigit(c=zw_feld[11]))
         && (isdigit(c=zw_feld[12])) && (isdigit(c=zw_feld[13]))
         && (zw_feld[14] == '\0'))
               return(SSU);

        if ((zw_feld[6] == '-') && (zw_feld[7] == 'J')
         && (isdigit(c=zw_feld[8])) && (isdigit(c=zw_feld[9]))
         && (zw_feld[10] == '-')
         && (isdigit(c=zw_feld[11])) && (isdigit(c=zw_feld[12]))
         && (isdigit(c=zw_feld[13])) && (isdigit(c=zw_feld[14]))
         && (zw_feld[15] == '\0'))
               return(SSU);

        if ((zw_feld[6] == '/') && (zw_feld[7] == '3') && (zw_feld[8] == '3')
         && (zw_feld[9] == '-') && (zw_feld[10] == 'J')
         && (zw_feld[11] == '\0'))
               return(SSU);

				if ((zw_feld[6] == '/') && (zw_feld[7] == '3') && (zw_feld[8] == '3')
         && (zw_feld[9] == '-') && (zw_feld[10] == 'J') && (zw_feld[11] == '-')
         && (isdigit(c=zw_feld[12])) && (isdigit(c=zw_feld[13]))
         && (isdigit(c=zw_feld[14])) && (isdigit(c=zw_feld[15]))
         && (zw_feld[16] == '\0'))
               return(SSU);


        if ((zw_feld[6] == '/') && (zw_feld[7] == '3') && (zw_feld[8] == '3')
         && (zw_feld[9] == '-') && (zw_feld[10] == 'J')
         && (isdigit(c=zw_feld[11])) && (isdigit(c=zw_feld[12]))
         && (zw_feld[13] == '\0'))
               return(SSU);

        if ((zw_feld[6] == '/') && (zw_feld[7] == '3') && (zw_feld[8] == '3')
         && (zw_feld[9] == '-') && (zw_feld[10] == 'J')
         && (isdigit(c=zw_feld[11])) && (isdigit(c=zw_feld[12]))
         && (zw_feld[13] == '-')
         && (isdigit(c=zw_feld[14])) && (isdigit(c=zw_feld[15]))
         && (isdigit(c=zw_feld[16])) && (isdigit(c=zw_feld[17]))
				 && (zw_feld[18] == '\0'))
               return(SSU);

    }


	/* ----------------------------- */
	/*  M-SIEMENS-SACHNUMMER         */
	/*  'MNN' oder 'M*' oder 'M.'    */
	/* ----------------------------- */
	if   ((zw_feld[0] == 'M'
		 && isdigit(c = zw_feld[1])
	   && isdigit(c = zw_feld[2]))

      || (zw_feld[0] == 'M'
			&& (zw_feld[1] == '*'
      ||  zw_feld[1] == '.')))

      return(MSNR);


	/* ----------------------------- */
	/*  SIEMENS-SACHNUMMER           */
	/*  'ANN' oder 'A*' oder 'A.'    */
	/* ----------------------------- */
    /*  20.12.93, mg 'AN*'           */
	/* ----------------------------- */
	if ((isalpha(c = zw_feld[0])
	   && isdigit(c = zw_feld[1])
	   && (isdigit(c = zw_feld[2]) || zw_feld[2] == '*'))

      || (isalpha(c = zw_feld[0])
      && (zw_feld[1] == '*'
      ||  zw_feld[1] == '.'))

			|| ((zw_feld[0] == 'D'
			||  zw_feld[0] == 'N'
			||  zw_feld[0] == 'B'
			||  zw_feld[0] == 'U')
	    && isdigit(c = zw_feld[1])))
			return(SSNR);

/* ----------------------------------------------- */
/*  FREMD-SACHNUMMER                               */
/*  'AAA:' oder 'AAA-'                             */
/*  Änderung: Juni 2001 Mindestbelegung: 5 Stellen */
/*  5. Stelle : 'A' oder 'N'                       */  
/* ----------------------------------------------- */

/* ba16.12.2002 */
/* ba12.01.2004  RFC Nummer 3 Stelle nummerisch*/

	if   (isalpha(c = zw_feld[0])
		 && isalpha(c = zw_feld[1])
	   && (isalpha(c = zw_feld[2])
	    ||  zw_feld[2] == '2'
		||  zw_feld[2] == '3'
		||  zw_feld[2] == '4'
		||  zw_feld[2] == '5'
		||  zw_feld[2] == '6'
		||  zw_feld[2] == '7'
		||  zw_feld[2] == '8'
		||  zw_feld[2] == '9')		
	   && (zw_feld[3] == ':'
	   ||  zw_feld[3] == '.'
	   ||  zw_feld[3] == '-')
 	   && (zw_feld[4] != '\0'))

	   return(FRNR);

/* Änderung: 01.02.99 auf AAAA: oder A/N A/N A/N : */
/*
	if   (isalpha(c = zw_feld[0])
		 && isalpha(c = zw_feld[1])
		 && isalpha(c = zw_feld[2])
	   && isalpha(c = zw_feld[3])
	   && (zw_feld[4] == ':'
	   ||  zw_feld[4] == '-'))
                return(FRNR);

	if   ((isalpha(c = zw_feld[0]) || isdigit(c = zw_feld[0]))
		 && (isalpha(c = zw_feld[1]) || isdigit(c = zw_feld[1]))
	   && (isalpha(c = zw_feld[2]) || isdigit(c = zw_feld[2]))
	   && (zw_feld[3] == ':'
	   ||  zw_feld[3] == '-'))
                return(FRNR);
*/

/* -------------------- */
/*  MLFB                */
/*  'NAAN'              */
/* -------------------- */
	if(isdigit(c = zw_feld[0]) && isalpha(c = zw_feld[1]) &&
					 isalpha(c = zw_feld[2]) &&
					 isdigit(c = zw_feld[3]))
								return(MLFB);

/* -------------------- */
/*  Fabrikate-Nummer    */
/*  'A:' oder 'A-'      */
/* -------------------- */
	if   (isalpha(c = zw_feld[0])
		 && (zw_feld[1] ==':' 
	   ||  zw_feld[1] == '-'))
		return(SFN);

/* -------------------- */
/*  Produktstammnummer  */
/*  'PN:' oder 'PN-'    */
/* -------------------- */
	if  ((isalpha(c = zw_feld[0])
		 && isalpha(c = zw_feld[1]))
		 && (zw_feld[2] == '-'
		 ||  zw_feld[2] == ':'))
	   return(PSTNR);

/* --------------------------------------------------------- */
/*  Technisches Nummernsystem                                */
/*  'ANA'                                                    */
/*  Änderung: zusätzliche 4. Stelle Mindestbelegung 05.06.01 */
/*  4. Stelle:  'A' oder 'N' erlaubt                         */
/* --------------------------------------------------------- */
       if (isalpha(c = zw_feld[0])
		 && isdigit(c = zw_feld[1])
		 && isalpha(c = zw_feld[2])
		 && (isalpha(c = zw_feld[3])
	     ||  isdigit(c = zw_feld[3])))

	   return(TNS); 

/* --------------------   */
/*  EAN-Nummer 13 Stellen */
/*  'NNNNNNNNNNNNN'       */
/* --------------------   */
       if    (isdigit(c = zw_feld[0])
	   && isdigit(c = zw_feld[1])
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
	   && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
	   && isdigit(c = zw_feld[7])
	   && isdigit(c = zw_feld[8])
	   && isdigit(c = zw_feld[9])
	   && isdigit(c = zw_feld[10])
	   && isdigit(c = zw_feld[11])
		 && isdigit(c = zw_feld[12]))
		 return(EAN);

/* --------------------   */
/*  UPC-Nummer 12 Stellen */
/*  'NNNNNNNNNNNN'        */
/*  'ba17.12.2002         */
/*  'angepasste EAN-Nummer*/
/* --------------------   */
       if    (isdigit(c = zw_feld[0])
	   && isdigit(c = zw_feld[1])
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
	   && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
	   && isdigit(c = zw_feld[7])
	   && isdigit(c = zw_feld[8])
	   && isdigit(c = zw_feld[9])
	   && isdigit(c = zw_feld[10])
	   && isdigit(c = zw_feld[11])) 

		 return(UPC);

/* --------------------   */
/*  VKN-Nummer 13 Stellen */
/*  'NNNNN.NN.N.NN'       */
/* --------------------   */
       if    (isdigit(c = zw_feld[0])
	   && isdigit(c = zw_feld[1])
	   && isdigit(c = zw_feld[2])
	   && isdigit(c = zw_feld[3])
	   && isdigit(c = zw_feld[4])
	   && (zw_feld[5] == '.')
	   && isdigit(c = zw_feld[6])
	   && isdigit(c = zw_feld[7])
	   && (zw_feld[8] == '.')
	   && isdigit(c = zw_feld[9])
	   && (zw_feld[10] == '.')
	   && isdigit(c = zw_feld[11])
	   && isdigit(c = zw_feld[12]))
		 return(VKN);
/* --------------------   */
/*  VERSNR mer 16 Stellen */
/*  'NNNN-NN-NNN-NNNN'    */
/* --------------------   */
	 if (isdigit(c = zw_feld[0])
		 && isdigit(c = zw_feld[1])
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && (zw_feld[4] == '-')
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && (zw_feld[7] == '-')
		 && isdigit(c = zw_feld[8])
		 && isdigit(c = zw_feld[9])
		 && isdigit(c = zw_feld[10])
		 && (zw_feld[11] == '-')
		 && isdigit(c = zw_feld[12])
		 && isdigit(c = zw_feld[13])
		 && isdigit(c = zw_feld[14])
		 && isdigit(c = zw_feld[15]))

		 return(VERS);




	/* -----------------------------------                  */
	/*  Sachnummer fr milit„risches Ger„t                  */
	/*  'NNA:'                                              */
	/* -----------------------------------                  */

	 if (isdigit(c = zw_feld[0])
		 && isdigit(c = zw_feld[1])
		 && isalpha(c = zw_feld[2])
		 && zw_feld[3] != '\0')
		 return(SMG);
	

	/* ----------------------------------- */
	/*  VNS-Vertriebliches Nummernsystem   */
	/*  'AAANNNN' oder 'AAANNNN-'          */
	/* ----------------------------------- */
	if  (isalpha(c = zw_feld[0])
		&& isalpha(c = zw_feld[1])
		&& isalpha(c = zw_feld[2])
		&& isalnum(c = zw_feld[3])
		&& isalnum(c = zw_feld[4])
		&& isalnum(c = zw_feld[5])
		&& isalnum(c = zw_feld[6])
		&& (zw_feld[7] == '-' || zw_feld[7] == '\0'))
		{
		if ((zw_feld[0] == 'A') &&
			 ((zw_feld[1] == 'A') ||
			 (zw_feld[1] == 'B') ||
			 (zw_feld[1] == 'C') ||
			 (zw_feld[1] == 'D') ||
			 (zw_feld[1] == 'E') ||
			 (zw_feld[1] == 'F') ||
			 (zw_feld[1] == 'G') ||
			 (zw_feld[1] == 'H') ||
			 (zw_feld[1] == 'J') ||
			 (zw_feld[1] == 'K')))
			{
				return(VNS);
			}
		}

	/* --------------------------*/
	/*  MED-Nummer min 8 Stellen */
	/*  'NNNNNNNN'               */
	/* --------------------------*/
		if (isdigit(c = zw_feld[0])
		 && isdigit(c = zw_feld[1])
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && isdigit(c = zw_feld[7])
		 && zw_feld[8] == '\0')
		 return(MED);

/* Änderung von Sept. 2007 NNB, MED beschränkt sich auf 8-stelligen Nr. */
	/* --------------------------*/
	/*  MED-Nummer min 7 Stellen */
	/*  'NNNNNNNN'               */
	/* --------------------------*/
	/*	if (isdigit(c = zw_feld[0])
		 && isdigit(c = zw_feld[1])
		 && isdigit(c = zw_feld[2])
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && zw_feld[7] == '\0')
		 return(MED);

		/*********************/


	/* --------------------------*/
	/*  MED-Nummer 8 Stellen  und Stelle 1- 3 Wert: 082 */
	/*  '082NNNNN'               */
	/* --------------------------*/
/*		if ((zw_feld[0] == '0')
		 && (zw_feld[1] == '8')
		 && (zw_feld[2] == '2')
		 && isdigit(c = zw_feld[3])
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && isdigit(c = zw_feld[6])
		 && isdigit(c = zw_feld[7])
		 && zw_feld[8] == '\0')
		 return(MED);

/* --------------------------*/
	/*  MED-Nummer druckaufbereitet 10 Stellen mit 2 Leerzeichen */
	/*  '082NNNNN'               */
	/* --------------------------*/
		if ((zw_feld[0] == ' ')
		 && isdigit(c = zw_feld[1])
 		 && isdigit(c = zw_feld[2])
		 && (zw_feld[3] == ' ')
		 && isdigit(c = zw_feld[4])
		 && isdigit(c = zw_feld[5])
		 && (zw_feld[6] == ' ')
		 && isdigit(c = zw_feld[7])
		 && isdigit(c = zw_feld[8])
		 && isdigit(c = zw_feld[9])
		 && zw_feld[10] == '\0')
		{
/* Zusammenschieben der Sachnummer */
			zw_feld[0] = '0';
			zw_feld[1] = zw_feld[1];
			zw_feld[2] = zw_feld[2];
			zw_feld[3] = zw_feld[4];
			zw_feld[4] = zw_feld[5];
			zw_feld[5] = zw_feld[7];
			zw_feld[6] = zw_feld[8];
			zw_feld[7] = zw_feld[9];
			zw_feld[8] = '\0';
			zw_feld[9] = '\0';
			zw_feld[10] = '\0';
    	    return(MED);
		}


	/* ----------------------------- */
	/*  BW-Sachnummer                */
	/*  'ANNNN bis AAANNNNNNN        */
	/* ----------------------------- */
	/* Aenderung fr Landesgesellschaft 21.11.95 */
    /* Steuerbyte A1 keine BW-Sachnummerverarbeitung                          */ 
	/* Änderung fuer DASA USH 20.12.99  Steuerbyte A2 BW-Sachnummer ansteuern */
    /* BW-Sachnummer-Verarbeitung nur bei Steuerbyte "A2"                     */
    

	 if (steubyte[1] == '2') 
	 {
	 for (i=0;((i < 3) && (isalpha(c = zw_feld[i])))  ; i++)
	 {
	 }
	 j = i;
	 k = 0;
	 for (;(j < 11) && (isdigit(c = zw_feld[j])); j++,k++)
	 {
	 }
	 if ((zw_feld[j] == '-' ||
			 zw_feld[j] == '\0') &&
			 k > 3)
	 {
					 return(BWSNR);
	 }
	 }

    c=c;/* vermeidet Warnings*/

	return(NOK);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_feh                                            */
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
size_t norm00_feh(char *steubyte,size_t feh_wert)
#else
#ifdef __BORLANDC__
size_t norm00_feh(char *steubyte,size_t feh_wert)
#else
size_t norm00_feh(steubyte, feh_wert)
char *steubyte;
size_t feh_wert;
#endif
#endif
{
  size_t ret=OK;

        steubyte[0] = (char)feh_wert;
        steubyte[1] = ' ';
        steubyte[2] = '\0';

	return(ret);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm00_nb                                             */
/*                                                                            */
/*      Aufgabe:        Belegen Normbyte                                      */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm00_nb(char normbyte[],char nb_wert[])
#else
/*Ba06.09.2004 HP-UX Compiler kann einrücken nicht*/
#ifdef __BORLANDC__
size_t norm00_nb(char normbyte[],char nb_wert[])
#else
size_t norm00_nb(normbyte,nb_wert)
char normbyte[];
char nb_wert[];
#endif
#endif
{
  size_t ret=OK;
	normbyte[0] = nb_wert[0];
	normbyte[1] = nb_wert[1];
	normbyte[2] = '\0';

	return(ret);
}
