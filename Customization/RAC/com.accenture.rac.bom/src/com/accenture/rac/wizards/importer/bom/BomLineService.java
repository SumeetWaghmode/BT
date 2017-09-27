package com.accenture.rac.wizards.importer.bom;


import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Set;

import com.accenture.rac.wizards.importer.RowStatus;
import com.accenture.rac.wizards.importer.spec.IRow;
import com.accenture.rac.wizards.importer.spec.IRowService;
import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCAccessControlService;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentBOMLine;
import com.teamcenter.rac.kernel.TCComponentBOMView;
import com.teamcenter.rac.kernel.TCComponentBOMViewRevision;
import com.teamcenter.rac.kernel.TCComponentBOMWindow;
import com.teamcenter.rac.kernel.TCComponentBOMWindowType;
import com.teamcenter.rac.kernel.TCComponentItem;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentQuery;
import com.teamcenter.rac.kernel.TCComponentQueryType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCProperty;
import com.teamcenter.rac.kernel.TCSession;

public class BomLineService implements IRowService{
	
	private TCComponentBOMLine bomTopLine;
	private TCComponentBOMWindow bomWindow = null;
	protected AIFComponentContext[] tcBomChildren;
	protected Set<BomLine> bomChildren;
	private TCComponentItemRevision itemRevision;
	private TCSession session;
	private Class<? extends IRow> rowClazz;
	private Map<String, String> tcProperties;
	
	public BomLineService() throws TCException {
		this(getItemRevision());
	}
	
	public BomLineService(Class<? extends IRow> rowClazz) throws TCException {
		this();
		this.rowClazz = rowClazz;
	}
	
	public BomLineService(TCComponentItemRevision itemRevision) {
		this.itemRevision = itemRevision;
		this.session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();
		this.bomChildren = new LinkedHashSet<BomLine>();
		this.rowClazz = BomLine.class;
		
		this.tcProperties = new HashMap<String, String>();
	}
	
	public Map<String, String> getTCProperties(){
		return this.tcProperties;
	}
	
	public void addTCProperty(String key, String value){
		this.tcProperties.put(key, value);
	}
	
	public void popTCProperty(String key){
		this.tcProperties.remove(key);
	}
	
	@Override
	public void setRowStatus(IRow row) {
		BomLine bomLine = (BomLine) row;
		
		if(bomLine.getRevisionId() != null && bomLine.getItemId() == null){
			bomLine.setStatus(RowStatus.ERROR);
			bomLine.setStatusDescription("ItemRevision updates requires ItemId");
			return;
		}
		
		//check if the item exists
		TCComponentItem tcItem = null;
		TCComponentItemRevision tcItemRevision = null;
		if(bomLine.getItemId() != null){
			try {
				tcItem = (TCComponentItem) getItemById(bomLine.getItemId());
			} catch (TCException e) {
				bomLine.setStatus(RowStatus.ERROR);
				bomLine.setStatusDescription("The Item does not exist");
				return;
			}
			
			if(tcItem == null){
				bomLine.setStatus(RowStatus.ERROR);
				bomLine.setStatusDescription("The Item does not exist");
				return;
			}
			
			
			//check if the itemRevision exists
			if(bomLine.getRevisionId() != null){
				tcItemRevision = getItemRevision(tcItem, bomLine.getRevisionId());
				if(tcItemRevision == null){
					bomLine.setStatus(RowStatus.ERROR);
					bomLine.setStatusDescription("The ItemRevision does not exist");
					return;
				}
			}else{
				try {
					tcItemRevision = tcItem.getLatestItemRevision();
				} catch (TCException e) {
					bomLine.setStatus(RowStatus.ERROR);
					bomLine.setStatusDescription("Latest ItemRevision not found");
				}
			}
		}
		
		// get the latest status
		if(tcItemRevision != null){
			try {
				String[] statusList= tcItemRevision.getProperty("release_status_list").split(",");
				bomLine.setLatestStatus(statusList[statusList.length-1]);
			} catch (TCException e) {
				e.printStackTrace();
			}
			
			// get the 3d related dataset
			try {
				AIFComponentContext[] children = tcItemRevision.getChildren("IMAN_specification");
				bomLine.setHas3D(false);
				for (int i = 0; i < children.length; i++) {
					if(children[i].getComponent().getType().equals("UGMASTER")){
						bomLine.setHas3D(true);
					}
				}
			} catch (TCException e) {
				e.printStackTrace();
			}
			
		}
		
		// IGNORE :: check if item exists in existing BOM
		BomLine existing = getChildByObject(bomLine);
		if( existing != null && existing.equalObject(bomLine) && existing.equalQuantity(bomLine)){
			bomLine.setStatus(RowStatus.IGNORE);
			return;
		}
		
		// UPDATE :: check if item exists in existing BOM
		if(getChildById(bomLine)!=null){
			bomLine.setStatus(RowStatus.UPDATE);
			bomLine.setEnabled(true);
			return;
		}
		
		bomLine.setStatus(RowStatus.INSERT);
		bomLine.setEnabled(true);
	}

