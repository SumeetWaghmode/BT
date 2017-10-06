package com.accenture.sbt.rac.serialization;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import com.teamcenter.rac.aif.AbstractAIFOperation;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.services.rac.core.DataManagementService;
import com.teamcenter.services.rac.core._2007_06.DataManagement;
import com.teamcenter.services.rac.core._2007_06.DataManagement.ExpandGRMRelationsPref;
import com.teamcenter.services.rac.core._2007_06.DataManagement.ExpandGRMRelationsResponse;


public class SerializationValidationOperation extends AbstractAIFOperation {
	
	private ArrayList<TCComponentItemRevision> itemRevList = new ArrayList<TCComponentItemRevision>();
	private SerializationValidationResult valResult = new SerializationValidationResult();
	
	public SerializationValidationOperation(ArrayList<TCComponentItemRevision> selectedArrayList){
		super();
		this.itemRevList = selectedArrayList;
	}
	
	public SerializationValidationResult getValidationResult(){
		return this.valResult;
	}

	@Override
	public void executeOperation() throws Exception {
		/* *** Starting validation on each incoming object *** */
		/* *** Check if one of the selected objects is the first revision *** */
		for(TCComponentItemRevision check1ItemRev : itemRevList){
			String revID = check1ItemRev.getStringProperty("item_revision_id");
			if(revID.equals("A")){
				this.valResult.addFirstRefErrObj(check1ItemRev);
			}
		}
		if(this.valResult.hasFirstRevError()){
			this.valResult.setValError(true);
			this.valResult.setErrorHeader("Selection Error");
			this.valResult.setErrorCode(SerializationValidationResult.FIRST_REVISION_ERROR);
		}
		else{
			/* *** Check if any of the selected objects is already a solution item revision for an ECO *** */
			for(TCComponentItemRevision check2ItemRev : itemRevList){
				TCComponent wrongECORev = getFirstRelatedObject(check2ItemRev,2,"CMHasSolutionItem");
				if(wrongECORev != null){
					this.valResult.addIsSolItemErrObjs(check2ItemRev,wrongECORev);
				}
			}
			if(this.valResult.hasSolItemError()){
				this.valResult.setValError(true);
				this.valResult.setErrorCode(SerializationValidationResult.IS_SOLUTION_ITEM_ERROR);
			}
			else{
				/* *** Get ECO objects via the predecessor item revision. If one of the given item revision does not *** */
				/* *** have an ECO obj, put a error message. 														 *** */
				HashMap<TCComponent,TCComponent> foundECOObjMap = new HashMap<TCComponent,TCComponent>();
				for(TCComponentItemRevision check3ItemRev : itemRevList){
					TCComponent ecoObj = getECOViaPredecessor(check3ItemRev);
					if(ecoObj == null){
						this.valResult.addNoECOObjErrObj(check3ItemRev);
					}
					else{
						foundECOObjMap.put(check3ItemRev,ecoObj);
					}
					
				}
				if(this.valResult.hasNoECOError()){
					this.valResult.setValError(true);
					this.valResult.setErrorCode(SerializationValidationResult.NO_ECO_OBJECT_ERRROR);
				}
				else if(!foundECOObjMap.isEmpty()){
					/* *** Check if the found ECO objects are not different (must be the same instance for each *** */
					/* *** selected item revision).                                                             *** */
					TCComponent firstKey = (TCComponent)foundECOObjMap.keySet().toArray()[0];
					TCComponent compareECO = foundECOObjMap.get(firstKey);
					for(Map.Entry<TCComponent, TCComponent> entry : foundECOObjMap.entrySet()) {
						TCComponent currentECO = entry.getValue();
						if(!compareECO.equals(currentECO)){
							this.valResult.setDifferentECOMap(foundECOObjMap);
							break;
						}
					}
					if(this.valResult.hasDifferentECOError()){
						this.valResult.setValError(true);
						this.valResult.setErrorCode(SerializationValidationResult.HAS_DIFFERENT_ECO_ERROR);
					}
					else{
						/* *** Check if the valid ECO object has the correct status named "5" *** */
						if(!isObjecstLatestStatusValid(compareECO,"5")){
							this.valResult.setValError(true);
							this.valResult.setErrorCode(SerializationValidationResult.ECO_NOT_VALID_STATUS);
							this.valResult.setProblemObj(compareECO);
						}
						else{
							/* *** Found ECO obj is valid - set it on the result now *** */
							this.valResult.setSetValidECOObj(compareECO);
							
							/* *** Check if every selected object has the correct status named "10" *** */
							for(TCComponentItemRevision check4ItemRev : itemRevList){
								if(!isObjecstLatestStatusValid(check4ItemRev,"10")){
									this.valResult.addRemObjList(check4ItemRev);
								}
								else{
									this.valResult.addOKObjList(check4ItemRev);
								}
								
								if(this.valResult.getOKObjList().isEmpty()){
									this.valResult.setValError(true);
									this.valResult.setErrorCode(SerializationValidationResult.NO_SEL_OBJ_WITH_VALID_STATUS);
								}
								else if(this.valResult.hasRemovedObjects()){
									this.valResult.setSelError(true);
								}
							}
						}
					}
				}
			}
		}
	}
	
