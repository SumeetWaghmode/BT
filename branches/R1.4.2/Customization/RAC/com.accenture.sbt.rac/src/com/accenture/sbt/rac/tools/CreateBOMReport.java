package com.accenture.sbt.rac.tools;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.IMessageProvider;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentBOMLine;
import com.teamcenter.rac.kernel.TCComponentBOMWindow;
import com.teamcenter.rac.kernel.TCComponentBOMWindowType;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentRevisionRule;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.rac.util.Registry;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.rac.core.DispatcherManagementService;
import com.teamcenter.services.rac.core._2008_06.DispatcherManagement;
import com.teamcenter.services.rac.core._2008_06.DispatcherManagement.KeyValueArguments;

public class CreateBOMReport extends AbstractHandler {

	private class CreateBOMReportDialog extends TitleAreaDialog {

		private TCComponentItemRevision inputComp;

		private Combo comboReportType;
		private Combo comboRevisionRule;
		private Combo comboFormat;
		private Combo comboLevel;
		private Button buttonSingleReportForEveryLevel;
		private Button buttonIncludePCB;
		private Button buttonIncludeSUP;
		private String sBOMReportType;
		private String sRevisionRule;
		private String sFormat;
		private String sLevel;
		private String sFilename;
		private boolean bIsSingleReportForEveryLevel;
		private boolean bIncludePCB;
		private boolean bIncludeSUP;

		public CreateBOMReportDialog(Shell parentShell, TCComponentItemRevision inputComp) {
			super(parentShell);
			this.inputComp = inputComp;
		}

		@Override
		public void create() {
			super.create();
			setTitle(MSG_TITLE);
			setMessage("Please enter the details of the BOM Report", IMessageProvider.INFORMATION);
		}

		@Override
		protected Control createDialogArea(Composite parent) {
			Composite area = (Composite) super.createDialogArea(parent);
			Composite container = new Composite(area, SWT.NONE);
			container.setLayoutData(new GridData(GridData.FILL_BOTH));
			GridLayout layout = new GridLayout(2, false);
			container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			container.setLayout(layout);

			selectBOMReportType(container);
			selectRevisionRule(container);
			selectFormat(container);
			selectLevel(container);
			singleReportForEveryLevel(container);
			includePCB(container);
			includeSUP(container);
			return area;
		}

		private void selectBOMReportType(Composite container) {
			Label lbtReportType = new Label(container, SWT.NONE);
			lbtReportType.setText("BOM Report Type");

			GridData dataReportType = new GridData();
			dataReportType.grabExcessHorizontalSpace = true;
			dataReportType.horizontalAlignment = GridData.FILL;

			comboReportType = new Combo(container, SWT.READ_ONLY);
			comboReportType.setLayoutData(dataReportType);
			
			// Fill combox with valid BOM Report Types
			String sReportTypes = myRegistry.getString("BOM_REPORT.REPORT_TYPES");
			for (String sReportType : sReportTypes.split(";")) {
				comboReportType.add(sReportType);
			}

			// Select first entry
			if (comboReportType.getItemCount() > 0) {
				comboReportType.select(0);
			}
			//buttonIncludeSUP.setEnabled(false);
			//buttonIncludeSUP.setSelection(true);
			// Selection listener
			comboReportType.addSelectionListener(new SelectionListener() {

				@Override
				public void widgetSelected(SelectionEvent arg0) {
					// Control Format-ComboBox by the ReportType
					if (comboReportType.getText().equalsIgnoreCase(myRegistry.getString("REPORT_TYPES.EQUAL_PARTS_EQUIPMENT.NAME"))
							|| comboReportType.getText().equalsIgnoreCase(myRegistry.getString("REPORT_TYPES.WHERE_USED.NAME"))) {
						comboFormat.removeAll();
						comboFormat.add("Text");
						comboFormat.select(0);
						comboFormat.setEnabled(false);
					} else {
						comboFormat.setEnabled(true);
						initComboFormat();
					}

					// Control Level-ComboBox by the ReportType
					/*
					 * if (comboReportType .getText() .equalsIgnoreCase( myRegistry .getString("REPORT_TYPES.EQUAL_PARTS_EQUIPMENT.NAME")) ||
					 * comboReportType .getText() .equalsIgnoreCase( myRegistry .getString("REPORT_TYPES.EQUAL_PARTS.NAME"))) {
					 * comboLevel.removeAll(); comboLevel.add("1"); comboLevel.select(0); comboLevel.setEnabled(false); } else {
					 */
					comboLevel.setEnabled(true);
					initComboLevel();
					/* } */

					// Control IncludePCB
					if (comboReportType.getText().equalsIgnoreCase(myRegistry.getString("REPORT_TYPES.MD.NAME"))) {
						buttonIncludePCB.setEnabled(true);
						buttonIncludeSUP.setEnabled(false);
						buttonIncludeSUP.setSelection(true);
						
					} else {
						buttonIncludePCB.setEnabled(false);
						buttonIncludePCB.setSelection(false);
						buttonIncludeSUP.setEnabled(true);
						buttonIncludeSUP.setSelection(false);
						
					}
				}

				@Override
				public void widgetDefaultSelected(SelectionEvent arg0) {
				}
			});
		}

