package com.accenture.rac.wizards.importer;

import java.util.Set;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.widgets.Shell;

import com.accenture.rac.PopupWindow;
import com.accenture.rac.wizards.AbstractWizard;
import com.accenture.rac.wizards.importer.spec.IRow;
import com.accenture.rac.wizards.importer.spec.IRowService;
import com.accenture.rac.wizards.importer.spec.IImportProvider;


public class ImportWizard extends AbstractWizard{

	private IImportProvider importProvider;
	
	/**
	 * On finnished pressed calls the {@link IImportProvider}.run();
	 * and {@link IImportProvider}.done();
	 */
	public ImportWizard(String windowTitle) {
		super(windowTitle);
	}
	
	public IImportProvider getImportProvider() {
		return importProvider;
	}

	public void setImportProvider(IImportProvider importService) {
		this.importProvider = importService;
	}
	
	@Override
	public boolean performFinish() {

		boolean result = true;
		Set<IRow> rows = importProvider.getRows();
		for (IRow row : rows) {
			if((row.getStatus() == RowStatus.REMOVE) && row.isEnabled()) {
				result = MessageDialog.openConfirm(getShell(), "Confirmation for removing Objects", "Objects will be removed from the structure. Do you want to proceed?");
				break;
			}
		}
		
		if(!result)
			return false;
		
		final ProgressMonitorDialog dialog = new ProgressMonitorDialog(getShell());

		// just exit
		if(importProvider == null)
			return true;
		
		IRowService rowService = importProvider.getRowService();
		
		if( rowService != null && ! rowService.getPermission()){
			MessageDialog.openError(getShell(), "An error has occured","You do not have the required access right to modify the item");
			return false;
		}
		
		if(importProvider != null)
			try {
				dialog.run(false, false, (IRunnableWithProgress) importProvider);
			} catch (Exception e) {
				PopupWindow.errorMessage(new Shell(), "An error has occured", "Dialog import error:\n%s", e.getMessage());
			} 				

		if(importProvider != null)
			importProvider.done();
		
		getShell().redraw();
		
		return true;
	}
	

}
