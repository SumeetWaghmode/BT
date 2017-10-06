/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.accenture.dispatcher.report_bom.ets.util;

import com.teamcenter.ets.ETSInit;
import com.teamcenter.ets.soa.ConnectionManager;
import com.teamcenter.ets.soa.TeamcenterServerProxy;
import com.teamcenter.services.loose.core._2006_03.FileManagement;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.Type;
import com.teamcenter.soa.client.model.strong.Dataset;
import com.teamcenter.soa.client.model.strong.DatasetType;
import com.teamcenter.soa.client.model.strong.Group;
import com.teamcenter.soa.client.model.strong.ImanFile;
import com.teamcenter.soa.client.model.strong.ImanVolume;
import com.teamcenter.soa.client.model.strong.ItemRevision;
import com.teamcenter.soa.client.model.strong.ReleaseStatus;
import com.teamcenter.soa.client.model.strong.RevisionAnchor;
import com.teamcenter.soa.client.model.strong.Tool;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.exceptions.NotLoadedException;
import com.teamcenter.tstk.util.log.ITaskLogger;
import com.teamcenter.tstk.util.log.LoggerFactoryUtil;
import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import com.teamcenter.ets.util.Registry;

public class PRIONDataSetHelper {
	private Registry m_zRegistry = null;
	protected ImanVolume m_zTempVol = null;
	protected boolean m_qUpdateExistingVisData = false;
	protected boolean m_qStoreInSrcVol = false;
	protected String m_scOwner;
	protected static final String DATASET_OWNER_DEFAULT = "DC";
	protected static final String DATASET_OWNER_CAD = "CAD";
	protected static final String DATASET_OWNER_NONE = "NONE";
	protected static final String BINARY_FILE_TYPE = "BINARY";
	protected static final String NO_FILE_SUB_TYPE = "";
	public static final String TC_DS_TYPE_DIRECT_MODEL = "DirectModel";
	public static final String TC_NR_TYPE_JT_PART = "JTPART";
	public static final String TC_REL_TYPE_RENDERING = "IMAN_Rendering";
	public static final String TC_DS_TYPE_IMAGE = "Image";
	public static final String TC_NR_TYPE_IMAGE = "Image";
	public static final String TC_REL_TYPE_CAPTURE = "IMAN_capture";
	public static final String TC_REL_TYPE_MANIFESTATION = "IMAN_manifestation";
	public static final String TC_REL_TYPE_SPECIFICATION = "IMAN_specification";
	public static final int CREATE_INSERT_PENDING = 0;
	public static final int CREATED = 1;
	public static final int INSERTED = 2;
	public static final int COMPLETED = 3;
	public static final int NOT_LOCALLY_OWNED = 4;
	protected ITaskLogger m_zTaskLogger = LoggerFactoryUtil.getDefaultTaskLogger();

	private boolean m_qSoftFailureHandlingEnabled = true;

	public PRIONDataSetHelper(ITaskLogger zTaskLogger, boolean updateExistingVisData, boolean qSoftFailureHandlingEnabled) {
		this.m_zTaskLogger = zTaskLogger;
		this.m_qUpdateExistingVisData = updateExistingVisData;

		this.m_zRegistry = Registry.getRegistry("com.teamcenter.ets.ets");
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> DataSetHelper updateExistingVisData = " + updateExistingVisData);

		this.m_qSoftFailureHandlingEnabled = qSoftFailureHandlingEnabled;
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> DataSetHelper softFailureHandlingEnabled = "
				+ this.m_qSoftFailureHandlingEnabled);

		this.m_scOwner = ETSInit.getETSProperties().getProperty("Service.DataSetOwner", "DC");
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> DataSetHelper DataSetOwner = " + this.m_scOwner);
		if ((!this.m_scOwner.equalsIgnoreCase("DC")) && (!this.m_scOwner.equalsIgnoreCase("CAD"))
				&& (!this.m_scOwner.equalsIgnoreCase("NONE"))) {
			this.m_zTaskLogger.error("Unknown Service.DataSetOwner property value. Using DC");

			this.m_scOwner = "DC";
		}

