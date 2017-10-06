package com.accenture.rac.wizards.importer;

import java.lang.reflect.InvocationTargetException;
import java.util.concurrent.atomic.AtomicBoolean;

import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;

import com.accenture.rac.table.CellIsEnabledEditSupport;
import com.accenture.rac.table.CellIsEnabledLabelProvider;
import com.accenture.rac.table.DynamicLabelProvider;
import com.accenture.rac.table.DynamicSelectionAdapter;
import com.accenture.rac.table.DynamicViewerComparator;
import com.accenture.rac.table.Images;
import com.accenture.rac.table.ToggleAllEventListener;
import com.accenture.rac.wizards.AbstractWizardPage;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IRow;
import com.accenture.rac.wizards.importer.spec.IImportProvider;

public class ImportPage extends AbstractWizardPage {

	
	private TableViewer viewer;
	private Table table;
	
	volatile AtomicBoolean loading = new AtomicBoolean(true);

	private IImportProvider importProvider;
	
	private RowStatus rowStatusFilter;
	private ToggleAllEventListener toggleEvent;
	
	Combo actionFilter;
	private boolean showAllColumns;
	private boolean enableShowAllColums;
	
	public ImportPage(IImportProvider importProvider) {
		this.importProvider = importProvider;
		this.rowStatusFilter = RowStatus.NO_SATUS;
	}
	
	public RowStatus getRowStatusFilter() {
		return this.rowStatusFilter;
	}

	public void setRowStatusFilter(RowStatus rowStatusFilter) {
		this.rowStatusFilter = rowStatusFilter;
		if(this.toggleEvent != null){
			this.toggleEvent.setFilter(rowStatusFilter);
		}
	}
	
	@Override
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NONE);
		
		//int cols = RowStatus.values().length ;
		int cols = 5 ;
		GridLayout layout = new GridLayout(cols, true);
		container.setLayout(layout);
		
		// define the TableViewer
		viewer = new TableViewer(container, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.FULL_SELECTION | SWT.BORDER);
		viewer.setContentProvider(ArrayContentProvider.getInstance());
		GridData gd = new GridData(GridData.FILL_BOTH);
		gd.horizontalSpan = cols;
		viewer.getTable().setLayoutData(gd);
		
		// add filter based on status
		viewer.addFilter(new ViewerFilter() {
			@Override
			public boolean select(Viewer viewer, Object parent, Object element) {
				IRow p = (IRow) element;
				if(rowStatusFilter == RowStatus.NO_SATUS){
					return true;
				}else if(p.getStatus() == rowStatusFilter){
					return true;
				}
				return false;
			}
		});
		
		viewer.setComparator(new DynamicViewerComparator());
		
		viewer.refresh();
		
		// create the checkbox column to toggle enabled
		TableViewerColumn column;
		column = new TableViewerColumn(viewer, SWT.NONE);
		column.getColumn().setWidth(25);
		column.getColumn().setImage(Images.CHECKED.getImage());
		column.getColumn().setResizable(false);
		toggleEvent = new ToggleAllEventListener(viewer, importProvider.getRows());
		toggleEvent.setFilter(rowStatusFilter);
		column.getColumn().addSelectionListener(toggleEvent);
		column.setLabelProvider(new CellIsEnabledLabelProvider());
		column.setEditingSupport(new CellIsEnabledEditSupport(viewer));
		
		// create the action column
		column = new TableViewerColumn(viewer, SWT.NONE);
		column.getColumn().setWidth(100);
		column.getColumn().setResizable(true);
		column.getColumn().setMoveable(true);
		column.getColumn().setText("Action");
		column.setLabelProvider(new ColumnLabelProvider(){
			@Override
			public String getText(Object element) {
				IRow p = (IRow) element;
				if(p.getStatus() == RowStatus.ERROR)
					return p.getStatusDescription();
				return p.getStatus().getLabel();
			}
		});
			
		/*
		 * toggle the filter on action column click
		 */
		column.getColumn().addSelectionListener(new SelectionAdapter() {
		
			@Override
			public void widgetSelected(SelectionEvent e) {
				RowStatus current = getRowStatusFilter();
				RowStatus next = RowStatus.getNextStatus(current);
				setRowStatusFilter(next);
				actionFilter.select(next.getCode());
				viewer.refresh();
			}
			
		});
		
		// create the columns from the the file
		for (final IColumn p : importProvider.getColumns()) {
			String method = "get" + p.getPropertyName();
			column = new TableViewerColumn(viewer, SWT.NONE);
			column.getColumn().setWidth(0);
			column.getColumn().setText(p.getLabel());
			column.getColumn().setMoveable(true);
			DynamicLabelProvider labelProvider = new DynamicLabelProvider(method);
			column.setLabelProvider(labelProvider);
			
			DynamicSelectionAdapter selectionAdapter = new DynamicSelectionAdapter(viewer, column.getColumn(), method);
			column.getColumn().addSelectionListener(selectionAdapter);
			
		}
		
				
		// make lines and header visible
		table = viewer.getTable();
		table.setHeaderVisible(true);
		table.setLinesVisible(true);
		
		
		final Label label = new Label(container, SWT.NONE);
		gd = new GridData();
		gd.horizontalSpan = cols - 2;
		label.setLayoutData(gd);
		
		if(enableShowAllColums){
			final Button button = new Button (container, SWT.CHECK);
			button.setText ("Show all columns");
			button.setLayoutData(new GridData());
			button.addSelectionListener(new SelectionAdapter() {
				@Override
				public void widgetSelected(SelectionEvent e) {
					showAllColumns = showAllColumns ? false : true;
					toggleColumnVisibility();
					
				}
			});
		}
		
		// get all statuses
		RowStatus[] status = RowStatus.realValues();
		
