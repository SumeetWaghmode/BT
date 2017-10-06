package com.accenture.sbt.rac.tools;

import java.math.BigInteger;
import java.util.HashMap;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItem;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentQuery;
import com.teamcenter.rac.kernel.TCComponentQueryType;
import com.teamcenter.rac.kernel.TCQueryClause;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.services.rac.core.DataManagementService;
import com.teamcenter.services.rac.core._2006_03.DataManagement.CreateRelationsResponse;
import com.teamcenter.services.rac.core._2006_03.DataManagement.Relationship;
import com.teamcenter.services.rac.core._2008_06.DataManagement.CreateIn;
import com.teamcenter.services.rac.core._2008_06.DataManagement.CreateInput;
import com.teamcenter.services.rac.core._2008_06.DataManagement.CreateResponse;

public class CreateDocument extends AbstractHandler {

	private static String MSG_TITLE = "Create BT Document";
	private static String REVISION = "revision";
	private static String IMANQUERY = "ImanQuery";
	private static String QUERY_NAME = "Item...";
	private static String BT7_DOC = "BT7_Document";
	private static String BT7_DOCREV = "BT7_DocumentRevision";
	private static String REFERENCE_RELATION = "IMAN_reference";

	private static String PROP_ITEM_ID = "item_id";
	private static String PROP_OBJECT_NAME = "object_name";
	private static String PROP_OBJECT_DESC = "object_desc";
	private static String PROP_BT7_DOCSUBTYPE = "bt7_docsubtype";
	private static String PROP_BT7_DOCORGTYPE = "bt7_docorgtype";
	private static String PROP_BT7_DOCEQPARTNO = "bt7_doceqpartno";
	private static String PROP_BT7_DOCESAPMANDO = "bt7_docsapmando";
	private static String PROP_BT7_DOCPUBLISHPIP = "bt7_docpublishpip";

	IWorkbenchWindow window;