	@Override
	public void execute(IRow row) throws Exception {
		if(!row.isEnabled())
			return;
		
		BomLine newBomLine = (BomLine)row;
		
		TCComponentItemRevision itemRevision = null;
		
		try {
			switch (newBomLine.getStatus()) {
			case INSERT:
				itemRevision = getItemRevision(newBomLine);
				TCComponentBOMLine inserted = bomTopLine.add(itemRevision.getItem(), itemRevision, new TCComponentBOMViewRevision() , false );

				// update string properties
				for (String field : tcProperties.keySet()) {
					String value = newBomLine.getPropertyValue(newBomLine, field);
					String tcProperty = tcProperties.get(field);
					
					// set only if exists
					if(value != null && inserted.isModifiable(tcProperty) && ! field.equalsIgnoreCase("revisionId") && !field.equalsIgnoreCase("itemId") ){
						inserted.setProperty(tcProperty, value);
					}
				}
			break;
			case UPDATE:
				BomLine oldBomLine = getChildById(newBomLine);
				if(oldBomLine == null){
					throw new Exception("Update error, BOM line not found: " + newBomLine);
				}
				
				TCComponentBOMLine updated = getTcChildById(newBomLine);
				
				if((newBomLine.getItemId() != null && !newBomLine.getItemId().equalsIgnoreCase(oldBomLine.getItemId()))
						|| (newBomLine.getRevisionId() != null && !newBomLine.getRevisionId().equalsIgnoreCase(oldBomLine.getRevisionId()))){

					itemRevision = getItemRevision(newBomLine);
					updated.replace(itemRevision.getItem(), itemRevision, null);
					oldBomLine.setItemId(newBomLine.getItemId());
					oldBomLine.setRevisionId(newBomLine.getRevisionId());
				}
				
				// update string properties
				for (String field : tcProperties.keySet()) {
					// update only if the value differ
					if(!oldBomLine.equals(newBomLine, field) && updated.isModifiable(tcProperties.get(field))){
						String value = newBomLine.getPropertyValue(newBomLine, field);
						if (field.compareToIgnoreCase("uom") == 0 && value.compareToIgnoreCase("each") == 0)
						{
							updated.setProperty(tcProperties.get(field), "");
						}
						else
						{
							updated.setProperty(tcProperties.get(field), (value == null) ? "" : value);
						}
					}
				}
				
			break;
			case REMOVE:
				int rIndex = 0;
				for (BomLine child : bomChildren) {
					if(child.equalObject(newBomLine) && tcBomChildren[rIndex] != null){
						TCComponentBOMLine bomLine = (TCComponentBOMLine) tcBomChildren[rIndex].getComponent();
						bomLine.cut();
						tcBomChildren[rIndex] = null;
						break;
					}
					rIndex++;
				}
			break;
	
			default:
			break;
			}
		} catch (TCException e) {
			e.printStackTrace();
			Exception customE = new Exception("TC exception, cannot process: " + newBomLine.toString(), e);
			customE.setStackTrace(e.getStackTrace());
			throw customE;
		}
		
	}
	

