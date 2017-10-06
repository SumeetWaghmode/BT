package com.accenture.rac.wizards.importer.spec;

/**
 * This interface is used declare if an object has the flag implicit implemented. <br />
 * The implicit flag can be toggled by setimplicit(boolean implicit) 
 */
public interface IImplicit {
	
	/**
	 * Declared if an object is implicit or not
	 * @return boolean
	 */
	public boolean isImplicit();
	
	/**
	 * Set the object status implicit to true or false
	 * @param implicit boolean
	 */
	public void setImplicit (boolean implicit);

}