		private void selectRevisionRule(Composite container) {
			Label lbtRevisionRule = new Label(container, SWT.NONE);
			lbtRevisionRule.setText("Revision Rule");

			GridData dataRevisionRule = new GridData();
			dataRevisionRule.grabExcessHorizontalSpace = true;
			dataRevisionRule.horizontalAlignment = GridData.FILL;

			comboRevisionRule = new Combo(container, SWT.READ_ONLY);
			comboRevisionRule.setLayoutData(dataRevisionRule);

			// Fill combox with valid Revision Rules
			try {
				TCComponentRevisionRule[] listAllRules = TCComponentRevisionRule.listAllRules(session);
				for (TCComponentRevisionRule tcComponentRevisionRule : listAllRules) {
					comboRevisionRule.add(tcComponentRevisionRule.getProperty("object_name"));
				}

				if (comboRevisionRule.getItemCount() > 0) {
					comboRevisionRule.select(0);
				}
				// Set Default to "Latest Working" if exists
				// for (int i = 0; i < comboRevisionRule.getItemCount(); i++) {
				// if (comboRevisionRule.getItem(i).equalsIgnoreCase("Latest Working")) {
				// comboRevisionRule.select(i);
				// break;
				// }
				// }

			} catch (TCException e) {
				e.printStackTrace();
			}
		}

		private void selectFormat(Composite container) {
			Label lbtFormat = new Label(container, SWT.NONE);
			lbtFormat.setText("Report Format");

			GridData dataFormat = new GridData();
			dataFormat.grabExcessHorizontalSpace = true;
			dataFormat.horizontalAlignment = GridData.FILL;

			comboFormat = new Combo(container, SWT.READ_ONLY);
			comboFormat.setLayoutData(dataFormat);

			initComboFormat();
		}

		private void singleReportForEveryLevel(Composite container) {
			Label lbtSingleReportForEveryLevel = new Label(container, SWT.NONE);
			lbtSingleReportForEveryLevel.setText("Single Report / Level");

			GridData dataSingleReportForEveryLevel = new GridData();
			dataSingleReportForEveryLevel.grabExcessHorizontalSpace = true;
			dataSingleReportForEveryLevel.horizontalAlignment = GridData.FILL;

			buttonSingleReportForEveryLevel = new Button(container, SWT.CHECK);
			buttonSingleReportForEveryLevel.setLayoutData(dataSingleReportForEveryLevel);

			buttonSingleReportForEveryLevel.setEnabled(false);
		}

		private void includePCB(Composite container) {
			Label lblIncludePCB = new Label(container, SWT.NONE);
			lblIncludePCB.setText("Include PCBA");

			GridData dataIncludePCB = new GridData();
			dataIncludePCB.grabExcessHorizontalSpace = true;
			dataIncludePCB.horizontalAlignment = GridData.FILL;

			buttonIncludePCB = new Button(container, SWT.CHECK);
			buttonIncludePCB.setLayoutData(dataIncludePCB);

			buttonIncludePCB.setEnabled(true);
		}
		private void includeSUP(Composite container) {
			Label lblIncludeSUP = new Label(container, SWT.NONE);
			lblIncludeSUP.setText("Include Supplier Specifications");

			GridData dataIncludeSUP = new GridData();
			dataIncludeSUP.grabExcessHorizontalSpace = true;
			dataIncludeSUP.horizontalAlignment = GridData.FILL;

			buttonIncludeSUP = new Button(container, SWT.CHECK);
			buttonIncludeSUP.setLayoutData(dataIncludeSUP);
			buttonIncludeSUP.setEnabled(false);
			buttonIncludeSUP.setSelection(true);
		}
		
