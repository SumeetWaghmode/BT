/* -------------------------------------------------------------------------- */
/*      Modul:  norm05                                                        */
/*                                                                            */
/*      Entry:  norm05                                                        */
/*                                                                            */
/*      Aufgabe:        Technisches Nummernsystem                             */
/*                                                                            */
/*      Author:                                                               */
/*                      SNI AP 621                                            */
/*                                                                            */
/*      Aenderungen:                                                          */
/*      V1.0  11/91			Ursprungsversion                                  */
/*      V1.1  03/96			Erweiterung Zustandangabe                         */
/*      V2.0	01/97			Fehlerbehebung Normbyte TZ                    */
/*      V3.0	04.02.98  Erweiterung Normbyte TP fuer Bereich PL             */
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
/*  Prototypen */
/* #include <ctype.h>
#include <string.h> */

size_t norm05_fp(char *zw_feld,size_t *z_laenge);
size_t norm05_auf(char zw_feld[],char block1[],char block3[]);
size_t norm05_af1(norm_p1 *n02_p1,char *zw_feld,char *block1,char *block3,size_t *z_laenge);
size_t norm05_afm(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm05_afma(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm05_afd(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm05_afs(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm05_afsa(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm05_afb(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm05_normby(norm_p1 *n02_p1);
#else
size_t norm05_fp();
size_t norm05_auf();
size_t norm05_af1();
size_t norm05_afm();
size_t norm05_afma();
size_t norm05_afd();
size_t norm05_afs();
size_t norm05_afsa();
size_t norm05_afb();
size_t norm05_normby();
#endif

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul TNS-Nummer         */
/*                                                                            */
/*      Aufrufe:        norm05_fp       Fehlerpruefung                        */
/*                      norm05_auf      Aufbereiten der Erzeugnisnummer       */
/*                      norm05_af1      Auswahl nach Steuerbyte               */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05(norm_p1 *n02_p1,char *zw_feld)
#else
size_t norm05(n02_p1,zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            zblock1_1[9];
	char            *block1=zblock1_1;
	char            zblock3_1[4];
	char            *block3=zblock3_1;
	size_t       ret_code = NOK;
	size_t 	          z_laenge   =  0;

	/* Fehlerpruefung: */
	if((ret_code = norm05_fp(zw_feld,&z_laenge)) == NOK)
	{
		 norm00_nb(n02_p1->normbyte,NB_NR);
		 return(ret_code);
	}
	/* Aufbereiten der Erzeugnisnummer: */
	if((ret_code = norm05_auf(zw_feld,block1,block3)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}
	/* Auswahl nach Steuerbyte: */
	if((ret_code = norm05_af1(n02_p1,zw_feld,block1,block3,&z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_fp                                             */
/*                                                                            */
/*      Aufgabe:        Fehlerpruefung                                        */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm05_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	size_t ret_code=NOK;
	size_t i;
	char	c;
	size_t a=0;
	size_t d=0;

	*z_laenge = strlen(zw_feld);

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
 



	/* Gesamtlaenge max. 14 Byte: */
	if  (*z_laenge > 14)
		ret_code = NOK;
	else
		ret_code = OK;

	/* TNR-Kopf darf nicht 'I' oder 'O' enthalten: */
/*	for (i=0;i<3;i++)*/
	for (i=0;i<*z_laenge;i++)
		if ((zw_feld[i] == 'I') || (zw_feld[i] == 'O'))
		 ret_code = NOK;

	/* TNR-Rumpf */
	for (i=0;((i<= *z_laenge) && (zw_feld[i] != '\0'));i++)
	{
		/* '-' erfordert nachfolgende Zustandsangabe: */
		if ((zw_feld[i] == '-') && ( (i + 1) == *z_laenge))
		{
		 ret_code = NOK;
		}
		/* Umwandeln 'O' in '0': */
	/*	if (zw_feld[i] == 'O') 
		{
			zw_feld[i] = '0';
		}	*/		/* 20.02.2008 NNB, die Umwandlung gilt nicht mehr */
		/* Nur Alpha-Zeichen, numerische Zeichen und '-' zugelassen: */
		c=zw_feld[i];

		if ( (a=isalpha(c))!=0 || (d=isdigit(c))!=0 || zw_feld[i] == '-' )
		{
	
		}
		else
		{
			ret_code = NOK;
		}
	}
	 c=c;
	 a=a;
	 d=d;
	 return (ret_code);
}

/*----------------------------------------------*/
/* Aufbereiten der Erzeugnisnummer              */
/*----------------------------------------------*/
#ifdef __STDC__
size_t norm05_auf(char zw_feld[],char block1[],char block3[])
#else
size_t norm05_auf(zw_feld, block1, block3)
char zw_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,len_block3,anz_digit,anz_alpha,anz_null;
	char	c;

	for (i=0;i<4;i++)
	{
		block3[i] = '\0';
	}

	/* Block 1 verdichten (numerisch, max. 8 Byte): */
	k = 0;
	for (i = 3; zw_feld[i] != '\0' && i < 11 ; i++)
	{
		 if (zw_feld[i] != ' ')
		 {
			 if (isdigit(c=zw_feld[i]))
			 {
				block1[k] = zw_feld[i];
				k++;
				if (k == 8)
				{
					block1[k] = '\0';
					i++;
					break;
				}
			 }
			 else
			 {
				 block1[k] = '\0';
				 break;
			 }
		 }
	}
	for (; k<9;k++)
	{
		 block1[k]= '\0';
	}

	/* Block 3 aufbereiten: */
	k = 0;
	anz_null=0;

	for (; zw_feld[i] != '\0'; i++)
	{
		/* max. 3 Byte lang: */
		if (k > 2)
		{
			ret_code = NOK;
			return(ret_code);
		}
		/* 'I' wird in 'J' umgewandelt;  */
		block3[k] = zw_feld[i];
	/*	if (block3[k] == 'I')
		{
			block3[k] = 'J';
		}*/		/* 2.02.2008 NNB: Umwandlung ist nicht mehr zulaessig */
		if (block3[k] == '0')
		{
			anz_null++;
		}
		k++;
	}
	block3[k] = '\0';


	/* Ermitteln von Entscheidungskriterien in Block 3: */
	anz_digit=0;
	anz_alpha=0;
	for (i=0;i<3;i++)
	{
		if (isdigit(c=block3[i]))
			anz_digit++;
		if (isalpha(c=block3[i]))
			anz_alpha++;
	}
	len_block3=strlen(block3);


	/* Identifizierung und Plausibilitaeten: */
	/* TE: */
	if (isdigit(c=block3[0]) || block3[0]=='\x00')
	{
		/* nur Ziffern erlaubt */
		if (anz_digit < len_block3) 
			ret_code = NOK;
	}
	else
	{
		/* TZ: */
		if (block3[0]=='-')
		{
			/* Behandlung von Nullen: */
			if (anz_null != 0)
			{
				/* '-0' oder '-00' wird geloescht: */
				if  ( (len_block3 == 2) ||
							((block3[1] == '0') && (block3[2] == '0')) )
				{
					block3[0] = ' ';
					block3[1] = ' ';
					block3[2] = ' ';
				}
				else
				/* Zustandsangabe zweistellig mit einer Null: */
				{
					/* '-0A' (Fuehrende Null vor Alphazeichen wird eliminiert): */
					if (block3[1] == '0' && (isalpha(c=block3[2])))
					{
						block3[1] = block3[2];
						block3[2] = '\0';
					}
					else
					{
						/* '-A0' (Nachfolgende Null wird eliminiert): */
						if (isalpha(c=block3[1]))
							block3[2] = '\0';
					}
				}
			}
			else
			/* keine Nullen: */
			{
				/* '-N' wird mit 0 aufgefuellt: */
				if (len_block3 == 2 && anz_digit == 1)
				{
					block3[2] = block3[1];
					block3[1] = '0';
				}
			}
		}
		else
			/* TU: */
			if (anz_alpha==len_block3)
				;
			else
				/* TP: */
				/* Behandlung von Nullen: */
				if (anz_null != 0)
				{
					if (anz_digit==2)
					{
						/* 'A00' nicht erlaubt */
						if (block3[1]=='0' && block3[2]=='0')
							ret_code = NOK;
					}
					else
					{
					/* 'A0A' und 'AA0' nicht erlaubt */
						for (i=1;i<3;i++)
							if (block3[i]=='0')
								ret_code = NOK;
					}
				}
				else
					/* Auffuellen ('AN'  -> 'A0N')*/
					if (block3[2]=='\0')
					{
						block3[2] = block3[1];
						block3[1] = '0';
						block3[3] = '\0';
					}
	}
	c=c;
	len_block3=len_block3;
	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm05_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm05_afd      Aufbereitung DRUCK                    */
/*                      norm05_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm05_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_af1(norm_p1 *n02_p1,char *zw_feld,char *block1,char *block3,size_t *z_laenge)
#else
size_t norm05_af1(n02_p1,zw_feld,block1,block3,z_laenge)
norm_p1 *n02_p1;
char *zw_feld;
char *block1;
char *block3;
size_t *z_laenge;
#endif
{
			size_t	ret_code = NOK;

			/* Auswahl */

					norm05_afm(zw_feld,n02_p1->m_norm,block1,block3);
					norm05_afd(zw_feld,n02_p1->d_norm,block1,block3);
					norm05_afs(zw_feld,n02_p1->s_norm,block1,block3);
					norm05_afb(zw_feld,n02_p1->b_norm,block1,block3);
					norm05_afma(zw_feld,n02_p1->ma_norm,block1,block3);
					norm05_afsa(zw_feld,n02_p1->sa_norm,block1,block3);
					ret_code = OK;

	norm05_normby(n02_p1);
	z_laenge=z_laenge;

return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_afm                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung MASCHINELL                               */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_afm(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm05_afm(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t  i,k,a,zaehl;
	char	c;

		a=strlen(block1);

		i=0;
		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		for (k = 10; k > 2; k--)
			{
			if (i >= 0)
				aus_feld[k] = block1[i];
			else
				aus_feld[k] = ' ';
				i--;
			}

		k=0;

		a=strlen(block3);
		if ((a == 2) && (block3[0] == '-'))
		{
			aus_feld[13] = block3[1];
			aus_feld[12] = ' ';
			aus_feld[11] = block3[0];
		}
		else
		{
		for (i=11 ;i<14;i++)
		{
			if (block3[k] != '\0')
				{
				aus_feld[i] = block3[k];
				k++;
				}
			else
				{
				aus_feld[i] = ' ';
				}
		 }
		 }
	 zaehl=0;
	 if (block3[0] != '-')
	 {
		for (i=0;i < 3;i++)
		{
			if (isdigit(c=block3[i]))
				{
					zaehl++;
				}
		}
		}
		if (zaehl > 0)
		 {
		 i=13;
		 for (k = 13; k > 2; k--)
			{
			if (aus_feld[k] != ' ')
			 {
				aus_feld[i] = aus_feld[k];
				i--;
			 }
			}
		 for (;i>2;i--)
		 {
				aus_feld[i] = ' ';
		 }
		}


		for (i=14;i<25;i++)
		{
			aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_afma                                           */
/*                                                                            */
/*      Aufgabe:        Aufbereitung MASCHINELL                               */
/*                      2. Form                                               */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_afma(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm05_afma(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,a,zaehl;
	char	c;

		a=strlen(block1);

		i=0;
		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		for (k = 10; k > 2; k--)
			{
			if (i >= 0)
				aus_feld[k] = block1[i];
			else
				aus_feld[k] = ' ';
				i--;
			}

		k=0;

		a=strlen(block3);
		if ((a == 2) && (isdigit(c=block3[1])))
		{
			aus_feld[13] = block3[1];
			aus_feld[12] = ' ';
			aus_feld[11] = block3[0];
		}
		else
		{
		for (i=11 ;i<14;i++)
		{
			if (block3[k] != '\0')
				{
				aus_feld[i] = block3[k];
				k++;
				}
			else
				{
				aus_feld[i] = ' ';
				}
		 }
		}

		zaehl=0;
	 if (block3[0] != '-')
	 {
		for (i=0;i < 3;i++)
		{
			if (isdigit(c=block3[i]))
				{
					zaehl++;
				}
		}
		}
		if (zaehl > 0)
		 {
		 i=13;
		 for (k = 13; k > 2; k--)
			{
			if (aus_feld[k] != ' ')
			 {
				aus_feld[i] = aus_feld[k];
				i--;
			 }
			}
		 for (;i>2;i--)
		 {
				aus_feld[i] = ' ';
		 }
		}

		for (i=14;i<25;i++)
		{
			aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_afd                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung DRUCK                                    */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_afd(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm05_afd(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t  k=0;
	size_t  i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}
		 k=0;
		 for (i=3; (i<11) && (block1[k] != '\0');i++,k++)
		 {
				aus_feld[i] = block1[k];
		 }

		 for (k=0; block3[k] != '\0';k++)
		 {
				aus_feld[i] = block3[k];
				i++;
		 }

		 for (;i<25;i++)
		 {
				aus_feld[i] = ' ';
		 }
		 aus_feld[25] = '\0';


return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_afs                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SORTIERT                                 */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_afs(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm05_afs(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,a,zaehl;
	char	c;

		a = strlen(block1);
		i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		for (k = 10; k > 2; k--)
			{
			 if (i >= 0)
				aus_feld[k] = block1[i];
			else
					aus_feld[k] = ' ';
					i--;
			}

		k=0;
		a=strlen(block3);
		if ((a == 2) && (block3[0] == '-'))
		{
			aus_feld[13] = block3[1];
			aus_feld[12] = ' ';
			aus_feld[11] = block3[0];
		}
		else
		{
		for (i=11 ;i<14;i++)
		{
			if (block3[k] != '\0')
				 {
				 aus_feld[i] = block3[k];
				 k++;
				 }
			 else
				 {
				 aus_feld[i] = ' ';
				 }
		 }
		}

		zaehl=0;
		if (block3[0] != '-')
		{
		for (i=0;i < 3;i++)
		{
			if (isdigit(c=block3[i]))
				{
					zaehl++;
				}
		}
		}
		if (zaehl > 0)
		 {
		 i=13;
		 for (k = 13; k > 2; k--)
			{
			if (aus_feld[k] != ' ')
			 {
				aus_feld[i] = aus_feld[k];
				i--;
			 }
			}
		 for (;i>2;i--)
		 {
				aus_feld[i] = ' ';
		 }
		}


		for (i=14;i<25;i++)
		{
			 aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_afsa                                           */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SORTIERT                                 */
/*                      2.Form                                                */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_afsa(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm05_afsa(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,a,zaehl;
	char	c;

		a = strlen(block1);
		i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		for (k = 10; k > 2; k--)
			{
			 if (i >= 0)
				aus_feld[k] = block1[i];
			else
					aus_feld[k] = ' ';
					i--;
			}

		k=0;

		a=strlen(block3);
		if ((a == 2) && (isdigit(c=block3[1])))
		{
			aus_feld[13] = block3[1];
			aus_feld[12] = ' ';
			aus_feld[11] = block3[0];
		}
		else
		{
		for (i=11 ;i<14;i++)
		{
			if (block3[k] != '\0')
			{
				 aus_feld[i] = block3[k];
				 k++;
			}
			else
			{
				 aus_feld[i] = ' ';
			}
		 }
		}

		zaehl=0;
		if (block3[0] != '-')
		{
		for (i=0;i < 3;i++)
		{
			if (isdigit(c=block3[i]))
				{
					zaehl++;
				}
		}
		}
		if (zaehl > 0)
		 {
		 i=13;
		 for (k = 13; k > 2; k--)
			{
			if (aus_feld[k] != ' ')
			 {
				aus_feld[i] = aus_feld[k];
				i--;
			 }
			}
		 for (;i>2;i--)
		 {
				aus_feld[i] = ' ';
		 }
		}


		for (i=14;i<25;i++)
		{
			 aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_afb                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung BARCODE                                  */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_afb(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm05_afb(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	k=0;
	size_t	i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}
		 k=0;
		 for (i=3; (i<11) && (block1[k] != '\0');i++,k++)
		 {
				aus_feld[i] = block1[k];
		 }

		 for (k=0; block3[k] != '\0';k++)
		 {
				aus_feld[i] = block3[k];
				i++;
		 }

		 for (;i<25;i++)
		 {
				aus_feld[i] = ' ';
		 }
		 aus_feld[25] = '\0';

	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm05_normnb                                         */
/*                                                                            */
/*      Aufgabe:        Normbyte-Belegung                                     */
/*                                                                            */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm05_normby(norm_p1 *n02_p1)
#else
size_t norm05_normby(n02_p1)
norm_p1 *n02_p1;
#endif
{
	size_t	ret_code=OK;
	size_t	i,anz_digit;
	char	c;
	char dummy[26];

	size_t length = strlen(dummy)+1;

	strcpy(dummy, n02_p1->m_norm);
	// strcpy_s(dummy, length, n02_p1->m_norm);

/* Normbyte uebergeben: */
	/* TE: */	
	if ((isdigit(c=dummy[11])) || dummy[11]==' ')
		norm00_nb(n02_p1->normbyte,"TE");	
	else
		/* TZ: */
		if (dummy[11]=='-')
			norm00_nb(n02_p1->normbyte,"TZ");
		else
		{
			anz_digit=0;
			for (i=11;i<14;i++)
			{
				if (isdigit(c=dummy[i]))
				{
					anz_digit++;
				}
			}
			/* TU: */
			if (anz_digit==0)
				norm00_nb(n02_p1->normbyte,"TU");	
			/* TP: */
			else
				norm00_nb(n02_p1->normbyte,"TP");	
		}

	c=c;
	return (ret_code);
}
