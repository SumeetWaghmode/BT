/**
 * 
 * Copyright 2015
 * Siemens Building Technologies
 * All Rights Reserved.
 * 
 */
package com.accenture.sbt.rac.pse;

import com.teamcenter.rac.aif.AbstractAIFCommand;
import com.teamcenter.rac.pse.common.InterfaceBOMEditingApplication;

/**
 * 
 * Class BT7SaveBOMCommand
 * 
 * Override SaveBOMCommand to execute customized SaveBOMOperation.
 * 
 * @author Danyi Wang
 *
 */
public class BT7SaveBOMCommand extends AbstractAIFCommand {

	/**
	 * 
	 * Call the customized BT7SaveBOMOperation.
	 * 
	 * @param interfacebomeditingapplication
	 * 
	 * @author Danyi Wang
	 * 
	 */
	public BT7SaveBOMCommand(InterfaceBOMEditingApplication interfacebomeditingapplication) {
		
		// Call the operation class
        BT7SaveBOMOperation bt7_savebomoperation = new BT7SaveBOMOperation(interfacebomeditingapplication);
        interfacebomeditingapplication.getSession().queueOperation(bt7_savebomoperation);
	}

}
