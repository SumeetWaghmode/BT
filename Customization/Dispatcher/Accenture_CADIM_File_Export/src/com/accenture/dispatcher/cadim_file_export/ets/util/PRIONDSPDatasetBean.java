/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.accenture.dispatcher.cadim_file_export.ets.util;

/**
 * 
 * @author ftorres
 */
public class PRIONDSPDatasetBean {
	private String finalFileName;

	private String dataSetType;
	private String transExecutableName;
	private String transExecutableDir;
	private String sourceFileExtension;
	private String sourceFileLocation;
	private String sourceFileRelationWithSon;
	private String translatedDatasetType;
	private String translatedDatasetRelationWithIR;
	private String translatedFileExtension;
	private String translatedFileReferenceType;
	private String dsNamePrefix;
	private String dsNameSufix;
	private String dsName;
	private String fileNamePrefix;
	private String fileNameSufix;
	private String fileName;

	public PRIONDSPDatasetBean() {
		clean();
	}

	public PRIONDSPDatasetBean(PRIONDSPDatasetBean x) {
		this.finalFileName = new String(x.finalFileName);

		this.dataSetType = new String(x.dataSetType);
		this.transExecutableName = new String(x.transExecutableName);
		this.transExecutableDir = new String(x.transExecutableDir);
		this.sourceFileExtension = new String(x.sourceFileExtension);
		this.sourceFileLocation = new String(x.sourceFileLocation);
		this.sourceFileRelationWithSon = new String(x.sourceFileRelationWithSon);
		this.translatedDatasetType = new String(x.translatedDatasetType);
		this.translatedDatasetRelationWithIR = new String(
				x.translatedDatasetRelationWithIR);
		this.translatedFileExtension = new String(x.translatedFileExtension);
		this.translatedFileReferenceType = new String(
				x.translatedFileReferenceType);
		this.dsNamePrefix = new String(x.dsNamePrefix);
		this.dsNameSufix = new String(x.dsNameSufix);
		this.dsName = new String(x.dsName);
		this.fileNamePrefix = new String(x.fileNamePrefix);
		this.fileNameSufix = new String(x.fileNameSufix);
		this.fileName = new String(x.fileName);
	}

	public void setFinalFileName(String x) {
		if (x == null)
			x = "";
		this.finalFileName = x;
	}

	public void setDataSetType(String x) {
		if (x == null)
			x = "";
		this.dataSetType = x;
	}

	public void setTransExecutableName(String x) {
		if (x == null)
			x = "";
		this.transExecutableName = x;
	}

	public void setTransExecutableDir(String x) {
		if (x == null)
			x = "";
		this.transExecutableDir = x;
	}

	public void setSourceFileExtension(String x) {
		if (x == null)
			x = "";
		this.sourceFileExtension = x;
	}

	public void setSourceFileLocation(String x) {
		if (x == null)
			x = "";
		this.sourceFileLocation = x;
	}

	public void setSourceFileRelationWithSon(String x) {
		if (x == null)
			x = "";
		this.sourceFileRelationWithSon = x;
	}

	public void setTranslatedDatasetType(String x) {
		if (x == null)
			x = "";
		this.translatedDatasetType = x;
	}

	public void setTranslatedDatasetRelationWithIR(String x) {
		if (x == null)
			x = "";
		this.translatedDatasetRelationWithIR = x;
	}

	public void setTranslatedFileExtension(String x) {
		if (x == null)
			x = "";
		this.translatedFileExtension = x;
	}

	public void setTranslatedFileReferenceType(String x) {
		if (x == null)
			x = "";
		this.translatedFileReferenceType = x;
	}

	public void setDsNamePrefix(String x) {
		if (x == null)
			x = "";
		this.dsNamePrefix = x;
	}

	public void setDsNameSufix(String x) {
		if (x == null)
			x = "";
		this.dsNameSufix = x;
	}

	public void setDsName(String x) {
		if (x == null)
			x = "";
		this.dsName = x;
	}

	public void setFileNamePrefix(String x) {
		if (x == null)
			x = "";
		this.fileNamePrefix = x;
	}

	public void setFileNameSufix(String x) {
		if (x == null)
			x = "";
		this.fileNameSufix = x;
	}

	public void setFileName(String x) {
		if (x == null)
			x = "";
		this.fileName = x;
	}

	public String getFinalFileName() {
		return this.finalFileName;
	}

	public String getDataSetType() {
		return this.dataSetType;
	}

	public String getTransExecutableName() {
		return this.transExecutableName;
	}

	public String getTransExecutableDir() {
		return this.transExecutableDir;
	}

	public String getSourceFileExtension() {
		return this.sourceFileExtension;
	}

	public String getSourceFileLocation() {
		return this.sourceFileLocation;
	}

	public String getSourceFileRelationWithSon() {
		return this.sourceFileRelationWithSon;
	}

	public String getTranslatedDatasetType() {
		return this.translatedDatasetType;
	}

	public String getTranslatedDatasetRelationWithIR() {
		return this.translatedDatasetRelationWithIR;
	}

	public String getTranslatedFileExtension() {
		return this.translatedFileExtension;
	}

	public String getTranslatedFileReferenceType() {
		return this.translatedFileReferenceType;
	}

	public String getDsNamePrefix() {
		return this.dsNamePrefix;
	}

	public String getDsNameSufix() {
		return this.dsNameSufix;
	}

	public String getDsName() {
		return this.dsName;
	}

	public String getFileNamePrefix() {
		return this.fileNamePrefix;
	}

	public String getFileNameSufix() {
		return this.fileNameSufix;
	}

	public String getFileName() {
		return this.fileName;
	}

	public void clean() {
		finalFileName = "";

		dataSetType = "";
		transExecutableName = "";
		transExecutableDir = "";
		sourceFileExtension = "";
		sourceFileLocation = "";
		sourceFileRelationWithSon = "";
		translatedDatasetType = "";
		translatedDatasetRelationWithIR = "";
		translatedFileExtension = "";
		translatedFileReferenceType = "";
		dsNamePrefix = "";
		dsNameSufix = "";
		dsName = "";
		fileNamePrefix = "";
		fileNameSufix = "";
		fileName = "";
	}
}
