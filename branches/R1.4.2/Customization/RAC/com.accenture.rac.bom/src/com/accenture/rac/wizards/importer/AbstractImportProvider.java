package com.accenture.rac.wizards.importer;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IRow;
import com.accenture.rac.wizards.importer.spec.IRowService;
import com.accenture.rac.wizards.importer.spec.IImportProvider;


public abstract class AbstractImportProvider implements IImportProvider{

	private Object source = null;
	private String sourceName = null;
	protected Set<IColumn> columns = null;
	protected Set<IRow> rows = null;
	protected File logFile;
	protected static String EOL = System.getProperty("line.separator");
	protected BufferedWriter log;
	protected Class<? extends IRow> rowClazz;
	protected IRowService rowService;

	public AbstractImportProvider(Class<? extends IRow> rowClazz) throws IOException {
		super();
		 //create a temp file
	    this.logFile = File.createTempFile("tempfile", ".txt");
	    this.log = new BufferedWriter(new FileWriter(logFile));
	    
		this.columns = new LinkedHashSet<IColumn>();
		this.rows = new HashSet<IRow>();
		this.rowClazz = rowClazz;
	}

	@Override
	public Object getSource() {
		return source;
	}

	@Override
	public void setSource(Object source) {
		this.source = source;
		
	}

	@Override
	public Set<IColumn> getColumns() {
		return columns;
	}
	
	@Override
	public void setSourceName(String sourceName){
		this.sourceName = sourceName;
	}

	@Override
	public String getSourceName() {
		return sourceName;
	}

	@Override
	public void disableAllColumns() {
		for (IColumn p : getColumns()) {
			p.setEnabled(false);
			p.setExtName(null);
		}
	}
	
	@Override
	public boolean columnsAreComplete() {
		// Check if any mandatory fields are empty
		for (IColumn p : getColumns()) {
			if(p.isMandatory() && ( p.getExtName() == null || p.getExtName().isEmpty() || !p.isEnabled() )){
				return false;
			}
		}
		return true;
	}
	
	@Override
	public void addColumn(IColumn column) {
		this.columns.add(column);
	}


	@Override
	public void setRowService(IRowService rowService) {
		this.rowService = rowService;
	}

	@Override
	public IRowService getRowService() {
		return rowService;
	}

	@Override
	public void setRowStatus(Set<IRow> rows, RowStatus status) {
		for (IRow row : rows) {
			row.setStatus(status);
		}
	}
	
	@Override
	public Set<IRow> getRows() {
		return rows;
	}

}