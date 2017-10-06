package com.accenture.rac.wizards.importer;

import java.util.Arrays;

import com.accenture.rac.wizards.importer.spec.ICellProcessor;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IImportProvider;


public class Column implements  IColumn{
	
	private String label;
	private String propertyName;
	private String extName;
	private String intName;
	private String[] mapping;
	private boolean enabled;
	private boolean mandatory;
	private boolean implicit;
	private ICellProcessor cellProcessor;
	
	/**
	 * Creates a column object for the {@link IImportProvider}.addColumn({@link IColumn});
	 * @param label String
	 * @param propertyName String bean propertyName
	 * @param mapping Array of valid header names
	 * @param enabled Boolean if the column is enabled
	 * @param mandatory Boolean if the column is mandatory for the import
	 */
	public Column(String label, String propertyName, String intName, String[] mapping, ICellProcessor processor, boolean enabled, boolean mandatory) {
		super();
		this.label = label;
		this.propertyName = propertyName;
		this.intName = intName;
		this.mapping = mapping;
		this.enabled = enabled;
		this.mandatory = mandatory;
		this.cellProcessor = processor;
	}
	/**
	 * Constructor without processor
	 * @param label
	 * @param propertyName
	 * @param intName
	 * @param mapping
	 * @param enabled
	 * @param mandatory
	 */
	public Column(String label, String propertyName, String intName, String[] mapping, boolean enabled, boolean mandatory) {
		this(label, propertyName, intName, mapping, null, enabled, mandatory);
	}
	
	public Column(String label, String propertyName, String intName, boolean implicit){
		this(label, propertyName, intName, null, null, false, false);
		this.implicit = implicit;
	}
	
	public Column(String label, String propertyName) {
		this(label, propertyName, null, null, null, false, false);
	}

	@Override
	public boolean hasMap(String name){
		if (mapping == null)
			return false; 
		
		for (int i = 0; i < mapping.length; i++) {
			if(mapping[i].equalsIgnoreCase(name))
				return true;
		}
		return false;
	}
	
	@Override
	public String toString() {
		return "FileColumn [label=" + label + ", propertyName=" + propertyName
				+ ", extName=" + extName + ", mapping="
				+ Arrays.toString(mapping) + ", enabled=" + enabled
				+ ", mandatory=" + mandatory + "]";
	}

	@Override
	public String getLabel() {
		return label;
	}

	@Override
	public void setLabel(String label) {
		this.label = label;
	}

	@Override
	public String getPropertyName() {
		return propertyName;
	}

	@Override
	public void setPropertyName(String propertyName) {
		this.propertyName = propertyName;
	}

	@Override
	public String[] getMapping() {
		return mapping;
	}

	@Override
	public void setMapping(String[] mapping) {
		this.mapping = mapping;
	}

	public boolean isEnabled() {
		return enabled;
	}

	@Override
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}

	@Override
	public boolean isMandatory() {
		return mandatory;
	}

	@Override
	public void setMandatory(boolean mandatory) {
		this.mandatory = mandatory;
	}

	@Override
	public String getExtName() {
		return extName;
	}

	@Override
	public void setExtName(String extName) {
		this.extName = extName;
	}

	@Override
	public ICellProcessor getCellProcessor() {
		return this.cellProcessor;
	}

	@Override
	public void setCellProcessor(ICellProcessor processor) {
		this.cellProcessor = processor;
		
	}

	@Override
	public String getIntName() {
		return intName;
	}

	@Override
	public void setIntName(String intName) {
		this.intName = intName;
		
	}
	@Override
	public boolean isImplicit() {
		return implicit;
	}
	@Override
	public void setImplicit(boolean implicit) {
		this.implicit = implicit;
	}

}
