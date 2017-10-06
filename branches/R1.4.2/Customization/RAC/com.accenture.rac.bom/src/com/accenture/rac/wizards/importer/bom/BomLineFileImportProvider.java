package com.accenture.rac.wizards.importer.bom;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

import org.supercsv.cellprocessor.constraint.NotNull;
import org.supercsv.cellprocessor.ift.CellProcessor;
import org.supercsv.exception.SuperCsvCellProcessorException;
import org.supercsv.io.CsvBeanReader;

import com.accenture.rac.wizards.importer.CsvPreferences;
import com.accenture.rac.wizards.importer.RowStatus;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IFileImportProvider;
import com.accenture.rac.wizards.importer.spec.IRow;
import com.teamcenter.rac.kernel.TCComponentItem;
import com.teamcenter.rac.kernel.TCProperty;

public class BomLineFileImportProvider extends AbstractBomLineImportProvider implements IFileImportProvider{
	
	private String[] header = null;
	protected CsvBeanReader reader = null;

	public BomLineFileImportProvider(Class<? extends IRow> rowClazz) throws IOException {
		super(rowClazz);
	}

	@Override
	public void read() throws Exception {
		
		((BomLineService) rowService).getTCProperties().clear();
		for (IColumn column : columns) {
			if(column.getIntName() != null && (column.isEnabled() || column.isMandatory() || column.isImplicit()) ){
				((BomLineService) rowService).addTCProperty(column.getPropertyName(), column.getIntName());
			}
		}
		
		((BomLineService) rowService).readBom();

		// clear the rows collection
		rows.clear();
		
		readFile();
		
		// get rows from file
		readFileRows();
		
		// Issue 472. CSV import Quantity problem. Check Quantity and set to 1 when empty
		for (IRow child : rows) {
			if ( child instanceof BomLine )
			{
				
				TCComponentItem childItem = ((BomLineService) rowService).getItem((BomLine) child);
				
				if (childItem == null )
				{
					continue;
				}
				
				TCProperty tUom = childItem.getTCProperty("uom_tag"); 
				
				String quantity = ((BomLine) child).getQuantity();
				if ( quantity == null || quantity.isEmpty() || quantity.equals("0") )
				{
					if (tUom == null || tUom.getUIFValue().equalsIgnoreCase("each") )
					{
						quantity = "1";
					}
					else
					{
						quantity = "1.0";
					}
					
					((BomLine) child).setQuantity(quantity);
				}
				else
				{
					double quantityFloat = Double.valueOf(quantity);
					double epsilon = 0.0000000000000001;
					
					if ( Math.abs(quantityFloat - 0.0) < epsilon )
					{
						if (tUom == null || tUom.getUIFValue().equalsIgnoreCase("each") )
						{
							quantity = "1";
						}
						else
						{
							quantity = "1.0";
						}
					}
					
					((BomLine) child).setQuantity(quantity);

				}
			}
		}
		
		// Check line consistency
		for (IRow child : rows) {
			for (IRow parent : rows) {
				if(child != parent && child.equalId(parent) && !((BomLine) child).equalObject(parent)){
					parent.setStatus(RowStatus.ERROR);
					child.setStatus(RowStatus.ERROR);
					parent.setStatusDescription("Inconsistent duplicate");
					child.setStatusDescription("Inconsistent duplicate");
				}
			}
		}

		// remove duplicates and aggregate
		HashSet<BomLine> duplicates = new HashSet<BomLine>();
		for (IRow child : rows) {
			if(duplicates.contains(child) || child.getStatus() == RowStatus.ERROR)
				continue;
			
			for (IRow parent : rows) {
				if(child != parent && ((BomLine) child).equalObject(parent)){
					int pQ = ((BomLine) parent).getQuantityAsInteger();
					((BomLine) child).setQuantityAdd(pQ);
					rowService.setRowStatus(child);
					duplicates.add((BomLine) parent);
				}
			}
		}
		rows.removeAll(duplicates);
		// get remove rows from the server
		Set<IRow> bomChildrenToRemove = new HashSet<IRow>( ((BomLineService) rowService).getBomChildren() );
		for (BomLine bomLine : ((BomLineService) rowService).getBomChildren()){
			for (IRow row : rows){
				if(row.equalId(bomLine)){
					bomChildrenToRemove.remove(bomLine);
				}
			}
		}
		setRowStatus(bomChildrenToRemove, RowStatus.REMOVE);
		setEnabled(bomChildrenToRemove, false);
		rows.addAll(bomChildrenToRemove);
	}
	
