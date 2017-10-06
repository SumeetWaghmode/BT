package com.accenture.rac.wizards.importer;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import com.accenture.rac.wizards.importer.spec.IRow;


public abstract class AbstractRow implements IRow{
	
	protected static final long serialVersionUID = 790954859537652723L;
	protected boolean enabled;
	protected RowStatus status = RowStatus.NO_SATUS;;
	protected String statusDescription;
	
	public void setPropertyValue(Object obj, String property, Object value) throws IllegalArgumentException, IllegalAccessException, InvocationTargetException{
		Method[] methods = obj.getClass().getMethods();
		for (int i = 0; i < methods.length; i++) {
			if(methods[i].getName().equalsIgnoreCase("set" + property)){
				methods[i].invoke(obj, value);
			}
		}
	}
	
	public String getPropertyValue(Object obj, String property){
		Method[] methods = obj.getClass().getMethods();
		for (int i = 0; i < methods.length; i++) {
			if(methods[i].getName().equalsIgnoreCase("get" + property)){
				try {
					return (String) methods[i].invoke(obj);
				} catch (IllegalArgumentException e) {
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					e.printStackTrace();
				}
			}
		}
		return null;
	}
	
	public boolean equals(Object obj, String property){
		
		Method method = null;
		Method[] methods = obj.getClass().getMethods();
		for (int i = 0; i < methods.length; i++) {
			if(methods[i].getName().equalsIgnoreCase("get" + property)){
				method = methods[i]; 
			}
		}
		
		if(method == null)
			return false;
		
		String obj1 = null;
		String obj2 = null;
		try {
			obj1 = (String) method.invoke(obj);
			obj2 = (String) method.invoke(this);
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
			return false;
		} catch (IllegalAccessException e) {
			e.printStackTrace();
			return false;
		} catch (InvocationTargetException e) {
			e.printStackTrace();
			return false;
		}
		
		if(obj1 == null && obj2==null)
			return true;
		
		if(obj1 != null && obj1.equalsIgnoreCase(obj2))
			return true;
		
		return false;
	}
	
	@Override
	public boolean isEnabled() {
		return enabled;
	}

	@Override
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}

	@Override
	public RowStatus getStatus() {
		return status;
	}

	@Override
	public void setStatus(RowStatus status) {
		this.status = status;
	}

	@Override
	public String getStatusDescription() {
		return statusDescription;
	}
	
	@Override
	public void setStatusDescription(String statusDescription) {
		this.statusDescription = statusDescription;
	}


}
