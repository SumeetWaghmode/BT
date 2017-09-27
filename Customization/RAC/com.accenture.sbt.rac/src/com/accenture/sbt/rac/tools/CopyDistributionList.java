package com.accenture.sbt.rac.tools;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.Transferable;
import java.util.Arrays;
import java.util.Vector;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFClipboard;
import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.AIFPortal;
import com.teamcenter.rac.aif.AIFTransferable;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentParticipant;

/**
 * 
 * Class CopyDistributionList
 * 
 * Copys the Item ID of the selected Item to the Clipboard
 * 
 * @author Kai Frantz
 *
 */
public class CopyDistributionList extends AbstractHandler implements ISelectionProvider, ClipboardOwner{
	
	private IWorkbenchWindow window;	
	private Vector<InterfaceAIFComponent> vecClipBoardObjects;
	
	private static String MSG_TITLE = "Copy Distribution List to Clipboard";
	
	/**
	 * 
	 * The default constructor.
	 * 
	 */
	public CopyDistributionList() {
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.core.commands.IHandler#execute(org.eclipse.core.commands.ExecutionEvent)
	 */
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		
		InterfaceAIFComponent selectedRev = null;
		TCComponentParticipant selectedParticpants[];
		
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		
		// Get the selected components
		try {
			int iSelectedObjects = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponents().length;
			if (iSelectedObjects != 1) {
				warningMessage("Please select one Object");

				return null;
			}
			
			selectedRev = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();
			// Selected component is ItemRevision ?
			if (selectedRev instanceof TCComponentItemRevision) {
				TCComponentItemRevision itemRev = (TCComponentItemRevision) selectedRev;

				// Take the Participants from the Revision and fill them into a Vector
				selectedParticpants = itemRev.getParticipants();
				vecClipBoardObjects = new Vector<InterfaceAIFComponent>(Arrays.asList(selectedParticpants));
				AIFTransferable aiftransferable = new AIFTransferable(vecClipBoardObjects);
				
                // Get the Clipboard
				AIFClipboard aifclipboard = AIFPortal.getClipboard();
				
				// Clear and fill the Clipboard
				aifclipboard.clearClipboard();
				aifclipboard.setContents(aiftransferable, null);
			}
		} 
		catch (Exception e) {
			e.printStackTrace();
			errorMessage("EXCEPTION: " + e.getMessage());
		}
	
		return null;
	}
	
	void errorMessage(String sMessage, Object... objList) {
		MessageDialog.openError(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}
	
	void infoMessage(String sMessage, Object... objList) {
		MessageDialog.openInformation(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}
		
	void warningMessage(String sMessage, Object... objList) {
		MessageDialog.openWarning(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}
	
	@Override
	public void addSelectionChangedListener(ISelectionChangedListener iselectionchangedlistener) {
		
	}

	@Override
	public ISelection getSelection() {
		return null;
	}

	@Override
	public void removeSelectionChangedListener(ISelectionChangedListener iselectionchangedlistener) {
		
	}

	@Override
	public void setSelection(ISelection iselection) {
		
	}
	
	@Override
	public void lostOwnership(Clipboard clipboard, Transferable transferable) {

		
	}
	
}
