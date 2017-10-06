/**
 * 
 */
package com.accenture.sbt.rac.dialog;

import java.awt.Frame;

import com.teamcenter.rac.commands.newitem.NewItemDialog;
import com.teamcenter.rac.commands.newitem.NewItemPanel;
import com.teamcenter.rac.commands.newpart.NewPartCommand;
import com.teamcenter.rac.commands.newpart.NewPartDialog;
import com.teamcenter.rac.kernel.TCSession;
/**
 * @author Vinod Munukoti
 *
 */
public class BT7NewPartDialog extends NewPartDialog {

	/**
	 * 
	 */
	private static final long serialVersionUID = -495887534147563097L;

	public BT7NewPartDialog(Frame frame, boolean flag) {
		super(frame, flag);
		// TODO Auto-generated constructor stub
	}

	/**
	 * @param newpartcommand
	 */
	public BT7NewPartDialog(NewPartCommand newpartcommand) {
		super(newpartcommand);
		// TODO Auto-generated constructor stub
	}

	@Override
	protected NewItemPanel createDialogPanel(Frame frame, TCSession tcsession, NewItemDialog newitemdialog) {
		// TODO Auto-generated method stub
		//return super.createDialogPanel(frame, tcsession, newitemdialog);
		BT7VMSNewPartPanel partPanel = new BT7VMSNewPartPanel(frame, tcsession, newitemdialog);
		
	//	Registry registry = getRegistry();
	//	NewItemPanel itemPanel = (NewItemPanel) registry.newInstanceFor(getRegistrationKeyForWizardPanel(), new Object[] { frame, tcsession, partPanel } );
		return partPanel;
	}
	

}
