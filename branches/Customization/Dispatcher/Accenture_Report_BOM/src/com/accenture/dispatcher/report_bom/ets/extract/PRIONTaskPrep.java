/*==============================================================================
Copyright 2009.
Siemens Product Lifecycle Management Software Inc.
All Rights Reserved.
================================================================================
File description: Custom TaskPrep class for proetojt translator. This sub class
prepares a proetojt translation task. This is a configuration
specified class based on provider name and translator name in
DispatcherClient property file which creates the proe specific
translation request by preparing the data for translation and
creating the Translation request object.

Filename:   TaskPrep.java
================================================================================*/
//==== Package  ================================================================
package com.accenture.dispatcher.report_bom.ets.extract;

//==== Imports  ================================================================
import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.accenture.dispatcher.report_bom.ets.util.PRIONConfigManager;
import com.accenture.dispatcher.report_bom.ets.util.PRIONDSPDatasetBean;
import com.teamcenter.ets.extract.DefaultTaskPrep;
import com.teamcenter.ets.request.TranslationRequest;
import com.teamcenter.ets.util.Registry;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.Dataset;
import com.teamcenter.soa.client.model.strong.ImanFile;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.translationservice.task.TranslationTask;

//==== Class ====================================================================
/**
 * Custom TaskPrep class for proetojt translator. This sub class prepares a proetojt translation task. This is a configuration specified class based
 * on provider name and translator name in DispatcherClient property file which creates the proe specific translation request by preparing the data
 * for translation and creating the Translation request object.
 */
public class PRIONTaskPrep extends DefaultTaskPrep {

	@SuppressWarnings("rawtypes")
	private Vector PRIONDSPDatasetBean_vector = null;
	// This registry is needed for:
	// PRIONTaskPrep.sanitize.BadChars
	// PRIONTaskPrep.sanitize.replacementChar
	//
	private static Registry registry = Registry.getRegistry("com.accenture.dispatcher.report_bom.tstk.translator.priontranslator");
	private String m_objectType = "";
	private Dataset m_FinalDataset = null;

	private String ReportType = "";
	private String RevisionRule = "";
	private String ReportFormat = "";
	private String UserName = "";
	private String MaxBOMLevel = "";
	private String EqualPart = "";
	private String Ordering = "";
	private String IncludePCB = "";
	private String IncludeSUP = "";
	private String OnlySMD = "";
	private String ReportFilename = "";

