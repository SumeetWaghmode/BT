package com.accenture.rac.wizards.importer;

import com.accenture.rac.wizards.importer.spec.ICellProcessor;


public final class CellProcessors {

	public static final ICellProcessor Int = new ParseInt();
	public static final ICellProcessor BigDecimalMilliToMeter = new ParseBigDecimalMilliToMeter();
	public static final ICellProcessor DoubleMilliToMeter = new ParseDoubleMilliToMeter();
	public static final ICellProcessor Optional = new ParseOptional();
		
}
