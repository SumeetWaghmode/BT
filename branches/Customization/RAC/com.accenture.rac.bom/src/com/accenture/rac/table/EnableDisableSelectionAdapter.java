package com.accenture.rac.table;

import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import com.accenture.rac.wizards.importer.spec.IEnabled;

public class EnableDisableSelectionAdapter extends SelectionAdapter{
	
	IEnabled object;
	public EnableDisableSelectionAdapter(IEnabled object) {
		this.object = object;
	}
	
	@Override
	public void widgetSelected(SelectionEvent e) {
		boolean enabled = (object.isEnabled()) ? false : true;
		object.setEnabled(enabled);
	}	
}