		private void initComboFormat() {
			comboFormat.removeAll();

			// Fill combox with valid BOM Report Formats
			comboFormat.add("PDF");
			comboFormat.add("CSV");

			// Select first entry
			if (comboFormat.getItemCount() > 0) {
				comboFormat.select(0);
			}
		}

		private void selectLevel(Composite container) {
			Label lbtLevel = new Label(container, SWT.NONE);
			lbtLevel.setText("Max. BOM Level");

			GridData dataLevel = new GridData();
			dataLevel.grabExcessHorizontalSpace = true;
			dataLevel.horizontalAlignment = GridData.FILL;

			comboLevel = new Combo(container, SWT.READ_ONLY);
			comboLevel.setLayoutData(dataLevel);

			initComboLevel();

			// Selection listener
			comboLevel.addSelectionListener(new SelectionListener() {

				@Override
				public void widgetDefaultSelected(SelectionEvent arg0) {
				}

				@Override
				public void widgetSelected(SelectionEvent arg0) {
					if (comboLevel.getText().equalsIgnoreCase("1")) {
						buttonSingleReportForEveryLevel.setEnabled(false);
						buttonSingleReportForEveryLevel.setSelection(false);
					} else {
						if (comboReportType.getText().equalsIgnoreCase(myRegistry.getString("REPORT_TYPES.WHERE_USED.NAME")) == false) {
							buttonSingleReportForEveryLevel.setEnabled(true);
						}
					}
				}
			});
		}

		private void initComboLevel() {
			comboLevel.removeAll();

			// Fill combox with BOM levels
			comboLevel.add("1");
			comboLevel.add("2");
			comboLevel.add("3");
			comboLevel.add("4");
			comboLevel.add("5");
			comboLevel.add("unlimited");

			// Select '1'
			if (comboLevel.getItemCount() > 0) {
				comboLevel.select(0);
			}
		}

		@Override
		protected boolean isResizable() {
			return true;
		}

		// save content of the Text fields because they get disposed
		// as soon as the Dialog closes
		private void saveInput() {
			sBOMReportType = comboReportType.getText();
			sRevisionRule = comboRevisionRule.getText();
			sFormat = comboFormat.getText();
			sLevel = comboLevel.getText();
			bIsSingleReportForEveryLevel = buttonSingleReportForEveryLevel.getSelection();
			bIncludePCB = buttonIncludePCB.getSelection();
			bIncludeSUP = buttonIncludeSUP.getSelection();
		}

