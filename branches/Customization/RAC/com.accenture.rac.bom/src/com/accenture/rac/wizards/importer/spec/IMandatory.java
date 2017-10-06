package com.accenture.rac.wizards.importer.spec;

/**
 * This interface is used declare if an object has the flag mandatory implemented. <br />
 * The mandatory flag can be toggled by setMandatory(boolean mandatory) 
 */
public interface IMandatory {

	/**
	 * Declared if an object is mandatory or not
	 * @return boolean
	 */
	public abstract boolean isMandatory();

	/**
	 * Set the object mandatory to true or false
	 * @param mandatory boolean
	 */
	public abstract void setMandatory(boolean mandatory);

}