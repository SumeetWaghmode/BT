package com.accenture.rac.wizards.importer.demo;

import com.accenture.rac.wizards.importer.AbstractRow;
import com.accenture.rac.wizards.importer.spec.IRow;

public class MyObject extends AbstractRow{
	
	private static final long serialVersionUID = 8081440821341654010L;
	private String id;
	private String name;
	
	@Override
	public String getId() {
		return id;
	}

	@Override
	public boolean equalId(Object obj) {
		if(obj == null || !(obj instanceof IRow))
			return false;
		
		AbstractRow row = (AbstractRow) obj;
			
		if(this.getId().equals(row.getId()))
			return true;
		
		return false;
		
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}
}
