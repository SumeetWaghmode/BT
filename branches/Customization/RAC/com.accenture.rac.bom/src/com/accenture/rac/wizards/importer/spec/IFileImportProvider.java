package com.accenture.rac.wizards.importer.spec;

import java.io.IOException;

import org.supercsv.prefs.CsvPreference;

public interface IFileImportProvider extends IImportProvider{
	
	/**
	 * Opens the file from the getFilePath()
	 * @throws IOException
	 */
	void readFile() throws IOException;
	
	/**
	 * Close the file if open
	 * @throws IOException
	 */
	void closeFile() throws IOException;
	
	/**
	 * Set the {@link CsvPreference} by String. <br />
	 * Calls the CsvPreferences.getInstance().get(String preferenceName)
	 * @param preferenceName {@link String} 
	 */
	void setCsvPreference(String preferenceName);
	
	/**
	 * Enables the columns that are found in the file header
	 */
	void enableColumnsFromFileHeader() throws NullPointerException;
	
	/**
	 * Returns the header from the file
	 * @return String[]
	 */
	String[] getFileHeader() throws NullPointerException;
	
	/**
	 * Returns the header with the enabled columns. Disabled elements has null values.
	 * @return String[]
	 */
	String[] getFileHeaderEnabled() throws NullPointerException;
	
	
	/**
	 * Opens and close the file, reads each row in the opened file
	 * @throws IOException
	 */
	void readFileRows() throws IOException;

}
