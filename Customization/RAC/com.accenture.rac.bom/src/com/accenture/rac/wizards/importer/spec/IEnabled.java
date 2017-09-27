package com.accenture.rac.wizards.importer.spec;

/**
 * This interface is used declare if an object has the flag enabled implemented. <br />
 * The enabled flag can be toggled by setEnabled(boolean enabled) 
 */
public interface IEnabled {
	
	/**
	 * Declared if an object is enabled or not
	 * @return boolean
	 */
	public boolean isEnabled();
	
	/**
	 * Set the object status enabled to true or false
	 * @param enabled boolean
	 */
	public void setEnabled(boolean enabled);

}
