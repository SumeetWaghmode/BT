package com.accenture.rac.wizards.importer;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public enum RowStatus {
	NO_SATUS(0, "None"),
	REMOVE  (1, "Remove"), 
	UPDATE  (2, "Update"), 
	INSERT  (3, "Insert"),
	IGNORE  (4, "Ignore"),
	ERROR   (5, "Error");
	
	private int code;
	private String label;
	
	private static Map<Integer, RowStatus> codeStatus;
	
	private RowStatus(int code, String label){
		this.code = code;
        this.label = label;
    }

	public static RowStatus getStatus(int i) {
        if (codeStatus == null) {
            initMapping();
        }
        return codeStatus.get(i);
    }
	
	public static RowStatus getNextStatus(RowStatus s) {
		int nextCode = s.code + 1;
		return (nextCode == values().length) ? getStatus(0) : getStatus(nextCode);
		
    }
 
    private static void initMapping() {
    	codeStatus = new HashMap<Integer, RowStatus>();
        for (RowStatus s : values()) {
        	codeStatus.put(s.code, s);
        }
    }
    
    /**
     * Returns all values that will lead to an action.
     * Ecludes NO_STATUS, ERROR and IGNORE
     * @return RowStatus[]
     */
    public static RowStatus[] activeValues() {
    	RowStatus[] value = values();
    	ArrayList<RowStatus> lessValues = new ArrayList<RowStatus>();
		for (int i = 0; i < value.length; i++) {
			if(value[i] != NO_SATUS && value[i] != ERROR && value[i] != IGNORE)
				lessValues.add(value[i]);
		}
		return (RowStatus[]) lessValues.toArray(new RowStatus[0]);
	}
    
    /**
     * Returns all values but NO_STATUS
     * @return RowStatus[]
     */
    public static RowStatus[] realValues() {
    	RowStatus[] value = values();
    	ArrayList<RowStatus> lessValues = new ArrayList<RowStatus>();
		for (int i = 0; i < value.length; i++) {
			if(value[i] != NO_SATUS)
				lessValues.add(value[i]);
		}
		return (RowStatus[]) lessValues.toArray(new RowStatus[0]);
	}
	
	public String getLabel() {
		return label;
	}

	public int getCode() {
		return code;
	}

}