	@Override
	public void save() throws Exception {
		if ( bomTopLine != null ){ 
			bomTopLine.save();
			bomTopLine.refresh();
		}
		
		if ( bomWindow != null ){ 
			bomWindow.save();
			bomWindow.refresh();
		}
		
	}
	
	public BomLine getChildByObject(BomLine bomLine){
		for (BomLine child : bomChildren) {
			if(bomLine.equalObject(child)){
				return child;
			}
		}
		return null;
	}
	
	public BomLine getChildById(BomLine bomLine){
		for (BomLine child : bomChildren) {
			if(bomLine.equalId(child)){
				return child;
			}
		}
		return null;
	}
	
	public TCComponentBOMLine getTcChildById(BomLine bomLine){
		int i = 0;
		for (BomLine child : bomChildren) {
			if(bomLine.equalId(child)){
				TCComponentBOMLine tcBomLine = (TCComponentBOMLine) tcBomChildren[i].getComponent();
				return tcBomLine;
			}
			i++;
		}
		return null;
	}
	
	public Set<BomLine> getBomChildren() {
		return bomChildren;
	}

	public void setBomChildren(Set<BomLine> bomChildren) {
		this.bomChildren = bomChildren;
	}
	
	public boolean itemHasRevision(TCComponentItem item, String itemRevisionId){
		if(getItemRevision(item, itemRevisionId) != null){
			return true;
		}
		return false;
	}
	
