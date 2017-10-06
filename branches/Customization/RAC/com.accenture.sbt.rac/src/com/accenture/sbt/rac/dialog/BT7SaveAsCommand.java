package com.accenture.sbt.rac.dialog;

import java.awt.Frame;

import com.teamcenter.rac.aif.AbstractAIFDialog;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.commands.saveas.SaveAsCommand;
import com.teamcenter.rac.commands.saveas.SaveItemAsDialog;
import com.teamcenter.rac.commands.saveasconnection.SaveConnectionAsDialog;
import com.teamcenter.rac.commands.saveassignal.SaveSignalAsDialog;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentCfgAttachmentLine;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCPreferenceService;
import com.teamcenter.rac.util.Registry;
import com.teamcenter.rac.kernel.TCSession;
public class BT7SaveAsCommand extends SaveAsCommand {

	public BT7SaveAsCommand() {
		// TODO Auto-generated constructor stub
	}

	public BT7SaveAsCommand(Frame frame, InterfaceAIFComponent[] ainterfaceaifcomponent, Boolean boolean1) {
		super(frame, ainterfaceaifcomponent, boolean1);
		// TODO Auto-generated constructor stub
		InterfaceAIFComponent target = null;
		final Registry comdReg = Registry.getRegistry("com.teamcenter.rac.common.actions.actions");
        if ( ainterfaceaifcomponent[0] instanceof TCComponentCfgAttachmentLine )
        {
           try
           {
              target = ((TCComponentCfgAttachmentLine)ainterfaceaifcomponent[0]).getUnderlyingComponent();
              ainterfaceaifcomponent[0] = target;
           }
           catch ( TCException ex )
           {
        	   ex.printStackTrace();
           }
        }
        else
        {
           target = ainterfaceaifcomponent[0];
        }
		TCComponent c = (TCComponent)target;
		try {
			if ( c.isTypeOf("Item") )
			{
				
				TCSession s1 = (TCSession) ainterfaceaifcomponent[0].getSession();
				TCPreferenceService prefService = s1.getPreferenceService();
				String[] idAutoGenerateHideTypes;
		        // Get which item types are required to auto-generate alternate id
		        idAutoGenerateHideTypes = prefService.getStringArray(
		                TCPreferenceService.TC_preference_site,
		                "BT7_HideAssignButton_types" );

			    if( c.getClassType().toString().equals( "PSConnection" ) )
			    {
			        setRunnable ( new SaveConnectionAsDialog(frame, target) );
			    }
			    else if( c.getClassType().toString().equals( "PSSignal" ) )
			    {
			        setRunnable ( new SaveSignalAsDialog(frame, target) );
			    }
			    else if (c.getClassType().toString().equals( "Schedule" ))
			    {
			        AbstractAIFDialog dlg = (AbstractAIFDialog) comdReg.newInstanceFor("saveAsScheduleDialog", new Object[]{frame, target});
			        setRunnable (dlg);
			    }// Check the Item type with the preference values
			    else if( idAutoGenerateHideTypes != null && idAutoGenerateHideTypes.length > 0 )
		        {
		            for( String element : idAutoGenerateHideTypes )
		            {
		                if(  c.getType().equals( element ) )
		                {
		                	setRunnable ( new com.accenture.sbt.rac.dialog.BT7SaveItemAsDialog(frame, target) );
		                }
		            }
		        }
			    else  
			    {
			        setRunnable ( new SaveItemAsDialog(frame, target) );
			    }
			}
		} catch (TCException e) {
			e.printStackTrace();
		}

	}

}
