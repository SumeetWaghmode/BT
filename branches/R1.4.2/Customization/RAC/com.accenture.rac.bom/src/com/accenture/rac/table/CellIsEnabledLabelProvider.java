package com.accenture.rac.table;

import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

import com.accenture.rac.wizards.importer.spec.IEnabled;


public class CellIsEnabledLabelProvider extends ColumnLabelProvider{

	@Override
	public Image getImage(Object element) {
		IEnabled p = (IEnabled) element;
		return (p.isEnabled()) ? Images.CHECKED.getImage() : Images.UNCHECKED.getImage(); 
	}

	@Override
	public Color getBackground(Object element) {
		//return Display.getCurrent().getSystemColor(SWT.COLOR_RED);
		return null;
	}

	@Override
	public Color getForeground(Object element) {
		//return Display.getCurrent().getSystemColor(SWT.COLOR_RED);
		return null;
	}

	@Override
	public String getText(Object element) {
		return null;
	}

}
