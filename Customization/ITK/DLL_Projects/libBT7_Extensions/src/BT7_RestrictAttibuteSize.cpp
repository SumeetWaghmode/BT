#include "BT7_ServerExtensions.h"

#define BT7_SUBTITLE_1	"bt7_Subtitle_1"
#define BT7_SUBTITLE_2	"bt7_Subtitle_2"
#define BT7_MIG_VERSION	"bt7_migrated_version"

/**
    restrict user not to pass value of size not more than 50 for 
	"bt7_Subtitle_1" and "bt7_Subtitle_2" attributes

    @param METHOD_message_t*  msg
    @param va_list			  args
*/
extern USER_EXT_DLL_API int BT7_RestrictAttibuteSize(METHOD_message_t* msg, va_list args) {

	int				retcode					=	ITK_ok;
    logical			idChildIR				=	false;
	tag_t			tItemRev				=	va_arg(args,tag_t);//get the item tag from the args
	tag_t			pObjectClassIdTag		=	NULLTAG;//(O)

	printf("++++++Entering method BT7_RestrictAttibuteSize \n");
	if(DEBUG == 1) printf("################################################################ \n");
	if(DEBUG == 1) printf("#######Entering method BT7_RestrictAttibuteSize \n");
	if(DEBUG == 1) printf("################################################################ \n");	

	msg->prop_name;
	if(tItemRev != NULLTAG)
	{
		logical			idChildIR				=	false;
		tag_t			pObjectClassIdTag		=	NULLTAG;//(O)

		idChildIR = isDerivedFromClass("ItemRevision",tItemRev);
		if(idChildIR== false)
		{
			if(DEBUG == 1) printf("\n\n++++++++++++Target Revision is NOT child of ItemRevision or it descendant");
			//return retcode;
		}
		else
		{
			double	valMigratedVer	= 0;//(O)
			double	valZero			= 0;//(O)
			char*	valSubtitle_1	= NULL;//(OF)
			char*	valSubtitle_2	= NULL;//(OF)
			if(DEBUG == 1) printf("\n\n************Target Revision is CHILD of ItemRevision or it descendant");

			ITK_CALL(retcode = AOM_ask_value_double(tItemRev,BT7_MIG_VERSION,&valMigratedVer));
			
			valMigratedVer = floor( valMigratedVer * 100.00 + 0.5 ) / 100.00;// consider upto 2 decimal point
						
			if(retcode == ITK_ok && (valMigratedVer != 0.1 || valZero == valMigratedVer))
			{
				ITK_CALL(retcode = AOM_ask_value_string(tItemRev,BT7_SUBTITLE_1,&valSubtitle_1));
				if(DEBUG == 1) printf("\n************value of Subtitle 1 = {%s}",valSubtitle_1);
				if(retcode == ITK_ok)
				{
					ITK_CALL(retcode=AOM_ask_value_string(tItemRev,BT7_SUBTITLE_2,&valSubtitle_2));
					if(DEBUG == 1) printf("\n************value of Subtitle 2 = {%s}",valSubtitle_2);

					if(retcode == ITK_ok && valSubtitle_1 != NULL && valSubtitle_2!=NULL)
					{
						int lenSubtitle_1 = tc_strlen(valSubtitle_1);
						if(DEBUG == 1) printf("\n************size of Subtitle 1 = {%d}",lenSubtitle_1);

						int lenSubtitle_2 = tc_strlen(valSubtitle_2);
						if(DEBUG == 1) printf("\n\n************size of Subtitle 2 = {%d},lenSubtitle_2");

						if(lenSubtitle_1>50 || lenSubtitle_2>50)
						{
							return 919733;
						}
					}
				}
			}
			else if(retcode == ITK_ok && valMigratedVer == 0.1){
				return ITK_ok;
			}
			MEM_free(valSubtitle_1);
			MEM_free(valSubtitle_2);
		}
	}

	return ITK_ok;
}