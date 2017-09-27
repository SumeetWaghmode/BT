package com.accenture.rac.wizards.importer;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.accenture.rac.wizards.AbstractWizardPage;
import com.accenture.rac.wizards.importer.spec.IFileImportProvider;

public class SelectFilePage extends AbstractWizardPage {
	
	private String[] dialogExt;
	private String dialogPath;
	private IFileImportProvider importSerivice;
	
	private FileDialog fileDialog;
	private Text fileText;
	
	/**
	 * Creates a page to browse and select a file.
	 */
	public SelectFilePage(IFileImportProvider importSerivice) {
		this.dialogPath = "C:\\";
		this.dialogExt = new String[]{"*.*"};
		this.importSerivice = importSerivice;
	}
	
	/**
	 * Allows the page to continue without a user input. <br />
	 * By default, set to false
	 * @param debug boolean
	 */
	public void setDebug(boolean debug){
		pageReady = debug;
	}
	
	/**
	 * Get the allowed file extensions for the browse popup.
	 * @return dialogExt String[]
	 */
	public String[] getDialogExt() {
		return dialogExt;
	}

	/**
	 * Set the allowed file extensions for the browse popup. <br /> 
	 * By default *.*
	 * @param dialogExt the dialogExt to set
	 */
	public void setDialogExt(String[] dialogExt) {
		this.dialogExt = dialogExt;
	}

	/**
	 * Get the dialog path of the browse popup
	 * @return the dialogPath
	 */
	public String getDialogPath() {
		return dialogPath;
	}

	/**
	 * Set the dialogPath for the browse popup. By default this is C:/
	 * @param dialogPath String
	 */
	public void setDialogPath(String dialogPath) {
		this.dialogPath = dialogPath;
	}

	@Override
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NONE);
		
		GridLayout layout = new GridLayout(4, false);
		container.setLayout(layout);
		
		Label label = new Label(container, SWT.NONE);
		label.setText("File");
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan=2;
		fileText = new Text(container, SWT.BORDER | SWT.SINGLE);
		fileText.setLayoutData(gd);

		// Listen for manual input
		// assign Text changes to filePath
		fileText.addModifyListener(new ModifyListener() {
			@Override
			public void modifyText(ModifyEvent e) {
				String source = fileText.getText();
				importSerivice.setSource(source);
				source.substring(source.lastIndexOf("\\") +1);
				enableNextButton(true);
			}
		});

		fileDialog = new FileDialog(container.getShell(), SWT.BORDER | SWT.SINGLE);
		fileDialog.setText("Open");
		fileDialog.setFilterPath(dialogPath);
        fileDialog.setFilterExtensions(dialogExt);
		Button openBtn = new Button(container, SWT.PUSH);
        openBtn.setText("Browse..");
        openBtn.addSelectionListener(new SelectionListener() {
			
        	@Override
			public void widgetDefaultSelected(SelectionEvent arg0) { }
        	
        	@Override
			public void widgetSelected(SelectionEvent arg0) {
        		String filePath = fileDialog.open();
        		
				if (filePath != null && !filePath.isEmpty()){
					fileText.setText(filePath);
        			importSerivice.setSource(fileText.getText());
					enableNextButton(true);
				}
			}
		});
        
        
     // create the combo filters
        
        Label filePreferencelabel = new Label(container, SWT.NONE);
        filePreferencelabel.setText("Parsing settings");
		
        Combo filePreference = new Combo(container, SWT.READ_ONLY | SWT.BREAK);
		filePreference.setToolTipText("quote, delimiter, line break, comment");
		Map<String, ?> preferences = CsvPreferences.getInstance();
		
		String active = CsvPreferences.getInstance().getActivePreferenceName();
		List<String> keyList = new ArrayList<String>(preferences.keySet());
		int j = 0;
		for ( int i = keyList.size()-1 ; i >= 0 ; i-- ){
			String preference = keyList.get(i);
			filePreference.add(preference);
			if(keyList.get(i).equalsIgnoreCase(active)){
				filePreference.select(j);
			}
			j++;
		}
		
		filePreference.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				Combo combo = (Combo) e.widget;
				importSerivice.setCsvPreference(combo.getText());
			}
		});
		
		
		gd = new GridData(GridData.FILL_HORIZONTAL);
		gd.horizontalSpan=2;
		Label settingLabel = new Label(container, SWT.NONE);
		settingLabel.setText("quote, delimiter, end of line, comment");
        settingLabel.setLayoutData(gd);
        setControl(container);
	}
	
	@Override
	public void onEnter() {
		getAWizard().canFinish(false);
	}

	@Override
	public void onLeave() {
		File file = new File((String) importSerivice.getSource());
		if(file.exists() && file.isFile() && file.canRead()){
			// reset errorMessage on success
			setErrorMessage(null);
			canLeave(true);
		}else {
			setErrorMessage("The file does not exist or cannot be read");
			enableNextButton(false);
			canLeave(false);
		}
	}
}
