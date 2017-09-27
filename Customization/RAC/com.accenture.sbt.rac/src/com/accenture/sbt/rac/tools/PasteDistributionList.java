package com.accenture.sbt.rac.tools;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.Transferable;
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
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentParticipant;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.services.rac.core.DataManagementService;
// simport com.teamcenter.services.rac.core._2006_03.DataManagement.CreateRelationsResponse;
import com.teamcenter.services.rac.core._2006_03.DataManagement.Relationship;

/**
 * 
 * Class CopyDistributionList
 * 
 * Copys the Item ID of the selected Item to the Clipboard
 * 
 * @author Kai Frantz
 *
 */
public class PasteDistributionList extends AbstractHandler implements ISelectionProvider, ClipboardOwner{
	
	private static String HASPARTICIPANT_RELATION = "HasParticipant";
	
	private IWorkbenchWindow window;	
	private Vector<InterfaceAIFComponent> vecClipBoardObjects;
	
	private static String MSG_TITLE = "Paste Distribution List to Clipboard";
	
	/**
	 * 
	 * The default constructor.
	 * 
	 */
	public PasteDistributionList() {
	}
	
	@SuppressWarnings("unchecked")
	/* (non-Javadoc)
	 * @see org.eclipse.core.commands.IHandler#execute(org.eclipse.core.commands.ExecutionEvent)
	 */
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		
		InterfaceAIFComponent selectedObject = null;
		
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		
		// Get the selected components
		try {
			int iSelectedObjects = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponents().length;
			if (iSelectedObjects != 1) {
				warningMessage("Please select one Object");

				return null;
			}

			selectedObject = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();
			((TCComponent) selectedObject).refresh();
			
			// Selected component is ItemRevision ?
			if (selectedObject instanceof TCComponentItemRevision) {
				TCComponentItemRevision itemRev = (TCComponentItemRevision) selectedObject;
					
	
				// Take the Participants from the Clipboard and fill them into a Vector
				AIFClipboard aifclipboard = AIFPortal.getClipboard();
                Transferable transferable = aifclipboard.getContents(this);
                
                if ( transferable != null ) {
                	vecClipBoardObjects = aifclipboard.toVector();
                	
                	Relationship[] relations = new Relationship[vecClipBoardObjects.size()];
                	
                	// Build Relations for every Participant in the Clipboard
                	for(int i = 0; i < vecClipBoardObjects.size(); i++) {
                		InterfaceAIFComponent interfaceaifcomponent = vecClipBoardObjects.elementAt(i);
                		TCComponentParticipant participant = (TCComponentParticipant) interfaceaifcomponent;
                		
	                	// Create relation between Revision and Participant
						
						relations[i] = new Relationship();
						relations[i].clientId = new Integer(1).toString();
						relations[i].primaryObject = itemRev;
						relations[i].secondaryObject = participant;
						relations[i].relationType = HASPARTICIPANT_RELATION;
                	}
                	TCSession session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();
                	
                	DataManagementService dmService = DataManagementService.getService(session);
                	
					/*CreateRelationsResponse createRelationsResponse = */dmService.createRelations(relations);
                	
					// Check for errors#
					/*
					if (createRelationsResponse.serviceData.sizeOfPartialErrors() > 0) {
						String[] messageArray = createRelationsResponse.serviceData
								.getPartialError(0).getMessages();
						String message = new String();
						for (int i = 0; i < messageArray.length; i++) {
							message += messageArray[i] + "\n";
						}
	
						errorMessage("Create-Relation error: " + message);
					}
					*/
					// Clear the Clipboard
					aifclipboard.clearClipboard();
                }
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
