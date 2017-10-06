package com.accenture.sbt.rac.dialog;

import java.util.ArrayList;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.wb.swt.ResourceManager;

import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCProperty;


public class BT7AssignAltMaterial extends Shell {
	
	TCComponent[] SBTMaterial= null;
	private Table table;
	ArrayList<String > alternate = new ArrayList<>();
	// Stores selected alternate material TCComponent
	ArrayList<TCComponent > alternateMatTCComp = new ArrayList<>();
	// Stores selected alternate material from Table
	ArrayList<String > SelectedAlternate = new ArrayList<>(); 
	Button btnCancel;
	TCComponent partRevision=null;
	
	/**
	 * Launch the application.
	 * @param args
	 * @wbp.parser.constructor
	 */
	
	public BT7AssignAltMaterial(final Display display ,TCComponent[] Material,TCComponent tccomp ) {
		super(display, SWT.CLOSE | SWT.MIN | SWT.TITLE | SWT.APPLICATION_MODAL | SWT.CENTER);
		setImage(ResourceManager.getPluginImage("com.accenture.sbt.rac", "icons/tc_core.ico"));
		partRevision = tccomp;
		table = new Table(this, SWT.BORDER | SWT.CHECK | SWT.FULL_SELECTION |SWT.H_SCROLL |SWT.V_SCROLL);
		table.setBounds(22, 31, 253, 194);
		table.setHeaderVisible(true);
		table.setLinesVisible(true);
		
		TableColumn tblclmnPreferredAlternateMaterial = new TableColumn(table, SWT.NONE);
		tblclmnPreferredAlternateMaterial.setWidth(249);
		tblclmnPreferredAlternateMaterial.setText("Preferred  - Alternate material");
		
		btnCancel = new Button(this, SWT.NONE);
		btnCancel.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
			display.getActiveShell().dispose();		
			}
		});
		
		btnCancel.setBounds(207, 241, 68, 23);
		btnCancel.setText("Cancel");
		
		Button btnOk = new Button(this, SWT.NONE);
		// Ok button action
		btnOk.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) 
			{
				try 
				{	// Store temporary alternate material
					ArrayList<TCComponent > TempalternateMat = new ArrayList<>();
					TCComponent[] altArr=null;
					for (int i = 0; i < table.getItemCount(); i++) 
					{
						Boolean check = table.getItem(i).getChecked();
						if (check) 
						{
							String AltMatText = table.getItem(i).getText();
							SelectedAlternate.add(AltMatText);
						}
					}

					for (int m = 0; m < SelectedAlternate.size(); m++) 
					{
						String selected = SelectedAlternate.get(m);
						for (int n = 0; n < alternateMatTCComp.size(); n++) 
						{
							TCComponent selectedComp = alternateMatTCComp.get(n);
							String str = selectedComp.getProperty("object_string");
							if(str.equalsIgnoreCase(selected))
							{
								TempalternateMat.add(selectedComp);								
							}
						}
					}
					// Set Property
					if(TempalternateMat.size()>0)
					{
						altArr = TempalternateMat.toArray(new TCComponent[TempalternateMat.size()]);
						TCProperty tcprop = partRevision.getTCProperty("bt7_AltMaterial");
						tcprop.setReferenceValueArray(altArr);	
					}					
				display.getActiveShell().dispose();
			} 
			catch (TCException e1)
			{
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
		});
		btnOk.setBounds(121, 241, 68, 23);
		btnOk.setText("OK");
		
		final Button btnSelectAll = new Button(this, SWT.CHECK);
		btnSelectAll.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				
				boolean select = btnSelectAll.getSelection();
				
				if(select)
				{
					 for (int i = 0; i < table.getItemCount(); i++) 
					 {
						 table.getItem(i).setChecked(true);			            
					 }
				}
				if(!select)
				{
					 for (int i = 0; i < table.getItemCount(); i++) 
					 {
						 table.getItem(i).setChecked(false);			          
					 }
				}
			}
		});
		btnSelectAll.setBounds(22, 10, 85, 16);
		btnSelectAll.setText("Select All");		
		alternate = getAlternateMaterial(Material);
		createContents();
	}
	/**
	 * Create contents of the shell.
	 */
	protected void createContents() {
		setText("Assign Alternate Material");
		setSize(306, 301);
		
		for (int k=0 ; k < alternate.size();k++)
		{
			TableItem tabItem = new TableItem(table,SWT.NONE);
			boolean checked = false;
			tabItem.setText(alternate.get(k));			
		}
	}

	// Get alternate material of Material
	public ArrayList<String> getAlternateMaterial(TCComponent[] Material)
	{
		ArrayList<String> AltMatlist = new ArrayList<String>();
		String temp = new String();
		for(int i=0; i<Material.length;i++)
		{
			try 
			{
				TCComponent mat = Material[i];
				// Get from relation
				TCComponent[] AltMat= mat.getRelatedComponents("Mat1HasAlternateMaterial");
				
				for(int j=0;j < AltMat.length;j++)
				{
					temp = AltMat[j].getProperty("object_string");
					AltMatlist.add(temp); // Add names of alternate material
					alternateMatTCComp.add(AltMat[j]); // Add TCComponent to list
				}
				
			} 
			catch (TCException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}			
		}		
		return AltMatlist;			
	}

	@Override
	protected void checkSubclass() {
		// Disable the check that prevents subclassing of SWT components
	}
}
