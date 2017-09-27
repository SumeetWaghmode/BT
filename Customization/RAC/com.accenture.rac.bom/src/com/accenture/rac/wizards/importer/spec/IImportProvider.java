package com.accenture.rac.wizards.importer.spec;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.Set;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.operation.IRunnableWithProgress;

import com.accenture.rac.wizards.importer.RowStatus;


/**
 * The Interface IImportProvider.
 */
public interface IImportProvider extends IRunnableWithProgress{
	
	/**
	 * Sets the {@link IRowService}.
	 *
	 * @param rowService the new row service
	 */
	void setRowService(IRowService rowService);
	
	/**
	 * Sets the {@link IRowService}.
	 *
	 * @return {@link IRowService}
	 */
	IRowService getRowService();
	
	/**
	 * Set the source.
	 *
	 * @param source the new source
	 */
	void setSource(Object source);
	
	/**
	 * Returns the source.
	 *
	 * @return String
	 */
	Object getSource();
	
	/**
	 * Returns the Source Name.
	 *
	 * @return String
	 */
	String getSourceName();
	
	/**
	 * Sets the source name.
	 *
	 * @param sourceName the new source name
	 */
	void setSourceName(String sourceName);
	
	/**
	 * Get the {@link IColumn}s in a collection.
	 *
	 * @return {@link Set}&lt;{@link IColumn}&gt;
	 */
	Set<IColumn> getColumns();
	
	/**
	 * Get the {@link IRow}s in a collection.
	 *
	 * @return {@link Set}&lt;{@link IRow}&gt;
	 */
	Set<IRow> getRows();
	
	/**
	 * Adds a {@link IColumn} to the column collection.
	 *
	 * @param column {@link IColumn}
	 */
	void addColumn(IColumn column);
	
	/**
	 * Set the {@link RowStatus} to the collection of {@link IRow} .
	 *
	 * @param rows {@link Set}&lt;{@link IRow}&gt;
	 * @param status {@link RowStatus}
	 */
	void setRowStatus(Set<IRow> rows, RowStatus status);
	
	/**
	 * Toggles all columns to disabled, ie. enabled = false
	 */
	void disableAllColumns();
	
	
	/**
	 * Indicates if all madatory columns are available and set to enabled.
	 *
	 * @return boolean
	 */
	boolean columnsAreComplete();
	
	
	/**
	 * Read the data into objects
	 *
	 * @throws IOException Signals that an I/O exception has occurred.
	 * @throws Exception the exception
	 */
	void read() throws IOException, Exception;
	
	/**
	 * Derived from RunnableWithProgressMonitor.
	 *
	 * @param monitor {@link IProgressMonitor}
	 * @throws InvocationTargetException the invocation target exception
	 * @throws InterruptedException the interrupted exception
	 */

	void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException;
	
	/**
	 * Executed when the import service has run
	 * ie. when the wizard has finished and all rows has been processed
	 */
	void done();
	 
}
