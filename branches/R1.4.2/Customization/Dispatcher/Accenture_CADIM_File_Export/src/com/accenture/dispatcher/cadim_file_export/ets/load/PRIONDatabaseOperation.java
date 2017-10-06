/*==============================================================================
Copyright 2009.
Siemens Product Lifecycle Management Software Inc.
All Rights Reserved.
================================================================================
File description:   This custom class is a proe specific sub class of the base
DatabaseOperation class which performs the loading operation
to Tc. This class stores results for translation requests.
This is a configuration specified class based on provider
name and translator name in DispatcherClient property file.

Filename:   DatabaseOperation.java
=================================================================================*/
//==== Package  =================================================================
package com.accenture.dispatcher.cadim_file_export.ets.load;

//==== Imports  =================================================================
import com.accenture.dispatcher.cadim_file_export.ets.util.Dataset;
import com.accenture.dispatcher.cadim_file_export.ets.util.PRIONConfigManager;
import com.accenture.dispatcher.cadim_file_export.ets.util.PRIONDSPDatasetBean;
import com.accenture.dispatcher.cadim_file_export.ets.util.PRIONDataSetHelper;

import java.util.List;

import com.teamcenter.ets.load.DefaultDatabaseOperation;

import com.teamcenter.translationservice.task.TranslationDBMapInfo;

//import com.teamcenter.ets.util.Registry;

import com.teamcenter.soa.client.model.strong.ItemRevision;
import java.util.Vector;

//==== Class ====================================================================
/**
 * This custom class is a proe specific sub class of the base DatabaseOperation
 * class which performs the loading operation to Tcg. This class stores results
 * for translation requests. This is a configuration specified class based on
 * provider name and translator name in ETS service property file.
 */
public class PRIONDatabaseOperation extends DefaultDatabaseOperation {

	@SuppressWarnings("rawtypes")
	private Vector PRIONDSPDatasetBean_vector = null;
	private PRIONDSPDatasetBean datasetBean = null;

	/**
	 * Stores translation result data on the source dataset. If there are
	 * existing translation result data files, they may be replaced depending
	 * upon the value of the UpdateExistingVisualizationData TaskPrep instance
	 * variable.
	 * 
	 * @param zDbMapInfo
	 *            DB mapper info object.
	 * @param zFileList
	 *            List of files mapped to the source file.
	 * 
	 * @throws Exception
	 */
	@SuppressWarnings("rawtypes")
	@Override
	public void query() throws Exception {

		m_zTaskLogger.debug("PRIONTaskPrep -> Starting query() operation\n");

		// The registry must be linked with our properties file
		// registry =
		// Registry.getRegistry("com.prion.tstk.translator.priontranslator");

		PRIONDSPDatasetBean_vector = new Vector();
		PRIONConfigManager.RetrieveTransaltorConfig(transTask.getService(),
				PRIONDSPDatasetBean_vector, true, m_zTaskLogger);

		m_zTaskLogger.debug("PRIONTaskPrep -> DsName:"
				+ ((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0))
						.getDsName() + "\n");
		m_zTaskLogger.debug("PRIONTaskPrep -> FileName:"
				+ ((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0))
						.getFileName() + "\n");

		if (!((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0))
				.getDataSetType().equals("NONE")) {
			super.query();
			this.sourceDataset = ((Dataset) this.primaryObj);
			this.sourceItemRev = ((ItemRevision) this.secondaryObj);
		}
	}

	@SuppressWarnings("rawtypes")
	@Override
	public void processTask() throws Exception {

		PRIONDSPDatasetBean_vector = new Vector();
		PRIONConfigManager.RetrieveTransaltorConfig(transTask.getService(),
				PRIONDSPDatasetBean_vector, true, m_zTaskLogger);

		if (!((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0))
				.getDataSetType().equals("NONE")) {
			super.processTask();
		}
	}

