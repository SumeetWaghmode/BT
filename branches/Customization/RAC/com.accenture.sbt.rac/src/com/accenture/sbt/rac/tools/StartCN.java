package com.accenture.sbt.rac.tools;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCAttachmentType;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentItemRevision;
import com.teamcenter.rac.kernel.TCComponentProcessType;
import com.teamcenter.rac.kernel.TCComponentTaskTemplate;
import com.teamcenter.rac.kernel.TCComponentTaskTemplateType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCProperty;
import com.teamcenter.rac.kernel.TCSession;

public class StartCN extends AbstractHandler {

	private static String MSG_TITLE = "Change Order";
	private static String WF_NAME = "BT Change Order";

	private IWorkbenchWindow window = null;
	private TCSession session = null;

	public StartCN() {
	}

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {
			// Get window and session
			window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			session = (TCSession) AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getSession();
			String userName = session.getUserName();
			
			// Get current group
			String currentRole = session.getCurrentRole().getStringProperty("role_name");

			// Prevent Infodba from starting Workflows
			if (userName.equalsIgnoreCase("infodba")) {
				warningMessage("Infodba is an installation user account and should not be used to create Workspace Objects.");
				return null;
			}else if (currentRole.equalsIgnoreCase("Change Control Manager") == false ){
				warningMessage("Only Change Control Managers are allowed to start an ECO Workflow.");
				return null;
			}

			// Only one component selected?
			int iSelectedComponents = AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getTargetComponents().length;
			if (iSelectedComponents != 1) {
				warningMessage("Please select exactly one ItemRevision");

				return null;
			}

			InterfaceAIFComponent selectedComp = AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getTargetComponent();

			((TCComponent) selectedComp).refresh();

			// Selected component is ItemRevision ?
			if (selectedComp instanceof TCComponentItemRevision) {
				// Does the Change Notice Revision already have a status ?
				final TCProperty datasetReleaseStatusList = ((TCComponentItemRevision) selectedComp).getTCProperty("release_status_list");
				int iStatusCount = datasetReleaseStatusList.getReferenceValueArray().length;

				if (iStatusCount > 0) {
					warningMessage("Please select exactly one ItemRevision which doesn't have a status assigned.");

					return null;
				}
				
				// Get TaskTemplate
				final TCComponentTaskTemplateType taskTemplateType = (TCComponentTaskTemplateType) session
						.getTypeComponent("EPMTaskTemplate");

				TCComponentTaskTemplate ccrTemplate = taskTemplateType.find(
						WF_NAME, TCComponentTaskTemplate.PROCESS_TEMPLATE_TYPE);

				if (ccrTemplate == null) {
					errorMessage("Workflow template not found: '" + WF_NAME
							+ "'");
					return null;
				}

				// Confirmation message
				if (!questionMessage("Do you want to start a Change Order ?")) {
					return null;
				}

				// Start Workflow
				final TCComponentProcessType processType = (TCComponentProcessType) session
						.getTypeComponent("Job");

				processType
						.create(((TCComponentItemRevision) selectedComp)
								.getItem().getStringProperty("item_id")
								+ " - "
								+ ((TCComponentItemRevision) selectedComp)
										.getStringProperty("object_name"),
								"",
								ccrTemplate,
								new TCComponent[] { (TCComponentItemRevision) selectedComp },
								new int[] { TCAttachmentType.TARGET });

				// Success message
				infoMessage("Change Order successfully started - please check your Worklist.");
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
