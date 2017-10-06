/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.accenture.dispatcher.report_bom.ets.util;

import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.Type;
import com.teamcenter.soa.exceptions.NotLoadedException;

/**
 * 
 * @author ftorres
 */
public class Dataset extends com.teamcenter.soa.client.model.strong.Dataset {

	public Dataset(Type paramType, String paramString) {
		super(paramType, paramString);
	}

	public ModelObject[] get_IMAN_capture() throws NotLoadedException {
		return getPropertyObject("IMAN_capture").getModelObjectArrayValue();
	}

	@Override
	public ModelObject[] get_IMAN_Rendering() throws NotLoadedException {
		return getPropertyObject("IMAN_Rendering").getModelObjectArrayValue();
	}
}
