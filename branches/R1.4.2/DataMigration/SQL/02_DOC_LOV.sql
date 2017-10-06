-- first execute this
UPDATE /*+PARALLEL(16)*/ PBT7_DOCUMENTREVISION SET PBT7_DOCUMENTCLASS = DECODE (PBT7_DOCUMENTCLASS, 
           'Development', 'dclass_dev',
           'Manufacturing','dclass_manu',
           'Usage and Disposal','dclass_UaD',
           'Management and Process', 'dclass_MaP',
           PBT7_DOCUMENTCLASS);
		   
commit;

-- second execute this
UPDATE /*+PARALLEL(16)*/ PBT7_DOCUMENTREVISION SET pbt7_documentsubclass = DECODE (pbt7_documentsubclass, 
           'Condition', 'dsclass_Con_manu',
           'Specification','dsclass_Spec_manu',
           'Verification','dsclass_Ver_manu',
           'Project and Process', 'dsclass_PaP_manu',
                      'Condition', 'dsclass_Con_dev',
                      'Specification', 'dsclass_Spec_dev',
                      'Verification', 'dsclass_Ver_dev',
                      'Project and Process', 'dsclass_PaP_dev',
                      'Condition', 'dsclass_Con_UaD',
                      'Specification', 'dsclass_Spec_UaD',
                      'Verification', 'dsclass_Ver_UaD',
                      'Project and Process', 'dsclass_PaP_UaD',
                      'Condition', 'dsclass_Con_MaP',
                      'Specification', 'dsclass_Spec_MaP',
                      'Verification', 'dsclass_Ver_MaP',
                      'Project and Process', 'dsclass_PaP_MaP',                      
           pbt7_documentsubclass);

commit;

-- third execute this		   
UPDATE /*+PARALLEL(16)*/ PBT7_DOCUMENTREVISION SET pbt7_documentsntype = DECODE (pbt7_documentsntype, 
           'Directives and standards', 'dtype_07',
           'Education and training','dtype_09',
           'Information','dtype_13',
           'Others', 'dtype_22',
           'Procedure instructions', 'dtype_24',
           'Report', 'dtype_25',
           'Technical specification', 'dtype_27',
           'Template', 'dtype_28',     
           'Calculation and dimensioning', 'dtype_02',   
           'Design Drawing', 'dtype_06',   
           'Inspection and test report', 'dtype_16',   
           'Material declaration', 'dtype_20',      
           'Data sheet', 'dtype_04',       
           'Inspection and test document', 'dtype_15',     
           'Document regarding changes', 'dtype_08',      
           'Inquiry, calculation and offer document', 'dtype_14',   
           'Order and delivery document', 'dtype_21',   
           'Assembly drawing', 'dtype_01',   
           'Circuit diagram', 'dtype_03',   
           'Firmware', 'dtype_11',   
           'Marking', 'dtype_19',   
           'Parts list', 'dtype_23',   
           'Technical delivery conditions', 'dtype_26',   
           'First article inspection report', 'dtype_12',
           'Declaration of conformity', 'dtype_05',
           'Inspection document', 'dtype_17',
           'Environment-related information', 'dtype_10',                      
           'Instructions for use', 'dtype_18',  
		   'Design drawing', 'dtype_06',
		   'Inquiry@ calculation and offer document','dtype_14',		   
           pbt7_documentsntype) ;

commit;		   