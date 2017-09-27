package com.accenture.sbt.rac.tools;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchWindow;

import com.accenture.sbt.rac.dialog.BT7AssignAltMaterial;
import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.util.Registry;

public class AssignAltmaterial extends AbstractHandler {
	
	private Registry myRegistry;
	private IWorkbenchWindow window;
	@Override
	public Object execute(ExecutionEvent arg0) throws ExecutionException {
			// TODO Auto-generated method stub
		try {
		myRegistry = Registry.getRegistry(this);		
		TCComponent tccomp = (TCComponentItemRevision)AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();
		
		TCComponent[] child = tccomp.getRelatedComponents(myRegistry.getString("PART_MATERIAL_REL"));
		//TCComponent[] child = tccomp.getRelatedComponents("Mat1UsesMaterial");
		//window.getWorkbench().getDisplay()
		Display display = Display.getDefault();
		BT7AssignAltMaterial shell = new BT7AssignAltMaterial(display , child , tccomp);
		shell.open();
		shell.layout();
		while (!shell.isDisposed()) {
			if (!display.readAndDispatch()) {
				display.sleep();
			}
		}
		} catch (TCException e) 
		{
			e.printStackTrace();
		}
		
		return null;
	}
	

}
