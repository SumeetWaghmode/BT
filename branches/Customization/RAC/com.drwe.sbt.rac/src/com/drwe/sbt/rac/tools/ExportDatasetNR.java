package com.drwe.sbt.rac.tools;

//import javax.swing.JFileChooser;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.aifrcp.AIFUtility;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCPIEService;
import com.teamcenter.rac.util.Registry;
import org.apache.log4j.Logger;

public class ExportDatasetNR extends AbstractHandler {

	final static String M0001 = "error on selected component";
	private static final Logger logger = Logger.getLogger(ExportDatasetNR.class);

	IWorkbenchWindow window;
	Registry reg;
	String prefREVISIONRULE = "Latest Working";

	/**
	 * The constructor.
	 */
	public ExportDatasetNR() {
	}

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		InterfaceAIFComponent selComps[];
		
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
//		reg = Registry.getRegistry(this);
//		prefREVISIONRULE = this.reg.getString("ExportReference.RevisionRule", "Latest Working");
		new AIFUtility();
		selComps = AIFUtility.getTargetComponents();
		// 2011/08 GW change to ((TCComponent)selComps[i]).getUnderlyingComponent()
		logger.debug(String.format("selected components: %d", selComps.length));
		for (int i = 0; i < selComps.length; i++) {
			try {
				logger.debug(String.format("selected component: %s", selComps[i].toString()));
				logger.debug(String.format("selected component class: %s", selComps[i].getClass().getName()));
				logger.debug(String.format("selected component: Underlying type %s", ((TCComponent)selComps[i]).getUnderlyingComponent().getTCClass().getClassName()));
				// ?? ob BOMLine to IR
				/*
				if (((TCComponent)selComps[i]).getUnderlyingComponent().getTCClass().getClassName().equals("BOMLine")) {
					// get Rev. Tag
					
				}
				*/
				startExport((TCComponent) ((TCComponent)selComps[i]).getUnderlyingComponent());
			} catch (TCException e) {
				errMessage(MessageDialog.ERROR,M0001);
			}
		}

		return null;
	}

	  public void startExport(TCComponent theIR)
	  {
		  String uids[] = null;
		  String uid0[] = new String[1];
		  TCComponent comp0[] = new TCComponent[1];
		  /*
		  public java.io.File exportObjectsToPLMXML(java.lang.String contextStr,
				                                            java.lang.String directory,
				                                            java.lang.String fileName,
				                                            java.lang.String revRule,
				                                            java.lang.String[] uIDsToExport)
				                                     throws TCException
		  */
		  
		if (theIR != null)
			// tag in array
			comp0[0] = theIR;
			// get UID
			uids = TCComponent.getComponentUids(comp0);
			// only first selection
			uid0[0] = uids[0];
		    try
		    {
		      String strPath = showSelectPathDialog();
		      if (strPath.length() > 0 && uids != null && uids[0].length() > 0) {
		    	  new TCPIEService().exportObjectsToPLMXML("SBT_EXP_BOM_NR_tm",strPath,"ExportBOM.xml",prefREVISIONRULE,uid0);
		      }
		    }
		    catch (Exception TCException) {
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

	  void errMessage(int MsgType, String pMMark, Object...o_list ) {
		// MessageBox
		MessageDialog.openInformation(window.getShell(),
				this.toString(),
				String.format(pMMark, o_list));
	}
}
