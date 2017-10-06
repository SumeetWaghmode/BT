#ifdef __cplusplus
extern "C" {
#endif
    
#include <tc/tc.h>
#include <tcinit/tcinit.h>
#include <pom/enq/enq.h>
#include <tccore/item.h>
#include <tccore/aom_prop.h>
#include <tc/emh.h>

#define ERRTXT() \
{ \
	if(iRetcode!=ITK_ok) \
	{ \
		char*pErrorText=NULL; \
		EMH_ask_error_text(iRetcode,&pErrorText); \
		printf("%s at line %d\n",pErrorText,__LINE__); \
		MEM_free(pErrorText); \
	} \
}

static int item_find_sorted ( 
	int		* piNumItems ,		/* (O) */
	tag_t	** ppItemTags ) ; 	/* (OF) */




/** 
 * @brief
 * 
 * Reports data of all items in the database in a semicolon separated .csv file:
 *
 * > ItemRevStatusCsvReport -u=user -p=password -g=group -f=outputfilespec
 *
 * The following header rows are created:
 *
 * # Version: 1
 * # Vendor: SAXON etc.
 * # Vendor-url: ...
 * EOC=;
 * COLUMNS=item_id;object_type;revision_id;release_status;
 *
 * There is one row per item:
 *
 * item_id;object_type;revision_id;[release_status_1[,release_status_2,...]];
 *
 * The rows are sorted by item_id.
 *
 * @param[in]  argc  number of command line arguments
 * @param[in]  argv  string array of command line arguments
 */
int ITK_user_main ( 
	int		argc ,			/* (I) */                                       
    char	* argv [] )		/* (I) */                                     
{
	int		iRetcode = ITK_ok	;
	int		iNumItems = 0		;
	int		iNumRevs = 0		;
	int		iNumStatus = 0		;
	int		iIdx0 = 0			;
	int		iIdx1 = 0			;
	int		iIdx2 = 0			;

	tag_t	* pItemTags = NULL		;
	tag_t	* pRevTags = NULL		;
	tag_t	* pStatusTags = NULL	;

	char	* pcFileSpec = NULL						;
	char	cItemId [ITEM_id_size_c+1] = ""			;
	char	cItemType [ITEM_type_size_c+1] = ""		;
	char	cRevId [ITEM_id_size_c+1] = ""			;
	char	* pcStatus = NULL						;

	FILE	* pFile = NULL	;

	const char	cColSep[] = ";"		;
	const char	cStatSep[] = ","	;

	if ( argc == 1 ) {
		printf ( "\nusage: ItemRevStatusCsvReport -u=user -p=password -g=group -f=outputfilespec\n" ) ; 
		return iRetcode ;
	}
	iRetcode = ITK_init_module ( ITK_ask_cli_argument("-u=") , 
								 ITK_ask_cli_argument("-p=") , 
								 ITK_ask_cli_argument("-g=") ) ; ERRTXT()

	if ( iRetcode == ITK_ok ) {
		pcFileSpec = ITK_ask_cli_argument("-f=") ;  
	}
	if ( ( iRetcode == ITK_ok ) && ( pcFileSpec != NULL ) ) {
		pFile = fopen ( pcFileSpec , "w" ) ;  
	}
	if ( pFile == NULL ) { 
		iRetcode = !ITK_ok ;
	}
	if ( iRetcode == ITK_ok ) {
		fprintf ( pFile , "# Version: 1\n" ) ;
		fprintf ( pFile , "# Vendor: SAXON etc.\n" ) ;
		fprintf ( pFile , "# Vendor-url: ...\n" ) ;
		fprintf ( pFile , "EOC=;\n" ) ;
		fprintf ( pFile , "COLUMNS=item_id;object_type;revision_id;release_status;\n" ) ;
	}
	if ( iRetcode == ITK_ok ) {
		iRetcode = item_find_sorted ( &iNumItems , &pItemTags ) ; ERRTXT()
	}
	for ( iIdx0 = 0 ; ( iRetcode == ITK_ok ) && ( iIdx0 < iNumItems) ; iIdx0 ++ )
	{
		iRetcode =  ITEM_ask_id ( pItemTags[iIdx0] , cItemId ) ; ERRTXT()

		if ( iRetcode == ITK_ok ) {
			iRetcode = ITEM_ask_type ( pItemTags[iIdx0] , cItemType ) ; ERRTXT()
		}

		if ( iRetcode == ITK_ok ) {
			iRetcode = ITEM_list_all_revs ( pItemTags[iIdx0] , &iNumRevs , &pRevTags ) ;  ERRTXT()
		}

		for ( iIdx1 = 0 ; ( iRetcode == ITK_ok ) && ( iIdx1 < iNumRevs) ; iIdx1 ++ )
		{
			iRetcode =  ITEM_ask_rev_id ( pRevTags[iIdx1] , cRevId ) ; ERRTXT()
			fprintf ( pFile , "ROW=%s%s%s%s%s%s" , cItemId , cColSep , cItemType , cColSep , cRevId , cColSep ) ;
			if ( iRetcode == ITK_ok ) {
				iRetcode =  AOM_ask_value_tags ( pRevTags[iIdx1] , "release_status_list" , &iNumStatus , &pStatusTags ) ; ERRTXT()
			}
			if ( ( iRetcode == ITK_ok ) && ( iNumStatus > 0 ) )  {
				for ( iIdx2 = 0 ; ( iRetcode == ITK_ok ) && ( iIdx2 < iNumStatus) ; iIdx2 ++ )
				{
					iRetcode =  AOM_ask_value_string ( pStatusTags[iIdx2] , "name" , &pcStatus ) ; ERRTXT()
					if ( ( iRetcode == ITK_ok ) && ( pcStatus != NULL ) ) {
						fprintf ( pFile , "%s" , pcStatus ) ;
						if ( iIdx2 != iNumStatus - 1 ) fprintf ( pFile , "%s" , cStatSep ) ;
									              else fprintf ( pFile , "%s" , cColSep ) ;
					}
					MEM_free ( pcStatus ) ;
				}
				MEM_free ( pStatusTags ) ;
			}
			else {
				fprintf ( pFile , "%s" , cColSep ) ;
			}

            // .......... PS_where_used_precise


			fprintf ( pFile , "\n" ) ;
		}
		MEM_free ( pRevTags ) ;
	}
	MEM_free ( pItemTags ) ;

	if ( pFile != NULL ) fclose ( pFile ) ;

	iRetcode = ITK_exit_module ( TRUE ) ; ERRTXT()

	return iRetcode ;
}




/** 
 * @brief
 * 
 * Retrieves all items and returns then sorted by item_id
 *
 * @param[out]	piNumItems  number of found items
 * @param[out]  ppItemTags  tags of found items
 */
int item_find_sorted ( 
	int		* piNumItems ,		/* (O) */
	tag_t	** ppItemTags ) 	/* (OF) */
{
	int		iRetcode = ITK_ok	;
	int		iNumItems = 0		;
	int		iRowCount = 0		;
	int		iColumnCount = 0	;
	int		iIdx = 0			;

	tag_t	* pItemTags = NULL	;

	const char	cQueryId[] = "ItemQuery"		;
	const char	* pcSelectAttrs[] = { "puid" }	;

	void	*** pppQueryResult = NULL	;

	* piNumItems = iNumItems ;
	* ppItemTags = pItemTags ;

	iRetcode = POM_enquiry_create           ( cQueryId ) ; ERRTXT()
	iRetcode = POM_enquiry_add_select_attrs ( cQueryId , (char*)"Item" , 1 , pcSelectAttrs ) ; ERRTXT()
    iRetcode = POM_enquiry_add_order_attr   ( cQueryId , "Item" , "item_id" , POM_enquiry_asc_order ) ; ERRTXT()
	iRetcode = POM_enquiry_execute          ( cQueryId , &iRowCount , &iColumnCount , &pppQueryResult ) ; ERRTXT()

	if ( iRowCount > 0 )
	{ 
		iNumItems = iRowCount ;
		pItemTags = (tag_t*) MEM_alloc ( iRowCount * sizeof ( tag_t ) ) ;
		for(iIdx = 0 ; iIdx < iRowCount ; iIdx ++ )
		{	
			pItemTags[iIdx] = *((tag_t*)(pppQueryResult[iIdx][0])) ;
		}
	}

	iRetcode = POM_enquiry_delete ( cQueryId ) ; ERRTXT()
	MEM_free ( pppQueryResult ) ;

	* piNumItems = iNumItems ;
	* ppItemTags = pItemTags ;

	return iRetcode ;
}




#ifdef __cplusplus
}
#endif

