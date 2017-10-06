package com.teamcenter.rac.commands.newdataset;

import java.awt.Frame;

import com.teamcenter.rac.kernel.TCSession;

public class SBTNewDatasetDialog extends NewDatasetDialog {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public SBTNewDatasetDialog(Frame arg0, TCSession arg1) {
		super(arg0, arg1);
		//System.out.println("print....................");
	}
	
	public SBTNewDatasetDialog(NewDatasetCommand paramNewDatasetCommand)
	  {
	    super(paramNewDatasetCommand);
	   
	  }
	
	@Override
	protected void showRelation(boolean arg0) {
		// TODO Auto-generated method stub
		super.showRelation(false);
	}
	
	@Override
	public void initUI() {
		// TODO Auto-generated method stub
		super.initUI();
	}
}
