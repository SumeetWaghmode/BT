package com.accenture.sbt.rac.dialog;

import java.util.HashMap;

import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;

import com.accenture.sbt.rac.tools.CreateVendor;
import com.teamcenter.rac.kernel.TCComponentFolderType;
import com.teamcenter.rac.kernel.TCComponentItemType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.rac.aif.AbstractAIFOperation;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.aifrcp.AIFUtility;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentContextList;
import com.teamcenter.rac.kernel.TCComponentFolder;
import com.teamcenter.rac.kernel.TCComponentItem;
import com.teamcenter.rac.kernel.TCComponentQuery;
import com.teamcenter.rac.kernel.TCComponentQueryType;
import com.teamcenter.rac.kernel.TCComponentUser;
import com.teamcenter.rac.kernel.TCComponentVendor;
import com.teamcenter.rac.util.MessageBox;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.rac.core.DataManagementService;
import com.teamcenter.services.rac.core._2006_03.DataManagement.CreateItemsResponse;
import com.teamcenter.services.rac.core._2006_03.DataManagement.ItemProperties;
import com.teamcenter.services.rac.core._2008_06.DataManagement.CreateIn;
import com.teamcenter.services.rac.core._2008_06.DataManagement.CreateInput;
import com.teamcenter.services.rac.core._2008_06.DataManagement.CreateResponse;
import org.eclipse.wb.swt.SWTResourceManager;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Monitor;

public class BT7CreateVendorPartDialogue extends Dialog {

	protected Object result;
	protected Shell shlCreateVendorPart;
	private Text Vendor_Part_ID_text;
	private Text Revision_ID_text;
	private Text Vendor_Part_Name_text;
	private Text Description_text;
	private Text Manufacturer_text;
	public String ManufacturerID =null;
	public String Vendor_Part_Revision = "BT7_VendorPartRevision";
	public String Vendor_Part = "BT7_VendorPart";
	public String query = "BT7_SBT_VendorParts";
	public String Vendor_create_titel = "Create Vendor Part ";
	TCSession session = (TCSession)AIFUtility.getCurrentApplication().getSession();
	private static DataManagementService dmService;
	private CreateItemsResponse response;

	/**
	 * Create the dialog.
	 * @param parent
	 * @param style
	 */
	public BT7CreateVendorPartDialogue(Shell parent, int style) {
		super(parent, style);
		setText("SWT Dialog");
	}

	/**
	 * Open the dialog.
	 * @return the result
	 */
	public Object open() {
		createContents();
		shlCreateVendorPart.open();
		shlCreateVendorPart.layout();
		Display display = getParent().getDisplay();
		while (!shlCreateVendorPart.isDisposed()) {
			if (!display.readAndDispatch()) {
				display.sleep();
			}
		}
		return result;
	}

