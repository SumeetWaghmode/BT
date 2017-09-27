package com.accenture.rac.table;

import java.util.Set;

import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.TableColumn;

import com.accenture.rac.wizards.importer.RowStatus;
import com.accenture.rac.wizards.importer.spec.IEnabled;
import com.accenture.rac.wizards.importer.spec.IStatus;

public class ToggleAllEventListener extends SelectionAdapter {
	
	private ColumnViewer viewer;
	private boolean toggle;
	private Set<?> objects;
	private RowStatus filter;
		
	public ToggleAllEventListener(ColumnViewer viewer, Set<?> objects) {
		this.viewer = viewer;
		this.objects = objects;
		this.toggle = false;
	}
	
	public ColumnViewer getViewer(){
        return viewer;
    }
	
	public RowStatus getFilter() {
		return filter;
	}

	public void setFilter(RowStatus filter) {
		this.filter = filter;
	}

	public void toggle(){
		toggle = toggle ? false : true;
	}

	@Override
	public void widgetSelected(SelectionEvent e) {
		TableColumn column = (TableColumn) e.widget;
		
		for (Object element : objects) {
			IEnabled p = (IEnabled) element;
			if(filter != null && filter != RowStatus.NO_SATUS){
				if(element instanceof IStatus){
					IStatus s = (IStatus) element;
					if(filter == s.getStatus()){
						p.setEnabled(toggle);
					}
				}
			}else{
				p.setEnabled(toggle);
			}
			
			this.getViewer().update(element, null);
		}
		column.setImage( (toggle) ? Images.CHECKED.getImage() : Images.UNCHECKED.getImage());
		toggle();
	}
}