	public TCComponent getFirstRelatedObject(TCComponent component,int side,String relName){
		if(component == null){
			return null;
		}
		else{
			TCSession session = component.getSession();
			DataManagementService service = DataManagementService.getService(session);
			ExpandGRMRelationsPref pref = new ExpandGRMRelationsPref();
			pref.info = new DataManagement.RelationAndTypesFilter2[1];
			pref.info[0] = new DataManagement.RelationAndTypesFilter2();
			pref.info[0].relationName = relName;
			
			ExpandGRMRelationsResponse response = null;
			if(side == 1){
				response = service.expandGRMRelationsForPrimary(new TCComponent[]{component},pref);
			}
			else if(side == 2){
				response = service.expandGRMRelationsForSecondary(new TCComponent[]{component},pref);
			}
			if(response != null){
				if (response.output.length == 0){
					return null;
				}
				if (response.output[0].otherSideObjData.length == 0){
					return null;
				}
				if (response.output[0].otherSideObjData[0].otherSideObjects.length == 0){
					return null;
				}
				return response.output[0].otherSideObjData[0].otherSideObjects[0];
			}
			else{
				return null;
			}
		}
	}
	
	public TCComponent getECOViaPredecessor(TCComponent comp){
		TCComponent ecoObj = null;
		TCComponent predecessorToCut = null;
		TCComponent predecessorRev = getFirstRelatedObject(comp,1,"IMAN_based_on");
		if(predecessorRev != null){
			ecoObj = getFirstRelatedObject(predecessorRev,2,"CMHasSolutionItem");
			if(ecoObj != null){
				predecessorToCut = predecessorRev;
			}
			else{
				/* *** Recursive call to find an ECO object on the predecessor revisions *** */
				return getECOViaPredecessor(predecessorRev);
			}
		}
		else{
			return null;
		}
		
		/* *** Add the relevant obj to the map *** */
		/* *** key = selected item - which should be later pasted to the ECO as solution item *** */
		/* *** value  = predecessor item - which should be later cut from ECO and be pasted as Impacted Item to the same ECO *** */
		if(ecoObj != null){
			this.valResult.addCutPastObjs(comp,predecessorToCut);
		}
		return ecoObj;
	}
	
	public boolean isObjecstLatestStatusValid(TCComponent comp,String validStatName){
		try {
			if(comp != null){
				TCComponent[] statArray = comp.getReferenceListProperty("release_status_list");
				if(statArray.length >= 1){
					String latestStateName = statArray[statArray.length -1].getStringProperty("object_name");
					if(latestStateName != null && latestStateName.equals(validStatName)){
						return true;
					}
				}
			}
		} 
		catch (TCException e) {
			e.printStackTrace();
		}
		return false;
	}

}
