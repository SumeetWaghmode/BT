/* -------------------------------------------------------------------------- */
/*      Modul:  norm051                                                       */
/*                                                                            */
/*      Entry:  norm051                                                       */
/*                                                                            */
/*      Aufgabe:        Technisches Nummernsystem, Max. Länge = 18            */
/*                                                                            */
/*      Author:			Njomo Nana Beseka                                     */
/*                                                                            */
/*      Erstellung:		04.02.2008                                            */
/*																			  */
/*      Aenderungen:                                                          */
/*																			  */
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

size_t norm051_fp(char *zw_feld,size_t *z_laenge);
size_t norm051_auf(char zw_feld[],char block1[],char block3[]);
size_t norm051_af1(norm_p1 *n02_p1,char *zw_feld,char *block1,char *block3,size_t *z_laenge);
size_t norm051_afm(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm051_afma(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm051_afd(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm051_afs(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm051_afsa(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm051_afb(char zw_feld[],char aus_feld[],char block1[],char block3[]);
size_t norm051_normby(norm_p1 *n02_p1);
#else
size_t norm051_fp();
size_t norm051_auf();
size_t norm051_af1();
size_t norm051_afm();
size_t norm051_afma();
size_t norm051_afd();
size_t norm051_afs();
size_t norm051_afsa();
size_t norm051_afb();
size_t norm051_normby();
#endif

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051                                                */
/*                                                                            */
/*      Aufgabe:        MAIN-Entry fuer Aufbereitungsmodul TNS-Nummer         */
/*                                                                            */
/*      Aufrufe:        norm051_fp       Fehlerpruefung                        */
/*                      norm051_auf      Aufbereiten der Erzeugnisnummer       */
/*                      norm051_af1      Auswahl nach Steuerbyte               */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051(norm_p1 *n02_p1,char *zw_feld)
#else
size_t norm051(n02_p1,zw_feld)
norm_p1 *n02_p1;
char *zw_feld;
#endif
{
	char            zblock1_1[13];
	char            *block1 = zblock1_1;
	char            zblock3_1[4];
	char            *block3=zblock3_1;
	size_t       ret_code = NOK;
	size_t 	          z_laenge   =  0;

	/* Fehlerpruefung: */
	if((ret_code = norm051_fp(zw_feld,&z_laenge)) == NOK)
	{
		 norm00_nb(n02_p1->normbyte,NB_NR);
		 return(ret_code);
	}
	/* Aufbereiten der Erzeugnisnummer: */
	if((ret_code = norm051_auf(zw_feld,block1,block3)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}
	/* Auswahl nach Steuerbyte: */
	if((ret_code = norm051_af1(n02_p1,zw_feld,block1,block3,&z_laenge)) == NOK)
	{
		norm00_nb(n02_p1->normbyte,NB_NR);
		return(ret_code);
	}
	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051_fp                                             */
/*                                                                            */
/*      Aufgabe:        Fehlerpruefung                                        */
/*                                                                             */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_fp(char *zw_feld,size_t *z_laenge)
#else
size_t norm051_fp(zw_feld,z_laenge)
char *zw_feld;
size_t *z_laenge;
#endif
{
	size_t ret_code=NOK;
	size_t i, c;
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
	if  (*z_laenge > MAXLEN2)
		ret_code = NOK;
	else
		ret_code = OK;

	/* TNR-Kopf darf nicht 'I' oder 'O' enthalten: */
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

		c=zw_feld[i];

		if ( (a = (size_t)isalpha((int)c))!=0 || (d = (size_t)isdigit((int)c))!=0 || zw_feld[i] == '-' )
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
size_t norm051_auf(char zw_feld[],char block1[],char block3[])
#else
size_t norm051_auf(zw_feld, block1, block3)
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

	/* Block 1 verdichten (numerisch, max. 12 Byte): */
	k = 0;
	for (i = 3; zw_feld[i] != '\0' && i < 15; i++)
	{
		 if (zw_feld[i] != ' ')
		 {
			 if (isdigit(c=zw_feld[i]))
			 {
				block1[k] = zw_feld[i];
				k++;
				if (k == 12)
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
	for (; k<13;k++)
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
		} */		/* Umwandlung von I in J wird abgelehnt */
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
/*      Funktion:       norm051_af1                                            */
/*                                                                            */
/*      Aufgabe:        Auswahl nach Steuerbyte                               */
/*                                                                            */
/*      Aufrufe:        norm051_afm      Aufbereitung MASCHINELL alt und neu   */
/*                      norm051_afd      Aufbereitung DRUCK                    */
/*                      norm051_afs      Aufbereitung SORTIERT alt und neu     */
/*                      norm051_afb      Aufbereitung BARCODE                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_af1(norm_p1 *n02_p1,char *zw_feld,char *block1,char *block3,size_t *z_laenge)
#else
size_t norm051_af1(n02_p1,zw_feld,block1,block3,z_laenge)
norm_p1 *n02_p1;
char *zw_feld;
char *block1;
char *block3;
size_t *z_laenge;
#endif
{
			size_t       ret_code = NOK;

			/* Auswahl */

					norm051_afm(zw_feld,n02_p1->m_norm,block1,block3);
					norm051_afd(zw_feld,n02_p1->d_norm,block1,block3);
					norm051_afs(zw_feld,n02_p1->s_norm,block1,block3);
					norm051_afb(zw_feld,n02_p1->b_norm,block1,block3);
					norm051_afma(zw_feld,n02_p1->ma_norm,block1,block3);
					norm051_afsa(zw_feld,n02_p1->sa_norm,block1,block3);
					ret_code = OK;

	norm051_normby(n02_p1);
	z_laenge=z_laenge;

return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051_afm                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung MASCHINELL                               */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_afm(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm051_afm(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,a,zaehl, a3;
	char	c;

		a=strlen(block1);
		a3=strlen(block3);

		i=0;
		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		if(a3 == 0)
		{
			for (k = 17; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		else
		{
			for (k = 14; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		

		k=0;

		if(a3 > 0)
		{
			if ((a3 == 2) && (block3[0] == '-'))
			{
				aus_feld[17] = block3[1];
				aus_feld[16] = ' ';
				aus_feld[15] = block3[0];
			}
			else
			{
				for (i=15 ;i<18;i++)
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
			i=17;
			for (k = 17; k > 2; k--)
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


		for (i=18;i<25;i++)
		{
			aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);
}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051_afma                                           */
/*                                                                            */
/*      Aufgabe:        Aufbereitung MASCHINELL                               */
/*                      2. Form                                               */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_afma(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm051_afma(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i, k, a, zaehl, a3;
	char	c;

		a=strlen(block1);
		a3=strlen(block3);

		i=0;
		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		if(a3 == 0)
		{
			for (k = 17; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		else
		{
			for (k = 14; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}


		k=0;

		if(a3 > 0)
		{
			if ((a3 == 2) && (isdigit(c=block3[1])))
			{
				aus_feld[17] = block3[1];
				aus_feld[16] = ' ';
				aus_feld[15] = block3[0];
			}
			else
			{
				for (i=15 ;i<18;i++)
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
		 i = 17;
		 for (k = 17; k > 2; k--)
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

		for (i=18;i<25;i++)
		{
			aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);

}

/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051_afd                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung DRUCK                                    */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_afd(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm051_afd(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t       ret_code = OK;
	size_t       k=0;
	size_t       i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}
		 k=0;
		 for (i=3; (i<15) && (block1[k] != '\0');i++,k++)
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
/*      Funktion:       norm051_afs                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SORTIERT                                 */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_afs(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm051_afs(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,a,zaehl, a3;
	char	c;

		a=strlen(block1);
		a3=strlen(block3);
		i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		if(a3 == 0)
		{
			for (k = 17; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		else
		{
			for (k = 14; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		

		k=0;

		if(a3 > 0)
		{
			if ((a3 == 2) && (block3[0] == '-'))
			{
				aus_feld[17] = block3[1];
				aus_feld[16] = ' ';
				aus_feld[15] = block3[0];
			}
			else
			{
				for (i=15 ;i<18;i++)
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
		 i=17;
		 for (k = 17; k > 2; k--)
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


		for (i=18;i<25;i++)
		{
			 aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051_afsa                                           */
/*                                                                            */
/*      Aufgabe:        Aufbereitung SORTIERT                                 */
/*                      2.Form                                                */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_afsa(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm051_afsa(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t	ret_code = OK;
	size_t	i,k,a,zaehl, a3;
	char	c;

		a=strlen(block1);
		a3=strlen(block3);
		i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}

		i=a;
		i--;
		if(a3 == 0)
		{
			for (k = 17; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		else
		{
			for (k = 14; k > 2; k--)
			{
				if (i >= 0)
					aus_feld[k] = block1[i];
				else
					aus_feld[k] = ' ';
					i--;
			}
		}
		
		k=0;
		if(a3 > 0)
		{
			if ((a3 == 2) && (isdigit(c=block3[1])))
			{
				aus_feld[17] = block3[1];
				aus_feld[16] = ' ';
				aus_feld[15] = block3[0];
			}
			else
			{
				for (i=15 ;i<18;i++)
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
		 i = 17;
		 for (k = 17; k > 2; k--)
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


		for (i = 18; i<25; i++)
		{
			 aus_feld[i] = ' ';
		}
		aus_feld[25] = '\0';

	c=c;

	return(ret_code);
}
/* -------------------------------------------------------------------------- */
/*      Funktion:       norm051_afb                                            */
/*                                                                            */
/*      Aufgabe:        Aufbereitung BARCODE                                  */
/*                                                                            */
/*      Aufrufe:                                                              */
/*                                                                            */
/*      Rueckgabe:                                                            */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_afb(char zw_feld[],char aus_feld[],char block1[],char block3[])
#else
size_t norm051_afb(zw_feld, aus_feld, block1, block3)
char zw_feld[];
char aus_feld[];
char block1[];
char block3[];
#endif
{
	size_t       ret_code = OK;
	size_t       k=0;
	size_t       i=0;

		for (i=0; i<3;i++)
		{
		 aus_feld[i] = zw_feld[i];
		}
		 k=0;
		 for (i = 3; (i < 15) && (block1[k] != '\0');i++,k++)
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
/*      Funktion:       norm051_normnb                                         */
/*                                                                            */
/*      Aufgabe:        Normbyte-Belegung                                     */
/*                                                                            */
/*      Aufrufe:        kein                                                  */
/*                                                                            */
/*      Rueckgabe:      "0" => Ende ohne Fehler                               */
/*                      "1" => Ende mit Fehler                                */
/* -------------------------------------------------------------------------- */
#ifdef __STDC__
size_t norm051_normby(norm_p1 *n02_p1)
#else
size_t norm051_normby(n02_p1)
norm_p1 *n02_p1;
#endif
{
	size_t	ret_code=OK;
	size_t	i,anz_digit;
	char	c;
	char dummy[26];

//	strcpy(dummy,n02_p1->m_norm);
	size_t length = strlen(dummy)+1;

	strcpy(dummy, n02_p1->m_norm);
	  // strcpy_s(dummy, length, n02_p1->m_norm);

/* Normbyte uebergeben: */
	/* TE: */	
	if ((isdigit(c=dummy[15])) || dummy[15]==' ')
		norm00_nb(n02_p1->normbyte,"TA");	
	else
		/* TZ: */
		if (dummy[15]=='-')
			norm00_nb(n02_p1->normbyte,"TS");
		else
		{
			anz_digit=0;
			for (i=15;i<18;i++)
			{
				if (isdigit(c=dummy[i]))
				{
					anz_digit++;
				}
			}
			/* TU: */
			if (anz_digit==0)
				norm00_nb(n02_p1->normbyte,"TD");	
			/* TP: */
			else
				norm00_nb(n02_p1->normbyte,"TM");	
		}

	c=c;
	return (ret_code);
}
