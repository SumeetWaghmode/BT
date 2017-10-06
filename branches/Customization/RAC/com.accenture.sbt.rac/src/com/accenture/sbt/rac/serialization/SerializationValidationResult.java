package com.accenture.sbt.rac.serialization;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import com.teamcenter.rac.kernel.TCComponent;

public class SerializationValidationResult {
	
	static int FIRST_REVISION_ERROR = 1000;
	static int IS_SOLUTION_ITEM_ERROR = 2000;
	static int NO_ECO_OBJECT_ERRROR = 3000;
	static int HAS_DIFFERENT_ECO_ERROR = 4000;
	static int NO_SEL_OBJ_WITH_VALID_STATUS = 5000;
	static int ECO_NOT_VALID_STATUS = 6000;
	
	private boolean valError = false;
	private boolean selError = false;
	private String errorHeader = "Selection Error";
	private int	errorCode = 0;
	
	private TCComponent validECOObj = null;
	private TCComponent problemObj = null;
	private ArrayList<TCComponent> okObjList = new ArrayList<TCComponent>();
	private ArrayList<TCComponent> remObjList = new ArrayList<TCComponent>();
	private ArrayList<TCComponent> firstRefErrL = new ArrayList<TCComponent>();
	private ArrayList<TCComponent> noECOObjErrL = new ArrayList<TCComponent>();
	private HashMap<TCComponent,TCComponent> isSolItemErrMap = new HashMap<TCComponent,TCComponent>();
	private HashMap<TCComponent,TCComponent> differentECOEerrMap = new HashMap<TCComponent,TCComponent>();
	private HashMap<TCComponent,TCComponent> cutPasteToECOMap = new HashMap<TCComponent,TCComponent>();
	
	public SerializationValidationResult() {
		super();
	}
	
	public boolean hasValError() {
		return valError;
	}

	public void setValError(boolean valError) {
		this.valError = valError;
	}
	
	public boolean hasSelError() {
		return selError;
	}

	public void setSelError(boolean selError) {
		this.selError = selError;
	}

	public String getErrorHeader() {
		return errorHeader;
	}

	public void setErrorHeader(String errorHeader) {
		this.errorHeader = errorHeader;
	}

	public int getErrorCode() {
		return errorCode;
	}
	
	public void setErrorCode(int errorCode) {
		this.errorCode = errorCode;
	}
	
	public TCComponent getValidECOObj(){
		return this.validECOObj;
	}
	
	public void setSetValidECOObj(TCComponent comp){
		this.validECOObj = comp;
	}
	
	public TCComponent getProblemObj(){
		return this.problemObj;
	}
	
	public void setProblemObj(TCComponent comp){
		this.problemObj = comp;
	}
	
	public ArrayList<TCComponent> getOKObjList(){
		return okObjList;
	}

	public void addOKObjList(TCComponent okObj) {
		this.okObjList.add(okObj);
	}
	
	public ArrayList<TCComponent> getRemObjList(){
		return remObjList;
	}

	public void addRemObjList(TCComponent remObj) {
		this.remObjList.add(remObj);
	}
	
	public boolean hasRemovedObjects() {
		return isListNotEmpty(remObjList);
	}
	
	public void addFirstRefErrObj(TCComponent errorObj) {
		this.firstRefErrL.add(errorObj);
	}
	
	public boolean hasFirstRevError() {
		return isListNotEmpty(firstRefErrL);
	}

	public void addNoECOObjErrObj(TCComponent errorObj) {
		this.noECOObjErrL.add(errorObj);
	}
	
	public boolean hasNoECOError() {
		return isListNotEmpty(noECOObjErrL);
	}
	
	public void addIsSolItemErrObjs(TCComponent errorObj,TCComponent ecoObj) {
		this.isSolItemErrMap.put(errorObj,ecoObj);
	}
	
	public boolean hasSolItemError() {
		return isMapNotEmpty(isSolItemErrMap);
	}
	
	public void setDifferentECOMap(HashMap<TCComponent,TCComponent> differentECOEerrMap) {
		this.differentECOEerrMap = differentECOEerrMap;
	}
	
	public boolean hasDifferentECOError() {
		return isMapNotEmpty(differentECOEerrMap);
	}
	
	public HashMap<TCComponent,TCComponent>  getCutPastObjMap() {
		return this.cutPasteToECOMap;
	}
	
	public void addCutPastObjs(TCComponent selObj,TCComponent cutPastObj) {
		this.cutPasteToECOMap.put(selObj,cutPastObj);
	}
	
	
	