	/**
	 * The ETS Extractor service invokes the prepareTask() class method to prepare input data for translation requests. This method does the following
	 * Validates that all data necessary to translate the primary object(s) is available. Primary object(s) that are data incomplete are generally
	 * excluded from the translator input with appropriate logging. Collects all input files and creates TranslationTask object
	 * 
	 * @see com.teamcenter.ets.extract.TaskPrep#prepareTask()
	 * 
	 * @return Returns the Translation Task object.
	 */
	@SuppressWarnings("rawtypes")
	@Override
	public TranslationTask prepareTask() throws Exception {

		// Get Mapping-Arguments and add them to a TXT File
		// Changed for upgrade
		String translatorArgsKeys[] = request.getPropertyObject(TranslationRequest.TRANSLATION_ARGS_KEYS).getStringArrayValue();
		String translatorArgsData[] = request.getPropertyObject(TranslationRequest.TRANSLATION_ARGS_DATA).getStringArrayValue();
		for (int i = 0; i < translatorArgsKeys.length; i++) {
			if (translatorArgsKeys[i].equalsIgnoreCase("ReportType")) {
				ReportType = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("RevisionRule")) {
				RevisionRule = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("ReportFormat")) {
				ReportFormat = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("UserName")) {
				UserName = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("MaxBOMLevel")) {
				MaxBOMLevel = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("EqualPart")) {
				EqualPart = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("Ordering")) {
				Ordering = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("IncludePCB")) {
				IncludePCB = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("IncludeSUP")) {
				IncludeSUP = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("OnlySMD")) {
				OnlySMD = translatorArgsData[i];
			} else if (translatorArgsKeys[i].equalsIgnoreCase("Filename")) {
				ReportFilename = translatorArgsData[i];
			} else {
				//
			}
		}
		// Changed for upgrade
		PRIONDSPDatasetBean_vector = new Vector();
		PRIONConfigManager.RetrieveTransaltorConfig(request.getPropertyObject(TranslationRequest.TRANSLATOR_NAME).getDisplayableValue(),
				PRIONDSPDatasetBean_vector, true, m_zTaskLogger);
		
		TranslationTask zTransTask = new TranslationTask();
		// Changed for upgrade
		m_zTaskLogger.debug("PRIONTaskPrep -> TRANSLATOR (lowercase): "
				+ request.getPropertyObject(TranslationRequest.TRANSLATOR_NAME).getDisplayableValue().toLowerCase());

		PRIONDSPDatasetBean datasetBean = new PRIONDSPDatasetBean();

		// If we don't want to translate a dataset, but we want to execute a
		// command parsing the ItemID and Revision as arguments
		// we must define the first Dataset type as NONE
		if (!((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0)).getDataSetType().toLowerCase().equals("none")) {
			// Changed for upgrade
			// get primary objects and secondary objects
			ModelObject primary_objs[] = request.getPropertyObject(TranslationRequest.PRIMARY_OBJS).getModelObjectArrayValue();
			ModelObject secondary_objs[] = request.getPropertyObject(TranslationRequest.SECONDARY_OBJS).getModelObjectArrayValue();

			for (int i = 0; i < primary_objs.length; i++) {

				// TranslationRequest PRIMARY Object must be a dataset
				Dataset transPrimaryObject_dataset = (Dataset) primary_objs[i];

				// TranslationRequest secondary object must be an ItemRevision
				ItemRevision transSecondaryObject_itemrev = (ItemRevision) secondary_objs[i];
				// Changed for TC upgrade
				m_zTaskLogger.debug("PRIONTaskPrep -> PRIMARY OBJECT TYPE IS: " + transPrimaryObject_dataset.get_object_type());
				m_zTaskLogger.debug("PRIONTaskPrep -> SECONDARY OBJECT TYPE IS: " + transSecondaryObject_itemrev.getTypeObject().getName());

				// START - This is only for UG
				// When a translation is created unsing the TSTK handler, the
				// attached primary object is a version and not the dataset
				// itself
				// So if we want the Image under the Dataset, we must get the
				// dataset and we can not work with the version, becuase the
				// version does not have any Image under it. So we will get the
				// secondary object, it should be an Item revision, and we
				// will look for the UG Dataset and then we will change the
				// primaryObjct by the dataset
				// Changed for TC upgrade
				if (transPrimaryObject_dataset.get_object_type().toLowerCase().equals("ugmaster")) {
					if (request.getPropertyObject(TranslationRequest.PRIMARY_OBJS).getDisplayableValue().contains(";")) { // It
						// means,
						// the
						// primary
						// object
						// is
						// a
						// version
						m_zTaskLogger.debug("PRIONTaskPrep -> The Primary object is a UG VERSION!!!");
						ModelObject mos[] = transSecondaryObject_itemrev.get_IMAN_specification();
						for (int xx = 0; xx < mos.length; xx++) {
							ModelObject mo = mos[xx];
							m_zTaskLogger.debug("PRIONTaskPrep ->     Object under ItemRev: " + mo.getTypeObject().getName());
							if (mo.getTypeObject().getName().toLowerCase().equals("ugmaster")) {
								m_zTaskLogger.debug("PRIONTaskPrep ->        current_name: " + mo.getPropertyObject("current_name").getDisplayableValue());
								m_zTaskLogger.debug("PRIONTaskPrep ->        PO current_name: "
										+ transPrimaryObject_dataset.getPropertyObject("current_name").getDisplayableValue());
								if (mo.getPropertyObject("current_name").getDisplayableValue()
										.equals(transPrimaryObject_dataset.getPropertyObject("current_name").getDisplayableValue())) {
									transPrimaryObject_dataset = (Dataset) mo;
									primary_objs[i] = mo;
									break;
								}
							}
						}

					}
				}
				// END - This is only for UG

				// We will check if the PRIMARY Object type corresponds to one
				// of the types
				// that we are willing to translate
				Boolean datasetTypeFound = false;
				for (int x = 0; x < PRIONDSPDatasetBean_vector.size(); x++) {
					m_zTaskLogger.debug("PRIONTaskPrep -> Comparing dataset type with: "
							+ ((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(x)).getDataSetType());
					if (((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(x)).getDataSetType().equals(
							transPrimaryObject_dataset.get_object_type())) {
						datasetTypeFound = true;
						datasetBean = (PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(x);
						m_objectType = datasetBean.getDataSetType();
						// This is the file Extension that we must search for
						// this dataset type
						m_scSourceFileExt = datasetBean.getSourceFileExtension();
						break;
					}
				}
				if (!datasetTypeFound) {
					// If the dataset type does not correspond, we will not
					// prepare it so
					// translation will be marked as NO_TRANS becuase there is
					// nothing to translate
					m_zTaskLogger.debug("PRIONTaskPrep -> The Dataset type: " + transPrimaryObject_dataset.get_object_type()
							+ " is not included on dataset.type property");
					continue;
				}

				m_zTaskLogger.debug("PRIONTaskPrep -> PRIMARY_OBJS: " + request.getPropertyObject(TranslationRequest.PRIMARY_OBJS).getDisplayableValue());

				// Now we must analyze where we should look for the final file
				// SAME object or SON object of transPrimaryObject_dataset, the
				// type of object
				// and the relation
				if (datasetBean.getSourceFileLocation().equals("SAME")) {
					m_FinalDataset = transPrimaryObject_dataset;

				} else if (datasetBean.getSourceFileLocation().equals("SON")) {

					String finalDatasetRelation = datasetBean.getSourceFileRelationWithSon();

					getSon(finalDatasetRelation, primary_objs[i], m_objectType);

				} else {
					m_zTaskLogger.debug("PRIONTaskPrep -> I do not know if the sourcefile is in the same dataset or in the son!!");
					m_zTaskLogger.debug("              -> Value: " + datasetBean.getSourceFileLocation());
					continue;
				}

				// --------------------------- COMMON
				// ---------------------------------------
				// Here we must have the rigth dataset in the m_FinalDataset
				// variable

				// In order to get the file associated to the dataset, we must
				// get
				// the references list of the dataset
				ModelObject[] contexts = m_FinalDataset.get_ref_list();
				ImanFile zIFile = null;

				// For each reference
				for (int j = 0; j < contexts.length; j++) {
					// We only want the ImanFiles
					if (contexts[j] instanceof ImanFile) {
						zIFile = (ImanFile) contexts[j];
						// And we only want the files with extension equal to
						// source.file.extension property
						if ((TranslationRequest.getFileToStaging(zIFile, stagingLoc)).getName().toLowerCase()
								.endsWith((registry.getString("PRIONTaskPrep.source.file.extension." + m_objectType)).toLowerCase())) {
							break;
						}
					}
				}

				// If there is not any file attached to the m_FinalDataset we
				// will exit through an Exception
				if (zIFile == null) {
					throw new Exception("PRIONTaskPrep ->  -> No named reference found for " + m_FinalDataset.get_object_string());
				}

				// Export the file to the staging directory
				File zFile = TranslationRequest.getFileToStaging(zIFile, stagingLoc);

				// Once we have the sourceFile on the Staging dir, we will write
				// the PRION TASK configuration file
				PRIONConfigManager.writePRIONTaskConfiguration(false, request, transPrimaryObject_dataset, transSecondaryObject_itemrev, ReportType,
						RevisionRule, ReportFormat, UserName, stagingLoc, MaxBOMLevel, EqualPart, Ordering, IncludePCB, IncludeSUP, OnlySMD, ReportFilename,
						m_FinalDataset.get_current_name(), zFile.getName(), datasetBean);

				// If the datasetBean has already included the file extension in
				// the filename
				// we do not have to include it agagin.
				if (datasetBean.getFinalFileName().endsWith(m_scSourceFileExt)) {
					this.m_zTaskLogger.debug("        -> zFileName: " + zFile.getName());
					this.m_zTaskLogger.debug("        -> finalName: " + datasetBean.getFinalFileName());
					if (!zFile.getName().toLowerCase().equals(datasetBean.getFinalFileName().toLowerCase())) {
						renameFile(stagingLoc + File.separator + zFile.getName(), stagingLoc + File.separator + datasetBean.getFinalFileName());
						this.m_zTaskLogger.debug("        -> THE FILE HAS BEEN RENAMED TO: " + datasetBean.getFinalFileName());
					}
				} else {
					this.m_zTaskLogger.debug("        -> zFileName: " + zFile.getName());
					this.m_zTaskLogger.debug("        -> finalName: " + datasetBean.getFinalFileName() + m_scSourceFileExt);
					if (!zFile.getName().toLowerCase().equals(datasetBean.getFinalFileName().toLowerCase() + m_scSourceFileExt.toLowerCase())) {
						renameFile(stagingLoc + File.separator + zFile.getName(), stagingLoc + File.separator + datasetBean.getFinalFileName()
								+ m_scSourceFileExt);
						this.m_zTaskLogger.debug("        -> THE FILE HAS BEEN RENAMED TO: " + datasetBean.getFinalFileName() + m_scSourceFileExt);
					}
				}

				// Sanitize the file if needed and if it is configured
				String fileName = "";
				// If the datasetBean has already included the file extension in
				// the filename
				// we do not have to include it agagin.
				if (datasetBean.getFinalFileName().endsWith(m_scSourceFileExt))
					fileName = datasetBean.getFinalFileName();
				else
					fileName = datasetBean.getFinalFileName() + m_scSourceFileExt;

				String exportFilename = null;
				int index = fileName.lastIndexOf(m_scSourceFileExt);
				String fileNameExtTrim = fileName.substring(0, index);
				String sanitizedFilename = getSanitizedFilename(registry.getString("PRIONTaskPrep.sanitize.BadChars"), fileNameExtTrim);

				if (sanitizedFilename != null) {
					// To have unique file names after extraction for files like
					// a b.ext, a-b.ext add the sanitize.replacementChar plus a
					// number
					// to the sanitized filename.
					// Sanitized file names will look like
					// a_b(replacementChar)0.ext, a_b(replacementChar)1.ext
					exportFilename = sanitizedFilename + registry.getString("PRIONTaskPrep.sanitize.replacementChar") + i + m_scSourceFileExt;

					// If the datasetBean has already included the file
					// extension in the filename
					// we do not have to include it agagin.
					if (datasetBean.getFinalFileName().endsWith(m_scSourceFileExt)) {
						// If the file name has been sanitized, rename the file
						// in the staging
						// directory to the sanitized name.
						renameFile(stagingLoc + File.separator + datasetBean.getFinalFileName(), stagingLoc + File.separator + exportFilename);
					} else {
						renameFile(stagingLoc + File.separator + datasetBean.getFinalFileName() + m_scSourceFileExt, stagingLoc + File.separator
								+ exportFilename);
					}
					this.m_zTaskLogger.debug("        -> SANITIZED: THE FILE HAS BEEN RENAMED TO: " + exportFilename);

					// And overwrite the PRION TASK configuration file
					PRIONConfigManager.writePRIONTaskConfiguration(true, request, transPrimaryObject_dataset, transSecondaryObject_itemrev,
							ReportType, RevisionRule, ReportFormat, UserName, stagingLoc, MaxBOMLevel, EqualPart, IncludeSUP, Ordering, IncludePCB, OnlySMD,
							ReportFilename, m_FinalDataset.get_current_name(), exportFilename, datasetBean);
				} else {
					exportFilename = fileName;
				}

				// Once we have the correct file into the staging dir, we
				// reasign the original dataset
				// to the translation task.
				m_FinalDataset = transPrimaryObject_dataset;

				zTransTask = prepTransTask(zTransTask, m_FinalDataset, transSecondaryObject_itemrev, exportFilename, true, true,
						datasetBean.getTranslatedFileExtension(), 0, null);

				this.m_zTaskLogger.debug("\n\n\n\n        -> TRANSTASKID: " + zTransTask.getId());
			}
		} else {
			// We are launching a translation without dataset

			ModelObject primary_objs[] = request.getPropertyObject(TranslationRequest.PRIMARY_OBJS).getModelObjectArrayValue();
			ItemRevision transSecondaryObject_itemrev = (ItemRevision) primary_objs[0];

			m_zTaskLogger.debug("PRIONTaskPrep -> DATASET: - NONE -");
			m_zTaskLogger.debug("ITEM ID: " + transSecondaryObject_itemrev.get_current_id());
			m_zTaskLogger.debug("ITEM REVISION: " + transSecondaryObject_itemrev.get_current_revision_id());

			// If we don't want to process any dataset we will write the
			// PRIONTraskConfiguration File and finish.
			PRIONConfigManager.writePRIONTaskConfigurationNONE(request, transSecondaryObject_itemrev.get_current_id(),
					transSecondaryObject_itemrev.get_current_revision_id(), ReportType, RevisionRule, ReportFormat, UserName, stagingLoc,
					MaxBOMLevel, EqualPart, Ordering, IncludePCB, IncludeSUP, OnlySMD, ReportFilename, (PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0));

			// zTransTask = prepTransTaskNONE(zTransTask,
			// transSecondaryObject_itemrev, 0, null);

			zTransTask = prepareTaskNONE();

			this.m_zTaskLogger.debug("\n\n\n\n        -> TRANSTASKID: " + zTransTask.getId());

		}
		return addRefIdToTask(zTransTask, 0);
	}

	@Override
	protected String getSanitizedFilename(String scBadString, String scFileName) {
		this.m_zTaskLogger.debug("PRIONTaskPrep ->  -> Sanitizing: " + scFileName);
		boolean qSanitized = false;
		char[] scBadChar = scBadString.toCharArray();

		for (int j = 0; j < scBadChar.length; j++) {
			this.m_zTaskLogger.debug("PRIONTaskPrep ->  -> Searching Bad Character: " + scBadChar[j]);
			if (scFileName.indexOf(scBadChar[j]) == -1) {
				continue;
			}
			this.m_zTaskLogger.debug("PRIONTaskPrep ->  ->       - Replacing by: " + registry.getString("PRIONTaskPrep.sanitize.replacementChar"));
			scFileName = scFileName.replace(scBadChar[j], (registry.getString("PRIONTaskPrep.sanitize.replacementChar")).charAt(0));
			qSanitized = true;
		}

		if (!qSanitized) {
			return null;
		}

		this.m_zTaskLogger.debug("PRIONTaskPrep ->  -> Sanitized FileName: " + scFileName);
		return scFileName;
	}

	@SuppressWarnings({ "rawtypes", "unchecked" })
	protected void getSon(String finalDatasetRelation, Object transPrimaryObject_dataset, String transPrimaryObject_type) {
		try {
			Class auxClass = null;
			try {
				auxClass = Class.forName("com.teamcenter.soa.client.model.strong." + transPrimaryObject_type);
			} catch (ClassNotFoundException ex) {
				try {
					this.m_zTaskLogger.debug("PRIONTaskPrep ->  com.teamcenter.soa.client.model.strong." + transPrimaryObject_type + " NOT FOUND");
					if (transPrimaryObject_type.equals("IdeasDrawing")) {
						// auxClass =
						// Class.forName("com.prion.ets.util.Dataset");
						auxClass = Class.forName("com.teamcenter.soa.client.model.strong.Dataset");
					} else {
						auxClass = Class.forName("com.teamcenter.soa.client.model.strong.Dataset");
					}
				} catch (ClassNotFoundException ex1) {
					Logger.getLogger(PRIONTaskPrep.class.getName()).log(Level.SEVERE, null, ex1);
				}
			}

			this.m_zTaskLogger.debug("PRIONTaskPrep ->  Class is " + auxClass.getName());

			Object auxObject = transPrimaryObject_dataset;
			transPrimaryObject_dataset.getClass().cast(auxObject);

			Method metodo = auxClass.getMethod("get_" + finalDatasetRelation, new Class[] {});
			ModelObject[] auxModelObject = (ModelObject[]) metodo.invoke(auxObject, (Object[]) null);

			m_zTaskLogger.debug("PRIONTaskPrep ->  -> " + auxModelObject.length + " Objects found using get_" + finalDatasetRelation + " on "
					+ transPrimaryObject_type);

			for (int y = 0; y < auxModelObject.length; y++) {
				m_FinalDataset = (Dataset) auxModelObject[y];
			}

		} catch (IllegalAccessException ex) {
			Logger.getLogger(PRIONTaskPrep.class.getName()).log(Level.SEVERE, null, ex);
		} catch (NoSuchMethodException ex) {
			Logger.getLogger(PRIONTaskPrep.class.getName()).log(Level.SEVERE, null, ex);
		} catch (IllegalArgumentException ex) {
			Logger.getLogger(PRIONTaskPrep.class.getName()).log(Level.SEVERE, null, ex);
		} catch (InvocationTargetException ex) {
			Logger.getLogger(PRIONTaskPrep.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	public static void renameFile(String oldName, String newName) throws IOException {
		File srcFile = new File(oldName);
		boolean bSucceeded = false;
		try {
			File destFile = new File(newName);
			if (destFile.exists()) {
				if (!destFile.delete()) {
					throw new IOException(oldName + " was not successfully renamed to " + newName);
				}
			}
			if (!srcFile.renameTo(destFile)) {
				throw new IOException(oldName + " was not successfully renamed to " + newName);
			} else {
				bSucceeded = true;
			}
		} finally {
			if (bSucceeded) {
				srcFile.delete();
			}
		}
	}

	public TranslationTask prepareTaskNONE() throws Exception {

		File zDummyFile = new File(this.stagingLoc + File.separatorChar + "dummy.txt");
		zDummyFile.createNewFile();
		TranslationTask zTransTask = addRefIdToTask(prepTransTask(null, null, null, "dummy.txt", false, false, null, 0, null), 0);

		return addOptions(zTransTask, "TASK_ID", this.request.getPropertyObject("taskID").getStringValue());
	}

}
