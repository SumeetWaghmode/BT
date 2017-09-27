package com.accenture.rac.wizards.importer.spec;

import java.io.Serializable;

/**
 * The Interface IRow.
 */
public interface IRow extends Serializable, IEnabled, IStatus{
	
	/**
	 * Returns the unique qualifier of the FileRow
	 * @return String
	 */
	public String getId();
	
	/**
	 * Compares the unique qualifiers of the FileRow
	 * @param IFileRow obj
	 * @return boolean
	 */
	public boolean equalId(Object obj);
	
	

}
