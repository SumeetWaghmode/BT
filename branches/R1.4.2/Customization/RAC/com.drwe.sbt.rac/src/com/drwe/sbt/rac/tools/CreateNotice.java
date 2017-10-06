package com.drwe.sbt.rac.tools;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.aifrcp.AIFUtility;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentContextList;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentDatasetType;
import com.teamcenter.rac.kernel.TCComponentType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.util.Registry;

/**
 * Our sample handler extends AbstractHandler, an IHandler base class.
 * @see org.eclipse.core.commands.IHandler
 * @see org.eclipse.core.commands.AbstractHandler
 */
public class CreateNotice extends AbstractHandler {

	final static String M0001 = "error on selected component";
	final static String M0002 = "note already available";
	final static String M0003 = "dataset was not created";
	final static String M0004 = "Error create dataset";

	IWorkbenchWindow window;
	Registry reg;
	
	/**
	 * The constructor.
	 */
	public CreateNotice() {
	}

	/**
	 * the command has been executed, so extract extract the needed information
	 * from the application context.
	 */
	public Object execute(ExecutionEvent event) throws ExecutionException {
		InterfaceAIFComponent selComps[];
		
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		reg = Registry.getRegistry(this);
		String prefNoteDatasetName = this.reg.getString("CreateNotice.NoteDatasetName", "Notiz");
		String prefNoteRelation = this.reg.getString("CreateNotice.NoteRelation", "SBT_notice");
		String prefNoteDatasetType = this.reg.getString("CreateNotice.NoteDatasetType", "SBT_Notiz");
		boolean isTypeIR = false;
		new AIFUtility();
		selComps = AIFUtility.getTargetComponents();
		for (int i = 0; i < selComps.length; i++) {
			try {
				// nur für ItemRevision Objekte
				// 2011/08 GW change to ((TCComponent)selComps[i]).getUnderlyingComponent()
				
				isTypeIR = ((TCComponent)selComps[i]).isTypeOf("ItemRevision");
				if (isTypeIR) {
					createDatasetNotiz(((TCComponent)selComps[i]).getUnderlyingComponent(), prefNoteDatasetName, "", prefNoteRelation, prefNoteDatasetType);
				}
			} catch (TCException e) {
				errMessage(MessageDialog.ERROR,M0001);
			}
		}

		return null;
	}

	  private TCComponentDataset createDatasetNotiz(InterfaceAIFComponent comp, String name, String description, String reltype, String datasettype)
	  {
	    if (FindSubDataset((TCComponent)comp, name, datasettype) != null) {
	    	errMessage(MessageDialog.INFORMATION,M0002);
	      return null;
	    }

	    TCComponentDataset dataset = InsertNewDataset((TCComponent)comp, name, description, reltype, datasettype);
	    if (dataset == null) {
	      errMessage(MessageDialog.ERROR,M0003);
	      return null;
	    }

	    return dataset;
	  }

	  private TCComponentDataset InsertNewDataset(TCComponent Root, String strName, String description, String reltype, String datasettype)
	  {
	    TCComponentDatasetType datasetType = new TCComponentDatasetType();
	    try {
	      datasetType = (TCComponentDatasetType)Root.getSession().getTypeComponent("Dataset");
	      TCComponentDataset datasetRet = datasetType.create(strName, description, datasettype);
	      datasetRet.setProperty("object_name", strName);
	      Root.add(reltype, datasetRet);
	      return datasetRet;
	    } catch (TCException ex) {
	    	errMessage(MessageDialog.ERROR,M0004);
	    	ex.dump();
	    }
	    return null;
	  }

	  private TCComponentDataset FindSubDataset(TCComponent Root, String strName, String datasettype) {
	    try {
	      TCComponentContextList list = Root.getRelatedList();
	      for (int i = 0; i < list.getListCount(); i++) {
	        TCComponent comp = (TCComponent)list.get(i).getComponent();

	        String strObjType = comp.getType().toString();
	        if (!strObjType.equals(datasettype))
	          continue;
	        String name = "";
	        try {
	          name = comp.getTCProperty("object_name").getStringValue();
	        } catch (Exception localException) {
	        }
	        if (name.equals(strName))
	          return (TCComponentDataset)comp;
	      }
	    } catch (TCException ex) {
	      ex.dump();
	    }
	    return null;
	  }
	
	void errMessage(int MsgType, String pMMark, Object...o_list ) {
		// MessageBox
		MessageDialog.openInformation(window.getShell(),
				this.toString(),
				String.format(pMMark, o_list));
	}
}
