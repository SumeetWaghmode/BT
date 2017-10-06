/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.accenture.dispatcher.report_bom.ets.util;

/**
 * 
 * @author ftorres
 */
public class PRIONTaskConfBean {
	private String taskID;
	private String transExec;
	private String transExecDir;
	private String datasetName;
	private String fileName;
	private String itemId;
	private String itemRevId;
	private String reportType;
	private String revisionRule;
	private String reportFormat;
	private String userName;
	private String stagingLoc;
	private String maxBOMLevel;
	private String equalPart = "-";
	private String ordering = "-";
	private String includePCB = "false";
	private String includeSUP = "false";
	private String onlySMD = "-";
	private String reportFilename = "";

	public PRIONTaskConfBean() {
		clean();
	}

	public PRIONTaskConfBean(PRIONTaskConfBean x) {
		this.taskID = new String(x.taskID);
		this.transExec = new String(x.transExec);
		this.transExecDir = new String(x.transExecDir);
		this.datasetName = new String(x.datasetName);
		this.fileName = new String(x.fileName);
		this.itemId = new String(x.itemId);
		this.itemRevId = new String(x.itemRevId);
		this.reportType = new String(x.reportType);
		this.revisionRule = new String(x.revisionRule);
		this.reportFormat = new String(x.reportFormat);
		this.userName = new String(x.reportFormat);
		this.stagingLoc = new String(x.stagingLoc);
		this.maxBOMLevel = new String(x.maxBOMLevel);
		this.equalPart = new String(x.equalPart);
		this.ordering = new String(x.ordering);
		this.includePCB = new String(x.includePCB);
		this.includeSUP = new String(x.includeSUP);
		this.onlySMD = new String(x.onlySMD);
		this.reportFilename = new String(x.reportFilename);
	}

	public void setTaskID(String x) {
		if (x == null)
			x = "";
		this.taskID = x;
	}

	public void setTransExec(String x) {
		if (x == null)
			x = "";
		this.transExec = x;
	}

	public void setTransExecDir(String x) {
		if (x == null)
			x = "";
		this.transExecDir = x;
	}

	public void setDatasetName(String x) {
		if (x == null)
			x = "";
		this.datasetName = x;
	}

	public void setFileName(String x) {
		if (x == null)
			x = "";
		this.fileName = x;
	}

	public void setItemId(String x) {
		if (x == null)
			x = "";
		this.itemId = x;
	}

	public void setItemRevId(String x) {
		if (x == null)
			x = "";
		this.itemRevId = x;
	}

	public void setReportType(String x) {
		if (x == null)
			x = "";
		this.reportType = x;
	}

	public void setRevisionRule(String x) {
		if (x == null)
			x = "";
		this.revisionRule = x;
	}

	public void setReportFormat(String x) {
		if (x == null)
			x = "";
		this.reportFormat = x;
	}

	public void setUserName(String x) {
		if (x == null)
			x = "";
		this.userName = x;
	}

	public void setStagingLoc(String x) {
		if (x == null)
			x = "";
		this.stagingLoc = x;
	}

	public void setMaxBOMLevel(String x) {
		if (x == null)
			x = "";
		this.maxBOMLevel = x;
	}

	public void setEqualPart(String x) {
		if (x == null)
			x = "-";
		this.equalPart = x;
	}

	public void setOrdering(String x) {
		if (x == null)
			x = "-";
		this.ordering = x;
	}

	public void setIncludePCB(String x) {
		if (x == null)
			x = "false";
		this.includePCB = x;
	}

	public void setIncludeSUP(String x) {
		if (x == null)
			x = "false";
		this.includeSUP = x;
	}
	
	public void setOnlySMD(String x) {
		if (x == null)
			x = "-";
		this.onlySMD = x;
	}

	public void setReportFilename(String x) {
		if (x == null)
			x = "";
		this.reportFilename = x;
	}

	public String getTraskID() {
		return this.taskID;
	}

	public String getTransExec() {
		return this.transExec;
	}

	public String getTransExecDir() {
		return this.transExecDir;
	}

	public String getDatasetName() {
		return this.datasetName;
	}

	public String getFileName() {
		return this.fileName;
	}

	public String getItemId() {
		return this.itemId;
	}

	public String getItemRevId() {
		return this.itemRevId;
	}

	public String getReportType() {
		return this.reportType;
	}

	public String getRevisionRule() {
		return this.revisionRule;
	}

	public String getReportFormat() {
		return this.reportFormat;
	}

	public String getUserName() {
		return this.userName;
	}

	public String getStagingLoc() {
		return this.stagingLoc;
	}

	public String getMaxBOMLevel() {
		return this.maxBOMLevel;
	}

	public String getEqualPart() {
		return this.equalPart;
	}

	public String getOrdering() {
		return this.ordering;
	}

	public String getIncludePCB() {
		return this.includePCB;
	}
	
	public String getIncludeSUP() {
		return this.includeSUP;
	}

	public String getOnlySMD() {
		return this.onlySMD;
	}

	public String getReportFilename() {
		return this.reportFilename;
	}

	public void clean() {
		taskID = "";
		transExec = "";
		transExecDir = "";
		datasetName = "";
		fileName = "";
		itemId = "";
		itemRevId = "";
		reportType = "";
		revisionRule = "";
		userName = "";
		stagingLoc = "";
		maxBOMLevel = "";
		equalPart = "-";
		ordering = "-";
		includePCB = "false";
		includeSUP = "false";
		onlySMD = "-";
		reportFilename = "";
	}
}
