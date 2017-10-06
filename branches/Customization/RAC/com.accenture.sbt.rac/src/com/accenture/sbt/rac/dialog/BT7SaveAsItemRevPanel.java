/**
 * 
 */
package com.accenture.sbt.rac.dialog;

import java.awt.Frame;

import com.teamcenter.rac.commands.saveas.SaveAsItemRevDialog;
import com.teamcenter.rac.commands.saveas.SaveAsItemRevPanel;
import com.teamcenter.rac.kernel.TCPreferenceService;
import com.teamcenter.rac.kernel.TCSession;

/**
 * @author Administrator
 *
 */
public class BT7SaveAsItemRevPanel extends SaveAsItemRevPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3569924673424980225L;

	/**
	 * @param frame
	 * @param tcsession
	 * @param saveasitemrevdialog
	 */
	public BT7SaveAsItemRevPanel(Frame frame, TCSession tcsession, SaveAsItemRevDialog saveasitemrevdialog) {
		super(frame, tcsession, saveasitemrevdialog);
		// TODO Auto-generated constructor stub
	}

	/**
	 * @param frame
	 * @param tcsession
	 * @param saveasitemrevdialog
	 * @param boolean1
	 */
	public BT7SaveAsItemRevPanel(Frame frame, TCSession tcsession, SaveAsItemRevDialog saveasitemrevdialog, Boolean boolean1) {
		super(frame, tcsession, saveasitemrevdialog, boolean1);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void loadPanels() {
		super.loadPanels();
		
		// Get preference
		TCPreferenceService prefService = session.getPreferenceService();
		String[] idAutoGenerateHideTypes;
        // Get which item types are required to auto-generate alternate id
        idAutoGenerateHideTypes = prefService.getStringArray(
                TCPreferenceService.TC_preference_site,
                "BT7_HideAssignButton_types" );

        // Hide Assign button if necessary
        if( idAutoGenerateHideTypes != null && idAutoGenerateHideTypes.length > 0 )
        {
            for( String element : idAutoGenerateHideTypes )
            {
                if( selectedItemType.equals( element ) )
                {
                	 itemInfoPanel.assignButton.setVisible(false);
                }
            }
        }
        
	}

}
