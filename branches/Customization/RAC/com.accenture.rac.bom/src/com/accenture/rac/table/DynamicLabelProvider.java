package com.accenture.rac.table;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import org.eclipse.jface.viewers.ColumnLabelProvider;


public class DynamicLabelProvider extends ColumnLabelProvider{
	private String method;
	
	public DynamicLabelProvider (String method){
		this.method = method;
	}
	
	@Override
	public String getText(Object element) {
		String text = null;
		Method[] method = element.getClass().getMethods();
		for (int i = 0; i < method.length; i++) {
			if(method[i].getName().equalsIgnoreCase(this.method)){
				try {
					Object value = method[i].invoke(element);
					if(value != null){
						if(value instanceof Boolean){
							text = ((Boolean)value) ? "Yes" : "No";
						}else {
							text = value.toString();
						}
					}
				} catch (IllegalArgumentException e) {
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					e.printStackTrace();
				}
			}
		}
		return text;
	}
}
