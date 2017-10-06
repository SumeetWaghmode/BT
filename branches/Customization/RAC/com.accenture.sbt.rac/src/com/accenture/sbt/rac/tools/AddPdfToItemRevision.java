package com.accenture.sbt.rac.tools;

import java.io.File;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.IMessageProvider;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCAttachmentType;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentDatasetType;
import com.teamcenter.rac.kernel.TCComponentGroup;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentProcessType;
import com.teamcenter.rac.kernel.TCComponentTaskTemplate;
import com.teamcenter.rac.kernel.TCComponentTaskTemplateType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;

public class AddPdfToItemRevision extends AbstractHandler {

	private class GetPdfDialogue extends TitleAreaDialog {

		private Text txtFilePath;
		private Button btnChoosePDF;

		private String sFilePath;

		private Shell parentShell;

		public GetPdfDialogue(Shell parentShell) {
			super(parentShell);
			this.parentShell = parentShell;
		}

		@Override
		public void create() {
			super.create();
			setTitle("Add PDF to Item Revision and sync Status");
			setMessage("Please select a PDF from your local OS", IMessageProvider.INFORMATION);
		}

		@Override
		protected Control createDialogArea(Composite parent) {
			Composite area = (Composite) super.createDialogArea(parent);
			Composite container = new Composite(area, SWT.NONE);
			container.setLayoutData(new GridData(GridData.FILL_BOTH));
			GridLayout layout = new GridLayout(3, false);
			container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			container.setLayout(layout);

			defineDialogue(container);

			return area;
		}

		private void defineDialogue(Composite container) {
			Label lbtTaskName = new Label(container, SWT.NONE);
			lbtTaskName.setText("PDF File:");

			GridData dataTaskName = new GridData();
			dataTaskName.grabExcessHorizontalSpace = true;
			dataTaskName.horizontalAlignment = GridData.FILL;

			txtFilePath = new Text(container, SWT.BORDER);
			txtFilePath.setLayoutData(dataTaskName);
			txtFilePath.setEnabled(false);

			btnChoosePDF = new Button(container, SWT.NONE);
			btnChoosePDF.setText("...");

			btnChoosePDF.addSelectionListener(new SelectionListener() {

				@Override
				public void widgetSelected(SelectionEvent arg0) {
					FileDialog dialog = new FileDialog(parentShell, SWT.OPEN);
					dialog.setFilterNames(new String[] { "PDF Files" });
					dialog.setFilterExtensions(new String[] { "*.pdf" });
					dialog.setFilterPath("c:\\");

					// Get selected file
					String selectedFile = dialog.open();

					//
					if (selectedFile != null && selectedFile.equalsIgnoreCase("") == false) {
						txtFilePath.setText(selectedFile);
					}
				}

				@Override
				public void widgetDefaultSelected(SelectionEvent arg0) {
				}
			});
		}

		@Override
		protected boolean isResizable() {
			return true;
		}

		// save content of the Text fields because they get disposed
		// as soon as the Dialog closes
		private void saveInput() {
			sFilePath = txtFilePath.getText();
		}

		@Override
		protected void okPressed() {
			saveInput();
			// operation.startTask(getTaskName());

			super.okPressed();
		}

		public String getFilePath() {
			return sFilePath;
		}
	}

	private static String MSG_TITLE = "Add PDF to Item Revision and sync Status";
	private static String WF_NAME = "_BT7_Admin_AddFileToItemRevisionAndSetStatus";

	private IWorkbenchWindow window = null;
	private TCSession session = null;

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {
			// Get window and session
			window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			session = (TCSession) AIFDesktop.getActiveDesktop().getCurrentApplication().getSession();

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) {
				warningMessage("Please select exactly one Item Revision");

				return null;
			}

			InterfaceAIFComponent selectedComp = AIFDesktop.getActiveDesktop().getCurrentApplication().getTargetComponent();

			((TCComponent) selectedComp).refresh();

