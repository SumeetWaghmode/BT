package com.accenture.rac.wizards.importer.bom;

import java.io.IOException;

import com.accenture.rac.wizards.importer.RowStatus;
import com.accenture.rac.wizards.importer.spec.IColumn;
import com.accenture.rac.wizards.importer.spec.IRow;
import com.teamcenter.rac.kernel.TCComponentItemRevision;

public class BomLineCopyPropertiesImportProvider extends AbstractBomLineImportProvider {

	public BomLineCopyPropertiesImportProvider(Class<? extends IRow> rowClazz) throws IOException {
		super(rowClazz);
	}

	@Override
	public void read() throws Exception {
		TCComponentItemRevision masterItemRevision = (TCComponentItemRevision) getSource();
		
		BomLineService targetBomService = (BomLineService) getRowService();
		BomLineService masterBomService = new BomLineService(masterItemRevision);
		masterBomService.setRowClazz(targetBomService.getRowClazz());
		
		targetBomService.getTCProperties().clear();
		masterBomService.getTCProperties().clear();
		for (IColumn column : columns) {
			if(column.isEnabled() || column.isImplicit()){
				targetBomService.addTCProperty(column.getPropertyName(), column.getIntName());
				masterBomService.addTCProperty(column.getPropertyName(), column.getIntName());
			}
		}
		
		targetBomService.readBom();
		masterBomService.readBom();
		
		rows.clear();
		for (IRow targetLine : targetBomService.getBomChildren()) {
			masterBomService.setRowStatus(targetLine);
			switch (targetLine.getStatus()) {
			case UPDATE:
				targetLine = masterBomService.getChildById((BomLine) targetLine);
				targetLine.setStatus(RowStatus.UPDATE);
				break;
			case INSERT:
				targetLine.setStatus(RowStatus.IGNORE);
				targetLine.setStatusDescription("Item not found in BOM reference");
			default:
				break;
			}
			
			targetLine.setEnabled(true);
			rows.add(targetLine);
		}
	}
}
