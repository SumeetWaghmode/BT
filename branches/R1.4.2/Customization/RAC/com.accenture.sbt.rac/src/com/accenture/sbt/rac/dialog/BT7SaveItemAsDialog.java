package com.accenture.sbt.rac.dialog;

import java.awt.Frame;

import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.commands.saveas.SaveItemAsDialog;

public class BT7SaveItemAsDialog extends SaveItemAsDialog {

	/**
	 * 
	 */
	private static final long serialVersionUID = -8214591482315126409L;

	public BT7SaveItemAsDialog(Frame frame, InterfaceAIFComponent interfaceaifcomponent) {
		super(frame, interfaceaifcomponent);
		// TODO Auto-generated constructor stub
		this.assignButton.setVisible(false);
	}

}