			// Selected component is TCComponentTask ?
			if (selectedComp instanceof TCComponentItemRevision) {
				TCComponentItemRevision itemRevision = (TCComponentItemRevision) selectedComp;
				String sTargetItemId = itemRevision.getItem().getStringProperty("item_id");

				// Current User is the owner of the IR?
				String currentGroup = session.getCurrentGroup().getGroupName();
				String objectOwningGroupName = ((TCComponentGroup)itemRevision.getTCProperty("owning_group").getReferenceValue()).getGroupName();

				// Current User is not infodba?
				String currentUserName = session.getUserName();
			
				if(currentUserName.equalsIgnoreCase("infodba") == false)
				{				
					if(currentGroup.equalsIgnoreCase(objectOwningGroupName) == true) {
						// Start file chooser dialogue
						GetPdfDialogue getPDFDialogue = new GetPdfDialogue(window.getShell());
						getPDFDialogue.create();
						if (getPDFDialogue.open() == Window.OK) {
							String pdfFilePath = getPDFDialogue.getFilePath();
							File pdfFile = new File(pdfFilePath);
							String sPdfFileName = pdfFile.getName();
							
							// Cleanup filename
							{
								// Cut file-extension
								int iExtensionIndex = sPdfFileName.lastIndexOf(".pdf");
								sPdfFileName = sPdfFileName.substring(0, iExtensionIndex == -1 ? sPdfFileName.length() : iExtensionIndex);
								
								// Cut _sheet
								int iSheetIndex = sPdfFileName.toLowerCase().lastIndexOf("_sheet");
								sPdfFileName = sPdfFileName.substring(0, iSheetIndex == -1 ? sPdfFileName.length() : iSheetIndex);
								
								// Cut prefix
								int iPrefixIndex = sPdfFileName.indexOf(sTargetItemId);
								sPdfFileName = sPdfFileName.substring(iPrefixIndex == -1 ? 0 : iPrefixIndex, sPdfFileName.length());								
							}
							
							if (pdfFilePath != null && pdfFilePath.equalsIgnoreCase("") == false) {
								// Create Dataset
								TCComponentDatasetType dsType = (TCComponentDatasetType) session.getTypeComponent("Dataset");
								TCComponentDataset pdfDataset = dsType.create(sPdfFileName, sPdfFileName, "", "", "PDF", null);
								
								// Upload Files
								pdfDataset.setFiles(new String[] { pdfFilePath }, new String[] { "PDF_Reference" });
		
								// Get TaskTemplate
								final TCComponentTaskTemplateType taskTemplateType = (TCComponentTaskTemplateType) session
										.getTypeComponent("EPMTaskTemplate");
		
								TCComponentTaskTemplate addFileWorkflowTemplate = taskTemplateType.find(WF_NAME,
										TCComponentTaskTemplate.PROCESS_TEMPLATE_TYPE);
		
								if (addFileWorkflowTemplate == null) {
									errorMessage("Workflow template not found: '" + WF_NAME + "'");
									return null;
								}
		
								// Confirmation message
								if (!questionMessage("Do you want to add the PDF file to the Item Revision and sync the release status ?")) {
									return null;
								}
		
								// Start Workflow
								final TCComponentProcessType processType = (TCComponentProcessType) session.getTypeComponent("Job");
		
								processType.create(WF_NAME, WF_NAME, addFileWorkflowTemplate, new TCComponent[] { itemRevision, pdfDataset }, new int[] {
										TCAttachmentType.TARGET, TCAttachmentType.TARGET });
		
								// Success message
								infoMessage("File successfully added - please refresh the Item Revision");
							}
						}
					} else {
						warningMessage("Files can only be added by the owning group of the selected Item Revision" + "\n" + "( owning group : " + objectOwningGroupName + " )");
					}
				} else {
					warningMessage("Infodba is an installation user account and should not be used to create Workspace Objects.");
				}
			}
		} catch (TCException e) {
			e.printStackTrace();
			errorMessage("EXCEPTION: " + e.getMessage());
		}
		return null;
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

}
