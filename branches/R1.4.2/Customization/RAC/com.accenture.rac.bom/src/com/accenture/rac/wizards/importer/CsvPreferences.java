package com.accenture.rac.wizards.importer;

import java.util.LinkedHashMap;

import org.supercsv.comment.CommentStartsWith;
import org.supercsv.prefs.CsvPreference;


public final class CsvPreferences extends LinkedHashMap<String, CsvPreference>{

	private static final long serialVersionUID = 2187039569683032807L;
	
	private String activePreference;
	private String defaultPreference = "\" \\t \\r\\n";
	
	public static CsvPreferences INSTANCE;

	public CsvPreferences() {
		this.put("\" \\t \\n", CsvPreference.TAB_PREFERENCE);
		this.put("\" , \\n", CsvPreference.EXCEL_PREFERENCE);
		this.put("\" ; \\n", CsvPreference.EXCEL_NORTH_EUROPE_PREFERENCE);
		this.put("\" , \\r\\n", CsvPreference.STANDARD_PREFERENCE);
		this.put("\' ; \\r\\n #", new CsvPreference.Builder('\'', ';', "\r\n").skipComments(new CommentStartsWith("#")).build());
		this.put("\" ; \\r\\n #", new CsvPreference.Builder('"', ';', "\r\n").skipComments(new CommentStartsWith("#")).build());
		activePreference = defaultPreference;
	}
	
	public static CsvPreferences getInstance(){
		if(INSTANCE == null)
			INSTANCE = new CsvPreferences();
		return INSTANCE;
	}
	
	public void setDefaultPreference(){
		getInstance().activePreference = defaultPreference;
	}
	public String getDefaultPreference(){
		return getInstance().defaultPreference;
	}
	
	public CsvPreference getActive(){
		return getInstance().get(activePreference);
	}
	
	public String getActivePreferenceName(){
		return activePreference;
	}
	
	public void setActivePreferenceName(String preferenceName){
		for (String key : this.keySet()) {
			if(key.toLowerCase().startsWith(preferenceName.toLowerCase())){
				this.activePreference = key;
				break;
			}
			
		}
	}
	
}
