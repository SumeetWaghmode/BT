package com.accenture.sbt.rac.tools;

import java.sql.Timestamp;
import java.util.Date;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

import com.teamcenter.rac.aif.AIFDesktop;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCComponentProcessType;
import com.teamcenter.rac.kernel.TCComponentTaskTemplate;
import com.teamcenter.rac.kernel.TCComponentTaskTemplateType;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.kernel.TCSession;

public class StartCCR extends AbstractHandler {

	private static String MSG_TITLE = "Start CCR";
	private static String WF_NAME = "CC Request";

	private IWorkbenchWindow window = null;
	private TCSession session = null;

	public StartCCR() {
	}

	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {
		try {
			// Get window and session
			window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
			session = (TCSession) AIFDesktop.getActiveDesktop()
					.getCurrentApplication().getSession();
			String userName = session.getUserName();

			// Prevent Infodba from starting Workflows
			if (userName.equalsIgnoreCase("infodba")) {
				warningMessage("Infodba is an installation user account and should not be used to create Workspace Objects.");
				return null;
			}

			// Get TaskTemplate
			final TCComponentTaskTemplateType taskTemplateType = (TCComponentTaskTemplateType) session
					.getTypeComponent("EPMTaskTemplate");

			TCComponentTaskTemplate ccrTemplate = taskTemplateType.find(
					WF_NAME, TCComponentTaskTemplate.PROCESS_TEMPLATE_TYPE);

			if (ccrTemplate == null) {
				errorMessage("Workflow template not found: '" + WF_NAME + "'");
				return null;
			}

			// Confirmation message
			if (!questionMessage("Do you want to start a Common Component Request (CCR) ?")) {
				return null;
			}

			// Start Workflow
			final TCComponentProcessType processType = (TCComponentProcessType) session
					.getTypeComponent("Job");

			processType.create("CCR_" + userName + "_"
					+ new Timestamp(new Date().getTime()), "", ccrTemplate,
					new TCComponent[] {}, new int[] {});

			// Success message
			infoMessage("Common Component Request (CCR) successfully started. Please check your Worklist and enter the details of the requst.");
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
