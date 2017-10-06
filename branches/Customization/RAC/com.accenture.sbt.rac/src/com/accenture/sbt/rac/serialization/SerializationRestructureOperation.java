package com.accenture.sbt.rac.serialization;

import java.util.ArrayList;
import java.util.HashMap;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import com.teamcenter.rac.aif.AbstractAIFOperation;
import com.teamcenter.rac.kernel.ServiceData;
import com.teamcenter.rac.kernel.TCAttachmentType;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentDatasetType;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentProcessType;
import com.teamcenter.rac.kernel.TCComponentTaskTemplate;
import com.teamcenter.rac.kernel.TCComponentTaskTemplateType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.services.rac.core.DataManagementService;
import com.teamcenter.services.rac.core._2006_03.DataManagement.CreateRelationsResponse;
import com.teamcenter.services.rac.core._2006_03.DataManagement.Relationship;


public class SerializationRestructureOperation extends AbstractAIFOperation {
	
	private static String WF_SUB_NAME = "BT Change Order - Sub Process";
	private static String SERIALIZATION_TASK_NAME = "Serialization";
	
	private SerializationValidationResult result = null;
	private TCSession session = null;
	
	public SerializationRestructureOperation(SerializationValidationResult result){
		super();
		this.result = result;
	}

	@Override
	public void executeOperation() throws Exception {
		TCComponent validECOObj = this.result.getValidECOObj();
		this.session = validECOObj.getSession();
		DataManagementService service = DataManagementService.getService(this.session);
		
		ArrayList<TCComponent> okObjList = this.result.getOKObjList();
		HashMap<TCComponent,TCComponent> cutPastObjMap = this.result.getCutPastObjMap();
		
		for (TCComponent curSelObj : okObjList) {
			/* *** The current selected object is the key object to get the value object from the map.*** */
			TCComponent predToCutObj = cutPastObjMap.get(curSelObj);
			/* *** Cut the predecessor object which is currently attached as "Impacted Item" from the ECO object *** */
			this.addOrRemoveObject(20,service,validECOObj,"CMHasSolutionItem",predToCutObj); 
			
			/* *** Add the predecessor object as impacted item to the eco *** */
			this.addOrRemoveObject(10,service,validECOObj,"CMHasImpactedItem",predToCutObj); 
			
			/* *** Add the corresponding selected object as new "Solution Item" to the ECO object *** */
			this.addOrRemoveObject(10,service,validECOObj,"CMHasSolutionItem",curSelObj); 
		}
		
		/* *** Create a text datset and start the workflow for the valid ECO object ***/
		TCComponentDataset dataset = this.createDataset(validECOObj);
		final TCComponentTaskTemplateType taskTemplateType = (TCComponentTaskTemplateType)this.session.getTypeComponent(TCComponentTaskTemplate.TYPE_NAME);
		final TCComponentTaskTemplate taskTemplate = taskTemplateType.find(SerializationRestructureOperation.WF_SUB_NAME,TCComponentTaskTemplate.PROCESS_TEMPLATE_TYPE);
		if (taskTemplate != null) {
			final TCComponentProcessType processType = (TCComponentProcessType) this.session.getTypeComponent("Job");
			/* *** Add the ECO object also to the workflow task *** */
			okObjList.add(validECOObj);
			
			/* *** Reserve place also for the new dataset for attachments and attachmentsType array *** */
			/* *** Add the selected objects and the valid ECO object as attachments to the workflow *** */
			final TCComponent[] attachments = okObjList.toArray(new TCComponent[okObjList.size()+1]);
			final int[] attachmentsType = new int[okObjList.size()+1];
			for (int i=0;i<attachmentsType.length;i++) {
				attachmentsType[i] = TCAttachmentType.TARGET;
			}
			
			/* *** Add the new dataset on the last position as a referenc object *** */
			if(attachments.length > 0 && attachmentsType.length > 0){
				attachments[attachments.length -1] = dataset;
				attachmentsType[attachmentsType.length -1] = TCAttachmentType.REFERENCE;
			}
			
			/* *** Start the workflow *** */
			processType.create("Serialization Task for "+validECOObj.toDisplayString(),"Serialization",taskTemplate,attachments,attachmentsType);
		} 
		else {
			
			MessageDialog.openError(Display.getCurrent().getActiveShell(),"Workflow Error","Workflow-Template '"+SerializationRestructureOperation.WF_SUB_NAME+"' not found!");
			throw new TCException("SBT - SerializationRestructureOperaionError:\nWorkflow-Template '"+SerializationRestructureOperation.WF_SUB_NAME+"' not found!");
		}
		
		/* *** Refresh the current ECO *** */
		validECOObj.refresh();
	}
	
	
	private void addOrRemoveObject(int actionCode,DataManagementService service,TCComponent primary,String relationName,TCComponent secondary) throws TCException{
		Relationship relationship = new Relationship();
		relationship.primaryObject = primary;
		relationship.relationType = relationName;
		relationship.secondaryObject = secondary;
		
		if(actionCode == 10){
			/* *** If we get an error, throw an exception and inform the user *** */
			CreateRelationsResponse creRelResponse = service.createRelations(new Relationship[]{relationship});
			if(creRelResponse.serviceData.sizeOfPartialErrors() >= 1 ){
				Shell shell = Display.getCurrent().getActiveShell();
				String title = "Create Relation Error";
				String primaryName = primary.toDisplayString();
				String secondaryName = secondary.toDisplayString();
				StringBuilder strBuilder = new StringBuilder();
				strBuilder.append("Could not create the relation \""+relationName+"\" between \""+primaryName+"\" and \""+secondaryName+"\".");
				
				/* *** Get the original error message *** */
				String[] origMessages = creRelResponse.serviceData.getPartialError(0).getMessages();
				if(origMessages.length >=1){
					strBuilder.append("\n\n"+origMessages[0]);
				}
				
				MessageDialog.openError(shell,title,strBuilder.toString());
				throw new TCException("SBT - SerializationRestructureOperaionError:\n"+strBuilder.toString());
			}
		}
		else if(actionCode == 20){
			/* *** If we get an error, throw an exception and inform the user *** */
			ServiceData servData = service.deleteRelations(new Relationship[]{relationship});
			if(servData.sizeOfPartialErrors() >= 1 || servData.sizeOfDeletedObjects() == 0){
				Shell shell = Display.getCurrent().getActiveShell();
				String title = "Delete Relation Error";
				String primaryName = primary.toDisplayString();
				String secondaryName = secondary.toDisplayString();
				StringBuilder strBuilder = new StringBuilder();
				strBuilder.append("Could not delete the relation \""+relationName+"\" between \""+primaryName+"\" and \""+secondaryName+"\".");
				
				/* *** Get the original error message *** */
				String[] origMessages = servData.getPartialError(0).getMessages();
				if(origMessages.length >=1){
					strBuilder.append("\n\n"+origMessages[0]);
				}
				
				MessageDialog.openError(shell,title,strBuilder.toString());
				throw new TCException("SBT - SerializationRestructureOperaionError:\n"+strBuilder.toString());
			}
		}
	}
	
	private TCComponentDataset createDataset(TCComponent validECOObj){
		TCComponentDataset newDataset = null;
		if(validECOObj != null && this.session != null){
			if(validECOObj instanceof TCComponentItemRevision){
				try {
					/* *** Create the dataset name *** */
					TCComponentItemRevision ecoRev = (TCComponentItemRevision)validECOObj;
					String datasetName = ecoRev.getItem().getProperty("item_id")+ " - " + SerializationRestructureOperation.SERIALIZATION_TASK_NAME;

					/* *** Create the dataset itself *** */
					TCComponentDatasetType dsType = (TCComponentDatasetType) this.session.getTypeComponent("Dataset");
					newDataset = dsType.create(datasetName,datasetName,"Dataset ID","Dataset Rev", "TEXT", null);
				} 
				catch (TCException e) {
					e.printStackTrace();
					Shell shell = Display.getCurrent().getActiveShell();
					MessageDialog.openError(shell,"Change Order Task","EXCEPTION: " + e.getMessage());
				}
			}
		}
		return newDataset;
	}
	
}
