/**
 * 
 */
package com.accenture.sbt.rac.dialog;

import java.awt.Frame;

import com.teamcenter.rac.commands.newitem.ItemInfoPanel;
import com.teamcenter.rac.commands.newitem.NewItemAlternateIdInfoPanel;
import com.teamcenter.rac.kernel.TCPreferenceService;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.rac.vendormanagement.commands.newvendor.VMSNewVendorDialog;
import com.teamcenter.rac.vendormanagement.commands.newvendor.VMSNewVendorPanel;

/**
 * @author Danyi Wang
 *
 */
public class BT7NewVendorPanel extends VMSNewVendorPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = 8412322411773756747L;

	public String[] idAutoGenerateHideTypes;

	/**
	 * @param frame
	 * @param tcsession
	 * @param vmsnewvendordialog
	 */
	public BT7NewVendorPanel(Frame frame, TCSession tcsession, VMSNewVendorDialog vmsnewvendordialog) {
		super(frame, tcsession, vmsnewvendordialog);
		// TODO Auto-generated constructor stub
	}

	/**
	 * @param frame
	 * @param tcsession
	 * @param vmsnewvendordialog
	 * @param boolean1
	 */
	public BT7NewVendorPanel(Frame frame, TCSession tcsession, VMSNewVendorDialog vmsnewvendordialog, Boolean boolean1) {
		super(frame, tcsession, vmsnewvendordialog, boolean1);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void afterSetPanel(int index) {
		super.afterSetPanel(index);
		TCPreferenceService prefService = session.getPreferenceService();

        // Get which item types are required to auto-generate alternate id
        idAutoGenerateHideTypes = prefService.getStringArray(
                TCPreferenceService.TC_preference_site,
                "BT7_HideAssignButton_types" );
        

        if( index == getItemInfoStep() )
        {
            final ItemInfoPanel itemInfoP = (ItemInfoPanel) stepPanels.get( index );
            itemInfoP.idTextField.requestFocusInWindow();
            itemInfoP.assignButton.setVisible(true);
            
            // Custom logic to hide the assign button
            if( idAutoGenerateHideTypes != null && idAutoGenerateHideTypes.length > 0 )
            {
                for( String element : idAutoGenerateHideTypes )
                {
                    if( selectedItemType.equals( element ) )
                    {
                    	itemInfoP.assignButton.setVisible(false);
                    }
                }
            }
        }

        if( index == getAltIdInfoStep() )
        {
            NewItemAlternateIdInfoPanel idPanel = (NewItemAlternateIdInfoPanel) stepPanels.get( index );
            idPanel.setLoadedFlag( true );
        }
	}

}
