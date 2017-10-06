package com.accenture.sbt.rac.tools;

import java.io.IOException;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.accenture.rac.PopupWindow;
import com.accenture.rac.wizards.importer.Column;
import com.accenture.rac.wizards.importer.ImportPage;
import com.accenture.rac.wizards.importer.ImportWizard;
import com.accenture.rac.wizards.importer.SelectComponentPage;
import com.accenture.rac.wizards.importer.bom.BomLineCopyPropertiesImportProvider;
import com.accenture.rac.wizards.importer.bom.BomLineService;
import com.accenture.rac.wizards.importer.spec.IImportProvider;
import com.accenture.sbt.rac.objects.SbtBomLine;
import com.teamcenter.rac.kernel.TCException;

public class CopyBomProperties extends AbstractHandler {
	
	private static String MSG_TITLE = "Copy BOM properties";


	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		Shell shell = window.getShell();

		IImportProvider importProvider;
		try {
			importProvider = new BomLineCopyPropertiesImportProvider(SbtBomLine.class);
		} catch (IOException e1) {
			PopupWindow.errorMessage(shell, MSG_TITLE, "Cannot initiate the log writer");
			e1.printStackTrace();
			return null;
		}
		
		try {
			BomLineService rowService = new BomLineService();
			rowService.setRowClazz(SbtBomLine.class);
			importProvider.setRowService(rowService);
		} catch (TCException e) {
			PopupWindow.errorMessage(shell, "An error has occured", "Cannot read the ItemRevision from the server:\n%s", e.getMessage());
			e.printStackTrace();
			return null;
		}

		importProvider.addColumn(new Column("Find No.", "findNo", "bl_sequence_no", null, true, true));
		importProvider.addColumn(new Column("Reference Designator", "refDes", "bl_ref_designator", null, true, true));
		importProvider.addColumn(new Column("Item ID", "itemId", "bl_item_item_id", null, false, false));
		importProvider.addColumn(new Column("Revison ID", "revisionId", "bl_rev_item_revision_id", null, false, false));
		importProvider.addColumn(new Column("Quantity", "quantity", "bl_quantity", null, false, false));
		importProvider.addColumn(new Column("Transformation Matrix", "transformation", "bl_plmxml_occ_xform", null, false, false));
		
		SelectComponentPage selectComponentPage = new SelectComponentPage(importProvider);
		selectComponentPage.setDescription("Paste an item revision from the clip board and select what properties that should be copied.");

		ImportPage importPage = new ImportPage(importProvider);
		importPage.setEnableShowAllColums(true);
		
		ImportWizard wizard = new ImportWizard("Update BOM");
		wizard.setImportProvider(importProvider);
		wizard.addPage(selectComponentPage );
		wizard.addPage(importPage);
		
		WizardDialog dialog = new WizardDialog(shell, wizard);
		dialog.open();
		return null;
	}
}
