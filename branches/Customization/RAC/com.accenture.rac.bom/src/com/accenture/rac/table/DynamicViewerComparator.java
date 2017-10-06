package com.accenture.rac.table;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerComparator;
import org.eclipse.swt.SWT;

public class DynamicViewerComparator extends ViewerComparator {
	private String method = "toString";
	private static final int DESCENDING = 1;
	private int direction = DESCENDING;

	public int getDirection() {
		return direction == 1 ? SWT.DOWN : SWT.UP;
	}

	public void setColumn(String method) {
		if(this.method.equalsIgnoreCase(method)){
			direction = 1 - direction;
		}else {
			this.method = method;
			direction =  DESCENDING;
		}
	}

	@Override
	public int compare(Viewer viewer, Object e1, Object e2) {
		Object obj1 = null;
		Object obj2 = null;
		Method[] method = e1.getClass().getMethods();
		for (int i = 0; i < method.length; i++) {
			if(method[i].getName().equalsIgnoreCase(this.method)){
				try {
					obj1 = method[i].invoke(e1);
					obj2 = method[i].invoke(e2);
				} catch (IllegalArgumentException e) {
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				} catch (InvocationTargetException e) {
					e.printStackTrace();
				}
			}
		}
		
		String text1 = (obj1 == null) ? "" : obj1.toString();
		String text2 = (obj2 == null) ? "" : obj2.toString();
		
		try{
			Float float1 = Float.parseFloat(text1);
			Float float2 = Float.parseFloat(text2);
			int rc = float1.compareTo(float2);
			return (direction == DESCENDING) ? -rc : rc;
		}catch (Exception e) {
			// TODO: handle exception
		}
		
		int rc = text1.toString().compareTo(text2.toString());
		return (direction == DESCENDING) ? -rc : rc;
	}

}