	public CreateDocument() {
	}

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		InterfaceAIFComponent selectedComp = null;
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);

		// Get the selected components
		try {
			final TCSession session = (TCSession) AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getSession();

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) {
				warningMessage("Please select exactly one ItemRevision");

				return null;
			}

			selectedComp = AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getTargetComponent();

			((TCComponent) selectedComp).refresh();

			// Selected component is ItemRevision ?
			if (selectedComp instanceof TCComponentItemRevision) {
				TCComponentItemRevision inputComp = (TCComponentItemRevision) selectedComp;

				// Get Item ID
				String sItemId = inputComp.getItem().getStringProperty(
						PROP_ITEM_ID);
				String sItemIdSuffix = "";
				boolean bItemIdIsBlocked = true;
				
				// Is TNS Number?
				if(sItemId.matches("^((A5W|A5Q)\\d{8})$"))
				{
					// Init query
					TCComponentQueryType queryType = (TCComponentQueryType) session
							.getTypeComponent(IMANQUERY);
					TCComponentQuery query = (TCComponentQuery) queryType
							.find(QUERY_NAME);
					String itemIdUserEntryName = "";
	
					// Query exists ?
					if (query == null) {
						errorMessage("Query '" + QUERY_NAME + "' not found!");
	
						return null;
					}
	
					for (TCQueryClause tcQueryClause : query.describe()) {
						if (tcQueryClause.getAttributeName().equalsIgnoreCase(
								PROP_ITEM_ID)) {
							itemIdUserEntryName = tcQueryClause
									.getUserEntryNameDisplay();
							break;
						}
					}
	
					// Loop suffixes
					while (bItemIdIsBlocked == true) {
						// Generate next suffix
						sItemIdSuffix = nextChar(sItemIdSuffix);
	
						// Search for ItemID
						int queryResultCount = query.count(
								new String[] { itemIdUserEntryName },
								new String[] { sItemId + sItemIdSuffix });
	
						System.out.println("Result count for ItemID '"
								+ (sItemId + sItemIdSuffix) + "'" + " = "
								+ queryResultCount);
	
						if (queryResultCount == 0) {
							bItemIdIsBlocked = false;
						}
					}
	
					// Confirmation message
					if (!questionMessage("Do you want to create the document '"
							+ sItemId + sItemIdSuffix + "' ?")) {
						return null;
					}
	
					// Create Item
					DataManagementService dmService = DataManagementService
							.getService(session);
	
					CreateIn soaCreateDef = new CreateIn();
	
					soaCreateDef.data.boName = BT7_DOC;
	
					// Set client id
					soaCreateDef.clientId = new Integer(1).toString();
	
					HashMap<String, String> stringPropMap = new HashMap<String, String>();
					stringPropMap.put(PROP_ITEM_ID, sItemId + sItemIdSuffix);
					stringPropMap.put(PROP_OBJECT_NAME, "Doc for Part '" + sItemId
							+ "'");
					stringPropMap.put(PROP_OBJECT_DESC, "Document for Part '"
							+ sItemId + sItemIdSuffix + "'");
	
					soaCreateDef.data.stringProps = stringPropMap;
	
					// Create a revision input
					HashMap<String, CreateInput[]> secondaryCreateInputs = new HashMap<String, CreateInput[]>();
					CreateInput secondarySOACreateDef = new CreateInput();
					secondarySOACreateDef.boName = BT7_DOCREV;
	
					// Set up a map for the revision properties
					HashMap<String, String> secondaryStringProps = new HashMap<String, String>();
					secondaryStringProps.put(PROP_BT7_DOCSUBTYPE, "-");
					secondaryStringProps.put(PROP_BT7_DOCORGTYPE, "-");
					secondaryStringProps.put(PROP_BT7_DOCEQPARTNO, "-");
	
					HashMap<String, BigInteger> secondaryIntProps = new HashMap<String, BigInteger>();
					secondaryIntProps.put(PROP_BT7_DOCESAPMANDO,
							new BigInteger("0"));
	
					HashMap<String, Boolean> secondaryBoolProps = new HashMap<String, Boolean>();
					secondaryBoolProps.put(PROP_BT7_DOCPUBLISHPIP, true);
	
					// Set this map on the secondary create definition
					secondarySOACreateDef.stringProps = secondaryStringProps;
					secondarySOACreateDef.intProps = secondaryIntProps;
					secondarySOACreateDef.boolProps = secondaryBoolProps;
	
					// Put this in the secondary create input map to be set on the
					// primary create definition
					secondaryCreateInputs.put(REVISION,
							new CreateInput[] { secondarySOACreateDef });
	
					// Set the compond create input map on the primary create
					// definition
					soaCreateDef.data.compoundCreateInput = secondaryCreateInputs;
	
					// Make the call to create the objects
					CreateResponse response = dmService
							.createObjects(new CreateIn[] { soaCreateDef });
	
					// Check for errors
					if (response.serviceData.sizeOfPartialErrors() > 0) {
						String[] messageArray = response.serviceData
								.getPartialError(0).getMessages();
						String message = new String();
						for (int i = 0; i < messageArray.length; i++) {
							message += messageArray[i] + "\n";
						}
	
						errorMessage("Create-Item error: " + message);
	
						return null;
					}
	
					// Get the array of newly created components
					TCComponent[] responseComponents = response.output[0].objects;
	
					// Get the newly create item
					TCComponentItem docItem = (TCComponentItem) responseComponents[0];
	
					// Get the item revision from the item just created
					TCComponentItemRevision docItemRev = docItem
							.getLatestItemRevision();
	
					// Create relation between Item and Document
					Relationship[] relations = new Relationship[1];
					relations[0] = new Relationship();
					relations[0].clientId = new Integer(1).toString();
					relations[0].primaryObject = inputComp;
					relations[0].secondaryObject = docItemRev;
					relations[0].relationType = REFERENCE_RELATION;
	
					CreateRelationsResponse createRelationsResponse = dmService
							.createRelations(relations);
	
					// Check for errors
					if (createRelationsResponse.serviceData.sizeOfPartialErrors() > 0) {
						String[] messageArray = createRelationsResponse.serviceData
								.getPartialError(0).getMessages();
						String message = new String();
						for (int i = 0; i < messageArray.length; i++) {
							message += messageArray[i] + "\n";
						}
	
						errorMessage("Create-Relation error: " + message);
	
						return null;
					}
	
					inputComp.refresh();
	
					// Success message
					infoMessage("Document '" + sItemId + sItemIdSuffix
							+ "' successfully created");
				} else {
					// No TNS
					warningMessage("Item ID is not a TNS (A5Wnnnnnnnn | A5Q2nnnnnnn)");					
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			errorMessage("EXCEPTION: " + e.getMessage());
		}

		return null;
	}

	void errorMessage(String sMessage, Object... objList) {
		MessageDialog.openError(window.getShell(), MSG_TITLE,
				String.format(sMessage, objList));
	}

	void infoMessage(String sMessage, Object... objList) {
		MessageDialog.openInformation(window.getShell(), MSG_TITLE,
				String.format(sMessage, objList));
	}

	void warningMessage(String sMessage, Object... objList) {
		MessageDialog.openWarning(window.getShell(), MSG_TITLE,
				String.format(sMessage, objList));
	}

	boolean questionMessage(String sMessage, Object... objList) {
		return MessageDialog.openQuestion(window.getShell(), MSG_TITLE,
				String.format(sMessage, objList));
	}

	String nextChar(String s) {
		int length = s.length();
		char c = ' ';
		if (length > 0) {
			c = s.charAt(length - 1);
		}

		if (c == 'z')
			return (length > 1 ? nextChar(s.substring(0, length - 1)) + 'a'
					: "aa").toUpperCase();

		return (length > 0 ? s.substring(0, length - 1) + ++c : "a")
				.toUpperCase();
	}
}
