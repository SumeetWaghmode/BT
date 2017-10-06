package com.accenture.rac.wizards.importer;

import java.io.IOException;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;

import com.accenture.rac.table.CellIsEnabledEditSupport;
import com.accenture.rac.table.CellIsEnabledLabelProvider;
import com.accenture.rac.table.Images;
import com.accenture.rac.table.ToggleAllEventListener;
import com.accenture.rac.wizards.AbstractWizardPage;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IFileImportProvider;

public class SelectFileColumnsPage extends AbstractWizardPage {

	private TableViewer viewer;
	
	private IFileImportProvider importService;
	
	public SelectFileColumnsPage(IFileImportProvider importService) {
		this.importService = importService;
	}
	
	@Override
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NONE);
		
		GridLayout layout = new GridLayout(4, false);
		container.setLayout(layout);

		// define the TableViewer
		viewer = new TableViewer(container, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.HIDE_SELECTION | SWT.NO_FOCUS | SWT.BORDER);

		// set the content provider
		viewer.setContentProvider(ArrayContentProvider.getInstance());
		viewer.getTable().setLayoutData(new GridData(GridData.FILL_BOTH));
		// show only mandatory properties or properties with an extName set
		viewer.addFilter(new ViewerFilter() {
			@Override
			public boolean select(Viewer viewer, Object parent, Object element) {
				IColumn p = (IColumn) element;
			    if (p.isMandatory()) {
			      return true;
			    }
			    if (p.getExtName() != null) {
			      return true;
			    }
			    return false;
			}
		});
		viewer.refresh();
		
		// create the columns
		TableViewerColumn column;
		
		// enable/disable
		column = new TableViewerColumn(viewer, SWT.CENTER);
		column.getColumn().setWidth(25);
		column.getColumn().setImage(Images.CHECKED.getImage());
		column.getColumn().setResizable(false);
		column.getColumn().addSelectionListener(new ToggleAllEventListener(viewer, importService.getColumns()));
		column.setLabelProvider(new CellIsEnabledLabelProvider());
		column.setEditingSupport(new CellIsEnabledEditSupport(viewer));
		
		// Property
		column = new TableViewerColumn(viewer, SWT.NONE);
		column.getColumn().setWidth(100);
		column.getColumn().setText("Property");
		column.setLabelProvider(new ColumnLabelProvider() {
			  @Override
			  public String getText(Object element) {
				  IColumn p = (IColumn) element;
				  return p.getLabel() + (p.isMandatory() ? "*" : "");
			  }
			});
		
		// File Columns
		column = new TableViewerColumn(viewer, SWT.NONE);
		column.getColumn().setWidth(100);
		column.getColumn().setText("File column");
		column.setLabelProvider(new ColumnLabelProvider() {
			  @Override
			  public String getText(Object element) {
				  IColumn p = (IColumn) element;
				  return (p.getExtName() == null ? "n/a" : p.getExtName());
			  }
			});
		
		// make lines and header visible
		final Table table = viewer.getTable();
		table.setHeaderVisible(true);
		table.setLinesVisible(true); 
		setControl(container);
	}
	
	@Override
	public void onEnter() {
		getAWizard().canFinish(false);
		setErrorMessage(null);
		
		
		try {
			
			importService.readFile();
			
		} catch (IOException e) {
			e.printStackTrace();
			setErrorMessage("Cannot read file: " + importService.getSource());
			return;
		}
		
		try {
			importService.getFileHeader();
		} catch (NullPointerException e) {
			setErrorMessage("No headers found in:" + importService.getSource());
			return;
		}
		
		importService.enableColumnsFromFileHeader();
		
		try {
			importService.closeFile();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		// update the table
		viewer.setInput(importService.getColumns());
		viewer.refresh();
		
		if (!importService.columnsAreComplete())
			setErrorMessage("There are missing mandatory properties marked with an asterisk (*)");

		// enable next if no error is set
		if(getErrorMessage() == null || getErrorMessage().isEmpty()){
			enableNextButton(true);
			canLeave(true);
		}
	}
	
	@Override
	public void onLeave() {
		
		if(importService.columnsAreComplete()){
			canLeave(true);
		}else{
			setErrorMessage("There are missing mandatory properties marked with an asterisk (*)");
			canLeave(false);
		}
		
		setPageReady(canLeave);
	}
}