		if (ETSInit.getETSProperties().getProperty("Service.StoreJTFilesInSourceVol", "true").equalsIgnoreCase("true")) {
			this.m_qStoreInSrcVol = true;
		}
	}

	public boolean hasNRFileObj(ModelObject context, Dataset dataset, String relationType, String datasetType,
			String namedReference) throws Exception {
		boolean hasNRFileObj = false;
		this.m_zTaskLogger.trace("  hasNRFileObj() begin...", 0);

		if (context == null) {
			throw new NullPointerException("null context");
		}
		if (dataset == null) {
			throw new NullPointerException("null dataset");
		}
		if (relationType == null) {
			throw new NullPointerException("null relationType");
		}
		if (datasetType == null) {
			throw new NullPointerException("null datasetType");
		}
		if (namedReference == null) {
			throw new NullPointerException("null namedReference");
		}

		try {
			ImanFile[] nrFiles = getNRFileObjs(context, dataset, relationType, datasetType, namedReference);

			if ((nrFiles != null) && (nrFiles.length > 0)) {
				hasNRFileObj = true;
			}
		} catch (Exception e) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     in hasNRFileObj() catch block", e);
			throw e;
		}

		this.m_zTaskLogger.trace("  hasNRFileObj() end", 1);
		return hasNRFileObj;
	}

	public boolean hasNRFileObj(Dataset dataset, String namedReference) throws Exception {
		boolean hasNRFileObj = false;
		this.m_zTaskLogger.trace("  hasNRFileObj() " + namedReference + " begin...", 0);

		if (dataset == null) {
			throw new NullPointerException("null dataset");
		}
		if (namedReference == null) {
			throw new NullPointerException("null namedReference");
		}

		try {
			ImanFile[] nrFiles = getNRFiles(dataset, namedReference);

			if ((nrFiles != null) && (nrFiles.length > 0)) {
				hasNRFileObj = true;
			}
		} catch (Exception e) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     in hasNRFileObj() catch block", e);
			throw e;
		}

		this.m_zTaskLogger.trace("  hasNRFileObj() end", 0);
		return hasNRFileObj;
	}

	public boolean hasNRObj(Dataset dataset, String namedReference) throws Exception {
		boolean hasNRObj = false;
		this.m_zTaskLogger.trace("  hasNRObj() " + namedReference + " begin...", 0);

		if (dataset == null) {
			throw new NullPointerException("null dataset");
		}
		if (namedReference == null) {
			throw new NullPointerException("null namedReference");
		}

		try {
			ModelObject[] nrComps = getNRObjs(dataset, namedReference);

			if ((nrComps != null) && (nrComps.length > 0)) {
				hasNRObj = true;
			}
		} catch (Exception e) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     in hasNRObj() catch block", e);
			throw e;
		}

		this.m_zTaskLogger.trace("  hasNRObj() end", 0);
		return hasNRObj;
	}

	public ImanFile[] getNRFileObjs(ModelObject context, Dataset dataset, String relationType, String datasetType,
			String namedReference) throws Exception {
		ImanFile[] nrFiles = null;
		this.m_zTaskLogger.trace("  getNRFileObjs() " + namedReference + " begin...", 0);

		if (context == null) {
			throw new NullPointerException("null context");
		}
		if (dataset == null) {
			throw new NullPointerException("null dataset");
		}
		if (relationType == null) {
			throw new NullPointerException("relationType");
		}
		if (datasetType == null) {
			throw new NullPointerException("datasetType");
		}
		if (namedReference == null) {
			throw new NullPointerException("namedReference");
		}

		int cadVersion = dataset.get_revision_number();
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->     dataset version = " + cadVersion);
		if (cadVersion <= 0) {
			this.m_zTaskLogger.error("    dataset version <= 0");
			throw new Exception(this.m_zRegistry.getString("DatasetHelper.invalidCADDatasetVersionNumber"));
		}

		try {
			Dataset relatedDataset = findDataset(context, dataset, relationType, datasetType);

			int version = 0;
			Dataset relatedDatasetVersion = null;

			if (relatedDataset != null) {
				ModelObject[] relatedDatasetVersions = getAllRevisions(relatedDataset);

				for (int i = 0; i < relatedDatasetVersions.length; i++) {
					version = relatedDataset.get_revision_number();
					if (version != cadVersion)
						continue;
					relatedDatasetVersion = (Dataset) relatedDatasetVersions[i];
					break;
				}
			}

			if (version == cadVersion) {
				nrFiles = getNRFiles(relatedDatasetVersion, namedReference);
			}
		} catch (Exception e) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     in getNRFileObjs() catch block", e);
			throw e;
		}

		this.m_zTaskLogger.trace("  getNRFileObjs() end", 0);
		return nrFiles;
	}

	@SuppressWarnings("deprecation")
	public Dataset findDataset(ModelObject sourceComponent, Dataset sourceDataset, String relationType, String datasetType)
			throws Exception {
		this.m_zTaskLogger.trace("  findDataset() begin...", 0);

		if (sourceComponent == null) {
			this.m_zTaskLogger.error("    input param sourceComponent is null");
			throw new NullPointerException("sourceComponent");
		}
		if (relationType == null) {
			this.m_zTaskLogger.error("    input param relationType is null");
			throw new NullPointerException("relationType");
		}
		if (datasetType == null) {
			this.m_zTaskLogger.error("    input param datasetType is null");
			throw new NullPointerException("datasetType");
		}

		ModelObject sourceComponentToCheck = sourceComponent;
		if ((sourceComponent instanceof Dataset)) {
			Dataset sourceDS = (Dataset) sourceComponent;

			if (sourceDS.get_revision_number() != 0) {
				ModelObject[] datasetVersions = getAllRevisions(sourceDS);

				if ((datasetVersions != null) && (datasetVersions.length > 0)) {
					sourceComponentToCheck = datasetVersions[0];
				}
			}
		}

		com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2[] relationFilters = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2[1];
		relationFilters[0] = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2();
		relationFilters[0].relationName = relationType;
		ModelObject[] childComps = TeamcenterServerProxy.getInstance().getChildrenPrimary(sourceComponentToCheck,
				relationFilters);

		Dataset foundDataset = null;

		if ((childComps != null) && (childComps.length > 0)) {
			for (int i = 0; i < childComps.length; i++) {
				if (!(childComps[i] instanceof Dataset))
					continue;
				Dataset targetDataset = (Dataset) childComps[i];
				targetDataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(targetDataset,
						new String[] { "dataset_type", "object_name" });
				DatasetType zDsType = targetDataset.get_dataset_type();
				zDsType = (DatasetType) TeamcenterServerProxy.getInstance().getProperties(zDsType, "datasettype_name");
				String dsType = zDsType.get_datasettype_name();

				sourceDataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(sourceDataset, "object_name");
				String sourceDatasetName = sourceDataset.get_object_name();

				String targetDatasetName = targetDataset.get_object_name();

				if ((!sourceDatasetName.equals(targetDatasetName)) || (!dsType.equals(datasetType))) {
					continue;
				}
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->     dataset found");
				foundDataset = targetDataset;
				TeamcenterServerProxy.getInstance().refresh(foundDataset);
				break;
			}

		} else {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> No child datasets related to source component "
					+ sourceDataset.get_object_string());
		}

		this.m_zTaskLogger.trace("  findDataset() end", 0);
		return foundDataset;
	}

	@SuppressWarnings("deprecation")
	protected Dataset[] insertDatasetVersion(Dataset sourceDataset, Dataset resultDataset) throws Exception {
		this.m_zTaskLogger.trace("insertDatasetVersion() begin...", 0);

		if (sourceDataset == null) {
			this.m_zTaskLogger.error("  input param sourceDataset is null");
			throw new NullPointerException("sourceDataset");
		}

		if (resultDataset == null) {
			this.m_zTaskLogger.error("  input param resultDataset is null");
			throw new NullPointerException("resultDataset");
		}

		int version = sourceDataset.get_revision_number();
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   sourceDataset version = " + version);

		if (version <= 0) {
			this.m_zTaskLogger.error("  sourceDataset version <= 0");
			throw new Exception(this.m_zRegistry.getString("DatasetHelper.invalidCADDatasetVersionNumber"));
		}

		if (this.m_scOwner.equalsIgnoreCase("DC")) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->   insure ETS ownership of resultDataset");

			User dsUser = (User) resultDataset.get_owning_user();
			Group dsGroup = (Group) resultDataset.get_owning_group();
			User etsUser = ConnectionManager.getLoggedInUser();
			etsUser = (User) TeamcenterServerProxy.getInstance().getProperties(etsUser, "login_group");
			Group etsGroup = (Group) etsUser.get_login_group();

			String dsUserId = dsUser.get_user_id();
			String dsGroupName = dsGroup.get_name();
			String etsUserId = etsUser.get_user_id();
			String etsGroupName = etsGroup.get_name();

			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     resultDataset owning user " + dsUserId + ", group "
					+ dsGroupName);
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     ETS owning user " + etsUserId + ", group " + etsGroupName);

			if ((!dsUser.equals(etsUser)) || (!dsGroup.equals(etsGroup))) {
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->     changing vis dataset owner to ETS user/group...");
				try {
					TeamcenterServerProxy.getInstance().changeOwner(resultDataset, etsUser, etsGroup);
				} catch (Exception e) {
					String msg = this.m_zRegistry.getString("DatasetHelper.chgRsltsDsOwnerFailed") + " ("
							+ resultDataset.get_object_string() + ")";

					this.m_zTaskLogger.error(msg, e);
					throw new Exception(e.toString());
				}
			}
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   calling EtsInsertDatasetVersion");
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->     resultDataset = " + resultDataset);
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->     version       = " + version);

		ModelObject[] insertedDatasets = null;

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> SOA START: TranslationManagement::insertDatasetVersion()");

		insertedDatasets = TeamcenterServerProxy.getInstance().insertDatasetVersion(resultDataset, version);

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> SOA END: TranslationManagement::insertDatasetVersion()");

		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   EtsInsertDatasetVersion returned");

		if ((insertedDatasets.length != 2) || (insertedDatasets[0] == null)) {
			this.m_zTaskLogger
					.error("  insertDatasetVersion failed. This could happen if DispatcherClient proxy user does not have adequate permissions to create or insert a dataset.");

			throw new Exception("InsertDatasetVersion returned no objects");
		}

		this.m_zTaskLogger.trace("insertDatasetVersion() end", 0);

		Dataset[] azInsertedDatasets = new Dataset[insertedDatasets.length];
		for (int i = 0; i < insertedDatasets.length; i++) {
			ModelObject modelObject = insertedDatasets[i];
			if (!(modelObject instanceof Dataset))
				continue;
			azInsertedDatasets[i] = ((Dataset) modelObject);
		}

		return azInsertedDatasets;
	}

	@SuppressWarnings({ "rawtypes", "unchecked", "deprecation" })
	protected Dataset[] createDataset(ModelObject sourceComponent, Dataset sourceDataset, String datasetType,
			String relationType, String resultsDir) throws Exception {
		this.m_zTaskLogger.trace("createDataset() begin...", 0);

		if (sourceComponent == null) {
			this.m_zTaskLogger.error("  input param sourceComponent is null");
			throw new NullPointerException("sourceComponent");
		}
		if (sourceDataset == null) {
			this.m_zTaskLogger.error("  input param sourceDataset is null");
			throw new NullPointerException("sourceDataset");
		}
		if (datasetType == null) {
			this.m_zTaskLogger.error("  input param datasetType is null");
			throw new NullPointerException("datasetType");
		}
		if (relationType == null) {
			this.m_zTaskLogger.error("  input param relationType is null");
			throw new NullPointerException("relationType");
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> sourceComponent     "
				+ sourceComponent.getPropertyObject("object_string").getStringValue());
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   type              " + sourceComponent.getTypeObject().getName());
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> sourceDataset       " + sourceDataset.get_object_string());
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   type              " + sourceDataset.getTypeObject().getName());
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> datasetType         " + datasetType);
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> relationType        " + relationType);

		String name = sourceDataset.get_object_name();
		int version = sourceDataset.get_revision_number();

		if ((name == null) || (name.length() == 0)) {
			this.m_zTaskLogger.error("  sourceDataset name is null or zero length");
			throw new NullPointerException("sourceDataset name");
		}

		if (version <= 0) {
			this.m_zTaskLogger.error("  sourceDataset version <= 0");
			throw new NullPointerException("sourceDataset version");
		}

		String typeName = null;
		Tool tool = null;

		com.teamcenter.services.strong.core._2007_06.DataManagement.DatasetTypeInfo[] zInfos = TeamcenterServerProxy
				.getInstance().getDatasetTypeInfo(datasetType);
		DatasetType dsType = zInfos[0].tag;

		dsType = (DatasetType) TeamcenterServerProxy.getInstance().getProperties(dsType,
				new String[] { "datasettype_name", "list_of_tools" });

		typeName = dsType.get_datasettype_name();

		if ((typeName == null) || (typeName.length() == 0)) {
			this.m_zTaskLogger.error("Dataset data type name is null or zero length");
			throw new NullPointerException("DatasetTypeInfoResponse typeName");
		}
		tool = dsType.get_list_of_tools()[0];

		if ((tool == null) || (tool.get_object_string().length() == 0)) {
			this.m_zTaskLogger.error("Dataset data tool name is null or zero length");
			throw new NullPointerException("DatasetTypeInfoResponse tool");
		}

		Dataset[] returnedObjects = null;

		ItemRevision sourceItemRevision = null;

		if ((sourceComponent instanceof ItemRevision)) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Source component IS an instance of an Item Revision");
			sourceItemRevision = (ItemRevision) sourceComponent;
		} else {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Source component IS NOT an instance of an Item Revision.");
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Checking for a related Item Revision using source dataset.");

			ModelObject[] arSourceDatasetsVersions = getAllRevisions((Dataset) sourceComponent);

			if ((arSourceDatasetsVersions.length > 0) && ((arSourceDatasetsVersions[0] instanceof Dataset))) {
				sourceItemRevision = (ItemRevision) ((Dataset) arSourceDatasetsVersions[0]).get_item_revision();
			}
		}

		PRIONTaskConfBean taskConf = new PRIONTaskConfBean();
		String prionTaskConfPath = resultsDir.substring(0, resultsDir.lastIndexOf("\\"));
		this.m_zTaskLogger.info("PRIONDataSetHelper -> " + prionTaskConfPath);
		PRIONConfigManager.readPRIONTaskConfiguration(taskConf, prionTaskConfPath + "\\PRIONTaskConf.xml");
		this.m_zTaskLogger.info("PRIONDataSetHelper ->    DSName: " + taskConf.getDatasetName());

		if (this.m_zTaskLogger.isDebugEnabled()) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Calling Service - createDatasetOfVersion - START");
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     source item revision = "
					+ sourceItemRevision.get_object_string());
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     target dataset name  = " + taskConf.getDatasetName());
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     target dataset type  = " + typeName);
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     target dataset tool  = " + tool.get_object_string());
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     target dataset ver   = " + version);
		}

		this.m_zTaskLogger.debug("\n\n\nPRIONDataSetHelper ->     HERE WE SET THE DATASET NAME!!!!\n\n\n");
		returnedObjects = TeamcenterServerProxy.getInstance().createDatasetOfVersion(sourceItemRevision,
				taskConf.getDatasetName(), typeName, tool, version);
		// returnedObjects =
		// TeamcenterServerProxy.getInstance().createDatasetOfVersion(sourceItemRevision,
		// name, typeName, tool, version);
		// returnedObjects =
		// TeamcenterServerProxy.getInstance().createDatasetOfVersion(sourceItemRevision,
		// "ESTO_ES_UNA_PRUEBA", typeName, tool, version);

		if (this.m_zTaskLogger.isDebugEnabled()) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> \tService returned " + returnedObjects.length
					+ " created datasets.");
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Calling Service - createDatasetOfVersion - COMPLETE");
		}

		if ((returnedObjects == null) || (returnedObjects.length != 2) || (returnedObjects[0] == null)
				|| (returnedObjects[1] == null)) {
			this.m_zTaskLogger
					.error("SOA createDatasetOfVersion Service Failed. In some cases this could be because the DispatcherClient proxy user does not have adequate permissions to modify the ItemRev or Dataset.");

			throw new NullPointerException("createDatasetOfVersion");
		}

		Exception changeOwnerException = null;

		if (this.m_scOwner.equalsIgnoreCase("CAD")) {
			User user = (User) sourceDataset.get_owning_user();
			Group group = (Group) sourceDataset.get_owning_group();
			try {
				TeamcenterServerProxy.getInstance().changeOwner(returnedObjects[1], user, group);
			} catch (Exception e) {
				String scMsg = this.m_zRegistry.getString("DatasetHelper.chgNewRsltsDsOwnerFailed") + " ("
						+ returnedObjects[1].get_object_string() + ")";

				this.m_zTaskLogger.error(scMsg, e);

				changeOwnerException = e;
			}
		}

		try {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->   adding to sourceComponent with " + relationType + " relation");

			ModelObject[] datasetVersions = null;

			if ((sourceComponent instanceof Dataset)) {
				Dataset sourceComponentAsDataset = (Dataset) sourceComponent;

				this.m_zTaskLogger.debug("PRIONDataSetHelper -> \tSource Component ("
						+ sourceComponentAsDataset.get_object_string() + ") is of type: DATASET");

				datasetVersions = getAllRevisions(sourceComponentAsDataset);

				TeamcenterServerProxy.getInstance().createRelation(datasetVersions[0], relationType, returnedObjects[0]);
			} else if ((sourceComponent instanceof ItemRevision)) {
				datasetVersions = getAllRevisions(sourceDataset);

				this.m_zTaskLogger.debug("PRIONDataSetHelper -> \tsource component "
						+ sourceComponent.getPropertyObject("object_string").getStringValue()
						+ " is NOT a dataset, relating to all related item revisions.");

				com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2[] azFilters = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2[1];
				azFilters[0] = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2();
				azFilters[0].objectTypeNames = new String[] { "ItemRevision" };
				ModelObject[] primaryComponents = TeamcenterServerProxy.getInstance().getChildrenPrimary(datasetVersions[0],
						azFilters);

				List sourceDatasetItemRevs = new ArrayList();

				boolean relatedSourceDataset = false;

				for (int iIndex = 0; iIndex < primaryComponents.length; iIndex++) {
					if (!(primaryComponents[iIndex] instanceof ItemRevision))
						continue;
					if (sourceComponent == primaryComponents[iIndex]) {
						relatedSourceDataset = true;
					}

					sourceDatasetItemRevs.add(primaryComponents[iIndex]);
				}

				if (relatedSourceDataset) {
					Iterator iter = sourceDatasetItemRevs.iterator();

					while (iter.hasNext()) {
						TeamcenterServerProxy.getInstance().createRelation((ModelObject) iter.next(), relationType,
								returnedObjects[0]);
					}

				} else {
					TeamcenterServerProxy.getInstance().createRelation(sourceComponent, relationType, returnedObjects[0]);
				}

			} else {
				throw new Exception("Source Component is not a DATASET or an ITEM REVISION.");
			}
		} catch (Exception e) {
			this.m_zTaskLogger.error(this.m_zRegistry.getString("DatasetHelper.relationCreationFailed1") + " "
					+ relationType + " " + this.m_zRegistry.getString("DatasetHelper.relationCreationFailed2") + " "
					+ datasetType + " " + this.m_zRegistry.getString("DatasetHelper.relationCreationFailed3") + " ("
					+ sourceComponent.getPropertyObject("object_string").getStringValue() + "," + returnedObjects[0] + ")",
					e);

			throw new Exception(e.toString());
		}
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   relation create complete");

		if (changeOwnerException != null) {
			throw new Exception(changeOwnerException.toString());
		}

		this.m_zTaskLogger.trace("createDataset() end", 0);

		return returnedObjects;
	}

	@SuppressWarnings("deprecation")
	public static ModelObject[] getAllRevisions(Dataset sourceComponent) throws NotLoadedException, Exception {
		sourceComponent = (Dataset) TeamcenterServerProxy.getInstance().getProperties(sourceComponent, "rev_chain_anchor");

		RevisionAnchor zRevAnch = sourceComponent.get_rev_chain_anchor();
		zRevAnch = (RevisionAnchor) TeamcenterServerProxy.getInstance().getProperties(zRevAnch, "revisions");
		ModelObject[] arSourceDatasetsVersions = zRevAnch.get_revisions();
		return arSourceDatasetsVersions;
	}

	public int createInsertDataset(ItemRevision sourceItemRev, Dataset sourceDataset, String datasetType,
			String relationType, String namedReference, String resultsDir, List<String> fileList, boolean qOnSourceDataset)
			throws Exception {
		int iCreateInsertStatus = 0;

		this.m_zTaskLogger.trace("createInsertDataset() begin...", 0);

		Dataset[] datasets = null;
		Dataset resultDataset = null;
		Dataset[] resultDatasetVers = null;
		Dataset resultDatasetVer = null;

		if (sourceItemRev == null) {
			this.m_zTaskLogger.error("  input param sourceItemRev is null");
			throw new NullPointerException("sourceItemRev");
		}
		if (sourceDataset == null) {
			this.m_zTaskLogger.error("  input param sourceDataset is null");
			throw new NullPointerException("sourceDataset");
		}
		if (datasetType == null) {
			this.m_zTaskLogger.error("  input param datasetType is null");
			throw new NullPointerException("datasetType");
		}
		if (relationType == null) {
			this.m_zTaskLogger.error("  input param relationType is null");
			throw new NullPointerException("relationType");
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   sourceItemRev    " + sourceItemRev.get_object_string());
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   sourceDataset    " + sourceDataset.get_object_string());
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   datasetType      " + datasetType);
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   relationType     " + relationType);

		if ((namedReference != null) && (namedReference.length() > 0))
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->   namedReference   " + namedReference);
		if (fileList != null) {
			for (int i = 0; i < fileList.size(); i++) {
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->   fileList[" + i + "]      " + fileList.get(i));
			}
		}
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   qOnSourceDataset " + qOnSourceDataset);
		try {
			TeamcenterServerProxy.getInstance().setBypass("true");

			ModelObject findSource = null;
			try {
				if (qOnSourceDataset) {
					ModelObject[] arDataset = getAllRevisions(sourceDataset);
					findSource = arDataset[0];
				} else {
					findSource = sourceItemRev;
				}

				resultDataset = findDataset(findSource, sourceDataset, relationType, datasetType);
			} catch (Exception e) {
				String scMsg = this.m_zRegistry.getString("DatasetHelper.findDsFailed") + " ("
						+ findSource.getPropertyObject("object_string").getStringValue() + ", " + relationType + ", "
						+ datasetType + ")";

				this.m_zTaskLogger.error(scMsg, e);
				throw new Exception(e.toString());
			}

			if (resultDataset == null) {
				this.m_zTaskLogger
						.debug("PRIONDataSetHelper ->   translation results dataset not found, creating dataset and dataset version...");

				if (qOnSourceDataset) {
					datasets = createDataset(sourceDataset, sourceDataset, datasetType, relationType, resultsDir);
				} else {
					datasets = createDataset(sourceItemRev, sourceDataset, datasetType, relationType, resultsDir);
				}
				resultDataset = datasets[0];
				resultDatasetVer = datasets[1];

				if (fileList != null) {
					if (fileList.isEmpty()) {
						this.m_zTaskLogger.debug("PRIONDataSetHelper ->   There are no files to attach to the new dataset");
					} else if ((namedReference != null) && (namedReference.length() != 0)) {
						if ((resultsDir != null) && (resultsDir.length() != 0)) {
							try {
								addNRFiles(resultDataset, namedReference, resultsDir, fileList, sourceDataset);
							} catch (Exception e) {
								this.m_zTaskLogger.error(this.m_zRegistry.getString("DatasetHelper.storeNRFileFailed")
										+ " (" + resultDataset + ")", e);

								if (this.m_qSoftFailureHandlingEnabled) {
									throw new Exception(e.toString());
								}
							}
						} else {
							String msg = this.m_zRegistry.getString("DatasetHelper.noTransResults") + "(" + resultDataset
									+ ")";

							this.m_zTaskLogger.error(msg);
							throw new Exception(msg);
						}
					} else {
						String msg = this.m_zRegistry.getString("DatasetHelper.noNamedRefType") + "(" + resultDataset + ")";

						this.m_zTaskLogger.error(msg);
						throw new Exception(msg);
					}

				}

				iCreateInsertStatus = 1;
			} else {
				this.m_zTaskLogger
						.debug("PRIONDataSetHelper ->   translation results dataset found, inserting dataset version...");

				resultDatasetVers = insertDatasetVersion(sourceDataset, resultDataset);
				resultDatasetVer = resultDatasetVers[0];

				if (resultDatasetVers[1] != null) {
					iCreateInsertStatus = 2;
				}

				if (fileList != null) {
					if (fileList.isEmpty()) {
						if (resultDatasetVers[1] != null) {
							this.m_zTaskLogger
									.debug("PRIONDataSetHelper ->   There are no files to attach to the newly inserted dataset version");
						} else {
							this.m_zTaskLogger
									.debug("PRIONDataSetHelper ->   There are no files to attach to the existing dataset version");
						}

					} else if ((namedReference != null) && (namedReference.length() != 0)) {
						if ((resultsDir != null) && (resultsDir.length() != 0)) {
							try {
								ImanFile[] nrFiles = getNRFiles(resultDatasetVer, namedReference);

								if ((this.m_qUpdateExistingVisData) || (nrFiles == null) || (nrFiles.length == 0)) {
									if ((nrFiles != null) && (nrFiles.length > 0)) {
										removeNRFiles(resultDatasetVer, namedReference, nrFiles);
									}
									addNRFiles(resultDatasetVer, namedReference, resultsDir, fileList, sourceDataset);

									iCreateInsertStatus = 3;
								} else {
									this.m_zTaskLogger
											.debug("PRIONDataSetHelper ->   SKIPPED remove/add translation results data for "
													+ resultDataset);
								}
							} catch (Exception e) {
								String msg = this.m_zRegistry.getString("DatasetHelper.storeNRFileFailed") + " ("
										+ resultDataset + ")";

								this.m_zTaskLogger.error(msg, e);

								if (this.m_qSoftFailureHandlingEnabled) {
									throw new Exception(e.toString());
								}
							}
						} else {
							String msg = this.m_zRegistry.getString("DatasetHelper.noTransResults") + " (" + resultDataset
									+ ")";

							this.m_zTaskLogger.error(msg);
							throw new Exception(msg);
						}
					} else {
						String msg = this.m_zRegistry.getString("DatasetHelper.noNamedRefType") + " (" + resultDataset + ")";

						this.m_zTaskLogger.error(msg);
						throw new Exception(msg);
					}
				}
			}

		} catch (Exception e) {
			throw e;
		} finally {
			TeamcenterServerProxy.getInstance().setBypass("false");
		}

		this.m_zTaskLogger.trace("createInsertDataset() end", 0);
		return iCreateInsertStatus;
	}

	@SuppressWarnings("rawtypes")
	public void createInsertDataset(Vector<Vector> vctDatasetsAndRevs, String datasetType, String relationType,
			boolean qOnSourceDataset) throws Exception {
		this.m_zTaskLogger.trace("createInsertDataset() SET begin...", 0);

		Vector vctDatasets = vctDatasetsAndRevs.get(0);
		Vector vctRevs = vctDatasetsAndRevs.get(1);

		if (vctDatasets.size() == vctRevs.size()) {
			int iSize = vctDatasets.size();
			for (int i = 0; i < iSize; i++) {
				createInsertDataset((ItemRevision) vctRevs.get(i), (Dataset) vctDatasets.get(i), datasetType, relationType,
						null, null, null, qOnSourceDataset);
			}

		} else {
			String msg = this.m_zRegistry.getString("DatasetHelper.createDiffSizes");
			this.m_zTaskLogger.error(msg);
			throw new Exception(msg);
		}

		this.m_zTaskLogger.trace("createInsertDataset() end", 0);
	}

	public int addReplaceNR(ItemRevision sourceItemRev, Dataset sourceDataset, String namedReference, String resultsDir,
			List<String> fileList) throws Exception {
		int iCreateInsertStatus = 3;

		this.m_zTaskLogger.trace("addReplaceNR() begin...", 0);

		if (sourceItemRev == null) {
			this.m_zTaskLogger.error("  input param sourceItemRev is null");
			throw new NullPointerException("sourceItemRev");
		}
		if (sourceDataset == null) {
			this.m_zTaskLogger.error("  input param sourceDataset is null");
			throw new NullPointerException("sourceDataset");
		}
		if (namedReference == null) {
			this.m_zTaskLogger.error("  input param namedReference is null");
			throw new NullPointerException("namedReference");
		}
		if (resultsDir == null) {
			this.m_zTaskLogger.error("  input param resultsDir is null");
			throw new NullPointerException("resultsDir");
		}
		if (fileList == null) {
			this.m_zTaskLogger.error("  input param fileList is null");
			throw new NullPointerException("fileList");
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   sourceItemRev    " + sourceItemRev.get_object_string());
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   sourceDataset    " + sourceDataset.get_object_string());
		this.m_zTaskLogger.debug("PRIONDataSetHelper ->   namedReference   " + namedReference);

		if (!fileList.isEmpty()) {
			for (int i = 0; i < fileList.size(); i++) {
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->   fileList[" + i + "]      " + fileList.get(i));
			}

			try {
				TeamcenterServerProxy.getInstance().setBypass("true");
				try {
					ImanFile[] nrFiles = getNRFiles(sourceDataset, namedReference);

					if ((nrFiles == null) || (nrFiles.length == 0) || (this.m_qUpdateExistingVisData)) {
						if ((nrFiles != null) && (nrFiles.length > 0)) {
							removeNRFiles(sourceDataset, namedReference, nrFiles);
						}
						addNRFiles(sourceDataset, namedReference, resultsDir, fileList, sourceDataset);

						iCreateInsertStatus = 3;
					} else {
						this.m_zTaskLogger.debug("PRIONDataSetHelper ->   SKIPPED remove/add translation results data for "
								+ sourceDataset);
					}
				} catch (Exception e) {
					this.m_zTaskLogger.error(this.m_zRegistry.getString("DatasetHelper.storeNRFileFailed") + " ("
							+ sourceDataset + ") ", e);

					if (this.m_qSoftFailureHandlingEnabled) {
						throw new Exception(e.toString());
					}
				}
			} catch (Exception e) {
				throw e;
			} finally {
				TeamcenterServerProxy.getInstance().setBypass("false");
			}
		} else {
			this.m_zTaskLogger
					.debug("PRIONDataSetHelper ->   There are no files to add/replace for the source dataset version");
		}

		this.m_zTaskLogger.trace("addReplaceNR() end", 0);
		return iCreateInsertStatus;
	}

	@SuppressWarnings("deprecation")
	protected void addNRFiles(Dataset dataset, String namedReference, String resultsDir, List<String> fileList,
			Dataset sourceDataset) throws Exception {
		this.m_zTaskLogger.trace("  addNRFiles() begin...", 0);

		String[] namedRefList = null;
		String[] refFormatList = null;
		String[] refTemplateList = null;

		dataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(dataset, "dataset_type");
		DatasetType zDsType = dataset.get_dataset_type();
		zDsType = (DatasetType) TeamcenterServerProxy.getInstance().getProperties(zDsType, "datasettype_name");
		String scDatasetType = zDsType.get_datasettype_name();

		com.teamcenter.services.strong.core._2007_06.DataManagement.DatasetTypeInfo[] zInfos = TeamcenterServerProxy
				.getInstance().getDatasetTypeInfo(scDatasetType);

		com.teamcenter.services.strong.core._2007_06.DataManagement.ReferenceInfo[] listOfNamedRefContext = null;
		if (zInfos.length > 0) {
			listOfNamedRefContext = zInfos[0].refInfos;
		}

		if (listOfNamedRefContext != null) {
			namedRefList = new String[listOfNamedRefContext.length];
			refFormatList = new String[listOfNamedRefContext.length];
			refTemplateList = new String[listOfNamedRefContext.length];

			for (int i = 0; i < listOfNamedRefContext.length; i++) {
				namedRefList[i] = listOfNamedRefContext[i].referenceName;
				refFormatList[i] = listOfNamedRefContext[i].fileFormat;
				refTemplateList[i] = listOfNamedRefContext[i].fileExtension;
			}
		}

		int iCount = fileList.size();
		String[] filePathnames = new String[iCount];
		String[] fileTypes = new String[iCount];
		String[] namedReferences = new String[iCount];
		for (int i = 0; i < iCount; i++) {
			String visFileName = fileList.get(i);
			if (visFileName.regionMatches(0, File.separator, 0, 1)) {
				filePathnames[i] = (resultsDir + visFileName);
			} else {
				filePathnames[i] = (resultsDir + File.separator + visFileName);
			}

			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     filePathnames[" + i + "] = " + filePathnames[i]);

			String incomingFileFormat = "BINARY";

			String incomingExtension = visFileName.substring(visFileName.lastIndexOf("."));

			if (listOfNamedRefContext != null) {
				for (int j = 0; j < listOfNamedRefContext.length; j++) {
					int iIndex = refTemplateList[j].lastIndexOf(".");
					if (iIndex <= 0)
						continue;
					String namedReferenceExtension = refTemplateList[j].substring(iIndex);

					if (!incomingExtension.equalsIgnoreCase(namedReferenceExtension))
						continue;
					incomingFileFormat = refFormatList[j];
					break;
				}

			}

			fileTypes[i] = incomingFileFormat;

			namedReferences[i] = namedReference;
		}

		if (this.m_qStoreInSrcVol) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     Storing files into source data volume");
			setTCSessionVolume(sourceDataset);
		}

		try {
			dataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(dataset, "revision_number");
			int version = dataset.get_revision_number();
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     setting files on " + dataset.get_object_string() + ";"
					+ version + "...");
			FileManagement.DatasetFileInfo[] azDFI = new FileManagement.DatasetFileInfo[filePathnames.length];
			for (int i = 0; i < filePathnames.length; i++) {
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->    filePathnames[" + i + "]    " + filePathnames[i]);
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->    fileTypes[" + i + "]        " + fileTypes[i]);
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->    namedReferences[" + i + "]  " + namedReferences[i]);
				azDFI[i] = new FileManagement.DatasetFileInfo();
				azDFI[i].allowReplace = this.m_qUpdateExistingVisData;
				azDFI[i].fileName = filePathnames[i];
				if (fileTypes[i].equalsIgnoreCase("BINARY")) {
					azDFI[i].isText = false;
				} else {
					azDFI[i].isText = true;
				}
				azDFI[i].namedReferencedName = namedReferences[i];
			}

			FileManagement.GetDatasetWriteTicketsInputData[] arg0 = new FileManagement.GetDatasetWriteTicketsInputData[1];
			arg0[0] = new FileManagement.GetDatasetWriteTicketsInputData();
			arg0[0].createNewVersion = false;
			arg0[0].dataset = dataset;
			arg0[0].datasetFileInfos = azDFI;

			TeamcenterServerProxy.getInstance().getFileMgtUtil().putFiles(arg0);
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     Files attached to dataset through named references");
		} catch (Exception e) {
			this.m_zTaskLogger.error("addNRFiles Exception - " + e.toString(), e);
			throw e;
		} finally {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Restoring volume to default");
			restoreTCSessionVolume();
		}

		this.m_zTaskLogger.trace("  addNRFiles() end", 0);
	}

	@SuppressWarnings({ "unused", "unchecked", "rawtypes", "deprecation" })
	public ImanFile[] getNRFiles(Dataset dataset, String namedReference) throws Exception {
		dataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(dataset, "ref_list");
		this.m_zTaskLogger.trace("  getNRFiles() " + dataset.get_object_string() + " " + namedReference + " begin...", 0);

		ArrayList nrFiles = new ArrayList();
		try {
			ModelObject[] comps = getNRObjs(dataset, namedReference);
			if ((comps != null) && (comps.length > 0)) {
				for (int i = 0; i < comps.length; i++) {
					this.m_zTaskLogger.debug("PRIONDataSetHelper ->     comps[" + i + "] is an "
							+ comps[i].getTypeObject().getName());
					if (!(comps[i] instanceof ImanFile))
						continue;
					nrFiles.add(comps[i]);
				}
			}

		} catch (Exception e) {
			this.m_zTaskLogger.error("    getNRFiles Exception - " + e.toString(), e);
			throw e;
		}

		if (nrFiles != null)
			this.m_zTaskLogger.trace("  getNRFiles() end, returning " + nrFiles.size() + " files", 0);
		else {
			this.m_zTaskLogger.trace("  getNRFiles() end, returning no files", 0);
		}
		return (ImanFile[]) nrFiles.toArray(new ImanFile[nrFiles.size()]);
	}

	@SuppressWarnings({ "rawtypes", "unchecked", "unused", "deprecation" })
	public ModelObject[] getNRObjs(Dataset dataset, String namedReference) throws Exception {
		this.m_zTaskLogger.trace("  getNRObjs() " + namedReference + " begin...", 0);

		ArrayList nrObjs = new ArrayList();
		try {
			dataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(dataset, "ref_names");
			String[] comps = dataset.get_ref_names();
			ModelObject[] objs = dataset.get_ref_list();
			if ((comps != null) && (comps.length > 0)) {
				for (int i = 0; i < comps.length; i++) {
					this.m_zTaskLogger.debug("PRIONDataSetHelper ->     comps[" + i + "] is an " + comps[i]);
					if (!comps[i].equalsIgnoreCase(namedReference))
						continue;
					nrObjs.add(objs[i]);
				}
			}

		} catch (Exception e) {
			this.m_zTaskLogger.error("  getNRObjs Exception - " + e.toString(), e);
			throw e;
		}

		if (nrObjs != null)
			this.m_zTaskLogger.trace("    getNRObjs() end, returning " + nrObjs.size() + " files", 0);
		else {
			this.m_zTaskLogger.trace("    getNRObjs() end, returning no files", 0);
		}
		return (ModelObject[]) nrObjs.toArray(new ModelObject[nrObjs.size()]);
	}

	protected void removeNRFiles(Dataset dataset, String namedReference, ImanFile[] files) throws Exception {
		this.m_zTaskLogger.trace("  removeNRFiles() begin...", 0);
		try {
			DataManagementService dmService = DataManagementService.getService(ConnectionManager.getActiveConnection());

			com.teamcenter.services.strong.core._2007_09.DataManagement.RemoveNamedReferenceFromDatasetInfo[] azArg = new com.teamcenter.services.strong.core._2007_09.DataManagement.RemoveNamedReferenceFromDatasetInfo[1];
			azArg[0] = new com.teamcenter.services.strong.core._2007_09.DataManagement.RemoveNamedReferenceFromDatasetInfo();
			azArg[0].dataset = dataset;
			com.teamcenter.services.strong.core._2007_09.DataManagement.NamedReferenceInfo[] azNRInfo = new com.teamcenter.services.strong.core._2007_09.DataManagement.NamedReferenceInfo[1];
			azNRInfo[0] = new com.teamcenter.services.strong.core._2007_09.DataManagement.NamedReferenceInfo();
			azNRInfo[0].deleteTarget = true;
			azNRInfo[0].type = namedReference;
			azArg[0].nrInfo = azNRInfo;
			dmService.removeNamedReferenceFromDataset(azArg);

			this.m_zTaskLogger.debug("PRIONDataSetHelper ->     remove named reference " + namedReference);
		} catch (Exception e) {
			this.m_zTaskLogger.error("    removeNRFiles Exception - " + e.toString(), e);
			throw e;
		}

		this.m_zTaskLogger.trace("  removeNRFiles() end", 0);
	}

	@SuppressWarnings("deprecation")
	protected void setTCSessionVolume(Dataset dataset) throws Exception {
		this.m_zTaskLogger.trace("    setTCSessionVolume() begin...", 0);

		this.m_zTempVol = null;

		User zUser = ConnectionManager.getLoggedInUser();
		zUser = (User) TeamcenterServerProxy.getInstance().getProperties(zUser, "volume");
		ImanVolume lVol = ConnectionManager.getLoggedInUser().get_volume();

		if (lVol != null) {
			lVol = (ImanVolume) TeamcenterServerProxy.getInstance().getProperties(lVol, "volume_name");
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->       vol = " + lVol.get_volume_name());

			dataset = (Dataset) TeamcenterServerProxy.getInstance().getProperties(dataset, "ref_list");
			ModelObject[] files = dataset.get_ref_list();
			for (ModelObject modelObject : files) {
				if (!(modelObject instanceof ImanFile))
					continue;
				modelObject = TeamcenterServerProxy.getInstance().getProperties(modelObject, "volume_tag");
				ImanVolume userVol = ((ImanFile) modelObject).get_volume_tag();
				userVol = (ImanVolume) TeamcenterServerProxy.getInstance().getProperties(userVol, "volume_name");

				TeamcenterServerProxy.getInstance().setSessionVol(userVol);

				this.m_zTempVol = lVol;

				this.m_zTaskLogger.debug("PRIONDataSetHelper ->       set " + userVol.get_volume_name());
				break;
			}

			if (this.m_zTempVol == null) {
				this.m_zTaskLogger.debug("PRIONDataSetHelper ->       NO SESSION VOLUME");
			}
		}

		this.m_zTaskLogger.trace("    setTCSessionVolume() end", 0);
	}

	protected void restoreTCSessionVolume() throws Exception {
		this.m_zTaskLogger.trace("    restoreTCSessionVolume() begin...", 0);

		if (this.m_zTempVol != null) {
			TeamcenterServerProxy.getInstance().setSessionVol(this.m_zTempVol);
			this.m_zTaskLogger.debug("PRIONDataSetHelper ->       restored " + this.m_zTempVol.get_volume_name());
			this.m_zTempVol = null;
		}
		this.m_zTaskLogger.trace("    restoreTCSessionVolume() end", 0);
	}

	@SuppressWarnings({ "rawtypes", "unchecked", "deprecation" })
	public Dataset[] getDatasets(ItemRevision itemRevObj, String datasetType, String relation) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getDatasets...");

		com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2[] relationFilters = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2[1];
		relationFilters[0] = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter2();
		relationFilters[0].relationName = relation;
		ModelObject[] childComps = TeamcenterServerProxy.getInstance().getChildrenPrimary(itemRevObj, relationFilters);

		Vector foundList = new Vector();

		if ((childComps != null) && (childComps.length > 0)) {
			for (int i = 0; i < childComps.length; i++) {
				if (!(childComps[i] instanceof Dataset))
					continue;
				Dataset childDataset = (Dataset) childComps[i];
				TeamcenterServerProxy.getInstance().getProperties(childDataset,
						new String[] { "dataset_type", "object_name" });

				DatasetType zDsType = childDataset.get_dataset_type();
				TeamcenterServerProxy.getInstance().getProperties(zDsType, "datasettype_name");

				String dsType = zDsType.get_datasettype_name();

				if (!dsType.equals(datasetType)) {
					continue;
				}
				foundList.add(childDataset);

				this.m_zTaskLogger.debug("PRIONDataSetHelper -> Added dataset to list: " + childDataset.toString());
			}

		}

		Dataset[] retArray = new Dataset[foundList.size()];

		foundList.copyInto(retArray);

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> Returning an array of length: " + retArray.length);

		return retArray;
	}

	public String getRelationType(ItemRevision itemRev, Dataset dataset) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getRelationType...");

		DataManagementService dmService = DataManagementService.getService(ConnectionManager.getActiveConnection());

		Dataset baseDataset = getBaseDataset(dataset);

		com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter relationTypeFilter = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter();

		relationTypeFilter.relationTypeName = "";

		com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsPref2 prefs = new com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsPref2();

		prefs.expItemRev = true;
		prefs.returnRelations = true;
		prefs.info = new com.teamcenter.services.strong.core._2007_06.DataManagement.RelationAndTypesFilter[1];
		prefs.info[0] = relationTypeFilter;

		com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsResponse2 response = dmService
				.expandGRMRelationsForPrimary(new ModelObject[] { itemRev }, prefs);

		if ((response.output == null) || (response.output.length != 1)) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Unexpected return from expandGRMRelationsForPrimary");
			throw new Exception("Unexpected return from expandGRMRelationsForPrimary");
		}

		for (com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationsData2 availableRelation : response.output[0].relationshipData) {
			for (com.teamcenter.services.strong.core._2007_09.DataManagement.ExpandGRMRelationship relation : availableRelation.relationshipObjects) {
				if (!(relation.otherSideObject instanceof Dataset)) {
					continue;
				}
				if (!baseDataset.equals(getBaseDataset((Dataset) relation.otherSideObject))) {
					continue;
				}
				Type type = relation.relation.getTypeObject();

				String typeName = type.getName();

				this.m_zTaskLogger.debug("PRIONDataSetHelper -> Relation Type returned: " + typeName);

				return typeName;
			}

		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> Relation Type returned: null");

		return null;
	}

	public Dataset getBaseDataset(Dataset dataset) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getBaseDataset...");

		ModelObject[] datasets = getAllRevisions(dataset);

		if ((datasets == null) || (datasets.length == 0)) {
			throw new Exception("getAllRevisions() did not return anything");
		}

		if (!(datasets[0] instanceof Dataset)) {
			throw new Exception("The returned ModelObjects from getAllRevisions() is not datasets.");
		}

		return (Dataset) datasets[0];
	}

	public String getReferenceName(Dataset dataset, String template) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getReferenceName...");

		DataManagementService dmService = DataManagementService.getService(ConnectionManager.getActiveConnection());

		String[] datasetTypes = { dataset.getTypeObject().getName() };
		com.teamcenter.services.strong.core._2007_06.DataManagement.DatasetTypeInfoResponse resp = dmService
				.getDatasetTypeInfo(datasetTypes);

		if (resp.infos.length != 1) {
			String message = "Invalid responce from getDatasetTypeInfo for dataset type: " + datasetTypes[0];

			this.m_zTaskLogger.debug(message);
			throw new Exception(message);
		}

		for (com.teamcenter.services.strong.core._2007_06.DataManagement.ReferenceInfo refInfo : resp.infos[0].refInfos) {
			String currentTemplate = refInfo.fileExtension;

			if ((currentTemplate != null) && (currentTemplate.equalsIgnoreCase(template))) {
				this.m_zTaskLogger.debug("PRIONDataSetHelper -> Found the file template: " + currentTemplate);

				return refInfo.referenceName;
			}

			this.m_zTaskLogger.debug("PRIONDataSetHelper -> No match on the currnet template: " + currentTemplate);
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> The template was not found: " + template);

		return null;
	}

	@SuppressWarnings("deprecation")
	public String getOriginalFilename(Dataset dataset, String namedReference) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getFilename...");

		TeamcenterServerProxy.getInstance().getProperties(dataset, new String[] { "ref_names", "ref_list" });

		ModelObject[] refList = dataset.get_ref_list();
		String[] refNames = dataset.get_ref_names();

		for (int i = 0; (refNames != null) && (i < refNames.length); i++) {
			if ((!refNames[i].equals(namedReference)) || (refList == null) || (refList.length <= i)
					|| (!(refList[i] instanceof ImanFile))) {
				continue;
			}
			ImanFile fileObject = (ImanFile) refList[i];

			TeamcenterServerProxy.getInstance().getProperties(fileObject, "original_file_name");

			String originalFileName = fileObject.get_original_file_name();

			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Found the original filname: " + originalFileName
					+ " for the namedReference: " + namedReference);

			return originalFileName;
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> No file named found for the named reference: " + namedReference);

		return null;
	}

	@SuppressWarnings("deprecation")
	public String[] getDatasetReleaseStatusList(Dataset dataset) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getDatasetReleaseStatusList...");

		TeamcenterServerProxy.getInstance().getProperties(dataset, "release_status_list");
		ReleaseStatus[] list = dataset.get_release_status_list();

		if ((list == null) || (list.length == 0)) {
			this.m_zTaskLogger.debug("PRIONDataSetHelper -> No release status found on the dataset.");

			return null;
		}

		TeamcenterServerProxy.getInstance().getProperties(list, "name");

		String[] outputData = new String[list.length];

		for (int i = 0; i < list.length; i++) {
			outputData[i] = list[i].get_name();

			this.m_zTaskLogger.debug("PRIONDataSetHelper -> Found release status :" + outputData[i]);
		}

		this.m_zTaskLogger.debug("PRIONDataSetHelper -> ...leaving getDatasetReleaseStatusList");

		return outputData;
	}

	public String getDatasetReleaseStatus(Dataset dataset, String delimeter) throws Exception {
		this.m_zTaskLogger.debug("PRIONDataSetHelper -> In getDatasetReleaseStatus...");

		String[] releaseStatusList = getDatasetReleaseStatusList(dataset);

		String retString = "";

		if ((releaseStatusList != null) && (releaseStatusList.length > 0)) {
			for (String statusName : releaseStatusList) {
				if (retString.length() > 0) {
					retString = retString + delimeter;
				}

				retString = retString + statusName;
			}

		}

		return retString;
	}

	public static boolean isLocallyOwned(ModelObject zComponent) throws Exception {
		boolean isLocallyOwned = true;

		ModelObject site = zComponent.getPropertyObject("owning_site").getModelObjectValue();
		if (site != null) {
			isLocallyOwned = false;
		}

		return isLocallyOwned;
	}

	public static String getFileExtension(String filename) {
		if (filename == null) {
			return null;
		}

		int index = filename.lastIndexOf('.');

		return index != -1 ? filename.substring(index) : "";
	}

	public static boolean isFilenameOnly(String filename) {
		if ((filename == null) || (filename.length() == 0)) {
			return false;
		}

		File validateFile = new File(filename);

		return (validateFile.getParent() == null) && (validateFile.getName() != null);
	}
}