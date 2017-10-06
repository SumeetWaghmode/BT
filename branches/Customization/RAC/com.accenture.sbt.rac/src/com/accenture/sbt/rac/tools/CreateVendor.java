package com.accenture.sbt.rac.tools;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;

import com.accenture.sbt.rac.dialog.BT7CreateVendorPartDialogue;
import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentCAEItemType;
import com.teamcenter.rac.kernel.TCComponentItem;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentItemType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.util.Registry;
import com.teamcenter.soa.client.model.Property;
import com.teamcenter.soa.exceptions.NotLoadedException;

public class CreateVendor extends AbstractHandler{

	public static String ManufacturerID =null;
	public static TCComponent ManfacturerItem = null;
	@Override
	public Object execute(ExecutionEvent arg0) throws ExecutionException {
		// TODO Auto-generated method stub
		
		try 
		{
			TCComponent tccomp = (TCComponentItemRevision)AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();
			//AIFComponentContext[] Pitem = tccomp.getPrimary();
			//Property ManfItem = tccomp.getPropertyObject("items_tag");
			// AIFComponentContext[] jk = tccomp.getRelated();
			TCComponent itemTag = tccomp.getRelatedComponent("items_tag");
			TCComponentItem ManItem = (TCComponentItem) itemTag; 
			ManfacturerItem = itemTag;
			ManufacturerID = tccomp.getProperty("object_string");
			
			// Call Create Vendor Part dialogue
				/*Display display = Display.getDefault();
				BT7CreateVndorPart shell = new BT7CreateVndorPart(display);
				shell.open();
				shell.layout();
				while (!shell.isDisposed()) {
					if (!display.readAndDispatch()) {
						display.sleep();
					}
				}	*/
								
				Shell parent = AIFDesktop.getActiveDesktop().getShell();
				BT7CreateVendorPartDialogue vendorDig = new BT7CreateVendorPartDialogue(parent , SWT.NONE);
				vendorDig.open();
		
		} 
		catch (TCException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

}
