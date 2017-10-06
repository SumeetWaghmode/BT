package com.accenture.sbt.rac.tools;

import java.util.Vector;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.IMessageProvider;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCAttachmentScope;
import com.teamcenter.rac.kernel.TCAttachmentType;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentDataset;
import com.teamcenter.rac.kernel.TCComponentDatasetType;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentProcessType;
import com.teamcenter.rac.kernel.TCComponentTask;
import com.teamcenter.rac.kernel.TCComponentTaskTemplate;
import com.teamcenter.rac.kernel.TCComponentTaskTemplateType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;

public class AddTaskToCN extends AbstractHandler {

	private class GetTaskNameDialog extends TitleAreaDialog {

		private Text txtTaskName;

		private String sTaskName;

		public GetTaskNameDialog(Shell parentShell) {
			super(parentShell);
		}

		@Override
		public void create() {
			super.create();
			setTitle("New task for ECO");
			setMessage("Please enter the name of the new task",
					IMessageProvider.INFORMATION);
		}

		@Override
		protected Control createDialogArea(Composite parent) {
			Composite area = (Composite) super.createDialogArea(parent);
			Composite container = new Composite(area, SWT.NONE);
			container.setLayoutData(new GridData(GridData.FILL_BOTH));
			GridLayout layout = new GridLayout(2, false);
			container
					.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
			container.setLayout(layout);

			createTaskName(container);

			return area;
		}

		private void createTaskName(Composite container) {
			Label lbtTaskName = new Label(container, SWT.NONE);
			lbtTaskName.setText("Task Name");

			GridData dataTaskName = new GridData();
			dataTaskName.grabExcessHorizontalSpace = true;
			dataTaskName.horizontalAlignment = GridData.FILL;

			txtTaskName = new Text(container, SWT.BORDER);
			txtTaskName.setLayoutData(dataTaskName);
		}

		@Override
		protected boolean isResizable() {
			return true;
		}

		// save content of the Text fields because they get disposed
		// as soon as the Dialog closes
		private void saveInput() {
			sTaskName = txtTaskName.getText();

		}

		@Override
		protected void okPressed() {
			saveInput();
			// operation.startTask(getTaskName());

			super.okPressed();
		}

		public String getTaskName() {
			return sTaskName;
		}
	}

	private static String MSG_TITLE = "Change Order Task";
	private static String WF_NAME = "BT Change Order";
	private static String WF_SUB_NAME = "BT Change Order - Sub Process";
	private static String WF_TASK_NAME = "Wait for Work Packages to be finalized";

	private IWorkbenchWindow window = null;
	private TCSession session = null;

	private TCComponentItemRevision tEcoRev = null;

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {
			// Get window and session
			window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			session = (TCSession) AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getSession();

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) {
				warningMessage("Please select exactly one EPMTask");

				return null;
			}

			InterfaceAIFComponent selectedComp = AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getTargetComponent();

			((TCComponent) selectedComp).refresh();

			// Selected component is TCComponentTask ?
			if (selectedComp instanceof TCComponentTask) {
				TCComponentTask task = (TCComponentTask) selectedComp;

				String sTaskName = task.getName();
				String sRootName = task.getRoot().getName();

				if (sRootName.equalsIgnoreCase(WF_NAME)
						&& sTaskName.equalsIgnoreCase(WF_TASK_NAME)) {
					// Get ECO-Object
					@SuppressWarnings("rawtypes")
					Vector allAttachments = task
							.getAllAttachments(TCAttachmentScope.GLOBAL);

					for (Object attachment : allAttachments) {
						if (attachment instanceof TCComponentItemRevision) {
							TCComponentItemRevision tmpRev = (TCComponentItemRevision) attachment;
							if (tmpRev.getType().equalsIgnoreCase(
									"BT7_ChangeNoticeRevision")) {
								tEcoRev = tmpRev;
								break;
							}
						}
					}

					if (tEcoRev != null) {
						GetTaskNameDialog getTaskNameDialog = new GetTaskNameDialog(
								window.getShell());
						getTaskNameDialog.create();
						if (getTaskNameDialog.open() == Window.OK) {
							String taskNameShort = getTaskNameDialog
									.getTaskName();

							// Define Task-Name
							String taskName = "Task for '"
									+ tEcoRev.getItem().getProperty("item_id")
									+ "' : " + taskNameShort;
							if (taskName.length() > 128) {
								taskName = taskName.substring(0, 125) + "...";
							}

							// Define Dataset-Name
							String datasetName = tEcoRev.getItem().getProperty(
									"item_id")
									+ " - " + taskNameShort;
							if (datasetName.length() > 40) {
								datasetName = taskName.substring(0, 37) + "...";
							}

							// Create Text-Dataset
							TCComponentDatasetType dsType = (TCComponentDatasetType) session
									.getTypeComponent("Dataset");
							TCComponentDataset tDataset = dsType.create(
									datasetName, datasetName, "Dataset ID",
									"Dataset Rev", "TEXT", null);

							// Get Worklfow Template
							final TCComponentTaskTemplateType taskTemplateType = (TCComponentTaskTemplateType) session
									.getTypeComponent(TCComponentTaskTemplate.TYPE_NAME);
							final TCComponentTaskTemplate taskTemplate = taskTemplateType
									.find(WF_SUB_NAME,
											TCComponentTaskTemplate.PROCESS_TEMPLATE_TYPE);

							if (taskTemplate != null) {
								final TCComponentProcessType processType = (TCComponentProcessType) session
										.getTypeComponent("Job");
								final TCComponent[] attachments = new TCComponent[2];
								attachments[0] = tEcoRev;
								attachments[1] = tDataset;
								final int[] attachmentsType = new int[2];
								attachmentsType[0] = TCAttachmentType.TARGET;
								attachmentsType[1] = TCAttachmentType.REFERENCE;

								// Start Workflow
								processType.create(taskName, taskNameShort,
										taskTemplate, attachments,
										attachmentsType);
							} else {
								errorMessage("Workflow-Template '"
										+ WF_SUB_NAME + "' not found!");
							}

						}

					} else {
						errorMessage("No ECO-Revision in the targets of the Workflow!");
					}
				} else {
					warningMessage("Invalid Workflow-Task. Please select:\n > Workflow: '"
							+ WF_NAME + "'\n > Task: '" + WF_TASK_NAME + "'");
				}
			}
		} catch (TCException e) {
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

}
