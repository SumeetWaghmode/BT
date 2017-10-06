package com.accenture.rac.table;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CheckboxCellEditor;
import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.swt.SWT;

import com.accenture.rac.wizards.importer.spec.IEnabled;


public class CellIsEnabledEditSupport extends EditingSupport {
	
	public CellIsEnabledEditSupport(ColumnViewer viewer) {
		super(viewer);
	}

	@Override
	protected CellEditor getCellEditor(Object element) {
		CheckboxCellEditor checkbox = new CheckboxCellEditor(null, SWT.CHECK | SWT.READ_ONLY);
		checkbox.setStyle(SWT.HIDE_SELECTION);
		return checkbox;
	}

	@Override
	protected boolean canEdit(Object element) {
		return true;
	}

	@Override
	protected Object getValue(Object element) {
		IEnabled p = (IEnabled) element;
		return p.isEnabled();
	}

	@Override
	protected void setValue(Object element, Object value) {
		IEnabled p = (IEnabled) element;
		p.setEnabled((Boolean) value);
		this.getViewer().update(element, null);
		//this.getViewer().refresh(true);
	}
}
