/**
 * 
 * Copyright 2015
 * Siemens Building Technologies
 * All Rights Reserved.
 * 
 */
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
import com.teamcenter.rac.aif.AIFTransferable;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentTcFile;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCProperty;

/**
 * 
 * Class BT7PasteAsObject
 * 
 * Paste the dataset from the clip board to the selected revision as object
 * 
 * @author Danyi Wang
 *
 */
public class BT7PasteAsObject extends AbstractHandler implements ISelectionProvider, ClipboardOwner{

	private IWorkbenchWindow window;
	//private TCSession session;
	private Vector<InterfaceAIFComponent> vecClipBoardObjects;
	private Vector<TCComponentDataset> vecNewDatasetObjects;
	
	private static String MSG_TITLE = "Paste Dataset as Object";

	
	/**
	 * 
	 * The default constructor.
	 * 
	 */
	public BT7PasteAsObject() {
	}

	/**
	 * 
	 * Build up the DHTML url of the selected object and paste it
	 * to the clip board.
	 * 
	 * @author	Danyi Wang
	 * 
	 */
	
	@SuppressWarnings("unchecked")
	/* (non-Javadoc)
	 * @see org.eclipse.core.commands.IHandler#execute(org.eclipse.core.commands.ExecutionEvent)
	 */
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		
		InterfaceAIFComponent selectedComp = null;
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);

		// Get the selected components
		try {
			//session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) 
			{
				warningMessage("Please select exactly one Object");

				return null;
			}

			selectedComp = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();

			//((TCComponent) selectedComp).refresh();
			
			// Selected component is ItemRevision ?
			if (selectedComp instanceof TCComponentItemRevision)
			{
				
				//TCComponentItemRevision itemRev = (TCComponentItemRevision) selectedComp;
				//selectedComp = itemRev.getItem();
				InterfaceAIFComponent[] ainterfaceaifcomponent = {selectedComp};
				
                AIFClipboard aifclipboard = AIFPortal.getClipboard();
                Transferable transferable = aifclipboard.getContents(this);
                
                
                if ( transferable != null )
                {
                	String revItemIDStr = "";
                	String revIDStr = "";
                	
                	vecClipBoardObjects = aifclipboard.toVector();
                	vecNewDatasetObjects = new Vector<TCComponentDataset>();

            		try 
            		{	        	
            			revItemIDStr = selectedComp.getProperty("item_id");
            			revIDStr = selectedComp.getProperty("item_revision_id");			
            		} 
            		catch (Exception e1) 
            		{			
            			e1.printStackTrace();
            		}
            		
            		// create new datasets with required naming convention for each dataset in the clip board 
            		// and replace the clip board with the newly created datasets. The original contents of the
            		// clip board will be restored after paste dialog is closed.
                    for(int i = 0; i < vecClipBoardObjects.size(); i++)
                    {
                        InterfaceAIFComponent interfaceaifcomponent = vecClipBoardObjects.elementAt(i);
                        if((interfaceaifcomponent instanceof TCComponentDataset) )
                        {
                        	
                        	TCComponentDataset origDataset = (TCComponentDataset) interfaceaifcomponent;
                        	String origDsName = origDataset.getStringProperty("object_name");
                        	String newDsName = getNewName(origDsName, revItemIDStr, revIDStr, "-", true);
                        	
                        	TCComponentDataset newDataset = origDataset.saveAs(newDsName);
                        	
                        	TCComponent[] nameRefs = newDataset.getNamedReferences();
                        	
                        	for (int j=0; j<nameRefs.length; j++)
                        	{
                        		TCComponent namedRef = nameRefs[j];
                        		
                        		TCProperty origFileName = namedRef.getTCProperty("original_file_name");
                        		if ( origFileName != null)
                        		{
                        			String tmpName = origFileName.getStringValue();
                        			String newTmpName = getNewName(tmpName, revItemIDStr, revIDStr, "_", false);
                        		
									try
									{
										if(namedRef instanceof TCComponentTcFile)
								        {
								            TCComponentTcFile tccomponenttcfile = (TCComponentTcFile) namedRef;
								            tccomponenttcfile.setOriginalFileName(newDataset, newTmpName);
								        } 
									}
									catch(TCException tcexception)
									{
										tcexception.printStackTrace();
									}
                        		}
                        			
                        	}
                        	
                        	vecNewDatasetObjects.addElement(newDataset);
                        }
                    }
                    
                    AIFTransferable aiftransferable = new AIFTransferable(vecNewDatasetObjects);
                    aifclipboard.setContents(aiftransferable, this);
                    
                    // Using copy command to fill clip board will be delayed due to run command.
//                    InterfaceAIFComponent tmpComps[] = new InterfaceAIFComponent[vecNewDatasetObjects.size()];
//                    for ( int j = 0; j < vecNewDatasetObjects.size(); j++)
//                    {
//                    	tmpComps[j] = vecNewDatasetObjects.elementAt(j);
//                    }
//                    CopyCommand dsCopyCommand = new CopyCommand( tmpComps );
//                    dsCopyCommand.run();
                    
				
                    // Call the paste command to remove dataset if paste fails
					BT7PasteCommand pasteCommand = new BT7PasteCommand(ainterfaceaifcomponent, AIFDesktop.getActiveDesktop().getFrame(), false, vecClipBoardObjects );
					pasteCommand.executeModeless();
                
                }
		
			}


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
	
	String getNewName(String strInputName, String strItemId, String strRevId, String del, boolean renameNeeded )
	{
		String strNewName = "";
		
       	String[] splits = strInputName.split("-|_");
       	
    	if(splits.length >=2)
    	{
    		strNewName = strItemId + del + strRevId;
    		for (int i=2;i<splits.length; i++ )
    		{
    			strNewName += del + splits[i];
    		}
    		
    		if (splits.length == 2)
    		{
    			String[] fileTypeSplits = splits[1].split("\\.");
    			if ( fileTypeSplits.length >= 2 )
    			{
    	    		for (int j=1;j<fileTypeSplits.length; j++ )
    	    		{
    	    			strNewName += "." + fileTypeSplits[j];
    	    		}
    			}
    		}
    	}
    	else if (renameNeeded)
    	{
    		strNewName = strItemId + del + strRevId;
    	}
    	else
    	{
    		strNewName = strInputName;
    	}
		
		return strNewName;				
	}

	@Override
	public void lostOwnership(Clipboard clipboard, Transferable transferable) {

		
	}
}
