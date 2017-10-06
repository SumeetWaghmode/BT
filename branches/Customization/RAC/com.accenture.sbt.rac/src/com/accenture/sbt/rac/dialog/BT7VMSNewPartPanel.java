/**
 * 
 */
package com.accenture.sbt.rac.dialog;

import java.awt.Frame;

import com.teamcenter.rac.commands.newitem.ItemInfoPanel;
import com.teamcenter.rac.commands.newitem.NewItemAlternateIdInfoPanel;
import com.teamcenter.rac.commands.newitem.NewItemDialog;
import com.teamcenter.rac.kernel.TCPreferenceService;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.rac.vendormanagement.commands.vmsnewpart.VMSNewPartPanel;
/**
 * @author Vinod Munukoti
 *
 */
public class BT7VMSNewPartPanel extends VMSNewPartPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = -3175051120177442980L;
	
	public BT7VMSNewPartPanel(Frame frame, TCSession tcsession, NewItemDialog newitemdialog) {
		super(frame, tcsession, newitemdialog);
		// TODO Auto-generated constructor stub
	}
	public String[] idAutoGenerateHideTypes;
	/* (non-Javadoc)
	 * @see com.teamcenter.rac.commands.newitem.NewItemPanel#afterSetPanel(int)
	 */
	@Override
	 public void afterSetPanel( int index )
    {
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
