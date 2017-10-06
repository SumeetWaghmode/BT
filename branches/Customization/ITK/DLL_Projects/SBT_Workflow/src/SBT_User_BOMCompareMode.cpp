#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <fclasses/cmp.h>
#include <fclasses/cmp_tokens.h>
#include <bom/bom_tokens.h>
#include <bom/bom.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tc/iman_arguments.h>
#include <tccore/imantype.h>
#include <tccore/item.h>
#include <tccore/item_msg.h>
#include <tccore/method.h>
#include <sa/sa.h>
#include <property/nr.h>
#include <epm/epm_toolkit_tc_utils.h>
#include "sy_debug.h"
#include <bmf/libuserext_exports.h>
#include <math.h>

int defineCompareModeDescriptor(tag_t *desc) {
	int retcode = ITK_ok;
	tag_t new_element = NULLTAG;
	tag_t new_desc = NULLTAG;
	tag_t bomline_type = NULLTAG;
	tag_t ordered_elements[7];

	*desc = NULLTAG;

	ITK ( TCTYPE_find_type("BOMLine", 0, &bomline_type) );
	ITK ( CMP_create_desc("name_desc", &new_desc) );

    {
        ITK ( CMP_create_prop_element(
			  new_desc,									/* descriptor */
			  // Issue 1030. BOM Compare - Mode = SBT_bom_compare_mode leads to TC Server crash
              CMP_primary_element /*CMP_aggregate_element*/,						/* priority */
              CMP_cache_sync,							/* cache type sync'ed to prop type */
              NULL,										/* element name */
              false,									/* don't suppress value display */
              bomline_type,								/* object type (bomline)*/
              "bl_item_item_id",						/* property name */
              NULL,										/* no special display property */
              &new_element
             ) );
        /* Sequence numbers are strings that need sorting by length then value */
        ITK ( CMP_set_prop_element_order(new_element, CMP_order_by_length_and_value) );
        ordered_elements[1] = new_element;
    }

    {
        ITK ( CMP_create_prop_element(
			  new_desc,									/* descriptor */
			  CMP_aggregate_element,						/* priority */
              CMP_cache_sync,							/* cache type sync'ed to prop type */
              NULL,										/* element name */
              false,									/* don't suppress value display */
              bomline_type,								/* object type (bomline)*/
              "bl_rev_item_revision_id",				/* property name */
              NULL,										/* no special display property */
              &new_element
             ) );
        /* Sequence numbers are strings that need sorting by length then value */
        ITK ( CMP_set_prop_element_order(new_element, CMP_order_by_length_and_value) );
        ordered_elements[2] = new_element;
    }

	{
		ITK ( CMP_create_prop_element(
			new_desc,									/* descriptor */
			CMP_aggregate_element,						/* priority */
			CMP_cache_sync,								/* cache type sync'ed to prop type */
			NULL,										/* Do not define element name - will default to prop name */
			false,										/* Show value of this property in output */
			bomline_type,								/* Type to which prop belongs */
			"bl_line_name",					/* Name of property */
			NULL,										/* No display prop - use real prop instead */
			&new_element) );
		ordered_elements[3] = new_element;
	}

	{
		ITK ( CMP_create_prop_element(
			new_desc,									/* descriptor */
			CMP_primary_element /*CMP_aggregate_element*/,						/* priority */
			CMP_cache_sync,								/* cache type sync'ed to prop type */
			NULL,										/* Do not define element name - will default to prop name */
			false,										/* Show value of this property in output */
			bomline_type,								/* Type to which prop belongs */
			"bl_occ_ref_designator",					/* Name of property */
			NULL,										/* No display prop - use real prop instead */
			&new_element) );
		ordered_elements[0] = new_element;
	}

	{
		ITK ( CMP_create_prop_element(
			new_desc,									/* descriptor */
			CMP_aggregate_element,						/* priority */
			CMP_cache_sync,								/* cache type sync'ed to prop type */
			NULL,										/* Do not define element name - will default to prop name */
			false,										/* Show value of this property in output */
			bomline_type,								/* Type to which prop belongs */
			"bl_sequence_no",							/* Name of property */
			NULL,										/* No display prop - use real prop instead */
			&new_element) );
		ordered_elements[4] = new_element;
	}

	{
		ITK ( CMP_create_prop_element(
			new_desc,									/* descriptor */
			CMP_aggregate_element,						/* priority */
			CMP_cache_sync,								/* cache type sync'ed to prop type */
			NULL,										/* Do not define element name - will default to prop name */
			false,										/* Show value of this property in output */
			bomline_type,								/* Type to which prop belongs */
			"bl_uom",									/* Name of property */
			NULL,										/* No display prop - use real prop instead */
			&new_element) );
		ordered_elements[5] = new_element;
	}
	{
		ITK ( CMP_create_prop_element(
			new_desc,									/* descriptor */
			CMP_aggregate_element,						/* priority */
			CMP_cache_sync,								/* cache type sync'ed to prop type */
			NULL,										/* Do not define element name - will default to prop name */
			false,										/* Show value of this property in output */
			bomline_type,								/* Type to which prop belongs */
			"bl_quantity",								/* Name of property */
			NULL,										/* No display prop - use real prop instead */
			&new_element) );
		ordered_elements[6] = new_element;
	}

	ITK ( CMP_set_element_display_order(new_desc, 7, ordered_elements) );

	*desc = new_desc;
	return ITK_ok;
}

extern DLLAPI int SBT_Workflow_define_bom_compare_mode(int* decision, va_list args) {
	int		retcode		= ITK_ok;
	tag_t	name_desc	= NULLTAG;
	 
	if (name_desc == NULLTAG) {
		defineCompareModeDescriptor(&name_desc);
	}

	if ( BOM_compare_define_mode("SBT_bom_compare_mode", BOM_compare_multilevel, name_desc, true, false, false) != ITK_ok ) {
		printf("ERROR: Registration of custom BOM-CompareMode 'SBT_bom_compare_mode' failed! \n");
		EMH_ask_last_error(&retcode);
	} else {
		printf("Registration of custom BOM-CompareMode 'SBT_bom_compare_mode' successful \n");
	}

	return retcode;
}
}