/* not implemented atm
		// create selected labels
		Label selected = new Label(container, SWT.READ_ONLY);
		selected.setText("Selected:");
		selected.setLayoutData(new GridData());

		// create the statuses
		for (int i = 0; i < status.length; i++) {
			Label statusLabel = new Label(container, SWT.READ_ONLY);
			statusLabel.setText(status[i].getLabel());
			statusLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
		}
*/		
		// create the combo filters
		actionFilter = new Combo(container, SWT.READ_ONLY | SWT.BREAK);
		actionFilter.add("All                  ", 0);
		for (int i = 0; i < status.length; i++) {
			actionFilter.add(status[i].getLabel(), status[i].getCode());
			
		}
		
		gd = new GridData(GridData.HORIZONTAL_ALIGN_END);
		gd.horizontalSpan = (enableShowAllColums) ? 1 : 2;
		actionFilter.setLayoutData(gd);
		actionFilter.select(0);
		actionFilter.addSelectionListener(new SelectionAdapter() {

			@Override
			public void widgetSelected(SelectionEvent e) {
				Combo combo = (Combo) e.widget;
				int index = combo.getSelectionIndex();
				if(index == 0){
					setRowStatusFilter(RowStatus.NO_SATUS);
				}else{
					setRowStatusFilter(RowStatus.getStatus(index));
				}
				viewer.refresh();
			}
		});
		
/* not implemented atm
		// create unselcted labels
		Label unselected = new Label(container, SWT.READ_ONLY);
		unselected.setText("Unselected:");
		unselected.setLayoutData(new GridData());
		
		// create the status labels
		for (int i = 0; i < status.length; i++) {
			Label statusLabel = new Label(container, SWT.READ_ONLY);
			statusLabel.setText(status[i].getLabel());
			statusLabel.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));
		}
*/		
		setControl(container);
	}
	
	public void onEnter() throws Exception {
		// init the page title
		if(importProvider.getSourceName() != null){
			setTitle("Update BOM from '" + importProvider.getSourceName() + "'");
		}else if(getTitle() == null){
			setTitle("Update BOM");
		}
		
		loading.set(true);

		toggleColumnVisibility();
		
		final Thread loader = new Thread("loader") {
			public void run() {
				Display display = new Display();
				Shell shell =  new Shell(display);
				final ProgressMonitorDialog progressBar = new ProgressMonitorDialog(shell);
				UnknownRunnable operation = new UnknownRunnable(loading);
				operation.setMessage("Loading Data..");
				try {
					progressBar.run(true, false, operation);
				} catch (InvocationTargetException e) {
					e.printStackTrace();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		};
		// start the infinite progress bar in a separate thread
		loader.start();
		
		// update the table
		try {
			importProvider.read();
			
		} catch(Exception e) {
			loading.set(false);
			throw e;
			
		} 
		
		viewer.setInput(importProvider.getRows());
		viewer.refresh();
		
		Composite parent = getControl().getParent();
		Composite container = new Composite(parent, SWT.NONE);
		GridLayout layout = new GridLayout(4, false);
		container.setLayout(layout);
		getAWizard().canFinish(true);
		
		loading.set(false);
				
	}
	
	public void toggleColumnVisibility(){
		// hide disabled columns
		TableColumn[] tableColumns = table.getColumns();
		for (int i = 1; i < tableColumns.length; i++) {
			for (IColumn c : importProvider.getColumns()) {
				if(tableColumns[i].getText().equalsIgnoreCase(c.getLabel())){
					int width = showAllColumns || c.isEnabled() ? 100 : 0;
					tableColumns[i].setResizable(showAllColumns || c.isEnabled());
					tableColumns[i].setWidth(width);
				}
			}
		}
	}

	public boolean isEnableShowAllColums() {
		return enableShowAllColums;
	}

	public void setEnableShowAllColums(boolean enableShowAllColums) {
		this.enableShowAllColums = enableShowAllColums;
	}
	
}
