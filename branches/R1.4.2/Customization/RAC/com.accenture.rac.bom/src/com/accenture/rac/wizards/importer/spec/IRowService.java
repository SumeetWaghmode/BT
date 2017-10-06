package com.accenture.rac.wizards.importer.spec;


public interface IRowService {
	
	public boolean getPermission();
	
	/**
	 * Set a particular status for each row in the import. <br \>
	 * Typically called upon {@link IImportProvider}.read();
	 * 
	 * @param row IFileRow
	 */
	public void setRowStatus(IRow row);
	
	/**
	 * Import logic for each line. <br \>
	 * Typically called upon {@link IImportProvider}.run();
	 * 
	 * @param row IFileRow
	 */
	public void execute(IRow row) throws Exception;
	
	/**
	 * Import logic for each line. <br \>
	 * Typically called upon {@link IImportProvider}.done();
	 *	
	 * @throws Exception
	 */
	public void save() throws Exception;
}
