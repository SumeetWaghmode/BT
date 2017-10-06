package com.accenture.rac.wizards.importer.demo;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.IProgressMonitor;

import com.accenture.rac.wizards.importer.AbstractImportProvider;
import com.accenture.rac.wizards.importer.spec.IFileImportProvider;
import com.accenture.rac.wizards.importer.spec.IRow;

public class MyImportProvider extends AbstractImportProvider implements IFileImportProvider{

	public MyImportProvider(Class<? extends IRow> rowClazz) throws IOException {
		super(rowClazz);
	}
	
	@Override
	public void read() throws IOException {
		// TODO Auto-generated method stub
	}

	@Override
	public void run(IProgressMonitor monitor) throws InvocationTargetException,
			InterruptedException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void done() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void readFile() throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void closeFile() throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setCsvPreference(String preferenceName) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void enableColumnsFromFileHeader() throws NullPointerException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String[] getFileHeader() throws NullPointerException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String[] getFileHeaderEnabled() throws NullPointerException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void readFileRows() throws IOException {
		// TODO Auto-generated method stub
		
	}

}
