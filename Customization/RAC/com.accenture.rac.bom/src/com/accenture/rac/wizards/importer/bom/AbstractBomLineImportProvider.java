package com.accenture.rac.wizards.importer.bom;

import java.awt.Desktop;
import java.io.IOException;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IProgressMonitorWithBlocking;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import com.accenture.rac.PopupWindow;
import com.accenture.rac.wizards.importer.AbstractImportProvider;
import com.accenture.rac.wizards.importer.RowStatus;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IRow;

abstract class AbstractBomLineImportProvider extends AbstractImportProvider{

	public AbstractBomLineImportProvider(Class<? extends IRow> rowClazz) throws IOException {
		super(rowClazz);
	}

	@Override
	public void run(IProgressMonitor monitor) {
		IProgressMonitorWithBlocking blocking = (IProgressMonitorWithBlocking) monitor;
		
		int jobs = 0;
		for (IRow row : rows) {
			for (int i = 0; i < RowStatus.activeValues().length; i++) {
				if(row.getStatus() == RowStatus.activeValues()[i]){
					jobs++;
				}
			}
		}
		
		blocking.beginTask("Prepare import...", jobs);
		
		for (int i = 0; i < RowStatus.realValues().length; i++) {
			RowStatus status = RowStatus.realValues()[i];
			
			for (IRow row : rows) {
				if(row.getStatus() == status){
					blocking.setTaskName(status.getLabel() + "..");
					try {
						if(rowService != null)
							rowService.execute(row);
							log.append(status.getLabel() + ": " + row.toString() + EOL);
					} catch (Throwable e) {
						PopupWindow.errorMessage(new Shell(), "An Error has occured", "There was an error processing %s\n%s", row.toString(), e.getMessage());
						try {
							log.append("Error: " + row.toString() + EOL);
						} catch (IOException e1) {
							PopupWindow.errorMessage(new Shell(), "An Error has occured", "Cannot write to the log file");
							e1.printStackTrace();
						}
						e.printStackTrace();
					}
					blocking.worked(1);
				}
			}
		}
		blocking.done();
	}

	@Override
	public void done() {
		
		try {
			if(rowService != null)
				rowService.save();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		if(log != null){
			try {
				log.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}
		
		Shell shell = Display.getCurrent().getActiveShell();
		StringBuilder message = new StringBuilder();
		boolean updatesDone = false;
		for (RowStatus status : RowStatus.activeValues()) {
			int count = 0;
			for (IRow row : rows) {
				if(row.isEnabled() && status == row.getStatus())
					count++;
			}
			message.append(status.getLabel() + "\t" + count + "\n");
			if(count != 0)
				updatesDone = true; 
		}
		String updatesNotDone = (!updatesDone) ? "No changes were done\n" : "";
		
		MessageDialog successDialog =  new MessageDialog(shell, "The import is complete", null, updatesNotDone + message.toString(), MessageDialog.INFORMATION, new String[] { "Open Log","OK"}, 0){
			@Override
			public boolean close() {
				if(this.getReturnCode() == 0){
					try {
						Desktop.getDesktop().open(logFile);
					} catch (IOException e) {
						PopupWindow.errorMessage(getShell(), "", "Cannot open " + logFile.getAbsoluteFile());
						e.printStackTrace();
					};
					return false;
				}
				logFile.delete();
				return super.close();
			}
			
		};
		successDialog.open();
		
		for (IColumn column: getColumns()) {
			if((updatesDone && column.getPropertyName().equalsIgnoreCase("t0") && column.isEnabled())
					|| ( updatesDone && column.getPropertyName().equalsIgnoreCase("t1") && column.isEnabled())
					|| ( updatesDone && column.getPropertyName().equalsIgnoreCase("t2") && column.isEnabled())
					){
				PopupWindow.warningMessage(shell, "", "Please control the 3D-placement of the BOM !");
				break;
			}
		}
	}
}
