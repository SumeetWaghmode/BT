/**
 * 
 * Copyright 2015
 * Siemens Building Technologies
 * All Rights Reserved.
 * 
 */
package com.accenture.sbt.rac.pse;

import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.kernel.TCComponentBOMLine;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.bom.PendingEditHelper;
import com.teamcenter.rac.pse.PSEApplicationPanel;
import com.teamcenter.rac.pse.common.InterfaceBOMEditingApplication;
import com.teamcenter.rac.pse.operations.SaveBOMOperation;

/**
 * 
 * Class BT7SaveBOMOperation
 * 
 * Customized SaveBomOperation. In this class, properties Ref Designator 
 * and UG NAME on each bom line will be inspected and processed.
 * 
 * @author Danyi Wang
 * 
 */
public class BT7SaveBOMOperation extends SaveBOMOperation {

	/**
	 * 
	 * The constructor.
	 * 
	 * @param bomEditingApplication
	 * 
	 * @author	Danyi Wang
	 * 
	 */
	public BT7SaveBOMOperation(InterfaceBOMEditingApplication bomEditingApplication) {
		super(bomEditingApplication);
	}

	/**
	 * 
	 * Override parent operation to inspect and process changes of the properties Ref Designator 
	 * and UG NAME on each bom line previously to the saving of the bom window.
	 * 
	 * @return	void
	 * 
	 * @author	Danyi Wang
	 * 
	 */
	@Override
	public void executeOperation() {

		// Retrieve the application panel
		PSEApplicationPanel appPanel = (PSEApplicationPanel) getCurrentDesktop().getCurrentApplication().getApplicationPanel();
		
		try {
			// Get the top bomline in the panel
			TCComponentBOMLine topLine = appPanel.getActiveBOMTreeTable().getBOMWindow().getTopBOMLine();
			
			// start processing the bom structure recursively
			processParentBomline(topLine);
			
		} catch (TCException e) {
			e.printStackTrace();
		}

		// Process parent tasks to save the bom window
		super.executeOperation();
	}
	

	/**
	 * 
	 * Process bom structure recursively.
	 * 
	 * @param	parentBomline		The parent bomline to be processed.
	 * @return	void
	 * 
	 * @author	Danyi Wang
	 * @throws	TCException 
	 * 
	 */
	private void processParentBomline(TCComponentBOMLine parentBomline) throws TCException {
		
		// Iterate all children bomlines to check UG NAME and bl_ref_designator
		AIFComponentContext[] children = parentBomline.getChildren();

		for (AIFComponentContext childCC : children) {
			
			// Get bomline from the component context
			TCComponentBOMLine childBomLine = (TCComponentBOMLine) childCC.getComponent();
			
			// process recursively for all children
			processParentBomline(childBomLine);

			// process the packed bomline.
			if (childBomLine.isPacked())
			{
				// This has to be executed before the unpack so that we can get children bomlines
				TCComponentBOMLine[] packedLines = childBomLine.getPackedLines();
				
				// Unpack the bom line so the we can change the attribute
				childBomLine.unpack();
				childBomLine.refresh();
				
				// Process the bomline
				processBomline(childBomLine);
				
				// Repack the bom line after the process
				childBomLine.pack();
				childBomLine.refresh();

				// Do the same for other occurrences of the packed bomline
				for (TCComponentBOMLine packedLine : packedLines )
				{
					// Process the packed bomline occurrence
					processBomline(packedLine);
				}
			}
			else
			{
				// Process the bomline
				processBomline(childBomLine);
			}
			
		}
	

		
	}

	/**
	 * 
	 * Check whether Ref Des need to be copied to UG NAME on the bomline and process if true.
	 * 
	 * @param	bomline		The bomline to be processed.
	 * @return	void
	 * @throws	TCException
	 * 
	 * @author	Danyi Wang
	 * 
	 */
	private void processBomline(TCComponentBOMLine bomLine) throws TCException {
		
		// Process only when this bomline has been modified.
		if (PendingEditHelper.hasPendedEdit(bomLine))
		{	
			// PendingEditHelper uses occurrence UID as the key to store original value of the changed entries
			String occUidKey = bomLine.getTCProperty(PendingEditHelper.OCCURRENCE_KEY).getReferenceValue().getUid();
			String originalRefDes = PendingEditHelper.getOccurrenceEdit(occUidKey, "bl_ref_designator");
			
			if (originalRefDes != null )
			{
				String newRefDes = bomLine.getProperty("bl_ref_designator");
				String newUgName = bomLine.getProperty("UG NAME");
				
				// Write Ref Des to UG Name if it is not empty and different to the original value and UG NAME
				if (!newRefDes.equalsIgnoreCase(newUgName) 
						&& !newRefDes.equalsIgnoreCase(originalRefDes)
						&& !newRefDes.equalsIgnoreCase("")){
					bomLine.setProperty("UG NAME", newRefDes);
				}
				
			}
		}
	}
}
