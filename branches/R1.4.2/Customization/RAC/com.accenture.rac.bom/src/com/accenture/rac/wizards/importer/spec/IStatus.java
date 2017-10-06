package com.accenture.rac.wizards.importer.spec;

import com.accenture.rac.wizards.importer.RowStatus;

/**
 * This interface declares if an object has an {@link RowStatus}. <br />
 * Additional to the {@link RowStatus}, there is also a description which is the verbose explaination to the status.
 * One status can have n descriptions. I.e. for the {@link RowStatus.ERROR}
 *
 */
public interface IStatus {
	public RowStatus getStatus();
	public void setStatus(RowStatus status);
	
	public String getStatusDescription();
	public void setStatusDescription(String statusDescription);
}
