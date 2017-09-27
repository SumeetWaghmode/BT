package com.accenture.sbt.rac.tools;


import java.io.IOException;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.accenture.rac.PopupWindow;
import com.accenture.rac.wizards.importer.CellProcessors;
import com.accenture.rac.wizards.importer.Column;
import com.accenture.rac.wizards.importer.ImportPage;
import com.accenture.rac.wizards.importer.ImportWizard;
import com.accenture.rac.wizards.importer.SelectFileColumnsPage;
import com.accenture.rac.wizards.importer.SelectFilePage;
import com.accenture.rac.wizards.importer.bom.BomLineFileImportProvider;
import com.accenture.rac.wizards.importer.bom.BomLineService;
import com.accenture.sbt.rac.objects.SbtBomLine;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCException;

public class UpdateBOM extends AbstractHandler {

	private static String MSG_TITLE = "Update BOM";
	private IWorkbenchWindow window = null;
	private Shell shell = null;

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {

		boolean debug = false;
		// get eclipse window and shell
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		shell = window.getShell();
		
		BomLineService rowService;
		try{
			rowService = new BomLineService();
			rowService.setRowClazz(SbtBomLine.class);
			rowService.readBom();
		} catch (Exception e) {
			PopupWindow.errorMessage(shell, MSG_TITLE, "An error has occured, cannot read the ItemRevision from the server");
			e.printStackTrace();
			return null;
		}
		
		BomLineFileImportProvider importProvider;
		try {
			importProvider = new BomLineFileImportProvider(SbtBomLine.class);
		} catch (IOException e2) {
			PopupWindow.errorMessage(shell, MSG_TITLE, "Cannot initiate the log writer");
			e2.printStackTrace();
			return null;
		}
		
		// Construct the properties to update
		Column findNo = new Column("Find No.", "findNo", "bl_sequence_no", new String[]{"find no"}, false, false);
		findNo.setImplicit(true);
		importProvider.addColumn(findNo);
		importProvider.addColumn(new Column("Reference Designator", "refDes", "bl_ref_designator", new String[]{"RefDes","Ref Des","ReferenceDesignator","Reference Designator", "refdes","Ref.Des."}, false, true));
		//importProvider.addColumn(new Column("UG NAME", "refDes", "UG NAME", new String[]{"RefDes","Ref Des","ReferenceDesignator","Reference Designator", "refdes","Ref.Des."}, false, true));
		importProvider.addColumn(new Column("Item ID", "itemId", "bl_item_item_id", new String[]{"ItemId","Item Id","PartNr."}, false, true));
		importProvider.addColumn(new Column("Revison ID", "revisionId", "bl_rev_item_revision_id", new String[]{"RevId","Rev Id","RevisionId","Revision Id"}, false, false));
		// Issue 472. CSV import Quantity problem. Import Quantity in any case.
		Column qualityColumn = new Column("Quantity", "quantity", "bl_quantity", new String[]{"quantity"}, true, false);
		qualityColumn.setImplicit(true);
		importProvider.addColumn(qualityColumn);
		
		Column uomColumn = new Column("Unit of Measure", "uom", "bl_uom", new String[]{"uom","unit of measure","UoM","Uom"}, true, false);
		uomColumn.setImplicit(true);
		importProvider.addColumn(uomColumn);
		
		// column without matching mapping array
		importProvider.addColumn(new Column("Matrix", "transformation", "bl_plmxml_occ_xform", true));
		importProvider.addColumn(new Column("X Pos", "t0", null, new String[]{"X_Cor"}, CellProcessors.BigDecimalMilliToMeter , false, false));
		importProvider.addColumn(new Column("Y Pos", "t1", null, new String[]{"Y_Cor"}, CellProcessors.BigDecimalMilliToMeter, false, false));
		importProvider.addColumn(new Column("Z Pos", "t2", null, new String[]{"Z_Cor"}, CellProcessors.BigDecimalMilliToMeter, false, false));
		importProvider.addColumn(new Column("Rotation", "rotation", null, new String[]{"Rotation"}, CellProcessors.Int, false, false));
		importProvider.addColumn(new Column("Placement", "placement", null, new String[]{"Placement Side"}, false, false));
		importProvider.addColumn(new Column("Release Status", "latestStatus"));
		importProvider.addColumn(new Column("Has 3D", "has3D"));
		//importProvider.addColumn(new Column("UOM", "uom"));

		importProvider.setRowService(rowService);
		// set the default file pasting setting to the importprovider
		importProvider.setCsvPreference("\" ; \\r\\n #");
		//importProvider.setFilePath("C:\\Vollbest_PXC3.csv");
		//importProvider.setSource("C:\\8000072773-C-Placementdata.csv");
		
		// Check if the item is valid for BOM import
		TCComponentItemRevision itemRevision = null;
		String itemRevisionType = null;
		try {
			itemRevision = BomLineService.getItemRevision();
			itemRevisionType = itemRevision.getType();
		} catch (TCException e1) {
			e1.printStackTrace();
		}
		
		String bt7_parttype = null;
		try {
			bt7_parttype = itemRevision.getStringProperty("bt7_parttype");
		} catch (TCException e) {
			e.printStackTrace();
		}
	
		if (!debug && ( itemRevision == null || itemRevisionType == null || ! itemRevisionType.equals("BT7_ComponentRevision") || bt7_parttype == null || ! bt7_parttype.equals("l7LGCodPEq"))) {
			PopupWindow.errorMessage(shell, MSG_TITLE, "The selected component is not valid for CSV BOM update. The import is only allowed for BT7_ComponentRevision with part type PCB equiped");
			return null;
		}
		
		// check read/write permission
        if (!debug && !rowService.getPermission()) {
        	PopupWindow.errorMessage(shell, MSG_TITLE, "You do not have the required access right to modify the item");
        	return null;
        }
		
		// create the import file page
		SelectFilePage selectFilePage = new SelectFilePage(importProvider);
		selectFilePage.setDialogExt(new String[]{"*.csv"});
		selectFilePage.setTitle("Import File");
		selectFilePage.setDescription("The file must contain a header row. The column name 'Item ID' is mandatory (case insensitive). \n Supported BOM attributes are: Revision ID, Find Nr, Reference Des and Quantity");
		selectFilePage.setDebug(debug);
		
		// create the validate file page
		IWizardPage validateFilePage = new SelectFileColumnsPage(importProvider);
		validateFilePage.setTitle("Validate Content");
		validateFilePage.setDescription("Select the columns to import or update");
		
		// create the Update BOM page
		ImportPage updateBOMPage = new ImportPage(importProvider);
		updateBOMPage.setTitle("Update BOM lines");
		updateBOMPage.setDescription("Select lines to insert, update or remove");
		updateBOMPage.setEnableShowAllColums(true);
		
		// create the wizard and the the title 
		ImportWizard wizard = new ImportWizard("Update BOM");
		wizard.setImportProvider(importProvider);
		wizard.addPage(selectFilePage);
		wizard.addPage(validateFilePage);
		wizard.addPage(updateBOMPage);
		wizard.needsProgressMonitor();
		
		WizardDialog dialog = new WizardDialog(shell, wizard);
		dialog.open();
		return null;
	}
}
