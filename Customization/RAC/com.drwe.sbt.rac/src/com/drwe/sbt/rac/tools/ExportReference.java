package com.drwe.sbt.rac.tools;

import java.io.File;

//import javax.swing.JFileChooser;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.aifrcp.AIFUtility;
import com.teamcenter.rac.kernel.NamedReferenceContext;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentItem;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentTcFile;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.util.Registry;
import org.apache.log4j.Logger;

public class ExportReference extends AbstractHandler {

	final static String M0001 = "error on selected component";
	private static final Logger logger = Logger.getLogger(ExportReference.class);

	IWorkbenchWindow window;
	Registry reg;
	String prefNAMERULE;
	String prefdelimiter;

	/**
	 * The constructor.
	 */
	public ExportReference() {
	}

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		InterfaceAIFComponent selComps[];
		
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		reg = Registry.getRegistry(this);
		prefNAMERULE = this.reg.getString("ExportReference.NAMERULE", "WITH_ORIGINAL_NAME");
		prefdelimiter = this.reg.getString("ExportReference.delimiter", "_");
		new AIFUtility();
		selComps = AIFUtility.getTargetComponents();
		// 2011/08 GW change to ((TCComponent)selComps[i]).getUnderlyingComponent()
		logger.debug(String.format("selected components: %d", selComps.length));
		for (int i = 0; i < selComps.length; i++) {
			try {
				logger.debug(String.format("selected component: %s", selComps[i].toString()));
				logger.debug(String.format("selected component class: %s", selComps[i].getClass().getName()));
				logger.debug(String.format("selected component: Underlying type %s", ((TCComponent)selComps[i]).getUnderlyingComponent().getTCClass().getClassName()));
				// nur für Dataset Objekte
				if (((TCComponent)selComps[i]).getUnderlyingComponent().getTCClass().getClassName().equals("Dataset")) {
					getNameReference((TCComponentDataset) ((TCComponent)selComps[i]).getUnderlyingComponent());
				}
			} catch (TCException e) {
				errMessage(MessageDialog.ERROR,M0001);
			}
		}

		return null;
	}

	  public void getNameReference(TCComponentDataset theDataset)
	  {
		if (theDataset != null)
		    try
		    {
		      String strPath = showSelectPathDialog();
		      if (strPath.length() > 0) {
		        NamedReferenceContext[] references = theDataset.getDatasetDefinitionComponent().getNamedReferenceContexts();
		        TCComponentTcFile[] file = theDataset.getTcFiles();
		
		        if (references == null)
		          return;
		        for (int i = 0; i < file.length; i++) {
		            File myFile = file[i].getFile(strPath);
		            if (prefNAMERULE.equals("WITH_ORIGINAL_NAME")) {
		              File myFile2 = new File(strPath, getItemAndRevisionID(theDataset) + prefdelimiter + theDataset.getType() + prefdelimiter + file[i]);
		              myFile.renameTo(myFile2);
		            }
		            else {
		              File myFile2 = new File(strPath, getItemAndRevisionID(theDataset) + prefdelimiter + Integer.toString(i) + getFileSuffix(file[i].getPath()));
		              myFile.renameTo(myFile2);
		            }
		        }
		      }
		    }
		    catch (Exception localException) {
		    }
	  }

	  private String showSelectPathDialog() {
	    String strPath = "";
	    /*
	    JFileChooser myChooser = new JFileChooser();
	    myChooser.setApproveButtonText("Save");
	    myChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	    myChooser.showOpenDialog(window.getShell());
	    strPath = myChooser.getSelectedFile().getPath();
	    */
	    DirectoryDialog DirDialog = new DirectoryDialog(window.getShell());
	    DirDialog.setText("select a directory");
	    strPath = DirDialog.open();
	    return strPath;
	  }

	  private String getItemAndRevisionID(TCComponentDataset theDataset) {
	    String sbtName = "";
	    try {
	      AIFComponentContext[] myComponents = theDataset.whereReferenced();
	      for (int i = 0; i < myComponents.length; i++) {
	        InterfaceAIFComponent myInterfaceComponent = myComponents[i].getComponent();
	        if (!((TCComponent)myInterfaceComponent).getTCClass().getClassName().equals("ItemRevision"))
	          continue;
	        TCComponentItemRevision myItemRevision = (TCComponentItemRevision)myInterfaceComponent;
	        TCComponentItem myItem = myItemRevision.getItem();
	        sbtName = myItem.getTCProperty("item_id") + prefdelimiter + myInterfaceComponent.getProperty("item_revision_id");
	      }
	    }
	    catch (Exception exp)
	    {
	      errMessage(MessageDialog.ERROR,exp.toString());
	    }

	    return sbtName;
	  }
	
	
	  private String getFileSuffix(String strPath)
	  {
	    int index = strPath.indexOf(".");
	    String strSuffix = strPath.substring(index);
	    return strSuffix;
	  }

	  void errMessage(int MsgType, String pMMark, Object...o_list ) {
		// MessageBox
		MessageDialog.openInformation(window.getShell(),
				this.toString(),
				String.format(pMMark, o_list));
	}
}
