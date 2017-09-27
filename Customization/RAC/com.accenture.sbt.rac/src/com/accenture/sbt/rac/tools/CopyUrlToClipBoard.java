/**
 * 
 * Copyright 2015
 * Siemens Building Technologies
 * All Rights Reserved.
 * 
 */
package com.accenture.sbt.rac.tools;

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCPreferenceService;
import com.teamcenter.rac.kernel.TCSession;

/**
 * 
 * Class CopyUrlToClipBoard
 * 
 * Copy DHTML URL of the selected object to the clip board.
 * 
 * @author Danyi Wang
 *
 */
public class CopyUrlToClipBoard extends AbstractHandler {

	private IWorkbenchWindow window;
	private TCSession session;
	
	private static String MSG_TITLE = "Copy DHTML Url to Clip Board";

	
	/**
	 * 
	 * The default constructor.
	 * 
	 */
	public CopyUrlToClipBoard() {
	}

	/**
	 * 
	 * Build up the DHTML url of the selected object and paste it
	 * to the clip board.
	 * 
	 * @author	Danyi Wang
	 * 
	 */
	
	/* (non-Javadoc)
	 * @see org.eclipse.core.commands.IHandler#execute(org.eclipse.core.commands.ExecutionEvent)
	 */
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		
		InterfaceAIFComponent selectedComp = null;
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);

		// Get the selected components
		try {
			session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) {
				warningMessage("Please select exactly one Object");

				return null;
			}

			selectedComp = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();

			((TCComponent) selectedComp).refresh();
			
			// Selected component is ItemRevision ?
			if (selectedComp instanceof TCComponentItemRevision) {
				TCComponentItemRevision itemRev = (TCComponentItemRevision) selectedComp;
				selectedComp = itemRev.getItem();
			}
			
			// Build up the DHTML url of the selected object
			String compUID = selectedComp.getUid();
			String webProtocol = session.getPreferenceService().getString( TCPreferenceService.TC_preference_site , "WEB_protocol", "http://");
			String webServerName = session.getPreferenceService().getString( TCPreferenceService.TC_preference_site , "WEB_default_site_server");
			String webAppName ="awc";
			String comURL = webProtocol + webServerName + "/" + webAppName + "/#com.siemens.splm.clientfx.tcui.xrt.showObject;uid=" + compUID;
			// Get the clip board and paste the url to it
			Toolkit toolkit = Toolkit.getDefaultToolkit();
			Clipboard clipboard = toolkit.getSystemClipboard();
			StringSelection strSel = new StringSelection(comURL);
			clipboard.setContents(strSel, null);
			
			// Show info dialog to the user
			infoMessage("URL: " + comURL + " is copied to clip board.");

		} catch (Exception e) {
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
	

}
