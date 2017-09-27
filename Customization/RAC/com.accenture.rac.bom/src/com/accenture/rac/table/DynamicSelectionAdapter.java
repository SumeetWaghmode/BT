package com.accenture.rac.table;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.TableColumn;

public class DynamicSelectionAdapter extends SelectionAdapter{
	
	private TableViewer viewer;
	private String method;
	private TableColumn column;

	public DynamicSelectionAdapter(TableViewer viewer, TableColumn column, String method) {
		this.viewer = viewer;
		this.column = column;
		this.method = method;
	}
	
	
	@Override
	public void widgetSelected(SelectionEvent e) {
		DynamicViewerComparator compatator = (DynamicViewerComparator) viewer.getComparator();
		compatator.setColumn(method);
		int dir = compatator.getDirection();
		viewer.getTable().setSortDirection(dir);
		viewer.getTable().setSortColumn(column);
        viewer.refresh();
	}	
	

}