		@Override
		protected void okPressed() {
			saveInput();

			// Verify that values not empty
			if (sBOMReportType.equalsIgnoreCase("") || sRevisionRule.equalsIgnoreCase("") || sFormat.equalsIgnoreCase("")
					|| sLevel.equalsIgnoreCase("")) {
				warningMessage("Please select a Report Type, Revision Rule, Report Format and Max. BOM Level");
			} else {
				// Check if ItemRevision contains a BOM
				TCComponentBOMWindow bomWindow = null;

				try {
					TCComponentBOMWindowType bowWinType = (TCComponentBOMWindowType) session.getTypeComponent("BOMWindow");
					bomWindow = bowWinType.create(null);

					bomWindow.setWindowTopLine(inputComp.getItem(), inputComp, null, null);
					boolean isHasChildren = bomWindow.getTopBOMLine().hasChildren();
					bomWindow.close();
					bomWindow = null;

					// Equal Parts and Equal Parts Equipment are only allowed to
					// be started on BT7_ComponentRevision
					boolean lIsEqualPartOrEqualPartEquipReport = (getBOMReportType().equalsIgnoreCase(
							myRegistry.getString("REPORT_TYPES.EQUAL_PARTS.NAME")) || getBOMReportType().equalsIgnoreCase(
							myRegistry.getString("REPORT_TYPES.EQUAL_PARTS_EQUIPMENT.NAME")));
					boolean lIsBT7ComponentRev = inputComp.getType().equalsIgnoreCase("Component Revision")
							|| inputComp.getType().equalsIgnoreCase("BT7_ComponentRevision");

					if (!lIsEqualPartOrEqualPartEquipReport || (lIsEqualPartOrEqualPartEquipReport && lIsBT7ComponentRev)) {
						// Only 'Where Used Report' is allowed to be started on
						// an
						// Item Revision without a Bill of material (BOM)
						if (isHasChildren == true || getBOMReportType().equalsIgnoreCase(myRegistry.getString("REPORT_TYPES.WHERE_USED.NAME"))) {
							// final Confirmation
							if (questionMessage("Do you want to create the report '" + getBOMReportType() + "' for the object '"
									+ inputComp.getItem().getProperty("item_id") + "/" + inputComp.getProperty("item_revision_id") + "' ?") == true) {

								// Init invalid chars
								ArrayList<String> alInvalidChars = new ArrayList<String>();
								alInvalidChars.add("<");
								alInvalidChars.add(">");
								alInvalidChars.add("?");
								alInvalidChars.add("\"");
								alInvalidChars.add(":");
								alInvalidChars.add("|");
								alInvalidChars.add("\\");
								alInvalidChars.add("/");
								alInvalidChars.add("*");
								boolean bInvaldItemId = false;

								// Init filename
								sFilename = inputComp.getItem().getProperty("item_id");

								// Replace all invalid chars
								for (String sTmpInvalidChar : alInvalidChars) {
									if (inputComp.getItem().getProperty("item_id").contains(sTmpInvalidChar)) {
										bInvaldItemId = true;
										sFilename = sFilename.replace(sTmpInvalidChar, "x");
									}
								}

								// Information about invalid ItemID
								if (bInvaldItemId) {
									// Selected ItemRevision hasn't a BOM
									infoMessage("The ItemID '"
											+ inputComp.getItem().getProperty("item_id")
											+ "' contains at least one character which is not allowed to be used within filenames - these characters will be relaced automatically by 'x'. The name of the Dataset is not affected by this operation."
											+ "\n\n" + "Invalid characters: '<', '>', '?', '\"', ':', '|', '\\', '/', '*'");
								}

								super.okPressed();
							}
						} else {
							// Selected ItemRevision hasn't a BOM
							warningMessage("Only the 'Where Used Report' is allowed to be started on an Item Revision without a Bill of Material (BOM).");
						}
					} else {
						warningMessage("The Report Types 'Equal Parts' and 'Equal Parts Equipment' are only allowed to be started on Component Revisions (BT7_ComponentRevision).");
					}
				} catch (TCException e) {
					e.printStackTrace();
				} finally {
					if (bomWindow != null) {
						try {
							bomWindow.close();
						} catch (TCException e) {
							e.printStackTrace();
						}
					}
				}
			}
		}

		public String getBOMReportType() {
			return sBOMReportType;
		}

		public String getRevisionRule() {
			return sRevisionRule;
		}

		public String getFormat() {
			return sFormat;
		}

		public String getFilename() {
			return sFilename;
		}

		public String getLevel() {
			return sLevel;
		}

		public boolean isSingleReportForEveryLevel() {
			return bIsSingleReportForEveryLevel;
		}

		public boolean includePCB() {
			return bIncludePCB;
		}
		public boolean includeSUP() {
			return bIncludeSUP;
		}
	}

	private static String MSG_TITLE = "Create BOM Report";
	private static TCSession session;
	private Registry myRegistry;
	private IWorkbenchWindow window;

	// Enable Progress Dialogue
	private String sSelectedBOMReportType;
	private String sSelectedRevisionRule;
	private String sSelectedFormat;
	private String sSelectedLevel;
	private String sIncludePCB;
	private String sIncludeSUP;
	private String sFilename;
	private String sUserName;
	private String sSelectedTranslationService;
	private TCComponentItemRevision inputComp;
	private boolean bRecursiveOperationFinished = false;
	private String sRecursiveRequestedItemIDs;

	public CreateBOMReport() {
		myRegistry = Registry.getRegistry(this);
	}

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		InterfaceAIFComponent selectedComp = null;
		window = HandlerUtil.getActiveWorkbenchWindowChecked(event);

