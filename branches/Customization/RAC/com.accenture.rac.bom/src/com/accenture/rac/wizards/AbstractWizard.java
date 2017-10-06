package com.accenture.rac.wizards;

import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.widgets.Shell;

import com.accenture.rac.PopupWindow;



public abstract class AbstractWizard extends Wizard {
	
	private boolean canFinish;
	
	public AbstractWizard(String windowTitle){
		setWindowTitle(windowTitle);
	}

	@Override
	public IWizardPage getStartingPage() {
		IWizardPage nextPage = super.getStartingPage();
		try{
			triggerOnEnter(nextPage);
		}catch (Throwable e) {
			PopupWindow.errorMessage(new Shell(), "An error has occured", e.getMessage());
			e.printStackTrace();
		}
		return nextPage;
	}
	
	@Override
	public boolean canFinish() {
		return canFinish;

	}
	
	public void canFinish(boolean canFinish) {
		this.canFinish = canFinish;
	}
	
	@Override
	public IWizardPage getNextPage(IWizardPage page) {
		
		triggerOnLeave(page);
		
		// return same page
		if(isAWizardPage(page) && !((AbstractWizardPage) page).canLeave())
			return page;
		
		IWizardPage nextPage = super.getNextPage(page);
		try{
			triggerOnEnter(nextPage);
		}catch (Throwable e) {
			PopupWindow.errorMessage(new Shell(), "An error has occured", e.getMessage());
			e.printStackTrace();
			return page;
		}
		return nextPage;
	}
	
	@Override
	public IWizardPage getPreviousPage(IWizardPage page) {
		IWizardPage nextPage = super.getPreviousPage(page);
		try{
			triggerOnEnter(nextPage);
		}catch (Throwable e) {
			PopupWindow.errorMessage(new Shell(), "An error has occured", e.getMessage());
			e.printStackTrace();
			return page;
		}
		return nextPage;
	}
	
	/**
	 * Triggers onEnter if the page is an instance of an {@link AbstractWizardPage} 
	 * @param page {@link IWizardPage}
	 * @throws Exception 
	 */
	private void triggerOnEnter(IWizardPage page) throws Exception{
		if (isAWizardPage(page))
			((AbstractWizardPage) page).onEnter();
	}
	
	/**
	 * Triggers onLeave if the page is an instance of an {@link AbstractWizardPage} 
	 * @param page {@link IWizardPage}
	 */
	private void triggerOnLeave(IWizardPage page){
		if (isAWizardPage(page))
			((AbstractWizardPage) page).onLeave();
	}

	/**
	 * Test the instance of a {@link IWizardPage}  
	 * @param page {@link IWizardPage}
	 * @return true if the page is an instance of an {@link AbstractWizardPage}
	 */
	private boolean isAWizardPage(IWizardPage page){
		if (page instanceof AbstractWizardPage)
			return true;
		return false;
	}

}
