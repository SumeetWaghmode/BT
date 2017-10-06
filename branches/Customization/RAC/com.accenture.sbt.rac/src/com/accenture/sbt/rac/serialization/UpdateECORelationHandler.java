package com.accenture.sbt.rac.serialization;

import java.util.ArrayList;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.util.Registry;

public class UpdateECORelationHandler extends AbstractHandler implements IHandler {
	
	Registry reg = null;
	Shell shell = null;
	
	public Object execute(ExecutionEvent event) throws ExecutionException {
		Shell pShell = HandlerUtil.getActiveShell(event);
		ArrayList<TCComponentItemRevision> selectedItemRevs = new ArrayList<TCComponentItemRevision>();
		
		/* *** Get selected object(s) *** */
		StructuredSelection selection = (StructuredSelection)HandlerUtil.getCurrentSelection(event);
		if(selection == null || selection.isEmpty()){
			MessageDialog.openError(pShell,"Serialization error","You have to selecet at least one item revision");
		}
		else{
			Object[] selArray = selection.toArray();
			for(int i=0;i<selArray.length;i++){
				if(selArray[i] instanceof AIFComponentContext){
				    AIFComponentContext context = (AIFComponentContext) selArray[i];
				    TCComponent component = (TCComponent)context.getComponent();
				    if(component instanceof TCComponentItemRevision){
						TCComponentItemRevision currentItemRev = (TCComponentItemRevision)component;
						selectedItemRevs.add(currentItemRev);
					}
				}
			}
		}
		
		/* *** Starting validation operation *** */
		SerializationValidationOperation valOp = new SerializationValidationOperation(selectedItemRevs);
		try {
			boolean startSerialization = false;
			valOp.executeOperation();
			SerializationValidationResult validationResult = valOp.getValidationResult();
			if(validationResult.hasValError()){
				/* *** Validation errors detected *** */
				int valErrorCode = validationResult.getErrorCode();
				MessageDialog.openError(pShell,validationResult.getErrorHeader(),validationResult.getValidationErrorMessage(valErrorCode));
				return null;
			}
			else if(validationResult.hasSelError()){
				/* *** Partly validation errors occurred *** */
				startSerialization = MessageDialog.openQuestion(pShell,validationResult.getErrorHeader(),validationResult.getSelectionErrorMessage());
			}
			else{
				/* *** No validation errors occurred *** */
				startSerialization = true;
			}
			
			if(startSerialization == true){
				SerializationRestructureOperation restructureOp = new SerializationRestructureOperation(validationResult);
				restructureOp.executeOperation();
			}
		} 
		catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}
}