		// Get the selected components
		try {
			session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) {
				warningMessage("Please select exactly one ItemRevision");

				return null;
			}

			selectedComp = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();

			((TCComponent) selectedComp).refresh();

			// Selected component is ItemRevision ?
			if (selectedComp instanceof TCComponentBOMLine) {
				TCComponentBOMLine bomLine = (TCComponentBOMLine) selectedComp;
				selectedComp = bomLine.getItemRevision();
			}

			// Selected component is ItemRevision ?
			if (selectedComp instanceof TCComponentItemRevision) {
				inputComp = (TCComponentItemRevision) selectedComp;

				// Open Report Dialogue
				CreateBOMReportDialog createBOMReportDialog = new CreateBOMReportDialog(window.getShell(), inputComp);
				createBOMReportDialog.create();
				if (createBOMReportDialog.open() == Window.OK) {
					sSelectedBOMReportType = createBOMReportDialog.getBOMReportType();
					sSelectedRevisionRule = createBOMReportDialog.getRevisionRule();
					sSelectedFormat = createBOMReportDialog.getFormat();
					sSelectedLevel = createBOMReportDialog.getLevel();
					sIncludePCB = createBOMReportDialog.includePCB() == true ? "true" : "false";
					sIncludeSUP = createBOMReportDialog.includeSUP() == true ? "true" : "false";
					sFilename = createBOMReportDialog.getFilename();

					sUserName = session.getUser().getUserId();
					sSelectedTranslationService = "";
					int iReportTypeCounter = 0;

					String sReportTypes = myRegistry.getString("BOM_REPORT.REPORT_TYPES");
					for (String sReportType : sReportTypes.split(";")) {
						if (sReportType.equalsIgnoreCase(sSelectedBOMReportType)) {
							break;
						}
						iReportTypeCounter++;
					}

					int iTranslationServiceCounter = 0;
					String sTranslationServices = myRegistry.getString("BOM_REPORT.TRANSLATION_TYPES");
					for (String sTranslationService : sTranslationServices.split(";")) {
						if (iTranslationServiceCounter == iReportTypeCounter) {
							sSelectedTranslationService = sTranslationService;
							break;
						}
						iTranslationServiceCounter++;
					}

					// One compined report or single reports for every level?
					if (createBOMReportDialog.isSingleReportForEveryLevel() == false) {
						// Trigger one request
						triggerRequest(inputComp, sSelectedTranslationService, sSelectedBOMReportType, sSelectedRevisionRule, sSelectedFormat,
								sUserName, sSelectedLevel, sIncludeSUP, sIncludePCB, sFilename);
						// Success Message
						infoMessage("The creation of the report has been submitted to the dispatcher queue. You will get a mail once the process is finished.");
					} else {
						// Trigger single request for every sub-bom

						IRunnableWithProgress runnable = new IRunnableWithProgress() {

							@Override
							public void run(final IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {

								try {
									monitor.beginTask("Creating recursive report requests ...", IProgressMonitor.UNKNOWN);

									// Start a separate Thread for the
									// Server-Task
									new Thread(new Runnable() {

										@Override
										public void run() {
											try {
												// Get selected
												// RevisionRule-Object
												TCComponentRevisionRule tSelectedRevisionRule = null;
												for (TCComponentRevisionRule tRevRule : TCComponentRevisionRule.listAllRules(session)) {
													if (tRevRule.getProperty("object_name").equalsIgnoreCase(sSelectedRevisionRule)) {
														tSelectedRevisionRule = tRevRule;
														break;
													}
												}

												if (tSelectedRevisionRule != null) {
													sRecursiveRequestedItemIDs = traverseBOM(tSelectedRevisionRule, inputComp,
															sSelectedTranslationService, sSelectedBOMReportType, sSelectedRevisionRule,
															sSelectedFormat, sUserName, sIncludeSUP, sIncludePCB, sFilename);
												}

											} catch (Exception e) {
												e.printStackTrace();
												// errorMessage("Exception: "
												// + e.getMessage());
											}

											bRecursiveOperationFinished = true;
										}
									}) {
									}.start();

									// Wait for the Server-Process to finish
									while (bRecursiveOperationFinished == false) {
										Thread.sleep(500);
										monitor.worked(1);
									}
								} catch (Exception e) {
									e.printStackTrace();
								} finally {
									monitor.done();
								}
							}
						};

						ProgressMonitorDialog dialog = new ProgressMonitorDialog(window.getShell());
						try {
							dialog.run(true, false, runnable);

							// Success Message
							infoMessage("The creation of the report has been submitted to the dispatcher queue. You will get a mail once the process is finished."
									+ "\n\n" + "Follwing Item Revisions have been submitted: " + sRecursiveRequestedItemIDs);
						} catch (Exception e) {
							e.printStackTrace();
							errorMessage("Exception: " + e.getMessage());
						}

					}

				}
			}
		} catch (Exception e) {
			e.printStackTrace();
			errorMessage("EXCEPTION: " + e.getMessage());
		}

