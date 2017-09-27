/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.accenture.dispatcher.cadim_file_export.ets.util;

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

	private String fileUID;
	private String cadimEntryUID;

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
		this.fileUID = new String(x.fileUID);
		this.cadimEntryUID = new String(x.cadimEntryUID);
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

	public void setFileUID(String x) {
		if (x == null)
			x = "";
		this.fileUID = x;
	}

	public void setCadimEntryUID(String x) {
		if (x == null)
			x = "";
		this.cadimEntryUID = x;
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

	public String getFileUID() {
		return this.fileUID;
	}

	public String getCadimEntryUID() {
		return this.cadimEntryUID;
	}

	public void clean() {
		taskID = "";
		transExec = "";
		transExecDir = "";
		datasetName = "";
		fileName = "";
		itemId = "";
		itemRevId = "";
		fileUID = "";
		cadimEntryUID = "";
	}
}