	public TCComponentItemRevision getItemRevision(TCComponentItem item, String itemRevisionId){
		AIFComponentContext[] tcItemRevisions = null;
		String tcRevisionId = null;
		try {
			tcItemRevisions = item.getChildren();
			for (int i = 0; i < tcItemRevisions.length; i++) {
				tcRevisionId = tcItemRevisions[i].getComponent().getProperty("item_revision_id");
				if(tcRevisionId != null && tcRevisionId.equals(itemRevisionId) && tcItemRevisions[i].getComponent() instanceof TCComponentItemRevision){
					return (TCComponentItemRevision) tcItemRevisions[i].getComponent();
				}
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}
		return null;
	}
	public TCComponentItemRevision getItemRevision(BomLine bomLine) throws TCException{
		if(bomLine.getItemId() == null){}
		TCComponentItem item = (TCComponentItem) getItemById(bomLine.getItemId());
		TCComponentItemRevision itemRevision = (bomLine.getRevisionId() != null) ? getItemRevision(item, bomLine.getRevisionId()) : item.getLatestItemRevision();
		return itemRevision;
	}
	
	public TCComponentItem getItem(BomLine bomLine) throws TCException{
		TCComponentItem item = (TCComponentItem) getItemById(bomLine.getItemId());
		return item;
	}
	
	public static TCComponentItemRevision getItemRevision() throws TCException{
		InterfaceAIFComponent selectedComp = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();
		((TCComponent) selectedComp).refresh();
		if (selectedComp instanceof TCComponentItemRevision) {
			return (TCComponentItemRevision) selectedComp;
		} else if (selectedComp instanceof TCComponentBOMLine) {
			TCComponentBOMLine bomLine = ((TCComponentBOMLine) selectedComp);
			while (bomLine.parent() != null ){
				bomLine = bomLine.parent();
			}
			return bomLine.getItemRevision();
		}
		return null;
	}


	public void readBom() throws Exception {
		TCSession session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();
		TCComponentBOMWindowType bowWinType = (TCComponentBOMWindowType) session.getTypeComponent("BOMWindow");
		bomWindow = bowWinType.create(null);
		TCComponentBOMView bv = new TCComponentBOMView();
		TCComponentBOMViewRevision bvr = new TCComponentBOMViewRevision();
		bomTopLine = bomWindow.setWindowTopLine(itemRevision.getItem(), itemRevision, bv, bvr);
		readBomChildren();
	}

	public void readBomChildren() throws Exception  {
		bomChildren.clear();
		
		if ( bomTopLine.hasChildren()) {
			tcBomChildren = bomTopLine.getChildren();
			for (int i = 0; i < tcBomChildren.length; i++) {
				TCComponentBOMLine bomLine = (TCComponentBOMLine) tcBomChildren[i].getComponent();
				while(bomLine.isPacked()){
					if(bomLine.isPacked())
						bomLine.unpack();
				}
			}
			
			tcBomChildren = bomTopLine.getChildren();
			for (int i = 0; i < tcBomChildren.length; i++) {
				TCComponentBOMLine tcBomChild = (TCComponentBOMLine) tcBomChildren[i].getComponent();
				
				BomLine bomLine = null;
				
				bomLine = (BomLine) rowClazz.getConstructor().newInstance();
				
				for (String property : getTCProperties().keySet()) {
					String value = null;
					try {
						String sTcValue = tcProperties.get(property);
						if ( sTcValue.compareToIgnoreCase("bl_uom" ) == 0)
						{
							TCProperty tcUom = tcBomChild.getTCProperty(sTcValue);
							if ( tcUom == null || tcUom.getUIFValue().compareToIgnoreCase("each") == 0)
							{
								value = "each";
							}
							else
							{
								value = tcUom.getUIFValue();
							}
						}
						else
						{
							value = tcBomChild.getStringProperty(tcProperties.get(property));
						}
					} catch (TCException e) {
						// no property
					}
					
					if(value != null){
						try {
							bomLine.setPropertyValue(bomLine, property, value);
						} catch (Exception e) {
							e.printStackTrace();
						} 
					}
				}
				bomChildren.add(bomLine);
			}
		}
	}
	
	
	/**
	 * Query the TC server for an item ID
	 * @param item_id
	 * @return true if exists
	 * @throws TCException 
	 */
	public boolean itemExist(String item_id) throws TCException  {
		TCComponent[] items = getItemsById(item_id);
		if ( items != null && items.length > 0 )
			return true;
		return false;
	}
	
	public TCComponent getItemById(String item_id) throws TCException  {
		TCComponent[] items = getItemsById(item_id);
		if ( items != null && items.length > 0 )
			return items[0];
		return null;
	}

	public TCComponent[] getItemsById(String item_id) throws TCException  {
        TCComponentQuery query = null;
        TCComponentQueryType queryType;
        TCComponent[] result;
        String entry[] = { "Item ID" };
        String value[] = { item_id };
        
		queryType = (TCComponentQueryType) session.getTypeComponent("ImanQuery");
		query = (TCComponentQuery) queryType.find("Item...");
		result = query.execute(entry, value);
		if (result == null || result.length == 0) {
            return null;
        }
		return result;
	}

	/**
	 * @return the rowClazz
	 */
	public Class<? extends IRow> getRowClazz() {
		return rowClazz;
	}

	/**
	 * Set the Object to work with. <br />
	 * Overrrides the default {@link IRow}.class
	 * @param rowClazz 
	 */
	public void setRowClazz(Class<? extends IRow> rowClazz) {
		this.rowClazz = rowClazz;
	}

	@Override
	public boolean getPermission() {
		TCAccessControlService access =  ((TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession()).getTCAccessControlService();                                                                                                                                  
		boolean read = false;
		boolean write = false;
		
		try {
			read = access.checkPrivilege(itemRevision, "READ");
			write = access.checkPrivilege(itemRevision, "WRITE");
		} catch (TCException e) {
			e.printStackTrace();
		}
		return (read && write) ? true : false;
	}

	public void setRowUom(IRow row) {
		
		BomLine bomLine = (BomLine) row;
		if (bomLine.getUom() != null )
		{
			return;
		}
			
		try {
			TCComponentItem item = getItem(bomLine);
			
			if (item != null )
			{
				TCProperty tcProp = item.getTCProperty("uom_tag");
				if (tcProp == null || tcProp.getUIFValue().compareToIgnoreCase("each") == 0 )
				{
					bomLine.setUom("each");
				}
				else
				{
					bomLine.setUom(tcProp.getUIFValue());
				}
				
			}
			
		} catch (TCException e) {
			e.printStackTrace();
		}
		
		
	}
	
	
}
