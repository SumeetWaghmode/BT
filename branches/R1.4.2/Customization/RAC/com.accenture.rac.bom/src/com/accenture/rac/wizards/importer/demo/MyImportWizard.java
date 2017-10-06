package com.accenture.rac.wizards.importer.demo;

import java.io.IOException;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.accenture.rac.wizards.importer.Column;
import com.accenture.rac.wizards.importer.ImportPage;
import com.accenture.rac.wizards.importer.ImportWizard;
import com.accenture.rac.wizards.importer.SelectFileColumnsPage;
import com.accenture.rac.wizards.importer.SelectFilePage;

	public class MyImportWizard extends AbstractHandler {
	
		@Override
		public Object execute(ExecutionEvent event) throws ExecutionException {
			
			// get the environment
			IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			Shell shell = window.getShell();
			
			// create importProvider and rowService
			MyImportProvider importProvider = null;
			try {
				importProvider = new MyImportProvider(MyObject.class);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			MyRowService rowService = new MyRowService();
			importProvider.setRowService(rowService);
			
			// add valid columns which are mapped to the import object and the propertyId mus be equal to the fieldname.
			importProvider.addColumn(new Column("Name", "name", null, new String[]{"name", "Name"}, true, true));
			
			// create the pages
			SelectFilePage filepage = new SelectFilePage(importProvider);
			SelectFileColumnsPage columnPage = new SelectFileColumnsPage(importProvider);
			ImportPage importPage = new ImportPage(importProvider);
			
			// create the wizard
			ImportWizard wizard = new ImportWizard("My Importer");
			wizard.setImportProvider(importProvider);
			
			// add the pages
			wizard.addPage(filepage);
			wizard.addPage(columnPage);
			wizard.addPage(importPage);
			
			// open the dialog
			WizardDialog dialog = new WizardDialog(shell, wizard);
			dialog.open();
			
			return null;
		}
	
	}
