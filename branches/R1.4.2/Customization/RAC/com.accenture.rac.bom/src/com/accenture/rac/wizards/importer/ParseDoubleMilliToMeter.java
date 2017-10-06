package com.accenture.rac.wizards.importer;

import org.supercsv.cellprocessor.CellProcessorAdaptor;
import org.supercsv.cellprocessor.ParseDouble;
import org.supercsv.util.CsvContext;

import com.accenture.rac.wizards.importer.spec.ICellProcessor;

public class ParseDoubleMilliToMeter extends CellProcessorAdaptor implements ICellProcessor{

	@Override
	public Object execute(Object value, CsvContext context) {
		ParseDouble processor = new ParseDouble();
		Double milli = 1000.00D;
		return (Double) processor.execute(value, context) / milli;
	}
}