		return null;
	}

	String traverseBOM(TCComponentRevisionRule tSelectedRevisionRule, TCComponentItemRevision inputComp, String sSelectedTranslationService,
			String sSelectedBOMReportType, String sSelectedRevisionRule, String sSelectedFormat, String sUserName,String sIncludeSUP, String sIncludePCB,
			String sFilename) throws ServiceException, TCException {
		boolean bReportRequested = false;
		String sItemId = inputComp.getItem().getStringProperty("item_id");
		String sItemRevId = inputComp.getStringProperty("item_revision_id");
		String sRequestedChildItems = "";

		// Traverse BOM
		TCComponentBOMWindow bomWindow = null;
		try {
			TCComponentBOMWindowType bowWinType = (TCComponentBOMWindowType) session.getTypeComponent("BOMWindow");
			bomWindow = bowWinType.create(null);
			bomWindow.setWindowTopLine(inputComp.getItem(), inputComp, null, null);
			bomWindow.setRevisionRule(tSelectedRevisionRule);
			
			// Init invalid chars
			ArrayList<String> alInvalidChars = new ArrayList<String>();
			alInvalidChars.add("<");
			alInvalidChars.add(">");
			alInvalidChars.add("?");
			alInvalidChars.add("\"");
			alInvalidChars.add(":");
			alInvalidChars.add("|");
			alInvalidChars.add("\\");
			alInvalidChars.add("/");
			alInvalidChars.add("*");
			
			sFilename = inputComp.getItem().getProperty("item_id");
			// Replace all invalid chars
			for (String sTmpInvalidChar : alInvalidChars) {
				if (inputComp.getItem().getProperty("item_id").contains(sTmpInvalidChar)) {
					sFilename = sFilename.replace(sTmpInvalidChar, "x");
				}
			}

			// Trigger request only if part has a BOM (as the 'traverse' option
			// is not available for 'Where Used' reports, we don't need to
			// implement an exception
			if (bomWindow.getTopBOMLine().hasChildren()) {
				triggerRequest(inputComp, sSelectedTranslationService, sSelectedBOMReportType, sSelectedRevisionRule, sSelectedFormat, sUserName,
						"1", sIncludeSUP, sIncludePCB, sFilename);
				bReportRequested = true;

				for (AIFComponentContext tChildLine : bomWindow.getTopBOMLine().getChildren()) {
					if (tChildLine.getComponent() instanceof TCComponentBOMLine) {
						TCComponentBOMLine bomLine = (TCComponentBOMLine) tChildLine.getComponent();
						String sTempRequestedChildItems = traverseBOM(tSelectedRevisionRule, bomLine.getItemRevision(), sSelectedTranslationService,
								sSelectedBOMReportType, sSelectedRevisionRule, sSelectedFormat, sUserName,sIncludeSUP, sIncludePCB, sFilename);
						if (sTempRequestedChildItems.equalsIgnoreCase("") == false) {
							sRequestedChildItems = sRequestedChildItems.equalsIgnoreCase("") ? sTempRequestedChildItems : sRequestedChildItems + ", "
									+ sTempRequestedChildItems;
						}
					}
				}
			}
			bomWindow.close();
			bomWindow = null;
		} catch (TCException e) {
			e.printStackTrace();
		} finally {
			if (bomWindow != null) {
				try {
					bomWindow.close();
				} catch (TCException e) {
					e.printStackTrace();
				}
			}
		}

		return bReportRequested ? sItemId + "/" + sItemRevId + (sRequestedChildItems.equalsIgnoreCase("") ? "" : ", " + sRequestedChildItems) : ""
				+ (sRequestedChildItems.equalsIgnoreCase("") ? "" : ", " + sRequestedChildItems);
	}

	boolean triggerRequest(TCComponentItemRevision inputComp, String sSelectedTranslationService, String sSelectedBOMReportType,
			String sSelectedRevisionRule, String sSelectedFormat, String sUserName, String sSelectedLevel,String sIncludeSUP, String sIncludePCB, String sFilename)
			throws ServiceException, TCException {
		//
		boolean lIsBT7ComponentRev = inputComp.getType().equalsIgnoreCase("Component Revision")
				|| inputComp.getType().equalsIgnoreCase("BT7_ComponentRevision");

		// Create dispatcher request
		DispatcherManagementService dispMgmtService = DispatcherManagementService.getService(session);

		DispatcherManagement.CreateDispatcherRequestArgs[] disArgs = new DispatcherManagement.CreateDispatcherRequestArgs[1];
		disArgs[0] = new DispatcherManagement.CreateDispatcherRequestArgs();
		disArgs[0].providerName = "Accenture";
		disArgs[0].serviceName = sSelectedTranslationService;
		disArgs[0].priority = 2;

		// Teamcenter Objects
		disArgs[0].primaryObjects = new TCComponent[] { inputComp };
		disArgs[0].secondaryObjects = new TCComponent[] { inputComp };

		// Arguments
		KeyValueArguments dispatcherArgument1 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument2 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument3 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument4 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument5 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument6 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument7 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument8 = new KeyValueArguments();
		KeyValueArguments dispatcherArgument9 = new KeyValueArguments();
		
		dispatcherArgument1.key = "ReportType";
		dispatcherArgument1.value = sSelectedBOMReportType;
		dispatcherArgument2.key = "RevisionRule";
		dispatcherArgument2.value = sSelectedRevisionRule;
		dispatcherArgument3.key = "ReportFormat";
		dispatcherArgument3.value = sSelectedFormat;
		dispatcherArgument4.key = "UserName";
		dispatcherArgument4.value = sUserName;
		dispatcherArgument5.key = "MaxBOMLevel";
		dispatcherArgument5.value = sSelectedLevel;
		dispatcherArgument6.key = "EqualPart";
		dispatcherArgument6.value = lIsBT7ComponentRev ? inputComp.getTCProperty("bt7_equalpart").getStringValue() : "";
		dispatcherArgument7.key = "IncludePCB";
		dispatcherArgument7.value = sIncludePCB;
		dispatcherArgument8.key = "IncludeSUP";
		dispatcherArgument8.value = sIncludeSUP;
		dispatcherArgument9.key = "Filename";
		dispatcherArgument9.value = sFilename;

		KeyValueArguments[] dispatcherArguments = new KeyValueArguments[] { dispatcherArgument1, dispatcherArgument2, dispatcherArgument3,
				dispatcherArgument4, dispatcherArgument5, dispatcherArgument6, dispatcherArgument7, dispatcherArgument8, dispatcherArgument9 };

		disArgs[0].keyValueArgs = dispatcherArguments;

		// Trigger request
		dispMgmtService.createDispatcherRequest(disArgs);

		return true;
	}

	void errorMessage(String sMessage, Object... objList) {
		MessageDialog.openError(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}

	void infoMessage(String sMessage, Object... objList) {
		MessageDialog.openInformation(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}

	void warningMessage(String sMessage, Object... objList) {
		MessageDialog.openWarning(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}

	boolean questionMessage(String sMessage, Object... objList) {
		return MessageDialog.openQuestion(window.getShell(), MSG_TITLE, String.format(sMessage, objList));
	}

	String nextChar(String s) {
		int length = s.length();
		char c = ' ';
		if (length > 0) {
			c = s.charAt(length - 1);
		}

		if (c == 'z')
			return (length > 1 ? nextChar(s.substring(0, length - 1)) + 'a' : "aa").toUpperCase();

		return (length > 0 ? s.substring(0, length - 1) + ++c : "a").toUpperCase();
	}
}