	@Override
	protected void load(TranslationDBMapInfo zDbMapInfo, List<String> zFileList)
			throws Exception {

		m_zTaskLogger.debug("PRIONTaskPrep -> Starting load() operation\n");

		if (!((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(0))
				.getDataSetType().equals("NONE")) {

			// We will check if the PRIMARY Object type corresponds to one of
			// the types
			// that we are willing to translate
			datasetBean = new PRIONDSPDatasetBean();
			Boolean datasetTypeFound = false;
			for (int x = 0; x < PRIONDSPDatasetBean_vector.size(); x++) {
				m_zTaskLogger
						.debug("PRIONTaskPrep -> Comparing dataset type with: "
								+ ((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector
										.get(x)).getDataSetType());
				// Changed for upgrade
				if (((PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector.get(x))
						.getDataSetType()
						.equals(primaryObj.getTypeObject().getName())) {
					datasetTypeFound = true;
					datasetBean = (PRIONDSPDatasetBean) PRIONDSPDatasetBean_vector
							.get(x);
					m_zTaskLogger.debug("PRIONTaskPrep -> DATSET Type FOUND!!");
					m_zTaskLogger.debug("\t - datasetType: "
							+ datasetBean.getDataSetType());
					m_zTaskLogger.debug("\t - destinationObject: "
							+ datasetBean.getTranslatedDatasetType());
					m_zTaskLogger.debug("\t - relationType: "
							+ datasetBean.getTranslatedDatasetRelationWithIR());
					m_zTaskLogger.debug("\t - namedReferenceType: "
							+ datasetBean.getTranslatedFileReferenceType());
					break;
				}
			}
			if (!datasetTypeFound) {
				// If the dataset type does not correspond, we will not prepare
				// it so
				// translation will be marked as NO_TRANS becuase there is
				// nothing to translate
				
				// Changed for Upgrade
				m_zTaskLogger.debug("PRIONTaskPrep -> The Dataset type: "
						+ primaryObj.getTypeObject().getName()
						+ " is not included on dataset.type property");
			} else {
				m_zTaskLogger
						.debug("PRIONDatabaseOperation -> Loading the file to Teamcenter ");
				loadPart(zFileList);
			}
		}
	} // end load()

	/**
	 * Loads part visualization data. This method calls the createInsetDataset
	 * which creates or inserts a dataset and/or dataset revision, relates newly
	 * created datasets to the item revision, and attaches the specified named
	 * reference files. If there are existing visualization data files, they may
	 * be replaced depending upon the setting of the
	 * UpdateExistingVisualizationData ETS Service property. This property is a
	 * boolean flag updateExistingVisData, which is read by the abstract
	 * DatabaseOperation class.
	 * 
	 * @param zFileList
	 *            List of files mapped to the source file.
	 * 
	 * @throws Exception
	 */
	protected void loadPart(List<String> zFileList) throws Exception {
		// Changed for upgrade
		m_zTaskLogger.debug("PRIONDatabaseOperation -> datasetType: "
				+ primaryObj.getTypeObject().getName());
		m_zTaskLogger.debug("\t - destinationObject: "
				+ datasetBean.getTranslatedDatasetType());
		m_zTaskLogger.debug("\t - relationType: "
				+ datasetBean.getTranslatedDatasetRelationWithIR());
		m_zTaskLogger.debug("\t - namedReferenceType: "
				+ datasetBean.getTranslatedFileReferenceType());

		PRIONDataSetHelper pDtSetHelper = new PRIONDataSetHelper(
				this.m_zTaskLogger, this.updateExistingVisData,
				this.softFailureHandlingEnabled);
		pDtSetHelper.createInsertDataset(sourceItemRev, sourceDataset,
				datasetBean.getTranslatedDatasetType(), // Type
														// of
														// Object
														// that
														// we
														// have
														// to
														// create
														// in
														// TC
														// to
														// store
														// the
														// file
				datasetBean.getTranslatedDatasetRelationWithIR(), // Relation to
																	// attach
																	// the new
																	// object to
																	// the
																	// ItemRevision
				datasetBean.getTranslatedFileReferenceType(), // NamedReference
																// type to
																// import the
																// file into the
																// new onject
				m_scResultDir, zFileList, false);

		// pDtSetHelper.createInsertDataset ( sourceItemRev,
		// sourceDataset,
		// "PDF", // Type of Object that we have to create in TC to store the
		// file
		// "IMAN_specification", // Relation to attach the new object to the
		// ItemRevision
		// "PDF", // NamedReference type to import the file into the new onject
		// m_scResultDir,
		// zFileList,
		// false);

		/*
		 * zDtSetHelper.createInsertDataset ( sourceItemRev, sourceDataset,
		 * registry
		 * .getString("PRIONDatabaseOperation.translated.file.destinationObject."
		 * + primaryObj.getTypeObject().getName()), // Type of Object that we
		 * have to create in TC to store the file
		 * registry.getString("PRIONDatabaseOperation.translated.file.relationType."
		 * + primaryObj.getTypeObject().getName()), // Relation to attach the
		 * new object to the ItemRevision registry.getString(
		 * "PRIONDatabaseOperation.translated.file.namedReferenceType." +
		 * primaryObj.getTypeObject().getName()), // NamedReference type to
		 * import the file into the new onject m_scResultDir, zFileList, false);
		 */

	} // loadPart
}
