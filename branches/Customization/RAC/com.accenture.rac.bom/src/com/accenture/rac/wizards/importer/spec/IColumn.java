package com.accenture.rac.wizards.importer.spec;


// TODO: Auto-generated Javadoc
/**
 * The Interface IColumn.
 */
public interface IColumn extends IEnabled, IMandatory, IImplicit{
	
	/**
	 * Checks for map.
	 *
	 * @param name the name
	 * @return true, if successful
	 */
	boolean hasMap(String name);

	/**
	 * Gets the label.
	 *
	 * @return the label
	 */
	String getLabel();

	/**
	 * Sets the label.
	 *
	 * @param label the new label
	 */
	void setLabel(String label);

	/**
	 * Gets the property name.
	 *
	 * @return the property name
	 */
	String getPropertyName();

	/**
	 * Sets the property name.
	 *
	 * @param propertyName the new property name
	 */
	void setPropertyName(String propertyName);

	/**
	 * Gets the mapping.
	 *
	 * @return the mapping
	 */
	String[] getMapping();

	/**
	 * Sets the mapping.
	 *
	 * @param mapping the new mapping
	 */
	void setMapping(String[] mapping);

	/**
	 * Gets the ext name.
	 *
	 * @return the ext name
	 */
	String getExtName();

	/**
	 * Sets the ext name.
	 *
	 * @param extName the new ext name
	 */
	void setExtName(String extName);

	/**
	 * Gets the int name.
	 *
	 * @return the int name
	 */
	String getIntName();

	/**
	 * Sets the int name.
	 *
	 * @param intName the new int name
	 */
	void setIntName(String intName);
	
	/**
	 * Gets the cell processor.
	 *
	 * @return the cell processor
	 */
	ICellProcessor getCellProcessor();
	
	/**
	 * Sets the cell processor.
	 *
	 * @param processor the new cell processor
	 */
	void setCellProcessor(ICellProcessor processor);
}
