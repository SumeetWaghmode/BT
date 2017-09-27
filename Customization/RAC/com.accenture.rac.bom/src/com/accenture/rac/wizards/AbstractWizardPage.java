package com.accenture.rac.wizards;

import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;

public abstract class AbstractWizardPage extends WizardPage {

	protected boolean pageReady;
	protected boolean canLeave;
	
	public AbstractWizardPage() {
		super("");
		setPageComplete(false);
		pageReady = false;
		canLeave = true;
	}
	
	public AbstractWizard getAWizard() {
		return (AbstractWizard)super.getWizard();
	}
	
	public boolean isPageReady() {
		return pageReady;
	}
	
	@Override
	public IWizardPage getPreviousPage() {
		getAWizard().canFinish(false);
		return super.getPreviousPage();
	}

	public void setPageReady(boolean pageReady) {
		this.pageReady = pageReady;
	}
	
	/**
	 * Enable or disable the next button if pageReady is true
	 */
	protected void drawNextButton() {
		getWizard().getContainer().updateButtons();
	}
	
	/**
	 * Enable or disable the next button
	 * @param pageReady boolean
	 */
	public void enableNextButton(boolean pageReady) {
		this.pageReady = pageReady;
		drawNextButton();
	}
	
	@Override
	public boolean canFlipToNextPage() {
		if (pageReady)
			return true;
		return false;
	}

	/**
	 * Event triggered when the page is opened <br />
	 * Triggered only if the page is in a wizard which extends {@link AbstractWizardPage}
	 */
	public void onEnter() throws Exception {}
	
	/**
	 * Event triggered when the page is left <br />
	 * Triggered only if the page is in a wizard which extends {@link AbstractWizardPage}
	 */
	public void onLeave() {}
	
	/**
	 * Prevents the dialog to move on.
	 */
	public boolean canLeave() {
		return canLeave;
	}
	
	public void canLeave(boolean canLeave) {
		this.canLeave = canLeave;
	}

}