	public String getValidationErrorMessage(int errorCode){
		String serializationNegation = "Therefore you can´t start the serialization operation.";
		StringBuilder strBuilder = new StringBuilder();
		if(errorCode == SerializationValidationResult.FIRST_REVISION_ERROR){
			strBuilder.append("The selected object(s):");
			strBuilder.append(this.getPrepObjNameFromList(this.firstRefErrL));
			strBuilder.append("\nis/are the first revision.");
			strBuilder.append("\n\n");
			strBuilder.append(serializationNegation);
		}
		else if(errorCode == SerializationValidationResult.IS_SOLUTION_ITEM_ERROR){
			strBuilder.append("The selected object(s):");
			strBuilder.append(this.getPrepObjNameFromMap(this.isSolItemErrMap," is already a solution item for "));
			strBuilder.append("\n\n");
			strBuilder.append(serializationNegation);
		}
		else if(errorCode == SerializationValidationResult.NO_ECO_OBJECT_ERRROR){
			strBuilder.append("The selected object(s):");
			strBuilder.append(this.getPrepObjNameFromList(this.noECOObjErrL));
			strBuilder.append("\ndo not has a reference to an ECO object.");
			strBuilder.append("\n\n");
			strBuilder.append(serializationNegation);
		}
		else if(errorCode == SerializationValidationResult.HAS_DIFFERENT_ECO_ERROR){
			strBuilder.append("The selected object(s)refer to different ECO objects:");
			strBuilder.append(this.getPrepObjNameFromMap(this.differentECOEerrMap," refers via predecessor to "));
			strBuilder.append("\n\n");
			strBuilder.append(serializationNegation);
		}
		else if(errorCode == SerializationValidationResult.NO_SEL_OBJ_WITH_VALID_STATUS){
			strBuilder.append("The selected object(s):");
			strBuilder.append(this.getPrepObjNameFromList(this.remObjList));
			strBuilder.append("\ndoes not have a valid status.");
			strBuilder.append("\n\n");
			strBuilder.append(serializationNegation);
		}
		else if(errorCode == SerializationValidationResult.ECO_NOT_VALID_STATUS){
			strBuilder.append("The selected object(s) refers to ");
			strBuilder.append(this.getPrepObjNameFromObj(this.problemObj));
			strBuilder.append("\nThe latest status on this ECO is not valid.");
			strBuilder.append("\n\n");
			strBuilder.append(serializationNegation);
		}
		return strBuilder.toString();
	}
	
	public String getSelectionErrorMessage(){
		StringBuilder strBuilder = new StringBuilder();
		strBuilder.append("Your selection includes objects which does not have the correct status.");
		strBuilder.append("\nInvalid objects:");
		strBuilder.append(this.getPrepObjNameFromList(this.remObjList));
		strBuilder.append("\n\n");
		strBuilder.append("Valid objects:");
		strBuilder.append(this.getPrepObjNameFromList(this.okObjList));
		strBuilder.append("\n\n");
		strBuilder.append("To start the serialization operation only with the valid objects click \"OK\".");
		return strBuilder.toString();
	}
	
	public String getPrepObjNameFromObj(TCComponent comp){
		String convertedStr = "";
		if(comp != null){
			convertedStr = "\""+comp.toDisplayString()+"\"";
		}
		return convertedStr;
	}
	

	public String getPrepObjNameFromList(ArrayList<TCComponent> objList){
		StringBuilder objNameStrBuilder = new StringBuilder();
		if(objList != null && !objList.isEmpty()){
			for (TCComponent curObj : objList) {
				objNameStrBuilder.append("\n\""+curObj.toDisplayString()+"\"");
			}
		}
		return objNameStrBuilder.toString();
	}
	
	public String getPrepObjNameFromMap(HashMap<TCComponent,TCComponent> objMap,String message){
		StringBuilder objNameStrBuilder = new StringBuilder();
		for(Map.Entry<TCComponent, TCComponent> entry : objMap.entrySet()) {
			objNameStrBuilder.append("\n\""+entry.getKey().toDisplayString()+"\""+message+"\""+entry.getValue().toDisplayString()+"\"");
		}
		return objNameStrBuilder.toString();
	}
	
	public boolean isListNotEmpty(ArrayList<TCComponent> list) {
		if(!list.isEmpty()){
			return true;
		}
		else{
			return false;
		}
	}
	
	public boolean isMapNotEmpty(HashMap<TCComponent,TCComponent> map) {
		if(!map.isEmpty()){
			return true;
		}
		else{
			return false;
		}
	}
}
