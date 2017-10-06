package com.accenture.rac.wizards.importer;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.accenture.rac.table.EnableDisableSelectionAdapter;
import com.accenture.rac.wizards.AbstractWizardPage;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IImportProvider;
import com.teamcenter.rac.aif.AIFClipboard;
import com.teamcenter.rac.aif.AIFPortal;
import com.teamcenter.rac.kernel.TCComponentItemRevision;

public class SelectComponentPage extends AbstractWizardPage {
	
	IImportProvider importSerivice;
	Group propertyGroup;
	Text itemRevisionText;
	
	public SelectComponentPage(IImportProvider importSerivice) {
		this.importSerivice = importSerivice;
	}
	
	@Override
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NONE);
		GridData gd;
		int cols = 6;
		GridLayout layout = new GridLayout(cols, true);
		container.setLayout(layout);
		
		gd = new GridData();
		Label itemRevisionLabel = new Label(container, SWT.NONE);
		itemRevisionLabel.setText("Item Revision");
		itemRevisionLabel.setLayoutData(gd);

		gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan = cols-2;
		itemRevisionText = new Text(container, SWT.BORDER);
		itemRevisionText.setLayoutData(gd);
		itemRevisionText.setEnabled(false);

		Button pasteButton = new Button(container, SWT.PUSH);
		pasteButton.setText("Paste");
		pasteButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent arg0) {
				TCComponentItemRevision itemRevision = null; 
				try {
					itemRevision = getItemRevisionFromClipboard();
				} catch (Exception e) {
					e.printStackTrace();
				}
				if(itemRevision != null){
					itemRevisionText.setText(itemRevision.toStringLabel());
					importSerivice.setSource(itemRevision);
					importSerivice.setSourceName(itemRevision.toString());
					enableNextButton(true);
				}else{
					setErrorMessage("Could not find an ItemRevision in the clip board.\nSelect an ItemRevision and press 'ctrl + c' before pasting.");
				}
			}
		});
		
		gd = new GridData(GridData.FILL_HORIZONTAL);
	    gd.horizontalSpan = cols;
	    propertyGroup = new Group(container, SWT.NONE);
	    propertyGroup.setText("Properties");
	    propertyGroup.setLayout(new GridLayout());
	    propertyGroup.setLayoutData(gd);

	    for (IColumn column : importSerivice.getColumns()) {
			if(!column.isMandatory()){
				Button button = new Button(propertyGroup, SWT.CHECK);
				button.setText(column.getLabel());
				button.addSelectionListener(new EnableDisableSelectionAdapter(column));
				button.pack();
			}
		}
        setControl(container);
	}
	
	@Override
	public void onEnter() {
		setErrorMessage(null);
	}

	@Override
	public void onLeave() {
		Control[] controls = propertyGroup.getChildren();
		int enabledProperties = 0;
		for (int i = 0; i < controls.length; i++) {
			Button control = (Button) controls[i];
			if(control.getSelection())
				enabledProperties++;
		}
		
		if(enabledProperties < 1){
			setErrorMessage("You need to select at least 1 property");
			canLeave(false);
		}else {
			canLeave(true);
		}
	}
	
	public TCComponentItemRevision getItemRevisionFromClipboard() throws UnsupportedFlavorException, IOException{
		AIFClipboard clipboard = AIFPortal.getClipboard();
		Transferable content = clipboard.getContents(this);
		 
		if ( content != null )
		{
            @SuppressWarnings("rawtypes")
			Vector aifCBComponents = (Vector)content.getTransferData(new DataFlavor(Vector.class, "AIF Vector"));
            for (Object object : aifCBComponents) {
                  if(object instanceof TCComponentItemRevision){
                	  return (TCComponentItemRevision) object;
                  }
            }
		}
		return null;
	}
}
