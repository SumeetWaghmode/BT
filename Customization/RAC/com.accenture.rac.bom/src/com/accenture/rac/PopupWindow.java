package com.accenture.rac;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;

public class PopupWindow {
	
	public static MessageDialog customDialog(Shell shell, Integer messageType, String title, String message, String[] buttons) {
		messageType = (messageType == null)? MessageDialog.NONE : messageType;
		return new MessageDialog(shell, title, null, message, messageType, buttons, 0);
	}
	
	public static void errorMessage(Shell shell, String title, String sMessage, Object... objList) {
		MessageDialog.openError(shell, title, String.format((sMessage == null) ? "": sMessage, objList));
	}

	public static void infoMessage(Shell shell, String title,String sMessage, Object... objList) {
		MessageDialog.openInformation(shell, title, String.format(sMessage, objList));
	}

	public static void warningMessage(Shell shell, String title,String sMessage, Object... objList) {
		MessageDialog.openWarning(shell, title, String.format(sMessage, objList));
	}

	public static boolean questionMessage(Shell shell, String title, String sMessage, Object... objList) {
		return MessageDialog.openQuestion(shell, title, String.format(sMessage, objList));
	}
}
