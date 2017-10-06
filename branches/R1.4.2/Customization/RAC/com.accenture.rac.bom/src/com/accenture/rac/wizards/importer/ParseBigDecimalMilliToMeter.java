package com.accenture.rac.wizards.importer;

import java.math.BigDecimal;

import org.supercsv.cellprocessor.CellProcessorAdaptor;
import org.supercsv.cellprocessor.ParseBigDecimal;
import org.supercsv.util.CsvContext;

import com.accenture.rac.wizards.importer.spec.ICellProcessor;

public class ParseBigDecimalMilliToMeter extends CellProcessorAdaptor implements ICellProcessor {

	@Override
	public Object execute(Object value, CsvContext context) {
		ParseBigDecimal processor = new ParseBigDecimal();
		BigDecimal parsedValue = (BigDecimal) processor.execute(value, context);
		BigDecimal milli = new BigDecimal(1000.0d);
		return  parsedValue.divide(milli);
	}
}