	/**
	 * Create contents of the dialog.
	 */
	private void createContents() {
		shlCreateVendorPart = new Shell(getParent(), SWT.DIALOG_TRIM | SWT.RESIZE | SWT.APPLICATION_MODAL | SWT.CENTER);
		shlCreateVendorPart.setImage(SWTResourceManager.getImage(BT7CreateVendorPartDialogue.class, "/com/teamcenter/rac/common/images/BT7_Vendor.png"));
		shlCreateVendorPart.setSize(537, 375);
		shlCreateVendorPart.setText("Create Vendor Part");
		shlCreateVendorPart.setLayout(new GridLayout(1, false));
		//  set it to center 
		 Monitor primary = getParent().getDisplay().getPrimaryMonitor();
		 Rectangle bounds = primary.getBounds();
		 Rectangle rect = shlCreateVendorPart.getBounds();
		    
		 int x = bounds.x + (bounds.width - rect.width) / 2;
		 int y = bounds.y + (bounds.height - rect.height) / 2;
		    
		 shlCreateVendorPart.setLocation(x, y);
		
		Composite composite = new Composite(shlCreateVendorPart, SWT.NONE);
		composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1, 1));
		composite.setLayout(new GridLayout(1, false));
		
		Group lblVendorPartInfo = new Group(composite, SWT.NONE);
		lblVendorPartInfo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
		lblVendorPartInfo.setText("Vendor Part Information");
		lblVendorPartInfo.setLayout(new GridLayout(1, false));
		
		Composite composite_3 = new Composite(lblVendorPartInfo, SWT.NONE);
		composite_3.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1, 1));
		composite_3.setLayout(new GridLayout(1, false));
		
		Label lbl3 = new Label(composite_3, SWT.NONE);
		lbl3.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 1, 1));
		lbl3.setText("Vendor Part Number / Revision - Vendor Part Name");
		
		Composite composite_4 = new Composite(lblVendorPartInfo, SWT.NONE);
		composite_4.setLayout(new GridLayout(5, false));
		composite_4.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
		
		Vendor_Part_ID_text = new Text(composite_4, SWT.BORDER);
		Vendor_Part_ID_text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
		
		Label lb1 = new Label(composite_4, SWT.NONE);
		lb1.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));
		lb1.setText("/");
		
		Revision_ID_text = new Text(composite_4, SWT.BORDER);
		GridData gd_Revision_ID_text = new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1);
		gd_Revision_ID_text.widthHint = 38;
		Revision_ID_text.setLayoutData(gd_Revision_ID_text);
		
		Label lbl2 = new Label(composite_4, SWT.NONE);
		lbl2.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));
		lbl2.setText("-");
		
		Vendor_Part_Name_text = new Text(composite_4, SWT.BORDER);
		Vendor_Part_Name_text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
		
		Composite composite_1 = new Composite(shlCreateVendorPart, SWT.NONE);
		composite_1.setLayout(new GridLayout(2, false));
		composite_1.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
		
		Label lblDescription = new Label(composite_1, SWT.NONE);
		GridData gd_lblDescription = new GridData(SWT.CENTER, SWT.CENTER, false, false, 1, 1);
		gd_lblDescription.widthHint = 78;
		lblDescription.setLayoutData(gd_lblDescription);
		lblDescription.setText("Description :");
		
		Description_text = new Text(composite_1, SWT.BORDER);
		GridData gd_Description_text = new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1);
		gd_Description_text.heightHint = 141;
		Description_text.setLayoutData(gd_Description_text);
		
		Label lblManfacturer = new Label(composite_1, SWT.NONE);
		lblManfacturer.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1));
		lblManfacturer.setText("Manufacturer :");
		
		Manufacturer_text = new Text(composite_1, SWT.BORDER);
		Manufacturer_text.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
		Manufacturer_text.setText(getManufacturerID());
		Manufacturer_text.setEnabled(false);
		
		Composite composite_2 = new Composite(shlCreateVendorPart, SWT.NONE);
		composite_2.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
		composite_2.setLayout(new GridLayout(13, false));
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		new Label(composite_2, SWT.NONE);
		
		Button btnOk = new Button(composite_2, SWT.NONE);
		btnOk.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				try 
				{
					// Get user entered item Id
					String VItemId = Vendor_Part_ID_text.getText().trim();
					
					// Get user entered item Id
					String VRevId = Revision_ID_text.getText().trim();
					
					// Get user entered item Id
					String VName = Vendor_Part_Name_text.getText().trim();
					
					// Check All necessary feilds are filled 
					
					if(VItemId.length() != 0 && VRevId.length() != 0 && VName.length() != 0)
					{

						//query item id , if it is already exits
						
						TCComponentQueryType tccomponentquerytype = (TCComponentQueryType)session.getTypeComponent("ImanQuery");
						TCComponentQuery query1 = (TCComponentQuery)tccomponentquerytype.find(query);
						
						if (query1 != null) 
						{
							AIFComponentContext[] resultlist = query1.getExecuteResults(new String[]{"ID"}, new String[]{VItemId});
							//TCComponent comp = (TCComponent) resultlist[0].getComponent();
							if(resultlist.length >0 )
							{
								// Else - messagebox - Vendor Part already exits with same ID.
								
								MessageBox.post( "Vendor Part already exits with same ID",Vendor_create_titel, MessageBox.INFORMATION);
							
							}
							else
							{
								// if no exits create item 
								dmService = DataManagementService.getService(session);
								
								CreateIn soaCreateDef = new CreateIn();
								
								soaCreateDef.data.boName = Vendor_Part;
								// Set client id
								soaCreateDef.clientId = "Siemens";
								HashMap<String, String> stringPropMap = new HashMap<String, String>();
								stringPropMap.put("item_id", VItemId);
								stringPropMap.put("object_name", VName);
								//stringPropMap.put("item_revision_id", VRevId);
								stringPropMap.put("object_desc", Description_text.getText().trim());
								
								HashMap<String, TCComponent> secondaryIntProps = new HashMap<String, TCComponent>();
								secondaryIntProps.put("vm0vendor_ref",CreateVendor.ManfacturerItem);
								
								
								// Create a revision input
								HashMap<String, CreateInput[]> secondaryCreateInputs = new HashMap<String, CreateInput[]>();
								CreateInput secondarySOACreateDef = new CreateInput();
								secondarySOACreateDef.boName = Vendor_Part_Revision;
								// Set up a map for the revision properties
								HashMap<String, String> secondaryStringProps = new HashMap<String, String>();
								secondaryStringProps.put("item_revision_id",VRevId);
				
								// Set this map on the secondary create definition
								secondarySOACreateDef.stringProps = secondaryStringProps;
								
								// primary create definition
								secondaryCreateInputs.put("revision",new CreateInput[] { secondarySOACreateDef });
									

								soaCreateDef.data.tagProps=secondaryIntProps;
					
								soaCreateDef.data.stringProps = stringPropMap;
								
								soaCreateDef.data.compoundCreateInput = secondaryCreateInputs;
									
								// Make the call to create the objects
								CreateResponse response = dmService.createObjects(new CreateIn[] { soaCreateDef });
								
								// Check for errors
								if (response.serviceData.sizeOfPartialErrors() > 0) 
								{
									String[] messageArray = response.serviceData.getPartialError(0).getMessages();
									String message = new String();
									for (int i = 0; i < messageArray.length; i++) 
									{
										message += messageArray[i] + "\n";
									}
				
									//errorMessage("Create-Item error: " + message);
									MessageBox.post( message,Vendor_create_titel, MessageBox.ERROR);
				
									//return null;
								}
								else
								{
									//TCComponent tccomp = response.serviceData.getCreatedObject(0);
									// Get the array of newly created components
									TCComponent[] responseComponents = response.output[0].objects;
					
									// Get the newly create item
									TCComponentItem docItem = (TCComponentItem) responseComponents[0];									
									TCComponent tccomp = responseComponents[0];
									pasteInNewStuffFolder(tccomp);
									
								}
				
								// Get the array of newly created components
								//TCComponent[] responseComponents = response.output[0].objects;																										
								System.out.println("\n Test");	
								shlCreateVendorPart.close();
							}

						}	// End of IF (query check null)		
					}
					else
					{
						MessageBox.post( "Please enter mandatory feilds - Vendor Part ID , Rev ID and Name ",Vendor_create_titel, MessageBox.ERROR);
					}
					
														
				} // End of Try block
				catch (TCException | ServiceException e1) 
				{
					// TODO Auto-generated catch block
					e1.printStackTrace();					
					MessageBox.post( e1.getMessage(),Vendor_create_titel, MessageBox.ERROR);
				}
			}
		});
		GridData gd_btnOk = new GridData(SWT.RIGHT, SWT.FILL, true, false, 1, 1);
		gd_btnOk.widthHint = 55;
		btnOk.setLayoutData(gd_btnOk);
		btnOk.setText("OK");
		
		Button btnCancel = new Button(composite_2, SWT.NONE);
		btnCancel.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				
				//display.getActiveShell().dispose();
				shlCreateVendorPart.close();
			}
		});
		GridData gd_btnCancel = new GridData(SWT.RIGHT, SWT.CENTER, false, false, 1, 1);
		gd_btnCancel.widthHint = 63;
		btnCancel.setLayoutData(gd_btnCancel);
		btnCancel.setText("Cancel");

	}
	
	String getManufacturerID()
	{
		ManufacturerID = CreateVendor.ManufacturerID;
		return ManufacturerID;
	}
	
	public void pasteInNewStuffFolder(TCComponent tccomp)
	{
		try 
		{
			TCComponentUser usr=session.getUser();
			TCComponentFolder folder;
			folder = usr.getNewStuffFolder();
			AIFComponentContext localContextOfcomp;
			localContextOfcomp = new AIFComponentContext(folder,tccomp,"contents");
			folder.pasteOperation(localContextOfcomp);
		} 
		catch (TCException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

}
