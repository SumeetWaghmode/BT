/**
 * 
 */
package com.accenture.sbt.rac.tools;

import java.awt.Frame;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.beans.PropertyChangeSupport;
import java.util.List;
import java.util.Vector;

import com.teamcenter.rac.aif.AbstractAIFUIApplication;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.aifrcp.AIFUtility;
import com.teamcenter.rac.commands.copy.CopyCommand;
import com.teamcenter.rac.commands.paste.PasteCommand;
import com.teamcenter.rac.commands.paste.PasteDialog;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCException;

/**
 * @author udwivedi, dwang
 *
 */
public class BT7PasteCommand extends PasteCommand {

	/**
	 *  Default Constructor
	 */
	public BT7PasteCommand() 
	{		
	}

	/**
	 * @param ainterfaceaifcomponent
	 */
	public BT7PasteCommand(InterfaceAIFComponent[] ainterfaceaifcomponent) {
		super(ainterfaceaifcomponent);
		
	}

	/**
	 * @param targetComponent		This is the target revision to paste Dataset to.
	 * @param frame
	 * @param boolean1
	 */
	public BT7PasteCommand(InterfaceAIFComponent[] targetComponent, Frame frame, Boolean boolean1, Vector<InterfaceAIFComponent> vecOrigComps) {
		super(targetComponent, frame, boolean1);
		parent = null;
		app = null;
		source = null;	        
		parent = frame;
		mVecOrigComps = vecOrigComps;

		// Array of all Datasets pasting context (context containing pasting target revision and relation info).
		AIFComponentContext datasetComponentContext[] = buildComponentContextFromClipboard(targetComponent, null);
		
		if (targetComponent.length > 0 && (targetComponent[0] instanceof TCComponentItemRevision) )
		{
			TCComponentItemRevision rev = (TCComponentItemRevision) targetComponent[0];
			try {
				mTargetIdRev = rev.getStringProperty("item_id") + "-" + rev.getStringProperty("item_revision_id");
			} catch (TCException e) {
				e.printStackTrace();
			}
		}

		if(datasetComponentContext != null && parent != null)
		{
			String relation = (String)datasetComponentContext[0].getContext();
			if(hasVisibleAttributesOnRelation(relation))
			{
				//setRunnablePropsOnRelation(null, datasetComponentContext);
				setRunnablePropsOnRelation(datasetComponentContext);
			} 
			else
			{
				dlg = new PasteDialog(datasetComponentContext, parent, boolean1.booleanValue());
				dlg.addPropertyChangeListener(this);
				dlg.addWindowListener( new WindowAdapter() {
					
					// delete created dataset if it is not pasted correctly
					@Override
					public void windowDeactivated(WindowEvent windowevent) {
						super.windowDeactivated(windowevent);
						
//						AIFClipboard aifclipboard = AIFPortal.getClipboard();
//						aifclipboard.clearClipboard();
						
		                InterfaceAIFComponent tmpOrigComps[] = new InterfaceAIFComponent[mVecOrigComps.size()];
		                for ( int k = 0; k < mVecOrigComps.size(); k++)
		                {
		                	tmpOrigComps[k] = mVecOrigComps.elementAt(k);
		                }
		                CopyCommand resetDsCopyCommand = new CopyCommand( tmpOrigComps );
		                try {
							resetDsCopyCommand.executeModal();
						} catch (Exception e1) {
							e1.printStackTrace();
						}
		                
						
						List<TCComponent> errorComps = dlg.getErrorComponents();
						
	                    for ( int jx = 0; jx < errorComps.size(); jx++)
	                    {
	                    	//if (errorComps.elementAt(jx) instanceof TCComponentDataset)
	                    	if (errorComps.get(jx) instanceof TCComponentDataset)
	                    	{
		                    	TCComponentDataset tmpDataset = (TCComponentDataset) errorComps.get(jx);
		                    	
								try {

									AIFComponentContext[] primary = tmpDataset.getPrimary();
									int refCount = tmpDataset.getWhereReferencedCount();
									
									String sObjName = tmpDataset.getStringProperty("object_name");
									
									// check whether the deleting dataset is the original one
									boolean bFoundOrigFlag = false;
					                for ( int m = 0; m < mVecOrigComps.size(); m++)
					                {
					                	InterfaceAIFComponent OrigComp = mVecOrigComps.elementAt(m);
					                	if (OrigComp.equals(tmpDataset))
					                	{
					                		bFoundOrigFlag = true;
					                		break;
					                	}
					                }
									
					                // delete only the newly created dataset by error pasting
			                    	if ( ( primary == null || primary.length == 0 )
			                    			&& ( mTargetIdRev.equals("") || sObjName.equals("") || !mTargetIdRev.equals("") && !sObjName.equals("") && sObjName.contains(mTargetIdRev) )
			                    			&& refCount == 0
			                    			&& !bFoundOrigFlag)
			                    	{
			                    		tmpDataset.delete();
			                    	}
			                    	
								} catch (TCException e) {
									e.printStackTrace();
								}
	                    	}
	                    }
					}
						
				});
				
				setRunnable(dlg);
			}
		} 
		else if(datasetComponentContext != null)
		{
			setRunnable(new PasteRunnable(datasetComponentContext));
		}
		
		if(datasetComponentContext != null)
		{
			app = AIFUtility.getCurrentApplication();
		}
		
		propertySupport = new PropertyChangeSupport(this);

	}
	
	Vector<InterfaceAIFComponent> mVecOrigComps = new Vector<InterfaceAIFComponent>();
	String mTargetIdRev = "";
	protected AbstractAIFUIApplication app = null; // newly added as class level variable; exist in TC83, but not in TC1121
	protected PropertyChangeSupport propertySupport;// newly added as class level variable; exist in TC83, but not in TC1121

}