	public void setEnabled(Set<IRow> rows, boolean status) {
		for (IRow row : rows) {
			row.setEnabled(status);
		}
	}
	
	@Override
	public void readFile() throws IOException {
		
		if(getSource() == null || !(getSource() instanceof String))
			throw new FileNotFoundException("No file was selected");
		
		String filePath = (String)getSource();
		try {
			reader = new CsvBeanReader(new FileReader(filePath), CsvPreferences.getInstance().getActive());
			header = reader.getHeader(true);
		} catch (IOException e) {
			e.printStackTrace();
			reader = null;
			header = null;
			throw e;
		}
	}
	
	@Override
	public void closeFile() throws IOException {
		if (reader != null)
			reader.close();
	}
	
	@Override
	public String[] getFileHeader() throws NullPointerException {
		if(header == null || header.length == 0)
			throw new NullPointerException("The header file is not set");
		
		return header;
	}
	
	@Override
	public String[] getFileHeaderEnabled() {
		if(header == null || header.length == 0)
			return null;
			
		String[] enabledHeader = new String[header.length];
		for (int i = 0; i < header.length; i++) {
			boolean enabled = false;
			for (IColumn p : getColumns()) {
				if(p.isEnabled() && p.hasMap(header[i])){
					enabled = true;
					enabledHeader[i] = p.getPropertyName();
				}
			}
			if(!enabled)
				enabledHeader[i] = null;
		}
		return enabledHeader;
	}
	
	@Override
	public void enableColumnsFromFileHeader() throws NullPointerException {
		if(header == null || header.length == 0)
			throw new NullPointerException("The header file is not set");
		
		disableAllColumns();
		// enable the columns found in the header 
		for (int i = 0; i < header.length; i++) {
			// loop over the columns
			for (IColumn p : getColumns()) {
				if(p.hasMap(header[i])){
					p.setEnabled(true);
					p.setExtName(header[i]);
				}
			}
		}
		
	}
	
	
	public CellProcessor[] getProcessors(){
		String[] header = getFileHeaderEnabled();
		CellProcessor[] processors = new CellProcessor[header.length];
		
		for (int i = 0; i < header.length; i++) {
			if(header[i] == null)
				continue;
			
			// get processor from Column
			 for (IColumn p : getColumns()) {
					if(p.getPropertyName().equals(header[i]) && p.getCellProcessor() != null){
						processors[i] = p.getCellProcessor();
						break;
					}
			 }
			 // set default processor, not null
			 processors[i] = (processors[i] == null) ? new NotNull() : processors[i];
		}
		return processors;
	}
	


	@Override
	public void readFileRows() throws IOException {
		
		
		if(reader == null)
			throw new FileNotFoundException(String.format("Cannot open file %s", getSourceName()));
		
		IRow line;
		String[] header = getFileHeaderEnabled();
		CellProcessor[] processors = getProcessors();
	
		try{
			while( (line = reader.read(rowClazz, header, processors ) ) != null ) {
				if(rowService != null)
				{
					((BomLineService) rowService).setRowUom(line);
					rowService.setRowStatus(line);
				}
				rows.add(line);
			}
		} catch (SuperCsvCellProcessorException e) {
			throw new IOException(
					String.format("Parsing error at line: %d, column: %d\n" +
							"The value cannot be '%s'",
							e.getCsvContext().getLineNumber(), e.getCsvContext().getColumnNumber(),
							e.getCsvContext().getRowSource().get(e.getCsvContext().getColumnNumber() -1))
					, e);
		}	
		closeFile();
	}
	
	@Override
	public void setCsvPreference(String preferenceName) {
		CsvPreferences.getInstance().setActivePreferenceName(preferenceName);
	}

}
